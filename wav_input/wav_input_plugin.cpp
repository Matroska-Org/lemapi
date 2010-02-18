/*
    Copyright (C) 2003, Jory Stone <jcsston @ toughguy.net>

    wav2mka is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    wav2mka is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/**
    \file wav_input_plugin.cpp
    Reader Plugin for WAV file support
*/

#include <windows.h>
#include <string.h>

#include "wav2mka.h"

//Include tinyXML
#include "tinyxml.h"

using namespace LIBEBML_NAMESPACE;
using namespace std;

//From VirtualDub's Sources, THE video editing program
static char base64[]=
	"ABCDEFGHIJKLMNOP"
	"QRSTUVWXYZabcdef"
	"ghijklmnopqrstuv"
	"wxyz0123456789+/"
	"=";

void memunbase64(char *t, const char *s, long cnt) {

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

void membase64(char *t, const char *s, long l) {

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

class wav_input_plugin_data {
public:
	wav_input_plugin_data()
	{
		memset(this, 0, sizeof(wav_input_plugin_data));
	};

	WAV_file *input_File;
	FMT_chunk *fmtChunk;
	DATA_chunk *dataChunk;
	BEXT_chunk *bextChunk;
	LIST_chunk *listChunk;
	LIST_Info *listInfo;
	uint64 TotalDuration;
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

struct DataPacketXML {
	char *packetXml;
	binary *data;
};

extern "C" char *GetLemAPIVersion(int writer_version)
{
	std::string versionXml;
	
	versionXml = "<?xml>";
	versionXml += "<LemAPI>";
	versionXml += "<Plugin>";
	versionXml += "<Name>Wav Plugin</Name>";
	versionXml += "<Author>Jory Stone using Steve Lhomme's wav Classes</Author>";
	versionXml += "<URL>http://www.somewhere.com/</URL>";
	versionXml += "<Version>1.00</Version>";	
	versionXml += "<SupportedFileTypes>";
	versionXml += "<FileType>";
	versionXml += "<Name>Window Audio Waveform Files</Name>";
	versionXml += "<Ext>wav</Ext>";
	versionXml += "</FileType>";
	versionXml += "</SupportedFileTypes>";
	//versionXml += "<Option name=\"Copy Info Frames\" type=\"Boolean\">True</Option>";
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
	wav_input_plugin_data *new_process = new wav_input_plugin_data();	

	return (void *)new_process;
};

extern "C" int ReadFileHeaders(void *using_process, const char *input_Filename)
{	
	try {		
		wav_input_plugin_data *currentProcess = (wav_input_plugin_data *)using_process;
    // open the input file		
		currentProcess->input_File = new WAV_file(std::string(input_Filename));
		WAV_file *input_File = currentProcess->input_File;

		if (!currentProcess->input_File->Open(RIFF_file::RIFF_READ)) {
			//printf("Impossible to read the file '%s'\n", input_Filename);
			return -1;
		}
		input_File->ValidateHeader();

    while (!currentProcess->dataChunk && input_File->FindNextChunk()) {
			switch(input_File->ChunkType()) {
				case WAV_file::DATA_CHUNK:
					currentProcess->dataChunk = new DATA_chunk(input_File->AcquireChunk());
					input_File->SetDataChunk(currentProcess->dataChunk);
					break;
				case WAV_file::FORMAT_CHUNK:
					currentProcess->fmtChunk = new FMT_chunk(input_File->AcquireChunk());
					input_File->SetFormatChunk(currentProcess->fmtChunk);
					currentProcess->fmtChunk->Read(input_File->IOfile());
					break;
				case WAV_file::BROADCAST_CHUNK:
					currentProcess->bextChunk = new BEXT_chunk(input_File->AcquireChunk());
					currentProcess->bextChunk->Read(input_File->IOfile());
					break;
				case WAV_file::LIST_CHUNK:
					currentProcess->listChunk = new LIST_chunk(input_File->AcquireChunk());
					switch(currentProcess->listChunk->FindType(input_File->IOfile())) {
						case LIST_chunk::LIST_INFO:
							currentProcess->listInfo = new LIST_Info;
							if (currentProcess->listInfo == NULL) {
								//printf("No more space to save LIST INFO\n");
								currentProcess->listChunk->SkipData(input_File->IOfile());
							}
							else {
								if (!currentProcess->listInfo->Read(input_File->IOfile())) {
									currentProcess->listChunk->SkipData(input_File->IOfile());
									delete currentProcess->listInfo;
									currentProcess->listInfo = NULL;
								}
							}
							break;
						default:
							currentProcess->listChunk->SkipData(input_File->IOfile());
							break;
					}
					delete currentProcess->listChunk;
					currentProcess->listChunk = NULL;
					break;
				default:
					// unknown chunk, skip data
					RIFF_chunk *Unknown = input_File->AcquireChunk();
					Unknown->SkipData(input_File->IOfile());
					break;
			}
		}

		if (!currentProcess->dataChunk) {
			//printf("Impossible to find the data part in '%s'\n", input_Filename.c_str());
			return -1;
		}
		if (!currentProcess->fmtChunk) {
			//printf("Impossible to find the format part in '%s'\n", input_Filename.c_str());
			return -1;
		}
		
		//Fill in the length of the wav file
		currentProcess->TotalDuration = currentProcess->input_File->myFileSize / currentProcess->fmtChunk->GetWAVEFORMATEX()->nAvgBytesPerSec * 1000;
		
		//Exit the function
		return 0;
	}catch (exception & Ex){        
        return -1;
  }
};

extern "C" DataPacket *GetNextDataFrame(void *process_to_read)
{
	try {
		wav_input_plugin_data *currentProcess = (wav_input_plugin_data *)process_to_read;
		
		//Read a WAV data packet
		const uint32 maxRead = 5000;
		binary* DataBuffer = new binary[maxRead];
		currentProcess->start_timecode = currentProcess->input_File->GetCurrentTimecode();
		uint32 readSize = currentProcess->input_File->ReadData(DataBuffer, maxRead);
		if (readSize != 0) {			
			currentProcess->end_timecode = currentProcess->input_File->GetCurrentTimecode();
			
			DataPacket *new_packet = new DataPacket;
			new_packet->TrackNumber = 1;
			new_packet->StartTimecode = currentProcess->start_timecode;
			new_packet->EndTimecode = currentProcess->end_timecode;
			new_packet->dataLength = readSize;
			new_packet->data = DataBuffer;

			return new_packet;
		} else {
			delete DataBuffer;
			return NULL;
		}				

		return NULL;
	}catch (exception & Ex){        
		return NULL;
	}
};

//This didn't work out, WAY too much overhead in processing time
extern "C" DataPacketXML *GetNextDataFrameXML(void *process_to_read)
{
	try {
		wav_input_plugin_data *currentProcess = (wav_input_plugin_data *)process_to_read;
		
		//Read a WAV data packet
		const uint32 maxRead = 5000;
		binary* DataBuffer = new binary[maxRead];
		currentProcess->start_timecode = currentProcess->input_File->GetCurrentTimecode();
		uint32 readSize = currentProcess->input_File->ReadData(DataBuffer, maxRead);
		if (readSize != 0) {			
			currentProcess->end_timecode = currentProcess->input_File->GetCurrentTimecode();
			
			DataPacketXML *new_packet = new DataPacketXML;
			std::string packetXml;
			char *buffer = new char[128];
			packetXml = "<?xml version=\"1.0\" ?>";
			packetXml += "<LemAPI>";
			packetXml += "<DataPacket>";
			packetXml += "<TrackNumber>";
			sprintf(buffer, "%i", 1);
			packetXml += buffer;
			packetXml += "</TrackNumber>";
			packetXml += "<StartTimecode>";
			sprintf(buffer, "%i", currentProcess->start_timecode);
			packetXml += buffer;
			packetXml += "</StartTimecode>"; //< In ms
			packetXml += "<EndTimecode>";
			sprintf(buffer, "%i", currentProcess->end_timecode);
			packetXml += buffer;
			packetXml += "</EndTimecode>"; //< In ms
			packetXml += "<DataLength>"; //< Length of binary *data
			sprintf(buffer, "%i", readSize);
			packetXml += buffer;
			packetXml += "</DataLength>";
			packetXml += "</DataPacket>";
			packetXml += "</LemAPI>";
			delete buffer;

			new_packet->packetXml = new char[packetXml.length()+1];
			strcpy(new_packet->packetXml, packetXml.c_str());
			new_packet->packetXml[packetXml.length()] = 0;
			
			new_packet->data = DataBuffer;
			
			return new_packet;
		} else {
			delete DataBuffer;
			return NULL;
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
	wav_input_plugin_data *currentProcess = (wav_input_plugin_data *)process_to_read;

	char *txt_buffer = new char [1024];
	std::string trackXml;
	trackXml = "<?xml> ";
	trackXml += "<LemAPI>";
	trackXml += "<TrackList>";
	trackXml += "<Track>";
	trackXml += "<TrackType>audio</TrackType>";

	WAVEFORMATEX_ *acm_wav = currentProcess->fmtChunk->GetWAVEFORMATEX();
	
	trackXml += "<CodecID>";
	if ((acm_wav->wFormatTag[0] == 1) && (acm_wav->wFormatTag[1] == 0)) {//PCM
			trackXml += "A_PCM/INT/LIT";
			trackXml += "</CodecID>";
	}else {
			trackXml += "A_MS/ACM";
			trackXml += "</CodecID>";
			trackXml += "<CodecPrivate>";
			if (acm_wav->cbSize > 1)
				acm_wav->cbSize = acm_wav->cbSize - 2;
			int dataSize = sizeof(WAVEFORMATEX_) + acm_wav->cbSize;
			binary *cpriv = (binary *)malloc(dataSize);
			memcpy(cpriv, acm_wav, dataSize);
			char *buffer = new char[(4*dataSize/3)];
			membase64(buffer, (char *)cpriv, dataSize);
			trackXml += buffer;
			trackXml += "</CodecPrivate>";
	}	

	trackXml += "<Duration>";
	sprintf(txt_buffer, "%u", currentProcess->TotalDuration);
	trackXml += txt_buffer;
	trackXml += "</Duration>";
	trackXml += "<Audio>";
	trackXml += "<SampleRate>";
	sprintf(txt_buffer, "%u", currentProcess->fmtChunk->SampleRate());
	trackXml += txt_buffer;
	trackXml += "</SampleRate>";
	trackXml += "<Channels>";
	sprintf(txt_buffer, "%u", currentProcess->fmtChunk->ChannelPerSubtrack());
	trackXml += txt_buffer;
	trackXml += "</Channels>";
	trackXml += "<BitDepth>";
	sprintf(txt_buffer, "%u", currentProcess->fmtChunk->BitDepth());
	trackXml += txt_buffer;
	trackXml += "</BitDepth>";
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
	wav_input_plugin_data *goodbye_process = (wav_input_plugin_data *)process_to_close;

	delete goodbye_process->input_File;

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
