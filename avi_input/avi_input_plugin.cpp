/*
    Copyright (C) 2003, Jory Stone <jcsston @ toughguy.net>

    avi_input_plugin) is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    avi_input_plugin is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with avi_input_plugin; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/**
    \file avi_input_plugin.cpp
    \brief Reader Plugin for AVI
    \author Jory Stone <jcsston @ toughguy.net>
*/

#include <windows.h>
#include <string>

//Include avilib
#ifdef _MSC_VER
extern "C" {
#endif
#include "avilib.h"
#ifdef _MSC_VER
};
#endif

#define binary uint8
#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define uint64 uint64_t

//Include tinyXML
#include "tinyxml.h"

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

class avi_input_plugin_data {
public:
	avi_input_plugin_data()
	{
		memset(this, 0, sizeof(avi_input_plugin_data));
	};

	avi_t *input_File;
  long currentFrame;	
	uint32 start_timecode;
	uint32 end_timecode;
	char readAudioBlock;
	int audio_interleave;
	long audio_current_sample;
	uint32 audio_start_timecode;
	uint32 audio_end_timecode;
	char *trackXmlInfo;
};

struct DataPacket {
	uint8 TrackNumber;
	uint32 StartTimecode;
	uint32 EndTimecode;
	uint32 dataLength;
	binary *data;
  uint32 prevReference;
  uint32 nextReference;
};

extern "C" char *GetLemAPIVersion(int writer_version)
{
	return "<?xml version=\"1.0\" ?>"
	"<LemAPI version=\"2\">"
	"<Plugin>"
	"<Name>AVI Plugin</Name>"
	"<Author>Jory Stone using avilib</Author>"
	"<URL>http://www.somewhere.com/</URL>"
	"<Version>1.00</Version>"
	"<SupportedFileTypes>"
	"<FileType>"
	"<Name>Audio Video Interleave</Name>"
	"<Ext>avi</Ext>"
	"</FileType>"
	"</SupportedFileTypes>"
	"</Plugin>"
	"</LemAPI>";
};

extern "C" void *CreateLemAPIProcess(char *plugin_settings)
{
	avi_input_plugin_data *new_process = new avi_input_plugin_data();	

	return (void *)new_process;
};

extern "C" int ReadFileHeaders(void *using_process, const char *input_Filename)
{	
	avi_input_plugin_data *currentProcess = (avi_input_plugin_data *)using_process;
  // open the input file, generate a Index
	currentProcess->input_File = AVI_open_input_file(input_Filename, 1);
	
	if (AVI_audio_chunks(currentProcess->input_File) == 0) {
		//No audio
		currentProcess->readAudioBlock = -1;
	}else {
		currentProcess->audio_interleave = AVI_video_frames(currentProcess->input_File) / AVI_audio_chunks(currentProcess->input_File);
	}
	
	//Exit the function
	return 0;
};

