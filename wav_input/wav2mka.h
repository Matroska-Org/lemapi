#include <iostream>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "StdIOCallback.h"

#include "fio.hpp"

#include "Types.h"

const int SIZEOF_DWORD  = 4;
const int SIZEOF_WORD   = 2;

#include <sys/types.h>

#define wav2mka_app_name "wav2mka v0.1.4"
#if defined wxUSE_GUI
#include <wx/wx.h>
#endif

using namespace LIBEBML_NAMESPACE;

typedef struct {
    binary wFormatTag[2];
    uint16 nChannels;
    uint32 nSamplesPerSec;
    uint32 nAvgBytesPerSec;
    uint16 nBlockAlign;
    uint16 wBitsPerSample;
    uint16 cbSize;
} WAVEFORMATEX_;

const uint8 sizeof_WAVEFORMATEX = 18; // due to alignment problems

class FourCC {
    public:
	FourCC(char one=' ', char two=' ', char three=' ', char four=' ');
	/// The constructor will handle the endianess problems
	FourCC(void * buffer);
	~FourCC();

	bool operator==(const FourCC & other) const;
	bool operator!=(const FourCC & other) const;

	inline unsigned char c1() const {return val1;}
	inline unsigned char c2() const {return val2;}
	inline unsigned char c3() const {return val3;}
	inline unsigned char c4() const {return val4;}
    
    protected:
	unsigned char val1;
	unsigned char val2;
	unsigned char val3;
	unsigned char val4;
};

class RIFF_chunk {
    public:
	RIFF_chunk(const FourCC & aFourCC):header_sync(aFourCC) {}
	RIFF_chunk():header_sync(' ',' ',' ',' ') {}

	operator=(const RIFF_chunk & chunk);

	virtual ~RIFF_chunk() {}

	inline operator FourCC() const {return header_sync;}

	bool ReadHead(IO_file & file)
	{
	    binary buffer[4];
	    // read the fmt FourCC
	    if (!file.Read(buffer, 4))
	    {
		return false;
	    }

	    FourCC supposedFMT(buffer);
	    header_sync = supposedFMT;

	    if (!file.Read(buffer, 4))
	    {
		return false;
	    }
	    lil_uint32 lu32;
	    lu32.Eval(buffer);
	    myFollowingSize = lu32;

	    return true;
	}

	bool SkipData(IO_file & file)
	{
	    return file.SeekUp(myFollowingSize);
	}

	uint32 FollowingSize() const {return myFollowingSize;}

    protected:
        FourCC header_sync;
	uint32 myFollowingSize;
};

class FMT_chunk : public RIFF_chunk {
    public: 
//	FMT_chunk();
	FMT_chunk(const RIFF_chunk *aChunk);

	virtual ~FMT_chunk();

	bool Read(IO_file & file);

	uint32 SampleRate() const {return mySamplerate;}
	uint8 BitDepth() const {return uint8(myBitPerSample);}
	uint8 ChannelPerSubtrack() const {return uint8(myChannels);}
	uint16 BlockAlign() const {return myBlockAlign;}
	uint32 ByteRate() const {return myBytetrate;}
	WAVEFORMATEX_ *GetWAVEFORMATEX();
	static const FourCC fmtFourCC;

    protected:
	binary myFormat[2];
	uint16 myChannels;
	uint32 mySamplerate;
	uint32 myBytetrate;
	uint16 myBlockAlign;
	uint16 myBitPerSample;

	binary *ExtraCodecData;
	uint32 myExtraCodecDataSize;
};

class DATA_chunk : public RIFF_chunk {
    public: 
//	DATA_chunk();
	DATA_chunk(const RIFF_chunk *aChunk);

	virtual ~DATA_chunk() {}

	uint32 ReadData(IO_file & file, binary * Buffer, const uint32 SizeToRead);

	static const FourCC dataFourCC;
};


