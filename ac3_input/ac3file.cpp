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
\file mpafile.cpp
*/

#include <cassert>

#include "ac3file.h"
#include "Types.hpp"

using namespace std;

START_M2W_NAMESPACE

AC3_File::AC3_File(const std::string & filename, const open_mode mode)
:StdIOCallback(filename.c_str(), mode)
,bFirstFrame(true)
{
	ResetHeader();
}

/**
\note XING/LAME Info Frame could be handled here
*/
bool AC3_File::SkipToFrameStart()
{
	setFilePointer(0);
	/*
	if (read(Header, 3) != 3)
	return false;

	if (Header[0] == 0x0B && Header[1] == 0x77)
	{
	SkipID3v2();
	}
	else
	{
	setFilePointer(0);
	}
	*/
	return true;
}

/**
*/
bool AC3_File::FindNextSyncWord()
{
	do {
		Header[0] = Header[1];
		Header[1] = Header[2];
		Header[2] = Header[3];
		Header[3] = Header[4];
		if (read(&Header[4], 1) != 1)
			break;

		if (Header[0] == 0x0B && Header[1] == 0x77)
			return true;
	} while (1);

	return false;
}

/**
*/
AC3_Frame & AC3_File::GetFrame()
{
	while (!myFrame.Eval(Header))
		FindNextSyncWord();
	return myFrame;
}

uint16 AC3_File::SkipData()
{
	assert(myFrame.IsValid());
	ResetHeader();

	uint64 fileBefore = getFilePointer();
	uint16 SizeToSkip = myFrame.DataSize();
	setFilePointer(SizeToSkip - 5, seek_current);

	// check if we really moved
	char tmp;
	fread(&tmp, 1, 1, File);
	if (feof(File) != 0)
	{
		// not the real position
		setFilePointer(0, seek_end);
		uint64 newpos = getFilePointer();
		return uint16(getFilePointer() - fileBefore) + 5;
	}
	else
	{
		setFilePointer(fileBefore + myFrame.DataSize() - 5);
		return uint16(getFilePointer() - fileBefore) + 5;
	}
}

void AC3_File::ResetHeader()
{
	Header[0] = 0;
	Header[1] = 0;
	Header[2] = 0;
	Header[3] = 0;
	Header[4] = 0;
}

void AC3_File::SkipID3v2()
{
	setFilePointer(0); // restart from the beggining of the file
	setFilePointer(6, seek_current);

	uint32 Size = 0;
	// read Extended Header Size
	binary aByte;
	if (read(&aByte, 1) != 1)
		return;
	Size |= ((uint32)aByte) << 21;

	// read Frame size
	if (read(&aByte, 1) != 1)
		return;
	Size |= ((uint32)aByte) << 14;

	// read Padding size
	if (read(&aByte, 1) != 1)
		return;
	Size |= ((uint32)aByte) << 7;

	// read Footer size
	if (read(&aByte, 1) != 1)
		return;
	Size |= ((uint32)aByte) << 0;

	setFilePointer(Size, seek_current);
}

/**
\todo read/skip data in the frame
*/
bool AC3_File::CheckInfoFrame()
{
	uint64 CurrentPosition = getFilePointer();
	if (FindNextSyncWord())
	{
		// check if this is an info frame
		*static_cast<M2W_NAMESPACE::AC3_Frame *>(&myInfoFrame) = GetFrame();

		if (!myInfoFrame.ParseFrame(*this))
		{
			ResetHeader();
			setFilePointer(CurrentPosition);
			return false;
		}
	}

	return true;
}

void AC3_File::GetData(binary * buffer, const uint16 buffer_size)
{
	assert(myFrame.IsValid());
	uint16 SizeToRead = myFrame.DataSize();
	assert(SizeToRead <= buffer_size);
	/** \todo verify with CRC    if (myFrame.HasCrc())
	SizeToSkip += 2;*/
	read(buffer, SizeToRead);
	ResetHeader();
}

bool AC3_File::Feed()
{
	if (bFirstFrame)
	{
		myStatFrame = myFrame;
		bFirstFrame = false;
		myFrameNumber = 1;

		uint64 CurrentPosition = getFilePointer();
		// get informations about the file (like channels)
		binary Bitstream;
		read(&Bitstream, 1);
		if ((Bitstream >> 5) <= 8)
		{
			// we know this bitstream
			read(&Bitstream, 1);
			myStatFrame.setChannelMode(Ac3_Channel(Bitstream >> 5));
		}

		setFilePointer(CurrentPosition);
		myBitrateTable.insert(myFrame.BitRate());
		//	myTotalBitRate = myFrame.BitRate();
	}
	else
	{
		if (!Test())
			return false;

		myBitrateTable.insert(myFrame.BitRate());
		myFrameNumber++;
		//	myTotalBitRate += myFrame.BitRate();
	}

	return true;
}

bool AC3_File::Test()
{
	return myStatFrame.Compatible(myFrame);
}

void AC3_File::FixStats()
{
	std::set<uint16>::const_iterator bitrate = myBitrateTable.begin();

	if (IsCBR()) {
		myAverageBitrate = *bitrate * 1000;
		//	setAlignment(1);
	}
	else {
		myAverageBitrate = uint32((myTotalBitRate * 1000) / myFrameNumber);
		//	setAlignment(1);
	}
}

END_M2W_NAMESPACE
