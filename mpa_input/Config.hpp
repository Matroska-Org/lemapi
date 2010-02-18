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
	\version \$Id: Config.hpp,v 1.1 2003/07/21 15:24:43 jcsston Exp $
*/
#ifndef M2W_CONFIG_HPP
#define M2W_CONFIG_HPP

#define LIL_ENDIAN 0x0102
#define BIG_ENDIAN 0x0201

// automatic endianess detection working on GCC
#if !defined(WORDS_BIGENDIAN)
#if (defined (__arm__) && ! defined (__ARMEB__)) || defined (__i386__) || defined (__i860__) || defined (__ns32000__) || defined (__vax__)
#define WORDS_BIGENDIAN 0 // for my testing platform (x86)
#elif defined (__sparc__) || defined (__alpha__) || defined (__PPC__) || defined (__mips__)
#define WORDS_BIGENDIAN 1 // for my testing platform (x86)
#else
// not automatically detected, put it yourself
#define WORDS_BIGENDIAN 0 // for my testing platform (x86)
#endif
#endif // not autoconf

// We allow the user to set the namespace for the library for cases, where the name conflicts
#ifdef NO_NAMESPACE // for older GCC

#define M2W_NAMESPACE
#define START_M2W_NAMESPACE
#define END_M2W_NAMESPACE
#else // NO_NAMESPACE

#ifndef M2W_NAMESPACE
#define M2W_NAMESPACE m2w
#define START_M2W_NAMESPACE namespace M2W_NAMESPACE {
#define END_M2W_NAMESPACE   };
#endif // M2W_NAMESPACE

#endif // NO_NAMESPACE

#ifndef countof
#define countof(x) (sizeof(x)/sizeof(x[0]))
#endif

// The M2W_DEBUG symbol is defined, when we are creating a debug build. In this
// case the debug logging code is compiled in.
#if (defined(DEBUG)||defined(_DEBUG))&&!defined(M2W_DEBUG)
#define M2W_DEBUG
#endif


#endif
