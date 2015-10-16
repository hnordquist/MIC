// MCASummary.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "MCASummary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CMCASummary property page

IMPLEMENT_DYNCREATE(CMCASummary, CPropertyPage)

CMCASummary::CMCASummary() : CPropertyPage(CMCASummary::IDD)
{
}

CMCASummary::CMCASummary(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_MCASUMMARY_FIRST; i <= IDC_MCASUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_MCASUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CMCASummary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMCASummary::~CMCASummary()
{
}

BEGIN_MESSAGE_MAP(CMCASummary, CPropertyPage)
	//{{AFX_MSG_MAP(CMCASummary)
	ON_BN_CLICKED(IDC_GOTO_DATASTAT, OnGotoDataStatusPage)
	ON_BN_CLICKED(IDC_GOTO_GRAND, OnGotoMCAStatusPage)
	ON_BN_CLICKED(IDC_GOTO_USERSTAT, OnGotoMCAConfigurationPage)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMCASummary::SetPropertySheet(CMCAPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CMCASummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_MCASUMMARY_FIRST) &&
		(index <= IDC_MCASUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CMCASummary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_MCASUMMARY_FIRST) &&
		(index <= IDC_MCASUMMARY_LAST)) {
	  StaticColors[index-IDC_MCASUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}
void
CMCASummary::ResetColors()
{
	for (int i= IDC_MCASUMMARY_FIRST; i <= IDC_MCASUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_MCASUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}


void CMCASummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCASummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


void CMCASummary::OnGotoMCAStatusPage() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}

void CMCASummary::OnGotoDataStatusPage() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

void CMCASummary::OnGotoMCAConfigurationPage() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(3);
		m_pPropertySheet->SetToClose(0);
	}
}

HBRUSH CMCASummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_MCASUMMARY_FIRST) && (index <= IDC_MCASUMMARY_LAST))
	{
		int iFirstOfList = IDC_MCASUMMARY_FIRST; 
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

void CMCASummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_MCASUMMARY_FIRST) && (index <= IDC_MCASUMMARY_LAST))
		{
			if ((index == IDC_MCASUMMARY_COMMAND6) ||
				(index == IDC_MCASUMMARY_COMMAND5) ||
				(index == IDC_MCASUMMARY_COMMAND4) ||
				(index == IDC_MCASUMMARY_COMMAND3) ||
				(index == IDC_MCASUMMARY_COMMAND2) ||
				(index == IDC_MCASUMMARY_COMMAND1))
			{
				char Text[256];
				Text[0] = NULL;
				GetDlgItemText(index,Text,sizeof(Text));
				if (strlen(Text))
				{
					strcat(Text,"\"");
					memmove(&Text[2],Text,strlen(Text)+1);
					Text[0] = '"';
					Text[1] = 'M';
					_spawnl(_P_NOWAIT,m_pParent->m_szUtilFile,"MsgUtil.exe",Text,NULL);
				}
			}
		}
	}
	CPropertyPage::OnLButtonDown(nFlags, point);
}

