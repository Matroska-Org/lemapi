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

#include <cassert>

#include "wav2mka.h"

#include "Types.h"

using namespace LIBEBML_NAMESPACE;

const FourCC WAV_file::wave_FourCC('W','A','V','E');

WAV_file::WAV_file(std::string & filename)
:RIFF_file(filename)
,myReadData(0)
// ,myCurrChunk(NULL)
,myFmt(NULL)
,myData(NULL)
{
}

WAV_file::~WAV_file()
{
}

int WAV_file::ValidateHeader()
{
	if (!m_file.Opened())
		return -1;

	// validating header of file created not supported
	if (m_mode != RIFF_READ)
		return -1;

	if (RIFF_file::ValidateHeader() != 0)
		return -1;

	binary buffer[4];
	// read the supposed file size
	/// \todo extract the file size
	if (!m_file.Read(buffer, 4))
	{
		return -1;
	}
	lil_uint32 lu32;
	lu32.Eval(buffer);
	myFileSize = lu32;

	// read the WAVE FourCC
	if (!m_file.Read(buffer, 4))
	{
		return -1;
	}

	FourCC supposedWAVE(buffer);
	if (supposedWAVE != wave_FourCC)
	{
		printf("Found unexpected FourCC '%c%c%c%c' instead of 'WAVE' in %s\n", supposedWAVE.c1(), supposedWAVE.c2(), supposedWAVE.c3(), supposedWAVE.c4(), m_filename.c_str());
		return -1;
	}

	//    myCurrentChunkSize = 0;

	return 0;
}

uint32 WAV_file::ReadData(binary * Buffer, const uint32 MaxReadSize)
{
	assert(myFmt != NULL);
	assert(myData != NULL);

	uint32 result = 0;
	uint32 SizeToRead = MaxReadSize - (MaxReadSize % myFmt->BlockAlign());

	result = myData->ReadData(m_file, Buffer, SizeToRead);

	myReadData += result;

	return result;
}

uint32 WAV_file::GetCurrentTimecode() const
{
	assert(myFmt != NULL);

	return (1000 * myReadData) / myFmt->ByteRate(); // (bitrate / blockalign) / number of channels
}

/*
bool WAV_file::FindFormat()
{
if (!m_file.Opened())
return false;

// validating header of file created not supported
if (m_mode != RIFF_READ)
return false;

do {
if (!myFmt.ReadHead(m_file))
return false;
if (FourCC(myFmt) == myFmt.fmtFourCC)
break;
// skip the data
if (!m_file.SeekUp(myFmt.FollowingSize()))
return false;
} while (1);

return true;
}

bool WAV_file::FindData()
{
if (!m_file.Opened())
return false;

// validating header of file created not supported
if (m_mode != RIFF_READ)
return false;

do {
if (!myData.ReadHead(m_file))
return false;
if (FourCC(myData) == myData.dataFourCC)
break;
// skip the data
if (!m_file.SeekUp(myData.FollowingSize()))
return false;
} while (1);

return true;
}

bool WAV_file::ReadFormat()
{
return myFmt.Read(m_file);
}
*/

WAV_file::ChunkTypes WAV_file::ChunkType()
{
	if (FMT_chunk::fmtFourCC == *myCurrChunk)
		return FORMAT_CHUNK;
	else if (DATA_chunk::dataFourCC == *myCurrChunk)
		return DATA_CHUNK;
	else if (BEXT_chunk::bextFourCC == *myCurrChunk)
		return BROADCAST_CHUNK;
	else if (LIST_chunk::listFourCC == *myCurrChunk)
		return LIST_CHUNK;
	else
		return UNKNOWN_CHUNK;
}
