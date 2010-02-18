/*
Copyright (C) 2002, Steve Lhomme <steve.lhomme @ free.fr>
This file is part of ac32wav.

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
\file mpiframe.h
*/

#ifndef _M2W_INFO_FRAME_H_
#define _M2W_INFO_FRAME_H_

#include "ac3frame.h"
#include "IOCallback.hpp"

START_M2W_NAMESPACE

class MPA_Info_Frame : public AC3_Frame {
public:
	MPA_Info_Frame() :bIsLame(false), totalBuffer(NULL) {}

	/// \brief read the frame data and parse them
	virtual bool ParseFrame(IOCallback & iStream);

protected:
	bool bIsLame;
	binary *totalBuffer;
};

class MPA_Lame_Frame : public MPA_Info_Frame {
public:
	MPA_Lame_Frame() {}

	bool ParseFrame(IOCallback & iStream);
};

END_M2W_NAMESPACE

#endif // _M2W_INFO_FRAME_H_
