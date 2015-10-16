// Config.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "globals.h"
#include "ports.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfig dialog

CComPortList::CComPortList(CWnd* pParent, CString ** p)
	: CDialog(CComPortList::IDD, pParent)
{
	theListOfPorts = p;
}
CComPortList::~CComPortList()
{

}
void CComPortList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfig)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PORTLIST, meinpops);
}

BEGIN_MESSAGE_MAP(CComPortList, CDialog)
	//{{AFX_MSG_MAP(CConfig)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfig message handlers

void CComPortList::OnOK() 
{
	CDialog::OnOK();
}


BOOL CComPortList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//clock to update the total bbm
	SetTimer(2,1000,NULL);

	//clock to auto close the dialog
	SetTimer(1,g_iDlgCloseMillisec,NULL);
	if (theListOfPorts != NULL)
	{
		for (int myi = 0; myi < MAX_COMM; myi++)
		{
			if (theListOfPorts[myi])
			{
				LVITEM lvI;
				::ZeroMemory(&lvI, sizeof(LVITEM));
				lvI.pszText   = theListOfPorts[myi]->GetBuffer(); 
				lvI.mask      = LVIF_TEXT;
				meinpops.InsertItem(&lvI);
			}
		}
		//meinpops.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//catch timer messages
void CComPortList::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 2)
	{
		ULONG ulong;
		int iTotal = 0;
		for (int i = 0; i < g_iInstCount; i++)
		{
			g_pIISO[i]->get_CurrentBBM(&ulong);
			iTotal += (int)ulong;
		}
		SetDlgItemInt(IDC_STATIC_BBMTOTAL,iTotal);
	}

	//if this is the timeout event, then cancel this dialog
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}

