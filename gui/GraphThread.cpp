#include "pathGrid.hpp"

#include <utility>
#include "utility.hpp"

#include <iostream>

PathGrid::GraphThread::GraphThread(PathGrid* handler, Graph&& graph) : wxThread(wxTHREAD_DETACHED), handler_(handler), graph_(std::move(graph)) {}

wxThread::ExitCode PathGrid::GraphThread::Entry() {
	graph_.Dijkstra_Dynamic(fromWxPoint(from), fromWxPoint(to), [this](std::pair<size_t, size_t> grid, bool isTraced) {
		handler_->GetGrid(toWxPoint(grid)).isVisited = true;
		if (isTraced) {
			handler_->GraphTrace().push_back(toWxPoint(grid));
		}
		if (!TestDestroy()) {
			wxQueueEvent(handler_, new wxThreadEvent(wxEVT_GRAPH_THREAD_UPDATE));
		}
	});

	wxQueueEvent(handler_, new wxThreadEvent(wxEVT_GRAPH_THREAD_COMPLETED));
	return (wxThread::ExitCode)0;
}
bool PathGrid::GraphThread::StartDijkstra(wxPoint from, wxPoint to) {
	this->from = from;
	this->to = to;
	if(Run() != wxTHREAD_NO_ERROR) {
		return false;
	}
	return true;
}
