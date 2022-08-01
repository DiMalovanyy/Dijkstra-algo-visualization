#include "wx/frame.h"
#include "wx/sizer.h"

#include "pathGrid.hpp"

#include <memory>
#include <utility>

class MainFrame: public wxFrame {
public:
	MainFrame(const wxString& title);
private:
	PathGrid* pathGrid;
};
