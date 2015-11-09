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