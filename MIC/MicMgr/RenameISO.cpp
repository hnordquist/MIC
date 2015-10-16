// RenameISO.cpp

#include "stdafx.h"
#include "resource.h"
#include "RenameISO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameISO dialog


CRenameISO::CRenameISO(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameISO::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameISO)
	m_cNewName = _T("");
	m_cCurrentName = _T("");
	m_cOriginalName = _T("");
	m_iNewNode = 0;
	m_cExistingNode = _T("");
	m_cOriginalNode = _T("");
	//}}AFX_DATA_INIT
}


void CRenameISO::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameISO)
	DDX_Text(pDX, IDC_EDIT_NEWNAME, m_cNewName);
	DDV_MaxChars(pDX, m_cNewName, 127);
	DDX_Text(pDX, IDC_STATIC_CURRENTNAME, m_cCurrentName);
	DDX_Text(pDX, IDC_STATIC_ORIGINALNAME, m_cOriginalName);
	DDX_Text(pDX, IDC_EDIT_NEWNODE, m_iNewNode);
	DDV_MinMaxInt(pDX, m_iNewNode, -1, 65535);  //MIC_CB_028 to MIC 1.907 PJM 1/20/05
	DDX_Text(pDX, IDC_STATIC_EXISTINGNODE, m_cExistingNode);
	DDX_Text(pDX, IDC_STATIC_ORIGINALNODE, m_cOriginalNode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRenameISO, CDialog)
	//{{AFX_MSG_MAP(CRenameISO)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameISO message handlers
