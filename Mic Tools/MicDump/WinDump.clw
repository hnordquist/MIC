; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWinDumpDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "WinDump.h"
ODLFile=WinDump.odl

ClassCount=4
Class1=CWinDumpApp
Class2=CWinDumpDlg
Class3=CAboutDlg
Class4=CWinDumpDlgAutoProxy

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_WINDUMP_DIALOG

[CLS:CWinDumpApp]
Type=0
HeaderFile=WinDump.h
ImplementationFile=WinDump.cpp
Filter=N

[CLS:CWinDumpDlg]
Type=0
HeaderFile=WinDumpDlg.h
ImplementationFile=WinDumpDlg.cpp
Filter=D
LastObject=IDC_CHECK_GAPANALYSIS
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=WinDumpDlg.h
ImplementationFile=WinDumpDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[CLS:CWinDumpDlgAutoProxy]
Type=0
HeaderFile=DlgProxy.h
ImplementationFile=DlgProxy.cpp
BaseClass=CCmdTarget
Filter=N

[DLG:IDD_WINDUMP_DIALOG]
Type=1
Class=CWinDumpDlg
ControlCount=5
Control1=IDC_EDIT_LOG,edit,1352730692
Control2=IDC_STATIC,static,1342308353
Control3=IDC_CHECK_GAPANALYSIS,button,1342242819
Control4=IDC_CHECK_OPENOUTPUT,button,1342242819
Control5=IDC_EDIT_ACCEPTGAP,edit,1484857472

