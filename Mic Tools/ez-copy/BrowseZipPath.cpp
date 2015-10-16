// BrowseZipPath.cpp : implementation file
//

#include "stdafx.h"
#include "CullFile.h"
#include "BrowseZipPath.h"
#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrowseZipPath dialog


CBrowseZipPath::CBrowseZipPath(CWnd* pParent /*=NULL*/)
	: CDialog(CBrowseZipPath::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBrowseZipPath)
	m_csZIPPath = _T("");
	//}}AFX_DATA_INIT
}


void CBrowseZipPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrowseZipPath)
	DDX_Text(pDX, IDC_EDIT1, m_csZIPPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrowseZipPath, CDialog)
	//{{AFX_MSG_MAP(CBrowseZipPath)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseZipPath message handlers

void CBrowseZipPath::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	CDirDialog dir(this);
	dir.m_strTitle = "Select PKZIP25.EXE Directory";
	if (dir.DoBrowse() == 1)
	{
		CString cTemp(dir.m_strPath);
		if (cTemp.Find("PKZIP25.EXE")<0)
			cTemp += "\\PKZIP25.EXE";
		SetDlgItemText(IDC_EDIT1,dir.m_strPath);
	}
	
}
