/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
// DataWatchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DataWatchDlg.h"
#include "JulianT.h"
#include "WatchComDlg.h"

#define RESIZETIMER 1000

#define IDR_CHART1 10001
#define IDR_CHART2 10002
#define IDR_CHART3 10003
#define IDR_CHART4 10004
#define IDR_CHART5 10005
#define IDR_CHART6 10006
#define IDR_CHART7 10007
#define IDR_CHART8 10008

#define ID_LINE1   20001
#define ID_LINE2   20002
#define ID_LINE3   20003
#define ID_LINE4   20004
#define ID_LINE5   20005
#define ID_LINE6   20006
#define ID_LINE7   20007
#define ID_LINE8   20008

#define uH1		(WM_USER + 500)
#define uV1		(WM_USER + 501)
#define uH1s	(WM_USER + 502)
#define uFP1	(WM_USER + 503)
#define uLP1	(WM_USER + 504)

#define uH2		(WM_USER + 600)
#define uV2		(WM_USER + 601)
#define uH2s	(WM_USER + 602)
#define uFP2	(WM_USER + 603)
#define uLP2	(WM_USER + 604)

#define uH3		(WM_USER + 700)
#define uV3		(WM_USER + 701)
#define uH3s	(WM_USER + 702)
#define uFP3	(WM_USER + 703)
#define uLP3	(WM_USER + 704)

#define uH4		(WM_USER + 800)
#define uV4		(WM_USER + 801)
#define uH4s	(WM_USER + 802)
#define uFP4	(WM_USER + 803)
#define uLP4	(WM_USER + 804)

#define uH5		(WM_USER + 900)
#define uV5		(WM_USER + 901)
#define uH5s	(WM_USER + 902)
#define uFP5	(WM_USER + 903)
#define uLP5	(WM_USER + 904)

#define uH6		(WM_USER + 910)
#define uV6		(WM_USER + 911)
#define uH6s	(WM_USER + 912)
#define uFP6	(WM_USER + 913)
#define uLP6	(WM_USER + 914)

#define uH7		(WM_USER + 920)
#define uV7		(WM_USER + 921)
#define uH7s	(WM_USER + 922)
#define uFP7	(WM_USER + 923)
#define uLP7	(WM_USER + 924)

#define uH8		(WM_USER + 930)
#define uV8		(WM_USER + 931)
#define uH8s	(WM_USER + 932)
#define uFP8	(WM_USER + 933)
#define uLP8	(WM_USER + 934)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataWatchDlg dialog


CDataWatchDlg::CDataWatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataWatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataWatchDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iChart1x = 0;
	m_iChart2x = 0;
	m_iChart3x = 0;
	m_iChart4x = 0;
	m_iChart5x = 0;
	m_iChart5x = 0;
	m_iChart6x = 0;
	m_iChart7x = 0;
	m_iChart8x = 0;
	m_uBaseTime = 0;
//	m_cRGBbk = WHITE;
	m_uiLineIndex = 1;
	m_uiLastTime = 0;
	m_bNewLine = true;
	m_uiMultiplier = 8;
	m_cTitle = "Data Watcher";
	m_cs1 = "Data Ch 1";
	m_cs2 = "Data Ch 2";
	m_cs3 = "Data Ch 3";
	m_cs4 = "Data Ch 4";
	m_cs5 = "Data Ch 5";
	m_cs6 = "Data Ch 6";
	m_cs7 = "Data Ch 7";
	m_cs8 = "Data Ch 8";

	m_iOversize = 0;
}

CDataWatchDlg::~CDataWatchDlg()
{
//	m_pParent->m_pDataWatchDlg = NULL;
	((CWatchComDlg*)m_pParent)->GraphIsDestroyed(m_iMyIndex);
}

void CDataWatchDlg::SetDlgTitle(CString cTitle, CWnd* pParent, int iIndex)
{
	m_cTitle = cTitle;
	m_pParent = pParent;
	m_iMyIndex = iIndex;
}

void CDataWatchDlg::SetGraphTitles(CString cs1, CString cs2, CString cs3, CString cs4, 
								   CString cs5, CString cs6, CString cs7, CString cs8)
{
	m_cs1 = cs1;
	m_cs2 = cs2;
	m_cs3 = cs3;
	m_cs4 = cs4;
	m_cs5 = cs5;
	m_cs6 = cs6;
	m_cs7 = cs7;
	m_cs8 = cs8;
}

void CDataWatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataWatchDlg)
	DDX_Control(pDX, IDC_BUTTON_EXPAND, m_cButtonExpand);
	DDX_Control(pDX, IDC_BUTTON_CONTRACT, m_cButtonContract);
	DDX_Control(pDX, IDC_CHECK_LOCKED, m_cLocked);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataWatchDlg, CDialog)
	//{{AFX_MSG_MAP(CDataWatchDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_MESSAGE	  (uH1s,OnScrollH1Msg)
	ON_MESSAGE	  (uH2s,OnScrollH2Msg)
	ON_MESSAGE	  (uH3s,OnScrollH3Msg)
	ON_MESSAGE	  (uH4s,OnScrollH4Msg)
	ON_MESSAGE	  (uH5s,OnScrollH5Msg)
	ON_MESSAGE	  (uH6s,OnScrollH6Msg)
	ON_MESSAGE	  (uH7s,OnScrollH7Msg)
	ON_MESSAGE	  (uH8s,OnScrollH8Msg)
	ON_MESSAGE	  (uFP1,OnData1FPoint)
	ON_MESSAGE	  (uFP2,OnData2FPoint)
	ON_MESSAGE	  (uFP3,OnData3FPoint)
	ON_MESSAGE	  (uFP4,OnData4FPoint)
	ON_MESSAGE	  (uFP5,OnData5FPoint)
	ON_MESSAGE	  (uFP6,OnData6FPoint)
	ON_MESSAGE	  (uFP7,OnData7FPoint)
	ON_MESSAGE	  (uFP8,OnData8FPoint)
	ON_MESSAGE	  (uLP1,OnData1LPoint)
	ON_MESSAGE	  (uLP2,OnData2LPoint)
	ON_MESSAGE	  (uLP3,OnData3LPoint)
	ON_MESSAGE	  (uLP4,OnData4LPoint)
	ON_MESSAGE	  (uLP5,OnData5LPoint)
	ON_MESSAGE	  (uLP6,OnData6LPoint)
	ON_MESSAGE	  (uLP7,OnData7LPoint)
	ON_MESSAGE	  (uLP8,OnData8LPoint)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_LOCKED, OnCheckLocked)
	ON_BN_CLICKED(IDC_BUTTON_EXPAND, OnButtonExpand)
	ON_BN_CLICKED(IDC_BUTTON_CONTRACT, OnButtonContract)
	ON_MESSAGE    (uH1,OnNewH1Limits)
	ON_MESSAGE	  (uV1,OnNewV1Limits)	
	ON_MESSAGE    (uH2,OnNewH2Limits)
	ON_MESSAGE	  (uV2,OnNewV2Limits)	
	ON_MESSAGE    (uH3,OnNewH3Limits)
	ON_MESSAGE	  (uV3,OnNewV3Limits)
	ON_MESSAGE    (uH4,OnNewH4Limits)
	ON_MESSAGE	  (uV4,OnNewV4Limits)
	ON_MESSAGE    (uH5,OnNewH5Limits)
	ON_MESSAGE	  (uV5,OnNewV5Limits)
	ON_MESSAGE    (uH6,OnNewH6Limits)
	ON_MESSAGE	  (uV6,OnNewV6Limits)
	ON_MESSAGE    (uH7,OnNewH7Limits)
	ON_MESSAGE	  (uV7,OnNewV7Limits)
	ON_MESSAGE    (uH8,OnNewH8Limits)
	ON_MESSAGE	  (uV8,OnNewV8Limits)
	ON_BN_CLICKED(IDC_BUTTON_CONTRACT, OnButtonContract)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataWatchDlg message handlers
