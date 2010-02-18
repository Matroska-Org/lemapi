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

#ifndef _M2W_CHUNK_H_
#define _M2W_CHUNK_H_

#include "Config.h"
#include "Types.h"

#include "fio.h"
#include "FourCC.h"

START_M2W_NAMESPACE

class RIFF_chunk {
    public:
	RIFF_chunk(char one=' ', char two=' ', char three=' ', char four=' ', bool bWriteSize = true)
	  :myFourCC(one, two, three, four)
	  ,myFollowingSize(0)
	  ,mySizeOn(bWriteSize)
	{}

	uint16 WriteHeader(IO_file & aFile);

//	const binary * FOUR_CC() const {return (const binary *) myFourCC.buffer(); }

	void setSize(const uint32 size) {myFollowingSize = size;}

    protected:
	FourCC myFourCC;
	uint32 myFollowingSize;
	bool   mySizeOn;
};

END_M2W_NAMESPACE

#endif // _M2W_CHUNK_H_
