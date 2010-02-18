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
\file mpaframe.h
*/

#ifndef _M2W_FRAME_H_
#define _M2W_FRAME_H_

#include "Config.hpp"
#include "c_types.h"

START_M2W_NAMESPACE
/*
typedef enum {
MPEG_VERSION_1 = 0,
MPEG_VERSION_2,
MPEG_VERSION_25,
MPEG_VERSION_UNKNOWN,
Mpeg_Version_Size
} Mpeg_Version;

typedef enum {
MPEG_LAYER_1 = 0,
MPEG_LAYER_2,
MPEG_LAYER_3,
MPEG_LAYER_UNKNOWN,
Mpeg_Layer_Size
} Mpeg_Layer;

typedef enum {
MPEG_CHANNEL_STEREO = 0,
MPEG_CHANNEL_JOINT_STEREO,
MPEG_CHANNEL_DUAL_CHANNEL,
MPEG_CHANNEL_MONO,
Mpeg_Channel_Size
} Mpeg_Channel;

typedef enum {
MPEG_EMPH_NONE = 0,
MPEG_EMPH_50_15MS,
MPEG_EMPH_RESERVED,
MPEG_EMPH_CCITT_J17,
Mpeg_Emphasis_Size
} Mpeg_Emphasis;
*/
typedef enum {
	AC3_1_AND_1 = 0,
	AC3_CENTER,
	AC3_LEFT_RIGHT,
	AC3_LEFT_CENTER_RIGHT,
	AC3_LEFT_RIGHT_SURROUND,
	AC3_LEFT_CENTER_RIGHT_SURROUND,
	AC3_LEFT_SURROUND_RIGHT_SURROUND,
	AC3_CENTER_LEFT_SURROUND_RIGHT_SURROUND,
	AC3_Channel_Size
} Ac3_Channel;

class AC3_Frame {
public:
	AC3_Frame(); /// \todo initialise class variables
	bool Eval(const binary header[5]);
	bool IsValid() const {return bValid;}
	uint16 SampleSize() const {return 1536;}
	uint16 SamplingRate() const {return mySamplingRate;}
	uint16 DataSize() const;
	uint16 Channels() const;// {return (myChannels == MPEG_CHANNEL_MONO)?1:2;}
	bool HasCrc() const { return myCopyrightOn;}
	Ac3_Channel ChannelMode() const {return myChannels;}
	//	Mpeg_Layer Layer() const { return myLayer; }
	//	const Mpeg_Emphasis Emphasis() const {return myEmphasis;}
	uint16 BitRate() const {return myBitRate;}

	/*	bool Private() const {return myPrivateOn;}
	bool Copyright() const {return myCopyrightOn;}
	bool Original() const {return myOriginalOn;}
	bool Protected() const {return false;}
	*/
	inline operator const binary *() const
	{
		return myBinaryValue;
	}

	void setChannelMode(const Ac3_Channel value) {myChannels = value;}

	bool Compatible(const AC3_Frame & aFrame) const;
	/*
	bool pad() const { return myPaddingOn; }
	*/
protected:
	//	Mpeg_Layer    myLayer;
	Ac3_Channel   myChannels;
	//	bool          myCrcOn;
	int16         myBitRate;
	uint16        mySamplingRate;
	uint16        myCrc1;
	uint16        myCrc2;
	uint16        myFrameSize;
	bool          bValid;
	bool          myPaddingOn;
	bool          myPrivateOn;
	bool          myCopyrightOn;
	bool          myOriginalOn;
	//	Mpeg_Emphasis myEmphasis;

	binary        myBinaryValue[5];

	static const uint16 SamplingRates[4];
	static const int16  Bitrates[20];
};

END_M2W_NAMESPACE

#endif // _M2W_FRAME_H_
