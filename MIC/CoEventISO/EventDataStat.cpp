// EventDataStat.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "EventDataStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventDataStat property page

IMPLEMENT_DYNCREATE(CEventDataStat, CPropertyPage)

CEventDataStat::CEventDataStat() : CPropertyPage(CEventDataStat::IDD)
{
}

CEventDataStat::CEventDataStat(UINT id) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	//initialize all displayed (cstatic) text to the
	//default color (black)
	for (int i= IDC_EVENTDATA_FIRST; i <= IDC_EVENTDATA_LAST; i++)
	{
		StaticColors[i-IDC_EVENTDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CGenlStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void
CEventDataStat::SetPropertySheet(CEventPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}


void
CEventDataStat::SetText(int index, char * text)
{
	//set the cstatic to the provided text
	if ((index >= IDC_EVENTDATA_FIRST) &&
		(index <= IDC_EVENTDATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *
CEventDataStat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_EVENTDATA_FIRST) &&
		(index <= IDC_EVENTDATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void
CEventDataStat::SetColor(int index, COLORREF color)
{
	//set the current color of a cstatic
	//it will get updated via onctlcolor()
	if ((index >= IDC_EVENTDATA_FIRST) &&
		(index <= IDC_EVENTDATA_LAST)) {
	  StaticColors[index-IDC_EVENTDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CEventDataStat::ResetColors()
{
	for (int i= IDC_EVENTDATA_FIRST; i <= IDC_EVENTDATA_LAST; i++)
	{
		StaticColors[i-IDC_EVENTDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CEventDataStat::~CEventDataStat()
{
}

void CEventDataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventDataStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CEventDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventDataStat message handlers
HBRUSH CEventDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//get the id of the control 
	UINT index = pWnd->GetDlgCtrlID();
	//set the font and color of that control
	if ((index >= IDC_EVENTDATA_FIRST) && (index <= IDC_EVENTDATA_LAST))
	{
		int iFirstOfList = IDC_EVENTDATA_FIRST; 
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
void CEventDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}


