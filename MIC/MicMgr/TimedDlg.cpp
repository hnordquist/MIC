// TimedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "micmgr.h"
#include "TimedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimedDlg dialog

CTimedDlg::CTimedDlg(
	CString& cstitle,
	CString& csmessage,
	int   seconds,
	CWnd* pParent /*=NULL*/)
	: CDialog(CTimedDlg::IDD, pParent)
{

	//{{AFX_DATA_INIT(CTimedDlg)
	m_cStaticMessage = _T("");
	//}}AFX_DATA_INIT

	m_iTimeToGoInSeconds = seconds;
	m_csTitle = *cstitle;
	m_cStaticMessage = csmessage;
}

CTimedDlg::CTimedDlg(
	const char *title,
	const char *message,
	int   seconds,
	CWnd* pParent /*=NULL*/)
	: CDialog(CTimedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimedDlg)
	m_cStaticMessage = _T("");
	//}}AFX_DATA_INIT

	m_cStaticMessage = CString(message);
	m_iTimeToGoInSeconds = seconds;
	m_csTitle = title;
}

CTimedDlg::~CTimedDlg()
{
}

void CTimedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimedDlg)
	DDX_Text(pDX, IDC_STATIC_MESSAGE, m_cStaticMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimedDlg, CDialog)
	//{{AFX_MSG_MAP(CTimedDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimedDlg message handlers

void CTimedDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1)
		CDialog::OnOK();
	CDialog::OnTimer(nIDEvent);
}

BOOL CTimedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(m_csTitle);
	SetTimer(1,m_iTimeToGoInSeconds*1000,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTimedDlg::OnOK() 
{
	// TODO: Add extra validation here
	KillTimer(1);
	CDialog::OnOK();
}
