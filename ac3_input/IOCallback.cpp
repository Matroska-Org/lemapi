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
	\version \$Id: IOCallback.cpp,v 1.1 2003/07/21 15:08:18 jcsston Exp $
*/
//#include "StdInclude.hpp"
#include "IOCallback.hpp"

#include <sstream>
#include <stdexcept>

using namespace std;

START_M2W_NAMESPACE

void IOCallback::writeFully(const void*Buffer,size_t Size)
{
    if (Buffer == NULL)
	throw;
    if (Size == 0)
	throw;

    if(write(Buffer,Size) != Size)
    {
	stringstream Msg;
	Msg<<"EOF in writeFully("<<Buffer<<","<<Size<<")";
	throw runtime_error(Msg.str());
    }
}

void IOCallback::readFully(binary*Buffer,size_t Size)
{
    if(Buffer == NULL)
	throw;

    if(read(Buffer,Size) != Size)
    {
	stringstream Msg;
	Msg<<"EOF in readFully("<<Buffer<<","<<Size<<")";
	throw runtime_error(Msg.str());
    }
}

END_M2W_NAMESPACE
