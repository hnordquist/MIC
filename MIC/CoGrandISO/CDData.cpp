// CDData.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "CDData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCDData property page

IMPLEMENT_DYNCREATE(CCDData, CPropertyPage)

CCDData::CCDData() : CPropertyPage(CCDData::IDD)
{
}

//constructor for "Change Data" propertypage of GRAND instruments
CCDData::CCDData(UINT id) : CPropertyPage(id)// CPropertyPage(CGenlStat::IDD)
{
	//setup for "bold" and "color" text
	for (int i= IDC_CDDATA_FIRST; i <= IDC_CDDATA_LAST; i++)
	{
		StaticColors[i-IDC_CDDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CCDData)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//set a cstatic on this page to the provided text 
void
CCDData::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_CDDATA_FIRST) &&
		(index <= IDC_CDDATA_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

//set the color of a cstatic -- the actual work will
//be done when redrawn through the ON_WM_CTLCOLOR msg
void
CCDData::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_CDDATA_FIRST) &&
		(index <= IDC_CDDATA_LAST)) {
	  StaticColors[index-IDC_CDDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

CCDData::~CCDData()
{
}

void CCDData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCDData)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCDData, CPropertyPage)
	//{{AFX_MSG_MAP(CCDData)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCDData message handlers
//set the color of each of the cstatic controls
HBRUSH CCDData::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
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

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

