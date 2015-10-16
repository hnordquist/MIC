// WatchAboutDlg.cpp 

#include "stdafx.h"
#include "WatchAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWatchAboutDlg::CWatchAboutDlg(int iclose, CString* pversion) : CDialog(CWatchAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CWatchAboutDlg)
	//}}AFX_DATA_INIT

	m_iDlgCloseMilliseconds = iclose;
	m_pcsNameVersionDate = pversion;
}

void CWatchAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWatchAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWatchAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CWatchAboutDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWatchAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetTimer(1,m_iDlgCloseMilliseconds,NULL);

	//CString cs("MIC Watch Window ");
	//cs += "Version: " + *m_pcsVersion + " " + __DATE__;

	//SetDlgItemText(IDC_STATIC_VERSION, cs);
	SetDlgItemText(IDC_STATIC_VERSION, *m_pcsNameVersionDate);

#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG, "Release Build");
#endif

	return TRUE;
}

void CWatchAboutDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnOK();
	}
	CDialog::OnTimer(nIDEvent);
}