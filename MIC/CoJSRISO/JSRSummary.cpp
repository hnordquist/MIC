// JSRSummary.cpp

#include "stdafx.h"
#include "INI_definitions.h"
#include "colors.h"
#include "JSRSummary.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJSRSummary property page

IMPLEMENT_DYNCREATE(CJSRSummary, CPropertyPage)

CJSRSummary::CJSRSummary() : CPropertyPage(CJSRSummary::IDD)
{
}


CJSRSummary::CJSRSummary(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_JSRSUMMARY_FIRST; i <= IDC_JSRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_JSRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
	m_bDebug = false;
}

void CJSRSummary::SetPropertySheet(CJSRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CJSRSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_JSRSUMMARY_FIRST) &&
		(index <= IDC_JSRSUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CJSRSummary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_JSRSUMMARY_FIRST) &&
		(index <= IDC_JSRSUMMARY_LAST)) 
	{
	  StaticColors[index-IDC_JSRSUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CJSRSummary::ResetColors()
{
	for (int i= IDC_JSRSUMMARY_FIRST; i <= IDC_JSRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_JSRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CJSRSummary::~CJSRSummary()
{
}

void CJSRSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJSRSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJSRSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CJSRSummary)
	ON_BN_CLICKED(IDC_GOTO_DATASTAT, OnGotoDatastat)
	ON_BN_CLICKED(IDC_GOTO_MODIFYPARAMETERS_PAGE, OnGotoModifyParametersPage)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJSRSummary message handlers

void CJSRSummary::OnGotoDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}

void CJSRSummary::OnGotoModifyParametersPage() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

HBRUSH CJSRSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_JSRSUMMARY_FIRST) && (index <= IDC_JSRSUMMARY_LAST))
	{
		int iFirstOfList = IDC_JSRSUMMARY_FIRST; 
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

void CJSRSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet) m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if  (
			(((CButton*)GetDlgItem(IDC_CHECK_ALLOWCLICK))->GetCheck()==1) &&
			(index >= IDC_JSRSUMMARY_FIRST) && 
			(index <= IDC_JSRSUMMARY_LAST)
			)
		{
			if (
				(index == IDC_JSRSUMMARY_COMMAND13) ||
				(index == IDC_JSRSUMMARY_COMMAND12) ||
				(index == IDC_JSRSUMMARY_COMMAND11) ||
				(index == IDC_JSRSUMMARY_COMMAND10) ||
				(index == IDC_JSRSUMMARY_COMMAND9) ||
				(index == IDC_JSRSUMMARY_COMMAND8) ||
				(index == IDC_JSRSUMMARY_COMMAND7) ||
				(index == IDC_JSRSUMMARY_COMMAND6) ||
				(index == IDC_JSRSUMMARY_COMMAND5) ||
				(index == IDC_JSRSUMMARY_COMMAND4) ||
				(index == IDC_JSRSUMMARY_COMMAND3) ||
				(index == IDC_JSRSUMMARY_COMMAND2) ||
				(index == IDC_JSRSUMMARY_COMMAND1))
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

void CJSRSummary::OnShowWindow(BOOL bShow, UINT nStatus) 
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
