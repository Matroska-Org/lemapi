/*
Copyright (C) 2002, Steve Lhomme <steve.lhomme @ free.fr>
This file is part of ac32wav.

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
#include "ac3frame.h"

START_M2W_NAMESPACE

const uint16 AC3_Frame::SamplingRates[4] =
//  00,   01,   10, 11
{48000,44100,32000, 22050};

const int16 AC3_Frame::Bitrates[20] =
{ 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640, -1};

AC3_Frame::AC3_Frame()
:bValid(false)
{}

/**
\todo compute frame size
*/
bool AC3_Frame::Eval(const binary Header[5])
{
	bValid = true;

	myBinaryValue[0] = Header[0];
	myBinaryValue[1] = Header[1];
	myBinaryValue[2] = Header[2];
	myBinaryValue[3] = Header[3];
	myBinaryValue[4] = Header[4];

	big_uint16 bu16;
	bu16.Eval(&Header[2]);
	myCrc1 = bu16;

	// sampling rate
	mySamplingRate = SamplingRates[(Header[4] >> 6) & 0x03];

	if (mySamplingRate == 0)
		bValid = false;

	myBitRate = Bitrates[(Header[4] >> 1) & 0x1F];

	myFrameSize = (myBitRate * 96000L) / mySamplingRate;
	if ((myBitRate * 96000L) % mySamplingRate)
	{
		if (Header[4] & 0x01)
			myFrameSize++;
	}
	myFrameSize <<= 1; // size in words

	return bValid;
}

/*
uint16 AC3_Frame::SampleSize() const
{
return 0; // unsupported format
}
*/

/**
\todo simplify with more general cases
*/
uint16 AC3_Frame::DataSize() const
{
	assert(bValid);

	return myFrameSize;
}

bool AC3_Frame::Compatible(const AC3_Frame & aFrame) const
{
	if (!aFrame.bValid)
		return false;

	if (mySamplingRate != aFrame.mySamplingRate)
		return false;

	/*    if (myLayer != aFrame.myLayer)
	return false;

	if (Channels() != aFrame.Channels())
	return false;
	*/
	return true;
}

uint16 AC3_Frame::Channels() const
{
	switch (myChannels)
	{
	case AC3_1_AND_1:
		return 2;
	case AC3_CENTER:
		return 1;
	case AC3_LEFT_RIGHT:
		return 2;
	case AC3_LEFT_CENTER_RIGHT:
		return 3;
	case AC3_LEFT_RIGHT_SURROUND:
		return 3;
	case AC3_LEFT_CENTER_RIGHT_SURROUND:
		return 4;
	case AC3_LEFT_SURROUND_RIGHT_SURROUND:
		return 4;
	case AC3_CENTER_LEFT_SURROUND_RIGHT_SURROUND:
		return 5;
	}
	return 0;
}

END_M2W_NAMESPACE

