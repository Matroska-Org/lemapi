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
Classes corresponding to RIFF handling
*/

#include "wav2mka.h"

#include <cassert>

const FourCC RIFF_file::general_FourCC('R','I','F','F');
const FourCC RIFF_file::data_FourCC('d','a','t','a');

RIFF_file::RIFF_file(std::string & filename)
:m_filename(filename)
,m_mode(RIFF_UNKNOWN)
,passed_generalFourCC(false)
,myCurrChunk(NULL)
{
}

RIFF_file::~RIFF_file()
{
}

int RIFF_file::Open(RIFF_MODE mode)
{
	assert(m_mode == RIFF_UNKNOWN && !m_file.Opened());

	int result = 0;

	if (mode == RIFF_READ)
		result = m_file.Open(m_filename, FILE_MODE_EXIST|FILE_MODE_READ|FILE_MODE_BINARY);
	else if (mode == RIFF_WRITE)
		result = m_file.Open(m_filename, FILE_MODE_WRITE|FILE_MODE_BINARY);
	else if (mode == RIFF_CREATE)
		result = m_file.Open(m_filename, FILE_MODE_WRITE|FILE_MODE_ERASE|FILE_MODE_BINARY);

	if (result != 0)
	{
		m_mode = mode;
	}

	return result;
}

bool RIFF_file::FindNextChunk()
{
	if (myCurrChunk != NULL)
		return false;

	if (!m_file.Opened())
		return false;

	// validating header of file created not supported
	if (m_mode != RIFF_READ)
		return false;

	myCurrChunk = new RIFF_chunk;

	if (myCurrChunk == NULL)
		return false;

	if (!myCurrChunk->ReadHead(m_file))
		return false;

	return true;
}

int RIFF_file::ValidateHeader()
{
	if (!m_file.Opened())
		return -1;

	// validating header of file created not supported
	if (m_mode != RIFF_READ)
		return -1;

	char buffer[4];
	// read the RIFF FourCC
	if (!m_file.Read(buffer, 4))
	{
		return -1;
	}

	FourCC supposedRIFF(buffer);
	if (supposedRIFF != general_FourCC)
	{
		printf("Found unexpected FourCC %c%c%c%c instead of RIFF in %s\n", supposedRIFF.c1(), supposedRIFF.c2(), supposedRIFF.c3(), supposedRIFF.c4(), m_filename.c_str());
		return -1;
	}

	return 0;
}
