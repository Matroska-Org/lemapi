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

#ifndef _M2W_RIFF_H_
#define _M2W_RIFF_H_

#include "fio.h"

#include "chunk.h"

#include <string>

START_M2W_NAMESPACE

class RIFF_file {
    public:
	RIFF_file(const std::string & aFileName)
	    :myChunk('R','I','F','F', false)
	    ,myFileName(aFileName)
	{}
	virtual ~RIFF_file() {Close();}

	bool Open(const int mode);
	void Close();

	virtual void WriteHead(const uint32 riff_size = 0, bool seek_to_start = true);

    protected:
	RIFF_chunk  myChunk;
	IO_file     myFile;
	std::string myFileName;
};

END_M2W_NAMESPACE

#endif // _M2W_RIFF_H_