struct broadcast_audio_extension {
    char description[256]; /* ASCII : «Description of the sound sequence» */
    char originator[32]; /* ASCII : «Name of the originator» */
    char originatorreference[32]; /* ASCII : «Reference of the originator» */
    char originationdate[10]; /* ASCII : «yyyy-mm-dd» */
    char originationtime[8]; /* ASCII : «hh-mm-ss» */
    uint64 TimeReference; /* First sample count since midnight */
    uint32 Version; /* Version of the BWF; unsigned binary number */
    binary UMID[64]; /* Binary byte 0 of SMPTE UMID */
    binary reserved[188]; /* 190 bytes, reserved for future use, set to "NULL" (188 because of Wavelab) */
    char * codinghistory; /* ASCII : « History coding » */
};

class BEXT_chunk : public RIFF_chunk {
    public: 
//	BEXT_chunk();
	BEXT_chunk(const RIFF_chunk *aChunk);

	virtual ~BEXT_chunk();

	bool Read(IO_file & file);

	static const FourCC bextFourCC;

	inline operator broadcast_audio_extension() const {return myExtension;}

    protected:
	broadcast_audio_extension myExtension;
};

// LIST INFO chunks
class LISTI_IARL_chunk : public RIFF_chunk {
    public:
	static const FourCC gFourCC;
};

class LIST_Info : public RIFF_chunk {
    public:
	static const FourCC myFourCC;

	bool Read(IO_file & file);
};

class RIFF_structure {
    public:
//	bool FindNextChunk();
};

class LIST_chunk : public RIFF_chunk {
//class LIST_chunk : public RIFF_structure {
    public: 
	enum LIST_TYPE {
	    LIST_INFO,
	    LIST_UNKNOWN,
	}; 

	LIST_chunk(const RIFF_chunk *aChunk);

	virtual ~LIST_chunk() {}

	LIST_TYPE FindType(IO_file & file);

	static const FourCC listFourCC;
};

class RIFF_file : public RIFF_structure {
    public:
	enum RIFF_MODE {
	    RIFF_UNKNOWN,
	    RIFF_READ,
	    RIFF_WRITE,
	    RIFF_CREATE
	};

	RIFF_file(std::string & filename);
	virtual ~RIFF_file();
	
	virtual int ValidateHeader();

	bool FindNextChunk();

	IO_file & IOfile() {return m_file;}

	/// open the file for reading
	int Open(RIFF_MODE mode);
	    
    protected:
	std::string m_filename;
	IO_file m_file;
	RIFF_MODE m_mode;

	RIFF_chunk * myCurrChunk;

	bool passed_generalFourCC;
	static const FourCC general_FourCC;
	static const FourCC data_FourCC;
    
};

class WAV_file : public RIFF_file {
    public:
	enum ChunkTypes {
	    DATA_CHUNK,
	    FORMAT_CHUNK,
	    FACT_CHUNK,
	    BROADCAST_CHUNK,
	    LIST_CHUNK,
	    LIST_UNKNOWN_CHUNK,
	    UNKNOWN_CHUNK,
	};

	WAV_file(std::string & filename);
	virtual ~WAV_file();

	virtual int ValidateHeader();

	ChunkTypes ChunkType();
	RIFF_chunk * AcquireChunk() {
	    RIFF_chunk *tmp = myCurrChunk;
	    myCurrChunk = NULL;
	    return tmp;
	}

	void SetFormatChunk(FMT_chunk *aChunk) {myFmt = aChunk;}
	void SetDataChunk(DATA_chunk *aChunk) {myData = aChunk;}

	uint32 ReadData(binary * Buffer, const uint32 MaxReadSize);
	
	uint32 GetCurrentTimecode() const;// {return myTimeCode;}	
  //  protected:
	static const FourCC wave_FourCC;

	uint32    myFileSize;
	uint64    myReadData; ///< the size of already read data

//	RIFF_chunk * myCurrChunk;
	FMT_chunk  * myFmt;
	DATA_chunk * myData;
};