#include "wx/grid.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/listbox.h"
#include "wx/thread.h"
#include "wx/event.h"

#include <memory>
#include <vector>
#include <optional>


#include "graph.hpp"

wxDECLARE_EVENT(wxEVT_GRAPH_THREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_GRAPH_THREAD_UPDATE, wxThreadEvent);

class PathGrid : public wxPanel {
	struct GridData {
		bool isBlock = false;
		bool isVisited = false;
	};
public:
	PathGrid(wxWindow* parent, const wxPoint& pos, const wxSize& size, int amount = 20);

	GridData& GetGrid(wxPoint gridPos) { return grid[gridPos.x][gridPos.y]; }
	std::vector<wxPoint>& GraphTrace() { return graphTrace; }

private:
	class ChoiseListBox : public wxListBox {
	public:
		enum Choises {
			FROM,
			TO,
		};
		ChoiseListBox(wxWindow* parent, const std::vector<wxString>& choises, std::function<void(Choises choise)> cob);

	private:
		void OnDoubleClick(wxCommandEvent&);
		DECLARE_EVENT_TABLE();
	
		std::function<void(Choises)> callback;
	};
	void ShowGrid(wxPaintEvent&);
	void GridClick(wxMouseEvent&);
	void OnLeftMouseUp(wxMouseEvent&);
	void OnRightClickDown(wxMouseEvent&);
	void OnMouseMotion(wxMouseEvent&);
	void OnChoiseReceived(ChoiseListBox::Choises choise, wxWindow* listBoxWindow, const wxPoint& clickedPoint);
	void OnGraphThreadUpdate(wxEvent&);
	void OnGraphThreadComplete(wxEvent&);

	void RebuildGraph();

	void StartDijkstra();

	wxSize gridSize;
	std::vector<std::vector<GridData>> grid;

	std::optional<wxPoint> from, to;
	std::vector<wxPoint> visitedGrids;
	std::vector<wxPoint> graphTrace;

	bool rightClicked = false;

	wxWindow* parent_;
	DECLARE_EVENT_TABLE();

	class GraphThread: public wxThread {
	public:
		GraphThread(PathGrid* handler, Graph&& graph);
		bool StartDijkstra(wxPoint from, wxPoint to);
	protected:
		virtual ExitCode Entry() override;
	private:
		Graph graph_;
		PathGrid* handler_;
		wxPoint from, to;
	};

	GraphThread* graphThread;
	wxCriticalSection graphThreadCriticalSection;
};