BOOL CDataWatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

//	// IDM_ABOUTBOX must be in the system command range.
//	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
//	ASSERT(IDM_ABOUTBOX < 0xF000);
//
//	CMenu* pSysMenu = GetSystemMenu(FALSE);
//	if (pSysMenu != NULL)
//	{
//		CString strAboutMenu;
//		strAboutMenu.LoadString(IDS_GRANDABOUTBOX);
//		if (!strAboutMenu.IsEmpty())
//		{
//			pSysMenu->AppendMenu(MF_SEPARATOR);
//			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
//		}
//	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetWindowText(m_cTitle);
	m_bConfig     = false;
	m_uiCurrentHour = 0;

	if (m_VertFont.CreateFont(8,0,0,-900,FW_BOLD,0,0,0,0,0,0,0,0,"MS Sans Serif"))
	{
		GetDlgItem(IDC_STATIC_CHART1)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART2)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART3)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART4)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART5)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART6)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART7)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART8)->SetFont(&m_VertFont);

		GetDlgItem(IDC_STATIC_CHART1B)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART2B)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART3B)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART4B)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART5B)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART6B)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART7B)->SetFont(&m_VertFont);
		GetDlgItem(IDC_STATIC_CHART8B)->SetFont(&m_VertFont);

		GetDlgItem(IDC_STATIC_CHART1)->SetWindowText(m_cs1);
		GetDlgItem(IDC_STATIC_CHART2)->SetWindowText(m_cs2);
		GetDlgItem(IDC_STATIC_CHART3)->SetWindowText(m_cs3);
		GetDlgItem(IDC_STATIC_CHART4)->SetWindowText(m_cs4);
		GetDlgItem(IDC_STATIC_CHART5)->SetWindowText(m_cs5);
		GetDlgItem(IDC_STATIC_CHART6)->SetWindowText(m_cs6);
		GetDlgItem(IDC_STATIC_CHART7)->SetWindowText(m_cs7);
		GetDlgItem(IDC_STATIC_CHART8)->SetWindowText(m_cs8);

		GetDlgItem(IDC_CHART1H_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART1H_MAX)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART1V_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART1V_MAX)->SetFont(&m_VertFont);

		GetDlgItem(IDC_CHART2H_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART2H_MAX)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART2V_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART2V_MAX)->SetFont(&m_VertFont);

		GetDlgItem(IDC_CHART3H_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART3H_MAX)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART3V_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART3V_MAX)->SetFont(&m_VertFont);

		GetDlgItem(IDC_CHART4H_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART4H_MAX)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART4V_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART4V_MAX)->SetFont(&m_VertFont);

		GetDlgItem(IDC_CHART5H_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART5H_MAX)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART5V_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART5V_MAX)->SetFont(&m_VertFont);

		GetDlgItem(IDC_CHART6H_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART6H_MAX)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART6V_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART6V_MAX)->SetFont(&m_VertFont);

		GetDlgItem(IDC_CHART7H_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART7H_MAX)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART7V_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART7V_MAX)->SetFont(&m_VertFont);

		GetDlgItem(IDC_CHART8H_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART8H_MAX)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART8V_MIN)->SetFont(&m_VertFont);
		GetDlgItem(IDC_CHART8V_MAX)->SetFont(&m_VertFont);

		m_cButtonExpand.SetFont(&m_VertFont);
		m_cButtonContract.SetFont(&m_VertFont);

		m_cLocked.SetFont(&m_VertFont);

	}

	GetWindowRect(&m_WindowRect);
	ClientToScreen(&m_WindowRect);

	CRect rectChart,rect1Chart,rect2Chart,rect3Chart,rect4Chart,
					rect5Chart,rect6Chart,rect7Chart,rect8Chart;
	GetClientRect(&rectChart);
	rect1Chart = rect2Chart = rect3Chart = rect4Chart = rect5Chart = rect6Chart = rect7Chart = rect8Chart = rectChart;

	rect1Chart.bottom /= 8;

	rect2Chart.top = rect1Chart.bottom;
	rect2Chart.bottom = rect1Chart.bottom*2;

	rect3Chart.top = rect2Chart.bottom;
	rect3Chart.bottom = rect1Chart.bottom*3;

	rect4Chart.top = rect3Chart.bottom;
	rect4Chart.bottom = rect1Chart.bottom*4;

	rect5Chart.top = rect4Chart.bottom;
	rect5Chart.bottom = rect1Chart.bottom*5;

	rect6Chart.top = rect5Chart.bottom;
	rect6Chart.bottom = rect1Chart.bottom*6;

	rect7Chart.top = rect6Chart.bottom;
	rect7Chart.bottom = rect1Chart.bottom*7;

	rect8Chart.top = rect7Chart.bottom;
	rect8Chart.bottom = rect1Chart.bottom*8;

	m_wnd1Chart.Create( WS_VISIBLE | GS_NOTIFY,
                        rect1Chart,
                        this,
                        IDR_CHART1,uH1,uV1,uFP1,uLP1);
	m_wnd2Chart.Create( WS_VISIBLE | GS_NOTIFY,
                        rect2Chart,
                        this,
                        IDR_CHART2,uH2,uV2,uFP2,uLP2);
	m_wnd3Chart.Create( WS_VISIBLE | GS_NOTIFY,
                        rect3Chart,
                        this,
                        IDR_CHART3,uH3,uV3,uFP3,uLP3);
	m_wnd4Chart.Create( WS_VISIBLE | GS_NOTIFY,
                        rect4Chart,
                        this,
                        IDR_CHART4,uH4,uV4,uFP4,uLP4);
	m_wnd5Chart.Create( WS_VISIBLE | GS_NOTIFY,
                        rect5Chart,
                        this,
                        IDR_CHART5,uH5,uV5,uFP5,uLP5);
	m_wnd6Chart.Create( WS_VISIBLE | GS_NOTIFY,
                        rect6Chart,
                        this,
                        IDR_CHART5,uH6,uV6,uFP6,uLP6);
	m_wnd7Chart.Create( WS_VISIBLE | GS_NOTIFY,
                        rect7Chart,
                        this,
                        IDR_CHART7,uH7,uV7,uFP7,uLP7);
	m_wnd8Chart.Create( WS_VISIBLE | GS_NOTIFY,
                        rect8Chart,
                        this,
                        IDR_CHART8,uH8,uV8,uFP8,uLP8);

	ResizeChildren(0);

	m_wnd1Chart.AddLine(m_uiLineIndex, RGB(255,0,0), LS_HORZAUTO);
	m_wnd2Chart.AddLine(m_uiLineIndex, RGB(255,0,0), LS_HORZAUTO);
	m_wnd3Chart.AddLine(m_uiLineIndex, RGB(255,0,0), LS_HORZAUTO);
	m_wnd4Chart.AddLine(m_uiLineIndex, RGB(255,0,0), LS_HORZAUTO);
	m_wnd5Chart.AddLine(m_uiLineIndex, RGB(255,0,0), LS_HORZAUTO);
	m_wnd6Chart.AddLine(m_uiLineIndex, RGB(255,0,0), LS_HORZAUTO);
	m_wnd7Chart.AddLine(m_uiLineIndex, RGB(255,0,0), LS_HORZAUTO);
	m_wnd8Chart.AddLine(m_uiLineIndex, RGB(255,0,0), LS_HORZAUTO);

	m_wnd1Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
	m_wnd2Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
	m_wnd3Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
	m_wnd4Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
	m_wnd5Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
	m_wnd6Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
	m_wnd7Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
	m_wnd8Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);

	m_wnd1Chart.SetMaxPoints(36000);
	m_wnd2Chart.SetMaxPoints(36000);
	m_wnd3Chart.SetMaxPoints(36000);
	m_wnd4Chart.SetMaxPoints(36000);
	m_wnd5Chart.SetMaxPoints(36000);
	m_wnd6Chart.SetMaxPoints(36000);
	m_wnd7Chart.SetMaxPoints(36000);
	m_wnd8Chart.SetMaxPoints(36000);

	COleDateTime cNow = COleDateTime::GetCurrentTime();

	m_cLocked.SetCheck(1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDataWatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
//	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
//	{
//		CAboutDlg dlgAbout;
//		CString cName;
//		CString cTitle("About ");
//		cTitle += "GRAND ";
//		cName = "GRAND ";
//		cTitle += "Watcher";
//		cName += "Watcher, Version: ";
//		cName += VERSION;
//		dlgAbout.SetTitleName(cTitle,cName);
//		dlgAbout.DoModal();
//	}
//	else
//	{
		CDialog::OnSysCommand(nID, lParam);
//	}
}

void CDataWatchDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CDataWatchDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDataWatchDlg::OnTimer(UINT nIDEvent) 
{
	static unsigned int uiTime = 1700000000;
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == RESIZETIMER)
	{
		ResizeChildren(0);
		KillTimer(RESIZETIMER);
		Invalidate();
	}

//	else if (nIDEvent == 10101)
//	{
//		float f1,f2,f3,f4,f5;
//		f1 = rand();
//		f2 = rand();
//		f3 = rand()/100;
//		f4 = rand();
//		f5 = rand();
//		SetData(uiTime,f1,f2,f3,f4,f5,true);
//		uiTime++;
//	}

	CDialog::OnTimer(nIDEvent);
}

void CDataWatchDlg::SetData(unsigned int uiTime, float f1, float f2, float f3, float f4, 
												 float f5, float f6, float f7, float f8,bool bDraw)
{
	if (m_uBaseTime == 0)
	{
		m_uBaseTime = (uiTime/3600)*3600;
		m_wnd1Chart.SetBaseTime(m_uBaseTime);
		m_wnd2Chart.SetBaseTime(m_uBaseTime);
		m_wnd3Chart.SetBaseTime(m_uBaseTime);
		m_wnd4Chart.SetBaseTime(m_uBaseTime);
		m_wnd5Chart.SetBaseTime(m_uBaseTime);
		m_wnd6Chart.SetBaseTime(m_uBaseTime);
		m_wnd7Chart.SetBaseTime(m_uBaseTime);
		m_wnd8Chart.SetBaseTime(m_uBaseTime);
	}

	if (uiTime > m_uiLastTime)
	{
		m_wnd1Chart.AddData(1,uiTime-m_uBaseTime,f1,bDraw);
		m_wnd2Chart.AddData(1,uiTime-m_uBaseTime,f2,bDraw);
		m_wnd3Chart.AddData(1,uiTime-m_uBaseTime,f3,bDraw);
		m_wnd4Chart.AddData(1,uiTime-m_uBaseTime,f4,bDraw);
		m_wnd5Chart.AddData(1,uiTime-m_uBaseTime,f5,bDraw);
		m_wnd6Chart.AddData(1,uiTime-m_uBaseTime,f6,bDraw);
		m_wnd7Chart.AddData(1,uiTime-m_uBaseTime,f7,bDraw);
		m_wnd8Chart.AddData(1,uiTime-m_uBaseTime,f8,bDraw);
		m_uiLastTime = uiTime;
		m_bNewLine = true;
	}
	else
	{
		if (m_bNewLine)
		{
			CJulianTime cjTime;
			CJulianTime cjTimeLast;
			cjTime.Init(uiTime);
			cjTimeLast.Init(m_uiLastTime);
		}

		if (uiTime > m_uBaseTime)
		{
			if (m_bNewLine)
			{
				m_uiLineIndex++;
				m_wnd1Chart.AddLine(m_uiLineIndex, RGB(255,128,255), LS_HORZAUTO);
				m_wnd1Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
				m_wnd2Chart.AddLine(m_uiLineIndex, RGB(255,128,255), LS_HORZAUTO);
				m_wnd2Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
				m_wnd3Chart.AddLine(m_uiLineIndex, RGB(255,128,255), LS_HORZAUTO);
				m_wnd3Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
				m_wnd4Chart.AddLine(m_uiLineIndex, RGB(255,128,255), LS_HORZAUTO);
				m_wnd4Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
				m_wnd5Chart.AddLine(m_uiLineIndex, RGB(255,128,255), LS_HORZAUTO);
				m_wnd5Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
				m_wnd6Chart.AddLine(m_uiLineIndex, RGB(255,128,255), LS_HORZAUTO);
				m_wnd6Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
				m_wnd7Chart.AddLine(m_uiLineIndex, RGB(255,128,255), LS_HORZAUTO);
				m_wnd7Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);
				m_wnd8Chart.AddLine(m_uiLineIndex, RGB(255,128,255), LS_HORZAUTO);
				m_wnd8Chart.ShowPoints(m_uiLineIndex,TRUE,FALSE);

				m_bNewLine = false;
			}
			m_wnd1Chart.AddData(m_uiLineIndex,uiTime-m_uBaseTime,f1,bDraw);
			m_wnd2Chart.AddData(m_uiLineIndex,uiTime-m_uBaseTime,f2,bDraw);
			m_wnd3Chart.AddData(m_uiLineIndex,uiTime-m_uBaseTime,f3,bDraw);
			m_wnd4Chart.AddData(m_uiLineIndex,uiTime-m_uBaseTime,f4,bDraw);
			m_wnd5Chart.AddData(m_uiLineIndex,uiTime-m_uBaseTime,f5,bDraw);
			m_wnd6Chart.AddData(m_uiLineIndex,uiTime-m_uBaseTime,f6,bDraw);
			m_wnd7Chart.AddData(m_uiLineIndex,uiTime-m_uBaseTime,f7,bDraw);
			m_wnd8Chart.AddData(m_uiLineIndex,uiTime-m_uBaseTime,f8,bDraw);
		}
	}
}

LRESULT CDataWatchDlg::OnData1FPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nFDP:",m_cs1);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nFDP:\n%s\n%.0f",m_cs1,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART1)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData2FPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nFDP:",m_cs2);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nFDP:\n%s\n%.0f",m_cs2,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART2)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData3FPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nFDP:",m_cs3);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nFDP:\n%s\n%.0f",m_cs3,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART3)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData4FPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nFDP:",m_cs4);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nFDP:\n%s\n%.0f",m_cs4,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART4)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData5FPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nFDP:",m_cs5);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nFDP:\n%s\n%.0f",m_cs5,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART5)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData6FPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nFDP:",m_cs6);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nFDP:\n%s\n%.0f",m_cs6,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART6)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData7FPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nFDP:",m_cs7);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nFDP:\n%s\n%.0f",m_cs7,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART7)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData8FPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nFDP:",m_cs8);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nFDP:\n%s\n%.0f",m_cs8,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART8)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData1LPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nLDP:",m_cs1);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nLDP:\n%s\n%.0f",m_cs1,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART1B)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData2LPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nLDP:",m_cs2);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nLDP:\n%s\n%.0f",m_cs2,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART2B)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData3LPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nLDP:",m_cs3);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nLDP:\n%s\n%.0f",m_cs3,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART3B)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData4LPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nLDP:",m_cs4);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nLDP:\n%s\n%.0f",m_cs4,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART4B)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData5LPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nLDP:",m_cs5);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nLDP:\n%s\n%.0f",m_cs5,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART5B)->SetWindowText(cTemp);
}