extern "C" DataPacket *GetNextDataFrame(void *process_to_read)
{
	avi_input_plugin_data *currentProcess = (avi_input_plugin_data *)process_to_read;	
	
	if (currentProcess->readAudioBlock == currentProcess->audio_interleave) {
		currentProcess->readAudioBlock = 0;
		int size = 0;
		//if (AVI_audio_format(currentProcess->input_File) == 0x0001)
		//	size = AVI_audio_channels(currentProcess->input_File) * AVI_audio_bits(currentProcess->input_File) * AVI_audio_rate(currentProcess->input_File) / 8;
		//else
			size = AVI_audio_size(currentProcess->input_File, currentProcess->audio_current_sample);//16384;
		
		char *audio_buffer = new char[size];
		//AVI_read_audio_chunk
		long audioBytesRead = AVI_read_audio_chunk(currentProcess->input_File, audio_buffer);
		if (audioBytesRead > 0) {
			if (audioBytesRead < size) {
				//We've reached the end of the stream
				currentProcess->readAudioBlock = -1;
			}
			
			currentProcess->audio_start_timecode = currentProcess->audio_end_timecode;//size * 8 / AVI_audio_mp3rate(currentProcess->input_File);//((currentProcess->audio_current_sample * size * 250) / AVI_audio_rate(currentProcess->input_File));
			//currentProcess->audio_start_timecode = ((currentProcess->audio_current_sample * size * 250) / AVI_audio_rate(currentProcess->input_File));
			currentProcess->audio_end_timecode += size * 8 / AVI_audio_mp3rate(currentProcess->input_File);

			currentProcess->audio_current_sample++;
			
			DataPacket *new_packet = new DataPacket;
			new_packet->TrackNumber = 2;
			new_packet->StartTimecode = currentProcess->audio_start_timecode;
			new_packet->EndTimecode = currentProcess->audio_end_timecode;
			new_packet->dataLength = audioBytesRead;
			new_packet->data = (binary *)audio_buffer;
			//new_packet->prevReference = ;
			//new_packet->nextReference = ;
			

			return new_packet;
		}else {
			currentProcess->readAudioBlock = -1;
		}
	}
	long aviFrameSize = AVI_frame_size(currentProcess->input_File, currentProcess->currentFrame);
 	if (aviFrameSize > 0) {
	 	char *frame_buffer = new char[aviFrameSize+1];
	 	int isKeyframe = 0;
	 	AVI_read_frame(currentProcess->input_File, frame_buffer, &isKeyframe);	 	
	        	
	 	if (currentProcess->currentFrame != 0)
	 		currentProcess->start_timecode = (currentProcess->currentFrame / currentProcess->input_File->fps) * 1000;
		currentProcess->end_timecode = currentProcess->start_timecode + (uint32)((1 / currentProcess->input_File->fps) * 1000);
		
		// Next Frame for the next pass
		currentProcess->currentFrame++;

		if (currentProcess->readAudioBlock != -1)
			currentProcess->readAudioBlock++;

		DataPacket *new_packet = new DataPacket;
		new_packet->TrackNumber = 1;
		new_packet->StartTimecode = currentProcess->start_timecode;
		new_packet->EndTimecode = currentProcess->end_timecode;
		new_packet->dataLength = aviFrameSize;
		new_packet->data = (binary *)frame_buffer;		
		new_packet->prevReference = isKeyframe;

		return new_packet;
	}

	return NULL;
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
	avi_input_plugin_data *currentProcess = (avi_input_plugin_data *)process_to_read;

	char *txt_buffer = new char [128];
	std::string trackXml;
	trackXml = "<?xml version=\"1.0\"?>";
	trackXml += "<LemAPI version=\"2\">";
	trackXml += "<TrackList>";
	// Start Video Track example
	trackXml += "<Track>";
	trackXml += "<TrackType>video</TrackType>";
	trackXml += "<CodecID>";
	trackXml += "V_MS/VFW/FOURCC"; //< This will obivously depend on your video type
	trackXml += "</CodecID>";
	// The CodecPrivate is used for V_MS/FOURCC/VFW
	trackXml += "<CodecPrivate encoding=\"base64\">";	
	
  BITMAPINFOHEADER_avilib *aviFormat = currentProcess->input_File->bitmap_info_header;
	int dataSize = aviFormat->bi_size;	
  binary *cpriv_avi = (binary *)malloc(dataSize);
	memcpy(cpriv_avi, aviFormat, dataSize);	
 	char *buffer_avi = new char[(4*dataSize/3)+1];
	membase64(buffer_avi, (char *)cpriv_avi, dataSize);	
  trackXml += buffer_avi; //< A Base64 encoded string, (this example string isn't valid :P)
	//delete cpriv_avi;
	//delete buffer_avi;

	trackXml += "</CodecPrivate>";	
	trackXml += "<Duration>";
	sprintf(txt_buffer, "%u", uint64((double)currentProcess->input_File->video_frames * currentProcess->input_File->fps));
	trackXml += txt_buffer; //< Length of track in ms
	trackXml += "</Duration>";
	trackXml += "<DefaultDuration>";
	sprintf(txt_buffer, "%u", (1000 / currentProcess->input_File->fps) * 1000000);
	trackXml += txt_buffer; //< DefaultDuration of frames/blocks in ns
	trackXml += "</DefaultDuration>";
	trackXml += "<Video>";
	trackXml += "<Width>"; //< Also X
	sprintf(txt_buffer, "%u", currentProcess->input_File->width);
	trackXml += txt_buffer;
	trackXml += "</Width>";	
	trackXml += "<Height>"; //< Also Y
	sprintf(txt_buffer, "%u", currentProcess->input_File->height);
	trackXml += txt_buffer;
	trackXml += "</Height>"; 	
	/*trackXml += "<DisplayWidth>";
	trackXml += "640";
	trackXml += "</DisplayWidth>";	
	trackXml += "<DisplayHeight>";
	trackXml += "480";
	trackXml += "</DisplayHeight>";*/	
	// The frame-rate is only here because most other formats are based on a constant framerate
	// Matroska is VFR and so it only uses the Framerate to calcualate the DefaultDuration for each block (or frame)
	trackXml += "<FrameRate>";
	sprintf(txt_buffer, "%f", currentProcess->input_File->fps);
	trackXml += txt_buffer;
	trackXml += "</FrameRate>";	
	trackXml += "<ColorDepth>";
	sprintf(txt_buffer, "%u", currentProcess->input_File->bitmap_info_header->bi_bit_count);
	trackXml += txt_buffer;
	trackXml += "</ColorDepth>";	
	trackXml += "</Video>";
	trackXml += "</Track>";
	if (currentProcess->input_File->wave_format_ex[0] != NULL) {
		// Start Video Track example
		trackXml += "<Track>";
		trackXml += "<TrackType>audio</TrackType>";
		trackXml += "<CodecID>";
		trackXml += "A_MS/ACM"; //< This will obivously depend on your video type
		trackXml += "</CodecID>";
		// The CodecPrivate is used for A_MS/ACM
		trackXml += "<CodecPrivate encoding=\"base64\">";	
		
		WAVEFORMATEX_avilib *wavFormat = currentProcess->input_File->wave_format_ex[0];
		int dataSize = sizeof(WAVEFORMATEX_avilib) + wavFormat->cb_size;	
		binary *cpriv_wav = (binary *)malloc(dataSize);
		memcpy(cpriv_wav, wavFormat, dataSize);	
 		char *buffer_wav = new char[(4*dataSize/3)+1];
		membase64(buffer_wav, (char *)cpriv_wav, dataSize);		
		trackXml += buffer_wav; //< A Base64 encoded string		
		//delete cpriv_wav;
		//delete buffer_wav;

		trackXml += "</CodecPrivate>";	
		trackXml += "<Duration>";		
		sprintf(txt_buffer, "%u %u", AVI_audio_chunks(currentProcess->input_File) * AVI_audio_mp3rate(currentProcess->input_File), AVI_audio_bytes(currentProcess->input_File));
		trackXml += txt_buffer; //< Length of track in ms
		trackXml += "</Duration>";
		trackXml += "<Audio>";
		trackXml += "<SampleRate>";
		sprintf(txt_buffer, "%u", wavFormat->n_samples_per_sec);
		trackXml += txt_buffer;
		trackXml += "</SampleRate>";
		trackXml += "<Channels>";
		sprintf(txt_buffer, "%u", wavFormat->n_channels);
		trackXml += txt_buffer;
		trackXml += "</Channels>";
		trackXml += "<BitDepth>";
		sprintf(txt_buffer, "%u", wavFormat->w_bits_per_sample);
		trackXml += txt_buffer;
		trackXml += "</BitDepth>";
		trackXml += "</Audio>";
		trackXml += "</Track>";
	}
	trackXml += "</TrackList>";
	trackXml += "</LemAPI>";
	delete txt_buffer;
	txt_buffer = NULL;

	//Copy the track XML data to a char *
	//delete currentProcess->trackXmlInfo;
	currentProcess->trackXmlInfo = new char[trackXml.length()];
	strcpy(currentProcess->trackXmlInfo, trackXml.c_str());
	currentProcess->trackXmlInfo[trackXml.length()] = 0;
	//Return
	return currentProcess->trackXmlInfo;
};

extern "C" int DeleteLemAPIProcess(void *process_to_close)
{
	avi_input_plugin_data *goodbye_process = (avi_input_plugin_data *)process_to_close;

	//delete goodbye_process->trackXmlInfo;
	AVI_close(goodbye_process->input_File);
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
