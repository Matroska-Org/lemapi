/*
	libmcf : parse and create MCF files, see http://mcf.sf.net/
	Copyright (C) 2002 Steve Lhomme

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
    \version \$Id: Endian.hpp,v 1.1 2003/07/21 15:24:43 jcsston Exp $
    \author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef M2W_ENDIAN_HPP
#define M2W_ENDIAN_HPP

#include <algorithm>

#include "Config.hpp" // contains _ENDIANESS_

START_M2W_NAMESPACE

enum endianess {
    big_endian,   ///< PowerPC, Alpha, 68000
    little_endian ///< Intel x86 platforms
};

/*!
    \class Endian
    \brief general class to handle endian-specific buffers
    \note don't forget to define/undefine _ENDIANESS_ to BIG_ENDIAN depending on your machine
*/
template<class TYPE, endianess ENDIAN> class Endian
{
    public:
	Endian() {}

	Endian(const TYPE value)
	 :platform_value(value)
	{
	    process_endian();
	}

	inline Endian & Eval(const binary *endian_buffer)
	{
	    endian_value = *(TYPE *)(endian_buffer);
	    process_platform();
	    return *this;
	}

	inline void Fill(binary *endian_buffer) const
	{
	    *(TYPE*)endian_buffer = endian_value;
	}

	inline operator TYPE() const { return platform_value; }
	inline TYPE endian() const   { return endian_value; }
	inline TYPE &endian()        { return endian_value; }
	inline size_t size() const   { return sizeof(TYPE); }

    protected:
	TYPE platform_value;
	TYPE endian_value;

	inline void process_endian()
	{
	    endian_value = platform_value;
#if WORDS_BIGENDIAN
	    if (ENDIAN == little_endian)
		std::reverse(reinterpret_cast<uint8*>(&endian_value),reinterpret_cast<uint8*>(&endian_value+1));
#else  // _ENDIANESS_
	    if (ENDIAN == big_endian)
		std::reverse(reinterpret_cast<uint8*>(&endian_value),reinterpret_cast<uint8*>(&endian_value+1));
#endif // _ENDIANESS_
	}

	inline void process_platform()
	{
	    platform_value = endian_value;
#if WORDS_BIGENDIAN
	    if (ENDIAN == little_endian)
		std::reverse(reinterpret_cast<uint8*>(&platform_value),reinterpret_cast<uint8*>(&platform_value+1));
#else  // _ENDIANESS_
	    if (ENDIAN == big_endian)
		std::reverse(reinterpret_cast<uint8*>(&platform_value),reinterpret_cast<uint8*>(&platform_value+1));
#endif // _ENDIANESS_
	}
};

END_M2W_NAMESPACE

#endif // M2W_ENDIAN_HPP
