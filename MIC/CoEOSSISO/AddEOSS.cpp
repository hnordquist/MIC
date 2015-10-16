// AddEOSS.cpp : implementation file
//

#include "stdafx.h"
#include "coeossiso.h"
#include "AddEOSS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddEOSS dialog


CAddEOSS::CAddEOSS(CWnd* pParent /*=NULL*/)
	: CDialog(CAddEOSS::IDD, pParent), m_parentsListCtrl(NULL)
{
	//{{AFX_DATA_INIT(CAddEOSS)
	m_csEOSSSN = _T("");
	//}}AFX_DATA_INIT
}


void CAddEOSS::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddEOSS)
	DDX_Control(pDX, IDC_ADD_ID, m_cEOSSEdit);
	DDX_Text(pDX, IDC_ADD_ID, m_csEOSSSN);
	DDV_MaxChars(pDX, m_csEOSSSN, 5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddEOSS, CDialog)
	//{{AFX_MSG_MAP(CAddEOSS)
	ON_EN_CHANGE(IDC_ADD_ID, OnChangeAddId)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CAddEOSS::OnBnClickedOk)
	ON_EN_KILLFOCUS(IDC_ADD_ID, &CAddEOSS::OnEnKillfocusAddId)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddEOSS message handlers

void CAddEOSS::OnChangeAddId() 
{
	CString candidate;
	m_cEOSSEdit.GetWindowText(candidate);		
	GetDlgItem(IDOK)->EnableWindow(candidate.GetLength() == 5);
	GetDlgItem(IDC_ADD_DUP_TXT)->ShowWindow(SW_HIDE);
}

BOOL CAddEOSS::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_cEOSSEdit.SetFocus();
	return TRUE; 
}

void CAddEOSS::OnBnClickedOk()
{
	if (IsUniqueId())
		CDialog::OnOK();
}


void CAddEOSS::OnEnKillfocusAddId()
{
	if (IsDuplicateId())
	{
		GetDlgItem(IDC_ADD_DUP_TXT)->ShowWindow(SW_SHOWNOACTIVATE);
		GetDlgItem(IDOK)->EnableWindow(false);
	}
	else
		GetDlgItem(IDC_ADD_DUP_TXT)->ShowWindow(SW_HIDE);
		
}

bool CAddEOSS::IsDuplicateId()
{
	int iCnt = m_parentsListCtrl->GetItemCount();
	CString s, candidate;
	bool duplicate = false;
	m_cEOSSEdit.GetWindowText(candidate);	
	for (int i = 0; i < iCnt; i++)
	{
		s = m_parentsListCtrl->GetItemText(i,1);
		if (s.CompareNoCase(candidate) == 0)
		{
			duplicate = true;
			break;
		}
	}
	return duplicate;
}

bool CAddEOSS::IsUniqueId()
{
	return !IsDuplicateId();
}