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

#ifndef _M2W_FORMAT_H_
#define _M2W_FORMAT_H_

#include "chunk.h"
#include "fio.h"

START_M2W_NAMESPACE

typedef struct {
    uint16 wFormatTag;
    uint16 nChannels;
    uint32 nSamplesPerSec;
    uint32 nAvgBytesPerSec;
    uint16 nBlockAlign;
    uint16 wBitsPerSample;
    uint16 cbSize;
} WAVEFORMATEX;

const uint8 sizeof_WAVEFORMATEX = 18; // due to alignment problems

const uint16 WAVE_FORMAT_UNKNOWN    = 0x0000;
const uint16 WAVE_FORMAT_ADPCM      = 0x0001;

class FORMAT_chunk : public RIFF_chunk {
    public:
	FORMAT_chunk()
	    :RIFF_chunk('f','m','t',' ')
	{
	    myFormat.cbSize = 0;
	}
	virtual ~FORMAT_chunk() {}

	virtual uint32 Write(IO_file & aFile);

	uint32 Size() const {return sizeof_WAVEFORMATEX; }

	uint16 SamplingRate() const {return myFormat.nSamplesPerSec;}

	void setChannels(const uint16 value) {myFormat.nChannels = value;}
	void setSamplingRate(const uint32 value) {myFormat.nSamplesPerSec = value;}
	void setByterate(const uint32 value) {myFormat.nAvgBytesPerSec = value;}
	void setAlignment(const uint16 value) {myFormat.nBlockAlign = value;}
	void setBitsPerSample(const uint16 value) {myFormat.wBitsPerSample = value;}
	void setExtraSize(const uint16 value) {myFormat.cbSize = value;}
	void setFormatTag(const uint16 value) {myFormat.wFormatTag = value;}

    protected:
	WAVEFORMATEX myFormat;
};

END_M2W_NAMESPACE

#endif // _M2W_FORMAT_H_
