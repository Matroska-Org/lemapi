/*
 *  matroska_writer, Writes Matroska files
 *
 *  matroska_writer.h
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
	\file matroska_writer.h
	\version \$Id: matroska_writer.h,v 1.1 2003/07/21 15:10:51 jcsston Exp $
	\brief Writes Matroska files
	\author Jory Stone            <jcsston @ toughguy.net>
	
	\history
		$Log: matroska_writer.h,v $
		Revision 1.1  2003/07/21 15:10:51  jcsston
		First Commit
		
		
*/

// Standard C/C++ Includes
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <iostream>


// wxWindows Includes
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
#include <wx/dynlib.h>
#include <wx/dir.h>
#include <wx/thread.h>
#include <wx/splitter.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/listbox.h>

// libebml includes
#include "StdIOCallback.h"
#include "EbmlHead.h"
#include "EbmlSubHead.h"
#include "EbmlVoid.h"
#include "EbmlCRC32.h"
#include "EbmlVersion.h"
// libmatroska includes
#include "FileKax.h"
#include "KaxSegment.h"
#include "KaxTracks.h"
#include "KaxTrackEntryData.h"
#include "KaxTrackAudio.h"
#include "KaxTrackVideo.h"
#include "KaxCluster.h"
#include "KaxClusterData.h"
#include "KaxBlock.h"
#include "KaxBlockData.h"
#include "KaxCues.h"
#include "KaxCuesData.h"
#include "KaxSeekHead.h"
#include "KaxInfo.h"
#include "KaxInfoData.h"
#include "KaxTags.h"
#include "KaxTag.h"
#include "KaxTagMulti.h"
#include "KaxVersion.h"
#include "KaxAttachements.h"
#include "KaxAttached.h"

// tinyXML inxlude
#include "tinyxml.h"

using namespace LIBMATROSKA_NAMESPACE;
using namespace LIBEBML_NAMESPACE;
using namespace std;

#define printf OutputToBuffer

struct DataPacket {
	uint8 TrackNumber;
	uint32 StartTimecode;
	uint32 EndTimecode;
	uint32 dataLength;
	binary *data;
  uint32 prevReference;
  uint32 nextReference;
};

struct DataPacketXML {
	char *packetXml;
	binary *data;
};

typedef char* (*GetLemAPIVersion)(int);
typedef void* (*CreateLemAPIProcess)(char *);
typedef int (*ReadFileHeaders)(void *, const char *);
typedef char* (*GetTrackInfoXML_ASCII)(void *);
typedef DataPacket* (*GetNextDataFrame)(void *);
typedef DataPacketXML* (*GetNextDataFrameXML)(void *);
typedef int (*FreeDataFrame)(DataPacket *);
typedef int (*DeleteLemAPIProcess)(void *);

class LemAPIPlugin {
public:
	LemAPIPlugin() {
		wxFAIL_MSG(_T("Trying to load a plugin without a filename :P"));
	};
	LemAPIPlugin(wxString &pluginFilename);
	~LemAPIPlugin() {
		pluginDll.Unload();
	};
	bool IsPluginOk() {return pluginOk;};

	bool pluginOk;
	wxDynamicLibrary pluginDll;
	wxString name;
	wxString version;
	wxString author;
	wxString website;
	wxArrayString supportedFileTypes;
};

//WX_DEFINE_ARRAY(LemAPIPlugin *, ArrayOfPlugins);

// Struct's used for the internal MatroskaWriter track list
struct audioData {
		uint8 channels;
		uint16 bit_depth;
		float sample_rate;
		uint32 avg_bitrate;
};

struct videoData {
		uint32 pixel_x;
		uint32 pixel_y;
		uint32 display_x;
		uint32 display_y;
		float frame_rate;
		uint16 color_depth;
		uint32 cache_min; //< Used for B-Frames?
		uint32 cache_max;
};
struct trackData {
	uint8 trackType;
	char *codecName;
	binary *extraCodecData;
	uint32 extraCodecDataSize;
	uint64 duration;
	uint16 defaultDuration;
	videoData *video;
	audioData *audio;
};

WX_DEFINE_ARRAY(wxThread *, wxArrayThread);

