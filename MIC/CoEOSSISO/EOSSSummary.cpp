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
// EOSSSummary.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "EOSSSummary.h"
#include "EOSSInstrument.h"
#include "EOSSSafePS.h"
#include "eossdefs.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLOR_GREENRED	1
#define COLOR_BLUERED   2
#define COLOR_REDYELLOW 3


/////////////////////////////////////////////////////////////////////////////
// CEOSSSummary property page

IMPLEMENT_DYNCREATE(CEOSSSummary, CPropertyPage)

CEOSSSummary::CEOSSSummary() : CPropertyPage(CEOSSSummary::IDD)
{
	m_bCheckerCreated = false;
	m_cSpan = COleDateTimeSpan(1,0,0,0);
}


CEOSSSummary::CEOSSSummary(UINT id, CISO *parent) : CPropertyPage(id)// CPropertyPage(CGenlStat::IDD)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_EOSSSUMMARY_FIRST; i <= IDC_EOSSSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_EOSSSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
	m_bCheckerCreated = false;
	m_cSpan = COleDateTimeSpan(1,0,0,0);
	//{{AFX_DATA_INIT(CEOSSSummary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEOSSSummary::~CEOSSSummary()
{
	if (m_bCheckerCreated)
		delete m_ctrlChecker;
}

BEGIN_MESSAGE_MAP(CEOSSSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CEOSSSummary)
	ON_BN_CLICKED(IDC_GOTO_EOSS_DATASTAT, OnGotoEOSSDatastat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEOSSSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEOSSSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
#define CELLSX 8
#define CELLSY 5
void CEOSSSummary::PopulateChecker()
{
	bool bDone = false;
	unsigned int uiIndex = 0;
	char szTemp[16];
	while (!bDone)
	{
		if (uiIndex < (CELLSX*CELLSY))
		{
			if (((CEOSSInstrument*)m_pParent)->EOSSes[uiIndex])
			{
				COLORREF sColor = RGB(255,255,255);
				if (((CEOSSInstrument*)m_pParent)->sStatus[uiIndex].ucRedCount)
					sColor = COLOR_RED;
				else if (((CEOSSInstrument*)m_pParent)->sStatus[uiIndex].ucYellowCount)
					sColor = COLOR_YELLOW;
				else if (((CEOSSInstrument*)m_pParent)->sStatus[uiIndex].ucInstID == 6) // not yet read at all
					sColor = COLOR_WHITE;
				else if (strstr(((CEOSSInstrument*)m_pParent)->sGeneral.cLATEST_RESULTS,"Not yet read"))
					sColor = COLOR_WHITE;
				else
					sColor = COLOR_GREEN;
				m_ctrlChecker.SetBlock(uiIndex,sColor,_itoa(((CEOSSInstrument*)m_pParent)->sStatus[uiIndex].uiEOSSSN,szTemp,10));
				uiIndex++;
			}
			else
				bDone = true;
		}
		else
			bDone = true;
	}
	while (uiIndex < (CELLSX*CELLSY)) m_ctrlChecker.SetBlock(uiIndex++,RGB(255,255,255)," ");
}


void CEOSSSummary::SetPropertySheet(CEOSSPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CEOSSSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_EOSSSUMMARY_FIRST) &&
		(index <= IDC_EOSSSUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CEOSSSummary::ResetColors()
{
	for (int i= IDC_EOSSSUMMARY_FIRST; i <= IDC_EOSSSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_EOSSSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}


void CEOSSSummary::OnGotoEOSSDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}


HBRUSH CEOSSSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_EOSSSUMMARY_FIRST) && (index <= IDC_EOSSSUMMARY_LAST))
	{
		int iFirstOfList = IDC_EOSSSUMMARY_FIRST; 
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

void CEOSSSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_EOSSSUMMARY_FIRST) && (index <= IDC_EOSSSUMMARY_LAST))
		{
			if (
				(index == IDC_EOSSSUMMARY_COMMAND11)||
				(index == IDC_EOSSSUMMARY_COMMAND10)||
				(index == IDC_EOSSSUMMARY_COMMAND9) ||
				(index == IDC_EOSSSUMMARY_COMMAND8) ||
				(index == IDC_EOSSSUMMARY_COMMAND7) ||
				(index == IDC_EOSSSUMMARY_COMMAND6) ||
				(index == IDC_EOSSSUMMARY_COMMAND5) ||
				(index == IDC_EOSSSUMMARY_COMMAND4) ||
				(index == IDC_EOSSSUMMARY_COMMAND3) ||
				(index == IDC_EOSSSUMMARY_COMMAND2) ||
				(index == IDC_EOSSSUMMARY_COMMAND1))
			{
				char Text[256];
				Text[0] = NULL;
				GetDlgItemText(index,Text,sizeof(Text));
				if (strlen(Text))
				{
					strcat(Text,"\"");
					memmove(&Text[2],Text,strlen(Text)+1);
					Text[0] = '"';
					Text[1] = 'E';
					_spawnl(_P_NOWAIT,
						m_pParent->m_szUtilFile,"MsgUtil.exe",Text,NULL);
				}
			}
		}
	}

	CPropertyPage::OnLButtonDown(nFlags, point);
}



void CEOSSSummary::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if (!m_bCheckerCreated  && ::IsWindow(m_hWnd))
	{
		m_bCheckerCreated = true;
		CRect rect;
		GetDlgItem(IDC_STATIC_CHECKER)->GetWindowRect(rect);
		ScreenToClient(rect);
		m_ctrlChecker.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP, rect, this, IDC_STATIC_CHECKER);
		m_ctrlChecker.SetTotalBlocks(CELLSX,CELLSY,0);
		PopulateChecker();
	}
	if (::IsWindow(m_ctrlChecker.m_hWnd) && bShow)
	{
		PopulateChecker();
		m_ctrlChecker.Refresh();
	}
}

void CEOSSSummary::OnButtonEdit() 
{
	//get all the stuff and save it
	if (m_pParent->Validate())
	{
		bool bReturn = m_pPropertySheet->EditEOSSList();
		//if true do what ever is necessary at this level
		if (bReturn)
		{
			PopulateChecker();
			m_pPropertySheet->RefreshDataDisplay();
		}
	}
}
