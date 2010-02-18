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
\file fio.cpp

A class for I/O into a file
*/

#include "fio.hpp"

#include <cassert>
#include <stdio.h>

using namespace LIBEBML_NAMESPACE;

IO_file::IO_file()
:h_file(NULL)
,m_filename("")
,position(0)
{}

IO_file::~IO_file()
{
	Close();
}

int IO_file::Open(std::string & filename, int mode)
{
	enum open_mode format;
	if ((mode & FILE_MODE_READ) == FILE_MODE_READ)
	{
		format = MODE_READ;
	}

	if ((mode & FILE_MODE_WRITE) == FILE_MODE_WRITE)
	{
		if ((mode & FILE_MODE_EXIST) == FILE_MODE_EXIST)
		{
			format = MODE_WRITE;
		}
		else
		{
			format = MODE_CREATE;
		}
	}

	h_file = new StdIOCallback(filename.c_str(), format);

	m_filename = filename;

	return int(h_file);
}

int IO_file::Close()
{
	if (h_file != NULL)
	{
		h_file->close();
		h_file = NULL;
	}

	return 0;
}

int IO_file::Read(void * buffer, size_t buffer_size)
{
	assert(h_file != NULL);

	size_t result = h_file->read(buffer, buffer_size);

	position += buffer_size;

	if (result != buffer_size)
	{
		printf("Unexpected End Of File in %s at position %ld\n",m_filename.c_str(),h_file->getFilePointer());
		return 0;
	}

	return result;
}

uint32 IO_file::ReadUpTo(void * buffer, size_t buffer_size)
{
	assert(h_file != NULL);

	size_t result = h_file->read(buffer, buffer_size);

	position += buffer_size;

	return result;
}

bool IO_file::SeekUp(uint64 SeekSize)
{
	assert(h_file != NULL);

	h_file->setFilePointer(SeekSize, seek_current);

	return true;
}

int IO_file::Write(const void * buffer, size_t buffer_size)
{
	assert(h_file != NULL);

	return h_file->write(buffer, buffer_size);
}

bool IO_file::Seek(uint64 SeekPosition)
{
	assert(h_file != NULL);

	h_file->setFilePointer(SeekPosition, seek_beginning);

	return true;
}

