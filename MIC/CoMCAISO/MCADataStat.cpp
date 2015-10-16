// MCADataStat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "MCADataStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMCADataStat property page

IMPLEMENT_DYNCREATE(CMCADataStat, CPropertyPage)

CMCADataStat::CMCADataStat() : CPropertyPage(CMCADataStat::IDD)
{
}

CMCADataStat::CMCADataStat(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;
	//initialize all displayed (cstatic) text to the
	//default color (black)
	for (int i= IDC_MCADATA_FIRST; i <= IDC_MCADATA_LAST; i++)
	{
		StaticColors[i-IDC_MCADATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CMCADataStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void
CMCADataStat::SetPropertySheet(CMCAPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CMCADataStat::SetText(int index,CHAR * text)
{
	if ((index >= IDC_MCADATA_FIRST) &&
		(index <= IDC_MCADATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *
CMCADataStat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_MCADATA_FIRST) &&
		(index <= IDC_MCADATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void
CMCADataStat::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_MCADATA_FIRST) &&
		(index <= IDC_MCADATA_LAST)) {
	  StaticColors[index-IDC_MCADATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CMCADataStat::ResetColors()
{
	for (int i= IDC_MCADATA_FIRST; i <= IDC_MCADATA_LAST; i++)
	{
		StaticColors[i-IDC_MCADATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CMCADataStat::~CMCADataStat()
{
}

void CMCADataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCADataStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCADataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CMCADataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMCADataStat message handlers
HBRUSH CMCADataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_MCADATA_FIRST) && (index <= IDC_MCADATA_LAST))
	{
		int iFirstOfList = IDC_MCADATA_FIRST; 
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
void CMCADataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}


