// PSBInfo.cpp : implementation file
//

#include "stdafx.h"
#include "cougrandiso.h"
#include "PSBInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPSBInfo property page

IMPLEMENT_DYNCREATE(CPSBInfo, CPropertyPage)

CPSBInfo::CPSBInfo() : CPropertyPage(CPSBInfo::IDD)
{
	//{{AFX_DATA_INIT(CPSBInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPSBInfo::~CPSBInfo()
{
}

CPSBInfo::CPSBInfo(UINT id) : CPropertyPage(id)// CPropertyPage(CPSBInfo::IDD)
{
	m_pPropertySheet = NULL;
	//{{AFX_DATA_INIT(CPSBInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CPSBInfo::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CPSBInfo::SetText(int index,CHAR* text)
{
	if ((index >= IDC_PSBINFO_FIRST) &&
		(index <= IDC_PSBINFO_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CPSBInfo::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_PSBINFO_FIRST) &&
		(index <= IDC_PSBINFO_LAST)) {
	  StaticColors[index-IDC_PSBINFO_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CPSBInfo::ResetColors()
{
	for (int i= IDC_PSBINFO_FIRST; i <= IDC_PSBINFO_LAST; i++)
	{
		StaticColors[i-IDC_PSBINFO_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void CPSBInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSBInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPSBInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CPSBInfo)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSBInfo message handlers

HBRUSH CPSBInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_PSBINFO_FIRST) && (index <= IDC_PSBINFO_LAST))
	{
		int iFirstOfList = IDC_PSBINFO_FIRST; 
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

void CPSBInfo::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)
		m_pPropertySheet->SetToClose(0);

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CPSBInfo::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
}

void CPSBInfo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Do not call CPropertyPage::OnPaint() for painting messages
}


