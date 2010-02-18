/*
    Copyright (C) 2003, Jory Stone <jcsston @ toughguy.net>

    ac32mka is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ac32mka is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/**
    \file ac3_input_plugin.cpp
    Reader Plugin for MPEG I/II Layer 1/2/3
*/

#include <windows.h>
#include <string.h>

//Include Steve's AC3 Classes
#include "ac3file.h"
#include "ac3frame.h"
#include "ac3iframe.h"

//Include tinyXML
#include "tinyxml.h"

using namespace M2W_NAMESPACE;
using namespace std;

class ac3_input_plugin_data {
public:
	ac3_input_plugin_data()
	{
		memset(this, 0, sizeof(ac3_input_plugin_data));
	};

	AC3_File *input_File;
  uint32 FrameNumber;
  uint64 SampleNumbers;
  uint64 TotalFrameSize;
  uint16 SamplingRate;
  uint16 Channels;
  uint16 SampleSize;
	uint64 totalByteSize;
	uint32 AverageBitrate;
	uint32 AverageByterate;
	uint64 TotalDuration;
	uint32 prevSamplesNumber;
	uint32 start_timecode;
	uint32 end_timecode;
};

struct DataPacket {
	uint8 TrackNumber;
	uint32 StartTimecode;
	uint32 EndTimecode;
	uint32 dataLength;
	binary *data;
};

extern "C" char *GetLemAPIVersion(int writer_version)
{
	std::string versionXml;
	
	versionXml = "<?xml version=\"1.0\" ?>";
	versionXml += "<LemAPI>";
	versionXml += "<Plugin>";
	versionXml += "<Name>AC3 Plugin</Name>";
	versionXml += "<Author>Jory Stone using Steve Lhomme's AC3 Classes</Author>";
	versionXml += "<URL>http://www.somewhere.com/</URL>";
	versionXml += "<Version>1.00</Version>";	
	versionXml += "<SupportedFileTypes>";
	versionXml += "<FileType>";
	versionXml += "<Name>AC3</Name>";
	versionXml += "<Ext>ac3</Ext>";
	versionXml += "</FileType>";
	versionXml += "</SupportedFileTypes>";
	versionXml += "<Option name=\"Copy Info Frames\" type=\"Boolean\">True</Option>";
	versionXml += "</Plugin>";
	versionXml += "</LemAPI>";

	//Copy the version XML data to a char *
	char *versionXml_char = new char[versionXml.length()];
	strcpy(versionXml_char, versionXml.c_str());
	versionXml_char[versionXml.length()] = 0;
	return versionXml_char;
};

extern "C" void *CreateLemAPIProcess(char *plugin_settings)
{
	ac3_input_plugin_data *new_process = new ac3_input_plugin_data();	

	return (void *)new_process;
};

extern "C" int ReadFileHeaders(void *using_process, const char *input_Filename)
{	
	try {		
		ac3_input_plugin_data *currentProcess = (ac3_input_plugin_data *)using_process;
    // open the input file		
		currentProcess->input_File = new AC3_File(std::string(input_Filename), mode_read);

    currentProcess->input_File->SkipToFrameStart();

    MPA_Lame_Frame infoFrame;
    bool bInfoFrameOn = false;
    if (currentProcess->input_File->CheckInfoFrame()) {
		  *static_cast<AC3_Frame *>(&infoFrame) = currentProcess->input_File->GetInfoFrame();
		  bInfoFrameOn = true;
    }

    AC3_Frame frame;
    
    while (currentProcess->input_File->FindNextSyncWord()) {
		  currentProcess->FrameNumber++;
		  frame = currentProcess->input_File->GetFrame();
		  /// \todo handle the bitrate of the frame in a key/value table, so that we can detect real CBR files
		  currentProcess->TotalFrameSize += frame.DataSize() + 4;
		  currentProcess->SampleNumbers += frame.SampleSize();
		  currentProcess->SamplingRate = frame.SamplingRate();
		  currentProcess->Channels = frame.Channels();
		  currentProcess->SampleSize = frame.SampleSize();
		  //currentProcess->Layer = frame.Layer();
		  //Skip to the data, this is the first pass
			currentProcess->input_File->SkipData();
    }


    uint64 totalByteSize = currentProcess->TotalFrameSize * currentProcess->SamplingRate;
    uint32 AverageBitrate, AverageByterate;
    if ((8 * totalByteSize) % currentProcess->SampleNumbers > currentProcess->SampleNumbers>>1)
	    AverageBitrate = 1 + 8 * totalByteSize / currentProcess->SampleNumbers;
    else
	    AverageBitrate = 8 * totalByteSize / currentProcess->SampleNumbers;

    if (AverageBitrate % 8 > 4)
	    AverageByterate = 1 + (AverageBitrate >> 3);
    else
	    AverageByterate = AverageBitrate >> 3;

		//Copy the calculated data over to the process struct
		currentProcess->totalByteSize = totalByteSize;
		currentProcess->AverageBitrate = AverageBitrate;
		currentProcess->AverageByterate = AverageByterate;

		currentProcess->TotalDuration = (((int64)currentProcess->SampleNumbers * 1000) / currentProcess->SamplingRate);

    // prepare to read for output
    currentProcess->input_File->SkipToFrameStart();

    /// \todo add the option to skip the Info frame or not
	  if (bInfoFrameOn) {
		  currentProcess->input_File->FindNextSyncWord();
		  currentProcess->input_File->GetFrame();
		  currentProcess->input_File->SkipData();
    }

		//Reset the number of read samples
    currentProcess->SampleNumbers = 0;

		//Exit the function
		return 0;
	}catch (exception & Ex){        
        return -1;
  }
};

