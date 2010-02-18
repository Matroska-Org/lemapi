/*
 *  matroska_writer, Writes Matroska files
 *
 *  matroska_writer_gui.cpp
 *
 *  Copyright (C) Jory Stone - June 2003
 *
 *  matroska_writer is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  matroska_writer is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with matroska_writer; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*!
	\file matroska_writer_gui.cpp
	\version \$Id: matroska_writer_gui.cpp,v 1.1 2003/07/21 15:10:51 jcsston Exp $
	\brief Writes Matroska files
	\author Jory Stone            <jcsston @ toughguy.net>
	
	\history
		$Log: matroska_writer_gui.cpp,v $
		Revision 1.1  2003/07/21 15:10:51  jcsston
		First Commit
		
		
*/
#include "matroska_writer_gui.h"
#include "matroska_writer.h"

LemAPIPlugin *thePlugins[255];
wxArrayThread theThreads;

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(LemAPIFrame, wxFrame)
	EVT_MENU(LemAPI_Quit,  LemAPIFrame::OnQuit)
	EVT_MENU(LemAPI_About, LemAPIFrame::OnAbout)
	EVT_BUTTON(LemAPI_InputBrowse, LemAPIFrame::OnBrowseInputButton)
	EVT_BUTTON(LemAPI_Start, LemAPIFrame::OnStartButton)
	EVT_BUTTON(LemAPI_Stop, LemAPIFrame::OnStopButton)
	EVT_RADIOBUTTON(LemAPI_RadioThread, LemAPIFrame::OnRadioThread)
	EVT_RADIOBUTTON(LemAPI_RadioPlugins, LemAPIFrame::OnRadioPlugins)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(LemAPIApp)

// 'Main program' equivalent: the program execution "starts" here
bool LemAPIApp::OnInit()
{
	for (int i = 0; i < 255; i++)
		thePlugins[i] = NULL;
	// create the main application window
	LemAPIFrame *frame = new LemAPIFrame(_T("LemAPI Matroska Writer"), wxPoint(50, 50), wxSize(450, 340));

	wxCHECK_MSG(frame, FALSE, _T("Failed to create main window frame"));

	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	frame->Show(TRUE);

	// success: wxApp::OnRun() will be called which will enter the main message
	// loop and the application will run. If we returned FALSE here, the
	// application would exit immediately.
	return TRUE;
	}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
