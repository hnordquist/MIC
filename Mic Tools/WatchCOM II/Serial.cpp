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
// Serial.cpp : implementation file
//

#include "stdafx.h"
#include "Serial.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSerial

IMPLEMENT_DYNCREATE(CSerial, CWinThread)

CSerial::CSerial()
{
	m_pReceiver = NULL;
	m_bDone = false;
	m_hComm = INVALID_HANDLE_VALUE;
	lpCommProp = NULL;
	lpdcb = NULL;
	m_dIndex = 0;
}

bool
CSerial::Register(CWnd * pReceiver)
{
	m_pReceiver = pReceiver;
	return true;
}

bool
CSerial::UnRegister(int node)
{
	m_pReceiver = NULL;
	return true;
}

BOOL
CSerial::EscapeCommFunction(DWORD dwFunction)
{
	if (m_hComm != INVALID_HANDLE_VALUE)
		return ::EscapeCommFunction(m_hComm, dwFunction);
	return 0;
}

bool
CSerial::SetUpPort(char* pPort, int dBaudRate, int dDataBits,
					 int dStopBits, char* pParity)
{
	unsigned BaudRate;
	unsigned DataBits;
	char temp[32];
	strcpy(temp,"\\\\.\\");
	strcat(temp,pPort);

	m_dBaudRate = dBaudRate;
	m_dDataBits = dDataBits;
	m_dStopBits = dStopBits;
	strcpy(m_pPort,pPort);
	strcpy(m_pParity,pParity);

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
//	"\\\\.\\COM10"
	//get the port
	if ((m_hComm = CreateFile(temp,//m_pPort,	//port name
		GENERIC_READ | GENERIC_WRITE, 		//read and write
		0,					//not shared (can't on com)
		NULL,					//default security attributes
		OPEN_EXISTING,				//open existing com port
		FILE_FLAG_OVERLAPPED,			//async i/o
		NULL))==INVALID_HANDLE_VALUE)		//template file -- not used on com
	{
		m_hComm = NULL;
		return false;
	}
	
	//configure the port
	//get the size of the actual commprop
	lpCommProp = (LPCOMMPROP)malloc(sizeof(COMMPROP));
	lpCommProp->wPacketLength = sizeof(COMMPROP);
	GetCommProperties(m_hComm, lpCommProp);
	unsigned short dwSize = lpCommProp->wPacketLength;
	free(lpCommProp);

	//get the actual commprop
	lpCommProp = (LPCOMMPROP)malloc(dwSize);
	lpCommProp->wPacketLength = dwSize;
	GetCommProperties(m_hComm, lpCommProp);
	
	//verify that we have a valid packet
	bool bHaveNT = false;
	if (lpCommProp->wPacketVersion == 2)
		bHaveNT = true;

	if ((lpCommProp->wPacketVersion != 0xffff) && //win95
		(lpCommProp->wPacketVersion != 2))		  //NT	
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
		if (lpCommProp->dwProvSubType != PST_RS232)
		{
			free(lpCommProp);
			lpCommProp = NULL;
			return false;
		}

	//verify we can have the baud rate we need
	if (lpCommProp->dwMaxBaud != BAUD_USER)		//user settable?
		if (lpCommProp->dwMaxBaud != BaudRate)	//nope-is it at what we need
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
//	lpdcb->fOutxCtsFlow = true;
//	lpdcb->fOutxDsrFlow = true;

	//set DTR on
	lpdcb->fDtrControl = DTR_CONTROL_ENABLE;

	//set xoff and xon to non-functioning state
	lpdcb->fOutX = false;
	lpdcb->fInX  = false;

	//set parity error replace char flag
	lpdcb->fErrorChar = (bHaveNT)?true:false;

	//make sure we can receive NULL bytes
	lpdcb->fNull = false;

	//no RTS flow control
	lpdcb->fRtsControl = RTS_CONTROL_ENABLE;

	//always abort on error
	lpdcb->fAbortOnError = true;

	//set data size
	if (lpCommProp->dwSettableParams & SP_DATABITS)
		lpdcb->ByteSize = m_dDataBits;

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

	BOOL Result = SetCommState(m_hComm, lpdcb);
	SetCommMask(m_hComm, EV_RXCHAR | EV_BREAK | EV_ERR );

	m_Overlap.Internal = 0;
	m_Overlap.InternalHigh = 0;
	m_Overlap.Offset = 0;
	m_Overlap.OffsetHigh = 0;
	m_Overlap.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	return Result?true:false;
}