LRESULT CDataWatchDlg::OnData6LPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nLDP:",m_cs6);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nLDP:\n%s\n%.0f",m_cs6,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART6B)->SetWindowText(cTemp);
}
LRESULT CDataWatchDlg::OnData7LPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nLDP:",m_cs7);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nLDP:\n%s\n%.0f",m_cs7,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART7B)->SetWindowText(cTemp);
}
LRESULT CDataWatchDlg::OnData8LPoint(WPARAM wParam, LPARAM lParam)
{
	CString cTemp;
	if (wParam == NULL && lParam == NULL)
	{
		cTemp.Format("%s\nLDP:",m_cs8);
	}
	else
	{
		CJulianTime cJTime;
		cJTime.Init((double)wParam + m_uBaseTime);
		cTemp.Format("%s\nLDP:\n%s\n%.0f",m_cs8,cJTime.Time(),(double)lParam);
	}
	GetDlgItem(IDC_STATIC_CHART8B)->SetWindowText(cTemp);
}

void CDataWatchDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	CRect cRect(lpRect);
	if (cRect.Height() < m_WindowRect.Height() || cRect.Width() < m_WindowRect.Width())
	{
		switch (nSide) {
		case WMSZ_TOP:
			lpRect->top = lpRect->bottom - m_WindowRect.Height();
			break;
		case WMSZ_BOTTOM:
			lpRect->bottom = lpRect->top + m_WindowRect.Height();
			break;
		case WMSZ_LEFT:
			lpRect->left = lpRect->right - m_WindowRect.Width();
			break;
		case WMSZ_RIGHT:
			lpRect->right = lpRect->left + m_WindowRect.Width();
			break;
		case WMSZ_BOTTOMLEFT:
			lpRect->bottom = lpRect->top + m_WindowRect.Height();
			lpRect->left = lpRect->right - m_WindowRect.Width();
			break;
		case WMSZ_BOTTOMRIGHT:
			lpRect->bottom = lpRect->top + m_WindowRect.Height();
			lpRect->right = lpRect->left + m_WindowRect.Width();
			break;
		case WMSZ_TOPLEFT:
			lpRect->top = lpRect->bottom - m_WindowRect.Height();
			lpRect->left = lpRect->right - m_WindowRect.Width();
			break;
		case WMSZ_TOPRIGHT:
			lpRect->top = lpRect->bottom - m_WindowRect.Height();
			lpRect->right = lpRect->left + m_WindowRect.Width();
			break;
		}
	}

	CDialog::OnSizing(nSide,lpRect);
}

void CDataWatchDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	SetTimer(RESIZETIMER,200,NULL);
}

