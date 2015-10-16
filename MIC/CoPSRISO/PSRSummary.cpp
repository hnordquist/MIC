// PSRSummary.cpp

#include "stdafx.h"
#include "INI_definitions.h"
#include "colors.h"
#include "PSRSummary.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPSRSummary property page

IMPLEMENT_DYNCREATE(CPSRSummary, CPropertyPage)

CPSRSummary::CPSRSummary() : CPropertyPage(CPSRSummary::IDD)
{
}


CPSRSummary::CPSRSummary(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_PSRSUMMARY_FIRST; i <= IDC_PSRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_PSRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
	m_bDebug = false;
}

void CPSRSummary::SetPropertySheet(CPSRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CPSRSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_PSRSUMMARY_FIRST) &&
		(index <= IDC_PSRSUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CPSRSummary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_PSRSUMMARY_FIRST) &&
		(index <= IDC_PSRSUMMARY_LAST)) 
	{
	  StaticColors[index-IDC_PSRSUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CPSRSummary::ResetColors()
{
	for (int i= IDC_PSRSUMMARY_FIRST; i <= IDC_PSRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_PSRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CPSRSummary::~CPSRSummary()
{
}

void CPSRSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSRSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPSRSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CPSRSummary)
	ON_BN_CLICKED(IDC_GOTO_DATASTAT, OnGotoDatastat)
	ON_BN_CLICKED(IDC_GOTO_MODIFYPARAMETERS_PAGE, OnGotoModifyParametersPage)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSRSummary message handlers

void CPSRSummary::OnGotoDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}

void CPSRSummary::OnGotoModifyParametersPage() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

HBRUSH CPSRSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_PSRSUMMARY_FIRST) && (index <= IDC_PSRSUMMARY_LAST))
	{
		int iFirstOfList = IDC_PSRSUMMARY_FIRST; 
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

void CPSRSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet) m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if  (
			(((CButton*)GetDlgItem(IDC_CHECK_ALLOWCLICK))->GetCheck()==1) &&
			(index >= IDC_PSRSUMMARY_FIRST) && 
			(index <= IDC_PSRSUMMARY_LAST)
			)
		{
			if (
				(index == IDC_PSRSUMMARY_COMMAND13) ||
				(index == IDC_PSRSUMMARY_COMMAND12) ||
				(index == IDC_PSRSUMMARY_COMMAND11) ||
				(index == IDC_PSRSUMMARY_COMMAND10) ||
				(index == IDC_PSRSUMMARY_COMMAND9) ||
				(index == IDC_PSRSUMMARY_COMMAND8) ||
				(index == IDC_PSRSUMMARY_COMMAND7) ||
				(index == IDC_PSRSUMMARY_COMMAND6) ||
				(index == IDC_PSRSUMMARY_COMMAND5) ||
				(index == IDC_PSRSUMMARY_COMMAND4) ||
				(index == IDC_PSRSUMMARY_COMMAND3) ||
				(index == IDC_PSRSUMMARY_COMMAND2) ||
				(index == IDC_PSRSUMMARY_COMMAND1))
			{
				char Text[256];
				Text[0] = NULL;
				GetDlgItemText(index,Text,sizeof(Text));
				if (strlen(Text))
				{
					strcat(Text,"\"");
					memmove(&Text[2],Text,strlen(Text)+1);
					Text[0] = '"';
					Text[1] = 'J';
					_spawnl(_P_NOWAIT,m_pParent->m_szUtilFile,"MsgUtil.exe",Text,NULL);
				}
			}
		}
	}
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CPSRSummary::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	char Debug[16];
	GetPrivateProfileString(
		CONFIGURATION,"DEBUG","FALSE",Debug,sizeof(Debug),
		m_pParent->m_szIniFile);

	if (toupper(Debug[0]) == 'T')
		m_bDebug = true;
	else
		m_bDebug = false;

	if (m_bDebug)
	{
		GetDlgItem(IDC_STATIC_LASTDATA)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE5)->ShowWindow(SW_SHOW);

		//<<QA>> hide the version info
	}
	else
	{
		GetDlgItem(IDC_STATIC_LASTDATA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE5)->ShowWindow(SW_HIDE);

		//<<QA>> Show the version info
	}
}
