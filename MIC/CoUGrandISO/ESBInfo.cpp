// ESBInfo.cpp : implementation file
//

#include "stdafx.h"
#include "cougrandiso.h"
#include "ESBInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CESBInfo property page

IMPLEMENT_DYNCREATE(CESBInfo, CPropertyPage)

CESBInfo::CESBInfo() : CPropertyPage(CESBInfo::IDD)
{
	//{{AFX_DATA_INIT(CESBInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CESBInfo::~CESBInfo()
{
}

CESBInfo::CESBInfo(UINT id) : CPropertyPage(id)// CPropertyPage(CESBInfo::IDD)
{
	m_pPropertySheet = NULL;
	//{{AFX_DATA_INIT(CESBInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CESBInfo::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CESBInfo::SetText(int index,CHAR* text)
{
	if ((index >= IDC_ESBINFO_FIRST) &&
		(index <= IDC_ESBINFO_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CESBInfo::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_ESBINFO_FIRST) &&
		(index <= IDC_ESBINFO_LAST)) {
	  StaticColors[index-IDC_ESBINFO_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CESBInfo::ResetColors()
{
	for (int i= IDC_ESBINFO_FIRST; i <= IDC_ESBINFO_LAST; i++)
	{
		StaticColors[i-IDC_ESBINFO_FIRST] = COLOR_DEFAULT_TEXT;
	}
}


void CESBInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CESBInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CESBInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CESBInfo)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CESBInfo message handlers
HBRUSH CESBInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_ESBINFO_FIRST) && (index <= IDC_ESBINFO_LAST))
	{
		int iFirstOfList = IDC_ESBINFO_FIRST; 
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

void CESBInfo::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)
		m_pPropertySheet->SetToClose(0);

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CESBInfo::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
}

void CESBInfo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Do not call CPropertyPage::OnPaint() for painting messages
}
