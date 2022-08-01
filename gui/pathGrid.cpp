#include "pathGrid.hpp"

#include "wx/dcclient.h"
#include "wx/frame.h"
#include "wx/listbox.h"

#include "WINDOW_IDS.hpp"

#include <iostream>

#include "utility.hpp"

PathGrid::PathGrid(wxWindow* parent, const wxPoint& pos, const wxSize& size, int amount) : 
	wxPanel(parent, WindowID::PATH_PANEL, pos, size), 
	grid(std::vector(amount, std::vector(amount, GridData{}))),
	parent_(parent){
	gridSize.Set(GetSize().GetWidth() / amount, GetSize().GetHeight() / amount);
}

wxBEGIN_EVENT_TABLE(PathGrid, wxPanel)
	EVT_PAINT(PathGrid::ShowGrid)
	EVT_LEFT_DOWN(PathGrid::GridClick)
	EVT_LEFT_UP(PathGrid::OnLeftMouseUp)
	EVT_MOTION(PathGrid::OnMouseMotion)
	EVT_RIGHT_DOWN(PathGrid::OnRightClickDown)
	EVT_CUSTOM( wxEVT_GRAPH_THREAD_UPDATE, wxID_ANY, PathGrid::OnGraphThreadUpdate)
	EVT_CUSTOM( wxEVT_GRAPH_THREAD_COMPLETED, wxID_ANY, PathGrid::OnGraphThreadComplete)
wxEND_EVENT_TABLE()

wxDEFINE_EVENT(wxEVT_GRAPH_THREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_GRAPH_THREAD_COMPLETED, wxThreadEvent);


void PathGrid::ShowGrid(wxPaintEvent& event) {
	wxPaintDC dc(this);
	for (size_t i = 0; i < grid.size(); i++) {
		for(size_t j = 0; j < grid[i].size(); j++) {
			dc.SetPen(wxColour("#34495e"));
			dc.SetBrush(wxNullBrush);
			if (grid[i][j].isVisited) {
				dc.SetBrush(wxColour("#e74c3c"));
				//Will ovverite by "from" "to"
			}
			if (from) {
				if ( i == from->x && j == from->y) {
					grid[i][j].isBlock = false;
					dc.SetBrush(wxColour("#2ecc71"));
				}
			}
			if (to) {
				if ( i == to->x && j == to->y ) {
					grid[i][j].isBlock = false;
					dc.SetBrush(wxColour("#2ecc71"));
				}
			}
			if (grid[i][j].isBlock) {
				dc.SetBrush(*wxBLACK_BRUSH);
			}
			dc.DrawRectangle(wxPoint( i * gridSize.GetWidth(), j * gridSize.GetHeight()), gridSize);
		}
	}
	for(const auto& trace : graphTrace) {
		dc.SetBrush(wxColour("#3498db"));
		dc.DrawRectangle(wxPoint(trace.x * gridSize.GetWidth(), trace.y * gridSize.GetHeight()), gridSize);
	}
}

void PathGrid::GridClick(wxMouseEvent& event) {
	rightClicked = true;
}

void PathGrid::OnMouseMotion(wxMouseEvent& event) {
	if (rightClicked) {
		wxPoint current_point = wxGetMousePosition() - GetScreenPosition();
		int x = current_point.x / gridSize.GetWidth();
		int y = current_point.y / gridSize.GetHeight();
		if ( x >= grid.size() || y >= grid[x].size()) {
			return;
		}
		grid[x][y].isBlock = true;
		Refresh();
	}
}

void PathGrid::OnLeftMouseUp(wxMouseEvent& event) {
	if (rightClicked) {
		RebuildGraph();
	}
	rightClicked = false;	
}

PathGrid::ChoiseListBox::ChoiseListBox(wxWindow* parent, const std::vector<wxString>& choises,
										std::function<void(ChoiseListBox::Choises)> cob) : 
	wxListBox(parent, WindowID::CHOISE_LIST, 
			  wxDefaultPosition, wxDefaultSize, choises.size(), 
			  choises.data()),
	callback(cob){
		SetBackgroundStyle(parent->GetBackgroundStyle());
		SetBackgroundColour(wxColour("#7f8c8d"));
		SetFont(parent->GetFont());
	}

void PathGrid::ChoiseListBox::OnDoubleClick(wxCommandEvent & event) {
	callback(event.GetInt() ? PathGrid::ChoiseListBox::Choises::TO : 
							  PathGrid::ChoiseListBox::Choises::FROM);
}

BEGIN_EVENT_TABLE(PathGrid::ChoiseListBox, wxListBox)
	EVT_LISTBOX_DCLICK(WindowID::CHOISE_LIST, PathGrid::ChoiseListBox::OnDoubleClick)
END_EVENT_TABLE()


void PathGrid::OnRightClickDown(wxMouseEvent& event) {
	wxPoint current_point = wxGetMousePosition() - GetScreenPosition();

	wxFrame* choiseFrame = new wxFrame(this, -1, "", wxGetMousePosition() - wxPoint(50,50), wxSize(100,100),
	wxBORDER_DEFAULT);
	PathGrid::ChoiseListBox* listBox = new PathGrid::ChoiseListBox(choiseFrame, {"From", "To"}, 
			std::bind(&PathGrid::OnChoiseReceived, this, std::placeholders::_1, choiseFrame, current_point));
	choiseFrame->Show();
}

void PathGrid::OnChoiseReceived(ChoiseListBox::Choises choise, wxWindow* listBoxWindow, const wxPoint& clickedPoint ) {
	listBoxWindow->Destroy();

	wxPoint clickedGrid = wxPoint(clickedPoint.x / gridSize.GetWidth(), clickedPoint.y / gridSize.GetHeight());

	switch(choise) {
		case ChoiseListBox::Choises::FROM: from = clickedGrid; break;
		case ChoiseListBox::Choises::TO: to = clickedGrid; break;
		default: break;
	}
	Refresh();
}

void PathGrid::RebuildGraph() {
	std::vector<std::vector<bool>> grid_blocks = std::vector(grid.size(), std::vector<bool>(grid.size(), false));
	for (size_t i = 0; i < grid.size(); i++) {
		for (size_t j = 0; j < grid[i].size(); j++) {
			grid_blocks[i][j] = grid[i][j].isBlock;
			grid[i][j].isVisited = false;
		}
	}
	graphTrace = {};
	if (to && from) {
		if (graphThread) {
			if(graphThread->Delete() != wxTHREAD_NO_ERROR) {
				return;
			}
		}
		graphThread = new GraphThread(this, Graph(grid_blocks));
		StartDijkstra();
	}
}

void PathGrid::StartDijkstra() {
	if (!graphThread->StartDijkstra(from.value(), to.value())) {
		delete graphThread;
		graphThread = NULL;
	}
}
void PathGrid::OnGraphThreadUpdate(wxEvent&) {
	Refresh();
}
void PathGrid::OnGraphThreadComplete(wxEvent&) {
	std::cout << "Complete" << std::endl;
	graphThread->Delete() ;
	graphThread = NULL;
	Refresh();

}
