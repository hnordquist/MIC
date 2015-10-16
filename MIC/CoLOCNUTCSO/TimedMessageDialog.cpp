//$History: TimedMessageDialog.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Pelowitz     Date: 3/12/08    Time: 2:52p
 * Created in $/MIC 2004/CoLOCNUTCSO
 * At or near retirement (EOSS still being tested)
// 
// *****************  Version 2  *****************
// User: 149047       Date: 10/23/06   Time: 1:29p
// Updated in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 9/13/06    Time: 12:40p
// Created in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 1/11/06    Time: 7:47a
// Created in $/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Moore        Date: 8/09/05    Time: 4:12p
// Created in $/MIC 2.000/CoLOCNUTCSO
// 
// *****************  Version 2  *****************
// User: Moore        Date: 7/11/05    Time: 2:32p
// Updated in $/MIC 2.0/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Moore        Date: 7/07/05    Time: 12:41p
// Created in $/MIC 2.0/CoLOCNUTCSO
// Fixed problem with modeless message dialog 
// blocking other operations.

// TimedMessageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TimedMessageDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CTimedMessageDialog dialog


CTimedMessageDialog::CTimedMessageDialog(
	const char *title,
	const char *message,
	int   seconds,
	CWnd* pParent /*=NULL*/)
	: CDialog(CTimedMessageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimedMessageDialog)
	m_MessageStatic = _T("");
	//}}AFX_DATA_INIT
	m_MessageStatic = CString(message);
	m_iTimeToGoInSeconds = seconds;

	Create(IDD_MESSAGE_DIALOG,NULL);
	SetWindowText(title);
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
}

CTimedMessageDialog::CTimedMessageDialog(
	CString& cstitle,
	CString& csmessage,
	int   seconds,
	CWnd* pParent /*=NULL*/)
	: CDialog(CTimedMessageDialog::IDD, pParent)
{
	m_MessageStatic = csmessage;
	m_iTimeToGoInSeconds = seconds;

	Create(IDD_MESSAGE_DIALOG,NULL);
	SetWindowText(cstitle);
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
}

CTimedMessageDialog::~CTimedMessageDialog()
{
}

void CTimedMessageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimedMessageDialog)
	DDX_Text(pDX, IDC_MESSAGE_STATIC, m_MessageStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimedMessageDialog, CDialog)
	//{{AFX_MSG_MAP(CTimedMessageDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimedMessageDialog message handlers

void CTimedMessageDialog::OnTimer(UINT nIDEvent) 
{
//	TRACE("ONTIMER_HIGHEST_LEVEL %d\n",m_iTimeToGoInSeconds);

	if (nIDEvent == 1)
	{
//		TRACE("ONTIMER\n");
		if (--m_iTimeToGoInSeconds <= 0)
		{
			PostMessage(WM_CLOSE);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CTimedMessageDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	SetTimer(1,1000,NULL);
}

void CTimedMessageDialog::OnOK() 
{
	DestroyWindow();
	delete this;
}

void CTimedMessageDialog::OnClose() 
{	
	CDialog::OnClose();
	DestroyWindow();
	delete this;
}
