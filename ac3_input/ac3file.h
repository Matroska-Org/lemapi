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
\file mpafile.h
*/

#ifndef _M2W_FILE_H_
#define _M2W_FILE_H_

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER

#include <set>
#include <string>

#include "StdIOCallback.hpp"
#include "ac3frame.h"
#include "ac3iframe.h"

START_M2W_NAMESPACE

class AC3_File : public StdIOCallback {
public:
	/// \brief file constructor
	AC3_File(const std::string & filename, const open_mode mode);

	/// \brief Skip from the beggining of the file to the first valid frame
	bool SkipToFrameStart();

	/// \brief Find the next syncword in the file
	bool FindNextSyncWord();

	/// \brief return the current frame
	AC3_Frame & GetFrame();

	/// \brief check in the file for the InfoFrame (Xing/LAME)
	bool CheckInfoFrame();

	/// \brief Get the read/created info frame
	const AC3_Frame & GetInfoFrame() const { return myInfoFrame;}

	/// \brief Skip the data and the possible CRC
	uint16 SkipData();

	/// \brief put the frame data in the buffer
	void GetData(binary * buffer, const uint16 buffer_size);

	bool IsCBR() const {return (myBitrateTable.size() == 1);}

	uint32 AverageBitRate() const {return myAverageBitrate;}

	uint32 SamplingRate() const {return myStatFrame.SamplingRate();}

	bool Feed();
	bool Test();
	void FixStats();

	//	Mpeg_Layer Layer() const {return myStatFrame.Layer();}

	const AC3_Frame & FrameType() {return myStatFrame;}

protected:
	AC3_Frame      myFrame;
	AC3_Frame      myStatFrame;
	MPA_Lame_Frame myInfoFrame;

	binary Header[5]; // AC3 sync frame is 5 bytes big !

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
