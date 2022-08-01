#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <map>

using VertexDescriptor = std::pair<size_t, size_t>;
class EdgeData{
public:
	explicit EdgeData(VertexDescriptor fst, VertexDescriptor snd) : fstVrtx(fst), sndVrtx(snd) {}

	VertexDescriptor fst() const { return fstVrtx; }
	VertexDescriptor snd() const { return sndVrtx; }

private:
	friend std::hash<EdgeData>;
	friend bool operator == (const EdgeData&, const EdgeData&);
	friend bool operator < (const EdgeData&, const EdgeData&);

	VertexDescriptor fstVrtx;
	VertexDescriptor sndVrtx;

};
bool operator == (const EdgeData& lhs, const EdgeData& rhs);
bool operator < (const EdgeData& lhs, const EdgeData& rhs);

template<>
struct std::hash<VertexDescriptor> {
	size_t operator() (const VertexDescriptor& vertex) const noexcept {
		return std::hash<size_t>{}(vertex.first) * 113 +
	           std::hash<size_t>{}(vertex.second) * 37;
	}
};

template<>
struct std::hash<EdgeData> {
	size_t operator() (const EdgeData& data) const noexcept {
		return std::hash<VertexDescriptor>{}(data.fstVrtx) * 17 +
			   std::hash<VertexDescriptor>{}(data.sndVrtx) * 23;
	}
};

class Graph {
public:
	Graph() = default;
	Graph(const std::vector<std::vector<bool>>& grid);

	void Dijkstra_Dynamic(VertexDescriptor from, VertexDescriptor to, std::function<void(VertexDescriptor, bool)> notifier);
private:
	std::unordered_map<EdgeData, double> graph;
	std::unordered_map<VertexDescriptor, std::unordered_set<VertexDescriptor>> vertexConnections;
};


#endif 
