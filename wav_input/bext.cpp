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

const FourCC BEXT_chunk::bextFourCC('b','e','x','t');
/*
BEXT_chunk::BEXT_chunk()
{
myExtension.codinghistory = NULL;
}
*/
BEXT_chunk::BEXT_chunk(const RIFF_chunk *aChunk)
:RIFF_chunk(*aChunk)
{
	myExtension.codinghistory = NULL;
}

BEXT_chunk::~BEXT_chunk()
{
	if (myExtension.codinghistory != NULL)
		delete myExtension.codinghistory;
}


bool BEXT_chunk::Read(IO_file & file)
{
	if (myFollowingSize < sizeof(broadcast_audio_extension)-sizeof(char *))
	{
		printf("Don't know how to handle the format data\n");
		return false;
	}

	binary *totalBuffer = new binary[myFollowingSize];
	if (totalBuffer == NULL)
	{
		printf("No more space to read Broadcast Extension header\n");
		return false;
	}

	if (myExtension.codinghistory != NULL)
		delete myExtension.codinghistory;

	myExtension.codinghistory = new char[myFollowingSize-sizeof(broadcast_audio_extension)];
	if (myExtension.codinghistory == NULL)
	{
		printf("No more space to read Broadcast Extension header\n");
		return false;
	}

	if (!file.Read(totalBuffer, myFollowingSize))
	{
		printf("Impossible to read Format header\n");
		return false;
	}

	binary *pointer = totalBuffer;
	memcpy(myExtension.description, pointer, sizeof(myExtension.description));
	pointer += sizeof(myExtension.description);

	memcpy(myExtension.originator, pointer, sizeof(myExtension.originator));
	pointer += sizeof(myExtension.originator);

	memcpy(myExtension.originatorreference, pointer, sizeof(myExtension.originatorreference));
	pointer += sizeof(myExtension.originatorreference);

	memcpy(myExtension.originationdate, pointer, sizeof(myExtension.originationdate));
	pointer += sizeof(myExtension.originationdate);

	memcpy(myExtension.originationtime, pointer, sizeof(myExtension.originationtime));
	pointer += sizeof(myExtension.originationtime);

	lil_uint64 lu64;
	lu64.Eval(pointer);
	myExtension.TimeReference = lu64;
	pointer += sizeof(uint64);

	lil_uint32 lu32;
	lu32.Eval(pointer);
	myExtension.Version = lu32;
	pointer += sizeof(uint32);

	memcpy(myExtension.UMID, pointer, sizeof(myExtension.UMID));
	pointer += sizeof(myExtension.UMID);

	memcpy(myExtension.reserved, pointer, sizeof(myExtension.reserved));
	pointer += sizeof(myExtension.reserved);

	memcpy(myExtension.codinghistory, pointer, myFollowingSize-sizeof(broadcast_audio_extension));
	pointer += myFollowingSize-sizeof(broadcast_audio_extension);

	delete totalBuffer;

	return true;
}