class MatroskaWriter : public wxThread
{
public:
	void *Entry();
	void OnExit();
	MatroskaWriter(wxFrame *frame, wxTextCtrl *outputBox, wxString passed_InputFile, wxString passed_OutputFile = "");
	~MatroskaWriter();
	int Convert2Matroska();
	/// Set the input mpeg audio file
	bool SetInputFile(wxString &passed_InputFile);
	wxString &GetInputFilename() {return input_Filename;};
	
	/// Sets the output Matroska file
	bool SetOutputFile(wxString &passed_OutputFile);
	wxString &GetOutputFilename() {return output_Filename;};
	uint8 GetPercentDone() {return percentDone;};
	/// Prepare to be blasted with numbers, Default False
	bool SetVerboseModeOn(bool bVerboseOn = false);
	/// Turns off Lacing, which increases file sizes
	/// \param bLacingOff Default value is False
	bool SetLacingOff(bool bLacingOff = false);
	/// Sets the largest cluster size in bytes
	/// \param passed_maxClusterSize Range is 128bytes to 1GB
	bool SetMaxClusterByteSize(uint64 passed_maxClusterSize = 524288);
	/// Sets the largest cluster size in ms
	/// \param passed_maxTimePerCluster Range is 10ms to 1hour
	bool SetMaxClusterTimeSize(uint32 passed_maxTimePerCluster = 5000);
	/// Adds a filename to the list of files to attach
	bool AddAttachmentFilename(char *filename);

	uint32 GenerateUINT32RandomNumber();
	wchar_t *ConvertCharToWchar_t(const char *ansi_filename);
	void memunbase64(char *t, const char *s, long cnt);
	void membase64(char *t, const char *s, long l);
protected:
	//Define for Percent updater
	void UpdatePercentBar(unsigned int newPercent);
	void OutputToBuffer(char *formating_string, ...);
	
	DataPacket *ParseXMLDataPacket(DataPacketXML *xmlInputPacket);
	bool AddFrame(StdIOCallback &output_file, DataPacket *packet, bool bDeltaFrame = false);
	bool ChangeClusters(StdIOCallback &output_file, uint64 timecode, bool bForce = false);
	
	int InitMatroskaOutputFile(StdIOCallback &output_file);
	int ParseTrackXML(const char *inputXML);
	int SetupInfo(StdIOCallback &output_file);
	int SetupTracks(StdIOCallback &output_file);
	
	void CloseMatroskaFile(StdIOCallback &output_file);
	int CloseClusters(StdIOCallback &output_file);
	int WriteAttachments(StdIOCallback &output_file);
	int WriteTags(StdIOCallback &output_file);

	wxString input_Filename;
	wxString output_Filename;
	char *attachment_array[255];
	unsigned char attachment_count;

	KaxCluster *clust;
	bool needPframeRef;
	KaxCues AllCues;
	KaxTrackEntry *MyTrack[255];
	KaxBlockGroup *currentBlockGroup[255];
	KaxBlockGroup *lastBlockGroup[255];
	KaxSegment FileSegment;
	EbmlVoid Dummy;
	EbmlVoid Dummy_Crc;
	KaxSeekHead MySeekHead;

	DataBuffer* data;
	bool currentBlockGroupFull[255];
	bool noLacing;
	bool bVerbose;
	bool crcEnabled;
	uint32 maxTimePerCluster; //Default Value is 10 seconds in ms
	uint64 maxClusterSize; //Default value is 512kB in bytes
	uint64 currentClusterSize;
	uint64 currentClusterBaseTimecode;
	uint64 currentClusterBlockTimecode;
	uint64 TIMECODE_SCALE;
	uint32 track_uid[255];
	
	uint8 theTrackInfoCount;
	trackData *theTrackInfo[255];
	uint32 totalFrameCount;
	uint32 totalClusterCount;
	//wxWindows GUI controls
	uint8 percentDone;
	uint8 lastPercentDone;
	wxTextCtrl *console_output_box;
	wxFrame *parentFrame;
};

class MatroskaReader : public wxThread
{
	CreateLemAPIProcess pCreatePluginProcess;
	ReadFileHeaders pReadHeaders;
	GetTrackInfoXML_ASCII pGetTrackInfoXML_ASCII;
	GetNextDataFrame pGetNextDataFrame;
	FreeDataFrame pFreeDataFrame;
	DeleteLemAPIProcess pDeleteLemAPIProcess;
};