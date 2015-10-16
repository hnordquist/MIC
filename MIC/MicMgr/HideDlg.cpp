//
// HideDlg.cpp : implementation file
//
#include "stdafx.h"
#include "globals.h"
#include "HideDlg.h"
#include "DoFileCopy.h"
#include "ISO_definitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHideDlg dialog


CHideDlg::CHideDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHideDlg::IDD, pParent)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CHideDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHideDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHideDlg, CDialog)
	//{{AFX_MSG_MAP(CHideDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_HIDDEN_EXTEND, OnHiddenExtend)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHideDlg message handlers

void CHideDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		if (((CDoFileCopy*)m_pParent)->m_dNewMediaState == 2) 
			OnOK();

		m_dToClose++;

		char temp[32];
		int minutes = (g_iDlgCloseMillisec/1000-m_dToClose)/60;
		int seconds   = (g_iDlgCloseMillisec/1000-m_dToClose) % 60;
		sprintf(temp,"Ext. Auto Close: %d:%02d",minutes,seconds);

		SetDlgItemText(ID_HIDDEN_EXTEND,temp);
		if ((minutes == 0)&&(seconds==0)) OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CHideDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//	ModifyStyle(WS_VISIBLE,NULL,NULL);
//	ShowWindow(SW_HIDE);
//	MoveWindow(0,0,1,1,false);

	m_dToClose = 0;

	char temp[32];
	int minutes = (g_iDlgCloseMillisec/1000-m_dToClose)/60;
	int seconds   = (g_iDlgCloseMillisec/1000-m_dToClose) % 60;
	sprintf(temp,"Ext. Auto Close: %d:%02d",minutes,seconds);

	SetDlgItemText(ID_HIDDEN_EXTEND,temp);
	SetTimer(1,500,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHideDlg::OnHiddenExtend() 
{
	m_dToClose -= (BUMP_EXTEND*2);

	if ((g_iDlgCloseMillisec/1000 - m_dToClose/2) > MAX_EXTEND) 
		m_dToClose = g_iDlgCloseMillisec/500-(MAX_EXTEND * 2);

	char temp[32];
	int minutes = (g_iDlgCloseMillisec/1000-m_dToClose)/60;
	int seconds   = (g_iDlgCloseMillisec/1000-m_dToClose) % 60;
	sprintf(temp,"Ext. Auto Close: %d:%02d",minutes,seconds);

	SetDlgItemText(ID_HIDDEN_EXTEND,temp);
}

void CHideDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_dToClose = 0;
	CDialog::OnLButtonDown(nFlags, point);
}
