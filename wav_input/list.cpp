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

const FourCC LIST_chunk::listFourCC('L','I','S','T');

LIST_chunk::LIST_chunk(const RIFF_chunk *aChunk)
:RIFF_chunk(*aChunk)
{
}

LIST_chunk::LIST_TYPE LIST_chunk::FindType(IO_file & file)
{
	binary Type[4];

	if (!file.Read(Type, 4))
	{
		printf("Impossible to read LIST type\n");
		return LIST_UNKNOWN;
	}

	myFollowingSize -=4;

	if (LIST_Info::myFourCC == Type)
		return LIST_INFO;

	return LIST_UNKNOWN;
}

const FourCC LIST_Info::myFourCC('I','N','F','O');

const FourCC LISTI_IARL_chunk::gFourCC('I','A','R','L');

bool LIST_Info::Read(IO_file & file)
{
	// read all the chuncks
	/*    while (myFollowingSize && FindNextChunk())
	{
	}*/
	//    SkipData(file);

	return false;
}
