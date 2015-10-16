// IPAddressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tree.h"
#include "IPAddressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPAddressDlg dialog


CIPAddressDlg::CIPAddressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPAddressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIPAddressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIPAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPAddressDlg)
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPAddressDlg, CDialog)
	//{{AFX_MSG_MAP(CIPAddressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPAddressDlg message handlers

BOOL CIPAddressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	unsigned char b1,b2,b3,b4;
	if (strlen(szAddress))
	{
		sscanf(szAddress,"%d.%d.%d.%d",&b1,&b2,&b3,&b4);
		m_IPAddress.SetAddress(b1,b2,b3,b4);
	}
	if (strlen(szMsg))
		SetDlgItemText(IDC_STATIC_MSG,szMsg);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIPAddressDlg::OnOK() 
{
	// TODO: Add extra validation here
	unsigned char b1,b2,b3,b4;
	m_IPAddress.GetAddress(b1,b2,b3,b4);
	sprintf(szAddress,"%d.%d.%d.%d",b1,b2,b3,b4);
	CDialog::OnOK();
}
