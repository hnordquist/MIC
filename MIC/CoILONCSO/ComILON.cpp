/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
// ComILON.cpp : implementation file

#include "stdafx.h"
#include "CSO_definitions.h"
#include "INI_definitions.h"
#include "ComILON.h"
#include "Resource.h"
#include "TimedMessageDialog.h"
#include "NewILONComm.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
	
#define ASYNC_IO
#define CLOSEPORT "CLOSEPORT"
#define OPENPORT "OPENPORT"

//windows registered message values
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CComILON

IMPLEMENT_DYNCREATE(CComILON, CWinThread)

CComILON::CComILON()
{
	m_dCurrentState = 1000;	//startup state
	m_dMasterAt = 0;		//current insertion point in input buffer
	m_bAutoDelete = TRUE;	//set the CWinThread to automatically delete
	m_bDone = false;		//receive loop will continue until done is flaged
	m_bInLocalBreak = false;//used for timing local break
	memset(NodeInfo,NULL,sizeof(NodeInfo));//zero out the node info structure
	m_dLastLongBreak = 0;
	m_bWatchDogOk = true;
	m_bWatchDogParticipate = false;
	m_dSendMaxLength = 100;
	// initialized to allow longbreak at startup
	m_dAllowLocalLongBreakCount = LOCALLONGBREAKHOLDOFFCOUNT;

	m_hComm = NULL;
	lpCommProp = NULL;
	lpdcb = NULL;
	m_bLastDUMPBBMBinary = false;
	m_bStopped = false;

	strncpy(m_pType, ILON_DISCOVERY_TYPE, sizeof(m_pType));
}


CComILON::~CComILON()
{
//	TRACE("CComILON::DTOR\n");
	if (lpCommProp)free(lpCommProp);
	if (lpdcb)	free(lpdcb);
}

