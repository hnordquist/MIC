// EOSSSummary.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "EOSSSummary.h"
#include "EOSSInstrument.h"
#include "EOSSSafePS.h"
#include "eossdefs.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLOR_GREENRED	1
#define COLOR_BLUERED   2
#define COLOR_REDYELLOW 3


/////////////////////////////////////////////////////////////////////////////
// CEOSSSummary property page

IMPLEMENT_DYNCREATE(CEOSSSummary, CPropertyPage)

CEOSSSummary::CEOSSSummary() : CPropertyPage(CEOSSSummary::IDD)
{
	m_bCheckerCreated = false;
	m_cSpan = COleDateTimeSpan(1,0,0,0);
}


CEOSSSummary::CEOSSSummary(UINT id, CISO *parent) : CPropertyPage(id)// CPropertyPage(CGenlStat::IDD)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_EOSSSUMMARY_FIRST; i <= IDC_EOSSSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_EOSSSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
	m_bCheckerCreated = false;
	m_cSpan = COleDateTimeSpan(1,0,0,0);
	//{{AFX_DATA_INIT(CEOSSSummary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEOSSSummary::~CEOSSSummary()
{
	if (m_bCheckerCreated)
		delete m_ctrlChecker;
}

BEGIN_MESSAGE_MAP(CEOSSSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CEOSSSummary)
	ON_BN_CLICKED(IDC_GOTO_EOSS_DATASTAT, OnGotoEOSSDatastat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEOSSSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEOSSSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
#define CELLSX 8
#define CELLSY 5
void CEOSSSummary::PopulateChecker()
{
	bool bDone = false;
	unsigned int uiIndex = 0;
	char szTemp[16];
	while (!bDone)
	{
		if (uiIndex < (CELLSX*CELLSY))
		{
			if (((CEOSSInstrument*)m_pParent)->EOSSes[uiIndex])
			{
				COLORREF sColor = RGB(255,255,255);
				if (((CEOSSInstrument*)m_pParent)->sStatus[uiIndex].ucRedCount)
					sColor = COLOR_RED;
				else if (((CEOSSInstrument*)m_pParent)->sStatus[uiIndex].ucYellowCount)
					sColor = COLOR_YELLOW;
				else if (((CEOSSInstrument*)m_pParent)->sStatus[uiIndex].ucInstID == 6) // not yet read at all
					sColor = COLOR_WHITE;
				else if (strstr(((CEOSSInstrument*)m_pParent)->sGeneral.cLATEST_RESULTS,"Not yet read"))
					sColor = COLOR_WHITE;
				else
					sColor = COLOR_GREEN;
				m_ctrlChecker.SetBlock(uiIndex,sColor,_itoa(((CEOSSInstrument*)m_pParent)->sStatus[uiIndex].uiEOSSSN,szTemp,10));
				uiIndex++;
			}
			else
				bDone = true;
		}
		else
			bDone = true;
	}
	while (uiIndex < (CELLSX*CELLSY)) m_ctrlChecker.SetBlock(uiIndex++,RGB(255,255,255)," ");
}


void CEOSSSummary::SetPropertySheet(CEOSSPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CEOSSSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_EOSSSUMMARY_FIRST) &&
		(index <= IDC_EOSSSUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CEOSSSummary::ResetColors()
{
	for (int i= IDC_EOSSSUMMARY_FIRST; i <= IDC_EOSSSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_EOSSSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}


void CEOSSSummary::OnGotoEOSSDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}


HBRUSH CEOSSSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_EOSSSUMMARY_FIRST) && (index <= IDC_EOSSSUMMARY_LAST))
	{
		int iFirstOfList = IDC_EOSSSUMMARY_FIRST; 
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

void CEOSSSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_EOSSSUMMARY_FIRST) && (index <= IDC_EOSSSUMMARY_LAST))
		{
			if (
				(index == IDC_EOSSSUMMARY_COMMAND11)||
				(index == IDC_EOSSSUMMARY_COMMAND10)||
				(index == IDC_EOSSSUMMARY_COMMAND9) ||
				(index == IDC_EOSSSUMMARY_COMMAND8) ||
				(index == IDC_EOSSSUMMARY_COMMAND7) ||
				(index == IDC_EOSSSUMMARY_COMMAND6) ||
				(index == IDC_EOSSSUMMARY_COMMAND5) ||
				(index == IDC_EOSSSUMMARY_COMMAND4) ||
				(index == IDC_EOSSSUMMARY_COMMAND3) ||
				(index == IDC_EOSSSUMMARY_COMMAND2) ||
				(index == IDC_EOSSSUMMARY_COMMAND1))
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



void CEOSSSummary::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if (!m_bCheckerCreated  && ::IsWindow(m_hWnd))
	{
		m_bCheckerCreated = true;
		CRect rect;
		GetDlgItem(IDC_STATIC_CHECKER)->GetWindowRect(rect);
		ScreenToClient(rect);
		m_ctrlChecker.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP, rect, this, IDC_STATIC_CHECKER);
		m_ctrlChecker.SetTotalBlocks(CELLSX,CELLSY,0);
		PopulateChecker();
	}
	if (::IsWindow(m_ctrlChecker.m_hWnd) && bShow)
	{
		PopulateChecker();
		m_ctrlChecker.Refresh();
	}
}

void CEOSSSummary::OnButtonEdit() 
{
	//get all the stuff and save it
	if (m_pParent->Validate())
	{
		bool bReturn = m_pPropertySheet->EditEOSSList();
		//if true do what ever is necessary at this level
		if (bReturn)
		{
			PopulateChecker();
			m_pPropertySheet->RefreshDataDisplay();
		}
	}
}
