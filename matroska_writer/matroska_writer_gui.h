/*
 *  matroska_writer, Writes Matroska files
 *
 *  matroska_writer_gui.h
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
	\file matroska_writer_gui.h
	\version \$Id: matroska_writer_gui.h,v 1.1 2003/07/21 15:10:51 jcsston Exp $
	\brief Writes Matroska files
	\author Jory Stone            <jcsston @ toughguy.net>
	
	\history
		$Log: matroska_writer_gui.h,v $
		Revision 1.1  2003/07/21 15:10:51  jcsston
		First Commit
		
		
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// Define a new application type, each program should derive a class from wxApp
class LemAPIApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();    
};

// Define a new frame type: this is going to be our main frame
class LemAPIFrame : public wxFrame
{
public:
    // ctor(s)
    LemAPIFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
		void OnBrowseInputButton(wxCommandEvent& event);
		void OnStartButton(wxCommandEvent& event);
		void OnStopButton(wxCommandEvent& event);
		void OnRadioThread(wxCommandEvent& event);
		void OnRadioPlugins(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

		void LoadPlugins(wxString pluginDir);

		wxTextCtrl *inputFile_TextCtrl;
		wxButton *inputFileBrowse_Button;
		wxTextCtrl *output_TextCtrl;
		wxButton *start_Button;
		wxButton *stop_Button;

    wxStaticText* label_1;
    wxStaticText* label_percent;
    wxGauge* gauge_percent;
    wxRadioButton* radio_btn_threads;
    wxRadioButton* radio_btn_plugins;
    wxListBox* list_box_threads;
    wxPanel* panel_1;
private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    LemAPI_Quit = 1,
		LemAPI_Start,
		LemAPI_Stop,
		LemAPI_InputBox,
		LemAPI_InputBrowse,
		LemAPI_OutputTextBox,
		LemAPI_ThreadList,
		LemAPI_RadioThread,
		LemAPI_RadioPlugins,
		LemAPI_PercentGauge,


    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    LemAPI_About = wxID_ABOUT
};

