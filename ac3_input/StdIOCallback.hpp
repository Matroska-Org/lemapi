/*
	libmcf : parse and create MCF files, see http://mcf.sf.net/
	Copyright (C) 2002 Ingo Ralf Blum

	This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*!
	\file
	\version \$Id: StdIOCallback.hpp,v 1.1 2003/07/21 15:08:18 jcsston Exp $
*/
#ifndef M2W_STDIOCALLBACK_HPP
#define M2W_STDIOCALLBACK_HPP

#include "IOCallback.hpp"

#include <stdexcept>
#include <cerrno>

START_M2W_NAMESPACE

typedef enum open_mode {
    mode_read,
    mode_write,
    mode_create,
} open_mode_t;

// This class is currently private to the library, so there's no MCF_EXPORT.
class StdIOCallback:public IOCallback
{
    protected:
    	FILE*File;

    public:
	StdIOCallback(const char*Path, const enum open_mode Mode);
	virtual ~StdIOCallback()throw();

	virtual uint32 read(binary*Buffer,size_t Size);

	// Seek to the specified position. The mode can have either SEEK_SET, SEEK_CUR
	// or SEEK_END. The callback should return true(1) if the seek operation succeeded
	// or false (0), when the seek fails.
	virtual void setFilePointer(int64 Offset,seek_mode Mode=seek_beginning);

	// This callback just works like its read pendant. It returns the number of bytes written.
	virtual size_t write(const void*Buffer,size_t Size);

	// Although the position is always positive, the return value of this callback is signed to
	// easily allow negative values for returning errors. When an error occurs, the implementor
	// should return -1 and the file pointer otherwise.
	//
	// If an error occurs, an exception should be thrown.
	virtual uint64 getFilePointer();

	// The close callback flushes the file buffers to disk and closes the file. When using the stdio
	// library, this is equivalent to calling fclose. When the close is not successful, an exception
	// should be thrown.
	virtual void close();
};

END_M2W_NAMESPACE

#endif
