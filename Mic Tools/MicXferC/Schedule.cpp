// Schedule.cpp : implementation file
//

#include "stdafx.h"
#include "tree.h"
#include "Schedule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSchedule dialog


CSchedule::CSchedule(CWnd* pParent /*=NULL*/)
	: CDialog(CSchedule::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSchedule)
	m_cWeekDay = _T("");
	m_cSchedule = _T("");
	m_iDay = 0;
	m_iHours = 0;
	m_iMinutes = 0;
	m_iMonth = 0;
	m_iSeconds = 0;
	m_iYear = 0;
	//}}AFX_DATA_INIT
	m_dSched = 0;
	m_dWeekd = 0;
	m_dYear  = 0;
	m_dMonth = 0;
	m_dDay   = 0;
	m_dHours = 0;
	m_dMinutes = 0;
	m_dSeconds = 0;

}


void CSchedule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchedule)
	DDX_Control(pDX, IDC_EDIT_YEAR, m_pEditYear);
	DDX_Control(pDX, IDC_EDIT_SECONDS, m_pEditSeconds);
	DDX_Control(pDX, IDC_EDIT_MONTH, m_pEditMonth);
	DDX_Control(pDX, IDC_EDIT_MINUTES, m_pEditMinutes);
	DDX_Control(pDX, IDC_EDIT_HOURS, m_pEditHours);
	DDX_Control(pDX, IDC_EDIT_DAY, m_pEditDay);
	DDX_Control(pDX, IDC_LIST_SCHEDULE, m_pSchedule);
	DDX_Control(pDX, IDC_LIST_WEEKDAYS, m_pWeekDay);
	DDX_Control(pDX, IDC_SPIN_YEAR, m_pSpinYear);
	DDX_Control(pDX, IDC_SPIN_SECONDS, m_pSpinSeconds);
	DDX_Control(pDX, IDC_SPIN_MONTH, m_pSpinMonth);
	DDX_Control(pDX, IDC_SPIN_MINUTES, m_pSpinMinutes);
	DDX_Control(pDX, IDC_SPIN_HOURS, m_pSpinHours);
	DDX_Control(pDX, IDC_SPIN_DAY, m_pSpinDay);
	DDX_LBString(pDX, IDC_LIST_WEEKDAYS, m_cWeekDay);
	DDX_LBString(pDX, IDC_LIST_SCHEDULE, m_cSchedule);
	DDX_Text(pDX, IDC_EDIT_DAY, m_iDay);
	DDV_MinMaxInt(pDX, m_iDay, 1, 31);
	DDX_Text(pDX, IDC_EDIT_HOURS, m_iHours);
	DDV_MinMaxInt(pDX, m_iHours, 0, 23);
	DDX_Text(pDX, IDC_EDIT_MINUTES, m_iMinutes);
	DDV_MinMaxInt(pDX, m_iMinutes, 0, 59);
	DDX_Text(pDX, IDC_EDIT_MONTH, m_iMonth);
	DDV_MinMaxInt(pDX, m_iMonth, 1, 12);
	DDX_Text(pDX, IDC_EDIT_SECONDS, m_iSeconds);
	DDV_MinMaxInt(pDX, m_iSeconds, 0, 59);
	DDX_Text(pDX, IDC_EDIT_YEAR, m_iYear);
	DDV_MinMaxInt(pDX, m_iYear, 1999, 3000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSchedule, CDialog)
	//{{AFX_MSG_MAP(CSchedule)
	ON_LBN_SELCHANGE(IDC_LIST_SCHEDULE, OnSelchangeListSchedule)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchedule message handlers

