// AddEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Sync.h"
#include "AddEdit.h"
#include "DirDialog.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddEdit dialog


CAddEdit::CAddEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CAddEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddEdit)
	m_dCycDays = 0;
	m_dCycHours = 0;
	m_dCycMinutes = 0;
	m_dCycSeconds = 0;
	m_cPath = _T("");
	m_bConsiderArchive = FALSE;
	m_bDoSub = FALSE;
	//}}AFX_DATA_INIT
}


void CAddEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddEdit)
	DDX_Control(pDX, IDC_CHECK_CONSIDERARCHIVE, m_pConsiderArchive);
	DDX_Control(pDX, IDC_CHECK_DOSUB, m_pDoSub);
	DDX_Control(pDX, IDC_EDIT1, m_pPath);
	DDX_Control(pDX, IDC_SPIN_CYCSECONDS, m_pCycSeconds);
	DDX_Control(pDX, IDC_SPIN_CYCMINUTES, m_pCycMinutes);
	DDX_Control(pDX, IDC_SPIN_CYCHOURS, m_pCycHours);
	DDX_Control(pDX, IDC_SPIN_CYCDAYS, m_pCycDays);
	DDX_Text(pDX, IDC_EDIT_CYCDAYS, m_dCycDays);
	DDV_MinMaxUInt(pDX, m_dCycDays, 0, 999);
	DDX_Text(pDX, IDC_EDIT_CYCHOURS, m_dCycHours);
	DDV_MinMaxUInt(pDX, m_dCycHours, 0, 23);
	DDX_Text(pDX, IDC_EDIT_CYCMINUTES, m_dCycMinutes);
	DDV_MinMaxUInt(pDX, m_dCycMinutes, 0, 59);
	DDX_Text(pDX, IDC_EDIT_CYCSECONDS, m_dCycSeconds);
	DDV_MinMaxUInt(pDX, m_dCycSeconds, 0, 59);
	DDX_Text(pDX, IDC_EDIT1, m_cPath);
	DDV_MaxChars(pDX, m_cPath, 260);
	DDX_Check(pDX, IDC_CHECK_CONSIDERARCHIVE, m_bConsiderArchive);
	DDX_Check(pDX, IDC_CHECK_DOSUB, m_bDoSub);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddEdit, CDialog)
	//{{AFX_MSG_MAP(CAddEdit)
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonBrowse)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddEdit message handlers

void CAddEdit::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	CDirDialog Dir(this);
	Dir.m_strTitle = "Select Directory for DelFi to watch";
	ShowWindow(SW_HIDE);
	if (Dir.DoBrowse() == 1)
	{
		m_pPath.SetWindowText(Dir.m_strPath);
	}
	ShowWindow(SW_SHOW);
}

BOOL CAddEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(m_szTitle);
	m_pCycDays.SetRange(0,999);
	m_pCycDays.SetPos(m_dCycDays);
	m_pCycHours.SetRange(0,23);
	m_pCycHours.SetPos(m_dCycHours);
	m_pCycMinutes.SetRange(0,59);
	m_pCycMinutes.SetPos(m_dCycMinutes);
	m_pCycSeconds.SetRange(0,59);
	m_pCycSeconds.SetPos(m_dCycSeconds);

	CheckIt();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddEdit::OnOK()
{
	// TODO: Add extra validation here
	if (
		(GetDlgItemInt(IDC_EDIT_CYCDAYS)   ==0) &&
		(GetDlgItemInt(IDC_EDIT_CYCHOURS)  ==0) &&
		(GetDlgItemInt(IDC_EDIT_CYCMINUTES)==0) &&
		(GetDlgItemInt(IDC_EDIT_CYCSECONDS)==0) 
		)
	{
		AfxMessageBox("Error: You MUST set one or more values in \"Delete At Age Of\".",MB_OK);
		return;
	}

	CString cText;
	m_pPath.GetWindowText(cText);
	if (access(cText,06))
		if (AfxMessageBox("Warning: Could not access directory!  "
			"The directory may not exist!",MB_OKCANCEL | MB_ICONINFORMATION)!=IDOK)
			return;
	CDialog::OnOK();
}

void CAddEdit::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CheckIt();
}

void CAddEdit::CheckIt()
{
	CString cText;
	m_pPath.GetWindowText(cText);
//	if	(
//		(
//		(GetDlgItemInt(IDC_EDIT_CYCDAYS)   !=0) ||
//		(GetDlgItemInt(IDC_EDIT_CYCHOURS)  !=0) ||
//		(GetDlgItemInt(IDC_EDIT_CYCMINUTES)!=0) ||
//		(GetDlgItemInt(IDC_EDIT_CYCSECONDS)!=0) 
//		)
//		&&
	if
		(cText.GetLength() > 0)
//		)
		GetDlgItem(IDOK)->EnableWindow(true);
	else
		GetDlgItem(IDOK)->EnableWindow(false);
}
