// MCAUserProg.cpp


#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "MCAUserProg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CMCAUserProg property page

IMPLEMENT_DYNCREATE(CMCAUserProg, CPropertyPage)

CMCAUserProg::CMCAUserProg() : CPropertyPage(CMCAUserProg::IDD)
{
}


CMCAUserProg::CMCAUserProg(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pParent = parent;
	m_pPropertySheet = NULL;

	for (int i= IDC_USERPROG_FIRST; i <= IDC_USERPROG_LAST; i++)
	{
		StaticColors[i-IDC_USERPROG_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CUserProg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CMCAUserProg::SetText(int index,CHAR * text)
{
	if ((index >= IDC_USERPROG_FIRST) &&
		(index <= IDC_USERPROG_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CMCAUserProg::SetPropertySheet(CMCAPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}
void
CMCAUserProg::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_USERPROG_FIRST) &&
		(index <= IDC_USERPROG_LAST)) {
	  StaticColors[index-IDC_USERPROG_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

CMCAUserProg::~CMCAUserProg()
{
}

void CMCAUserProg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserProg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCAUserProg, CPropertyPage)
	//{{AFX_MSG_MAP(CMCAUserProg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserProg message handlers
HBRUSH CMCAUserProg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_USERPROG_FIRST) && (index <= IDC_USERPROG_LAST))
	{
		int iFirstOfList = IDC_USERPROG_FIRST; 
		if (StaticColors[index-iFirstOfList]==COLOR_RED)
		{
			pDC->SetBkColor(COLOR_RED);
			pDC->SetTextColor(RGB(255,255,255));
		}
		else if (StaticColors[(index-iFirstOfList)]==COLOR_YELLOW)
		{
			pDC->SetBkColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_RED);
		}
		else
		{
			pDC->SetTextColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(RGB(0,0,0));
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}
	return hbr;
}
