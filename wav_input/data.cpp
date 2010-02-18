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

#include "Types.h"

using namespace LIBEBML_NAMESPACE;

const FourCC DATA_chunk::dataFourCC('d','a','t','a');
/*
DATA_chunk::DATA_chunk()
:RIFF_chunk(dataFourCC)
{
}
*/
DATA_chunk::DATA_chunk(const RIFF_chunk *aChunk)
:RIFF_chunk(*aChunk)
{
}

uint32 DATA_chunk::ReadData(IO_file & file, binary * Buffer, const uint32 SizeToRead)
{
	uint32 data_read = file.ReadUpTo(Buffer, min(SizeToRead,myFollowingSize));
	myFollowingSize -= data_read;
	return data_read;
}
