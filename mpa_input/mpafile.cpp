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
    \file mpafile.cpp
*/

#include <cassert>

#include "mpafile.h"
#include "Types.hpp"

using namespace std;

START_M2W_NAMESPACE

MPA_File::MPA_File(const std::string & filename, const open_mode mode)
 :StdIOCallback(filename.c_str(), mode)
 ,bFirstFrame(true)
{
    ResetHeader();
}

/**
    \note XING/LAME Info Frame could be handled here
*/
bool MPA_File::SkipToFrameStart()
{
    setFilePointer(0);

    if (read(Header, 3) != 3)
	return false;

    if (Header[0] == 'I' && Header[1] == 'D' && Header[2] == '3')
    {
	SkipID3v2();
    }
    else
    {
	setFilePointer(0);
    }

    return true;
}

/**
*/
bool MPA_File::FindNextSyncWord()
{
    do {
	Header[0] = Header[1];
	Header[1] = Header[2];
	Header[2] = Header[3];
	if (read(&Header[3], 1) != 1)
	    break;

	big_uint32 bu32;
	bu32.Eval(Header);
	uint32 myHeaderValue = bu32;

	if ((myHeaderValue >> 21) == (0xFFFFFFFF >> 21))
	{
	    // this is a valid syncword
	    return true;
	}
    } while (1);

    return false;
}

/**
*/
MPA_Frame & MPA_File::GetFrame()
{
    while (!myFrame.Eval(Header))
	FindNextSyncWord();
    return myFrame;
}

void MPA_File::SkipData()
{
    assert(myFrame.IsValid());
    uint16 SizeToSkip = myFrame.DataSize();
/** \todo verify with CRC    if (myFrame.HasCrc())
	SizeToSkip += 2;*/
    setFilePointer(SizeToSkip, seek_current);
    ResetHeader();
}

void MPA_File::ResetHeader()
{
    Header[0] = 0;
    Header[1] = 0;
    Header[2] = 0;
    Header[3] = 0;
}

void MPA_File::SkipID3v2()
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
bool MPA_File::CheckInfoFrame()
{
    uint64 CurrentPosition = getFilePointer();
    if (FindNextSyncWord())
    {
	// check if this is an info frame
	*static_cast<M2W_NAMESPACE::MPA_Frame *>(&myInfoFrame) = GetFrame();

	if (!myInfoFrame.ParseFrame(*this))
	{
	    ResetHeader();
	    setFilePointer(CurrentPosition);
	    return false;
	}
    }

    return true;
}

void MPA_File::GetData(binary * buffer, const uint16 buffer_size)
{
    assert(myFrame.IsValid());
    uint16 SizeToRead = myFrame.DataSize();
    assert(SizeToRead <= buffer_size);
/** \todo verify with CRC    if (myFrame.HasCrc())
	SizeToSkip += 2;*/
    read(buffer, SizeToRead);
    ResetHeader();
}

bool MPA_File::Feed()
{
    if (bFirstFrame)
    {
	myStatFrame = myFrame;
	bFirstFrame = false;
	myFrameNumber = 1;
	myTotalBitRate = myFrame.BitRate();
    }
    else
    {
	if (!Test())
	    return false;

	myBitrateTable.insert(myFrame.BitRate());
	myFrameNumber++;
	myTotalBitRate += myFrame.BitRate();
    }

    return true;
}

bool MPA_File::Test()
{
    return myStatFrame.Compatible(myFrame);
}

void MPA_File::FixStats()
{
    std::set<uint16>::const_iterator bitrate = myBitrateTable.begin();

    if (IsCBR())
	myAverageBitrate = *bitrate * 1000;

    else
	myAverageBitrate = uint32((myTotalBitRate * 1000) / myFrameNumber);
}

END_M2W_NAMESPACE
