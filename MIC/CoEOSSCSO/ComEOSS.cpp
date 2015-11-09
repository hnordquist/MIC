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
// ComEOSS.cpp : implementation file

#include "stdafx.h"
#include "CSO_definitions.h"
#include "INI_definitions.h"
#include "ComEOSS.h"
#include "Resource.h"
#include "TimedMessageDialog.h"
#include "NewEOSSComm.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
	
//windows registered message values
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CComEOSS

IMPLEMENT_DYNCREATE(CComEOSS, CWinThread)

CComEOSS::CComEOSS()
{
	m_bAutoDelete = TRUE;	//set the CWinThread to automatically delete
	m_bDone = false;		//receive loop will continue until done is flaged
	memset(&NodeInfo,NULL,sizeof(NodeInfo));//zero out the node info structure

	m_bStopped = false;

	strncpy(m_pType, EOSS_DISCOVERY_TYPE, sizeof(m_pType));
}


CComEOSS::~CComEOSS()
{
//	TRACE("CComEOSS::DTOR\n");
}

BEGIN_MESSAGE_MAP(CComEOSS, CWinThread)
	//{{AFX_MSG_MAP(CComEOSS)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_START,DoStart)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_STOP,DoShutdown)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_RESET,DoReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL  CComEOSS::InitInstance()
{
//	TRACE("CComEOSS::InitInstance() - NOOP\n");
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

//register the node receiver
bool CComEOSS::Register(
	HWND receiver_handle, 
	bool out_of_band,
	int inode)
{
	if (!out_of_band)
	{
		if (NodeInfo.hReceiver == NULL)
		{
			NodeInfo.hReceiver = receiver_handle;
			return true;
		}
	}
	else  //out of band
	{
		inode &= 0x0000ffff;

		if (NodeInfo.hOOB == NULL)
		{
			NodeInfo.hOOB = receiver_handle;
			return true;
		}
	}
	m_csLastError = CString("Register::Node already registered");
	return false;
}

//register a node watcher for a specific node
bool CComEOSS::RegisterWatch(HWND watcher_handle, int inode)
{
	//is this one empty
	if (NodeInfo.hWatcher == NULL)
	{
		//yes, so use it
		NodeInfo.hWatcher = watcher_handle;
		return true;
	}
	m_csLastError = CString("RegisterWatch::Watcher already using this port");
	return false;
}

void CComEOSS::SetLocalRegisteredMessageValues()
{
//	TRACE("CComEOSS::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}

bool CComEOSS::SetUpPort(char* name)
{
	m_bStopped = false;
	return true;
}

//<<QA>> are both of these "SetUpPort" functions needed???
//initialize the serial port from parameter information
bool CComEOSS::SetUpPort(
	char* szPort, 
	unsigned BaudRate, 
	unsigned DataBits, 
	unsigned StopBits, 
	char* szParity, 
	bool bLongBreak)
{
	m_bStopped = false;
	return true;
}



void CComEOSS::DoShutdown(WPARAM, LPARAM)
{
//	TRACE("CComEOSS::DoShutdown()\n");
	AfxEndThread(0);  //This calls the DTOR
}

int CComEOSS::ExitInstance()
{
//	TRACE("CComEOSS::ExitInstance\n");
	return CCSO::ExitInstance();
}

//unregister the receiver
bool CComEOSS::UnRegister(int inode)
{
	if (!(inode & 0xffff0000))
	{
		if (NodeInfo.hReceiver != NULL)
		{
			NodeInfo.hReceiver = NULL;
			return true;
		}
	}
	else
	{
		inode &= 0x0000ffff;
		if (NodeInfo.hOOB != NULL)
		{
			NodeInfo.hOOB = NULL;
			return true;
		}
	}

	//not found return failure
	m_csLastError = CString("UnRegister::Node not registered");

	return false;
}

//unregister a node watcher
bool CComEOSS::UnRegisterWatch(int inode)
{
	if (NodeInfo.hWatcher != NULL)
	{
		//remove the watcher
		NodeInfo.hWatcher = NULL;
		//success
		return true;
	}
	//not found return failure
	m_csLastError = CString("UnRegisterWatch::Node not found");
	return false;
}

//Receive loop
//run until done 
void CComEOSS::DoStart(WPARAM /*WWord*/, LPARAM /*LWord*/)
{
	while(!m_bDone)
	{
		Sleep(1000);
	}
}


void CComEOSS::DoStop()
{
	//flag dostart to exit now
	m_bDone = true;
}

void CComEOSS::DoReset(WPARAM /*WWord*/, LPARAM LWord)
{
//	TRACE("CComEOSS::DoReset\n");
	if (SetUpPort(m_pName))
	{
		::PostThreadMessage(m_nThreadID,m_uIDC_COM_START,0,0);
		SendToWatch(14,"Comm RESTARTED",LWord);
		m_bStopped = false;
	}
}

//called to send a buffer of data to the port
int CComEOSS::Transmit(int Length, char* Output, int Node, bool Quiet)
{
	Node &= 0x0000ffff;

	Length = (Length==ISZSTRING)?strlen(Output):Length;

	//send to node watcher
	if (!Quiet)
	{
		if ((NodeInfo.hWatcher != NULL) && (Node != 0))
		{
			int CurLength = (Length==ISZSTRING)?strlen(Output):Length;
			if (Output[CurLength-1] != 0x0d)
				CurLength++;
			char* Data = (char*)CoTaskMemAlloc(CurLength);
			if (Data != NULL)
			{
				memcpy(Data,Output,CurLength);
				Data[CurLength-1] = '\n';
														
			::PostMessage(NodeInfo.hWatcher,
				m_uIDC_RECEIVE,
				CurLength,
				(LPARAM)Data);
			}
		}
	}

	return Length;
}
//////////////////////////////////////////////////////////////////////////////
//
//                      CollectParametersForNewCSO
//
//////////////////////////////////////////////////////////////////////////////
bool CComEOSS::CollectParametersForNewCSO(CString& csNameOut)
{
//	TRACE("CComEOSS::CollectParametersForNewCSO()\n");

	//This CSO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new CSO from that data.

	//Collect the data for the new communications object.
	CNewEOSSComm dlg(m_szIniFile,m_iDlgClose,m_pName);
	int result = dlg.DoModal();
	if (result == IDOK)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(COMMUNICATIONS,m_pName,EOSS_DISCOVERY_TYPE,m_szIniFile);
		return true;
	}
	else
	{
		csNameOut.Empty();
		return false;
	}
}

void CComEOSS::SendToWatch(int Length, char* Output, int Node)
{
	Node &= 0x0000ffff;
	if (NodeInfo.hWatcher != NULL)
	{
		int dlength = (Length==ISZSTRING)?strlen(Output):Length;
		char*Data = (char*)CoTaskMemAlloc(dlength+2);
		if (Data != NULL)
		{
			memcpy(Data,Output,dlength);
			if (Data[0]==0x0d) Data[0]= NULL;
			Data[dlength] = 0x0a;
			Data[dlength+1]=NULL;
			::PostMessage(NodeInfo.hWatcher, m_uIDC_RECEIVE,dlength+1,(LPARAM)Data);
		}
	}
}


BOOL CComEOSS::EscapeCommFunction(DWORD dwFunction)
{
	return TRUE;
}


