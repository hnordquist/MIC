// CISRDData.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "ISRCDData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CISRCDData property page

IMPLEMENT_DYNCREATE(CISRCDData, CPropertyPage)

CISRCDData::CISRCDData() : CPropertyPage(CISRCDData::IDD)
{
}


CISRCDData::CISRCDData(UINT id) : CPropertyPage(id)
{
	for (int i= IDC_CDDATA_FIRST; i <= IDC_CDDATA_LAST; i++)
	{
		StaticColors[i-IDC_CDDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CCDData)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CISRCDData::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_CDDATA_FIRST) &&
		(index <= IDC_CDDATA_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CISRCDData::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_CDDATA_FIRST) &&
		(index <= IDC_CDDATA_LAST)) {
	  StaticColors[index-IDC_CDDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

CISRCDData::~CISRCDData()
{
}

void CISRCDData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCDData)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CISRCDData, CPropertyPage)
	//{{AFX_MSG_MAP(CISRCDData)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CISRCDData message handlers

HBRUSH CISRCDData::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_CDDATA_FIRST) && (index <= IDC_CDDATA_LAST))
	{
		int iFirstOfList = IDC_CDDATA_FIRST; 
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

	return hbr;
}

