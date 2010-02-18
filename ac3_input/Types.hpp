/*
	libmcf : parse and create MCF files, see http://mcf.sf.net/
	Copyright (C) 2002 Ingo Ralf Blum

	This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*!
	\file
	\version \$Id: Types.hpp,v 1.1 2003/07/21 15:08:18 jcsston Exp $
*/
#ifndef M2W_TYPES_HPP
#define M2W_TYPES_HPP

#include <clocale>
#include <string>

#include "Config.hpp"

#include "c_types.h"

START_M2W_NAMESPACE

END_M2W_NAMESPACE

#include "Endian.hpp" // binary needs to be defined

START_M2W_NAMESPACE

typedef Endian<int16,little_endian>  lil_int16;
typedef Endian<int32,little_endian>  lil_int32;
typedef Endian<int64,little_endian>  lil_int64;
typedef Endian<uint16,little_endian> lil_uint16;
typedef Endian<uint32,little_endian> lil_uint32;
typedef Endian<uint64,little_endian> lil_uint64;
typedef Endian<int16,big_endian>     big_int16;
typedef Endian<int32,big_endian>     big_int32;
typedef Endian<int64,big_endian>     big_int64;
typedef Endian<uint16,big_endian>    big_uint16;
typedef Endian<uint32,big_endian>    big_uint32;
typedef Endian<uint64,big_endian>    big_uint64;
typedef Endian<uint32,big_endian>    checksum;

END_M2W_NAMESPACE

#endif