BOOL CSchedule::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pSchedule.AddString("NEVER");
	m_pSchedule.AddString("PROGRAM");
	m_pSchedule.AddString("MONTHLY");
	m_pSchedule.AddString("WEEKLY");
	m_pSchedule.AddString("DAILY");
	m_pSchedule.AddString("HOURLY");
	m_pSchedule.AddString("MINUTE");
	m_pSchedule.AddString("SECOND");

	m_pWeekDay.AddString("not set");
	m_pWeekDay.AddString("Sunday");
	m_pWeekDay.AddString("Monday");
	m_pWeekDay.AddString("Tuesday");
	m_pWeekDay.AddString("Wednesday");
	m_pWeekDay.AddString("Thursday");
	m_pWeekDay.AddString("Friday");
	m_pWeekDay.AddString("Saturday");

	m_pSchedule.SetCurSel(m_dSched);
	m_pWeekDay.SetCurSel(m_dWeekd);

	m_pSpinYear.SetRange(1999,3000);
	m_pSpinMonth.SetRange(1,12);
	m_pSpinDay.SetRange(1,31);
	m_pSpinHours.SetRange(0,23);
	m_pSpinMinutes.SetRange(0,59);
	m_pSpinSeconds.SetRange(0,59);

	m_pSpinYear.SetPos(m_dYear);
	m_pSpinMonth.SetPos(m_dMonth);
	m_pSpinDay.SetPos(m_dDay);
	m_pSpinHours.SetPos(m_dHours);
	m_pSpinMinutes.SetPos(m_dMinutes);
	m_pSpinSeconds.SetPos(m_dSeconds);

	SetDlgItemInt(IDC_EDIT_SECONDS,m_dSeconds);
	SetDlgItemInt(IDC_EDIT_MINUTES,m_dMinutes);
	SetDlgItemInt(IDC_EDIT_HOURS,m_dHours);
	SetDlgItemInt(IDC_EDIT_DAY,m_dDay);
	SetDlgItemInt(IDC_EDIT_MONTH,m_dMonth);
	SetDlgItemInt(IDC_EDIT_YEAR,m_dYear);

	SetTimer(1000,100,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CSchedule::Setup(int sched,int weekday,int year,int month,int day,int hours,int minutes,int seconds)
{
	m_dSched = sched;
	m_dWeekd = weekday;
	m_dYear  = year;
	m_dMonth = month;
	m_dDay   = day;
	m_dHours = hours;
	m_dMinutes = minutes;
	m_dSeconds = seconds;
}

void CSchedule::OnSelchangeListSchedule() 
{
	// TODO: Add your control notification handler code here
	switch(m_pSchedule.GetCurSel())
	{
		case 0://never
			m_pSpinYear.EnableWindow(false);
			m_pEditYear.EnableWindow(false);
			m_pSpinMonth.EnableWindow(false);
			m_pEditMonth.EnableWindow(false);
			m_pSpinDay.EnableWindow(false);
			m_pEditDay.EnableWindow(false);
			m_pSpinHours.EnableWindow(false);
			m_pEditHours.EnableWindow(false);
			m_pSpinMinutes.EnableWindow(false);
			m_pEditMinutes.EnableWindow(false);
			m_pSpinSeconds.EnableWindow(false);
			m_pEditSeconds.EnableWindow(false);
			m_pWeekDay.EnableWindow(false);
			SetDlgItemText(IDC_STATIC_TEXT,
				"Never run.");
			break;

		case 1://program
			m_pSpinYear.EnableWindow(true);
			m_pEditYear.EnableWindow(true);
			m_pSpinMonth.EnableWindow(true);
			m_pEditMonth.EnableWindow(true);
			m_pSpinDay.EnableWindow(true);
			m_pEditDay.EnableWindow(true);
			m_pSpinHours.EnableWindow(true);
			m_pEditHours.EnableWindow(true);
			m_pSpinMinutes.EnableWindow(true);
			m_pEditMinutes.EnableWindow(true);
			m_pSpinSeconds.EnableWindow(true);
			m_pEditSeconds.EnableWindow(true);
			m_pWeekDay.EnableWindow(false);
			SetDlgItemText(IDC_STATIC_TEXT,
				"Run once at programed date and time.");
			break;

		case 2://monthly
			m_pSpinYear.EnableWindow(false);
			m_pEditYear.EnableWindow(false);
			m_pSpinMonth.EnableWindow(false);
			m_pEditMonth.EnableWindow(false);
			m_pSpinDay.EnableWindow(true);
			m_pEditDay.EnableWindow(true);
			m_pSpinHours.EnableWindow(true);
			m_pEditHours.EnableWindow(true);
			m_pSpinMinutes.EnableWindow(true);
			m_pEditMinutes.EnableWindow(true);
			m_pSpinSeconds.EnableWindow(true);
			m_pEditSeconds.EnableWindow(true);
			m_pWeekDay.EnableWindow(false);
			SetDlgItemText(IDC_STATIC_TEXT,
				"Repeat at DAY of month at H:M:S.");
			break;

		case 3://weekly
			m_pSpinYear.EnableWindow(false);
			m_pEditYear.EnableWindow(false);
			m_pSpinMonth.EnableWindow(false);
			m_pEditMonth.EnableWindow(false);
			m_pSpinDay.EnableWindow(false);
			m_pEditDay.EnableWindow(false);
			m_pSpinHours.EnableWindow(true);
			m_pEditHours.EnableWindow(true);
			m_pSpinMinutes.EnableWindow(true);
			m_pEditMinutes.EnableWindow(true);
			m_pSpinSeconds.EnableWindow(true);
			m_pEditSeconds.EnableWindow(true);
			m_pWeekDay.EnableWindow(true);
			SetDlgItemText(IDC_STATIC_TEXT,
				"Repeat on WEEKDAY at time H:M:S.");
			break;

		case 4://daily
			m_pSpinYear.EnableWindow(false);
			m_pEditYear.EnableWindow(false);
			m_pSpinMonth.EnableWindow(false);
			m_pEditMonth.EnableWindow(false);
			m_pSpinDay.EnableWindow(false);
			m_pEditDay.EnableWindow(false);
			m_pSpinHours.EnableWindow(true);
			m_pEditHours.EnableWindow(true);
			m_pSpinMinutes.EnableWindow(true);
			m_pEditMinutes.EnableWindow(true);
			m_pSpinSeconds.EnableWindow(true);
			m_pEditSeconds.EnableWindow(true);
			m_pWeekDay.EnableWindow(false);
			SetDlgItemText(IDC_STATIC_TEXT,
				"Repeat at H:M:S of each day.");
			break;

		case 5://hourly
			m_pSpinYear.EnableWindow(false);
			m_pEditYear.EnableWindow(false);
			m_pSpinMonth.EnableWindow(false);
			m_pEditMonth.EnableWindow(false);
			m_pSpinDay.EnableWindow(false);
			m_pEditDay.EnableWindow(false);
			m_pSpinHours.EnableWindow(false);
			m_pEditHours.EnableWindow(false);
			m_pSpinMinutes.EnableWindow(true);
			m_pEditMinutes.EnableWindow(true);
			m_pSpinSeconds.EnableWindow(true);
			m_pEditSeconds.EnableWindow(true);
			m_pWeekDay.EnableWindow(false);
			SetDlgItemText(IDC_STATIC_TEXT,
				"Repeat at M:S after each hour.");
			break;

		case 6://minute
			m_pSpinYear.EnableWindow(false);
			m_pEditYear.EnableWindow(false);
			m_pSpinMonth.EnableWindow(false);
			m_pEditMonth.EnableWindow(false);
			m_pSpinDay.EnableWindow(false);
			m_pEditDay.EnableWindow(false);
			m_pSpinHours.EnableWindow(false);
			m_pEditHours.EnableWindow(false);
			m_pSpinMinutes.EnableWindow(true);
			m_pEditMinutes.EnableWindow(true);
			m_pSpinSeconds.EnableWindow(true);
			m_pEditSeconds.EnableWindow(true);
			m_pWeekDay.EnableWindow(false);
			SetDlgItemText(IDC_STATIC_TEXT,
				"Repeat every M:S.");
			break;

		case 7://second
			m_pSpinYear.EnableWindow(false);
			m_pEditYear.EnableWindow(false);
			m_pSpinMonth.EnableWindow(false);
			m_pEditMonth.EnableWindow(false);
			m_pSpinDay.EnableWindow(false);
			m_pEditDay.EnableWindow(false);
			m_pSpinHours.EnableWindow(false);
			m_pEditHours.EnableWindow(false);
			m_pSpinMinutes.EnableWindow(false);
			m_pEditMinutes.EnableWindow(false);
			m_pSpinSeconds.EnableWindow(true);
			m_pEditSeconds.EnableWindow(true);
			m_pWeekDay.EnableWindow(false);
			SetDlgItemText(IDC_STATIC_TEXT,
				"Repeat every S seconds.");
			break;
	}
}

void CSchedule::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1000)
	{
		KillTimer(1000);
		OnSelchangeListSchedule();
	}
	CDialog::OnTimer(nIDEvent);
}
