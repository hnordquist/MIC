// SetEdit.cpp

#include "stdafx.h"
#include "resource.h"
#include "SetEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetEdit dialog


CSetEdit::CSetEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CSetEdit::IDD, pParent)
{
	m_uLength = 0;
	//{{AFX_DATA_INIT(CSetEdit)
	m_cDesired = _T("");
	m_cDiscription = _T("");
	m_cReported = _T("");
	m_cIniName = _T("");
	m_cCommand = _T("");
	//}}AFX_DATA_INIT
}

void CSetEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetEdit)
	DDX_Control(pDX, IDC_EDIT1, m_cEdit);
	DDX_Text(pDX, IDC_EDIT1, m_cDesired);
	DDV_MaxChars(pDX, m_cDesired, 128);
	DDX_Text(pDX, IDC_STATIC_DESCRIPTION, m_cDiscription);
	DDV_MaxChars(pDX, m_cDiscription, 128);
	DDX_Text(pDX, IDC_STATIC_REPORTED, m_cReported);
	DDV_MaxChars(pDX, m_cReported, 128);
	DDX_Text(pDX, IDC_STATIC_ININAME, m_cIniName);
	DDX_Text(pDX, IDC_STATIC_COMMAND, m_cCommand);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetEdit, CDialog)
	//{{AFX_MSG_MAP(CSetEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetEdit message handlers

BOOL CSetEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_uLength>0)
		m_cEdit.SetLimitText(m_uLength);
	if (strlen(m_cDesired) == 0)
	{
		m_cEdit.SetWindowText(m_cReported);
		m_cDesired = m_cReported;
	}
	m_cEdit.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
