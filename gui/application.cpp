#include "application.hpp"
#include "mainFrame.hpp"

constexpr char appName[] = "ShortPath algo";


IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
	MainFrame* frame = new MainFrame(wxString(appName));
	frame->Show(true);
	return true;
}


