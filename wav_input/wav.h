/*
    Copyright (C) 2002, Steve Lhomme <steve.lhomme @ free.fr>
    This file is part of mpa2wav.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/**
    \file
*/

#ifndef _M2W_WAV_H_
#define _M2W_WAV_H_

#include "Config.h"
#include "riff.h"
#include "format.h"
#include "fact.h"

START_M2W_NAMESPACE

class WAV_file : public RIFF_file {
    public:
   enum ChunkTypes {
	    DATA_CHUNK,
	    FORMAT_CHUNK,
	    FACT_CHUNK,
	    BROADCAST_CHUNK,
	    LIST_CHUNK,
	    LIST_UNKNOWN_CHUNK,
	    UNKNOWN_CHUNK,
	};

	WAV_file(const std::string & aFileName)
	    :RIFF_file(aFileName)
	    ,myChunk('W','A','V','E', false)
	    ,myDataChunk('d','a','t','a', true)
	{}

	virtual FORMAT_chunk & FormatEx() const = 0;

	uint32 Write(const binary * buffer, const uint32 buffer_size);

	/**
	    \note the fact + data chunk is written afterward, called by the child class
	*/
	void WriteHead(const uint16 extra_format, const uint32 data_size, bool seek_to_start);
	void WriteDataStart(const uint32 data_size);
	/**
	    Write the fact chunk
	*/
	void WriteSampleNumber(const uint32 sample_number);


	void setChannels(const uint16 value)      {FormatEx().setChannels(value);}
	void setSamplingRate(const uint32 value)  {FormatEx().setSamplingRate(value);}
	void setByterate(const uint32 value)      {FormatEx().setByterate(value);}
	void setAlignment(const uint16 value)     {FormatEx().setAlignment(value);}
	void setBitsPerSample(const uint16 value) {FormatEx().setBitsPerSample(value);}
	void setFormatTag(const uint16 value)     {FormatEx().setFormatTag(value);}
	
    protected:
  static const FourCC wave_FourCC;

	uint32    myFileSize;
	uint64    myReadData; ///< the size of already read data


	RIFF_chunk myChunk;
	RIFF_chunk myDataChunk;
	FACT_chunk myFactChunk;
};

END_M2W_NAMESPACE

#endif // _M2W_WAV_H_
