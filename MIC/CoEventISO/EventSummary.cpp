// EventSummary.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "EventSummary.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventSummary property page

IMPLEMENT_DYNCREATE(CEventSummary, CPropertyPage)

CEventSummary::CEventSummary() : CPropertyPage(CEventSummary::IDD)
{
}


CEventSummary::CEventSummary(UINT id, CISO *parent) : CPropertyPage(id)// CPropertyPage(CGenlStat::IDD)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_EVENTSUMMARY_FIRST; i <= IDC_EVENTSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_EVENTSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CEventSummary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEventSummary::~CEventSummary()
{
}

BEGIN_MESSAGE_MAP(CEventSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CEventSummary)
	ON_BN_CLICKED(IDC_GOTO_EVENT_DATASTAT, OnGotoEventDatastat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEventSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CEventSummary::SetPropertySheet(CEventPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CEventSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_EVENTSUMMARY_FIRST) &&
		(index <= IDC_EVENTSUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CEventSummary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_EVENTSUMMARY_FIRST) &&
		(index <= IDC_EVENTSUMMARY_LAST)) 
	{
	  StaticColors[index-IDC_EVENTSUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CEventSummary::ResetColors()
{
	for (int i= IDC_EVENTSUMMARY_FIRST; i <= IDC_EVENTSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_EVENTSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}


void CEventSummary::OnGotoEventDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}


HBRUSH CEventSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_EVENTSUMMARY_FIRST) && (index <= IDC_EVENTSUMMARY_LAST))
	{
		int iFirstOfList = IDC_EVENTSUMMARY_FIRST; 
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

void CEventSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_EVENTSUMMARY_FIRST) && (index <= IDC_EVENTSUMMARY_LAST))
		{
			if (
				(index == IDC_EVENTSUMMARY_COMMAND11)||
				(index == IDC_EVENTSUMMARY_COMMAND10)||
				(index == IDC_EVENTSUMMARY_COMMAND9) ||
				(index == IDC_EVENTSUMMARY_COMMAND8) ||
				(index == IDC_EVENTSUMMARY_COMMAND7) ||
				(index == IDC_EVENTSUMMARY_COMMAND6) ||
				(index == IDC_EVENTSUMMARY_COMMAND5) ||
				(index == IDC_EVENTSUMMARY_COMMAND4) ||
				(index == IDC_EVENTSUMMARY_COMMAND3) ||
				(index == IDC_EVENTSUMMARY_COMMAND2) ||
				(index == IDC_EVENTSUMMARY_COMMAND1))
			{
				char Text[256];
				Text[0] = NULL;
				GetDlgItemText(index,Text,sizeof(Text));
				if (strlen(Text))
				{
					strcat(Text,"\"");
					memmove(&Text[2],Text,strlen(Text)+1);
					Text[0] = '"';
					Text[1] = 'E';
					_spawnl(_P_NOWAIT,
						m_pParent->m_szUtilFile,"MsgUtil.exe",Text,NULL);
				}
			}
		}
	}

	CPropertyPage::OnLButtonDown(nFlags, point);
}


