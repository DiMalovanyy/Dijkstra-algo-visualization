#include "mainFrame.hpp"

#include "WINDOW_IDS.hpp"

MainFrame::MainFrame(const wxString& title) : 
	wxFrame((wxFrame*)NULL, 
	WindowID::MAIN_FRAME, 
	title, 
	wxPoint(0, 0),
	wxSize(800,830),
	wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN) {

	pathGrid = new PathGrid(this, 
			wxPoint(
				GetPosition().x + GetSize().GetWidth() * 0.1,
				GetPosition().y + GetSize().GetHeight() * 0.1),
			wxSize(
				GetSize().GetWidth() * 0.8,
				GetSize().GetWidth() * 0.8));

								
	wxPanel* buttons = new wxPanel(this, WindowID::BUTTONS, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	Center();
}
