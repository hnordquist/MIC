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
// DataStat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "DataStat.h"
#include "Instrument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataStat property page

IMPLEMENT_DYNCREATE(CDataStat, CPropertyPage)

CDataStat::CDataStat() : CPropertyPage(CDataStat::IDD)
{
}

CDataStat::CDataStat(UINT id) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;

	//{{AFX_DATA_INIT(CDataStat)
	m_bCheckVector[0] = FALSE;
	m_bCheckVector[1] = FALSE;
	m_bCheckVector[2] = FALSE;
	m_bCheckVector[3] = FALSE;
	m_bCheckVector[4] = FALSE;
	m_bCheckVector[5] = FALSE;
	m_bCheckVector[6] = FALSE;
	m_bCheckVector[7] = FALSE;
	m_bCheckVector[8] = FALSE;
	m_bCheckVector[9] = FALSE;
	m_bCheckVector[10] = FALSE;
	m_bCheckVector[11] = FALSE;
	m_bCheckVector[12] = FALSE;
	m_bCheckVector[13] = FALSE;
	//}}AFX_DATA_INIT
}
void
CDataStat::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}


void
CDataStat::SetText(int index, char * text)
{
	//set the cstatic to the provided text
	if ((index >= IDC_DATA_FIRST) &&
		(index <= IDC_DATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *
CDataStat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_DATA_FIRST) &&
		(index <= IDC_DATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void
CDataStat::SetColor(int index, COLORREF color)
{
	//set the current color of a cstatic
	//it will get updated via onctlcolor()
	if ((index >= IDC_DATA_FIRST) &&
		(index <= IDC_DATA_LAST)) {
	  StaticColors[index-IDC_DATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CDataStat::ResetColors()
{
	for (int i= IDC_DATA_FIRST; i <= IDC_DATA_LAST; i++)
	{
		StaticColors[i-IDC_DATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CDataStat::~CDataStat()
{
}

void CDataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataStat)
	DDX_Check(pDX, IDC_CHECKC0CR, m_bCheckVector[0]);
	DDX_Check(pDX, IDC_CHECKC0SCR, m_bCheckVector[1]);
	DDX_Check(pDX, IDC_CHECKC0LCR, m_bCheckVector[2]);
	DDX_Check(pDX, IDC_CHECKC1CR, m_bCheckVector[3]);
	DDX_Check(pDX, IDC_CHECKC1SCR, m_bCheckVector[4]);
	DDX_Check(pDX, IDC_CHECKC1LCR, m_bCheckVector[5]);
	DDX_Check(pDX, IDC_CHECKC2CR, m_bCheckVector[6]);
	DDX_Check(pDX, IDC_CHECKC2SCR, m_bCheckVector[7]);
	DDX_Check(pDX, IDC_CHECKC2LCR, m_bCheckVector[8]);
	DDX_Check(pDX, IDC_CHECKC3CR, m_bCheckVector[9]);
	DDX_Check(pDX, IDC_CHECKC3SCR, m_bCheckVector[10]);
	DDX_Check(pDX, IDC_CHECKC3LCR, m_bCheckVector[11]);
	DDX_Check(pDX, IDC_CHECKCPUTEMP,m_bCheckVector[12]);
	DDX_Check(pDX, IDC_CHECKCPUTEMP2,m_bCheckVector[13]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHECKC0CR, OnCheckc0cr)
	ON_BN_CLICKED(IDC_CHECKC0SCR, OnCheckc0scr)
	ON_BN_CLICKED(IDC_CHECKC0LCR, OnCheckc0lcr)
	ON_BN_CLICKED(IDC_CHECKC1CR, OnCheckc1cr)
	ON_BN_CLICKED(IDC_CHECKC1LCR, OnCheckc1lcr)
	ON_BN_CLICKED(IDC_CHECKC1SCR, OnCheckc1scr)
	ON_BN_CLICKED(IDC_CHECKC2CR, OnCheckc2cr)
	ON_BN_CLICKED(IDC_CHECKC2LCR, OnCheckc2lcr)
	ON_BN_CLICKED(IDC_CHECKC2SCR, OnCheckc2scr)
	ON_BN_CLICKED(IDC_CHECKC3CR, OnCheckc3cr)
	ON_BN_CLICKED(IDC_CHECKC3LCR, OnCheckc3lcr)
	ON_BN_CLICKED(IDC_CHECKC3SCR, OnCheckc3scr)
	ON_BN_CLICKED(IDC_CHECKCPUTEMP2, OnCheckcputemp2)
	ON_BN_CLICKED(IDC_CHECKCPUTEMP, OnCheckcputemp)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataStat message handlers
HBRUSH CDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	//get the id of the control 
	UINT index = pWnd->GetDlgCtrlID();
	//set the font and color of that control
	if ((index >= IDC_DATA_FIRST) && (index <= IDC_DATA_LAST))
	{
		int iFirstOfList = IDC_DATA_FIRST; 
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
void CDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CDataStat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here
//	if (m_pPropertySheet->m_pcWndSpin && bShow)
//	{
//
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONDOWN,1,0x00000000);
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONUP,1,  0x00000000);
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONDOWN,1,0x00000000);
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONUP,1,  0x00000000);
//	}
}

int CDataStat::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_cVertFont.CreateFont(0,0,900,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Arial");
		
	return 0;
}

void CDataStat::CountChecks(int nID)
{
	int iCount = 0;
	for (int i = 0; i < 16; i++) m_bCheckVector[i] = FALSE;
	int iActive = -1;
	i = 0;
	if (((CButton*)GetDlgItem(IDC_CHECKC0CR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC0CR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC0SCR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC0SCR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC0LCR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC0LCR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC1CR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC1CR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC1SCR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC1SCR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC1LCR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC1LCR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC2CR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC2CR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC2SCR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC2SCR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC2LCR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC2LCR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC3CR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC3CR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC3SCR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC3SCR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKC3LCR))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKC3LCR == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKCPUTEMP))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKCPUTEMP == nID) iActive = i;
	}
	i++;
	if (((CButton*)GetDlgItem(IDC_CHECKCPUTEMP2))->GetCheck()==BST_CHECKED)
	{
		iCount++;
		m_bCheckVector[i] = true;
		if (IDC_CHECKCPUTEMP2 == nID) iActive = i;
	}

	if (iCount > 5)
	{
		((CButton*)GetDlgItem(nID))->SetCheck(BST_UNCHECKED);
		if (iActive >= 0)
			m_bCheckVector[iActive] = false;
	}
	
	char cTemp[16];
	for (i = 0; i < 14; i++)
		cTemp[i] = m_bCheckVector[i]?'1':'0';
	cTemp[14] = NULL;
	WritePrivateProfileString(m_pPropertySheet->m_pParent->m_pName,"DATADISPLAYVECTOR",cTemp,m_pPropertySheet->m_szIniFile);
}

void CDataStat::OnCheckc0cr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC0CR);
}

void CDataStat::OnCheckc0scr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC0SCR);
}

void CDataStat::OnCheckc0lcr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC0LCR);
}

void CDataStat::OnCheckc1cr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC1CR);
}

void CDataStat::OnCheckc1lcr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC1LCR);
}

void CDataStat::OnCheckc1scr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC1SCR);
}

void CDataStat::OnCheckc2cr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC2CR);
}

void CDataStat::OnCheckc2lcr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC2LCR);
}

void CDataStat::OnCheckc2scr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC2SCR);
}

void CDataStat::OnCheckc3cr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC3CR);
}

void CDataStat::OnCheckc3lcr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC3LCR);
}

void CDataStat::OnCheckc3scr() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKC3SCR);
}

void CDataStat::OnCheckcputemp() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKCPUTEMP);
}

void CDataStat::OnCheckcputemp2() 
{
	// TODO: Add your control notification handler code here
	CountChecks(IDC_CHECKCPUTEMP2);
}

void CDataStat::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CFont* oldfont = (CFont*)dc.SelectObject(&m_cVertFont);
	dc.TextOut(-3,360,"Select up to 5 for Recent Data Display");
	dc.SelectObject(oldfont);
	// Do not call CPropertyPage::OnPaint() for painting messages
}