LemAPIFrame::LemAPIFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, style)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(LemAPI_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(LemAPI_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR    
    CreateStatusBar(1);
    SetStatusText(_T("Welcome to LemAPI Matroska Writer!"));
#endif // wxUSE_STATUSBAR

    panel_1 = new wxPanel(this, -1);
    label_1 = new wxStaticText(panel_1, -1, _T(" Input File:"));
    inputFile_TextCtrl = new wxTextCtrl(panel_1, LemAPI_InputBox, _T(""));
    inputFileBrowse_Button = new wxButton(panel_1, LemAPI_InputBrowse, _T("Add Input File"));
    output_TextCtrl = new wxTextCtrl(panel_1, LemAPI_OutputTextBox, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    start_Button = new wxButton(panel_1, LemAPI_Start, _T("Start"));
    stop_Button = new wxButton(panel_1, LemAPI_Stop, _T("Stop"));
		label_percent = new wxStaticText(panel_1, -1, _T("Percent Complete: 0%"));
    gauge_percent = new wxGauge(panel_1, LemAPI_PercentGauge, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_PROGRESSBAR|wxGA_SMOOTH);
    radio_btn_threads = new wxRadioButton(panel_1, LemAPI_RadioThread, _T("Running Threads"));
    radio_btn_plugins = new wxRadioButton(panel_1, LemAPI_RadioPlugins, _T("Loaded Plugins"));
		 const wxString list_box_threads_choices[] = {
        _T("")
    };
    list_box_threads = new wxListBox(panel_1, LemAPI_ThreadList, wxDefaultPosition, wxDefaultSize, 1, list_box_threads_choices, wxLB_HSCROLL);

    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_8 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_9 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_7 = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizer_5 = new wxStaticBoxSizer(new wxStaticBox(panel_1, -1, _T("Output")), wxHORIZONTAL);
    wxBoxSizer* sizer_12 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxHORIZONTAL);
    sizer_3->Add(label_1, 0, wxALL, 2);
    sizer_4->Add(inputFile_TextCtrl, 1, wxALL, 2);
    sizer_4->Add(inputFileBrowse_Button, 0, wxRIGHT, 5);
    sizer_3->Add(sizer_4, 0, wxEXPAND, 0);
    sizer_2->Add(sizer_3, 0, wxEXPAND, 0);
    sizer_5->Add(output_TextCtrl, 1, wxEXPAND, 0);
    sizer_12->Add(start_Button, 0, wxALL, 3);
    sizer_12->Add(stop_Button, 0, wxALL, 3);
    sizer_5->Add(sizer_12, 0, wxTOP, 2);
    sizer_2->Add(sizer_5, 1, wxALL|wxEXPAND, 3);
    sizer_7->Add(label_percent, 0, wxALL, 3);
    sizer_7->Add(gauge_percent, 1, wxALL|wxEXPAND|wxALIGN_BOTTOM, 2);
    sizer_6->Add(sizer_7, 1, wxEXPAND, 0);
    sizer_9->Add(radio_btn_threads, 0, wxALL, 2);
    sizer_9->Add(radio_btn_plugins, 0, wxALL, 2);
    sizer_8->Add(sizer_9, 0, wxEXPAND, 0);
    sizer_8->Add(list_box_threads, 1, wxALL|wxEXPAND, 3);
    sizer_6->Add(sizer_8, 1, wxEXPAND, 0);
    sizer_2->Add(sizer_6, 1, wxEXPAND, 0);
    panel_1->SetAutoLayout(true);
    panel_1->SetSizer(sizer_2);
    sizer_2->Fit(panel_1);
    sizer_2->SetSizeHints(panel_1);
    sizer_1->Add(panel_1, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    sizer_1->SetSizeHints(this);
    Layout();

		LoadPlugins(wxGetCwd() + _T("\\plugins\\"));

		radio_btn_plugins->SetValue(1);
		//OnRadioPlugins(wxCommandEvent());
};


// event handlers

void LemAPIFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void LemAPIFrame::OnBrowseInputButton(wxCommandEvent& event)
{
	wxString supportedTypes, all_supportedTypes;
	int p = 0;
	while (thePlugins[p] != NULL)
	{
		LemAPIPlugin *aPlugin = thePlugins[p];
		wxCHECK_RET(aPlugin, _T("Failed to list supported file types"));
		for (int e = 0; e < aPlugin->supportedFileTypes.GetCount(); e++)
		{
			wxString supportedExt = aPlugin->supportedFileTypes[e];
			supportedTypes += supportedExt + _T("|");
			all_supportedTypes += _T("*.") + supportedExt.AfterLast('.') + _T(";");
		}
		p++;
	}
	supportedTypes += _T("All Files (*.*)|*.*");
	supportedTypes = _T("Supported Types (") + all_supportedTypes + _T(") |") + all_supportedTypes + _T("|") + supportedTypes;
	wxFileDialog dialog (this, _T("Locate input files"), _T(""), _T(""), supportedTypes, wxOPEN);

	if (dialog.ShowModal() == wxID_OK) {
		wxString input_file;
		
		input_file = dialog.GetPath();
		inputFile_TextCtrl->SetValue(input_file);
	}
}

void LemAPIFrame::OnStartButton(wxCommandEvent& event)
{
	if (!wxFileExists(inputFile_TextCtrl->GetValue())) {
		wxMessageBox(_T("The Input file: \"") + inputFile_TextCtrl->GetValue() + _T("\"\nDoes not exist!"), _T("Invalid Input Filename"));
		return;
	}

	MatroskaWriter *new_writer = new MatroskaWriter(this, output_TextCtrl, inputFile_TextCtrl->GetValue());
	wxCHECK_RET(new_writer, _T("Failed to create new MatroskaWriter thread object"));

	wxFileDialog dialog(this, _T("Locate files to attach"), _T(""), _T(""), _T("All Files (*.*)|*.*"), wxOPEN);
	while (dialog.ShowModal() == wxID_OK)
	{		
		wxString attach_wxfile = dialog.GetPath();
		char *attach_file = new char[attach_wxfile.Len()];
		strcpy(attach_file, attach_wxfile.mb_str());
		attach_file[attach_wxfile.Len()] = 0;
		new_writer->AddAttachmentFilename(attach_file);		
	}
	new_writer->Create();
	new_writer->Run();
	theThreads.Add(new_writer);
}
void LemAPIFrame::OnStopButton(wxCommandEvent& event)
{
	if (theThreads.GetCount() < 1) {
		SetStatusText(_T("Nothing to stop :P"));		
	}else {
		theThreads.Last()->Delete();
	}
};

void LemAPIFrame::OnRadioThread(wxCommandEvent& event)
{
	list_box_threads->Clear();
	for (int t = 0; t < theThreads.GetCount(); t++) {
		MatroskaWriter *next_thread = (MatroskaWriter *)theThreads[t];
		wxString thread_txt;
		wxFileName inputFileName(next_thread->GetInputFilename());
		thread_txt.Printf(_T("%s: %0i%%"), inputFileName.GetName(), next_thread->GetPercentDone());
		list_box_threads->Append(thread_txt);
	}
};

void LemAPIFrame::OnRadioPlugins(wxCommandEvent& event)
{
	list_box_threads->Clear();
	int t = 0;
	while (thePlugins[t] != NULL) {
		LemAPIPlugin *next_plugin = (LemAPIPlugin *)thePlugins[t];
		wxString plugin_txt;
		plugin_txt.Printf(_T("%s %s - by %s"), next_plugin->name, next_plugin->version , next_plugin->author);
		list_box_threads->Append(plugin_txt);
		t++;
	}
};

void LemAPIFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg = _T("LemAPI Matroska Writer is the product of creative thinking gone amuck ;)\n");

    wxMessageBox(msg, _T("About LemAPI Matroska Writer"), wxOK | wxICON_INFORMATION, this);
}

void LemAPIFrame::LoadPlugins(wxString pluginDir)
{
	//DEBUG(_T("LemAPIFrame::LoadPlugins"));
	if(!wxDirExists(pluginDir)) {
		SetStatusText(_T("No plugin dir found"));
		return;
	}

	wxStringList list_of_files;
	wxString *current_file = new wxString();
	wxArrayString file_types;
	int num_of_files = 0;

	//Setup the file types to scan for
	file_types.Alloc(1);														//< This should improve the speed a tiny bit ;P
	file_types.Add(_T("*.dll"));

	//Do a loop for each file type in the list
	for (int current_file_type = 0; current_file_type < file_types.Count(); current_file_type++)
	{		
		wxDir level_0_folder(pluginDir);
		if (level_0_folder.IsOpened())
		{
			bool good_name = level_0_folder.GetFirst(current_file, file_types.Item(current_file_type));
			while (good_name)
			{
				wxString curent_filename = pluginDir;
				curent_filename += *current_file;
				
				LemAPIPlugin *new_plugin = new LemAPIPlugin(curent_filename);
				if (new_plugin->IsPluginOk()) {
					int p = 0;					
					while (thePlugins[p] != NULL)
						p++;

					thePlugins[p] = new_plugin;
				}
				
				//Go to next item
				good_name = level_0_folder.GetNext(current_file);
				num_of_files++;
			}
		}
	}
	
	SetStatusText(wxString::Format(_T("Found %i reader plugins"), num_of_files));	
};
