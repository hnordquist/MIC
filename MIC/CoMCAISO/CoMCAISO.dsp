# Microsoft Developer Studio Project File - Name="CoMCAISO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CoMCAISO - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CoMCAISO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CoMCAISO.mak" CFG="CoMCAISO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CoMCAISO - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CoMCAISO - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/MIC 2.000", GJSAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CoMCAISO - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /I "../CommonSource" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib ws2_32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\CoMCAISO.dll
InputPath=.\Debug\CoMCAISO.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CoMCAISO - Win32 Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /I "../CommonSource" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 version.lib ws2_32.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\CoMCAISO.dll
InputPath=.\ReleaseMinDependency\CoMCAISO.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "CoMCAISO - Win32 Debug"
# Name "CoMCAISO - Win32 Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\CommonSource\ccbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\COM_Utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\CoMCAISO.cpp
# End Source File
# Begin Source File

SOURCE=.\CoMCAISO.def
# End Source File
# Begin Source File

SOURCE=.\CoMCAISO.idl
# ADD MTL /tlb ".\CoMCAISO.tlb" /h "CoMCAISO.h" /iid "CoMCAISO_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\CoMCAISO.rc
# End Source File
# Begin Source File

SOURCE=..\CommonSource\DirDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\GamNew.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\ISO.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\JulianT.cpp
# End Source File
# Begin Source File

SOURCE=.\MCACDData.cpp
# End Source File
# Begin Source File

SOURCE=.\MCAConfig2.cpp
# End Source File
# Begin Source File

SOURCE=.\MCADataStat.cpp
# End Source File
# Begin Source File

SOURCE=.\MCAGenlStat.cpp
# End Source File
# Begin Source File

SOURCE=.\MCAInst.cpp
# End Source File
# Begin Source File

SOURCE=.\MCAISO.cpp
# End Source File
# Begin Source File

SOURCE=.\MCALegend.cpp
# End Source File
# Begin Source File

SOURCE=.\MCAModParam.cpp
# End Source File
# Begin Source File

SOURCE=.\MCASafePS.cpp
# End Source File
# Begin Source File

SOURCE=.\MCASpectDataStat.cpp
# End Source File
# Begin Source File

SOURCE=.\MCAStateMachine.cpp
# End Source File
# Begin Source File

SOURCE=.\MCASummary.cpp
# End Source File
# Begin Source File

SOURCE=.\MCAUserProg.cpp
# End Source File
# Begin Source File

SOURCE=.\MCAXYCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\RegisterByCategory.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\CommonSource\TPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\VERSION.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\CommonSource\ccbutton.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\COM_Utilities.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\DirDialog.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\GamNew.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\ISO.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\ISO_definitions.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\JulianT.h
# End Source File
# Begin Source File

SOURCE=.\mcacddata.h
# End Source File
# Begin Source File

SOURCE=.\MCAConfig2.h
# End Source File
# Begin Source File

SOURCE=.\MCADataStat.h
# End Source File
# Begin Source File

SOURCE=.\MCAGenlStat.h
# End Source File
# Begin Source File

SOURCE=.\MCAInst.h
# End Source File
# Begin Source File

SOURCE=.\MCAISO.h
# End Source File
# Begin Source File

SOURCE=.\MCALegend.h
# End Source File
# Begin Source File

SOURCE=.\MCAModParam.h
# End Source File
# Begin Source File

SOURCE=.\MCASafePS.h
# End Source File
# Begin Source File

SOURCE=.\MCASpectDataStat.h
# End Source File
# Begin Source File

SOURCE=.\MCASummary.h
# End Source File
# Begin Source File

SOURCE=.\MCAUserProg.h
# End Source File
# Begin Source File

SOURCE=.\MCAXYCtrl.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\RegisterByCategory.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\TPropertySheet.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\VERSION.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bmp_red.bmp
# End Source File
# Begin Source File

SOURCE=.\res\clock.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00010.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00011.ico
# End Source File
# Begin Source File

SOURCE=.\MCAISO.rgs
# End Source File
# Begin Source File

SOURCE=.\res\mic.ico
# End Source File
# End Group
# End Target
# End Project
