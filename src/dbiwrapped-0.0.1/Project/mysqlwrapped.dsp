# Microsoft Developer Studio Project File - Name="mysqlwrapped" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mysqlwrapped - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mysqlwrapped.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mysqlwrapped.mak" CFG="mysqlwrapped - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mysqlwrapped - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mysqlwrapped - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mysqlwrapped - Win32 MT Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mysqlwrapped - Win32 MT Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mysqlwrapped - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\R"
# PROP Intermediate_Dir "R"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "c:\mysql\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\R\mysqlwrapped_ST.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\IError.h+..\StderrLog.h+..\SysLog.h+..\enum_t.h+..\set_t.h+..\Database.h+..\Query.h ..\..\include\libmysqlwrapped.h
# End Special Build Tool

!ELSEIF  "$(CFG)" == "mysqlwrapped - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\D"
# PROP Intermediate_Dir "D"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "c:\mysql\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\D\mysqlwrapped_ST.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\IError.h+..\StderrLog.h+..\SysLog.h+..\enum_t.h+..\set_t.h+..\Database.h+..\Query.h ..\..\include\libmysqlwrapped.h
# End Special Build Tool

!ELSEIF  "$(CFG)" == "mysqlwrapped - Win32 MT Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "ReleaseMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\R"
# PROP Intermediate_Dir "RMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "c:\mysql\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\IError.h+..\StderrLog.h+..\SysLog.h+..\enum_t.h+..\set_t.h+..\Database.h+..\Query.h ..\..\include\libmysqlwrapped.h
# End Special Build Tool

!ELSEIF  "$(CFG)" == "mysqlwrapped - Win32 MT Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "DebugMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\D"
# PROP Intermediate_Dir "DMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "c:\mysql\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\IError.h+..\StderrLog.h+..\SysLog.h+..\enum_t.h+..\set_t.h+..\Database.h+..\Query.h ..\..\include\libmysqlwrapped.h
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "mysqlwrapped - Win32 Release"
# Name "mysqlwrapped - Win32 Debug"
# Name "mysqlwrapped - Win32 MT Release"
# Name "mysqlwrapped - Win32 MT Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Database.cpp
# End Source File
# Begin Source File

SOURCE=..\enum_t.cpp
# End Source File
# Begin Source File

SOURCE=..\Query.cpp
# End Source File
# Begin Source File

SOURCE=..\set_t.cpp
# End Source File
# Begin Source File

SOURCE=..\StderrLog.cpp
# End Source File
# Begin Source File

SOURCE=..\SysLog.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Database.h
# End Source File
# Begin Source File

SOURCE=..\enum_t.h
# End Source File
# Begin Source File

SOURCE=..\IError.h
# End Source File
# Begin Source File

SOURCE=..\Query.h
# End Source File
# Begin Source File

SOURCE=..\set_t.h
# End Source File
# Begin Source File

SOURCE=..\StderrLog.h
# End Source File
# Begin Source File

SOURCE=..\SysLog.h
# End Source File
# End Group
# End Target
# End Project
