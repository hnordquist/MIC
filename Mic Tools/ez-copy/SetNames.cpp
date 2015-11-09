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
// SetNames.cpp : implementation file
//

#include "stdafx.h"
#include "CullFile.h"
#include "SetNames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetNames dialog


CSetNames::CSetNames(CWnd* pParent /*=NULL*/)
	: CDialog(CSetNames::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetNames)
	m_cSet1 = _T("");
	m_cSet2 = _T("");
	m_cSet3 = _T("");
	m_cSet4 = _T("");
	m_cSet5 = _T("");
	m_cSet6 = _T("");
	m_cSet7 = _T("");
	m_cSet8 = _T("");
	m_cSet9 = _T("");
	m_cSet10 = _T("");
	m_cSet11 = _T("");
	m_cSet12 = _T("");
	m_cSet13 = _T("");
	m_cSet14 = _T("");
	m_cSet15 = _T("");
	m_cSet16 = _T("");
	m_cSet17 = _T("");
	m_cSet18 = _T("");
	m_cSet19 = _T("");
	m_cSet20 = _T("");
	//}}AFX_DATA_INIT
}


void CSetNames::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetNames)
	DDX_Text(pDX, IDC_EDIT1, m_cSet1);
	DDX_Text(pDX, IDC_EDIT2, m_cSet2);
	DDX_Text(pDX, IDC_EDIT3, m_cSet3);
	DDX_Text(pDX, IDC_EDIT4, m_cSet4);
	DDX_Text(pDX, IDC_EDIT5, m_cSet5);
	DDX_Text(pDX, IDC_EDIT6, m_cSet6);
	DDX_Text(pDX, IDC_EDIT7, m_cSet7);
	DDX_Text(pDX, IDC_EDIT8, m_cSet8);
	DDX_Text(pDX, IDC_EDIT9, m_cSet9);
	DDX_Text(pDX, IDC_EDIT10, m_cSet10);
	DDX_Text(pDX, IDC_EDIT11, m_cSet11);
	DDX_Text(pDX, IDC_EDIT12, m_cSet12);
	DDX_Text(pDX, IDC_EDIT13, m_cSet13);
	DDX_Text(pDX, IDC_EDIT14, m_cSet14);
	DDX_Text(pDX, IDC_EDIT15, m_cSet15);
	DDX_Text(pDX, IDC_EDIT16, m_cSet16);
	DDX_Text(pDX, IDC_EDIT17, m_cSet17);
	DDX_Text(pDX, IDC_EDIT18, m_cSet18);
	DDX_Text(pDX, IDC_EDIT19, m_cSet19);
	DDX_Text(pDX, IDC_EDIT20, m_cSet20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetNames, CDialog)
	//{{AFX_MSG_MAP(CSetNames)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetNames message handlers