BEGIN_MESSAGE_MAP(CComILON, CWinThread)
	//{{AFX_MSG_MAP(CComILON)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_START,DoStart)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_STOP,DoShutdown)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_RESET,DoReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL  CComILON::InitInstance()
{
//	TRACE("CComILON::InitInstance() - NOOP\n");
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

//register the node receiver
bool CComILON::Register(
	HWND receiver_handle, 
	bool out_of_band,
	int inode)
{
	if (!out_of_band && (inode < MAX_NODE_ARRAY))
	{
		if (NodeInfo[inode].hReceiver == NULL)
		{
			NodeInfo[inode].hReceiver = receiver_handle;
			return true;
		}
	}
	else  //out of band
	{
		inode &= 0x0000ffff;

		if ((inode < MAX_NODE_ARRAY) && (NodeInfo[inode].hOOB == NULL))
		{
			NodeInfo[inode].hOOB = receiver_handle;
			return true;
		}
	}
	m_csLastError = CString("Register::Node already registered");
	return false;
}

//register a node watcher for a specific node
bool CComILON::RegisterWatch(HWND watcher_handle, int inode)
{
	//is this one empty
	if (NodeInfo[inode].hWatcher == NULL)
	{
		//yes, so use it
		NodeInfo[inode].hWatcher = watcher_handle;
		return true;
	}
	m_csLastError = CString("RegisterWatch::Watcher already using this port");
	return false;
}

void CComILON::SetLocalRegisteredMessageValues()
{
//	TRACE("CComILON::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}

bool CComILON::SetUpPort(char* name)
{
	unsigned BaudRate;
	unsigned DataBits;
	strcpy(m_pName,name);

	//get port info from ini file
	char temp[32];
	GetPrivateProfileString(name,LONGBREAK,"NO",temp,sizeof(temp),m_szIniFile);
	strupr(temp);
	if ((strcmp(temp,"YES")==0) || (strcmp(temp,"1")==0))
		m_bLongBreak = true;
	else
		m_bLongBreak = false;
	m_dBaudRate = GetPrivateProfileInt(name,BAUDRATE,9600,m_szIniFile);
	m_dDataBits = GetPrivateProfileInt(name,DATABITS,7,m_szIniFile);
	m_dStopBits = GetPrivateProfileInt(name,STOPBITS,1,m_szIniFile);
	GetPrivateProfileString(name,PARITY,"EVEN",m_pParity,sizeof(m_pParity),m_szIniFile);
	GetPrivateProfileString(name,PORT,"ERROR",m_pPort,sizeof(m_pPort),m_szIniFile);
	m_bWatchDogParticipate = GetPrivateProfileInt(name,WATCHDOG,0,m_szIniFile)==1;
	m_dWatchDogNode = GetPrivateProfileInt(name,WATCHDOGPORT,1,m_szIniFile);

//	m_bDoDumpFile = GetPrivateProfileInt(name,"DODUMP",0,m_szIniFile);

	//necessay to resolve ports com10 and above
	//but also works on com1 - com9
	strcpy(temp,"\\\\.\\");
	strcat(temp,m_pPort);

	//convert baudrate to "internal" form
	switch (m_dBaudRate) {
	case 300:	BaudRate = BAUD_300;	break;
	case 600:	BaudRate = BAUD_600;	break;
	case 1200:	BaudRate = BAUD_1200;	break;
	case 1800:	BaudRate = BAUD_1800;	break;
	case 2400:	BaudRate = BAUD_2400;	break;
	case 4800:	BaudRate = BAUD_4800;	break;
	case 7200:	BaudRate = BAUD_7200;	break;
	case 9600:	BaudRate = BAUD_9600;	break;
	case 14400:	BaudRate = BAUD_14400;	break;
	case 19200:	BaudRate = BAUD_19200;	break;
	case 38400:	BaudRate = BAUD_38400;	break;
	case 56000:	BaudRate = BAUD_56K;	break;
	case 57600:	BaudRate = BAUD_57600;	break;
	case 115200:BaudRate = BAUD_115200;	break;
	case 128000:BaudRate = BAUD_128K;	break;
	default:
			BaudRate = BAUD_9600; 
	}

	//convert databits to "internal" form
	switch (m_dDataBits) {
	case 5:		DataBits = DATABITS_5;	break;
	case 6:		DataBits = DATABITS_6;	break;
	case 7:		DataBits = DATABITS_7;	break;
	case 8:		DataBits = DATABITS_8;	break;
	case 16:	DataBits = DATABITS_16;	break;
	default:
		DataBits = DATABITS_7;
	}

	//get the port
	if ((m_hComm = CreateFile(temp,//m_pPort,	//port name
		GENERIC_READ | GENERIC_WRITE, 	//read and write
		0,								//not shared (can't on com)
		NULL,							//default security attributes
		OPEN_EXISTING,					//open existing com port
		FILE_FLAG_OVERLAPPED,			//async i/o
		NULL))==INVALID_HANDLE_VALUE)	//template file -- not used on com
	{
		//AfxMessageBox("Fail on CreateFile");
		m_csLastError = CString("Fail on CreateFile");
		return false;
	}	
	//configure the port
	//get the size of the actual commprop
	lpCommProp = (LPCOMMPROP)malloc(sizeof(COMMPROP));
	lpCommProp->wPacketLength = sizeof(COMMPROP);
	if (GetCommProperties(m_hComm, lpCommProp)==0)
	{
		free(lpCommProp);
		lpCommProp = NULL;
		//AfxMessageBox("Fail on GetCommProperties 1");
		m_csLastError = CString("Fail on GetCommProperties 1");
		return false;
	}
	unsigned short dwSize = lpCommProp->wPacketLength;
	free(lpCommProp);

	//get the actual commprop
	lpCommProp = (LPCOMMPROP)malloc(dwSize);
	lpCommProp->wPacketLength = dwSize;
	if (GetCommProperties(m_hComm, lpCommProp)==0)
	{
		free(lpCommProp);
		lpCommProp = NULL;
		//AfxMessageBox("Fail on GetCommProperties 2");
		m_csLastError = CString("Fail on GetCommProperties 2");
		return false;
	}
	
	//verify that we have a valid packet
	if ((lpCommProp->wPacketVersion != 0xffff) && //win95
		(lpCommProp->wPacketVersion != 0x0002) && //NT		//scr00498	
		(lpCommProp->wPacketVersion != 0x0100))				//scr00498
	{
		free(lpCommProp);
		lpCommProp = NULL;
		//AfxMessageBox("Fail on packet version");	
		m_csLastError = CString("Fail on packet version");
		return false;
	}

	//verify that we have a serial port
	if (lpCommProp->dwServiceMask != SP_SERIALCOMM)
	{
		free(lpCommProp);
		lpCommProp = NULL;
		//AfxMessageBox("Fail on packet version");
		m_csLastError = CString("Fail on service mask");
		return false;
	}
	else
		if ((lpCommProp->dwProvSubType != PST_RS232) &&
			(lpCommProp->dwProvSubType != PST_UNSPECIFIED) &&	//scr00498
			(lpCommProp->dwProvSubType != PST_MODEM))			//scr00498
		{
			free(lpCommProp);
			lpCommProp = NULL;
			//AfxMessageBox("Fail on dwProvSubType");
			m_csLastError = CString("Fail on dwProvSubType");
			return false;
		}

	//verify we can have the baud rate we need
	if (lpCommProp->dwMaxBaud != BAUD_USER)		//user settable?
		if (lpCommProp->dwMaxBaud < BaudRate)	//nope-is it at what we need
		{
			free(lpCommProp);
			lpCommProp = NULL;
			//AfxMessageBox("Fail on dwMaxBaud");
			m_csLastError = CString("Fail on dwMaxBaud");
			return false;
		}

	//get device control block
	lpdcb = (DCB*)malloc(sizeof(DCB));
	if (GetCommState(m_hComm,lpdcb)==0)
	{
		free(lpCommProp);
		lpCommProp = NULL;
		//AfxMessageBox("Fail on GetCommState");
		m_csLastError = CString("Fail on GetCommState");
		return false;
	}

	//set baud rate
	if (lpCommProp->dwSettableParams & SP_BAUD)
		lpdcb->BaudRate = m_dBaudRate;

	//force binary
	lpdcb->fBinary = true;

	//set parity checking 
	if (lpCommProp->dwSettableParams & SP_PARITY_CHECK)
	{
		if (strcmp(m_pParity,"NONE")==0)
			lpdcb->fParity = false;
		else
			lpdcb->fParity = true;
	}

	//set clear to send and data set ready flow control off
	lpdcb->fOutxCtsFlow = false;
	lpdcb->fOutxDsrFlow = false;

	//set DTR on
	lpdcb->fDtrControl = DTR_CONTROL_ENABLE;

	//set xoff and xon to non-functioning state
	lpdcb->fOutX = false;
	lpdcb->fInX  = false;

	//set parity error replace char flag
	lpdcb->fErrorChar = true;

	//make sure we can receive NULL bytes
	lpdcb->fNull = false;

	//no RTS flow control
	lpdcb->fRtsControl = RTS_CONTROL_ENABLE;

	//always abort on error
	lpdcb->fAbortOnError = true;

	//set data size
	if (lpCommProp->dwSettableParams & SP_DATABITS)
		lpdcb->ByteSize = (unsigned char)m_dDataBits;

	//set parity
	if (strcmp(m_pParity,"EVEN")==0)
		lpdcb->Parity = EVENPARITY;
	else if (strcmp(m_pParity,"ODD")==0)
		lpdcb->Parity = ODDPARITY;
	else if (strcmp(m_pParity,"NONE")==0)
		lpdcb->Parity = NOPARITY;
	else if (strcmp(m_pParity,"MARK")==0)
		lpdcb->Parity = MARKPARITY;
	else
		lpdcb->Parity = EVENPARITY;

	//set stop bits
	switch (m_dStopBits) {
	case 1: lpdcb->StopBits = ONESTOPBIT; break;
	case 2: lpdcb->StopBits = TWOSTOPBITS;break;
	case 15:lpdcb->StopBits = ONE5STOPBITS;break;
	default:lpdcb->StopBits = ONESTOPBIT; break;
	}

	//set replacement char for parity errors
	lpdcb->ErrorChar = '#';

	//setup the port
	BOOL Result = SetCommState(m_hComm, lpdcb);
	if (!Result)
	{
		//AfxMessageBox("Fail on SetCommState");
		m_csLastError = CString("Fail on SetCommState");
	}
	SetCommMask(m_hComm, EV_RXCHAR | EV_BREAK | EV_ERR );

	m_OverlapR.Internal = 0;
	m_OverlapR.InternalHigh = 0;
	m_OverlapR.Offset = 0;
	m_OverlapR.OffsetHigh = 0;
	m_OverlapR.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_OverlapT.Internal = 0;
	m_OverlapT.InternalHigh = 0;
	m_OverlapT.Offset = 0;
	m_OverlapT.OffsetHigh = 0;
	m_OverlapT.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	return Result?true:false;
}

//<<QA>> are both of these "SetUpPort" functions needed???
//initialize the serial port from parameter information
bool CComILON::SetUpPort(
	char* szPort, 
	unsigned BaudRate, 
	unsigned DataBits, 
	unsigned StopBits, 
	char* szParity, 
	bool bLongBreak)
{

	//allow long break on this port?
	m_bLongBreak = bLongBreak;
	//get the baudrate, databits, stopbits, parity, 
	//and physical port from INI file
	m_dBaudRate = BaudRate;
	m_dDataBits = DataBits;
	m_dStopBits = StopBits;
	strcpy(m_pParity,szParity);
	strcpy(m_pPort,szPort);

	//convert baudrate to "internal" form
	switch (m_dBaudRate) {
	case 300:	BaudRate = BAUD_075;	break;
	case 600:	BaudRate = BAUD_600;	break;
	case 1200:	BaudRate = BAUD_1200;	break;
	case 1800:	BaudRate = BAUD_1800;	break;
	case 2400:	BaudRate = BAUD_2400;	break;
	case 4800:	BaudRate = BAUD_4800;	break;
	case 7200:	BaudRate = BAUD_7200;	break;
	case 9600:	BaudRate = BAUD_9600;	break;
	case 14400:	BaudRate = BAUD_14400;	break;
	case 19200:	BaudRate = BAUD_19200;	break;
	case 38400:	BaudRate = BAUD_38400;	break;
	case 56000:	BaudRate = BAUD_56K;	break;
	case 57600:	BaudRate = BAUD_57600;	break;
	case 115200:BaudRate = BAUD_115200;	break;
	case 128000:BaudRate = BAUD_128K;	break;
	default:
			BaudRate = BAUD_9600; 
	}

	//convert databits to "internal" form
	switch (m_dDataBits) {
	case 5:		DataBits = DATABITS_5;	break;
	case 6:		DataBits = DATABITS_6;	break;
	case 7:		DataBits = DATABITS_7;	break;
	case 8:		DataBits = DATABITS_8;	break;
	case 16:	DataBits = DATABITS_16;	break;
	default:
		DataBits = DATABITS_7;
	}

	//necessay to resolve ports com10 and above
	//but also works on com1 - com9
	char temp[MAX_PATH];
	strcpy(temp,"\\\\.\\");
	strcat(temp,m_pPort);

	//if we already have an open com close it first
	if (lpCommProp)
	{
		free(lpCommProp);
		lpCommProp = NULL;
	}
	if (lpdcb)
	{
		free(lpdcb);
		lpdcb = NULL;
	}

	if (m_hComm)
	{
		CloseHandle(m_hComm);
		Sleep(500);
	}
//	if (lpCommProp)
//		free(lpCommProp);
//	if (lpdcb)
//		free(lpdcb);

	//open the physical the port by name
	if ((m_hComm = CreateFile(temp,//m_pPort,	//port name
		GENERIC_READ | GENERIC_WRITE, 	//read and write
		0,								//not shared (can't on com)
		NULL,							//default security attributes
		OPEN_EXISTING,					//open existing com port
#ifdef ASYNC_IO
		FILE_FLAG_OVERLAPPED,			//async i/o
#else
		NULL,							//sync i/o
#endif
		NULL))==INVALID_HANDLE_VALUE)	//template file -- not used on com
		return false;
	
	//configure the port
	//get the size of the actual commprop packet
	//need to do this twice, once to get the size of COMMPROP
	//and then to get the actual data
	lpCommProp = (LPCOMMPROP)malloc(sizeof(COMMPROP));
	lpCommProp->wPacketLength = sizeof(COMMPROP);
	GetCommProperties(m_hComm, lpCommProp);
	unsigned short dwSize = lpCommProp->wPacketLength;
	free(lpCommProp);

	//get the actual commprop
	lpCommProp = (LPCOMMPROP)malloc(dwSize);
	lpCommProp->wPacketLength = dwSize;
	if (GetCommProperties(m_hComm, lpCommProp) == 0)		//scr00498
	{														//scr00498
		free(lpCommProp);									//scr00498
		lpCommProp = NULL;									//scr00498
		m_csLastError = CString("Fail on GetCommProperties 1");//scr00498
		return false;										//scr00498
	}														//scr00498
	
	//verify that we have a valid packet
	if ((lpCommProp->wPacketVersion != 0xffff) && //win95									
		(lpCommProp->wPacketVersion != 0x0002) && //NT		//scr00498
		(lpCommProp->wPacketVersion != 0x0100))				//scr00498
	{
		free(lpCommProp);
		lpCommProp = NULL;
		return false;
	}

	//verify that we have a serial port
	if (lpCommProp->dwServiceMask != SP_SERIALCOMM)
	{
		free(lpCommProp);
		lpCommProp = NULL;
		return false;
	}
	else
		if ((lpCommProp->dwProvSubType != PST_RS232) &&
			(lpCommProp->dwProvSubType != PST_UNSPECIFIED) &&	//scr00498
			(lpCommProp->dwProvSubType != PST_MODEM))			//scr00498
		{
			free(lpCommProp);
			lpCommProp = NULL;
			return false;
		}

	//verify we can have the baud rate we need
	if (lpCommProp->dwMaxBaud != BAUD_USER)		//user settable?
		if (lpCommProp->dwMaxBaud < BaudRate)	//nope-is it at what we need?
		{
			free(lpCommProp);
			lpCommProp = NULL;
			return false;
		}

	//get device control block
	lpdcb = (DCB*)malloc(sizeof(DCB));
	GetCommState(m_hComm,lpdcb);

	//set baud rate
	if (lpCommProp->dwSettableParams & SP_BAUD)

		lpdcb->BaudRate = m_dBaudRate;

	//force binary
	lpdcb->fBinary = true;

	//set parity checking 
	if (lpCommProp->dwSettableParams & SP_PARITY_CHECK)
	{
		if (strcmp(m_pParity,"NONE")==0)
			lpdcb->fParity = false;
		else
			lpdcb->fParity = true;
	}

	//set clear to send and data set ready flow control off
	lpdcb->fOutxCtsFlow = false;
	lpdcb->fOutxDsrFlow = false;

	//set DTR on
	lpdcb->fDtrControl = DTR_CONTROL_ENABLE;

	//set xoff and xon to non-functioning state
	lpdcb->fOutX = false;
	lpdcb->fInX  = false;

	//set parity error replace char flag
	lpdcb->fErrorChar = true;

	//make sure we can receive NULL bytes
	lpdcb->fNull = false;

	//no RTS flow control
	//this seems backwards -- but was needed 
	//to get the grands to talk
	lpdcb->fRtsControl = RTS_CONTROL_ENABLE;

	//always abort on error
	lpdcb->fAbortOnError = true;

	//set data size
	if (lpCommProp->dwSettableParams & SP_DATABITS)
		lpdcb->ByteSize = (unsigned char)m_dDataBits;

	//set parity
	if (strcmp(m_pParity,"EVEN")==0)
		lpdcb->Parity = EVENPARITY;
	else if (strcmp(m_pParity,"ODD")==0)
		lpdcb->Parity = ODDPARITY;
	else if (strcmp(m_pParity,"NONE")==0)
		lpdcb->Parity = NOPARITY;
	else if (strcmp(m_pParity,"MARK")==0)
		lpdcb->Parity = MARKPARITY;
	else
		lpdcb->Parity = EVENPARITY;

	//set stop bits
	switch (m_dStopBits) {
	case 1: lpdcb->StopBits = ONESTOPBIT; break;//one
	case 2: lpdcb->StopBits = TWOSTOPBITS;break;//two
	case 15:lpdcb->StopBits = ONE5STOPBITS;break;//one and a half
	default:lpdcb->StopBits = ONESTOPBIT; break;//one
	}

	//set replacement char for parity errors
	lpdcb->ErrorChar = '#';

	//set the port to what we want
	BOOL Result = SetCommState(m_hComm, lpdcb);

	//The SetCommMask function specifies a set of events 
	//to be monitored for a communications device
	SetCommMask(m_hComm, EV_RXCHAR | EV_BREAK | EV_ERR );

	//create an event object for the com to wait on
	m_OverlapR.Internal = 0;
	m_OverlapR.InternalHigh = 0;
	m_OverlapR.Offset = 0;
	m_OverlapR.OffsetHigh = 0;
	m_OverlapR.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_OverlapT.Internal = 0;
	m_OverlapT.InternalHigh = 0;
	m_OverlapT.Offset = 0;
	m_OverlapT.OffsetHigh = 0;
	m_OverlapT.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_bStopped = false;
	return Result?true:false;
}



void CComILON::DoShutdown(WPARAM, LPARAM)
{
//	TRACE("CComILON::DoShutdown()\n");

	if (lpCommProp)
	{
		free(lpCommProp);
		lpCommProp = NULL;
	}
	if (lpdcb)
	{
		free(lpdcb);
		lpdcb = NULL;
	}

	AfxEndThread(0);  //This calls the DTOR
}

int CComILON::ExitInstance()
{
//	TRACE("CComILON::ExitInstance\n");
	CloseHandle(m_hComm);
	return CCSO::ExitInstance();
}

//unregister the receiver
bool CComILON::UnRegister(int inode)
{
	if (!(inode & 0xffff0000))
	{
		if (NodeInfo[inode].hReceiver != NULL)
		{
			NodeInfo[inode].hReceiver = NULL;
			return true;
		}
	}
	else
	{
		inode &= 0x0000ffff;
		if (NodeInfo[inode].hOOB != NULL)
		{
			NodeInfo[inode].hOOB = NULL;
			return true;
		}
	}

	//not found return failure
	m_csLastError = CString("UnRegister::Node not registered");

	return false;
}

//unregister a node watcher
bool CComILON::UnRegisterWatch(int inode)
{
	if (NodeInfo[inode].hWatcher != NULL)
	{
		//remove the watcher
		NodeInfo[inode].hWatcher = NULL;
		//success
		return true;
	}
	//not found return failure
	m_csLastError = CString("UnRegisterWatch::Node not found");
	return false;
}
void CComILON::WatchDogSend()
{
	m_dAllowLocalLongBreakCount++;
	if (m_bWatchDogParticipate && m_bWatchDogOk)
	{
		short ShortNodeBigEndian;
		short ShortNode;
		short ShortLengthBigEndian;
		short ShortLength;
		unsigned long BytesWritten;
		BYTE Packet[6];
		if (m_dWatchDogNode < 1 || m_dWatchDogNode > 32565)
			return;
		ShortNode = (short)m_dWatchDogNode;
		swab((char*)&ShortNode,(char*)&ShortNodeBigEndian,2);
		ShortLength = 0;
		swab((char*)&ShortLength,(char*)&ShortLengthBigEndian,2);
		//send ok to watchdog ILON 
		Packet[0] = CONTROL_DATA;
		memcpy(&Packet[1],&ShortNodeBigEndian,2);
		memcpy(&Packet[3],&ShortLengthBigEndian,2);
		Packet[5] = MIC_OK;
		//data cannot exceed 210 bytes, header is 10 bytes
		if (!m_bInLocalBreak && !m_bStopped)
		{
			WriteFile (m_hComm, Packet, 6, &BytesWritten, &m_OverlapT);
			//send to watcher if there is one

//			for (int i = 0; i < MAX_NODE_ARRAY; i++)
//			{
//				if  (
//					(NodeInfo[i].NodeID  == m_dWatchDogNode) && 
//					(NodeInfo[i].Watcher != NULL)  &&
//					(m_dWatchDogNode != 0)
//					)
				if ((NodeInfo[m_dWatchDogNode].hWatcher != NULL) && (m_dWatchDogNode != 0))
				{
					COleDateTime Now = COleDateTime::GetCurrentTime();
					char tempstr[64];
					strcpy(tempstr,Now.Format("%Y,%m,%d %H:%M:%S Watchdog Sent\n"));

					//<<QA>> original pre-MIC_GM code
					//char* xData = (char*)malloc(strlen(tempstr));
					//memcpy(xData,tempstr,strlen(tempstr));
					//NodeInfo[m_dWatchDogNode].Watcher->PostMessage(IDC_RECEIVE,strlen(tempstr),(LPARAM)xData);
					
					// begin new MIC_GM code
					int length = strlen(tempstr);  //do not include the terminating NULL.
					char *xData = (char*)CoTaskMemAlloc((ULONG)length);
					if (xData != 0)
					{
						memcpy(xData,tempstr,length);

						::PostMessage(
							NodeInfo[m_dWatchDogNode].hWatcher, 
							m_uIDC_RECEIVE,
							WPARAM(length),
							LPARAM(xData));
					}
					//end new MIC_GM code

//					break;
				}
//			}

			//send to node 0 watcher
			if (NodeInfo[0].hWatcher)
			{
				char tempstr[8];
				itoa(m_dWatchDogNode,tempstr,10);
				strcat(tempstr,"=");
				int length = strlen(tempstr);
				//char* xData = (char*)malloc(9+length);
				char *xData = (char *)CoTaskMemAlloc((ULONG)(length + 9));  //MIC_GM

				if (xData != 0)
				{
					xData[0] = '\n';
					xData[1] = '<';
					xData[2] = Packet[0];
					strncpy(&xData[3],tempstr,length);
					memcpy(&xData[3+length],&Packet[1],5);
					xData[8+length] = '\n';
					//NodeInfo[0].Watcher->PostMessage(IDC_RECEIVE,9+length,(LPARAM)xData);
					::PostMessage(
							NodeInfo[0].hWatcher, 
							m_uIDC_RECEIVE,
							WPARAM(length + 9),
							LPARAM(xData));
				}
			}
		}
	}
	if (m_bStopped)
		SendToWatch(12,"Comm STOPPED",m_iStopPort);
}


//Receive loop
//run until done 
void CComILON::DoStart(WPARAM /*WWord*/, LPARAM /*LWord*/)
{

	int j;
	char tempstr[8];

	if ((m_hComm == NULL) || (m_hComm == INVALID_HANDLE_VALUE))
		return;

	BYTE inbuff[COMILON_ACCUMULATOR_SIZE];
	BYTE tempbuff[COMILON_ACCUMULATOR_SIZE];
	BYTE bCommand = NULL;

	DWORD nBytesRead, dwEvent, dwError;
	int nBytesInMessage = 0;
	char bStatus = NULL;
	COMSTAT cs;
	//start NOT done
	m_bDone = false;
	//start receiving data at the beginning of the buffer
	m_dLookBack = 0;
	//hang reads here 
	//begin read loop here
	while (!m_bDone) {
		if (WaitCommEvent(m_hComm, &dwEvent, NULL))
		{
			unsigned TotalBytes = 0;
			ClearCommError(m_hComm, &dwError, &cs);
			if ((dwEvent & EV_RXCHAR) && cs.cbInQue) 
			{
				while (TotalBytes < cs.cbInQue)
				{
					memset(inbuff,0xff,sizeof(inbuff));
					if (!ReadFile(m_hComm, inbuff, min(cs.cbInQue,sizeof(inbuff)),
						&nBytesRead, &m_OverlapR)) 
					{
						TotalBytes += nBytesRead;
						//don't want to do modal dialog box and hang further reads
						//because we are running UN-ATTENDED there isn't much we
						//can do if there is a read error on the port -- other than
						//to clear the error (done above) and keep trying
						ClearCommError(m_hComm, &dwError, &cs);

///////////////////////////////////////////////////////////////////////////////////////
//						if (nBytesRead)
//						{
//							FILE* outfile = fopen("C:\\DUMP.txt","at");
//	
//							for (unsigned i = 0; i < nBytesRead; i++)
//								fprintf(outfile,"%02x.",inbuff[i]);
//	
//							fprintf(outfile,"\n");
//	
//							fclose(outfile);
//						}
///////////////////////////////////////////////////////////////////////////////////////

					}
					else 
					{
						TotalBytes += nBytesRead;
						//did we get some characters from the serial port
						if (nBytesRead)
						{

///////////////////////////////////////////////////////////////////////////////////////
//							{
//							FILE* outfile = fopen("C:\\DUMP.txt","at");
//	
//							for (unsigned i = 0; i < nBytesRead; i++)
//								fprintf(outfile,"%02x ",inbuff[i]);
//	
//							fprintf(outfile,"\n");
//	
//							fclose(outfile);
//							}
///////////////////////////////////////////////////////////////////////////////////////

							//state machine for cracking ILON packet
							//run state machine for each character received
							//timer message
							//${00}{0x}{00}{06}J1<y><m><d><h><m><s>

							for (unsigned int i=0;i<nBytesRead;i++)
							{
								switch (m_dCurrentState)
								{
								case 1000: //get type byte of an ILON Packet
									//tempbuff[lookback] used for port 0 watcher buffer
									//and to look backwards for the terminator
									//sequence 0xdeaddead

									tempbuff[m_dLookBack = 0] = '>';
									tempbuff[++m_dLookBack] = inbuff[i];
									//get the ILON packet type indicator
									m_dCurrentType = inbuff[i];
									m_dCurrentState = 1100;
									//begin throwing away if not one of 
									//the two acceptable packet types
									if (
										(m_dCurrentType != CONTROL_RESPONSE) 
										&& (m_dCurrentType != PASSTHROUGH_DATA) 
									   )
										m_dCurrentState = 9000;
									break;
	
								case 1100: //get the second byte (01) of ILON Packet
									//get the high order byte of the source node
									m_dSourceNode = inbuff[i];
									m_dCurrentState = 1101;
									break;
	
								case 1101: //get the third byte (02) of ILON Packet
									//get the low order byte of the source node
									m_dSourceNode = m_dSourceNode * 256 + inbuff[i];
									itoa(m_dSourceNode,tempstr,10);
									for (j = 0; ((tempstr[j] != NULL)&&(j<8)); j++)
									{
										tempbuff[++m_dLookBack] = tempstr[j];
									}
									tempbuff[++m_dLookBack] = '=';
									tempbuff[++m_dLookBack] = (unsigned char)((m_dSourceNode >> 8) & 0x000000ff);
									tempbuff[++m_dLookBack] = inbuff[i];
									m_dCurrentState = 1200;
//									found = false;
									
									//throw away anything coming from node 0
									//only our local ILON should send a packet
									//to us marked as from node 0.  May need
									//to catch this when we start doing the
									//time sync stuff
									if (m_dSourceNode == 0)
									{
										m_dCurrentState = 9000;
										break;
									}
									if ((m_dSourceNode < MAX_NODE_ARRAY) && (m_dSourceNode >= 0))
									{
										m_hReceiver = NodeInfo[m_dSourceNode].hReceiver;
										m_hWatcher = NodeInfo[m_dSourceNode].hWatcher;
										m_hOOB = NodeInfo[m_dSourceNode].hOOB;
									}
	
									//protect ourselves in case we didn't have a known node id
									if (m_hOOB == NULL && m_hWatcher == NULL && m_hReceiver == NULL) 
									{
										m_hReceiver = NULL;
										m_hWatcher = NULL;
										m_hOOB = NULL;
										m_dCurrentState=9000;
									}

									//override bailout if it is a control response
									//e.g. timer message
									if (m_dCurrentType == CONTROL_RESPONSE)
										m_dCurrentState = 1200;
									break;

								case 1200:
									//get the first (high order) byte of data length
									nBytesInMessage = inbuff[i];
									m_dCurrentState = 1206;
									break;

								case 1206: 
									//get the second (low order) byte of data length
									tempbuff[++m_dLookBack]= inbuff[i];
									//get the second byte (low order) of data length
									//and build the number of bytes in message
									nBytesInMessage = (nBytesInMessage << 8) | inbuff[i];
									//place bytes at the start of buffer
									m_dMasterAt = 0;
									//if we have anything other than a passthrough
									//if we have a control response
									if (m_dCurrentType == CONTROL_RESPONSE) 
									{
										m_dCurrentState = 1207;
										break;
									}

									//should have been caught above in earlier state
									if (m_dCurrentType != PASSTHROUGH_DATA)
									{
										m_dCurrentState = 9000;
										break;
									}

									//validate data size of packet
									if (nBytesInMessage > MAX_DATAINPACKET)
									{
										m_dCurrentState = 9000;
										break;
									}
	
									//just in case
									if (nBytesInMessage <= 0)
									{
										m_dCurrentState = 9000;
										break;
									}
	
									//ok so continue with PASSTHROUGH packet
									m_dCurrentState = 1300;
									break;
	
								case 1207: //CONTROL_RESPONSE packet
									tempbuff[++m_dLookBack] = inbuff[i];
									bCommand = inbuff[i];
									if (bCommand == READ_CLOCK)  
									{
										if (nBytesInMessage == 6)
											m_dCurrentState = 8000;
										else
											m_dCurrentState = 9000;
										break;
									}
									else if ((bCommand == POLL)       || 
											 (bCommand == LONG_BREAK) ||
											 (bCommand == WRITE_CONFIGURATION) ||
											 (bCommand == SET_CLOCK)  ||
											 (bCommand == RESET_NODE) ||
											 (bCommand == CLEAR_ERR_LOG)
											)
										m_dCurrentState = 1250;
									else if (nBytesInMessage) 
										m_dCurrentState = 1210;
									else 
										m_dCurrentState = 9000;
									break;

								case 1210: //get status byte
									tempbuff[++m_dLookBack] = inbuff[i];
									bStatus = inbuff[i];
									m_dCurrentState = 1300;
									break;

								case 1250:
									//simple ACK or NAC with no data
									//set up for node 0 watcher
									tempbuff[++m_dLookBack] = inbuff[i];
									tempbuff[++m_dLookBack] = '\r';
									tempbuff[++m_dLookBack] = '\n';
									//build pseudo packet wrapper around response for ISO
									m_pMasterAccumulator[m_dMasterAt++] = inbuff[i];
									m_pMasterAccumulator[m_dMasterAt++] = 0x00;//type
									m_pMasterAccumulator[m_dMasterAt++] = bCommand;
									nBytesInMessage = 3;

									//give a copy to the watcher first
									//if there is a problem then we will get
									//to see it in the watcher window
									if (m_hWatcher)
									{
										//char* Data = (char*)malloc(9);
										char* Data = (char*)CoTaskMemAlloc((ULONG)9);
										if (Data != NULL)
										{
											if (inbuff[i] == 0x30)
												memcpy(Data,"NAK (0)\r\n",9);
											else
												memcpy(Data,"ACK (1)\r\n",9);

											//m_pWatcher->PostMessage(IDC_RECEIVE,
											//	(WPARAM)(9),(LPARAM)Data);
											::PostMessage(m_hWatcher,m_uIDC_RECEIVE,
												(WPARAM)(9),(LPARAM)Data);
										}
									}

									if (m_hOOB)
									{
										//char* Data = (char*)malloc(nBytesInMessage);
										char* Data = (char*)CoTaskMemAlloc((ULONG)nBytesInMessage);
										if (Data != NULL)
										{
											memcpy(Data,m_pMasterAccumulator,nBytesInMessage);

											//m_pOOB->PostMessage(IDC_RECEIVE,
											//	(WPARAM)nBytesInMessage,(LPARAM)Data);
											::PostMessage(
												m_hOOB,
												m_uIDC_RECEIVE,
												(WPARAM)nBytesInMessage,
												(LPARAM)Data);
										}

									}

									//give all of the master buffer 
									//to the message nominated receiver
									if (m_hReceiver)
									{
										m_dAllowLocalLongBreakCount = 0;
										char* Data;
										if (inbuff[i] == '1')
										{
											strcpy(m_pMasterAccumulator," ILON appears ok--ACK received.");
											nBytesInMessage = strlen(m_pMasterAccumulator);
										}
										else if (inbuff[i] == '0')
										{
											strcpy(m_pMasterAccumulator," ILON problem--NAK received.");
											nBytesInMessage = strlen(m_pMasterAccumulator);
										}
										
										//Data = (char*)malloc(nBytesInMessage);
										Data = (char*)CoTaskMemAlloc((ULONG)nBytesInMessage);
										memcpy(Data,m_pMasterAccumulator,nBytesInMessage);
										//send out of band message to receiver
										//m_pReceiver->PostMessage(IDC_RECEIVE_OOB,
										//	(WPARAM)nBytesInMessage,(LPARAM)Data);
										::PostMessage(m_hReceiver,m_uIDC_RECEIVE_OOB,
											(WPARAM)nBytesInMessage,(LPARAM)Data);
									}

									if (NodeInfo[0].hWatcher)
									{
										tempbuff[++m_dLookBack] = '\n';
										tempbuff[++m_dLookBack] = '\r';
										//char* Data = (char*)malloc(m_dLookBack);
										char* Data = (char*)CoTaskMemAlloc((ULONG)m_dLookBack);
										memcpy(Data,tempbuff,m_dLookBack);
										//NodeInfo[0].Watcher->PostMessage(
										//	IDC_RECEIVE,
										//	(WPARAM)m_dLookBack,
										//	(LPARAM)Data);
										::PostMessage(NodeInfo[0].hWatcher,
											m_uIDC_RECEIVE,
											(WPARAM)m_dLookBack,
											(LPARAM)Data);
									}
									m_dLookBack = 0;
									m_dTerminus = 0;
									m_dCurrentState = 1000;
									break;

								case 1300: 
									//data bytes of ILON packet
									//save for port 0 watcher and sentinal check
									tempbuff[++m_dLookBack]= inbuff[i];
	
									//accumulate the data in the Master Accumulator
									m_pMasterAccumulator[m_dMasterAt++] = inbuff[i];
									//if we have received all the bytes
									//from the ILON packet's data
									if (m_dMasterAt >= nBytesInMessage) 
									{
										if (m_dCurrentType == CONTROL_RESPONSE)
										{
											for (int i = m_dMasterAt; i > 0; i--)
												m_pMasterAccumulator[i] = m_pMasterAccumulator[i-1];
											m_pMasterAccumulator[0] = bStatus;
											nBytesInMessage++;
										}
#ifdef SENTINAL_REC					
										//pull off the sentinal
										m_dCurrentState = 9000;
#else
										// or go get another packet
										m_dCurrentState = 1000;
#endif
										//give a copy to the watcher first
										//if there is a problem then we will get
										//to see it in the watcher window
										if (m_hWatcher)
										{
											if (nBytesInMessage > 0)
											{
												char* Data;
												if (m_dCurrentType == CONTROL_RESPONSE)
												{
													//Data = (char*)malloc(nBytesInMessage+2);
													Data = (char*)CoTaskMemAlloc((ULONG)(nBytesInMessage+2));
													if (Data != NULL)
													{
														memcpy(Data,m_pMasterAccumulator,
															nBytesInMessage);
														Data[nBytesInMessage] = '\r';
														Data[nBytesInMessage+1] = '\n';
														//m_pWatcher->PostMessage(IDC_RECEIVE,
														//	(WPARAM)(nBytesInMessage+2),(LPARAM)Data);
														::PostMessage(m_hWatcher,m_uIDC_RECEIVE,
															(WPARAM)(nBytesInMessage+2),(LPARAM)Data);
													}
												}
												else
												{
													//Data = (char*)malloc(nBytesInMessage);
													Data = (char*)CoTaskMemAlloc((ULONG)nBytesInMessage);
													if (Data != NULL)
													{
														memcpy(Data,m_pMasterAccumulator,
															nBytesInMessage);
														//m_pWatcher->PostMessage(IDC_RECEIVE,
														//	(WPARAM)(nBytesInMessage),(LPARAM)Data);
														::PostMessage(m_hWatcher,m_uIDC_RECEIVE,
															(WPARAM)(nBytesInMessage),(LPARAM)Data);
													}
												}
											}
										}

										if (m_hOOB && (m_dCurrentType == CONTROL_RESPONSE))
										{
											if (nBytesInMessage > 0)
											{
												//char* Data = (char*)malloc(nBytesInMessage);
												char *Data = (char*)CoTaskMemAlloc((ULONG)nBytesInMessage);
												if (Data != NULL)
												{
													memcpy(Data,m_pMasterAccumulator,
														nBytesInMessage);
													//m_pOOB->PostMessage(IDC_RECEIVE,
													//	(WPARAM)(nBytesInMessage),(LPARAM)Data);
													::PostMessage(m_hOOB,m_uIDC_RECEIVE,
														(WPARAM)(nBytesInMessage),(LPARAM)Data);
												}
											}
										}

										//give all of the master buffer 
										//to the message nominated receiver
										if (m_hReceiver && (nBytesInMessage > 0))
										{
											m_dAllowLocalLongBreakCount = 0;
											char* Data;
#ifdef COM_LOOP_BACK_MODE_MESSAGE
											if (strncmp(m_szLastSend,m_pMasterAccumulator,nBytesInMessage)==0)
											{
												char szTemp[64];
												strcpy(szTemp," Possible COM Loop-back mode detected.");
												//Data = (char*)malloc(strlen(szTemp)+1);
												Data = (char*)CoTaskMemAlloc(strlen(szTemp)+1);
												if (Data != NULL)
												{
													strcpy(Data,szTemp);
													//m_pReceiver->PostMessage(IDC_RECEIVE_OOB,
													//	(WPARAM)strlen(szTemp)+1,(LPARAM)Data);
													::PostMessage(m_hReceiver,m_uIDC_RECEIVE_OOB,
														(WPARAM)strlen(szTemp)+1,(LPARAM)Data);
												}
											}
#endif
											if (m_dCurrentType == CONTROL_RESPONSE)
											{
												if (m_pMasterAccumulator[0] == '1')
												{
													strcpy(m_pMasterAccumulator," ILON appears ok--ACK received.");
													nBytesInMessage = strlen(m_pMasterAccumulator);
												}
												else if (m_pMasterAccumulator[0] == '0')
												{
													strcpy(m_pMasterAccumulator," ILON problem--NAK received.");
													nBytesInMessage = strlen(m_pMasterAccumulator);
												}
							
											}

											//Data = (char*)malloc(nBytesInMessage);
											Data = (char *)CoTaskMemAlloc(nBytesInMessage);
											if (Data != NULL)
											{
												memcpy(Data,m_pMasterAccumulator,nBytesInMessage);
												//if it was control response then send out of band
												//message to receiver

												if (m_dCurrentType == CONTROL_RESPONSE)
												{
													//m_pReceiver->PostMessage(IDC_RECEIVE_OOB,
													//	(WPARAM)nBytesInMessage,(LPARAM)Data);
													::PostMessage(m_hReceiver, m_uIDC_RECEIVE_OOB,
														(WPARAM)nBytesInMessage,(LPARAM)Data);
												}
												else
												{
													//else send it to the Instrument Support Object
													//m_pReceiver->PostMessage(IDC_RECEIVE,
													//	(WPARAM)nBytesInMessage,(LPARAM)Data);
													::PostMessage(m_hReceiver,m_uIDC_RECEIVE,
														(WPARAM)nBytesInMessage,(LPARAM)Data);
												}
											}
										}
									}//end if (m_dMasterAt >= nBytesInMessage)
	
									//m_dCurrentState is still 1300; 
									//stay here until we drop to state 9000
									//or back to 1000 if no sentinal on receive
									break;

								case 8000: //CONTROL_RESPONSE packet with read clock
									//get the ack (0x31); 
									tempbuff[++m_dLookBack] = inbuff[i];
									m_dCurrentState = 8010;
									break;
								case 8010: //get year
									tempbuff[++m_dLookBack] = inbuff[i];
									m_dYear = (int)inbuff[i];
									if (m_dYear >= 52)
										m_dYear += 1900;
									else
										m_dYear += 2000;
									m_dCurrentState = 8020;
									break;
								case 8020: //get month
									tempbuff[++m_dLookBack] = inbuff[i];
									m_dMonth = (int)inbuff[i];
									m_dCurrentState = 8030;
									break;
								case 8030: //get day
									tempbuff[++m_dLookBack] = inbuff[i];
									m_dDay = (int)inbuff[i];
									m_dCurrentState = 8040;
									break;
								case 8040: //get hour
									tempbuff[++m_dLookBack] = inbuff[i];
									m_dHour = (int)inbuff[i];
									m_dCurrentState = 8050;
									break;
								case 8050: //get minute
									tempbuff[++m_dLookBack] = inbuff[i];
									m_dMinute = (int)inbuff[i];
									m_dCurrentState = 8060;
									break;
								case 8060: //get second
									tempbuff[++m_dLookBack] = inbuff[i];
									m_dSecond = (int)inbuff[i];

									//create coletime for now (local time)
									Now = COleDateTime::GetCurrentTime();

									//create coletime for message
									Msg.SetDateTime(m_dYear,m_dMonth,m_dDay,m_dHour,m_dMinute,m_dSecond);
									//tell the ini file what the two times are
									{
										CString tmpBuff;
										//don't change this -- event is looking for this string to
										//differentiate between data and true OOB information.
										tmpBuff.Format("OOB TIME SET - COLLECTs TIME:%s MESSAGEs TIME:%s ",Now.Format("%Y,%m,%d %H:%M:%S"),Msg.Format("%Y,%m,%d %H:%M:%S"));
										WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME_COMMAND",tmpBuff,m_szIniFile);

										//char* szData = (char*)malloc(tmpBuff.GetLength()+1);
										//strcpy(szData,tmpBuff);
										//m_pReceiver->PostMessage(IDC_RECEIVE_OOB,
										//	(WPARAM)strlen(szData)+1,(LPARAM)szData);

										//figure difference
										Span = Msg - Now;
										double Seconds;
										Seconds = fabs(Span.GetTotalSeconds());

										//if difference is > 2 seconds and < 5 minutes
										int iMin = GetPrivateProfileInt(m_pName,"MIN_TIME_SET",2,m_szIniFile);
										int iMax = GetPrivateProfileInt(m_pName,"MAX_TIME_SET",300,m_szIniFile);
										if (Seconds > iMin && Seconds < iMax)
										{
											//then reset the system clock
											SYSTEMTIME SysTime;
											GetSystemTime(&SysTime);
											Now.SetDateTime(
												SysTime.wYear,
												SysTime.wMonth,
												SysTime.wDay,
												SysTime.wHour,
												SysTime.wMinute,
												SysTime.wSecond);
											Now = Now + Span;
											SysTime.wYear	= (unsigned short)(Now.GetYear());		
											SysTime.wMonth	= (unsigned short)(Now.GetMonth());
											SysTime.wDay	= (unsigned short)(Now.GetDay());
											SysTime.wHour	= (unsigned short)(Now.GetHour());
											SysTime.wMinute = (unsigned short)(Now.GetMinute());
											SysTime.wSecond = (unsigned short)(Now.GetSecond());
																		
											if (SetSystemTime(&SysTime))
											{
												WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME",Now.Format("%Y,%m,%d %H:%M:%S Done"),m_szIniFile);
												tmpBuff += "Done";
											}
											else
											{
												WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME",Now.Format("%Y,%m,%d %H:%M:%S Failed"),m_szIniFile);
												tmpBuff += "FAILED";
											}
										}
										else
										{
											if (Seconds >= 300) 
											{
												WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME",Now.Format("%Y,%m,%d %H:%M:%S Time Diff. Too Large"),m_szIniFile);
												tmpBuff += "Not Done (time diff. too large)";
											}
											else
											{
												WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME",Now.Format("%Y,%m,%d %H:%M:%S Not Required"),m_szIniFile);
												tmpBuff += "Not Required";
											}
										}

										//send tmpBuff to all instruments
										int iCount = 0;
										for (int iInstrument = 0 ; iInstrument < MAX_NODE_ARRAY; iInstrument++)
										{
											if (NodeInfo[iInstrument].hOOB != NULL)
											{
												iCount++;
												//get memory
												//char* szData = (char*)malloc(tmpBuff.GetLength()+1);
												char *szData = (char*)CoTaskMemAlloc(tmpBuff.GetLength()+1);
												if (szData != NULL)
												{
													//fill memory
													strcpy(szData,LPCSTR(tmpBuff));
													szData[tmpBuff.GetLength()] = NULL;
													//post message
													//NodeInfo[iInstrument].OOB->PostMessage(IDC_RECEIVE_OOB,
													//	(WPARAM)strlen(szData)+1,(LPARAM)szData);
													::PostMessage(
														NodeInfo[iInstrument].hOOB, m_uIDC_RECEIVE_OOB,
														(WPARAM)strlen(szData)+1,(LPARAM)szData);
												}
											}
											//<<QA>> Need to clarify this for MIC_GM.  Is it needed?
											//if (iCount >= g_dCount)
											//	iInstrument = MAX_NODE_ARRAY;
										}


										/*
										for (int iInstrument = 0; iInstrument < g_dCount; iInstrument++)
										{
											if (g_pInstruments[iInstrument] != NULL)
											{
												//get memory
												char* szData = (char*)malloc(tmpBuff.GetLength()+1);
												//fill memory
												strcpy(szData,LPCSTR(tmpBuff));
												szData[tmpBuff.GetLength()] = NULL;
												//post message
												g_pInstruments[iInstrument]->PostMessage(IDC_RECEIVE_OOB,
													(WPARAM)strlen(szData)+1,(LPARAM)szData);
											}
										}
										*/
									}
									
									m_dCurrentState = 9000;
									break;

								case 9000:
									m_dTerminus = ((m_dTerminus << 8) | inbuff[i]) & SENTINALMASK;
	
									tempbuff[++m_dLookBack] = '&';
									tempbuff[++m_dLookBack] = inbuff[i];
	
									//if we have sentinal then the next
									//character we receive should be the
									//start of another ILON packet
									if ((m_dTerminus == SENTINAL) || ((m_dTerminus & SENTINALMASKSHORT) == SENTINALSHORT))
									{
										if (NodeInfo[0].hWatcher)
										{
											tempbuff[++m_dLookBack] = '\n';
											tempbuff[++m_dLookBack] = '\r';
											//char* Data = (char*)malloc(m_dLookBack);
											char* Data = (char*)CoTaskMemAlloc(m_dLookBack);
											if (Data != NULL)
											{
												memcpy(Data,tempbuff,m_dLookBack);
												//NodeInfo[0].Watcher->PostMessage(
												//	IDC_RECEIVE,
												//	(WPARAM)m_dLookBack,
												//	(LPARAM)Data);
												::PostMessage(NodeInfo[0].hWatcher,
													m_uIDC_RECEIVE,
													(WPARAM)m_dLookBack,
													(LPARAM)Data);
											}
										}
										m_dLookBack = 0;
										m_dTerminus = 0;
										m_dCurrentState = 1000;
									}
									break;

								}//end state switch
							}//end for i = 0 to number of bytes read 
						}//end if number of bytes read
					}//end if !readfile else
				}//end while
			}//end if dwEvent

		}//end if (WaitCommEvent
		else
		{
			//because we are running UN-ATTENDED there isn't much we
			//can do if there is a comm error on the wait -- other than
			//try to keep running
			ClearCommError(m_hComm, &dwError, &cs);
		}
	}//end while not done
	PurgeComm(m_hComm, PURGE_RXCLEAR);
}


void CComILON::DoStop()
{
	//flag dostart to exit now
	m_bDone = true;
	//close handle will also trigger a waitcommevent error
	CloseHandle(m_hComm);
}

void CComILON::DoReset(WPARAM /*WWord*/, LPARAM LWord)
{
//	TRACE("CComILON::DoReset\n");

	if (lpCommProp)
	{
		free(lpCommProp);
		lpCommProp = NULL;
	}
	if (lpdcb)
	{
		free(lpdcb);
		lpdcb = NULL;
	}
	//wait a couple of seconds
	//before we attempt to reestablish 
	//the connection -- this is safe 'cause 
	//doreset runs in this thread 
	Sleep(2000);
		//reset the port and restart
	if (SetUpPort(m_pName))
		{
			//::PostThreadMessage(m_nThreadID,IDC_COM_START,0,0);
			::PostThreadMessage(m_nThreadID,m_uIDC_COM_START,0,0);

			SendToWatch(14,"Comm RESTARTED",LWord);
			m_bStopped = false;
		}
	else
		{
			m_hComm = NULL;
			SendToWatch(19,"Comm RESTART FAILED",LWord);
		}
}

//called to send a buffer of data to the port
int CComILON::Transmit(int Length, char* Output, int Node, bool Quiet)
{
	//output Output to file\com.
	unsigned long BytesWritten = 0;
	char tempstr[32];
	BYTE Packet[256];
	int Size=0;
	short ShortNodeBigEndian;
	short ShortLengthBigEndian;
	bool LongBreakHoldOff = false;
	int TempNode = Node;
	char *At;
	bool bDoControlPacket = false;

	Node &= 0x0000ffff;
	if (Node != TempNode)
		bDoControlPacket = true;

	//build multibyte node as big endian
	short ShortNode = (short)Node;
	swab((char*)&ShortNode,(char*)&ShortNodeBigEndian,2);
	
	Length = (Length==ISZSTRING)?strlen(Output):Length;

	//save the last send command
	strncpy(m_szLastSend,Output,min(Length,sizeof(m_szLastSend)-1));
	m_szLastSend[min(Length,sizeof(m_szLastSend)-1)] = NULL;

	//ILON network requires a valid node id between 0 and 65519 (ffef)
	if ((Node < 0)||(Node > 65519)) return 0;

	At = Output;
	while (Length > m_dSendMaxLength)
	{
		//build data length as big endian
		short ShortLength = (short)m_dSendMaxLength;
		swab((char*)&ShortLength,(char*)&ShortLengthBigEndian,2);
		
		//clear the packet header
		memset(Packet, NULL, sizeof(Packet));
		//set the type 
		Packet[0] = PASSTHROUGH_DATA;
		//set the destination node
		memcpy(&Packet[1],&ShortNodeBigEndian,2);

		//get the data itself
		memcpy(&Packet[5],At,ShortLength);
	
		//size is the full packet length
		Size = 5 + ShortLength;
	
		//save the size in the data size 
		memcpy(&Packet[3],&ShortLengthBigEndian,2);

		if (!m_bInLocalBreak)
		{
//#ifdef SENTINAL_TRANS
//			//append the sentenal
//			int Sentinal = SENTINAL;
//			memcpy(&Packet[Size],&Sentinal,sizeof(Sentinal));
//	
//			//adjust size for sentenal
//			Size += sizeof(Sentinal);
//#endif
			//data cannot exceed m_dSendMaxLength bytes, header is 10 bytes
			WriteFile(m_hComm, Packet, Size, &BytesWritten, &m_OverlapT);
		
			//send to node watcher
			if (!Quiet)
			{
				if ((NodeInfo[Node].hWatcher != NULL) && (Node != 0))
				{
					if (strcmp(At,"DUMPBBM")==0)
						m_bLastDUMPBBMBinary = false;
					else if (strcmp(At,"DUMPBBBM")==0)
						m_bLastDUMPBBMBinary = true;
					if ((strcmp(At,"DUMPOK")==0) && m_bLastDUMPBBMBinary)
					{
						//char*Data = (char*)malloc(2);
						char* Data = (char*)CoTaskMemAlloc(2);
						if (Data != NULL)
						{
							Data[0] = '\n';
							Data[1] = NULL;
							//NodeInfo[Node].Watcher->PostMessage(IDC_RECEIVE,2,(LPARAM)Data);
							::PostMessage(NodeInfo[Node].hWatcher,m_uIDC_RECEIVE,2,(LPARAM)Data);
						}
					}
					//char* Data = (char*)malloc(m_dSendMaxLength+2);
					char* Data = (char*)CoTaskMemAlloc(m_dSendMaxLength+2);
					if (Data != NULL)
					{
						memcpy(Data,At,m_dSendMaxLength);
						Data[m_dSendMaxLength] = '\n';
						Data[m_dSendMaxLength+1] = NULL;											
						//NodeInfo[Node].Watcher->PostMessage(IDC_RECEIVE,m_dSendMaxLength+2,(LPARAM)Data);
						::PostMessage(NodeInfo[Node].hWatcher,m_uIDC_RECEIVE,m_dSendMaxLength+2,(LPARAM)Data);
					}
					break;//out of for loop
				}
			}

			//send to node 0 watcher
			if (NodeInfo[0].hWatcher)
			{
				if (strcmp(At,"DUMPBBM")==0)
					m_bLastDUMPBBMBinary = false;
				else if (strcmp(At,"DUMPBBBM")==0)
					m_bLastDUMPBBMBinary = true;
				if ((strcmp(At,"DUMPOK")==0) && m_bLastDUMPBBMBinary)
				{
					//char*Data = (char*)malloc(2);
					char* Data = (char*)CoTaskMemAlloc(2);
					if (Data != NULL)
					{
						Data[0] = '\n';
						Data[1] = NULL;
						//NodeInfo[0].Watcher->PostMessage(IDC_RECEIVE,2,(LPARAM)Data);
						::PostMessage(NodeInfo[0].hWatcher,m_uIDC_RECEIVE,2,(LPARAM)Data);
					}
				}
				itoa(Node,tempstr,10);
				strcat(tempstr,"=");
				int length = strlen(tempstr);
				//char* xData = (char*)malloc(Size+3+length);
				char* xData = (char*)CoTaskMemAlloc(Size+3+length);
				if (xData != NULL)
				{
					xData[0] = '\n';
					xData[1] = '<';
					xData[2] = Packet[0];
					strncpy(&xData[3],tempstr,length);
					memcpy(&xData[length+3],&Packet[1],Size-1);
					xData[Size+2+length] = '\n';
					//NodeInfo[0].Watcher->PostMessage(IDC_RECEIVE,Size+3+length,(LPARAM)xData);
					::PostMessage(NodeInfo[0].hWatcher,m_uIDC_RECEIVE,Size+3+length,(LPARAM)xData);
				}
			}
		}

		At += m_dSendMaxLength;
		Length -= m_dSendMaxLength;
	}

	if (Length > 0)
	{
		//build data length as big endian
		short ShortLength = (short)((Length==ISZSTRING)?strlen(Output):Length);
		swab((char*)&ShortLength,(char*)&ShortLengthBigEndian,2);

		//do we need to do a LOCAL long break
		LongBreakHoldOff = false;
		if (strncmp(Output, LOCALLONGBREAK,14)==0)
		{
			if (m_dAllowLocalLongBreakCount >= LOCALLONGBREAKHOLDOFFCOUNT)
			{
				if (m_bLongBreak)
				{
					DWORD TickCount = GetTickCount() - m_dLastLongBreak;
					//don't allow local long breaks
					//faster than five minutes apart
					if ((TickCount < 0) || (TickCount > 300000) || (m_dLastLongBreak == 0))//five minutes
					{
						m_bInLocalBreak = true;
						SetCommBreak(m_hComm);
						memcpy(&Packet[0]," LOCALLONGBREAK ",16);
						Size = 16;
						m_dLastLongBreak = GetTickCount();
					}
					else
					{
						memcpy(&Packet[0]," LOCALLONGBREAK Held Off ",25);
						LongBreakHoldOff = true;
						Size = 25;
					}
				}
			}
			else
			{
				memcpy(&Packet[0]," LOCALLONGBREAK Not Allowed ",28);
				LongBreakHoldOff = true;
				Size = 28;
			}
		}
		//do we need to send a REMOTE long break
		else if (strncmp(Output,CLOSEPORT,9)==0)
		{
			DoStop();
			SendToWatch(12,"Comm STOPPED",Node);
			m_bStopped = true;
			m_iStopPort = Node;
		}
		else if (strncmp(Output,OPENPORT,8)==0)
		{
			::PostThreadMessage(m_nThreadID,m_uIDC_COM_RESET,0,Node);
		}
		else if (strncmp(Output,LONGBREAKSET,12)==0)
		{
			if (!m_bLongBreak) return 0;
			ShortLength = 0;
			ShortLengthBigEndian = 0;
			memset(Packet, NULL, 10);
			Packet[0] = CONTROL_DATA;
			memcpy(&Packet[1],&ShortNodeBigEndian,2);
			Packet[5] = POLL;
			Size = 6;
			//send the poll command so we know if ILON is ok or not
			//we'll get an ACK back from this command
			WriteFile(m_hComm, Packet, Size, &BytesWritten, &m_OverlapT);
			//finish set up to send long break
			Packet[5] = LONG_BREAK;
		} 
		//get com status at the dist end
		else if (strncmp(Output,COMSTATUSDE,11)==0)
		{
			if (!m_bLongBreak) return 0;
			ShortLength = 0;
			ShortLengthBigEndian = 0;
			memset(Packet, NULL, 10);
			Packet[0] = CONTROL_DATA;
			memcpy(&Packet[1],&ShortNodeBigEndian,2);
			Packet[5] = POLL;
			Size = 6;
		}
		else if (strncmp(Output,COMSTATUSLE,11)==0)
		{
			TempNode = 0;
			ShortLength = 0;
			ShortLengthBigEndian = 0;
			memset(Packet, NULL, 10);
			Packet[0] = CONTROL_DATA;
			ShortNodeBigEndian = 0;
			memcpy(&Packet[1],&ShortNodeBigEndian,2);
			//leave the source at  at 0 to get the local 
			//memcpy(&Packet[1],&ShortNodeBigEndian,2);
			Packet[5] = POLL;
			Size = 6;
		}
		//do we need to "unset" a long break
		else if (strncmp(Output,LONGBREAKCLR,12)==0)
		{
			Size = 0;
			if (m_bInLocalBreak)
			{
				ClearCommBreak(m_hComm);
				memcpy(&Packet[0]," LONGBREAKCLR",14);
				Size = 14;
			}
			else
			{
				return 0;
			}
		}
		//any other control packet
		else if (bDoControlPacket)
		{
			//clear the packet header
			memset(Packet, NULL, 100);
			//set the type
			Packet[0] = CONTROL_DATA;
			//set the destination node
			memcpy(&Packet[1],&ShortNodeBigEndian,2);
			//set the data itself
			memcpy(&Packet[5],At,ShortLength);
			//size is the full packet length
			ShortLength--;
			Size = 6 + ShortLength;
			//adjust for special "collect" command
			//if (At[0] == 0x54) || At[0] == 0x49)
			swab((char*)&ShortLength,(char*)&ShortLengthBigEndian,2);
			//save the size in the packet data size 
			memcpy(&Packet[3],&ShortLengthBigEndian,2);
			//set the subcommand in the packet
		}
		//then we must have a packet to send
		else
		{
			//clear the packet header
			memset(Packet, NULL, 100);
			//set the type 
			Packet[0] = PASSTHROUGH_DATA;
			//set the destination node
			memcpy(&Packet[1],&ShortNodeBigEndian,2);
	
			//get the data itself
			memcpy(&Packet[5],At,ShortLength);
	
			//size is the full packet length
			Size = 5 + ShortLength;
	
			//save the size in the data size 
			memcpy(&Packet[3],&ShortLengthBigEndian,2);

		}

		if (!m_bInLocalBreak)
		{
//#ifdef SENTINAL_TRANS
//			//append the sentenal
//			int Sentinal = SENTINAL;
//			memcpy(&Packet[Size],&Sentinal,sizeof(Sentinal));
//	
//			//adjust size for sentenal
//			Size += sizeof(Sentinal);
//#endif
			//data cannot exceed 210 bytes, header is 10 bytes
			if ((Size <= (MAX_DATAINPACKET+10)) && !m_bInLocalBreak)
			{
				WriteFile(m_hComm, Packet, Size, &BytesWritten, &m_OverlapT);
			}
		}

		if (strncmp(Output,LONGBREAKCLR,12)==0)
			m_bInLocalBreak = false;

		//send to node watcher
		if (!Quiet)
		{
			if ((NodeInfo[Node].hWatcher != NULL) && (Node != 0))
			{
				int CurLength = (Length==ISZSTRING)?strlen(Output):Length;
				if (Output[CurLength-1] != 0x0d)
					CurLength++;
				//char* Data = (char*)malloc((LongBreakHoldOff)?CurLength+9:CurLength);
				char* Data = (char*)CoTaskMemAlloc((LongBreakHoldOff)?CurLength+9:CurLength);
				if (Data != NULL)
				{
					memcpy(Data,Output,CurLength);
					if (LongBreakHoldOff)
						memcpy(&Data[CurLength],"Held off ",9);
					Data[(LongBreakHoldOff)?CurLength+8:CurLength-1] = '\n';
															
				//NodeInfo[Node].Watcher->PostMessage(IDC_RECEIVE,(LongBreakHoldOff)?CurLength+9:CurLength,(LPARAM)Data);
				::PostMessage(NodeInfo[Node].hWatcher,
					m_uIDC_RECEIVE,
					(LongBreakHoldOff)?CurLength+9:CurLength,
					(LPARAM)Data);
				}
			}
		}

		//send to node 0 watcher
		if (NodeInfo[0].hWatcher != NULL)
		{
			if(Size > 0)
			{
				itoa(Node,tempstr,10);
				strcat(tempstr,"=");
				int length = strlen(tempstr);
				//char* xData = (char*)malloc(Size+3+length);
				char* xData = (char*)CoTaskMemAlloc(Size+3+length);
				if (xData != NULL)
				{
					xData[0] = '\n';
					xData[1] = '<';
					xData[2] = Packet[0];
					strncpy(&xData[3],tempstr,length);
					memcpy(&xData[length+3],&Packet[1],Size-1);
					xData[Size+2+length] = '\n';
					//NodeInfo[0].Watcher->PostMessage(IDC_RECEIVE,Size+3+length,(LPARAM)xData);
					::PostMessage(NodeInfo[0].hWatcher,m_uIDC_RECEIVE,Size+3+length,(LPARAM)xData);
				}
			}
			else
			{
				//For local commands that are not getting transmitted
				//to an instrument.  Size == 0.

				int CurLength = (Length==ISZSTRING)?strlen(Output):Length;
				//char* Data = (char*)malloc(CurLength+1);
				char* Data = (char*)CoTaskMemAlloc(CurLength+1);
				if (Data != NULL)
				{
					memcpy(Data,Output,CurLength);
					Data[CurLength] = '\n';											
					//NodeInfo[0].Watcher->PostMessage(IDC_RECEIVE,CurLength+1,(LPARAM)Data);
					::PostMessage(NodeInfo[0].hWatcher,m_uIDC_RECEIVE,CurLength+1,(LPARAM)Data);
				}
			}
		}
	}

	if (LongBreakHoldOff)
		return -1;
	else
		return (int)BytesWritten;
}
//////////////////////////////////////////////////////////////////////////////
//
//                      CollectParametersForNewCSO
//
//////////////////////////////////////////////////////////////////////////////
bool CComILON::CollectParametersForNewCSO(CString& csNameOut)
{
//	TRACE("CComILON::CollectParametersForNewCSO()\n");

	//This CSO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new CSO from that data.

	//Collect the data for the new communications object.
	CNewILONComm dlg(m_szIniFile,m_iDlgClose,m_pName);
	int result = dlg.DoModal();
	if (result == IDOK)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(COMMUNICATIONS,m_pName,ILON_DISCOVERY_TYPE,m_szIniFile);
		return true;
	}
	else
	{
		csNameOut.Empty();
		return false;
	}
}


void CComILON::SendToWatch(int Length, char* Output, int Node)
{
	Node &= 0x0000ffff;
	if (NodeInfo[Node].hWatcher != NULL)
	{
		int dlength = (Length==ISZSTRING)?strlen(Output):Length;
		//char*Data = (char*)malloc(dlength+2);
		char*Data = (char*)CoTaskMemAlloc(dlength+2);
		if (Data != NULL)
		{
			memcpy(Data,Output,dlength);
			if (Data[0]==0x0d) Data[0]= NULL;
			Data[dlength] = 0x0a;
			Data[dlength+1]=NULL;
			//NodeInfo[Node].Watcher->PostMessage(IDC_RECEIVE,dlength+1,(LPARAM)Data);
			::PostMessage(NodeInfo[Node].hWatcher, m_uIDC_RECEIVE,dlength+1,(LPARAM)Data);
		}
	}
}


BOOL CComILON::EscapeCommFunction(DWORD dwFunction)
{
	if (m_hComm != INVALID_HANDLE_VALUE)
		return ::EscapeCommFunction(m_hComm, dwFunction);
	return 0;
}


void CComILON::WatchDogOk(bool set)
{
	m_bWatchDogOk = set;
}

