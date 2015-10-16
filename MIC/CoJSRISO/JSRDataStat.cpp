// JSRDataStat.cpp : implementation file
//

#include "stdafx.h"
#include "colors.h"
#include "JSRDataStat.h"
#include "JSRInst.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJSRDataStat property page

IMPLEMENT_DYNCREATE(CJSRDataStat, CPropertyPage)

CJSRDataStat::CJSRDataStat() : CPropertyPage(CJSRDataStat::IDD)
{
}

CJSRDataStat::CJSRDataStat(UINT id, CISO *parent) : CPropertyPage(id)
{
	for (int i= IDC_JSRDATA_FIRST; i <= IDC_JSRDATA_LAST; i++)
	{
		StaticColors[i-IDC_JSRDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	m_pParent = parent;
}


void
CJSRDataStat::SetText(int index,CHAR * text)
{
	if ((index >= IDC_JSRDATA_FIRST) &&
		(index <= IDC_JSRDATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *
CJSRDataStat::GetText(int index, char * buff, int length)
{
	buff[0] = NULL;
	if ((index >= IDC_JSRDATA_FIRST) &&
		(index <= IDC_JSRDATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}
void
CJSRDataStat::SetPropertySheet(CJSRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CJSRDataStat::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_JSRDATA_FIRST) &&
		(index <= IDC_JSRDATA_LAST)) 
	{
	  StaticColors[index-IDC_JSRDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CJSRDataStat::ResetColors()
{
	for (int i= IDC_JSRDATA_FIRST; i <= IDC_JSRDATA_LAST; i++)
	{
		StaticColors[i-IDC_JSRDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CJSRDataStat::~CJSRDataStat()
{
}

void CJSRDataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJSRDataStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CJSRDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CJSRDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_WATCH, OnButtonWatch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJSRDataStat message handlers
HBRUSH CJSRDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_JSRDATA_FIRST) && (index <= IDC_JSRDATA_LAST))
	{
		int iFirstOfList = IDC_JSRDATA_FIRST; 
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

void CJSRDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

BOOL CJSRDataStat::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_RADIO_RAW))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_RATES))->SetCheck(1);
	
	return TRUE;
}

void CJSRDataStat::OnButtonWatch() 
{
	// TODO: Add your control notification handler code here
	char szTemp[_MAX_PATH];
	strcpy(szTemp,m_pPropertySheet->m_pParent->m_pName);
	strcat(szTemp," Watcher");
	CWnd * pcWnd = FindWindow(NULL,szTemp);
	if (pcWnd == NULL)
	{
		char szNewPathName[_MAX_PATH];
		char szDrive[_MAX_DRIVE];
		char szDir[_MAX_DIR];
		char szFName[_MAX_FNAME];
		char szExt[_MAX_EXT];
		//m_pPropertySheet->m_pParent->m_szIniFile
		_splitpath(m_pPropertySheet->m_pParent->m_szIniFile,szDrive,szDir,szFName,szExt);
		_makepath(szNewPathName,szDrive,szDir,"JSRWatch",".exe");
		if (_spawnl(_P_NOWAIT,szNewPathName,"JSRWatch",m_pPropertySheet->m_pParent->m_pName,NULL) == -1)
			SetDlgItemText(IDC_BUTTON_WATCH,"Watch Data Graphically (exe missing)");
		else
			SetDlgItemText(IDC_BUTTON_WATCH,"Watch Data Graphically");
	}
	else
	{
		pcWnd->ShowWindow(SW_RESTORE);
		pcWnd->SetWindowPos(&wndTop,0,0,0,0,SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOMOVE);
	}
	
}
