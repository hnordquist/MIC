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
// MCAUserProg.cpp


#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "MCAUserProg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CMCAUserProg property page

IMPLEMENT_DYNCREATE(CMCAUserProg, CPropertyPage)

CMCAUserProg::CMCAUserProg() : CPropertyPage(CMCAUserProg::IDD)
{
}


CMCAUserProg::CMCAUserProg(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pParent = parent;
	m_pPropertySheet = NULL;

	for (int i= IDC_USERPROG_FIRST; i <= IDC_USERPROG_LAST; i++)
	{
		StaticColors[i-IDC_USERPROG_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CUserProg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CMCAUserProg::SetText(int index,CHAR * text)
{
	if ((index >= IDC_USERPROG_FIRST) &&
		(index <= IDC_USERPROG_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CMCAUserProg::SetPropertySheet(CMCAPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}
void
CMCAUserProg::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_USERPROG_FIRST) &&
		(index <= IDC_USERPROG_LAST)) {
	  StaticColors[index-IDC_USERPROG_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

CMCAUserProg::~CMCAUserProg()
{
}

void CMCAUserProg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserProg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCAUserProg, CPropertyPage)
	//{{AFX_MSG_MAP(CMCAUserProg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserProg message handlers
HBRUSH CMCAUserProg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_USERPROG_FIRST) && (index <= IDC_USERPROG_LAST))
	{
		int iFirstOfList = IDC_USERPROG_FIRST; 
		if (StaticColors[index-iFirstOfList]==COLOR_RED)
		{
			pDC->SetBkColor(COLOR_RED);
			pDC->SetTextColor(RGB(255,255,255));
		}
		else if (StaticColors[(index-iFirstOfList)]==COLOR_YELLOW)
		{
			pDC->SetBkColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_RED);
		}
		else
		{
			pDC->SetTextColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(RGB(0,0,0));
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}
	return hbr;
}
