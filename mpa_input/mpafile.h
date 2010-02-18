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
    \file mpafile.h
*/

#ifndef _M2W_FILE_H_
#define _M2W_FILE_H_

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER

#include <set>
#include <string>

#include "StdIOCallback.h"
#include "mpaframe.h"
#include "mpiframe.h"

START_M2W_NAMESPACE

class MPA_File : public StdIOCallback {
    public:
	/// \brief file constructor
	MPA_File(const std::string & filename, const open_mode mode);

	/// \brief Skip from the beggining of the file to the first valid frame
	bool SkipToFrameStart();

	/// \brief Find the next syncword in the file
	bool FindNextSyncWord();

	/// \brief return the current frame
	MPA_Frame & GetFrame();

	/// \brief check in the file for the InfoFrame (Xing/LAME)
	bool CheckInfoFrame();

	/// \brief Get the read/created info frame
	const MPA_Frame & GetInfoFrame() const { return myInfoFrame;}

	/// \brief Skip the data and the possible CRC
	void SkipData();

	/// \brief put the frame data in the buffer
	void GetData(binary * buffer, const uint16 buffer_size);

	bool IsCBR() const {return (myBitrateTable.size() == 1);}

	uint32 AverageBitRate() const {return myAverageBitrate;}

	uint32 SamplingRate() const {return myStatFrame.SamplingRate();}

	bool Feed();
	bool Test();
	void FixStats();

//	Mpeg_Layer Layer() const {return myStatFrame.Layer();}

	const MPA_Frame & FrameType() {return myStatFrame;}
	
    protected:
	MPA_Frame      myFrame;
	MPA_Frame      myStatFrame;
	MPA_Lame_Frame myInfoFrame;

	binary Header[4]; // MPEG-I/II audio header is 32 bits in big endian

	void ResetHeader();
	void SkipID3v2();

	bool             bFirstFrame;
	std::set<uint16> myBitrateTable; // bitrate/frame count
	uint32           myAverageBitrate;

	uint32           myFrameNumber;
	uint64           myTotalBitRate;
};

END_M2W_NAMESPACE

#endif // _M2W_FILE_H_
