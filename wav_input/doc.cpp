/*
Copyright (C) 2002, Steve Lhomme <steve.lhomme @ free.fr>
This file is part of mpa2wav.

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
The KASWAV program suite is intended to be a command-line set of tools to convert
Matroska files to and from RIFF/WAV files.

These tools are written in C++ and use the libmatroska library to read/write Matroska content.
The RIFF/WAV reading/writing part was totally written from scratch using documentation
from the following places :

http://ccrma-www.stanford.edu/courses/422/projects/WaveFormat/	
http://www.cwi.nl/ftp/audio/RIFF-format
http://download.microsoft.com/download/platformsdk/mdrk/3.1/W31/EN-US/Mdrk.exe
http://www.oreilly.com/centers/gff/formats/micriff/

For broadcast WAV chunks
http://www.ebu.ch/pmc_bwf.html

*/