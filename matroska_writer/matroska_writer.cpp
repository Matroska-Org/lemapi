/*
 *  matroska_writer, Writes Matroska files
 *
 *  matroska_writer.cpp
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
	\file matroska_writer.cpp
	\version \$Id: matroska_writer.cpp,v 1.1 2003/07/21 15:10:51 jcsston Exp $
	\brief Writes Matroska files
	\author Jory Stone            <jcsston @ toughguy.net>
	
	\history
		$Log: matroska_writer.cpp,v $
		Revision 1.1  2003/07/21 15:10:51  jcsston
		First Commit
		
		
*/

#include "matroska_writer_gui.h"
#include "matroska_writer.h"

extern LemAPIPlugin *thePlugins[255];
extern wxArrayThread theThreads;

using namespace LIBMATROSKA_NAMESPACE;
using namespace LIBEBML_NAMESPACE;
using namespace std;

LemAPIPlugin::LemAPIPlugin(wxString &pluginFilename)
{
	wxASSERT_MSG(wxFileExists(pluginFilename), _T("Plugin " + pluginFilename + " doesn't exist!"));
	
	pluginDll.Load(pluginFilename);
	if (pluginDll.IsLoaded()) {
		GetLemAPIVersion pVersion = (GetLemAPIVersion)pluginDll.GetSymbol("GetLemAPIVersion");
		if (pVersion != NULL) {
			char *plugin_version = pVersion(1);
			if (plugin_version != NULL) {
				//Process the Version XML document
				TiXmlDocument xmlVersion("");
				xmlVersion.Parse(plugin_version);
#ifdef DEBUG
				xmlVersion.SaveFile("version.xml");
#endif
				TiXmlElement *root_element = xmlVersion.RootElement();
				if (!strcmp(root_element->Value(), "LemAPI"))
				{
					//Good this is our document
					TiXmlNode *level_1 = root_element->IterateChildren(NULL);
					while (level_1 != NULL)
					{					
						if (!stricmp(level_1->Value(), "Plugin"))
						{				
							TiXmlNode *level_2 = level_1->IterateChildren(NULL);
							while (level_2 != NULL)
							{
								if (!stricmp(level_2->Value(), "Name")) {
									TiXmlNode *level_2_data = level_2->FirstChild();
									if (level_2_data != NULL) {
										this->name = wxString(level_2_data->Value(), wxConvUTF8);
									}
								}else if (!stricmp(level_2->Value(), "Author")) {
									TiXmlNode *level_2_data = level_2->FirstChild();
									if (level_2_data != NULL) {
										this->author = wxString(level_2_data->Value(), wxConvUTF8);
									}
								}else if (!stricmp(level_2->Value(), "URL")) {
									TiXmlNode *level_2_data = level_2->FirstChild();
									if (level_2_data != NULL) {
										this->website = wxString(level_2_data->Value(), wxConvUTF8);
									}
								}else if (!stricmp(level_2->Value(), "Version")) {
									TiXmlNode *level_2_data = level_2->FirstChild();
									if (level_2_data != NULL) {
										this->version = wxString(level_2_data->Value(), wxConvUTF8);
									}
								}else if (!stricmp(level_2->Value(), "SupportedFileTypes")) {
									TiXmlNode *level_3 = level_2->IterateChildren(NULL);
									while (level_3 != NULL)
									{
										if (!stricmp(level_3->Value(), "FileType")) {
											TiXmlNode *level_4 = level_3->IterateChildren(NULL);
											wxString fileType_Name, fileType_Ext;
											while (level_4 != NULL)
											{
												if (!stricmp(level_4->Value(), "Name")) {
													TiXmlNode *level_4_data = level_4->FirstChild();
													if (level_4_data != NULL) {
														fileType_Name = wxString(level_4_data->Value(), wxConvUTF8);
													}
												}else if (!stricmp(level_4->Value(), "Ext")) {
													TiXmlNode *level_4_data = level_4->FirstChild();
													if (level_4_data != NULL) {
														fileType_Ext = wxString(level_4_data->Value(), wxConvUTF8);
													}
												}
												level_4 = level_3->IterateChildren(level_4);
											}
											this->supportedFileTypes.Add(fileType_Name + _T("| *.") + fileType_Ext.Trim(false));
										}
										level_3 = level_2->IterateChildren(level_3);
									}	// while (level_3 != NULL)
								}else if (!stricmp(level_2->Value(), "Option")) {

								}
								level_2 = level_1->IterateChildren(level_2);
							} //while (level_2 != NULL)
						}
						level_1 = root_element->IterateChildren(level_1);
					}
				}
			}else {
				//This must not be a LemAPI plugin
				pluginOk = false;
			}
		}else {
			//This must not be a LemAPI plugin
			pluginOk = false;
		}
	}else {
		pluginOk = false;
	}
};

MatroskaWriter::MatroskaWriter(wxFrame *frame, wxTextCtrl *outputBox, wxString passed_InputFile, wxString passed_OutputFile)
{	
	console_output_box = outputBox;
	parentFrame = frame;
	//Store filenames
	input_Filename = passed_InputFile;

	if (passed_OutputFile == "") {
		output_Filename = passed_InputFile + ".mkv";
	} else {
		output_Filename = passed_OutputFile;
	}

	int size = sizeof(*this);
	clust = NULL;
	needPframeRef =false;
	data = NULL;
	
	noLacing = false;
	bVerbose = false;
	crcEnabled = false;
	maxTimePerCluster = 2000; //2 seconds
	maxClusterSize = 512 * 1024; //512kB
	currentClusterSize = 0;
	currentClusterBaseTimecode = 0;
	currentClusterBlockTimecode = 0;
	TIMECODE_SCALE = 1000000;
			
	for (int i = 0; i < 255; i++)
	{
		MyTrack[i] = NULL;
		currentBlockGroupFull[i] = false;
		currentBlockGroup[i] = NULL;
		lastBlockGroup[i] = NULL;		
		track_uid[i] = 0;
		attachment_array[i] = NULL;
		theTrackInfo[i] = NULL;
	}	
	
	attachment_count = 0;
	theTrackInfoCount = 0;	

	totalFrameCount = 0;
	totalClusterCount = 0;
	percentDone = 0;
	lastPercentDone = 0;
};

