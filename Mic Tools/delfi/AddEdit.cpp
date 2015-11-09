/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
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
// AddEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Sync.h"
#include "AddEdit.h"
#include "DirDialog.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddEdit dialog


CAddEdit::CAddEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CAddEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddEdit)
	m_dCycDays = 0;
	m_dCycHours = 0;
	m_dCycMinutes = 0;
	m_dCycSeconds = 0;
	m_cPath = _T("");
	m_bConsiderArchive = FALSE;
	m_bDoSub = FALSE;
	//}}AFX_DATA_INIT
}


void CAddEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddEdit)
	DDX_Control(pDX, IDC_CHECK_CONSIDERARCHIVE, m_pConsiderArchive);
	DDX_Control(pDX, IDC_CHECK_DOSUB, m_pDoSub);
	DDX_Control(pDX, IDC_EDIT1, m_pPath);
	DDX_Control(pDX, IDC_SPIN_CYCSECONDS, m_pCycSeconds);
	DDX_Control(pDX, IDC_SPIN_CYCMINUTES, m_pCycMinutes);
	DDX_Control(pDX, IDC_SPIN_CYCHOURS, m_pCycHours);
	DDX_Control(pDX, IDC_SPIN_CYCDAYS, m_pCycDays);
	DDX_Text(pDX, IDC_EDIT_CYCDAYS, m_dCycDays);
	DDV_MinMaxUInt(pDX, m_dCycDays, 0, 999);
	DDX_Text(pDX, IDC_EDIT_CYCHOURS, m_dCycHours);
	DDV_MinMaxUInt(pDX, m_dCycHours, 0, 23);
	DDX_Text(pDX, IDC_EDIT_CYCMINUTES, m_dCycMinutes);
	DDV_MinMaxUInt(pDX, m_dCycMinutes, 0, 59);
	DDX_Text(pDX, IDC_EDIT_CYCSECONDS, m_dCycSeconds);
	DDV_MinMaxUInt(pDX, m_dCycSeconds, 0, 59);
	DDX_Text(pDX, IDC_EDIT1, m_cPath);
	DDV_MaxChars(pDX, m_cPath, 260);
	DDX_Check(pDX, IDC_CHECK_CONSIDERARCHIVE, m_bConsiderArchive);
	DDX_Check(pDX, IDC_CHECK_DOSUB, m_bDoSub);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddEdit, CDialog)
	//{{AFX_MSG_MAP(CAddEdit)
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonBrowse)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddEdit message handlers

void CAddEdit::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	CDirDialog Dir(this);
	Dir.m_strTitle = "Select Directory for DelFi to watch";
	ShowWindow(SW_HIDE);
	if (Dir.DoBrowse() == 1)
	{
		m_pPath.SetWindowText(Dir.m_strPath);
	}
	ShowWindow(SW_SHOW);
}

BOOL CAddEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(m_szTitle);
	m_pCycDays.SetRange(0,999);
	m_pCycDays.SetPos(m_dCycDays);
	m_pCycHours.SetRange(0,23);
	m_pCycHours.SetPos(m_dCycHours);
	m_pCycMinutes.SetRange(0,59);
	m_pCycMinutes.SetPos(m_dCycMinutes);
	m_pCycSeconds.SetRange(0,59);
	m_pCycSeconds.SetPos(m_dCycSeconds);

	CheckIt();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddEdit::OnOK()
{
	// TODO: Add extra validation here
	if (
		(GetDlgItemInt(IDC_EDIT_CYCDAYS)   ==0) &&
		(GetDlgItemInt(IDC_EDIT_CYCHOURS)  ==0) &&
		(GetDlgItemInt(IDC_EDIT_CYCMINUTES)==0) &&
		(GetDlgItemInt(IDC_EDIT_CYCSECONDS)==0) 
		)
	{
		AfxMessageBox("Error: You MUST set one or more values in \"Delete At Age Of\".",MB_OK);
		return;
	}

	CString cText;
	m_pPath.GetWindowText(cText);
	if (access(cText,06))
		if (AfxMessageBox("Warning: Could not access directory!  "
			"The directory may not exist!",MB_OKCANCEL | MB_ICONINFORMATION)!=IDOK)
			return;
	CDialog::OnOK();
}

void CAddEdit::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CheckIt();
}

void CAddEdit::CheckIt()
{
	CString cText;
	m_pPath.GetWindowText(cText);
//	if	(
//		(
//		(GetDlgItemInt(IDC_EDIT_CYCDAYS)   !=0) ||
//		(GetDlgItemInt(IDC_EDIT_CYCHOURS)  !=0) ||
//		(GetDlgItemInt(IDC_EDIT_CYCMINUTES)!=0) ||
//		(GetDlgItemInt(IDC_EDIT_CYCSECONDS)!=0) 
//		)
//		&&
	if
		(cText.GetLength() > 0)
//		)
		GetDlgItem(IDOK)->EnableWindow(true);
	else
		GetDlgItem(IDOK)->EnableWindow(false);
}
