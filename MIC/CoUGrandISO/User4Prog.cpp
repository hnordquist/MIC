// User4Prog.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "User4Prog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUser4Prog property page

IMPLEMENT_DYNCREATE(CUser4Prog, CPropertyPage)

CUser4Prog::CUser4Prog() : CPropertyPage(CUser4Prog::IDD)
{
}

CUser4Prog::CUser4Prog(UINT id) : CPropertyPage(id)// CPropertyPage(CUser4Prog::IDD)
{
	m_pPropertySheet = NULL;
//	for (int i= IDC_USER4PROG_FIRST; i <= IDC_USER4PROG_LAST; i++)
//	{
//		StaticColors[i-IDC_USER4PROG_FIRST] = COLOR_DEFAULT_TEXT;
//	}
	//{{AFX_DATA_INIT(CUser4Prog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CUser4Prog::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CUser4Prog::SetText(int index,CHAR * text)
{
	if ((index >= IDC_USER4PROG_FIRST) && (index <= IDC_USER4PROG_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CUser4Prog::ResetColors()
{
	for (int i= IDC_USER4PROG_FIRST; i <= IDC_USER4PROG_LAST; i++)
	{
		StaticColors[i-IDC_USER4PROG_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void
CUser4Prog::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_USER4PROG_FIRST) &&
		(index <= IDC_USER4PROG_LAST)) {
	  StaticColors[index-IDC_USER4PROG_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

CUser4Prog::~CUser4Prog()
{
}

void CUser4Prog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUser4Prog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUser4Prog, CPropertyPage)
	//{{AFX_MSG_MAP(CUser4Prog)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUser4Prog message handlers
HBRUSH CUser4Prog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_USER4PROG_FIRST) && (index <= IDC_USER4PROG_LAST))
	{
//		pDC->SetTextColor(StaticColors[index-IDC_USER4PROG_FIRST]);
//		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
		int iFirstOfList = IDC_USER4PROG_FIRST; 
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
void CUser4Prog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CUser4Prog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here
//causes the tab bar to shift to the right
//	if (m_pPropertySheet->m_pcWndSpin && bShow)
//	{
//
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONDOWN,1,0x00000000);
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONUP,1,  0x00000000);
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONDOWN,1,0x00000000);
//         m_pPropertySheet->m_pcWndSpin->PostMessage(WM_LBUTTONUP,1,  0x00000000);
//	}
	
}

