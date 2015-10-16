// ISRUserProg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "ISRUserProg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CISRUserProg property page

IMPLEMENT_DYNCREATE(CISRUserProg, CPropertyPage)

CISRUserProg::CISRUserProg() : CPropertyPage(CISRUserProg::IDD)
{
}


CISRUserProg::CISRUserProg(UINT id) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	for (int i= IDC_ISRUSERPROG_FIRST; i <= IDC_ISRUSERPROG_LAST; i++)
	{
		StaticColors[i-IDC_ISRUSERPROG_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CUserProg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CISRUserProg::SetPropertySheet(CISRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CISRUserProg::SetText(int index,CHAR * text)
{
	if ((index >= IDC_ISRUSERPROG_FIRST) &&
		(index <= IDC_ISRUSERPROG_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CISRUserProg::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_ISRUSERPROG_FIRST) &&
		(index <= IDC_ISRUSERPROG_LAST)) 
	{
	  StaticColors[index-IDC_ISRUSERPROG_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

CISRUserProg::~CISRUserProg()
{
}

void CISRUserProg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CISRUserProg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CISRUserProg, CPropertyPage)
	//{{AFX_MSG_MAP(CISRUserProg)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserProg message handlers
HBRUSH CISRUserProg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_ISRUSERPROG_FIRST) && (index <= IDC_ISRUSERPROG_LAST))
	{
		int iFirstOfList = IDC_ISRUSERPROG_FIRST; 
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
void CISRUserProg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)
		m_pPropertySheet->SetToClose(0);

	CPropertyPage::OnLButtonDown(nFlags, point);
}


void CISRUserProg::IssuePrint(FILE* outfile)
{
#ifdef _DEBUG
	AfxMessageBox("CISRUserProg::IssuePrint() unimplemented\n");
#endif
}