MatroskaWriter::~MatroskaWriter()
{
	//Free memory here
	for (int i = 0; i < 20; i++)
	{
		delete attachment_array[i];
		attachment_array[i] = NULL;
	}
	/*for (int i = 0; i < 255; i++)
	{
		delete MyTrack[i];
		MyTrack[i] = NULL;
	}*/

};

bool MatroskaWriter::SetInputFile(wxString &passed_InputFile)
{
	input_Filename = passed_InputFile;
	return true;
};

bool MatroskaWriter::SetOutputFile(wxString &passed_OutputFile)
{
	output_Filename = passed_OutputFile;
	return true;
};

bool MatroskaWriter::SetVerboseModeOn(bool bVerboseOn)
{
	bVerbose = bVerboseOn;
	return true;
};

bool MatroskaWriter::SetLacingOff(bool bLacingOff)
{
	noLacing = bLacingOff;
	return true;
};

bool MatroskaWriter::SetMaxClusterByteSize(uint64 passed_maxClusterSize)
{
	//The allowed range is 128 bytes to 1073741824 bytes or 1GB
	if ((passed_maxClusterSize > 128) && (passed_maxClusterSize < 1073741824)) {
		maxClusterSize = passed_maxClusterSize;
		return true;
	}else {
		//Assign the default value
		maxClusterSize = 512 * 1024;
		return false;
	}
};

bool MatroskaWriter::SetMaxClusterTimeSize(uint32 passed_maxTimePerCluster)
{
	//The allowed range is 10ms to 3600000ms or 1 hour
	if ((passed_maxTimePerCluster > 10) && (passed_maxTimePerCluster < 3600000)) {
		maxTimePerCluster = passed_maxTimePerCluster;
		return true;
	}else {
		//Assign the default value, 5 seconds
		maxTimePerCluster = 5000;
		return false;
	}
};

bool MatroskaWriter::AddAttachmentFilename(char *filename)
{
	if (filename != NULL)
	{
		attachment_array[attachment_count] = filename;
		attachment_count++;	
		return true;
	}
	return false;
};

void MatroskaWriter::OutputToBuffer(char *formating_string, ...)
{
	va_list argptr;

	//Get a pointer to the list of arg
	va_start(argptr, formating_string);	
#if defined wxUSE_GUI 
	//Redirect the output to a wxWindow wxTextCtrl
	wxString formated_text;
	formated_text.PrintfV(wxString(formating_string, wxConvUTF8).c_str(), argptr);
	wxMutexGuiEnter();
	console_output_box->AppendText(formated_text);
	//Give the processor some relief
	wxYield();
	wxMutexGuiLeave();
#else
	//Just let the function call pass through unaffected
	vprintf(formating_string, argptr);	
#endif
}

void MatroskaWriter::UpdatePercentBar(unsigned int newPercent)
{
	if (parentFrame != NULL)
	{
		wxMutexGuiEnter();
		((LemAPIFrame *)parentFrame)->gauge_percent->SetValue(newPercent);
		((LemAPIFrame *)parentFrame)->label_percent->SetLabel(wxString::Format(_T("Percent Complete: %0u%%"), newPercent));		
		/*if (((LemAPIFrame *)parentFrame)->radio_btn_threads->GetValue()) {
			wxString name_to_search_for = wxFileName(input_Filename).GetName();
			wxListBox * theListBox = ((LemAPIFrame *)parentFrame)->list_box_threads;
			for (int t = 0; t < theListBox->GetCount(); t++) {
				if (!theListBox->GetString(t).BeforeFirst(':').Cmp(name_to_search_for.c_str())) {
					theListBox->SetString(t, wxString::Format(_T("%s: %0i%%"), name_to_search_for, newPercent));
				}
			}
		}*/
		wxMutexGuiLeave();	
	}
};

