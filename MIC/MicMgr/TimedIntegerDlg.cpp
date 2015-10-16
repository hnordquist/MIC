
// TimedIntegerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TimedIntegerDlg.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimedIntegerDlg dialog


CTimedIntegerDlg::CTimedIntegerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimedIntegerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimedIntegerDlg)
	m_cMsg1 = _T("");
	m_cMsg2 = _T("");
	m_cEditCtrl = _T("");
	m_cMsg3 = _T("");
	m_cEditName = _T("");
	m_bLimited = FALSE;
	m_bAppend = FALSE;
	m_bDoOnce = FALSE;
	m_cTime = 0;
	//}}AFX_DATA_INIT
	m_iTimeCount = -1;
	m_iMin = 0;
	m_iMax = 9999;
	m_iPos = 0;
}


void CTimedIntegerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimedIntegerDlg)
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_cDateTimeCtrl);
	DDX_Control(pDX, IDC_EDIT_TIMED_INTEGER_2, m_cEditNameCtrl);
	DDX_Control(pDX, IDC_EDIT_TIMED_INTEGER_1, m_cEditCtrlCtrl);
	DDX_Control(pDX, IDC_SPIN_TIMED_INTEGER, m_cSpin1);
	DDX_Control(pDX, IDCANCEL, m_cCancelButton);
	DDX_Text(pDX, IDC_STATIC_TIMED_INTEGER_MSG_1, m_cMsg1);
	DDX_Text(pDX, IDC_STATIC_TIMED_INTEGER_MSG_2, m_cMsg2);
	DDX_Text(pDX, IDC_EDIT_TIMED_INTEGER_1, m_cEditCtrl);
	DDX_Text(pDX, IDC_STATIC_TIMED_INTEGER_MSG_3, m_cMsg3);
	DDX_Text(pDX, IDC_EDIT_TIMED_INTEGER_2, m_cEditName);
	DDX_Check(pDX, IDC_CHECK_LIMITED, m_bLimited);
	DDX_Check(pDX, IDC_CHECK_APPEND, m_bAppend);
	DDX_Check(pDX, IDC_CHECK1, m_bDoOnce);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_cTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimedIntegerDlg, CDialog)
	//{{AFX_MSG_MAP(CTimedIntegerDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_EN_CHANGE(IDC_EDIT_TIMED_INTEGER_2, OnChangeEdit2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimedIntegerDlg message handlers

void CTimedIntegerDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMED_DLG)
	{
		m_iTimeCount--;
		if (m_iTimeCount <= 0)
		{
			KillTimer(TIMED_DLG);
			OnCancel();
		}
		else
		{
			char szTemp[32];
			sprintf(szTemp,"Cancel (%d)",m_iTimeCount);
			m_cCancelButton.SetWindowText(szTemp);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CTimedIntegerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_cMsg0);
//	SetTimer(TIMED_DLG,g_iDlgCloseMillisec,NULL);
	SetTimer(TIMED_DLG,1000,NULL);								//scr00079

	if (m_iTimeCount < 0)
		m_iTimeCount = 60;

	m_iFullTimeCount = m_iTimeCount;
	char szTemp[32];
	sprintf(szTemp,"Cancel (%d)",m_iTimeCount);
	m_cCancelButton.SetWindowText(szTemp);
	m_cSpin1.SetRange(m_iMin,m_iMax);
	m_cSpin1.SetPos(m_iPos);
	sprintf(szTemp,"%d",m_iPos);
	m_cEditCtrlCtrl.SetWindowText(szTemp);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTimedIntegerDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CTimedIntegerDlg::OnButtonBrowse() 
{
	UpdateData();
	CFileDialog cFileDialog(TRUE, "*.txt", m_cEditName, NULL);
	KillTimer(TIMED_DLG);

	if (cFileDialog.DoModal()==IDOK)
		m_cEditNameCtrl.SetWindowText(cFileDialog.GetPathName());

	SetTimer(TIMED_DLG,g_iDlgCloseMillisec,NULL);
}

void CTimedIntegerDlg::OnChangeEdit2() 
{
	m_iTimeCount = m_iFullTimeCount;
}