void CDataWatchDlg::ResizeChildren(int iOversize)
{

	CRect rectClient,rectChart,rect1Chart,rect2Chart,rect3Chart,rect4Chart,
							   rect5Chart,rect6Chart,rect7Chart,rect8Chart,rectTemp;
	GetClientRect(&rectClient);

	//don't like using a fixed value
	//'cause it will look odd with 
	//different screen resolutions.
	int iBorder = 10;
//	if (IsZoomed())
//		iBorder = 4;

	int iLeft  = rectClient.Width()/100;
	int iRight = rectClient.Width() - iLeft*iBorder;
	int iSpace = rectClient.Height()/40;

//	rectChart.left = iLeft*iBorder;
//	rectChart.right = iRight;

	rectChart.left = 8 * iBorder;
	rectChart.right = rectClient.Width() - 8*iBorder;

	rectChart.top = rectClient.top+iSpace;

	if (iOversize == 0)
	{
		rectChart.bottom = rectClient.bottom;
		rect1Chart = rect2Chart = rect3Chart = rect4Chart = rect5Chart = rect6Chart = rect7Chart = rect8Chart = rectChart;

		rect1Chart.bottom = rectChart.bottom/11+iSpace;

		rect2Chart.top = rect1Chart.bottom + iSpace;
		rect2Chart.bottom = rect1Chart.bottom + iSpace + rect1Chart.Height();

		rect3Chart.top = rect2Chart.bottom + iSpace;
		rect3Chart.bottom = rect2Chart.bottom + iSpace + rect1Chart.Height();

		rect4Chart.top = rect3Chart.bottom + iSpace + 5;
		rect4Chart.bottom = rect3Chart.bottom + iSpace + rect1Chart.Height() + 5;

		rect5Chart.top = rect4Chart.bottom + iSpace + 5;
		rect5Chart.bottom = rect4Chart.bottom + iSpace + rect1Chart.Height() + 5;

		rect6Chart.top = rect5Chart.bottom + iSpace + 5;
		rect6Chart.bottom = rect5Chart.bottom + iSpace + rect1Chart.Height() + 5;

		rect7Chart.top = rect6Chart.bottom + iSpace + 5;
		rect7Chart.bottom = rect6Chart.bottom + iSpace + rect1Chart.Height() + 5;

		rect8Chart.top = rect7Chart.bottom + iSpace + 5;
		rect8Chart.bottom = rect7Chart.bottom + iSpace + rect1Chart.Height() + 5;
	}
	else
	{
		rectChart.bottom = rectClient.bottom;
		int iHeight = rectChart.Height()/12;
		rect1Chart = rect2Chart = rect3Chart = rect4Chart = rect5Chart = rect6Chart = rect7Chart = rect8Chart = rectChart;
		
		rect1Chart.bottom = (iOversize==1)?rect1Chart.top+iHeight*2+iSpace:rect1Chart.top+iHeight;

		rect2Chart.top = rect1Chart.bottom + iSpace;
		rect2Chart.bottom = (iOversize==2)?rect1Chart.bottom + iSpace + iHeight*2:rect1Chart.bottom + iSpace + iHeight;

		rect3Chart.top = rect2Chart.bottom + iSpace;
		rect3Chart.bottom = (iOversize==3)?rect2Chart.bottom + iSpace + iHeight*2:rect2Chart.bottom + iSpace + iHeight;

		rect4Chart.top = rect3Chart.bottom + iSpace;
		rect4Chart.bottom = (iOversize==4)?rect3Chart.bottom + iSpace + iHeight*2:rect3Chart.bottom + iSpace + iHeight;

		rect5Chart.top = rect4Chart.bottom + iSpace;
		rect5Chart.bottom = (iOversize==5)?rect4Chart.bottom + iSpace + iHeight*2:rect4Chart.bottom + iSpace + iHeight;

		rect6Chart.top = rect5Chart.bottom + iSpace;
		rect6Chart.bottom = (iOversize==6)?rect5Chart.bottom + iSpace + iHeight*2:rect5Chart.bottom + iSpace + iHeight;

		rect7Chart.top = rect6Chart.bottom + iSpace;
		rect7Chart.bottom = (iOversize==7)?rect6Chart.bottom + iSpace + iHeight*2:rect6Chart.bottom + iSpace + iHeight;

		rect8Chart.top = rect7Chart.bottom + iSpace;
		rect8Chart.bottom = (iOversize==8)?rect7Chart.bottom + iSpace + iHeight*2:rect7Chart.bottom + iSpace + iHeight;
		Invalidate();

	}

	m_wnd1Chart.MoveWindow(rect1Chart);
	m_wnd2Chart.MoveWindow(rect2Chart);
	m_wnd3Chart.MoveWindow(rect3Chart);
	m_wnd4Chart.MoveWindow(rect4Chart);
	m_wnd5Chart.MoveWindow(rect5Chart);
	m_wnd6Chart.MoveWindow(rect6Chart);
	m_wnd7Chart.MoveWindow(rect7Chart);
	m_wnd8Chart.MoveWindow(rect8Chart);

	m_cLocked.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	int iHeight = rectTemp.Height();
	int iWidth = rectTemp.Width();
	rectTemp.top = rect8Chart.bottom;
	rectTemp.bottom = rectTemp.top + iHeight;
	rectTemp.left = (rect8Chart.left + rect5Chart.right - iWidth)/2;
	rectTemp.right = rectTemp.left + iWidth;
	rectTemp.left = (rect8Chart.left + rect5Chart.right - iWidth)/2;
	rectTemp.right = rectTemp.left + iWidth;
	m_cLocked.MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART1V_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect1Chart.left-4;

	rectTemp.top = rect1Chart.top - iHeight/2;
	rectTemp.bottom = rect1Chart.top + iHeight/2;
	GetDlgItem(IDC_CHART1V_MAX)->MoveWindow(&rectTemp);

	rectTemp.left = rectClient.left;
	rectTemp.right = rect1Chart.left;

	rectTemp.top = rect1Chart.top + iHeight;
	rectTemp.bottom = rect1Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART1)->MoveWindow(&rectTemp);

	rectTemp.top = rect2Chart.top + iHeight;
	rectTemp.bottom = rect2Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART2)->MoveWindow(&rectTemp);

	rectTemp.top = rect3Chart.top + iHeight;
	rectTemp.bottom = rect3Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART3)->MoveWindow(&rectTemp);

	rectTemp.top = rect4Chart.top + iHeight;
	rectTemp.bottom = rect4Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART4)->MoveWindow(&rectTemp);

	rectTemp.top = rect5Chart.top + iHeight;
	rectTemp.bottom = rect5Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART5)->MoveWindow(&rectTemp);

	rectTemp.top = rect6Chart.top + iHeight;
	rectTemp.bottom = rect6Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART6)->MoveWindow(&rectTemp);

	rectTemp.top = rect7Chart.top + iHeight;
	rectTemp.bottom = rect7Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART7)->MoveWindow(&rectTemp);

	rectTemp.top = rect8Chart.top + iHeight;
	rectTemp.bottom = rect8Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART8)->MoveWindow(&rectTemp);

	rectTemp.left = rect1Chart.right;
	rectTemp.right = rectClient.right;

	rectTemp.top = rect1Chart.top + iHeight;
	rectTemp.bottom = rect1Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART1B)->MoveWindow(&rectTemp);

	rectTemp.top = rect2Chart.top + iHeight;
	rectTemp.bottom = rect2Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART2B)->MoveWindow(&rectTemp);

	rectTemp.top = rect3Chart.top + iHeight;
	rectTemp.bottom = rect3Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART3B)->MoveWindow(&rectTemp);

	rectTemp.top = rect4Chart.top + iHeight;
	rectTemp.bottom = rect4Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART4B)->MoveWindow(&rectTemp);

	rectTemp.top = rect5Chart.top + iHeight;
	rectTemp.bottom = rect5Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART5B)->MoveWindow(&rectTemp);

	rectTemp.top = rect6Chart.top + iHeight;
	rectTemp.bottom = rect6Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART6B)->MoveWindow(&rectTemp);

	rectTemp.top = rect7Chart.top + iHeight;
	rectTemp.bottom = rect7Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART7B)->MoveWindow(&rectTemp);

	rectTemp.top = rect8Chart.top + iHeight;
	rectTemp.bottom = rect8Chart.bottom - iHeight*2;
	GetDlgItem(IDC_STATIC_CHART8B)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART1V_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect1Chart.left-4;
	rectTemp.top = rect1Chart.bottom - iHeight*2;
	rectTemp.bottom = rect1Chart.bottom - iHeight;
	GetDlgItem(IDC_CHART1V_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART1H_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect1Chart.left;
	rectTemp.right = rect1Chart.left+iWidth;
	rectTemp.top = rect1Chart.bottom;
	rectTemp.bottom = rect1Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART1H_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART1H_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect1Chart.right - iWidth;
	rectTemp.right = rect1Chart.right;
	rectTemp.top = rect1Chart.bottom;
	rectTemp.bottom = rect1Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART1H_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART2V_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect2Chart.left-4;
	rectTemp.top = rect2Chart.top - iHeight/2;
	rectTemp.bottom = rect2Chart.top + iHeight/2;
	GetDlgItem(IDC_CHART2V_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART2V_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect2Chart.left-4;
	rectTemp.top = rect2Chart.bottom - iHeight*2;
	rectTemp.bottom = rect2Chart.bottom - iHeight;
	GetDlgItem(IDC_CHART2V_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART2H_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect2Chart.left;
	rectTemp.right = rect2Chart.left+iWidth;
	rectTemp.top = rect2Chart.bottom;
	rectTemp.bottom = rect2Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART2H_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART2H_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect2Chart.right - iWidth;
	rectTemp.right = rect2Chart.right;
	rectTemp.top = rect2Chart.bottom;
	rectTemp.bottom = rect2Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART2H_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART3V_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect3Chart.left-4;
	rectTemp.top = rect3Chart.top - iHeight/2;
	rectTemp.bottom = rect3Chart.top + iHeight/2;
	GetDlgItem(IDC_CHART3V_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART3V_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect3Chart.left-4;
	rectTemp.top = rect3Chart.bottom - iHeight*2;
	rectTemp.bottom = rect3Chart.bottom - iHeight;
	GetDlgItem(IDC_CHART3V_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART3H_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect3Chart.left;
	rectTemp.right = rect3Chart.left+iWidth;
	rectTemp.top = rect3Chart.bottom;
	rectTemp.bottom = rect3Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART3H_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART3H_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect3Chart.right - iWidth;
	rectTemp.right = rect3Chart.right;
	rectTemp.top = rect3Chart.bottom;
	rectTemp.bottom = rect3Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART3H_MAX)->MoveWindow(&rectTemp);


	GetDlgItem(IDC_CHART4V_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect4Chart.left-4;
	rectTemp.top = rect4Chart.top - iHeight/2;
	rectTemp.bottom = rect4Chart.top + iHeight/2;
	GetDlgItem(IDC_CHART4V_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART4V_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect4Chart.left-4;
	rectTemp.top = rect4Chart.bottom - iHeight*2;
	rectTemp.bottom = rect4Chart.bottom - iHeight;
	GetDlgItem(IDC_CHART4V_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART4H_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect4Chart.left;
	rectTemp.right = rect4Chart.left+iWidth;
	rectTemp.top = rect4Chart.bottom;
	rectTemp.bottom = rect4Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART4H_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART4H_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect4Chart.right - iWidth;
	rectTemp.right = rect4Chart.right;
	rectTemp.top = rect4Chart.bottom;
	rectTemp.bottom = rect4Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART4H_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART5V_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect5Chart.left-4;
	rectTemp.top = rect5Chart.top - iHeight/2;
	rectTemp.bottom = rect5Chart.top + iHeight/2;
	GetDlgItem(IDC_CHART5V_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART5V_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect5Chart.left-4;
	rectTemp.top = rect5Chart.bottom - iHeight*2;
	rectTemp.bottom = rect5Chart.bottom - iHeight;
	GetDlgItem(IDC_CHART5V_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART5H_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect5Chart.left;
	rectTemp.right = rect5Chart.left+iWidth;
	rectTemp.top = rect5Chart.bottom;
	rectTemp.bottom = rect5Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART5H_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART5H_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect5Chart.right - iWidth;
	rectTemp.right = rect5Chart.right;
	rectTemp.top = rect5Chart.bottom;
	rectTemp.bottom = rect5Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART5H_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART6V_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect6Chart.left-4;
	rectTemp.top = rect6Chart.top - iHeight/2;
	rectTemp.bottom = rect6Chart.top + iHeight/2;
	GetDlgItem(IDC_CHART6V_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART6V_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect6Chart.left-4;
	rectTemp.top = rect6Chart.bottom - iHeight*2;
	rectTemp.bottom = rect6Chart.bottom - iHeight;
	GetDlgItem(IDC_CHART6V_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART6H_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect6Chart.left;
	rectTemp.right = rect6Chart.left+iWidth;
	rectTemp.top = rect6Chart.bottom;
	rectTemp.bottom = rect6Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART6H_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART6H_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect6Chart.right - iWidth;
	rectTemp.right = rect6Chart.right;
	rectTemp.top = rect6Chart.bottom;
	rectTemp.bottom = rect6Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART6H_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART7V_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect7Chart.left-4;
	rectTemp.top = rect7Chart.top - iHeight/2;
	rectTemp.bottom = rect7Chart.top + iHeight/2;
	GetDlgItem(IDC_CHART7V_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART7V_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect7Chart.left-4;
	rectTemp.top = rect7Chart.bottom - iHeight*2;
	rectTemp.bottom = rect7Chart.bottom - iHeight;
	GetDlgItem(IDC_CHART7V_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART7H_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect7Chart.left;
	rectTemp.right = rect7Chart.left+iWidth;
	rectTemp.top = rect7Chart.bottom;
	rectTemp.bottom = rect7Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART7H_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART7H_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect7Chart.right - iWidth;
	rectTemp.right = rect7Chart.right;
	rectTemp.top = rect7Chart.bottom;
	rectTemp.bottom = rect7Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART7H_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART8V_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect8Chart.left-4;
	rectTemp.top = rect8Chart.top - iHeight/2;
	rectTemp.bottom = rect8Chart.top + iHeight/2;
	GetDlgItem(IDC_CHART8V_MAX)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART8V_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	rectTemp.left = 0;
	rectTemp.right = rect8Chart.left-4;
	rectTemp.top = rect8Chart.bottom - iHeight*2;
	rectTemp.bottom = rect8Chart.bottom - iHeight;
	GetDlgItem(IDC_CHART8V_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART8H_MIN)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect8Chart.left;
	rectTemp.right = rect8Chart.left+iWidth;
	rectTemp.top = rect8Chart.bottom;
	rectTemp.bottom = rect8Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART8H_MIN)->MoveWindow(&rectTemp);

	GetDlgItem(IDC_CHART8H_MAX)->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	iWidth = rectTemp.Width();
	rectTemp.left = rect8Chart.right - iWidth;
	rectTemp.right = rect8Chart.right;
	rectTemp.top = rect8Chart.bottom;
	rectTemp.bottom = rect8Chart.bottom + iHeight;
	GetDlgItem(IDC_CHART8H_MAX)->MoveWindow(&rectTemp);

	m_cButtonExpand.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	m_cButtonExpand.MoveWindow(2,rectClient.bottom - iHeight,rect8Chart.left - 4,iHeight);

	m_cButtonContract.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	iHeight = rectTemp.Height();
	m_cButtonContract.MoveWindow(rect8Chart.right+2,rectClient.bottom - iHeight, rect8Chart.left - 4,iHeight);

}

LRESULT CDataWatchDlg::OnNewV1Limits(WPARAM wParam, LPARAM lParam)
{
	CString strTmp;
	float fMin, fMax;
	m_wnd1Chart.GetVertMinMax(&fMin,&fMax);
	strTmp.Format("%6.0f",fMax);
	SetDlgItemText(IDC_CHART1V_MAX,strTmp);
	strTmp.Format("%6.0f",fMin);
	SetDlgItemText(IDC_CHART1V_MIN,strTmp);
	int iDiff = ceil(fMax) - floor(fMin);
	m_wnd1Chart.SetVertDiv(iDiff);
	return 0;
}

LRESULT CDataWatchDlg::OnNewV2Limits(WPARAM wParam, LPARAM lParam)
{
	CString strTmp;
	float fMin, fMax;
	m_wnd2Chart.GetVertMinMax(&fMin,&fMax);
	strTmp.Format("%6.0f",ceil(fMax));
	SetDlgItemText(IDC_CHART2V_MAX,strTmp);
	strTmp.Format("%6.0f",floor(fMin));
	SetDlgItemText(IDC_CHART2V_MIN,strTmp);
	int iDiff = ceil(fMax) - floor(fMin);
	m_wnd2Chart.SetVertDiv(iDiff);
}

LRESULT CDataWatchDlg::OnNewV3Limits(WPARAM wParam, LPARAM lParam)
{
	CString strTmp;
	float fMin, fMax;
	m_wnd3Chart.GetVertMinMax(&fMin,&fMax);
	strTmp.Format("%6.0f",ceil(fMax));
	SetDlgItemText(IDC_CHART3V_MAX,strTmp);
	strTmp.Format("%6.0f",floor(fMin));
	SetDlgItemText(IDC_CHART3V_MIN,strTmp);
	int iDiff = ceil(fMax) - floor(fMin);
	m_wnd3Chart.SetVertDiv(iDiff);
	return 0;
}

LRESULT CDataWatchDlg::OnNewV4Limits(WPARAM wParam, LPARAM lParam)
{
	CString strTmp;
	float fMin, fMax;
	m_wnd4Chart.GetVertMinMax(&fMin,&fMax);
	strTmp.Format("%6.0f",ceil(fMax));
	SetDlgItemText(IDC_CHART4V_MAX,strTmp);
	strTmp.Format("%6.0f",floor(fMin));
	SetDlgItemText(IDC_CHART4V_MIN,strTmp);
	int iDiff = ceil(fMax) - floor(fMin);
	m_wnd4Chart.SetVertDiv(iDiff);
	return 0;
}

LRESULT CDataWatchDlg::OnNewV5Limits(WPARAM wParam, LPARAM lParam)
{
	CString strTmp;
	float fMin, fMax;
	m_wnd5Chart.GetVertMinMax(&fMin,&fMax);
	strTmp.Format("%6.0f",ceil(fMax));
	SetDlgItemText(IDC_CHART5V_MAX,strTmp);
	strTmp.Format("%6.0f",floor(fMin));
	SetDlgItemText(IDC_CHART5V_MIN,strTmp);
	int iDiff = ceil(fMax) - floor(fMin);
	m_wnd5Chart.SetVertDiv(iDiff);
}

LRESULT CDataWatchDlg::OnNewV6Limits(WPARAM wParam, LPARAM lParam)
{
	CString strTmp;
	float fMin, fMax;
	m_wnd6Chart.GetVertMinMax(&fMin,&fMax);
	strTmp.Format("%6.0f",ceil(fMax));
	SetDlgItemText(IDC_CHART6V_MAX,strTmp);
	strTmp.Format("%6.0f",floor(fMin));
	SetDlgItemText(IDC_CHART6V_MIN,strTmp);
	int iDiff = ceil(fMax) - floor(fMin);
	m_wnd6Chart.SetVertDiv(iDiff);
	return 0;
}

LRESULT CDataWatchDlg::OnNewV7Limits(WPARAM wParam, LPARAM lParam)
{
	CString strTmp;
	float fMin, fMax;
	m_wnd7Chart.GetVertMinMax(&fMin,&fMax);
	strTmp.Format("%6.0f",ceil(fMax));
	SetDlgItemText(IDC_CHART7V_MAX,strTmp);
	strTmp.Format("%6.0f",floor(fMin));
	SetDlgItemText(IDC_CHART7V_MIN,strTmp);
	int iDiff = ceil(fMax) - floor(fMin);
	m_wnd7Chart.SetVertDiv(iDiff);
}

LRESULT CDataWatchDlg::OnNewV8Limits(WPARAM wParam, LPARAM lParam)
{
	CString strTmp;
	float fMin, fMax;
	m_wnd8Chart.GetVertMinMax(&fMin,&fMax);
	strTmp.Format("%6.0f",ceil(fMax));
	SetDlgItemText(IDC_CHART8V_MAX,strTmp);
	strTmp.Format("%6.0f",floor(fMin));
	SetDlgItemText(IDC_CHART8V_MIN,strTmp);
	int iDiff = ceil(fMax) - floor(fMin);
	m_wnd8Chart.SetVertDiv(iDiff);
	return 0;
}


LRESULT CDataWatchDlg::OnScrollH1Msg(WPARAM wParam, LPARAM lParam)
{
	if (m_cLocked.GetCheck())
	{
		m_wnd2Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd3Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd4Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd5Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd6Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd7Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd8Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
	}
	return 0;
}
LRESULT CDataWatchDlg::OnScrollH2Msg(WPARAM wParam, LPARAM lParam)
{
	if (m_cLocked.GetCheck())
	{
		m_wnd1Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd3Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd4Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd5Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd6Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd7Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd8Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
	}
	return 0;
}
LRESULT CDataWatchDlg::OnScrollH3Msg(WPARAM wParam, LPARAM lParam)
{
	if (m_cLocked.GetCheck())
	{
		m_wnd1Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd2Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd4Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd5Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd6Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd7Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd8Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
	}
	return 0;
}
LRESULT CDataWatchDlg::OnScrollH4Msg(WPARAM wParam, LPARAM lParam)
{
	if (m_cLocked.GetCheck())
	{
		m_wnd1Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd2Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd3Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd5Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd6Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd7Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd8Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
	}
	return 0;
}
LRESULT CDataWatchDlg::OnScrollH5Msg(WPARAM wParam, LPARAM lParam)
{
	if (m_cLocked.GetCheck())
	{
		m_wnd1Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd2Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd3Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd4Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd6Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd7Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd8Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
	}
	return 0;
}
LRESULT CDataWatchDlg::OnScrollH6Msg(WPARAM wParam, LPARAM lParam)
{
	if (m_cLocked.GetCheck())
	{
		m_wnd1Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd2Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd3Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd4Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd5Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd7Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd8Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
	}
}
LRESULT CDataWatchDlg::OnScrollH7Msg(WPARAM wParam, LPARAM lParam)
{
	if (m_cLocked.GetCheck())
	{
		m_wnd1Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd2Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd3Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd4Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd5Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd6Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd8Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
	}
	return 0;
}
LRESULT CDataWatchDlg::OnScrollH8Msg(WPARAM wParam, LPARAM lParam)
{
	if (m_cLocked.GetCheck())
	{
		m_wnd1Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd2Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd3Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd4Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd5Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd6Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
		m_wnd7Chart.OnHScroll(wParam,lParam,(CScrollBar*)-1);
	}
	return 0;
}

LRESULT CDataWatchDlg::OnNewH1Limits(WPARAM wParam, LPARAM lParam)
{
	CJulianTime jTime;
	jTime.Init(m_uBaseTime+wParam);
	CString cTemp;
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART1H_MIN,cTemp);
	jTime.Init(m_uBaseTime+float(lParam));//+m_wnd1Chart.GetHorzRange());
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART1H_MAX,cTemp);
}

LRESULT CDataWatchDlg::OnNewH2Limits(WPARAM wParam, LPARAM lParam)
{
	CJulianTime jTime;
	jTime.Init(m_uBaseTime+wParam);
	CString cTemp;
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART2H_MIN,cTemp);
	jTime.Init(m_uBaseTime+float(lParam));//wParam+m_wnd2Chart.GetHorzRange());
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART2H_MAX,cTemp);
	return 0;
}

LRESULT CDataWatchDlg::OnNewH3Limits(WPARAM wParam, LPARAM lParam)
{
	CJulianTime jTime;
	jTime.Init(m_uBaseTime+wParam);
	CString cTemp;
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART3H_MIN,cTemp);
	jTime.Init(m_uBaseTime+float(lParam));//wParam+m_wnd3Chart.GetHorzRange());
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART3H_MAX,cTemp);
	return 0;
}

LRESULT CDataWatchDlg::OnNewH4Limits(WPARAM wParam, LPARAM lParam)
{
	CJulianTime jTime;
	jTime.Init(m_uBaseTime+wParam);
	CString cTemp;
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART4H_MIN,cTemp);
	jTime.Init(m_uBaseTime+float(lParam));//wParam+m_wnd3Chart.GetHorzRange());
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART4H_MAX,cTemp);
	return 0;
}

LRESULT CDataWatchDlg::OnNewH5Limits(WPARAM wParam, LPARAM lParam)
{
	CJulianTime jTime;
	jTime.Init(m_uBaseTime+wParam);
	CString cTemp;
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART5H_MIN,cTemp);
	jTime.Init(m_uBaseTime+float(lParam));//wParam+m_wnd3Chart.GetHorzRange());
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART5H_MAX,cTemp);
	return 0;
}
LRESULT CDataWatchDlg::OnNewH6Limits(WPARAM wParam, LPARAM lParam)
{
	CJulianTime jTime;
	jTime.Init(m_uBaseTime+wParam);
	CString cTemp;
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART6H_MIN,cTemp);
	jTime.Init(m_uBaseTime+float(lParam));//wParam+m_wnd3Chart.GetHorzRange());
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART6H_MAX,cTemp);
	return 0;
}
LRESULT CDataWatchDlg::OnNewH7Limits(WPARAM wParam, LPARAM lParam)
{
	CJulianTime jTime;
	jTime.Init(m_uBaseTime+wParam);
	CString cTemp;
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART7H_MIN,cTemp);
	jTime.Init(m_uBaseTime+float(lParam));//wParam+m_wnd3Chart.GetHorzRange());
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART7H_MAX,cTemp);
	return 0;
}
LRESULT CDataWatchDlg::OnNewH8Limits(WPARAM wParam, LPARAM lParam)
{
	CJulianTime jTime;
	jTime.Init(m_uBaseTime+wParam);
	CString cTemp;
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART8H_MIN,cTemp);
	jTime.Init(m_uBaseTime+float(lParam));//wParam+m_wnd3Chart.GetHorzRange());
	cTemp.Format("%s %s",jTime.m_pDate,jTime.m_pTime);
	SetDlgItemText(IDC_CHART8H_MAX,cTemp);
	return 0;
}