int MatroskaWriter::Convert2Matroska()
{
	//Store values
	if (input_Filename.length() < 2)
		return -1;

	if (output_Filename == "") {
		output_Filename = input_Filename + ".mkv";
	}
	
	wxDynamicLibrary *inputPlugin = NULL;
	wxString inputExt = input_Filename.AfterLast('.');
	int p = 0;
	while (thePlugins[p] != NULL)
	{
		LemAPIPlugin *aPlugin = thePlugins[p];		
		for (int e = 0; e < aPlugin->supportedFileTypes.GetCount(); e++)
		{
			wxString supportedExt = aPlugin->supportedFileTypes[e];
			if (supportedExt.AfterLast('.').IsSameAs(inputExt, false)) {
				inputPlugin = &aPlugin->pluginDll;
				break;
			}
		}
		p++;
	}
	if (inputPlugin == NULL)
	{
		printf("No plugin found to handle %s's\n", inputExt.mb_str());
		return -1;
	}

	try {		
		if (bVerbose)
		{
			printf("In Verbose Mode, prepare to be blasted with numbers\n");
		}
		if (noLacing)
		{
			printf("Lacing turned off\n");
		}		
		
		long start_time = clock();
		CreateLemAPIProcess pCreatePluginProcess = (CreateLemAPIProcess)inputPlugin->GetSymbol("CreateLemAPIProcess");
		void *current_plugin = pCreatePluginProcess(NULL);

		printf("Reading Input File Headers... ");
		ReadFileHeaders pReadHeaders = (ReadFileHeaders)inputPlugin->GetSymbol("ReadFileHeaders");
		pReadHeaders(current_plugin, input_Filename.mb_str());		
    printf("Done\n");

		// Open the output file		
    StdIOCallback output_file(output_Filename.c_str(), MODE_CREATE);
 
		GetTrackInfoXML_ASCII pGetTrackInfoXML_ASCII = (GetTrackInfoXML_ASCII)inputPlugin->GetSymbol("GetTrackInfoXML_ASCII");
		char *track_infos = pGetTrackInfoXML_ASCII(current_plugin);

		ParseTrackXML(track_infos);

		// Setup the Matroska output file
		InitMatroskaOutputFile(output_file);

		printf("Starting Matroska muxing... ");		 

		GetNextDataFrame pGetNextDataFrame = (GetNextDataFrame)inputPlugin->GetSymbol("GetNextDataFrame");
		FreeDataFrame pFreeDataFrame = (FreeDataFrame)inputPlugin->GetSymbol("FreeDataFrame");
		
		DataPacket *current_packet = NULL;		
		do {			
			current_packet = pGetNextDataFrame(current_plugin);
			if (current_packet != NULL)
			{
				++totalFrameCount;
				percentDone = (100 / (float)theTrackInfo[0]->duration) * (float)(current_packet->StartTimecode+1);
				if (percentDone != lastPercentDone)
				{
					lastPercentDone = percentDone;
					if (percentDone == 99)
						printf("Almost Done... ");
					UpdatePercentBar(percentDone);					
				}				
				if (current_packet->prevReference == 0) {
					AddFrame(output_file, current_packet, true);
				}else {
					AddFrame(output_file, current_packet, false);
				}
				pFreeDataFrame(current_packet);
				if (TestDestroy())
				{
					printf("Muxing aborted!\n");
					CloseMatroskaFile(output_file);
					output_file.close();
					DeleteLemAPIProcess pDeleteLemAPIProcess = (DeleteLemAPIProcess)inputPlugin->GetSymbol("DeleteLemAPIProcess");
					pDeleteLemAPIProcess(current_plugin);
					return 0;
				}
			}
		} while (current_packet != NULL);
    printf("Done\n");
    CloseMatroskaFile(output_file);

    output_file.close();
		
		float totalTimeElapsed = (float)(clock()-start_time) / CLOCKS_PER_SEC;
		printf("MatroskaWriter Complete\nTook %.2f seconds to mux %u frames into %u clusters\nAvg Muxing Rate: %.2f fps\n", totalTimeElapsed, totalFrameCount, totalClusterCount, (float)totalFrameCount / totalTimeElapsed);

    DeleteLemAPIProcess pDeleteLemAPIProcess = (DeleteLemAPIProcess)inputPlugin->GetSymbol("DeleteLemAPIProcess");
		pDeleteLemAPIProcess(current_plugin);
		
		return 0;    
    }catch (exception & Ex){
        cout << Ex.what() << endl;
        return -1;
    }
}

int MatroskaWriter::InitMatroskaOutputFile(StdIOCallback & output_file){
	try{
		// write output file
		EbmlHead FileHead;
	  
		EDocType & MyDocType = GetChild<EDocType>(FileHead);
		*static_cast<EbmlString *>(&MyDocType) = "matroska";
	  
		EDocTypeVersion & MyDocTypeVer = GetChild<EDocTypeVersion>(FileHead);
		*(static_cast<EbmlUInteger *>(&MyDocTypeVer)) = 1;
	  
		EDocTypeReadVersion & MyDocTypeReadVer = GetChild<EDocTypeReadVersion>(FileHead);
		*(static_cast<EbmlUInteger *>(&MyDocTypeReadVer)) = 1;
	  
		FileHead.Render(output_file);

		// size is unknown and will always be, we can render it right away
		uint64 SegmentSize = FileSegment.WriteHead(output_file, 5);
		
		//Write a dummy element to reserve space for the seekhead
		Dummy.SetSize(2000);
		Dummy.Render(output_file);

		Dummy_Crc.SetSize(10);
		Dummy_Crc.Render(output_file);

		//Setup the Infomation segment
		SetupInfo(output_file);

		//Setup the track entries
		SetupTracks(output_file);
		
		//Setup the Cues but don't render them yet
		//AllCues = &GetChild<KaxCues>(FileSegment);

		printf("Matroska File created...\n");
		return true;
	}catch (exception & Ex) {
		cout << Ex.what() << endl;        
	}
	return false;
}

