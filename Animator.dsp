# Microsoft Developer Studio Project File - Name="modeler" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=modeler - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Animator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Animator.mak" CFG="modeler - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "modeler - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "modeler - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "modeler - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../fltk-1.0.10" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "SAMPLE_SOLUTION" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 opengl32.lib glu32.lib wsock32.lib fltk.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /pdbtype:sept /libpath:"../fltk-1.0.10/lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "modeler - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "local/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "SAMPLE_SOLUTION" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib wsock32.lib local/lib/fltk.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtb" /nodefaultlib:"msvcrt" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "modeler - Win32 Release"
# Name "modeler - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Curves"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\curve.cpp
# End Source File
# Begin Source File

SOURCE=.\curveevaluator.cpp
# End Source File
# Begin Source File

SOURCE=.\linearcurveevaluator.cpp
# End Source File
# End Group
# Begin Group "Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\modelerapp.cpp
# End Source File
# Begin Source File

SOURCE=.\modelerdraw.cpp
# End Source File
# Begin Source File

SOURCE=.\modelerui.cpp
# End Source File
# Begin Source File

SOURCE=.\modeleruiwindows.cxx
# End Source File
# Begin Source File

SOURCE=.\modelerview.cpp
# End Source File
# Begin Source File

SOURCE=.\robotarm.cpp
# End Source File
# End Group
# Begin Group "Particles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\particle.cpp
# End Source File
# Begin Source File

SOURCE=.\particleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\point.cpp
# End Source File
# Begin Source File

SOURCE=.\pointObj.cpp
# End Source File
# End Group
# Begin Group "UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\graphwidget.cpp
# End Source File
# Begin Source File

SOURCE=.\indicatorwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\rect.cpp
# End Source File
# Begin Source File

SOURCE=.\rulerwindow.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Curves."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\curve.h
# End Source File
# Begin Source File

SOURCE=.\curveevaluator.h
# End Source File
# Begin Source File

SOURCE=.\linearcurveevaluator.h
# End Source File
# End Group
# Begin Group "Model."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\modelerdraw.h
# End Source File
# Begin Source File

SOURCE=.\modelerui.h
# End Source File
# Begin Source File

SOURCE=.\modeleruiwindows.h
# End Source File
# Begin Source File

SOURCE=.\modelerview.h
# End Source File
# End Group
# Begin Group "Particles."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitmap.h
# End Source File
# Begin Source File

SOURCE=.\particle.h
# End Source File
# Begin Source File

SOURCE=.\particleSystem.h
# End Source File
# Begin Source File

SOURCE=.\point.h
# End Source File
# Begin Source File

SOURCE=.\pointObj.h
# End Source File
# End Group
# Begin Group "UI."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\graphwidget.h
# End Source File
# Begin Source File

SOURCE=.\indicatorwindow.h
# End Source File
# Begin Source File

SOURCE=.\modelerapp.h
# End Source File
# Begin Source File

SOURCE=.\rect.h
# End Source File
# Begin Source File

SOURCE=.\rulerwindow.h
# End Source File
# End Group
# Begin Group "Math."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mat.h
# End Source File
# Begin Source File

SOURCE=.\vec.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\cleanskel.pl
# End Source File
# Begin Source File

SOURCE=.\modeleruiwindows.fl
# End Source File
# Begin Source File

SOURCE=.\skelrenamer.pl
# End Source File
# End Group
# End Target
# End Project
