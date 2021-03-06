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
// Data2Stat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "Data2Stat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CData2Stat property page

IMPLEMENT_DYNCREATE(CData2Stat, CPropertyPage)

CData2Stat::CData2Stat() : CPropertyPage(CData2Stat::IDD)
{
}

CData2Stat::CData2Stat(UINT id) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;

	//{{AFX_DATA_INIT(CData2Stat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void
CData2Stat::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}


void
CData2Stat::SetText(int index, char * text)
{
	//set the cstatic to the provided text
	if ((index >= IDC_DATA2_FIRST) &&
		(index <= IDC_DATA2_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *
CData2Stat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_DATA2_FIRST) &&
		(index <= IDC_DATA2_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void
CData2Stat::SetColor(int index, COLORREF color)
{
	//set the current color of a cstatic
	//it will get updated via onctlcolor()
	if ((index >= IDC_DATA2_FIRST) &&
		(index <= IDC_DATA2_LAST)) {
	  StaticColors[index-IDC_DATA2_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CData2Stat::ResetColors()
{
	for (int i= IDC_DATA2_FIRST; i <= IDC_DATA2_LAST; i++)
	{
		StaticColors[i-IDC_DATA2_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CData2Stat::~CData2Stat()
{
}

void CData2Stat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CData2Stat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CData2Stat, CPropertyPage)
	//{{AFX_MSG_MAP(CData2Stat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataStat message handlers
HBRUSH CData2Stat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	//get the id of the control 
	UINT index = pWnd->GetDlgCtrlID();
	//set the font and color of that control
	if ((index >= IDC_DATA2_FIRST) && (index <= IDC_DATA2_LAST))
	{
		int iFirstOfList = IDC_DATA2_FIRST; 
		if (StaticColors[index-iFirstOfList]==COLOR_RED)
		{
			pDC->SetBkColor(COLOR_RED);
			pDC->SetTextColor(COLOR_WHITE);
		}
		else if (StaticColors[(index-iFirstOfList)]==COLOR_YELLOW)
		{
			pDC->SetBkColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_RED);
		}
		else
		{
			pDC->SetTextColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_BLACK);
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}
void CData2Stat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CData2Stat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here
//causes the tab bar to shift to the right
//	if (m_pPropertySheet->m_pcWndSpin && bShow)
//	{
//
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONDOWN,1,0x00000000);
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONUP,1,  0x00000000);
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONDOWN,1,0x00000000);
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONUP,1,  0x00000000);
//	}
	
}

int CData2Stat::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
