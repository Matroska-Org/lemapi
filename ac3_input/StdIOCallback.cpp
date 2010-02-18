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
	\version \$Id: StdIOCallback.cpp,v 1.1 2003/07/21 15:08:18 jcsston Exp $
*/
#include "StdIOCallback.hpp"
#include "Config.hpp"

#include <cassert>
#include <sstream>
#include <climits>

using namespace std;

START_M2W_NAMESPACE

StdIOCallback::StdIOCallback(const char*Path, const open_mode aMode)
{
    assert(Path!=0);

    char *Mode;
    switch (aMode)
    {
	case mode_read:
	    Mode = "rb";
	    break;
	case mode_write:
	    Mode = "wb";
	    break;
	case mode_create:
	    Mode = "wb+";
	    break;
	default:
	    throw 0;
    }

    File=fopen(Path,Mode);
    if(File==0)
    {
	stringstream Msg;
	Msg<<"Can't open stdio file \""<<Path<<"\" in mode \""<<Mode<<"\"";
////	throw CRTError(Msg.str());
	throw Msg.str();
    }

//    M2W_TRACE("Successfully opened file \"%hs\" in mode \"%hs\". The handle is %p\n",Path,Mode,File);
}

StdIOCallback::~StdIOCallback()throw()
{
	try
	{
		close();
	}
	catch(std::exception&Ex)
	{
//		M2W_TRACE("Error in ~StdIOCallback: %hs\n",Ex.what());
	}
	catch(...)
	{
//		M2W_TRACE("Unknown error in ~StdIOCallback\n");
	}
}

uint32 StdIOCallback::read(binary*Buffer,size_t Size)
{
    assert(File!=0);

    size_t result = fread(Buffer, 1, Size, File);
    return result;
}

void StdIOCallback::setFilePointer(int64 Offset,seek_mode Mode)
{
	assert(File!=0);

	// There is a numeric cast in the boost library, which would be quite nice for this checking
/* 
	SL : replaced because unknown class in cygwin

	assert(Offset<=numeric_limits<long>::max());
	assert(Offset>=numeric_limits<long>::min());
*/
	assert(Offset <= LONG_MAX);
	assert(Offset >= LONG_MIN);

	assert(Mode==SEEK_CUR||Mode==SEEK_END||Mode==SEEK_SET);

	if(fseek(File,Offset,Mode)!=0)
	{
		ostringstream Msg;
		Msg<<"Failed to seek file "<<File<<" to offset "<<(unsigned long)Offset<<" in mode "<<Mode;
////		throw CRTError(Msg.str());
		throw Msg.str();
	}
}

size_t StdIOCallback::write(const void*Buffer,size_t Size)
{
	assert(File!=0);

	return fwrite(Buffer,1,Size,File);
}

uint64 StdIOCallback::getFilePointer()
{
	assert(File!=0);

	long Result=ftell(File);
	if(Result<0)
	{
		stringstream Msg;
		Msg<<"Can't tell the current file pointer position for "<<File;
////		throw CRTError(Msg.str());
		throw Msg.str();
	}

	return Result;
}

void StdIOCallback::close()
{
	if(File==0)
		return;

	if(fclose(File)!=0)
	{
		stringstream Msg;
		Msg<<"Can't close file "<<File;
////		throw CRTError(Msg.str());
		throw Msg.str();
	}

	File=0;
}

END_M2W_NAMESPACE
