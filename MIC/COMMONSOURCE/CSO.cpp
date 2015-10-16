// CSO.cpp 

#include "stdafx.h"
#include "CSO.h"
#include "CSO_definitions.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CSO.cpp
////////////////////////////////////////////////
//
//            Constructor and destructor
//
////////////////////////////////////////////////
CCSO::CCSO()
{
//	TRACE("CCSO::CTOR\n");
	m_hReceiver = 0;
	m_hWatcher = 0;
	m_hOOB = 0;
	m_uIDC_RECEIVE = 0;
	m_uIDC_RECEIVE_OOB = 0;
 	m_uIDC_COM_START = 0;
	m_uIDC_COM_STOP = 0;
	m_uIDC_COM_RESET = 0;

	CVersionInfo VersionInfo;
	m_csVersion = VersionInfo.FileVersion();
	m_csVersion.Remove(' ');
	m_csVersion.Replace(',','.');
	CString csName = VersionInfo.InternalName();
	m_csNameVersionDate = csName + " " + m_csVersion + " " + __DATE__;

	//CVersionInfo VersionInfo;
	//CString Name    = VersionInfo.InternalName();
	//CString Version = VersionInfo.FileVersion();
	//Version.Remove(' ');
	//Version.Replace(',','.');
	//Version.Format("%c.%c.%c.%c",Version[0],Version[3],Version[6],Version[9]);
	//CString cs = 
}

CCSO::~CCSO()
{
//	TRACE("CCSO::DTOR NOOP\n");
}
////////////////////////////////////////////////
//
//            GetLastError
//
////////////////////////////////////////////////
CString * CCSO::GetLastError(){return &m_csLastError;}
////////////////////////////////////////////////
//
//            GetName
//
////////////////////////////////////////////////
char * CCSO::GetName(){return m_pName;}
////////////////////////////////////////////////
//
//            GetName
//
////////////////////////////////////////////////
char * CCSO::GetType(){return m_pType;}
////////////////////////////////////////////////
//
//            PutDialogCloseTime
//
////////////////////////////////////////////////
void CCSO::PutDialogCloseTime(int milliseconds)
{m_iDlgClose = milliseconds;}
////////////////////////////////////////////////
//
//            PutIniFileName
//
////////////////////////////////////////////////
void CCSO::PutIniFileName(char *inifile)
{strncpy(m_szIniFile,inifile,sizeof(m_szIniFile));}
////////////////////////////////////////////////
//
//            WatchDogSend
//
////////////////////////////////////////////////
void CCSO::WatchDogSend(){;}
////////////////////////////////////////////////
//
//            WatchDogOk
//
////////////////////////////////////////////////
void CCSO::WatchDogOk(bool set){m_bWatchDogOk = set;}
////////////////////////////////////////////////
//
//            SetThresholdValue
//
//  Only needed for DSPEC at this time...
//
////////////////////////////////////////////////
void CCSO::SetThresholdValue(UINT)
{
}
////////////////////////////////////////////////
//
//     SetWindowsRegisteredMessageStrings
//
////////////////////////////////////////////////
void CCSO::SetWindowsRegisteredMessageStrings(
	ULONG ulRECEIVE, 
	ULONG ulRECEIVE_OOB,
	ULONG ulSTART,
	ULONG ulSTOP,
	ULONG ulRESET)
{
//	TRACE("CCSO::SetWindowsRegisteredMessageStrings()\n");

	m_uIDC_RECEIVE         = (UINT)ulRECEIVE;
	m_uIDC_RECEIVE_OOB     = (UINT)ulRECEIVE_OOB;
	m_uIDC_COM_START       = (UINT)ulSTART;
	m_uIDC_COM_STOP        = (UINT)ulSTOP;
	m_uIDC_COM_RESET       = (UINT)ulRESET;

	SetLocalRegisteredMessageValues();
}