int MatroskaWriter::ParseTrackXML(const char *inputXML)
{
	TiXmlDocument xml_track_data("");
	xml_track_data.Parse(inputXML);
#ifdef DEBUG
	xml_track_data.SaveFile("track.xml");
#endif //DEBUG
	TiXmlElement *root_element = xml_track_data.RootElement();
	if (!strcmp(root_element->Value(), "LemAPI"))
	{
		//Good this is our document
		TiXmlNode *level_1 = root_element->IterateChildren(NULL);
		while (level_1 != NULL)
		{
			if (!stricmp(level_1->Value(), "TrackList"))
			{				
				TiXmlNode *level_2 = level_1->IterateChildren(NULL);				
				trackData *new_track = new trackData;
				while (level_2 != NULL)
				{
					if (!stricmp(level_2->Value(), "Track")) {
						TiXmlNode *level_3 = level_2->IterateChildren(NULL);				
												
						trackData *new_track = new trackData;
						memset((void *)new_track, 0, sizeof(trackData));
						while (level_3 != NULL)
						{
							if (!stricmp(level_3->Value(), "TrackType")) {
								TiXmlNode *level_3_data = level_3->FirstChild();
								if (level_3_data != NULL) {
									if (!stricmp(level_3_data->Value(), "audio"))
										new_track->trackType = track_audio;
									else if (!stricmp(level_3_data->Value(), "video"))
										new_track->trackType = track_video;
								}
							}else if (!stricmp(level_3->Value(), "CodecID")) {
								TiXmlNode *level_3_data = level_3->FirstChild();
								if (level_3_data != NULL) {
									new_track->codecName = new char[strlen(level_3_data->Value())+1];
									strcpy(new_track->codecName, level_3_data->Value());
									new_track->codecName[strlen(level_3_data->Value())] = 0;
								}
							}else if (!stricmp(level_3->Value(), "CodecPrivate")) {
								TiXmlNode *level_3_data = level_3->FirstChild();
								if (level_3_data != NULL) {
									const char *input = level_3_data->Value();									
									long length = strlen(input);
									long decoded_length = (length+3)/4*3;
									new_track->extraCodecData = new binary[decoded_length];
									memunbase64((char *)new_track->extraCodecData, input, decoded_length);
									new_track->extraCodecDataSize = decoded_length-1;
								}
							}else if (!stricmp(level_3->Value(), "Duration")) {
								TiXmlNode *level_3_data = level_3->FirstChild();
								if (level_3_data != NULL) {
									new_track->duration = _atoi64(level_3_data->Value());
								}
							}else if (!stricmp(level_3->Value(), "DefaultDuration")) {
								TiXmlNode *level_3_data = level_3->FirstChild();
								if (level_3_data != NULL) {
									new_track->defaultDuration = _atoi64(level_3_data->Value());
								}
							}else if (!stricmp(level_3->Value(), "Audio")) {
								TiXmlNode *level_4 = level_3->IterateChildren(NULL);
								if (new_track->audio == NULL) {
									new_track->audio = new audioData;
									memset((void *)new_track->audio, 0, sizeof(audioData));
								}
								while (level_4 != NULL)
								{
									if (!stricmp(level_4->Value(), "SampleRate")) {
										TiXmlNode *level_4_data = level_4->FirstChild();
										if (level_4_data != NULL) {
											new_track->audio->sample_rate = atof(level_4_data->Value());
										}										
									}else if (!stricmp(level_4->Value(), "Channels")) {
										TiXmlNode *level_4_data = level_4->FirstChild();
										if (level_4_data != NULL) {
											new_track->audio->channels = atol(level_4_data->Value());
										}										
									}else if (!stricmp(level_4->Value(), "BitDepth")) {
										TiXmlNode *level_4_data = level_4->FirstChild();
										if (level_4_data != NULL) {
											new_track->audio->bit_depth = atol(level_4_data->Value());
										}										
									}else if (!stricmp(level_4->Value(), "AvgBitrate")) {
										TiXmlNode *level_4_data = level_4->FirstChild();
										if (level_4_data != NULL) {
											new_track->audio->avg_bitrate = atol(level_4_data->Value());
										}										
									}
									level_4 = level_3->IterateChildren(level_4);
								} //while (level_4 != NULL)
							}else if (!stricmp(level_3->Value(), "Video")) {
								TiXmlNode *level_4 = level_3->IterateChildren(NULL);
								if (new_track->video == NULL) {
									new_track->video = new videoData;
									memset((void *)new_track->video, 0, sizeof(videoData));
								}
								while (level_4 != NULL)
								{
									if (!stricmp(level_4->Value(), "Width")) {
										TiXmlNode *level_4_data = level_4->FirstChild();
										if (level_4_data != NULL) {
											new_track->video->pixel_x = atol(level_4_data->Value());
										}																				
									}else if (!stricmp(level_4->Value(), "Height")) {
										TiXmlNode *level_4_data = level_4->FirstChild();
										if (level_4_data != NULL) {
											new_track->video->pixel_y = atol(level_4_data->Value());
										}																				
									}else if (!stricmp(level_4->Value(), "ColorDepth")) {
										TiXmlNode *level_4_data = level_4->FirstChild();
										if (level_4_data != NULL) {
											new_track->video->color_depth = atoi(level_4_data->Value());
										}																				
									}else if (!stricmp(level_4->Value(), "FrameRate")) {
										TiXmlNode *level_4_data = level_4->FirstChild();
										if (level_4_data != NULL) {
											new_track->video->frame_rate = atof(level_4_data->Value());
										}																				
									}
									level_4 = level_3->IterateChildren(level_4);
								} // while (level_4 != NULL)
							}
							level_3 = level_2->IterateChildren(level_3);
						}	// while (level_3 != NULL)
						theTrackInfo[theTrackInfoCount] = new_track;
						theTrackInfoCount++;
					}
					level_2 = level_1->IterateChildren(level_2);
				} //while (level_2 != NULL)
			}else if (!stricmp(level_1->Value(), "")) {

			}
			level_1 = root_element->IterateChildren(level_1);
		}
	}
	return 0;
}