HBRUSH CDataWatchDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CDataWatchDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int i;
	CRect cRect[8];
	m_wnd1Chart.GetWindowRect(cRect[0]);
	m_wnd2Chart.GetWindowRect(cRect[1]);
	m_wnd3Chart.GetWindowRect(cRect[2]);
	m_wnd4Chart.GetWindowRect(cRect[3]);
	m_wnd5Chart.GetWindowRect(cRect[4]);
	m_wnd6Chart.GetWindowRect(cRect[5]);
	m_wnd7Chart.GetWindowRect(cRect[6]);
	m_wnd8Chart.GetWindowRect(cRect[7]);
	for (i = 0; i < 8; i++)
	{
		ScreenToClient(cRect[i]);
		if (point.y >= cRect[i].top && point.y <= cRect[i].bottom)
		{
			i++;
			if (m_iOversize == i) 
			{
				ResizeChildren(0);
				m_iOversize = 0;
			}
			else
			{
				ResizeChildren(i);	
				m_iOversize = i;
			}
			break;
		}
	}
	CDialog::OnLButtonDown(nFlags, point);

}

void CDataWatchDlg::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
/*
	m_uiMultiplier *= 2;
	if (m_uiMultiplier > 256)
		m_uiMultiplier = 256;
//TRACE("L Multiplier = %d\r\n",m_uiMultiplier);
	m_wnd1Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd2Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd3Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd4Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd5Chart.SetHorzDiv(15*m_uiMultiplier/2);

	m_wnd1Chart.SetHorzRange(450*m_uiMultiplier);
	m_wnd2Chart.SetHorzRange(450*m_uiMultiplier);
	m_wnd3Chart.SetHorzRange(450*m_uiMultiplier);
	m_wnd4Chart.SetHorzRange(450*m_uiMultiplier);
	m_wnd5Chart.SetHorzRange(450*m_uiMultiplier);

	CString cTemp;
	if (m_uiMultiplier > 8)
		cTemp.Format("Lock Scrolling Together (%5.0f hours displayed)",m_uiMultiplier/8.0);
	else if (m_uiMultiplier == 8)
		cTemp.Format("Lock Scrolling Together (%5.0f hour displayed)",m_uiMultiplier/8.0);
	else
	{
		float fTemp = (60.0 * m_uiMultiplier)/8.0;
		cTemp.Format("Lock Scrolling Together (%5.1f minutes displayed)",fTemp);
	}

	m_cLocked.SetWindowText(cTemp);
	
	m_pParent->RestartAutoCloseTimer();
*/
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CDataWatchDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
/*
	m_uiMultiplier /= 2;
	if (m_uiMultiplier < 1)
		m_uiMultiplier = 1;
	if (m_uiMultiplier > 1)
	{
		m_wnd1Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd2Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd3Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd4Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd5Chart.SetHorzDiv(15*m_uiMultiplier/2);
	}

	m_wnd1Chart.SetHorzRange(450*m_uiMultiplier);
	m_wnd2Chart.SetHorzRange(450*m_uiMultiplier);
	m_wnd3Chart.SetHorzRange(450*m_uiMultiplier);
	m_wnd4Chart.SetHorzRange(450*m_uiMultiplier);
	m_wnd5Chart.SetHorzRange(450*m_uiMultiplier);

	CString cTemp;
	if (m_uiMultiplier > 8)
		cTemp.Format("Lock Scrolling Together (%5.0f hours displayed)",m_uiMultiplier/8.0);
	else if (m_uiMultiplier == 8)
		cTemp.Format("Lock Scrolling Together (%5.0f hour displayed)",m_uiMultiplier/8.0);
	else
	{
		float fTemp = (60.0 * m_uiMultiplier)/8.0;
		cTemp.Format("Lock Scrolling Together (%5.1f minutes displayed)",fTemp);
	}
	m_cLocked.SetWindowText(cTemp);
	
	m_pParent->RestartAutoCloseTimer();
*/
	CDialog::OnLButtonDblClk(nFlags, point);
}


