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
// VACOSSDataStat.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "VACOSSDataStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVACOSSDataStat property page

IMPLEMENT_DYNCREATE(CVACOSSDataStat, CPropertyPage)

CVACOSSDataStat::CVACOSSDataStat() : CPropertyPage(CVACOSSDataStat::IDD)
{
}

CVACOSSDataStat::CVACOSSDataStat(UINT id) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	//initialize all displayed (cstatic) text to the
	//default color (black)
	for (int i= IDC_VACOSSDATA_FIRST; i <= IDC_VACOSSDATA_LAST; i++)
	{
		StaticColors[i-IDC_VACOSSDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CVACOSSStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void
CVACOSSDataStat::SetPropertySheet(CEventPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}


void
CVACOSSDataStat::SetText(int index, char * text)
{
	//set the cstatic to the provided text
	if ((index >= IDC_VACOSSDATA_FIRST) &&
		(index <= IDC_VACOSSDATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *
CVACOSSDataStat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_VACOSSDATA_FIRST) &&
		(index <= IDC_VACOSSDATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void
CVACOSSDataStat::SetColor(int index, COLORREF color)
{
	//set the current color of a cstatic
	//it will get updated via onctlcolor()
	if ((index >= IDC_VACOSSDATA_FIRST) &&
		(index <= IDC_VACOSSDATA_LAST)) {
	  StaticColors[index-IDC_VACOSSDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CVACOSSDataStat::ResetColors()
{
	for (int i= IDC_VACOSSDATA_FIRST; i <= IDC_VACOSSDATA_LAST; i++)
	{
		StaticColors[i-IDC_VACOSSDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void CVACOSSDataStat::ShiftRight()
{
	char temp[64];

	SetDlgItemText(IDC_VACOSSDATA_DATADATE4,
		GetText(IDC_VACOSSDATA_DATADATE3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_DATADATE3,
		GetText(IDC_VACOSSDATA_DATADATE2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_DATADATE2,
		GetText(IDC_VACOSSDATA_DATADATE1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_DATADATE1,"");

	SetDlgItemText(IDC_VACOSSDATA_DATATIME4,
		GetText(IDC_VACOSSDATA_DATATIME3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_DATATIME3,
		GetText(IDC_VACOSSDATA_DATATIME2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_DATATIME2,
		GetText(IDC_VACOSSDATA_DATATIME1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_DATATIME1,"");

	SetDlgItemText(IDC_VACOSSDATA_NODE4,
		GetText(IDC_VACOSSDATA_NODE3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_NODE3,
		GetText(IDC_VACOSSDATA_NODE2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_NODE2,
		GetText(IDC_VACOSSDATA_NODE1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_NODE1,"");

	SetDlgItemText(IDC_VACOSSDATA_SEALID4,
		GetText(IDC_VACOSSDATA_SEALID3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_SEALID3,
		GetText(IDC_VACOSSDATA_SEALID2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_SEALID2,
		GetText(IDC_VACOSSDATA_SEALID1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_SEALID1,"");

	SetDlgItemText(IDC_VACOSSDATA_TYPE4,
		GetText(IDC_VACOSSDATA_TYPE3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_TYPE3,
		GetText(IDC_VACOSSDATA_TYPE2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_TYPE2,
		GetText(IDC_VACOSSDATA_TYPE1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_TYPE1,"");

	SetDlgItemText(IDC_VACOSSDATA_ERRDATE4,
		GetText(IDC_VACOSSDATA_ERRDATE3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_ERRDATE3,
		GetText(IDC_VACOSSDATA_ERRDATE2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_ERRDATE2,
		GetText(IDC_VACOSSDATA_ERRDATE1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_ERRDATE1,"");

	SetDlgItemText(IDC_VACOSSDATA_RESTIME4,
		GetText(IDC_VACOSSDATA_RESTIME3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_RESTIME3,
		GetText(IDC_VACOSSDATA_RESTIME2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_RESTIME2,
		GetText(IDC_VACOSSDATA_RESTIME1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_RESTIME1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT01_4,
		GetText(IDC_VACOSSDATA_EVENT01_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT01_3,
		GetText(IDC_VACOSSDATA_EVENT01_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT01_2,
		GetText(IDC_VACOSSDATA_EVENT01_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT01_1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT02_4,
		GetText(IDC_VACOSSDATA_EVENT02_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT02_3,
		GetText(IDC_VACOSSDATA_EVENT02_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT02_2,
		GetText(IDC_VACOSSDATA_EVENT02_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT02_1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT03_4,
		GetText(IDC_VACOSSDATA_EVENT03_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT03_3,
		GetText(IDC_VACOSSDATA_EVENT03_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT03_2,
		GetText(IDC_VACOSSDATA_EVENT03_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT03_1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT04_4,
		GetText(IDC_VACOSSDATA_EVENT04_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT04_3,
		GetText(IDC_VACOSSDATA_EVENT04_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT04_2,
		GetText(IDC_VACOSSDATA_EVENT04_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT04_1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT05_4,
		GetText(IDC_VACOSSDATA_EVENT05_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT05_3,
		GetText(IDC_VACOSSDATA_EVENT05_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT05_2,
		GetText(IDC_VACOSSDATA_EVENT05_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT05_1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT06_4,
		GetText(IDC_VACOSSDATA_EVENT06_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT06_3,
		GetText(IDC_VACOSSDATA_EVENT06_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT06_2,
		GetText(IDC_VACOSSDATA_EVENT06_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT06_1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT07_4,
		GetText(IDC_VACOSSDATA_EVENT07_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT07_3,
		GetText(IDC_VACOSSDATA_EVENT07_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT07_2,
		GetText(IDC_VACOSSDATA_EVENT07_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT07_1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT08_4,
		GetText(IDC_VACOSSDATA_EVENT08_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT08_3,
		GetText(IDC_VACOSSDATA_EVENT08_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT08_2,
		GetText(IDC_VACOSSDATA_EVENT08_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT08_1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT09_4,
		GetText(IDC_VACOSSDATA_EVENT09_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT09_3,
		GetText(IDC_VACOSSDATA_EVENT09_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT09_2,
		GetText(IDC_VACOSSDATA_EVENT09_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT09_1,"");

	SetDlgItemText(IDC_VACOSSDATA_EVENT10_4,
		GetText(IDC_VACOSSDATA_EVENT10_3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT10_3,
		GetText(IDC_VACOSSDATA_EVENT10_2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT10_2,
		GetText(IDC_VACOSSDATA_EVENT10_1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_EVENT10_1,"");

	SetDlgItemText(IDC_VACOSSDATA_LRC4,
		GetText(IDC_VACOSSDATA_LRC3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_LRC3,
		GetText(IDC_VACOSSDATA_LRC2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_LRC2,
		GetText(IDC_VACOSSDATA_LRC1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_LRC1,"");

	SetDlgItemText(IDC_VACOSSDATA_SIG4,
		GetText(IDC_VACOSSDATA_SIG3,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_SIG3,
		GetText(IDC_VACOSSDATA_SIG2,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_SIG2,
		GetText(IDC_VACOSSDATA_SIG1,temp,sizeof(temp)));
	SetDlgItemText(IDC_VACOSSDATA_SIG1,"");
}

CVACOSSDataStat::~CVACOSSDataStat()
{
}

void CVACOSSDataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVACOSSDataStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVACOSSDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CVACOSSDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVACOSSDataStat message handlers
HBRUSH CVACOSSDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	//get the id of the control 
	UINT index = pWnd->GetDlgCtrlID();
	//set the font and color of that control
	if ((index >= IDC_VACOSSDATA_FIRST) && (index <= IDC_VACOSSDATA_LAST))
	{
		int iFirstOfList = IDC_VACOSSDATA_FIRST; 
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
void CVACOSSDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}