int MatroskaWriter::SetupInfo(StdIOCallback & output_file)
{
	try {
		//Add the Info segment
		KaxInfo & MyInfos = GetChild<KaxInfo>(FileSegment);
		KaxSegmentFilename & MyKaxSegmentFilename = GetChild<KaxSegmentFilename>(MyInfos);
		*static_cast<EbmlUnicodeString *>(&MyKaxSegmentFilename) = ConvertCharToWchar_t(input_Filename.c_str());

		KaxTitle & MyKaxTitle = GetChild<KaxTitle>(MyInfos);
		*static_cast<EbmlUnicodeString *>(&MyKaxTitle) = ConvertCharToWchar_t(input_Filename.c_str());		

		//Add info tag for writing app
		KaxWritingApp & MyKaxWritingApp = GetChild<KaxWritingApp>(MyInfos);
		*static_cast<EbmlUnicodeString *>(&MyKaxWritingApp) = ConvertCharToWchar_t("LemAPI Matroska Writer");

		std::string muxingapp;
		muxingapp.append("libebml v");
		muxingapp.append(LIBEBML_NAMESPACE::EbmlCodeVersion.c_str());
		muxingapp.append(" compiled on ");
		muxingapp.append(LIBEBML_NAMESPACE::EbmlCodeDate.c_str());
		muxingapp.append(" libmatroska v");
		muxingapp.append(LIBMATROSKA_NAMESPACE::KaxCodeVersion.c_str());
		muxingapp.append(" compiled on ");
		muxingapp.append(LIBMATROSKA_NAMESPACE::KaxCodeDate.c_str());

		KaxMuxingApp & MyKaxMuxingApp = GetChild<KaxMuxingApp>(MyInfos);
		*static_cast<EbmlUnicodeString *>(&MyKaxMuxingApp) = ConvertCharToWchar_t(muxingapp.c_str());
		
		KaxDateUTC &MyKaxDateUTC = GetChild<KaxDateUTC>(MyInfos);
		MyKaxDateUTC.SetEpochDate(time(NULL));
		
		//Add timecode scale
		KaxTimecodeScale & TimeScale = GetChild<KaxTimecodeScale>(MyInfos);
		*(static_cast<EbmlUInteger *>(&TimeScale)) = TIMECODE_SCALE;
											 
		KaxDuration & Duration = GetChild<KaxDuration>(MyInfos);
		if (theTrackInfo[0] != NULL)
			*(static_cast<EbmlFloat *>(&Duration)) = theTrackInfo[0]->duration;
		
		uint32 InfoSize = MyInfos.Render(output_file);
	}catch (exception & Ex) {
		cout << Ex.what() << endl;
		return 1;
	}
	return 0;
};
int MatroskaWriter::SetupTracks(StdIOCallback & output_file)
{
	try {
		KaxTracks & MyTracks = GetChild<KaxTracks>(FileSegment);
		
		//Fill in track params
		KaxTrackEntry *last_track = NULL;
		unsigned short current_track = 0;	
		for (int current_track_loop = 0; current_track_loop < theTrackInfoCount; current_track_loop++)
		{
			trackData *current_track_data = theTrackInfo[current_track_loop];
			if(current_track_data != NULL)
			{				
				if (last_track != NULL)
				{
					MyTrack[current_track] = &GetNextChild<KaxTrackEntry>(MyTracks, *last_track);
				}else
				{
					MyTrack[current_track] = &GetChild<KaxTrackEntry>(MyTracks);
					last_track = MyTrack[current_track];
				}
				
				MyTrack[current_track]->SetGlobalTimecodeScale(TIMECODE_SCALE);
			
				KaxTrackNumber & MyTrack1Number = GetChild<KaxTrackNumber>(*MyTrack[current_track]);
				*(static_cast<EbmlUInteger *>(&MyTrack1Number)) = current_track + 1;			
				
				KaxTrackMinCache & MyTrackMinCache = GetChild<KaxTrackMinCache>(*MyTrack[current_track]);
				*(static_cast<EbmlUInteger *>(&MyTrackMinCache)) = 1;			

				KaxTrackMaxCache & MyTrackMaxCache = GetChild<KaxTrackMaxCache>(*MyTrack[current_track]);
				*(static_cast<EbmlUInteger *>(&MyTrackMaxCache)) = 1;			

				KaxTrackUID & MyKaxTrack1UID = GetChild<KaxTrackUID>(*MyTrack[current_track]);
				track_uid[current_track] = GenerateUINT32RandomNumber();
				*(static_cast<EbmlUInteger *>(&MyKaxTrack1UID)) = track_uid[current_track];
					
				*(static_cast<EbmlUInteger *>(&GetChild<KaxTrackType>(*MyTrack[current_track]))) = current_track_data->trackType;

				KaxCodecID & MyTrack1CodecID = GetChild<KaxCodecID>(*MyTrack[current_track]);
				*static_cast<EbmlString *>(&MyTrack1CodecID) = current_track_data->codecName;

				if (current_track_data->extraCodecData != NULL) {
					KaxCodecPrivate & MyCodecPrivate = GetChild<KaxCodecPrivate>(*MyTrack[current_track]);
					MyCodecPrivate.CopyBuffer((binary*)current_track_data->extraCodecData, current_track_data->extraCodecDataSize);
				}

				if ((current_track_data->trackType == track_video) && (current_track_data->video != NULL))
				{					
					KaxTrackDefaultDuration & MyKaxTrackDefaultDuration = GetChild<KaxTrackDefaultDuration >(*MyTrack[current_track]);
					*static_cast<EbmlUInteger *>(&MyKaxTrackDefaultDuration) = (1000 / current_track_data->video->frame_rate) * 1000000;

					KaxTrackVideo & MyTrack1Video = GetChild<KaxTrackVideo>(*MyTrack[current_track]);

					KaxVideoPixelWidth & MyTrack1VideoPixelWidth = GetChild<KaxVideoPixelWidth>(MyTrack1Video);
					*static_cast<EbmlUInteger *>(&MyTrack1VideoPixelWidth) = current_track_data->video->pixel_x;

					KaxVideoPixelHeight & MyTrack1VideoPixelHeight = GetChild<KaxVideoPixelHeight>(MyTrack1Video);
					*static_cast<EbmlUInteger *>(&MyTrack1VideoPixelHeight) = current_track_data->video->pixel_y;
					
					//We can disable Lacing on Video tracks
					MyTrack[current_track]->EnableLacing(false);
				}
				else if ((current_track_data->trackType == track_audio) && (current_track_data->audio != NULL))
				{					
					KaxTrackAudio & MyTrack2Audio = GetChild<KaxTrackAudio>(*MyTrack[current_track]);

					KaxAudioChannels & MyTrackAudioChannels = GetChild<KaxAudioChannels>(MyTrack2Audio);
					*(static_cast<EbmlUInteger *>(&MyTrackAudioChannels)) = current_track_data->audio->channels;

					KaxAudioSamplingFreq & MyTrack2Freq = GetChild<KaxAudioSamplingFreq>(MyTrack2Audio);
					*(static_cast<EbmlFloat *>(&MyTrack2Freq)) = (float)current_track_data->audio->sample_rate;

					MyTrack2Freq.ValidateSize();

					KaxAudioBitDepth & MyTrackAudioBitDepth = GetChild<KaxAudioBitDepth>(MyTrack2Audio);
					*(static_cast<EbmlUInteger *>(&MyTrackAudioBitDepth)) = current_track_data->audio->bit_depth;

					//We can enable Lacing on Audio tracks
					MyTrack[current_track]->EnableLacing(true);
				}
				current_track++;
			}
		}


		uint64 TrackSize = MyTracks.Render(output_file);
		MySeekHead.IndexThis(MyTracks, FileSegment);

	}catch (exception & Ex) {
		cout << Ex.what() << endl;        
		return 1;
	}
	return 0;
};

