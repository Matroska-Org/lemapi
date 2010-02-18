/*
	libmcf, a library to parse/write MCF files, see http://mcf.sf.net/
	Copyright (c) 2002 Steve Lhomme

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
    \file libmcf_t.h
    \version \$Id: c_types.h,v 1.1 2003/07/21 15:12:57 jcsston Exp $
    \author Steve Lhomme     <robux4 @ users.sf.net>

    \brief Misc type definitions for the C API of libmcf

    \note These types should be compiler/language independant (just platform dependant)
*/

#ifndef M2W_T_H_INCLUDED_
#define M2W_T_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

// There are special implementations for certain platforms. For example on Windows
// we use the Win32 file API. here we set the appropriate macros.
#if defined(_WIN32)||defined(WIN32)
#define M2W_WIN32
#else
#define M2W_UNIX
#endif

// Changed char is unsigned now (signedness was causing trouble in mcfendil)
#if defined(_WIN32) && !defined(__GNUC__)		// Microsoft Visual C++
    typedef signed __int64 int64;
    typedef signed __int32 int32;
    typedef signed __int16 int16;
    typedef signed __int8 int8;
    typedef __int8 character;
    typedef unsigned __int64 uint64;
    typedef unsigned __int32 uint32;
    typedef unsigned __int16 uint16;
    typedef unsigned __int8 uint8;
#else
#ifdef DJGPP				/* SL : DJGPP doesn't support POSIX types ???? */
    typedef signed long long int64;
    typedef signed long int32;
    typedef signed short int16;
    typedef signed char int8;
    typedef char character;
    typedef unsigned long long uint64;
    typedef unsigned long uint32;
    typedef unsigned short uint16;
    typedef unsigned char uint8;
#else	/* DJGPP -> anything else */
#include <sys/types.h>
#if defined(sun) && (defined(__svr4__) || defined(__SVR4)) // SOLARIS
    typedef uint64_t u_int64_t;
    typedef uint32_t u_int32_t;
    typedef uint16_t u_int16_t;
    typedef uint8_t  u_int8_t;
#endif // SOLARIS
    typedef int64_t int64;
    typedef int32_t int32;
    typedef int16_t int16;
    typedef int8_t int8;
    typedef int8_t character;
    typedef u_int64_t uint64;
    typedef u_int32_t uint32;
    typedef u_int16_t uint16;
    typedef u_int8_t uint8;
#endif /* anything else */
#endif /* _MSC_VER */

typedef uint8  binary;
//typedef unsigned char binary;

#if !defined(min)
#define min(x,y) ((x)<(y) ? (x) : (y))
#endif

#if !defined(max)
#define max(x,y) ((x)<(y) ? (y) : (x))
#endif

#ifdef __cplusplus
}
#endif

#endif /* M2W_T_H_INCLUDED_ */