CSerial::~CSerial()
{
	if (lpCommProp)
		free(lpCommProp);
	if (lpdcb)
		free(lpdcb);
}
void
CSerial::DoShutdown(WPARAM, LPARAM)
{
	ExitThread(0);
}

void
CSerial::DoSetMessage(int index)
{
	m_dIndex = index;
}

BOOL CSerial::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CSerial::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	//close comm port 
	if (m_hComm)
		CloseHandle(m_hComm);
	return CWinThread::ExitInstance();
}

void CSerial::DoStart(WPARAM WWord, LPARAM LWord)
{

	BYTE EndOfLine1 = 0x0a;
	BYTE EndOfLine2 = 0x0d;
	struct ReceiveStruct 
	{
		int DataLength;
		char* Data;
	};
	int At=0;
	BYTE inbuff[1024];
	DWORD nBytesRead, dwEvent, dwError;
	COMSTAT cs;
	m_bDone = false;
	//begin read loop here
	while (!m_bDone) {
		//hang reads here 
		if (WaitCommEvent(m_hComm, &dwEvent, NULL))
		{
			//clear the "error" flags. actually the char received
			ClearCommError(m_hComm, &dwError, &cs);
			//if the event out of waitcom says we have a char and there is one
			if ((dwEvent & EV_RXCHAR) && cs.cbInQue) 
			{
				//go get the number of characters the port says we have
				//up to the size of the buffer
				if (!ReadFile(m_hComm, inbuff, min(cs.cbInQue,sizeof(inbuff)),
					&nBytesRead, &m_Overlap)) 
				{
					//if for some reason we can't do a read of that many 
					//characters or any other error occurs on the read
					//then do a time-out dialog box and continue
					if (!m_bDone)
					{
						ClearCommError(m_hComm, &dwError, &cs);
//						MessageBox(m_hWnd,"Communications Error on ReadFile","Com Exe",MB_OK);
					}
				}
				else 
				{
					//if we actually got some characters to read
					if (nBytesRead)
					{
						//give all of the buffer to the virtual instrument.
						if (m_pReceiver)
						{
							char* Data = (char*)malloc(nBytesRead+1);
							memmove(Data,inbuff,nBytesRead);
							Data[nBytesRead] = NULL;
							m_pReceiver->PostMessage(IDC_RECEIVE+m_dIndex,nBytesRead,(LPARAM)Data);
						}
					}//end if (nBytesRead
				}//if (file read ELSE
			}//end if ((dwEvent

		}//end if (WaitCommEvent
		else
		{
			if (!m_bDone)
			{
				ClearCommError(m_hComm, &dwError, &cs);
//				MessageBox(m_hWnd,"Communications Error on WaitCommEvent",Com Ext",MB_OK);
			}

		}
	}//end while not done
	PurgeComm(m_hComm, PURGE_RXCLEAR);
}

void CSerial::DoStop()
{
	//flag dostart to exit now
	m_bDone = true;
	if (m_hComm)
	{
		PurgeComm(m_hComm,PURGE_RXCLEAR);
		PurgeComm(m_hComm,PURGE_TXCLEAR);
		CloseHandle(m_hComm);
		CloseHandle(m_Overlap.hEvent);
		WaitForSingleObject(m_hComm,1000);
	}
	m_hComm = NULL;
}

unsigned long CSerial::Transmit(int Length, char* Output)
{
	//output Output to file\com.
	unsigned long BytesWritten;
	//send the command otherwise
	WriteFile (m_hComm, Output, (Length==ISZSTRING)?strlen(Output):Length,
		&BytesWritten, &m_Overlap);
	return BytesWritten;
}

BEGIN_MESSAGE_MAP(CSerial, CWinThread)
	//{{AFX_MSG_MAP(CSerial)
	ON_THREAD_MESSAGE(IDC_COM_START,DoStart)
	ON_THREAD_MESSAGE(IDC_COM_STOP,DoShutdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerial message handlers
