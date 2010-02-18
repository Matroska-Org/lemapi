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
	handle FourCC values
*/

#include "wav2mka.h"

FourCC::FourCC(char one, char two, char three, char four)
 :val1(one)
 ,val2(two)
 ,val3(three)
 ,val4(four)
{}

FourCC::FourCC(void *buffer)
{
    char * value = (char *)(buffer);

    val1 = value[0];
    val2 = value[1];
    val3 = value[2];
    val4 = value[3];
}


FourCC::~FourCC()
{}

bool FourCC::operator==(const FourCC & other) const
{
    return (val1 == other.val1 &&
            val2 == other.val2 &&
            val3 == other.val3 &&
            val4 == other.val4);
}

bool FourCC::operator!=(const FourCC & other) const
{
    return (val1 != other.val1 ||
            val2 != other.val2 ||
            val3 != other.val3 ||
            val4 != other.val4);
}

