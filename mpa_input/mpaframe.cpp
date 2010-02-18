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
    \file mpaframe.cpp
*/

#include <cassert>

#include "Types.hpp"
#include "mpaframe.h"

START_M2W_NAMESPACE

const uint16 MPA_Frame::SamplingRates[Mpeg_Version_Size][4] =
{   //  00,   01,   10, 11
    {44100,48000,32000, 0}, // v1
    {22050,24000,16000, 0}, // v2
    {11025,12000, 8000, 0}, // v2.5
    {    0,    0,    0, 0}  // reserved
};

const int16 MPA_Frame::Bitrates[6][16] =
{
    { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, -1}, // V1, L1
    { 0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, -1}, // V1, L2
    { 0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, -1}, // V1, L3
    { 0, 32, 48, 56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, -1}, // V2, L1
    { 0,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, -1}, // V2, L2/L3
    {-1, -1, -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, -1}
};

MPA_Frame::MPA_Frame()
 :bValid(false)
{}

bool MPA_Frame::Eval(const binary Header[4])
{
    myBinaryValue[0] = Header[0];
    myBinaryValue[1] = Header[1];
    myBinaryValue[2] = Header[2];
    myBinaryValue[3] = Header[3];

    big_uint32 bu32;
    bu32.Eval(Header);
    uint32 header = bu32;

    switch ((header >> 19) & 0x03)
    {
	case 0x00:
	    myVersion = MPEG_VERSION_25;
	    break;
	case 0x01:
	    myVersion = MPEG_VERSION_UNKNOWN;
	    break;
	case 0x02:
	    myVersion = MPEG_VERSION_2;
	    break;
	case 0x03:
	    myVersion = MPEG_VERSION_1;
	    break;
    }

    switch ((header >> 17) & 0x03)
    {
	case 0x00:
	    myLayer = MPEG_LAYER_UNKNOWN;
	    break;
	case 0x01:
	    myLayer = MPEG_LAYER_3;
	    break;
	case 0x02:
	    myLayer = MPEG_LAYER_2;
	    break;
	case 0x03:
	    myLayer = MPEG_LAYER_1;
	    break;
    }
    myCrcOn = (((header >> 16) & 0x01) == 0);

    int BitrateIndex = 5;
    if (myVersion == MPEG_VERSION_1)
    {
	if (myLayer == MPEG_LAYER_1)
	    BitrateIndex = 0;
	else if (myLayer == MPEG_LAYER_2)
	    BitrateIndex = 1;
	else if (myLayer == MPEG_LAYER_3)
	    BitrateIndex = 2;
    }
    else if ((myVersion == MPEG_VERSION_2) || (myVersion == MPEG_VERSION_25))
    {
	if (myLayer == MPEG_LAYER_1)
	    BitrateIndex = 3;
	else if ((myLayer == MPEG_LAYER_2) || (myLayer == MPEG_LAYER_3))
	    BitrateIndex = 4;
    }

    bValid = true;
    // sampling rate
    mySamplingRate = SamplingRates[myVersion][(header >> 10) & 0x03];

    if (mySamplingRate == 0)
	bValid = false;

    myBitRate = Bitrates[BitrateIndex][(header >> 12) & 0x0F];

    if (myBitRate == -1)
	bValid = false;

    /// \todo check for forbidden bitrates

    myPaddingOn = (((header >> 9) & 0x01) != 0);
    myPrivateOn = (((header >> 8) & 0x01) != 0);
    myCopyrightOn = (((header >> 3) & 0x01) != 0);
    myOriginalOn = (((header >> 2) & 0x01) != 0);

    switch ((header >> 6) & 0x03)
    {
	case 0x00:
	    myChannels = MPEG_CHANNEL_STEREO;
	    break;
	case 0x01:
	    myChannels = MPEG_CHANNEL_JOINT_STEREO;
	    break;
	case 0x02:
	    myChannels = MPEG_CHANNEL_DUAL_CHANNEL;
	    break;
	case 0x03:
	    myChannels = MPEG_CHANNEL_MONO;
	    break;
    }

    switch ((header >> 0) & 0x03)
    {
	case 0x00:
	    myEmphasis = MPEG_EMPH_NONE;
	    break;
	case 0x01:
	    myEmphasis = MPEG_EMPH_50_15MS;
	    break;
	case 0x02:
	    myEmphasis = MPEG_EMPH_RESERVED;
	    break;
	case 0x03:
	    myEmphasis = MPEG_EMPH_CCITT_J17;
	    break;
    }

    return bValid;
}

uint16 MPA_Frame::SampleSize() const
{
    if (myLayer == MPEG_LAYER_1)
	return 384;
    if (myLayer == MPEG_LAYER_2)
	return 1152;
    if (myLayer == MPEG_LAYER_3)
    {
	if (myVersion == MPEG_VERSION_1)
	    return 1152;
	else if ((myVersion == MPEG_VERSION_2) || (myVersion == MPEG_VERSION_25))
	    return 576;
    }

    return 0; // unsupported format
}

/**
    \todo simplify with more general cases
*/
uint16 MPA_Frame::DataSize() const
{
    assert(bValid);
    uint16 result = 0;

    if (myVersion == MPEG_VERSION_1) {
	if (myLayer == MPEG_LAYER_1 && myPaddingOn) {
	    result = (12 * myBitRate * 1000 / mySamplingRate + 4) * 4;
	} else if (myLayer == MPEG_LAYER_1 && !myPaddingOn) {
	    result = (12 * myBitRate * 1000 / mySamplingRate) * 4;
	} else if ((myLayer == MPEG_LAYER_2 || myLayer == MPEG_LAYER_3) && myPaddingOn) {
	    result = (144 * myBitRate * 1000 / mySamplingRate) + 1;
	} else if ((myLayer == MPEG_LAYER_2 || myLayer == MPEG_LAYER_3) && !myPaddingOn) {
	    result = (144 * myBitRate * 1000 / mySamplingRate);
	}
    } else if ((myVersion == MPEG_VERSION_2) || (myVersion == MPEG_VERSION_25)) {
	if (myLayer == MPEG_LAYER_1 && myPaddingOn) {
	    result = (240 * myBitRate * 1000 / mySamplingRate + 4) * 4;
	} else if (myLayer == MPEG_LAYER_1 && !myPaddingOn) {
	    result = (240 * myBitRate * 1000 / mySamplingRate) * 4;
	} else if ((myLayer == MPEG_LAYER_2 || myLayer == MPEG_LAYER_3) && myPaddingOn) {
	    result = (72 * myBitRate * 1000 / mySamplingRate) + 1;
	} else if ((myLayer == MPEG_LAYER_2 || myLayer == MPEG_LAYER_3) && !myPaddingOn) {
	    result = (72 * myBitRate * 1000 / mySamplingRate);
	}
    }

    if (result != 0)
	result -= 4; // header handled separately

    return result;
}

bool MPA_Frame::Compatible(const MPA_Frame & aFrame) const
{
    if (!aFrame.bValid)
	return false;

    if (mySamplingRate != aFrame.mySamplingRate)
	return false;

    if (myLayer != aFrame.myLayer)
	return false;

    if (Channels() != aFrame.Channels())
	return false;

  return true;
}

END_M2W_NAMESPACE

