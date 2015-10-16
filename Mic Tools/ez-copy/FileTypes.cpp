// FileTypes.cpp : implementation file
//

#include "stdafx.h"
#include "CullFile.h"
#include "FileTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileTypes dialog


CFileTypes::CFileTypes(CWnd* pParent /*=NULL*/)
	: CDialog(CFileTypes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileTypes)
	m_cFileTypes = _T("");
	m_bDoSubDirs = FALSE;
	//}}AFX_DATA_INIT
}


void CFileTypes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileTypes)
	DDX_Text(pDX, IDC_EDIT1, m_cFileTypes);
	DDX_Check(pDX, IDC_CHECK1, m_bDoSubDirs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileTypes, CDialog)
	//{{AFX_MSG_MAP(CFileTypes)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTypes message handlers
