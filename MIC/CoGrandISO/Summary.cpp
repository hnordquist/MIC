// Summary.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "Summary.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSummary property page

IMPLEMENT_DYNCREATE(CSummary, CPropertyPage)

CSummary::CSummary() : CPropertyPage(CSummary::IDD)
{
}

CSummary::CSummary(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;
	//{{AFX_DATA_INIT(CSummary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSummary::~CSummary()
{
}

void CSummary::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_SUMMARY_FIRST) &&
		(index <= IDC_SUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CSummary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_SUMMARY_FIRST) &&
		(index <= IDC_SUMMARY_LAST)) 
	{
	  StaticColors[index-IDC_SUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CSummary::ResetColors()
{
	for (int i= IDC_SUMMARY_FIRST; i <= IDC_SUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_SUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void CSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CSummary)
	ON_BN_CLICKED(IDC_GOTO_DATASTAT, OnGotoDatastat)
	ON_BN_CLICKED(IDC_GOTO_GRAND, OnGotoGrand)
	ON_BN_CLICKED(IDC_GOTO_USERSTAT, OnGotoUserstat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_TODAYDATA, OnButtonTodaydata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSummary message handlers


void CSummary::OnGotoGrand() 
{
	if (m_pPropertySheet)
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}

void CSummary::OnGotoDatastat() 
{
	if (m_pPropertySheet) 
	{
		if (m_pPropertySheet->m_eMonitorVers == UNKNOWN)
			m_pPropertySheet->SetActivePage(3);
		else
			m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

void CSummary::OnGotoUserstat() 
{
	if (m_pPropertySheet) 
	{
		if (m_pPropertySheet->m_eMonitorVers == UNKNOWN)
			m_pPropertySheet->SetActivePage(4);
		else
			m_pPropertySheet->SetActivePage(3);
		m_pPropertySheet->SetToClose(0);
	}
}

HBRUSH CSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_SUMMARY_FIRST) && (index <= IDC_SUMMARY_LAST))
	{
		int iFirstOfList = IDC_SUMMARY_FIRST; 
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

void CSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_SUMMARY_FIRST) && (index <= IDC_SUMMARY_LAST))
		{
			if ((index == IDC_SUMMARY_COMMAND6) ||
				(index == IDC_SUMMARY_COMMAND5) ||
				(index == IDC_SUMMARY_COMMAND4) ||
				(index == IDC_SUMMARY_COMMAND3) ||
				(index == IDC_SUMMARY_COMMAND2) ||
				(index == IDC_SUMMARY_COMMAND1))
			{
				char Text[256];
				Text[0] = NULL;
				GetDlgItemText(index,Text,sizeof(Text));
				if (strlen(Text))
				{
					strcat(Text,"\"");
					memmove(&Text[2],Text,strlen(Text)+1);
					Text[0] = '"';
					Text[1] = 'G';
					_spawnl(_P_NOWAIT,m_pParent->m_szUtilFile,"MsgUtil.exe",Text,NULL);
				}
			}
		}
	}

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CSummary::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
}

void CSummary::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CPropertyPage::OnTimer(nIDEvent);
}

BOOL CSummary::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSummary::OnButtonTodaydata() 
{
	// TODO: Add your control notification handler code here
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetToClose(0);
		m_pPropertySheet->DisplayTodayData();
	}
}