inline DataPacket *MatroskaWriter::ParseXMLDataPacket(DataPacketXML *xmlInputPacket)
{
	if (xmlInputPacket == NULL)
		return NULL;

	DataPacket *parsedPacket = new DataPacket;
	memset((void *)parsedPacket, 0, sizeof(DataPacket));
	parsedPacket->data = xmlInputPacket->data;

	TiXmlDocument xmlPacket("DataPacket");
	xmlPacket.Parse(xmlInputPacket->packetXml);
	TiXmlElement *root_element = xmlPacket.RootElement();
	if (!strcmp(root_element->Value(), "LemAPI"))
	{
		//Good this is our document
		TiXmlNode *level_1 = root_element->IterateChildren(NULL);
		while (level_1 != NULL)
		{
			if (!stricmp(level_1->Value(), "DataPacket"))
			{				
				TiXmlNode *level_2 = level_1->IterateChildren(NULL);
				while (level_2 != NULL)
				{
					if (!stricmp(level_2->Value(), "TrackNumber")) {
						TiXmlNode *level_2_data = level_2->FirstChild();
						if (level_2_data != NULL) {
							parsedPacket->TrackNumber = atoi(level_2_data->Value());
						}
					}else if (!stricmp(level_2->Value(), "StartTimecode")) {
						TiXmlNode *level_2_data = level_2->FirstChild();
						if (level_2_data != NULL) {
							parsedPacket->StartTimecode = atoi(level_2_data->Value());
						}
					}else if (!stricmp(level_2->Value(), "EndTimecode")) {
						TiXmlNode *level_2_data = level_2->FirstChild();
						if (level_2_data != NULL) {
							parsedPacket->EndTimecode = atoi(level_2_data->Value());
						}
					}else if (!stricmp(level_2->Value(), "DataLength")) {
						TiXmlNode *level_2_data = level_2->FirstChild();
						if (level_2_data != NULL) {
							parsedPacket->dataLength = atoi(level_2_data->Value());
						}
					}
					level_2 = level_1->IterateChildren(level_2);
				} //while (level_2 != NULL)
			}
			level_1 = root_element->IterateChildren(level_1);
		}
	}
	return parsedPacket;
};

bool MatroskaWriter::AddFrame(StdIOCallback & output_file, DataPacket *packet, bool bDeltaFrame) {
  try {
		KaxTrackEntry *TheTrack = MyTrack[packet->TrackNumber-1];
		uint8 track_number = TheTrack->TrackNumber();
		//If the current BlockGroup is Full and it exists ;) we set the blockduration
		if ((currentBlockGroupFull[track_number]) && (currentBlockGroup[track_number])) {
			//Set the final values for the block		
			/*if (currentClusterBlockTimecode == 0)
				currentClusterBlockTimecode = packet->StartTimecode;
			currentBlockGroup[track_number]->SetBlockDuration((packet->EndTimecode - currentClusterBlockTimecode)* TIMECODE_SCALE);
			currentClusterBlockTimecode =  packet->EndTimecode;*/
			//Set currentBlockGroup to NULL so the next if() { will get the next block to fill
			currentBlockGroup[track_number] = NULL;
		}

		//Find out if we are create a new cluster
		bool clusterChange = ChangeClusters(output_file, packet->StartTimecode);

		//If the current BlockGroup is NULL or we have a cluster change
		if ((!currentBlockGroup[track_number]) || clusterChange) {     
			//Create a new BlockGroup
			currentBlockGroup[track_number] = &clust->GetNewBlock();
			currentBlockGroup[track_number]->SetParentTrack(*TheTrack);
			//AllCues->AddBlockGroup(*currentBlockGroup);
    }            

		//Create a SimpleDataBuffer that has a pointer to our copy of/existing data in memory
		binary *cpy_buffer = new binary[packet->dataLength+1];
		memcpy(cpy_buffer, packet->data, packet->dataLength);
		data = new SimpleDataBuffer(cpy_buffer, packet->dataLength, 0);
		
		if (bDeltaFrame) {
			/*if ((clustLastPframe != NULL) && (clustLastPframe != clust) && (needPframeRef))
			{
				clustLastPframe->ReleaseFrames();
				delete clustLastPframe;
				clustLastPframe = NULL;
			}			
			clustLastPframe = clust;	*/		
			needPframeRef = true;
			
			//We have to create a new blockgroup for each P-Frames
			//if ((uint8)GetChild<KaxTrackType>(*TheTrack) == track_video) {
				currentBlockGroupFull[track_number] = currentBlockGroup[track_number]->AddFrame(*TheTrack, packet->StartTimecode * TIMECODE_SCALE, *data, *lastBlockGroup[track_number]);
				currentBlockGroupFull[track_number] = true;
			//} else {
				//currentBlockGroupFull[track_number] = !currentBlockGroup[track_number]->AddFrame(*TheTrack, packet->StartTimecode * TIMECODE_SCALE, *data, *lastBlockGroup[track_number]);
			//}
			//Store a pointer to this blockgroup as the last P-frame block
			lastBlockGroup[track_number] = currentBlockGroup[track_number];
		}else { //A keyframe or audio frame
			//Check if there is a last cluster AND that we are not chopping our feet off
			/*if (!clustLastIframe && (clustLastIframe != clust)
				//We also check if the previous cluster is needed for a previous P-frame
				&& !needPframeRef && (clustLastPframe != clustLastIframe))
			{
				clustLastIframe->ReleaseFrames();
				delete clustLastIframe;				
				clustLastIframe = NULL;
			}
			needPframeRef = false;
			clustLastIframe = clust;*/
			needPframeRef = false;
			
			//If Lacing is disabled we only put a single frame in each blockgroup
			if ((uint8)GetChild<KaxTrackType>(*TheTrack) == track_video) {
				currentBlockGroupFull[track_number] = currentBlockGroup[track_number]->AddFrame(*TheTrack, packet->StartTimecode * TIMECODE_SCALE, *data);
			} else {
				currentBlockGroupFull[track_number] = !currentBlockGroup[track_number]->AddFrame(*TheTrack, packet->StartTimecode * TIMECODE_SCALE, *data);
			}
			//Store a pointer to this blockgroup as the last keyframe block
			lastBlockGroup[track_number] = currentBlockGroup[track_number];
		}
  }catch (exception & Ex) {
    cout << Ex.what() << endl;
    return false;
  }
  return true;    
}

