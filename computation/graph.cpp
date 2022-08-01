#include "graph.hpp"

#include <algorithm> 
#include <set> 
#include <unordered_map>
#include <chrono>
#include <thread>

Graph::Graph(const std::vector<std::vector<bool>>& grid) {
	for(size_t i = 0; i < grid.size(); i++) {
		for(size_t j = 0; j < grid[i].size(); j++) {
			auto addIfExist = [this, &grid, current_i = i, current_j = j](size_t i, size_t j, double weight) {
				if (i >= 0 && j >= 0 && i < grid.size() && j < grid.size()) {
					if (grid[i][j]) {
						return;
					}
					graph[EdgeData({current_i, current_j}, {i,j})] = weight;
					vertexConnections[VertexDescriptor(current_i, current_j)].insert({i,j});
				}
			};
			/*
			(i-1,j-1)     (i,j-1)   (i+1,j-1)
					   \	 |	   /
						\    |    /
			(i-1,j) ---- (current) ---- (i+1,j)
						/	 |    \
					   /     |     \
			(i-1,j+1)    (i,j+1)      (i+1,j+1)	
			*/
			addIfExist(i-1, j-1, sqrt(2));
			addIfExist(i, j, 1);
			addIfExist(i+1, j-1, sqrt(2));
			addIfExist(i-1,j, 1);
			addIfExist(i+1, j, 1);
			addIfExist(i-1, j+1, sqrt(2));
			addIfExist(i, j+1,1);
			addIfExist(i+1, j+1, sqrt(2));
		}
	}

}

bool operator == (const EdgeData& lhs, const EdgeData& rhs) { 
	return (lhs.fstVrtx == rhs.fstVrtx && lhs.sndVrtx == rhs.sndVrtx) || 
		   (lhs.fstVrtx == rhs.sndVrtx && lhs.sndVrtx == rhs.fstVrtx);
}
bool operator < (const EdgeData& lhs, const EdgeData& rhs) {
	if (lhs == rhs) {
		return false;
	}
	return lhs < rhs;
}

void Graph::Dijkstra_Dynamic(VertexDescriptor from, VertexDescriptor to, std::function<void(VertexDescriptor, bool)> notifier) {
	// Shortest path from "from" to VertexDescriptor (true if finalized) -> std::vector<VertexDescriptor>
	std::unordered_map<VertexDescriptor, std::pair<size_t, bool>> shortedPathWeight;
	std::unordered_map<VertexDescriptor, VertexDescriptor> parents;


	/*
	for (const auto& [key, _]: vertexConnections) {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		notifier(key, true);
	}
	*/


	auto getMinDistance = [this, &shortedPathWeight]() {
		int min = INT_MAX;
		VertexDescriptor minIndex;
		for (const auto& [key, value]: shortedPathWeight) {
			if (!value.second && value.first < min) {
				min = value.first; 
				minIndex = key;
			}
		}
		return minIndex;
	};

	// Initialize all distances to INF
	for (const auto& [key, _]: vertexConnections) {
		shortedPathWeight[key] = {INT_MAX, false};
	}
	shortedPathWeight[from].first = true;

	for([[maybe_unused]] const auto& [vertex, _]: vertexConnections) {

		VertexDescriptor minDistVert = getMinDistance();
		notifier(minDistVert, false);
		std::this_thread::sleep_for(std::chrono::milliseconds(30));

		shortedPathWeight[minDistVert].second = true;
		if (minDistVert == to) {
			//Notify route

			std::function<void(VertexDescriptor vertex)> notifyRoute;
			notifyRoute = [&notifyRoute, from, notifier, &parents](VertexDescriptor vertex) {
				if (parents.find(vertex) == parents.end()) {
					return;
				}
				VertexDescriptor parent = parents[vertex];
				if (parent != from) {
					notifier(parent, true);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(30));
				notifyRoute(parent);
			};

			notifyRoute(to);


			return;
		}

		for (auto& neighVer: vertexConnections[minDistVert]) {
			auto& val = shortedPathWeight[neighVer];
			size_t weight = graph[EdgeData(neighVer, minDistVert)];
			if (!val.second && minDistVert.first != INT_MAX && minDistVert.first + weight < val.first) {
				val.first = minDistVert.first + weight;
				parents[neighVer] = minDistVert;
			}
		}
	}
}
