// UGINFO.cpp : implementation file
//

#include "stdafx.h"
#include "cougrandiso.h"
#include "UGINFO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUGINFO property page

IMPLEMENT_DYNCREATE(CUGINFO, CPropertyPage)

CUGINFO::CUGINFO() : CPropertyPage(CUGINFO::IDD)
{
	//{{AFX_DATA_INIT(CUGINFO)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CUGINFO::~CUGINFO()
{
}

CUGINFO::CUGINFO(UINT id) : CPropertyPage(id)// CPropertyPage(CPSBInfo::IDD)
{
	m_pPropertySheet = NULL;
	//{{AFX_DATA_INIT(CPSBInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CUGINFO::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CUGINFO::SetText(int index,CHAR* text)
{
	if ((index >= IDC_UGINFO_FIRST) &&
		(index <= IDC_UGINFO_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CUGINFO::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_UGINFO_FIRST) &&
		(index <= IDC_UGINFO_LAST)) {
	  StaticColors[index-IDC_UGINFO_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CUGINFO::ResetColors()
{
	for (int i= IDC_UGINFO_FIRST; i <= IDC_UGINFO_LAST; i++)
	{
		StaticColors[i-IDC_UGINFO_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void CUGINFO::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUGINFO)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUGINFO, CPropertyPage)
	//{{AFX_MSG_MAP(CUGINFO)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUGINFO message handlers
HBRUSH CUGINFO::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_UGINFO_FIRST) && (index <= IDC_UGINFO_LAST))
	{
		int iFirstOfList = IDC_UGINFO_FIRST; 
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

void CUGINFO::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)
		m_pPropertySheet->SetToClose(0);

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CUGINFO::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
}

void CUGINFO::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Do not call CPropertyPage::OnPaint() for painting messages
}


