# Microsoft Developer Studio Project File - Name="mpa_input_plugin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mpa_input_plugin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mpa_input_plugin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mpa_input_plugin.mak" CFG="mpa_input_plugin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mpa_input_plugin - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mpa_input_plugin - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "mpa_input_plugin"
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mpa_input_plugin - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\..\libebml\ebml" /I "..\tinyXML" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /D "_MBCS" /YX /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\..\libebml\ebml" /I "..\tinyXML" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /D "_MBCS" /YX /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mpa_input_plugin - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Ob1 /Gy /I "..\..\libebml\ebml" /I "..\tinyXML" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /D "_MBCS" /GF PRECOMP_VC7_TOBEREMOVED /c
# ADD CPP /nologo /MT /W3 /GX /Ob1 /Gy /I "..\..\libebml\ebml" /I "..\tinyXML" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /D "_MBCS" /GF PRECOMP_VC7_TOBEREMOVED /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x0 /d "NDEBUG"
# ADD RSC /l 0x0 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "mpa_input_plugin - Win32 Debug"
# Name "mpa_input_plugin - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\mpa_input_plugin.cpp
DEP_CPP_MPA_I=\
	"..\..\libebml\ebml\api\c\libebml_t.h"\
	"..\..\libebml\ebml\EbmlConfig.h"\
	"..\..\libebml\ebml\EbmlEndian.h"\
	"..\..\libebml\ebml\EbmlTypes.h"\
	"..\..\libebml\ebml\IOCallback.h"\
	"..\..\libebml\ebml\StdIOCallback.h"\
	"..\tinyXML\tinystr.h"\
	"..\tinyXML\tinyxml.h"\
	".\c_types.h"\
	".\Config.hpp"\
	".\mpafile.h"\
	".\mpaframe.h"\
	".\mpiframe.h"\
	
# End Source File
# Begin Source File

SOURCE=.\mpa_input_plugin.def
# End Source File
# Begin Source File

SOURCE=mpafile.cpp
DEP_CPP_MPAFI=\
	"..\..\libebml\ebml\api\c\libebml_t.h"\
	"..\..\libebml\ebml\EbmlConfig.h"\
	"..\..\libebml\ebml\EbmlEndian.h"\
	"..\..\libebml\ebml\EbmlTypes.h"\
	"..\..\libebml\ebml\IOCallback.h"\
	"..\..\libebml\ebml\StdIOCallback.h"\
	".\c_types.h"\
	".\Config.hpp"\
	".\Endian.hpp"\
	".\mpafile.h"\
	".\mpaframe.h"\
	".\mpiframe.h"\
	".\Types.hpp"\
	
# End Source File
# Begin Source File

SOURCE=mpaframe.cpp
DEP_CPP_MPAFR=\
	".\c_types.h"\
	".\Config.hpp"\
	".\Endian.hpp"\
	".\mpaframe.h"\
	".\Types.hpp"\
	
# End Source File
# Begin Source File

SOURCE=mpiframe.cpp
DEP_CPP_MPIFR=\
	"..\..\libebml\ebml\api\c\libebml_t.h"\
	"..\..\libebml\ebml\EbmlConfig.h"\
	"..\..\libebml\ebml\EbmlEndian.h"\
	"..\..\libebml\ebml\EbmlTypes.h"\
	"..\..\libebml\ebml\IOCallback.h"\
	"..\..\libebml\ebml\StdIOCallback.h"\
	".\c_types.h"\
	".\Config.hpp"\
	".\mpaframe.h"\
	".\mpiframe.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=c_types.h
# End Source File
# Begin Source File

SOURCE=Config.hpp
# End Source File
# Begin Source File

SOURCE=Endian.hpp
# End Source File
# Begin Source File

SOURCE=mpafile.h
# End Source File
# Begin Source File

SOURCE=mpaframe.h
# End Source File
# Begin Source File

SOURCE=mpiframe.h
# End Source File
# Begin Source File

SOURCE=Types.hpp
# End Source File
# End Group
# Begin Group "tinyXML"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\tinyXML\tinystr.cpp
DEP_CPP_TINYS=\
	"..\tinyXML\tinystr.h"\
	"..\tinyXML\tinyxml.h"\
	
# End Source File
# Begin Source File

SOURCE=..\tinyXML\tinystr.h
# End Source File
# Begin Source File

SOURCE=..\tinyXML\tinyxml.cpp
DEP_CPP_TINYX=\
	"..\tinyXML\tinystr.h"\
	"..\tinyXML\tinyxml.h"\
	
# End Source File
# Begin Source File

SOURCE=..\tinyXML\tinyxml.h
# End Source File
# Begin Source File

SOURCE=..\tinyXML\tinyxmlerror.cpp
DEP_CPP_TINYXM=\
	"..\tinyXML\tinystr.h"\
	"..\tinyXML\tinyxml.h"\
	
# End Source File
# Begin Source File

SOURCE=..\tinyXML\tinyxmlparser.cpp
DEP_CPP_TINYXML=\
	"..\tinyXML\tinystr.h"\
	"..\tinyXML\tinyxml.h"\
	
# End Source File
# End Group
# End Target
# End Project
