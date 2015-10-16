// FileSpec.cpp : implementation file
//

#include "stdafx.h"
#include "tree.h"
#include "FileSpec.h"
#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileSpec dialog


CFileSpec::CFileSpec(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSpec::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileSpec)
	m_cPath = _T("");
	//}}AFX_DATA_INIT
	m_bAllowBrowse = true;
	m_bAddGemAuth = false;
	m_bGetForm = false;
	m_AddOn = "GemAuth.exe";
}


void CFileSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileSpec)
	DDX_Control(pDX, IDC_EDIT_PATH, m_pEditPath);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_pBrowseButton);
	DDX_Text(pDX, IDC_EDIT_PATH, m_cPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileSpec, CDialog)
	//{{AFX_MSG_MAP(CFileSpec)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_EN_CHANGE(IDC_EDIT_PATH, OnChangeEditPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileSpec message handlers

void CFileSpec::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	CDirDialog Dir(this);
	Dir.m_strTitle = "Select DIRECTORY";
	ShowWindow(SW_HIDE);
	if (Dir.DoBrowse() == 1)
	{
		CString cTemp = Dir.m_strPath;
		cTemp.MakeUpper();
		if (m_bAddGemAuth)
		{
			if (cTemp.Find("GEMAUTH.EXE")==-1)
			{
				if (Dir.m_strPath.Right(1) != '\\')
					Dir.m_strPath += '\\';	
				Dir.m_strPath += m_AddOn;
			}
		}
		m_pEditPath.SetWindowText(Dir.m_strPath);
	}
	ShowWindow(SW_SHOW);
}

BOOL CFileSpec::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_Name.GetLength())
		SetWindowText(m_Name);
	if (m_Message.GetLength())
		SetDlgItemText(IDC_STATIC_MESSAGE,m_Message);
	else
		SetDlgItemText(IDC_STATIC_MESSAGE,"Enter full path of directory...");
	if (m_FileSpec.GetLength())
		SetDlgItemText(IDC_EDIT_PATH,m_FileSpec);
	else
		if (!m_bAllowBrowse)
			SetDlgItemText(IDC_EDIT_PATH,"*.BIN,*.ISR,*.MCA,*.JSR,*.CEV,*.PFM");
	
	if (m_bAllowBrowse)
		m_pBrowseButton.ShowWindow(SW_SHOW);
	else
		m_pBrowseButton.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PATH)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileSpec::OnChangeEditPath() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if (m_bGetForm)
	{
		CString cRawString;
		CString cFormattedString;
		GetDlgItemText(IDC_EDIT_PATH,cRawString);
		bool bEscape = false;
		COleDateTime cNow = COleDateTime::GetCurrentTime();
		for (int i = 0; i < cRawString.GetLength(); i++)
		{
			if (bEscape)
			{
				switch(cRawString[i]) {
				case 'H'://hours
					cFormattedString += cNow.Format("%H");
					break;
				case 'B'://text month
					cFormattedString += cNow.Format("%B");
					break;
				case 'b'://abrev month
					cFormattedString += cNow.Format("%b");				
					break;
				case 'A'://text day
					cFormattedString += cNow.Format("%A");
					break;
				case 'a'://abrev day
					cFormattedString += cNow.Format("%a");				
					break;
				case 'Y'://year w/century
					cFormattedString += cNow.Format("%Y");
					break;
				case 'y'://year wo/centry
					cFormattedString += cNow.Format("%y");				
					break;
				case 'M'://minute
					cFormattedString += cNow.Format("%M");
					break;
				case 'm'://month
					cFormattedString += cNow.Format("%m");
					break;
				case 'D':
				case 'd'://day
					cFormattedString += cNow.Format("%d");
					break;
				}
				bEscape = false;
			}
			else if (cRawString[i] == '%')
				bEscape = true;
			else
			{
				cFormattedString += cRawString[i];
				bEscape = false;
			}
		}
		SetDlgItemText(IDC_STATIC_MSG,cFormattedString);
	}
}