extern "C" DataPacket *GetNextDataFrame(void *process_to_read)
{
	try {
		ac3_input_plugin_data *currentProcess = (ac3_input_plugin_data *)process_to_read;
		if (currentProcess->input_File->FindNextSyncWord())
		{
			binary *frame_buffer = new binary[50000]; // should be enough for 32kHz/320kbps
			AC3_Frame frame = currentProcess->input_File->GetFrame();
			currentProcess->SampleNumbers += frame.SampleSize();
			memcpy(frame_buffer, (const binary *)(frame), 4);
			currentProcess->input_File->GetData(frame_buffer+4, 50000);			

			currentProcess->start_timecode = (((uint64)currentProcess->prevSamplesNumber * 1000) / currentProcess->SamplingRate);
			currentProcess->end_timecode = currentProcess->start_timecode + (((uint64)frame.SampleSize() * 1000) / currentProcess->SamplingRate);
			//AddFrame(output_file, MyTrack1, frame_buffer, frame.DataSize()+4, start_timecode, end_timecode);
			currentProcess->prevSamplesNumber = currentProcess->SampleNumbers;

			DataPacket *new_packet = new DataPacket;
			new_packet->TrackNumber = 1;
			new_packet->StartTimecode = currentProcess->start_timecode;
			new_packet->EndTimecode = currentProcess->end_timecode;
			new_packet->dataLength = frame.DataSize()+4;
			new_packet->data = frame_buffer;

			return new_packet;
		}	

		return NULL;
	}catch (exception & Ex){        
		return NULL;
	}
};

extern "C" int FreeDataFrame(DataPacket *packet_to_free)
{
	delete packet_to_free->data;
	packet_to_free->data = NULL;
	delete packet_to_free;

	return 0;
};

extern "C" char* GetTrackInfoXML_ASCII(void *process_to_read)
{
	ac3_input_plugin_data *currentProcess = (ac3_input_plugin_data *)process_to_read;

	char *txt_buffer = new char [1024];
	std::string trackXml;
	trackXml = "<?xml version=\"1.0\"?>";
	trackXml += "<LemAPI>";
	trackXml += "<TrackList>";
	trackXml += "<Track>";
	trackXml += "<TrackType>audio</TrackType>";
	trackXml += "<CodecID>";
	trackXml += "A_AC3";
	trackXml += "</CodecID>";
	trackXml += "<Duration>";
	sprintf(txt_buffer, "%u", currentProcess->TotalDuration);
	trackXml += txt_buffer;
	trackXml += "</Duration>";
	trackXml += "<Audio>";
	trackXml += "<SampleRate>";
	sprintf(txt_buffer, "%u", currentProcess->SamplingRate);
	trackXml += txt_buffer;
	trackXml += "</SampleRate>";
	trackXml += "<Channels>";
	sprintf(txt_buffer, "%u", currentProcess->Channels);
	trackXml += txt_buffer;
	trackXml += "</Channels>";
	trackXml += "</Audio>";
	trackXml += "</Track>";
	trackXml += "</TrackList>";
	trackXml += "</LemAPI>";
	delete txt_buffer;
	txt_buffer = NULL;

	//Copy the track XML data to a char *
	char *trackXml_char = new char[trackXml.length()];
	strcpy(trackXml_char, trackXml.c_str());
	trackXml_char[trackXml.length()] = 0;
	//Return
	return trackXml_char;
};

extern "C" int DeleteLemAPIProcess(void *process_to_close)
{
	ac3_input_plugin_data *goodbye_process = (ac3_input_plugin_data *)process_to_close;

	goodbye_process->input_File->close();

	delete goodbye_process;

	return 0;
};

BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
        break;

      case DLL_PROCESS_DETACH:
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}
