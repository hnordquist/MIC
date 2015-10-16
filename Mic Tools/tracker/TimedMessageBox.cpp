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