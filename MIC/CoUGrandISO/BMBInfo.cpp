// BMBInfo.cpp : implementation file
//

#include "stdafx.h"
#include "cougrandiso.h"
#include "BMBInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBMBInfo property page

IMPLEMENT_DYNCREATE(CBMBInfo, CPropertyPage)

CBMBInfo::CBMBInfo() : CPropertyPage(CBMBInfo::IDD)
{
	//{{AFX_DATA_INIT(CBMBInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CBMBInfo::~CBMBInfo()
{
}

CBMBInfo::CBMBInfo(UINT id) : CPropertyPage(id)// CPropertyPage(CBMBInfo::IDD)
{
	m_pPropertySheet = NULL;
	//{{AFX_DATA_INIT(CBMBInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CBMBInfo::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CBMBInfo::SetText(int index,CHAR* text)
{
	if ((index >= IDC_BMBINFO_FIRST) &&
		(index <= IDC_BMBINFO_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CBMBInfo::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_BMBINFO_FIRST) &&
		(index <= IDC_BMBINFO_LAST)) {
	  StaticColors[index-IDC_BMBINFO_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CBMBInfo::ResetColors()
{
	for (int i= IDC_BMBINFO_FIRST; i <= IDC_BMBINFO_LAST; i++)
	{
		StaticColors[i-IDC_BMBINFO_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void CBMBInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBMBInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBMBInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CBMBInfo)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBMBInfo message handlers
HBRUSH CBMBInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_BMBINFO_FIRST) && (index <= IDC_BMBINFO_LAST))
	{
		int iFirstOfList = IDC_BMBINFO_FIRST; 
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

void CBMBInfo::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)
		m_pPropertySheet->SetToClose(0);

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CBMBInfo::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
}

void CBMBInfo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Do not call CPropertyPage::OnPaint() for painting messages
}
