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
Classes corresponding to RIFF handling
*/

#include "wav2mka.h"

#include "Types.h"

using namespace LIBEBML_NAMESPACE;

const FourCC FMT_chunk::fmtFourCC('f','m','t',' ');
/*
FMT_chunk::FMT_chunk()
:RIFF_chunk(fmtFourCC)
,myChannels(0)
,mySamplerate(0)
,myBytetrate(0)
,myBlockAlign(0)
,myBitPerSample(0)
,ExtraCodecData(NULL)
,myExtraCodecDataSize(0)
{
}
*/
FMT_chunk::FMT_chunk(const RIFF_chunk *aChunk)
:RIFF_chunk(*aChunk)
,myChannels(0)
,mySamplerate(0)
,myBytetrate(0)
,myBlockAlign(0)
,myBitPerSample(0)
,ExtraCodecData(NULL)
,myExtraCodecDataSize(0)
{
}

FMT_chunk::~FMT_chunk()
{
	if (ExtraCodecData != NULL)
	{
		delete ExtraCodecData;
	}
}

bool FMT_chunk::Read(IO_file & file)
{
	if (myFollowingSize < 16)
	{
		printf("Don't know how to handle the format data\n");
		return false;
	}

	binary *totalBuffer = new binary[16];
	if (totalBuffer == NULL)
	{
		printf("No more space to read Format header\n");
		return false;
	}

	if (!file.Read(totalBuffer, 16))
	{
		printf("Impossible to read Format header\n");
		return false;
	}

	binary *seekPt = totalBuffer;
	myFormat[0] = *seekPt++;
	myFormat[1] = *seekPt++;

	lil_uint16 lu16;
	lu16.Eval(seekPt);
	myChannels = lu16;
	seekPt += sizeof(uint16);

	lil_uint32 lu32;
	lu32.Eval(seekPt);
	mySamplerate = lu32;
	seekPt += sizeof(uint32);

	lu32.Eval(seekPt);
	myBytetrate = lu32;
	seekPt += sizeof(uint32);

	lu16.Eval(seekPt);
	myBlockAlign = lu16;
	seekPt += sizeof(uint16);

	lu16.Eval(seekPt);
	myBitPerSample = lu16;
	seekPt += sizeof(uint16);

	myExtraCodecDataSize = myFollowingSize - 16;
	if (myExtraCodecDataSize == 0)
	{
		ExtraCodecData = NULL;
	}
	else
	{
		ExtraCodecData = new binary[myExtraCodecDataSize];
		if (ExtraCodecData == NULL)
		{
			printf("No more space to read Format header\n");
			return false;
		}

		if (!file.Read(ExtraCodecData, myExtraCodecDataSize))
		{
			printf("Impossible to read Codec Data\n");
			return false;
		}
	}

	delete totalBuffer;

	return true;
}

WAVEFORMATEX_* FMT_chunk::GetWAVEFORMATEX()
{
	WAVEFORMATEX_ *acm_wav = (WAVEFORMATEX_ *)malloc(sizeof(WAVEFORMATEX_)+myExtraCodecDataSize);
	memcpy(acm_wav->wFormatTag, myFormat, 2);
	acm_wav->nAvgBytesPerSec = myBytetrate;
	acm_wav->nBlockAlign = myBlockAlign;
	acm_wav->nChannels = myChannels;
	acm_wav->nSamplesPerSec = mySamplerate;
	acm_wav->wBitsPerSample = myBitPerSample;
	acm_wav->cbSize = myExtraCodecDataSize;	
	if (myExtraCodecDataSize > 0)
		memcpy(acm_wav+sizeof(WAVEFORMATEX_), ExtraCodecData, myExtraCodecDataSize);

	return acm_wav;
};