bool MatroskaWriter::ChangeClusters(StdIOCallback & output_file, uint64 timecode, bool bForce) {
  try {
		//If the cluster doesn't exist, we have passed the cluster time limit, or if we have passed the cluster size limit
		if (!clust || (bForce || ((timecode - currentClusterBaseTimecode) >= maxTimePerCluster) || (currentClusterSize >= maxClusterSize)) && !needPframeRef) {
			//Preform the cluster cleanup
      if (clust) {
				if (crcEnabled) {
					EbmlCrc32 & MyCrc32 = GetChild<EbmlCrc32>(FileSegment);
					MyCrc32.AddElementCRC32(*clust);
					MyCrc32.Render(output_file);
				}
        clust->Render(output_file, AllCues);				
        clust->ReleaseFrames();				
				
				for (int i = 0; i < 255; i++)
				{
					//delete currentBlockGroup[i];
					currentBlockGroup[i] = NULL;
					currentBlockGroupFull[i] = false;
				}
				//We will index the first cluster
				//if (currentClusterBaseTimecode == 0) {
					MySeekHead.IndexThis(*clust, FileSegment);
				//}
        //delete clust;
        currentClusterSize = 0;
      }
			++totalClusterCount;
			//Now we create a new cluster
      clust = new KaxCluster();
      clust->SetParent(FileSegment);
			clust->SetGlobalTimecodeScale(TIMECODE_SCALE);
      clust->SetPreviousTimecode(currentClusterBaseTimecode, TIMECODE_SCALE);
			clust->InitTimecode(timecode, TIMECODE_SCALE);

      //Set the timecode for this cluster?
			KaxClusterTimecode & MyClusterTimeCode = GetChild<KaxClusterTimecode>(*clust);
      *(static_cast<EbmlUInteger *>(&MyClusterTimeCode)) = timecode;
      
			//Store the starting timecode of this cluster
			currentClusterBaseTimecode = timecode;
      return true;
    }else {
      return false;
    }
  }catch (exception & Ex) {
    cout << Ex.what() << endl;
    return false;
  }   
}

void MatroskaWriter::CloseMatroskaFile(StdIOCallback & output_file) {	
	printf("Closing File... ");
	CloseClusters(output_file);
	WriteAttachments(output_file);
	WriteTags(output_file);
	
	//Replace the dummy element with the seek element
	Dummy.ReplaceWith(MySeekHead, output_file);

	//EbmlCrc32 &CRC_element = GetChild<EbmlCrc32>(FileSegment); 
	//CRC_element.AddElementCRC32(FileSegment);
	//Dummy_Crc.ReplaceWith(CRC_element, output_file);
	
	printf("Done\n");// CRC32: %X\n", CRC_element.GetCrc32());
}

int MatroskaWriter::CloseClusters(StdIOCallback & output_file)
{
	//KaxCuePoint &MyNewCuePoint = GetChild<KaxCuePoint>(*AllCues);
	//uint64 global_timecode = clust->GlobalTimecode();
	//uint64 global_timecodescale = clust->GlobalTimecodeScale();
	//MyNewCuePoint.Timecode(global_timecode, global_timecodescale);

	if (clust) {		
    clust->Render(output_file, AllCues);
    clust->ReleaseFrames();
    delete clust;		
  }
	//AllCues->Render(output_file);
	return 0;
};

