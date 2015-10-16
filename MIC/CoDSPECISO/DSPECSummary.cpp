// DSPECSummary.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "DSPECSummary.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDSPECSummary property page

IMPLEMENT_DYNCREATE(CDSPECSummary, CPropertyPage)


//////////////////////////////////////////////////////////////////////////////
//
//  Constructors, Destructor, DDX, Message Map
//
//////////////////////////////////////////////////////////////////////////////
CDSPECSummary::CDSPECSummary() : CPropertyPage(CDSPECSummary::IDD)
{
////	TRACE("CDSPECSummary::CTOR #1\n");
}
CDSPECSummary::CDSPECSummary(UINT id, CISO* parent) : CPropertyPage(id)
{
//	TRACE("CDSPECSummary::CTOR #2\n");

	m_pPropertySheet = NULL;
	m_pParent = parent;

	ResetColors();

	//{{AFX_DATA_INIT(CDSPECSummary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CDSPECSummary::~CDSPECSummary()
{
//		TRACE("CDSPECSummary::DTR\n");
}

void CDSPECSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSPECSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDSPECSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CDSPECSummary)
	ON_BN_CLICKED(IDC_GOTO_DSPEC_DATASTAT, OnGotoDatastat)
	ON_BN_CLICKED(IDC_GOTO_DSPEC_SETUP, OnGotoSetup)
	//ON_BN_CLICKED(IDC_GOTO_DSPEC_USERSTAT, OnGotoUserstat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





void CDSPECSummary::SetPropertySheet(CDSPECSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CDSPECSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_DSPECSUMMARY_FIRST) &&
		(index <= IDC_DSPECSUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CDSPECSummary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_DSPECSUMMARY_FIRST) &&
		(index <= IDC_DSPECSUMMARY_LAST)) 
	{
	  StaticColors[index-IDC_DSPECSUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CDSPECSummary::ResetColors()
{
	for (int i= IDC_DSPECSUMMARY_FIRST; i <= IDC_DSPECSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_DSPECSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void CDSPECSummary::OnGotoSetup() 
{
	if (m_pPropertySheet)
	{
		m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

void CDSPECSummary::OnGotoDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}

//void CDSPECSummary::OnGotoUserstat() 
//{
//	if (m_pPropertySheet) 
//	{
//		m_pPropertySheet->SetActivePage(3);
//		m_pPropertySheet->SetToClose(0);
//	}
//}

HBRUSH CDSPECSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_DSPECSUMMARY_FIRST) && (index <= IDC_DSPECSUMMARY_LAST))
	{
		int iFirstOfList = IDC_DSPECSUMMARY_FIRST; 
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
			//pDC->SetTextColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_BLACK);
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}

	return hbr;
}

void CDSPECSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_DSPECSUMMARY_FIRST) && (index <= IDC_DSPECSUMMARY_LAST))
		{
			if ((index == IDC_DSPECSUMMARY_COMMAND6) ||
				(index == IDC_DSPECSUMMARY_COMMAND5) ||
				(index == IDC_DSPECSUMMARY_COMMAND4) ||
				(index == IDC_DSPECSUMMARY_COMMAND3) ||
				(index == IDC_DSPECSUMMARY_COMMAND2) ||
				(index == IDC_DSPECSUMMARY_COMMAND1))
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

void CDSPECSummary::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here

	CString cs = "Version " + m_pParent->m_csVersion + " " + __DATE__;
	SetDlgItemText(IDC_SUMMARY_STATIC_ADVERT, cs);
#ifdef _DEBUG
	SetDlgItemText(IDC_UPS_SUMMARY_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_SUMMARY_STATIC_DEBUG, "Release Build");
#endif
}

void CDSPECSummary::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CPropertyPage::OnTimer(nIDEvent);
}

BOOL CDSPECSummary::OnInitDialog() 
{
//	TRACE("CDSPECSummary::OnInitDialog()\n");

	CPropertyPage::OnInitDialog();

	return TRUE;
}


void CDSPECSummary::IssuePrint(FILE *outfile)
{
	//fprintf(outfile,"************************BEGIN CDSPECSummary::IssuePrint()\n");

	CString temp, temp1, temp2, temp3, temp4;

	fprintf(outfile,"\nSUMMARY\n");

	fprintf(outfile," DSPEC Settings\n");

	GetDlgItemText(IDC_DSPECSUMMARY_UNIT_ID,temp);
	fprintf(outfile,"%+25s: %s\n","Unit Identification",temp);

	fprintf(outfile," Data Status\n");

	GetDlgItemText(IDC_DSPECSUMMARY_NONZERO_CHANS,temp);
	fprintf(outfile,"%+25s: %s\n","Non-Zero Chans",temp);

	GetDlgItemText(IDC_DSPECSUMMARY_ABOVE_THRESHOLD,temp);
	fprintf(outfile,"%+25s: %s\n","Chans Above Threshold",temp);

	GetDlgItemText(IDC_DSPECSUMMARY_DWELLTYPE,temp);
	fprintf(outfile,"%+25s: %s\n","Dwell Type",temp);

	GetDlgItemText(IDC_DSPECSUMMARY_SECS_TO_END_DWELL,temp);
	fprintf(outfile,"%+25s: %s\n","Secs Until End of Dwell",temp);

	GetDlgItemText(IDC_DSPECSUMMARY_LAST_CHN_FILE,temp);
	fprintf(outfile,"%+25s: %s\n","Latest CHN File",temp);

	GetDlgItemText(IDC_DSPECSUMMARY_CURRENT_HV,temp);
	fprintf(outfile,"%+25s: %s\n","Current HV",temp);

	GetDlgItemText(IDC_DSPECSUMMARY_HV_OVERLOAD,temp);
	fprintf(outfile,"%+25s: %s\n","HV Overload",temp);

	GetDlgItemText(IDC_DSPECSUMMARY_SHUT_ACTUAL,temp);
	fprintf(outfile,"%+25s: %s\n","Shutdown", temp);

	fprintf(outfile," Recent Commands and Responses\n");
	GetDlgItemText(IDC_DSPECSUMMARY_COMMAND6,temp);
	fprintf(outfile,"   %s\n",temp);
	GetDlgItemText(IDC_DSPECSUMMARY_COMMAND5,temp);
	fprintf(outfile,"   %s\n",temp);
	GetDlgItemText(IDC_DSPECSUMMARY_COMMAND4,temp);
	fprintf(outfile,"   %s\n",temp);
	GetDlgItemText(IDC_DSPECSUMMARY_COMMAND3,temp);
	fprintf(outfile,"   %s\n",temp);
	GetDlgItemText(IDC_DSPECSUMMARY_COMMAND2,temp);
	fprintf(outfile,"   %s\n",temp);
	GetDlgItemText(IDC_DSPECSUMMARY_COMMAND1,temp);
	fprintf(outfile,"   %s\n",temp);

	fprintf(outfile,"\n");

	//fprintf(outfile,"************************END CDSPECSummary::IssuePrint()\n");
}
