/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-040. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
// AddItem.cpp : implementation file
//

#include "stdafx.h"
#include "CullFile.h"
#include "DirDialog.h"
#include <io.h>
#include "AddItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AddItem dialog


AddItem::AddItem(CWnd* pParent /*=NULL*/)
	: CDialog(AddItem::IDD, pParent)
{
	//{{AFX_DATA_INIT(AddItem)
	m_bDoSubDirs = FALSE;
	m_bOnlyIfChange = FALSE;
	m_cSourceDir = _T("");
	m_cDestinationDir = _T("");
	m_cFileTypes = _T("");
	m_bDoBulk = FALSE;
	//}}AFX_DATA_INIT
}

void AddItem::SetUp(CString cSourceDir, 
		BOOL bDoSubDirs, BOOL bDoBulk,
		CString cDestinationDir, CString cFileTypes, char* szIniFile, BOOL bOnlyIfChange)
{
	m_cSourceDir = cSourceDir;
	m_bDoSubDirs = bDoSubDirs;
	m_bDoBulk = bDoBulk;
	m_cDestinationDir = cDestinationDir;
	m_cFileTypes = cFileTypes;
	m_bOnlyIfChange = bOnlyIfChange;
	m_szIniFile = szIniFile;
}


void AddItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddItem)
	DDX_Check(pDX, IDC_CHECK1, m_bDoSubDirs);
	DDX_Text(pDX, IDC_EDIT1, m_cSourceDir);
	DDX_Text(pDX, IDC_EDIT2, m_cDestinationDir);
	DDX_Text(pDX, IDC_EDIT3, m_cFileTypes);
	DDX_Check(pDX, IDC_CHECK2, m_bDoBulk);
	DDX_Check(pDX, IDC_CHECK3, m_bOnlyIfChange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddItem, CDialog)
	//{{AFX_MSG_MAP(AddItem)
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonBrowseSource)
	ON_BN_CLICKED(IDC_BUTTON2, OnButtonBrowseDestination)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeSource)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeDestination)
	ON_EN_CHANGE(IDC_EDIT3, OnChangeFileTypes)
	ON_BN_CLICKED(IDC_BUTTON3, OnButtonSaveDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddItem message handlers

void AddItem::OnButtonBrowseSource() 
{
	// TODO: Add your control notification handler code here
	CDirDialog dir(this);
	dir.m_strTitle = "Select \"FROM\" Directory";
	if (dir.DoBrowse() == 1)
	{
		SetDlgItemText(IDC_EDIT1,dir.m_strPath);
		CheckAll();	
	}
}

void AddItem::OnButtonBrowseDestination() 
{
	// TODO: Add your control notification handler code here
	CDirDialog dir(this);
	dir.m_strTitle = "Select \"TO\" Directory";
	if (dir.DoBrowse() == 1)
	{
		SetDlgItemText(IDC_EDIT2,dir.m_strPath);
		CheckAll();
	}
}

void AddItem::OnOK() 
{
	// TODO: Add extra validation here
	CString cTemp;
	GetDlgItemText(IDC_EDIT1,cTemp);
	if (access(cTemp,04))
		if (AfxMessageBox("Warning: Cannot read FROM directory.  "
			"This directory may not exist.",MB_OKCANCEL | MB_ICONEXCLAMATION) != IDOK)
			return;
	CDialog::OnOK();
}

BOOL AddItem::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(m_cTitle);
	OnChangeSource();
	OnChangeDestination();
	OnChangeFileTypes();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AddItem::OnChangeSource() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CString cTemp;
	GetDlgItemText(IDC_EDIT1,cTemp);
	if (cTemp.GetLength() == 0)
		m_bGoodSource = false;
	else
		m_bGoodSource = true;
	CheckAll();
}

void AddItem::OnChangeDestination() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CString cTemp;
	GetDlgItemText(IDC_EDIT2,cTemp);
	if (cTemp.GetLength() == 0)
		m_bGoodDestination = false;
	else
		m_bGoodDestination = true;
	CheckAll();
}

void AddItem::OnChangeFileTypes() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CString cTemp;
	GetDlgItemText(IDC_EDIT3,cTemp);
	if (cTemp.GetLength() == 0)
		m_bGoodFileTypes = false;
	else
		m_bGoodFileTypes = true;
	CheckAll();
}

void AddItem::CheckAll()
{
	if (m_bGoodFileTypes && m_bGoodDestination && m_bGoodSource)
		GetDlgItem(IDOK)->EnableWindow(true);
	else
		GetDlgItem(IDOK)->EnableWindow(false);
}


void AddItem::OnButtonSaveDefault() 
{
	// TODO: Add your control notification handler code here
	char szBuff[256];
	GetDlgItemText(IDC_EDIT3,szBuff,sizeof(szBuff));
	WritePrivateProfileString("CONFIGURATION","DEFAULT",szBuff,m_szIniFile);
}
