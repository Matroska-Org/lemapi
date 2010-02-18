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

#include "format.h"

START_M2W_NAMESPACE

uint32 FORMAT_chunk::Write(IO_file & aFile)
{
	setSize(sizeof_WAVEFORMATEX + myFormat.cbSize);
	//    aFile.Write(FOUR_CC(), 4);
	WriteHeader(aFile);

	lil_uint32 lu32;
	/*
	lu32 = sizeof_WAVEFORMATEX + myFormat.cbSize;
	aFile.Write(&lu32.endian(), sizeof(uint32));
	*/
	lil_uint16 lu16;
	lu16 = myFormat.wFormatTag;
	aFile.Write(&lu16.endian(), sizeof(uint16));

	lu16 = myFormat.nChannels;
	aFile.Write(&lu16.endian(), sizeof(uint16));

	lu32 = myFormat.nSamplesPerSec;
	aFile.Write(&lu32.endian(), sizeof(uint32));

	lu32 = myFormat.nAvgBytesPerSec;
	aFile.Write(&lu32.endian(), sizeof(uint32));

	lu16 = myFormat.nBlockAlign;
	aFile.Write(&lu16.endian(), sizeof(uint16));

	lu16 = myFormat.wBitsPerSample;
	aFile.Write(&lu16.endian(), sizeof(uint16));

	lu16 = myFormat.cbSize;
	aFile.Write(&lu16.endian(), sizeof(uint16));

	return 4 + sizeof_WAVEFORMATEX;
}

END_M2W_NAMESPACE
