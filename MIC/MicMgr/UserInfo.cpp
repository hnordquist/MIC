 

// UserInfo.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"
#include "UserInfo.h"
#include "TimedMessageDialog.h"
#include "mic_definitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserInfo dialog


CUserInfo::CUserInfo(CWnd* pParent, int mode, char* name, char* password )
	: CDialog(CUserInfo::IDD, pParent)
{
	m_dMode = mode;
	m_pcName = name;
	m_pcPassword = password;
	//{{AFX_DATA_INIT(CUserInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUserInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserInfo, CDialog)
	//{{AFX_MSG_MAP(CUserInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserInfo message handlers

BOOL CUserInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	char buffa[128];
	char buffb[128];
	GetWindowText(buffa,sizeof(buffa));
	switch(m_dMode){
	case 0:
		sprintf(buffb,buffa,"Edit");
		break;
	case 1:
		sprintf(buffb,buffa,"New");
		break;
	}
	SetWindowText(buffb);

	((CEdit*)GetDlgItem(IDC_USERNAME_EDIT))->SetWindowText(m_pcName);
	((CEdit*)GetDlgItem(IDC_PASSWORD1_EDIT))->SetWindowText(m_pcPassword);
	((CEdit*)GetDlgItem(IDC_PASSWORD2_EDIT))->SetWindowText(m_pcPassword);
	GetDlgItem(IDC_USERNAME_EDIT)->SetFocus();
	return false; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserInfo::OnOK() 
{
	CString name;
	CString password1;
	CString password2;
	((CEdit*)GetDlgItem(IDC_USERNAME_EDIT ))->GetWindowText(name);
	((CEdit*)GetDlgItem(IDC_PASSWORD1_EDIT))->GetWindowText(password1);
	((CEdit*)GetDlgItem(IDC_PASSWORD2_EDIT))->GetWindowText(password2);
	name.TrimLeft();
	name.TrimRight();
	password1.TrimLeft();
	password2.TrimLeft();
	password1.TrimRight();
	password2.TrimRight();

	if (g_bHideWindows) 
		ShowWindow(SW_HIDE);

	CString csTitle(MIC_NAMELONG);
	csTitle += ": Password";

	if (password1 != password2)
	{
		CString csText("Passwords don't match!");
		new CTimedMessageDialog(csTitle,csText,1000 * g_iDlgCloseMillisec, this);
		//MsgBox.DoModal(LPCTSTR (CString(MIC_NAMELONG)+CString(": Password")),
		//			"Passwords don't match!");
		ShowWindow(SW_SHOW);
		return;
	} 

	if (name.GetLength() == 0) 
	{
		CString csText("A user name must be entered!");
		new CTimedMessageDialog(csTitle,csText,1000 * g_iDlgCloseMillisec, this);
		//CTimeMsgDlg MsgBox(this, g_dDlgCloseMillisec);
		//MsgBox.DoModal(LPCTSTR (CString(MIC_NAMELONG)+CString(": Password")),
		//			"A user name must be entered!");
		ShowWindow(SW_SHOW);
		return;
	}

	if (password1.GetLength() == 0) 
	{
		CString csText("A password must be entered!");
		new CTimedMessageDialog(csTitle,csText,1000 * g_iDlgCloseMillisec, this);
		//CTimeMsgDlg MsgBox(this, g_dDlgCloseMillisec);
		//MsgBox.DoModal(LPCTSTR (CString(MIC_NAMELONG)+CString(": Password")),
		//"A password must be entered!");
		ShowWindow(SW_SHOW);
		return;
	}

	strcpy(m_pcName,name);
	strcpy(m_pcPassword,password1);
	CDialog::OnOK();
}

BOOL CUserInfo::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DestroyWindow();
}
