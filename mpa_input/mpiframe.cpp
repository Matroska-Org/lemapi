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
    \file mpiframe.cpp
*/

#include "mpiframe.h"

START_M2W_NAMESPACE

bool MPA_Lame_Frame::ParseFrame(IOCallback & iStream)
{
    if (!MPA_Info_Frame::ParseFrame(iStream))
	return false;

    if (bIsLame)
    {
    }

    return true;
}

bool MPA_Info_Frame::ParseFrame(IOCallback & iStream)
{
    if (totalBuffer)
	delete totalBuffer;

    totalBuffer = new binary[DataSize()];

    if (!totalBuffer)
	return false;

    if (iStream.read(totalBuffer, DataSize()) != DataSize())
    {
	return false;
    }

    binary *Head = totalBuffer;

    /*  determine offset of header */
    if( myVersion == MPEG_VERSION_1 )
    {
	/* mpeg1 */
	if( myLayer == MPEG_LAYER_3 )
	    Head+= 32;
	else
	    Head+= 17;
    }
    else
    {
	/* mpeg2 */
	if( myLayer == MPEG_LAYER_3 )
	    Head+= 17;
	else
	    Head+= 9;
    }

    if (Head[3] == 'g' && Head[2] == 'n' && Head[1] == 'i' && Head[0] == 'X') // Xing frame
    {
	// this is a Xing frame
	bIsLame = false;
    }
    else if (Head[3] == 'o' && Head[2] == 'f' && Head[1] == 'n' && Head[0] == 'I') // Lame frame
    {
	// this is a LAME frame
	bIsLame = true;
    }
    else
	return false;

    return true;
}

END_M2W_NAMESPACE

