/**
    \file
	
    A class for I/O into a file
*/

#ifndef _FIO_HPP_
#define _FIO_HPP_

#include <string>
#include <stdio.h>

#include "StdIOCallback.h"

const int FILE_MODE_BINARY = 0x0001;
const int FILE_MODE_READ   = 0x0002;
const int FILE_MODE_WRITE  = 0x0004;
const int FILE_MODE_ERASE  = 0x0008;
const int FILE_MODE_EXIST  = 0x0010;
const int FILE_MODE_APPEND = 0x0020;

class IO_file {
    public:
	IO_file();
	virtual ~IO_file();

	/// open the file for reading
	int Open(std::string & filename, int mode);
	int Close();

	int Read(void * buffer, size_t buffer_size);
	
  uint32 ReadUpTo(void * buffer, size_t buffer_size);
  int Write(const void * buffer, size_t buffer_size);

	inline bool Opened() const {return (h_file != NULL);}

	bool Seek(uint64 SeekPosition);
	bool SeekUp(uint64 SeekSize);

    protected:
	LIBEBML_NAMESPACE::StdIOCallback * h_file;
	std::string m_filename;
	unsigned int position;
};

#endif // _FIO_HPP_
