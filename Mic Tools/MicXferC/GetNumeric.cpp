// GetNumeric.cpp : implementation file
//

#include "stdafx.h"
#include "tree.h"
#include "GetNumeric.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetNumeric dialog


CGetNumeric::CGetNumeric(CWnd* pParent /*=NULL*/)
	: CDialog(CGetNumeric::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetNumeric)
	m_dValue = 0;
	//}}AFX_DATA_INIT
}


void CGetNumeric::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetNumeric)
	DDX_Text(pDX, IDC_EDIT_VALUE, m_dValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetNumeric, CDialog)
	//{{AFX_MSG_MAP(CGetNumeric)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetNumeric message handlers

BOOL CGetNumeric::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(m_cTitle);
	SetDlgItemText(IDC_STATIC_MESSAGE,m_cMessage);
	GetDlgItem(IDC_EDIT_VALUE)->SetFocus();
	return FALSE;
//	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
