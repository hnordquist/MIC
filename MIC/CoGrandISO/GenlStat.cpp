// GenlStat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "GenlStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenlStat property page

IMPLEMENT_DYNCREATE(CGenlStat, CPropertyPage)

CGenlStat::CGenlStat(): CPropertyPage(CGenlStat::IDD)
{
}


CGenlStat::CGenlStat(UINT id) : CPropertyPage(id)// CPropertyPage(CGenlStat::IDD)
{
	m_pPropertySheet = NULL;
	//initialize all displayed (cstatic) text to the
	//default color (black)
//	for (int i= IDC_GENERAL_FIRST; i <= IDC_GENERAL_LAST; i++)
//	{
//		StaticColors[i-IDC_GENERAL_FIRST] = COLOR_DEFAULT_TEXT;
//	}
	//{{AFX_DATA_INIT(CGenlStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CGenlStat::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CGenlStat::SetText(int index,CHAR* text)
{
	if ((index >= IDC_GENERAL_FIRST) &&
		(index <= IDC_GENERAL_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CGenlStat::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_GENERAL_FIRST) &&
		(index <= IDC_GENERAL_LAST)) {
	  StaticColors[index-IDC_GENERAL_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CGenlStat::ResetColors()
{
	for (int i= IDC_GENERAL_FIRST; i <= IDC_GENERAL_LAST; i++)
	{
		StaticColors[i-IDC_GENERAL_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CGenlStat::~CGenlStat()
{
}

void CGenlStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenlStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenlStat, CPropertyPage)
	//{{AFX_MSG_MAP(CGenlStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenlStat message handlers

HBRUSH CGenlStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_GENERAL_FIRST) && (index <= IDC_GENERAL_LAST))
	{
		int iFirstOfList = IDC_GENERAL_FIRST; 
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

void CGenlStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CGenlStat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here

	CString *csversion = m_pPropertySheet->GetVersionString();
	CString cs = "Version " + *csversion + " " + __DATE__;
	SetDlgItemText(IDC_GENERAL_STATIC_ADVERT, cs);

#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG_GRAND, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG_GRAND, "Release Build");
#endif

	
}
