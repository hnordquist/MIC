// ISRGenlStat.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "ISRGenlStat.h"
#include "ISRSafePS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CISRGenlStat property page

IMPLEMENT_DYNCREATE(CISRGenlStat, CPropertyPage)

CISRGenlStat::CISRGenlStat(): CPropertyPage(CISRGenlStat::IDD)
{
}

CISRGenlStat::CISRGenlStat(UINT id) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;

	for (int i= IDC_ISRSETUP_FIRST; i <= IDC_ISRSETUP_LAST; i++)
	{
		StaticColors[i-IDC_ISRSETUP_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CISRGenlStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CISRGenlStat::SetPropertySheet(CISRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CISRGenlStat::SetText(int index,CHAR* text)
{
	if ((index >= IDC_ISRSETUP_FIRST) &&
		(index <= IDC_ISRSETUP_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CISRGenlStat::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_ISRSETUP_FIRST) &&
		(index <= IDC_ISRSETUP_LAST)) {
	  StaticColors[index-IDC_ISRSETUP_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CISRGenlStat::ResetColors()
{
	for (int i= IDC_ISRSETUP_FIRST; i <= IDC_ISRSETUP_LAST; i++)
	{
		StaticColors[i-IDC_ISRSETUP_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CISRGenlStat::~CISRGenlStat()
{
}

void CISRGenlStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CISRGenlStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CISRGenlStat, CPropertyPage)
	//{{AFX_MSG_MAP(CISRGenlStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenlStat message handlers

HBRUSH CISRGenlStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_ISRSETUP_FIRST) && (index <= IDC_ISRSETUP_LAST))
	{
		int iFirstOfList = IDC_ISRSETUP_FIRST; 
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
void CISRGenlStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CISRGenlStat::IssuePrint(FILE *outfile)
{
	//<<QA>> No IssuePrint in ISRGenlStat
#ifdef _DEBUG
	AfxMessageBox("CISRGenlStat::IssuePrint() unimplemented\n");
#endif
}

void CISRGenlStat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here

	CString *csversion = m_pPropertySheet->GetVersionString();
	CString cs = "Version " + *csversion + " " + __DATE__;
	SetDlgItemText(IDC_GENERAL_STATIC_ADVERT, cs);

#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG, "Release Build");
#endif	
}
