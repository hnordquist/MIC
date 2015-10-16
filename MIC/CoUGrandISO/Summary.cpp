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
//	for (int i= IDC_SUMMARY_FIRST; i <= IDC_SUMMARY_LAST; i++)
//	{
//		StaticColors[i-IDC_SUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
//	}
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
	ON_BN_CLICKED(IDC_GOTO_UGRAND, OnGotoUGrand)
	ON_BN_CLICKED(IDC_GOTO_USERSTAT, OnGotoUserstat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_WATCH, OnButtonWatch)
	ON_BN_CLICKED(IDC_BUTTON_TODAYDATA, OnButtonTodaydata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSummary message handlers


void CSummary::OnGotoUGrand() 
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
//		if (m_pPropertySheet->m_eMonitorVers == UNKNOWN)
//			m_pPropertySheet->SetActivePage(3);
//		else
		m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

void CSummary::OnGotoUserstat() 
{
	if (m_pPropertySheet) 
	{
//		if (m_pPropertySheet->m_eMonitorVers == UNKNOWN)
//			m_pPropertySheet->SetActivePage(4);
//		else
		m_pPropertySheet->SetActivePage(3);
		m_pPropertySheet->SetToClose(0);
	}
}

HBRUSH CSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr;
#ifdef BMPBACKGRND
	//force all statics to transparent
//	if (nCtlColor == CTLCOLOR_STATIC) 
//		pDC->SetBkMode(TRANSPARENT);
	
	hbr = (HBRUSH)m_brush;
#else
	hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
#endif

//	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
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
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);

	// TODO: Add your message handler code here
}

void CSummary::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
//		dc.SelectStockObject(WHITE_BRUSH);
//		dc.SelectStockObject(BLACK_PEN);
//		dc.Rectangle(8,220,458,350);
//		int x = 8;
//		int y = 285;
//		dc.MoveTo(x,y);
//		for (int j=0;j<sum;j++)
//		{
//			dc.LineTo(x+j%450,y+sin((float)j/100)*60);	
//		}
		
	// Do not call CPropertyPage::OnPaint() for painting messages
}

void CSummary::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
//	if (nIDEvent == 4)
//	{
//		sum = sum + 500;
//		Invalidate();
//	}
	CPropertyPage::OnTimer(nIDEvent);
}

BOOL CSummary::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

		//fill the background of the main dialog box
#ifdef BMPBACKGRND
//	VERIFY(m_bmapBrush.LoadBitmap(IDB_BMAPBRUSH));
#ifdef ALLOWTRIGGERHOLDOFF
	VERIFY(m_bmapBrush.LoadBitmap(IDB_BITMAP8));  
#else
	VERIFY(m_bmapBrush.LoadBitmap(IDB_BITMAP7));
#endif
	CBitmap* pDlgBmp = &m_bmapBrush;
	m_brush.CreatePatternBrush(pDlgBmp);
#endif
	
	// TODO: Add extra initialization here
//	SetTimer(4,1000,NULL);
//	sum = 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/*
void CSummary::GetStrings(int nId, CString &szName, char*szFlag)
{
	if (StaticColors[nId - IDC_SUMMARY_FIRST] == COLOR_YELLOW) szFlag[0] = '#';
	else if (StaticColors[nId - IDC_SUMMARY_FIRST] != COLOR_DEFAULT_TEXT) szFlag[0] = '*';
	else szFlag[0] = ' ';
	GetDlgItemText(nId,szName);
}

void CSummary::IssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3, cTemp4;

	fprintf(outfile,"SUMMARY\n");
	fprintf(outfile," UGRAND Setup\n");


	GetStrings(IDC_SUMMARY_BATTERY1,temp1,&cTemp1);
	GetStrings(IDC_SUMMARY_BATTERY2,temp2,&cTemp2);
	GetStrings(IDC_SUMMARY_BATTERY3,temp3,&cTemp3);
	fprintf(outfile,"  Battery Voltage (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(IDC_SUMMARY_P25VOLT1,temp1,&cTemp1);
	GetStrings(IDC_SUMMARY_P25VOLT2,temp2,&cTemp2);
	GetStrings(IDC_SUMMARY_P25VOLT3,temp3,&cTemp3);
	fprintf(outfile,"  2.5 Volt Supply (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(IDC_SUMMARY_P33VOLT1,temp1,&cTemp1);
	GetStrings(IDC_SUMMARY_P33VOLT2,temp2,&cTemp2);
	GetStrings(IDC_SUMMARY_P33VOLT3,temp3,&cTemp3);
	fprintf(outfile,"  3.3 Volt Supply (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(IDC_SUMMARY_P5VOLT1,temp1,&cTemp1);
	GetStrings(IDC_SUMMARY_P5VOLT2,temp2,&cTemp2);
	GetStrings(IDC_SUMMARY_P5VOLT3,temp3,&cTemp3);
	fprintf(outfile,"   +5 Volt Supply (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	fprintf(outfile," Data Status\n");
	GetStrings(IDC_SUMMARY_OOMII,temp,&cTemp);
	fprintf(outfile,"   Measurement Interval of Interest: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(IDC_SUMMARY_ACPOWER,temp,&cTemp);
	fprintf(outfile,"                     External Power: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(IDC_SUMMARY_BATTERYOK,temp,&cTemp);
	fprintf(outfile,"                            Battery: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(IDC_SUMMARY_BBMOK,temp,&cTemp);
	fprintf(outfile,"           Battery Backed Up Memory: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(IDC_SUMMARY_INQUIRETIME,temp,&cTemp);
	fprintf(outfile,"  Last INQUIRE Status (Y.M.D H:M:S):\n  %s%c\n",LPCTSTR(temp),cTemp);

	fprintf(outfile,"  Recent Commands and Responses:\n");
	GetStrings(IDC_SUMMARY_COMMAND6,temp,&cTemp);
	fprintf(outfile,"  %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(IDC_SUMMARY_COMMAND5,temp,&cTemp);
	fprintf(outfile,"  %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(IDC_SUMMARY_COMMAND4,temp,&cTemp);
	fprintf(outfile,"  %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(IDC_SUMMARY_COMMAND3,temp,&cTemp);
	fprintf(outfile,"  %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(IDC_SUMMARY_COMMAND2,temp,&cTemp);
	fprintf(outfile,"  %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(IDC_SUMMARY_COMMAND1,temp,&cTemp);
	fprintf(outfile,"  %s%c\n",LPCTSTR(temp),cTemp);
}
*/
void CSummary::OnButtonWatch() 
{
	// TODO: Add your control notification handler code here
	if (m_pPropertySheet) 
	{
//		m_pPropertySheet->SetActivePage(3);
		m_pPropertySheet->SetToClose(0);
		m_pPropertySheet->DisplayWatchWindow();
	}
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
