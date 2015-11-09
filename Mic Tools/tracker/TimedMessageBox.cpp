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
//TimedMessageBox.cpp

#include "stdafx.h"
#include "TimedMessageBox.h"

CTimedMessageBox::CTimedMessageBox(
	const char *title,
	const char *message,
	int   seconds,
	CWnd *parent)
{
	memset(m_cTitle,0,sizeof(m_cTitle));
	memset(m_cMessage,0,sizeof(m_cMessage));
	strncpy_s(m_cTitle, title, sizeof(m_cTitle) - 1);
	strncpy_s(m_cMessage, message, sizeof(m_cMessage) - 1);
	m_iTimeToGoInSeconds = seconds;
	Create(NULL,"",0,CRect(0,0,0,0),parent,0);
}

CTimedMessageBox::CTimedMessageBox(
	CString& cstitle,
	CString& csmessage,
	int seconds,
	CWnd *parent)
{
	const char *title = LPCTSTR(cstitle);
	const char *message = LPCTSTR(csmessage);
	memset(m_cTitle,0,sizeof(m_cTitle));
	memset(m_cMessage,0,sizeof(m_cMessage));
	strncpy_s(m_cTitle, title, sizeof(m_cTitle) - 1);
	strncpy_s(m_cMessage, message, sizeof(m_cMessage) - 1);
	m_iTimeToGoInSeconds = seconds;
	Create(NULL,"",0,CRect(0,0,0,0),parent,0);
}


BEGIN_MESSAGE_MAP(CTimedMessageBox, CWnd)
	//{{AFX_MSG_MAP(CTimedMessageBox)
	ON_WM_TIMER()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CTimedMessageBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_iResult = MB_OK;
	m_uTimerID = SetTimer(1, 1000, NULL);
	MessageBox(m_cMessage,m_cTitle,MB_ICONWARNING);
	return 0;
}


void CTimedMessageBox::OnTimer(UINT nIDEvent) 
{
	TRACE("OnTimer\n");
	if (nIDEvent == m_uTimerID)
	{
		if (--m_iTimeToGoInSeconds <= 0)
		{
			m_iResult = ~MB_OK;
			CWnd *p = FindWindow(NULL, m_cTitle);
			if (p)
				p->PostMessage(WM_CLOSE);
			KillTimer(m_uTimerID);
		}
	}
	CWnd::OnTimer(nIDEvent);
}