BOOL CDataWatchDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
//	m_pParent->m_pDataWatchDlg = NULL;
	((CWatchComDlg*)m_pParent)->GraphIsDestroyed(m_iMyIndex);

	return CDialog::DestroyWindow();
}

void CDataWatchDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
//	m_pParent->m_bDataWatchDlgOpen = false;
	((CWatchComDlg*)m_pParent)->GraphIsRunning(m_iMyIndex);
	CDialog::OnClose();
}

void CDataWatchDlg::OnCheckLocked() 
{
	// TODO: Add your control notification handler code here
	
}

void CDataWatchDlg::OnButtonExpand() 
{
	// TODO: Add your control notification handler code here
	m_uiMultiplier /= 2;
	if (m_uiMultiplier < 1)
		m_uiMultiplier = 1;

	if (m_uiMultiplier == 1) m_cButtonExpand.EnableWindow(FALSE);
	m_cButtonContract.EnableWindow(TRUE);
	if (m_uiMultiplier > 1)
	{
		m_wnd1Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd2Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd3Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd4Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd5Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd6Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd7Chart.SetHorzDiv(15*m_uiMultiplier/2);
		m_wnd8Chart.SetHorzDiv(15*m_uiMultiplier/2);
	}

	m_wnd1Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd2Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd3Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd4Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd5Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd6Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd7Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd8Chart.SetHorzRange(450*float(m_uiMultiplier));

	CString cTemp;
	if (m_uiMultiplier > 8)
		cTemp.Format("Lock Scrolling Together (%5.0f hours displayed)",m_uiMultiplier/8.0);
	else if (m_uiMultiplier == 8)
		cTemp.Format("Lock Scrolling Together (%5.0f hour displayed)",m_uiMultiplier/8.0);
	else
	{
		float fTemp = (60.0 * m_uiMultiplier)/8.0;
		cTemp.Format("Lock Scrolling Together (%5.1f minutes displayed)",fTemp);
	}
	m_cLocked.SetWindowText(cTemp);
}

