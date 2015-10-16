// GetPortNumber.cpp : implementation file
//

#include "stdafx.h"
#include "Tracker.h"
#include "GetPortNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetPortNumber dialog


CGetPortNumber::CGetPortNumber(CWnd* pParent /*=NULL*/)
	: CDialog(CGetPortNumber::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetPortNumber)
	m_uPort = 0;
	m_cStaticAddress = _T("");
	//}}AFX_DATA_INIT
}


void CGetPortNumber::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetPortNumber)
	DDX_Text(pDX, IDC_EDIT_PORT, m_uPort);
	DDV_MinMaxUInt(pDX, m_uPort, 0, 65535);
	DDX_Text(pDX, IDC_STATIC_MESSAGE, m_cStaticAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetPortNumber, CDialog)
	//{{AFX_MSG_MAP(CGetPortNumber)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetPortNumber message handlers
