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
// MCAGenlStat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "MCAGenlStat.h"
#include "MCAInst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMCAGenlStat property page

IMPLEMENT_DYNCREATE(CMCAGenlStat, CPropertyPage)

CMCAGenlStat::CMCAGenlStat(): CPropertyPage(CMCAGenlStat::IDD)
{
}


CMCAGenlStat::CMCAGenlStat(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_MCAGENERAL_FIRST; i <= IDC_MCAGENERAL_LAST; i++)
	{
		StaticColors[i-IDC_MCAGENERAL_FIRST] = COLOR_DEFAULT_TEXT;
	}
}


void CMCAGenlStat::SetPropertySheet(CMCAPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CMCAGenlStat::SetText(int index,CHAR* text)
{
	if ((index >= IDC_MCAGENERAL_FIRST) &&
		(index <= IDC_MCAGENERAL_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CMCAGenlStat::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_MCAGENERAL_FIRST) &&
		(index <= IDC_MCAGENERAL_LAST)) 
	{
	  StaticColors[index-IDC_MCAGENERAL_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CMCAGenlStat::ResetColors()
{
	for (int i= IDC_MCAGENERAL_FIRST; i <= IDC_MCAGENERAL_LAST; i++)
	{
		StaticColors[i-IDC_MCAGENERAL_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CMCAGenlStat::~CMCAGenlStat()
{
}

void CMCAGenlStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCAGenlStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCAGenlStat, CPropertyPage)
	//{{AFX_MSG_MAP(CMCAGenlStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenlStat message handlers

HBRUSH CMCAGenlStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_MCAGENERAL_FIRST) && (index <= IDC_MCAGENERAL_LAST))
	{
		int iFirstOfList = IDC_MCAGENERAL_FIRST; 
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

void CMCAGenlStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

BOOL CMCAGenlStat::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	char *name = m_pParent->m_pName;
	char *inifile = m_pParent->m_szIniFile;
	
	char buff[16];
	GetPrivateProfileString(name,"ALARM1","",buff,sizeof(buff),inifile);
	if (strlen(buff)) SetDlgItemText(IDC_STATIC_ALARM1,buff);
	GetPrivateProfileString(name,"ALARM2","",buff,sizeof(buff),inifile);
	if (strlen(buff)) SetDlgItemText(IDC_STATIC_ALARM2,buff);
	GetPrivateProfileString(name,"ALARM3","",buff,sizeof(buff),inifile);
	if (strlen(buff)) SetDlgItemText(IDC_STATIC_ALARM3,buff);
	GetPrivateProfileString(name,"ALARM4","",buff,sizeof(buff),inifile);
	if (strlen(buff)) SetDlgItemText(IDC_STATIC_ALARM4,buff);
	GetPrivateProfileString(name,"ALARM5","",buff,sizeof(buff),inifile);
	if (strlen(buff)) SetDlgItemText(IDC_STATIC_ALARM5,buff);
	GetPrivateProfileString(name,"ALARM6","",buff,sizeof(buff),inifile);
	if (strlen(buff)) SetDlgItemText(IDC_STATIC_ALARM6,buff);
	GetPrivateProfileString(name,"ALARM7","",buff,sizeof(buff),inifile);
	if (strlen(buff)) SetDlgItemText(IDC_STATIC_ALARM7,buff);

	return TRUE;
}


void CMCAGenlStat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here

	CString *csversion = m_pPropertySheet->GetVersionString();
	CString cs = "Version " + *csversion + " " + __DATE__;
	SetDlgItemText(IDC_STATIC_ADVERT, cs);

#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG, "Release Build");
#endif	
}
