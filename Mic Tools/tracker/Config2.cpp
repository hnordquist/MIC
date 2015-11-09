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
// Config2.cpp : implementation file
//

#include "stdafx.h"
#include "Tracker.h"
#include "Config2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfig2 dialog


CConfig2::CConfig2(CWnd* pParent /*=NULL*/)
	: CDialog(CConfig2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfig2)
	m_cWidth = _T("");
	m_cHeight = _T("");
	m_cColumns = _T("");
	m_cFlagSize = _T("");
	//}}AFX_DATA_INIT

	m_dButtonWidth=175; 
	m_dButtonHeight=75;
	m_dButtonsWide=1;
	m_dButtonFlagSize=40;
}


void CConfig2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfig2)
	DDX_Control(pDX, IDC_SPIN4, m_pSpinFlagSize);
	DDX_Control(pDX, IDC_SPIN3, m_pSpinColumns);
	DDX_Control(pDX, IDC_SPIN2, m_pSpinHeight);
	DDX_Control(pDX, IDC_SPIN1, m_pSpinWidth);
	DDX_Text(pDX, IDC_EDIT1, m_cWidth);
	DDV_MaxChars(pDX, m_cWidth, 3);
	DDX_Text(pDX, IDC_EDIT2, m_cHeight);
	DDV_MaxChars(pDX, m_cHeight, 3);
	DDX_Text(pDX, IDC_EDIT3, m_cColumns);
	DDV_MaxChars(pDX, m_cColumns, 3);
	DDX_Text(pDX, IDC_EDIT4, m_cFlagSize);
	DDV_MaxChars(pDX, m_cFlagSize, 3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfig2, CDialog)
	//{{AFX_MSG_MAP(CConfig2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfig2 message handlers
void CConfig2::SetValues(int Width, int Height, int Columns, int Flags)
{
	m_dButtonWidth = Width;
	m_dButtonHeight = Height;
	m_dButtonsWide = Columns;
	m_dButtonFlagSize = Flags;
}
void CConfig2::GetValues(int *Width, int *Height, int *Columns, int *Flags)
{
	*Width = atoi(m_cWidth);
	*Height = atoi(m_cHeight);
	*Columns = atoi(m_cColumns);
	*Flags = atoi(m_cFlagSize);
}

BOOL CConfig2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pSpinWidth.SetRange(1,499);
	m_pSpinHeight.SetRange(1,499);
	m_pSpinColumns.SetRange(1,499);
	m_pSpinFlagSize.SetRange(1,499);

	m_pSpinWidth.SetPos(m_dButtonWidth); 
	m_pSpinHeight.SetPos(m_dButtonHeight);
	m_pSpinColumns.SetPos(m_dButtonsWide);
	m_pSpinFlagSize.SetPos(m_dButtonFlagSize);
			
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

