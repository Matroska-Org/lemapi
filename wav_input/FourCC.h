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

#ifndef _M2W_CONFG_HPP_
#define _M2W_CONFG_HPP_

#include "Config.h"
#include "c_types.h"

START_M2W_NAMESPACE

class FourCC {
    public:
	FourCC(char one=' ', char two=' ', char three=' ', char four=' ');
	/// The constructor will handle the endianess problems
	FourCC(void * buffer);
	~FourCC();

	const unsigned char* buffer() const {return val;}
	const uint8 size() const {return 4;}

	bool operator==(const FourCC & other) const;
	bool operator!=(const FourCC & other) const;

	inline unsigned char c1() const {return val[0];}
	inline unsigned char c2() const {return val[1];}
	inline unsigned char c3() const {return val[2];}
	inline unsigned char c4() const {return val[3];}

    protected:
	unsigned char val[4];
};

END_M2W_NAMESPACE

#endif // _M2W_CONFG_HPP_