void CDataWatchDlg::OnButtonContract() 
{
	// TODO: Add your control notification handler code here
	m_uiMultiplier *= 2;
	if (m_uiMultiplier > 256)
		m_uiMultiplier = 256;
	if (m_uiMultiplier == 256) m_cButtonContract.EnableWindow(FALSE);
	m_cButtonExpand.EnableWindow(TRUE);
	m_wnd1Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd2Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd3Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd4Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd5Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd6Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd7Chart.SetHorzDiv(15*m_uiMultiplier/2);
	m_wnd8Chart.SetHorzDiv(15*m_uiMultiplier/2);

	m_wnd1Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd2Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd3Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd4Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd5Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd6Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd7Chart.SetHorzRange(450*float(m_uiMultiplier));
	m_wnd8Chart.SetHorzRange(450*float(m_uiMultiplier));

	CString cTemp;
	if (m_uiMultiplier > 8)
		cTemp.Format("Lock Scrolling Together (%5.0f hours displayed)",m_uiMultiplier/8.0);
	else if (m_uiMultiplier == 8)
		cTemp.Format("Lock Scrolling Together (%5.0f hour displayed)",m_uiMultiplier/8.0);
	else
	{
		float fTemp = (60.0 * m_uiMultiplier)/8.0;
		cTemp.Format("Lock Scrolling Together (%5.1f minutes displayed)",fTemp);
	}

	m_cLocked.SetWindowText(cTemp);
}