int MatroskaWriter::WriteAttachments(StdIOCallback & output_file)
{
	if (attachment_count > 0)
	{	
		KaxAttachements & MyAttachements = GetChild<KaxAttachements>(FileSegment);
		
		KaxAttached *MyAttachedFile_last = NULL;
		for (int current_attachment = 0; current_attachment < attachment_count; current_attachment++)
		{
			KaxAttached *MyAttachedFile = NULL;
			if (MyAttachedFile_last != NULL) {
				MyAttachedFile = &GetNextChild<KaxAttached>(MyAttachements, *MyAttachedFile_last);
				MyAttachedFile_last = MyAttachedFile;
			}else {
				MyAttachedFile = &GetChild<KaxAttached>(MyAttachements);
				MyAttachedFile_last = MyAttachedFile;
			}

			KaxFileName & MyAttachedFile_Filename  = GetChild<KaxFileName>(*MyAttachedFile);
			*static_cast<EbmlUnicodeString *>(&MyAttachedFile_Filename) = ConvertCharToWchar_t(attachment_array[current_attachment]);

			KaxMimeType & MyAttachedFile_MimieType  = GetChild<KaxMimeType>(*MyAttachedFile);
			*static_cast<EbmlString *>(&MyAttachedFile_MimieType) = "";

			KaxFileData & MyAttachedFile_FileData  = GetChild<KaxFileData>(*MyAttachedFile);
			
			//Now that we have a buffer to put the data in, we open the file to attach
			FILE *file_to_attach = fopen(attachment_array[current_attachment], "rb");	
			//Find the total length of the file
			fseek(file_to_attach, 0, SEEK_END);
			int jpg_file_length = ftell(file_to_attach);
			rewind(file_to_attach);
			//Read in the file in one big chunk
			binary *the_buffer = new binary[jpg_file_length];
			fread(the_buffer, jpg_file_length, 1, file_to_attach);
			//Close the file handle
			fclose(file_to_attach);

			//Store the data in the element
			MyAttachedFile_FileData.SetBuffer(the_buffer, jpg_file_length);
			//Because we used SetBuffer instead of CopyBuffer we are not to free the memory used
			//CRC_element.FillCRC32(the_buffer, jpg_file_length);			
			//printf("\nAttached File: %s CRC32: %X\n", attachment_array[current_attachment], CRC_element.GetCrc32());
		}
		MyAttachements.Render(output_file);
		
		MySeekHead.IndexThis(MyAttachements, FileSegment);
	}	
	return 0;
};

int MatroskaWriter::WriteTags(StdIOCallback & output_file)
{
	KaxTags & MyKaxTags = GetChild<KaxTags>(FileSegment);
	KaxTag & MyKaxTag = GetChild<KaxTag>(MyKaxTags);

	//Fill Tag UID
	KaxTagTargets & MyKaxTagTargets = GetChild<KaxTagTargets>(MyKaxTag);
	KaxTagTrackUID & MyKaxTagTrackUID = GetChild<KaxTagTrackUID>(MyKaxTagTargets);
	*(static_cast<EbmlUInteger *>(&MyKaxTagTrackUID)) = track_uid[0];	

	KaxTagGeneral & MyKaxTagGeneral = GetChild<KaxTagGeneral>(MyKaxTag);
	KaxTagMultiComment *MyTagMultiComment_Last = NULL;	

	MyKaxTags.Render(output_file);

	MySeekHead.IndexThis(MyKaxTags, FileSegment);
	return 0;
};

uint32 MatroskaWriter::GenerateUINT32RandomNumber()
{
	int loop_total = 4294967294 / RAND_MAX;
	uint32 big_random_number = 0;

	srand((unsigned)clock());	
	for (int i = 0; i < loop_total; i++)
	{
		big_random_number += rand();
	}
	return big_random_number;
}

wchar_t *MatroskaWriter::ConvertCharToWchar_t(const char *ansi_filename)
{
	wchar_t *unicode_filename = new wchar_t[strlen(ansi_filename)+1];
	for (int c = 0; c < strlen(ansi_filename); c++)
	{
		mbtowc(unicode_filename+c, ansi_filename+c, 1);
	}
	unicode_filename[strlen(ansi_filename)] = 0;

	return unicode_filename;
}

//From VirtualDub's Sources, THE video editing program
static char base64[]=
	"ABCDEFGHIJKLMNOP"
	"QRSTUVWXYZabcdef"
	"ghijklmnopqrstuv"
	"wxyz0123456789+/"
	"=";

void MatroskaWriter::memunbase64(char *t, const char *s, long cnt) {

	char c1, c2, c3, c4, *ind, *limit = t+cnt;
	long v;

	for(;;) {
		while((c1=*s++) && !(ind = strchr(base64,c1)));
		if (!c1) break;
		c1 = (char)(ind-base64);

		while((c2=*s++) && !(ind = strchr(base64,c2)));
		if (!c2) break;
		c2 = (char)(ind-base64);

		while((c3=*s++) && !(ind = strchr(base64,c3)));
		if (!c3) break;
		c3 = (char)(ind-base64);

		while((c4=*s++) && !(ind = strchr(base64,c4)));
		if (!c4) break;
		c4 = (char)(ind-base64);

		// [c1,c4] valid -> 24 bits (3 bytes)
		// [c1,c3] valid -> 18 bits (2 bytes)
		// [c1,c2] valid -> 12 bits (1 byte)
		// [c1] valid    ->  6 bits (0 bytes)

		v = ((c1 & 63)<<18) | ((c2 & 63)<<12) | ((c3 & 63)<<6) | (c4 & 63);

		if (c1!=64 && c2!=64) {
			*t++ = (char)(v >> 16);

			if (t >= limit) return;

			if (c3!=64) {
				*t++ = (char)(v >> 8);
				if (t >= limit) return;
				if (c4!=64) {
					*t++ = (char)v;
					if (t >= limit) return;
					continue;
				}
			}
		}
		break;
	}
}

void MatroskaWriter::membase64(char *t, const char *s, long l) {

	unsigned char c1, c2, c3;

	while(l>0) {
		c1 = (unsigned char)s[0];
		if (l>1) {
			c2 = (unsigned char)s[1];
			if (l>2)
				c3 = (unsigned char)s[2];
		}

		t[0] = base64[(c1 >> 2) & 0x3f];
		if (l<2) {
			t[1] = base64[(c1<<4)&0x3f];
			t[2] = t[3] = '=';
		} else {
			t[1] = base64[((c1<<4)|(c2>>4))&0x3f];
			if (l<3) {
				t[2] = base64[(c2<<2)&0x3f];
				t[3] = '=';
			} else {
				t[2] = base64[((c2<<2) | (c3>>6))&0x3f];
				t[3] = base64[c3 & 0x3f];
			}
		}

		l-=3;
		t+=4;
		s+=3;
	}
	*t=0;
}

void *MatroskaWriter::Entry()
{
	Convert2Matroska();
	return 0;
};
void MatroskaWriter::OnExit()
{
	theThreads.Remove(this);
};
