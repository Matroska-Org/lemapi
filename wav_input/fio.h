/*
    Copyright (C) 2002, Steve Lhomme <steve.lhomme@free.fr>
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

    A class for I/O into a file
*/

#ifndef _FIO_HPP_
#define _FIO_HPP_

#include <string>
#include <stdio.h>

#include "StdIOCallback.h"

using namespace LIBEBML_NAMESPACE;

const int FILE_MODE_BINARY = 0x0001;
const int FILE_MODE_READ   = 0x0002;
const int FILE_MODE_WRITE  = 0x0004;
const int FILE_MODE_ERASE  = 0x0008;
const int FILE_MODE_EXIST  = 0x0010;
const int FILE_MODE_APPEND = 0x0020;

class IO_file {
    public:
	IO_file();
	virtual ~IO_file();

	/// open the file for reading
	int Open(std::string & filename, const int mode);
	int Close();

	int Read(binary * buffer, size_t buffer_size);
	uint32 ReadUpTo(binary * buffer, size_t buffer_size);

	int Write(const void * buffer, size_t buffer_size);

	inline bool Opened() const {return (h_file != NULL);}

	bool SeekUp(uint64 SeekSize);
	bool Seek(uint64 SeekPosition);

    protected:      
	LIBEBML_NAMESPACE::StdIOCallback * h_file;
	std::string m_filename;
	unsigned int position;
};

#endif // _FIO_HPP_
