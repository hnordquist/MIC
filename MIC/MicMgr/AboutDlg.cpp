//aboutdlg.cpp

#include "stdafx.h"
#include "AboutDlg.h"
#include "globals.h"
#include "mic_definitions.h"
#include "version.h"
#include "newcomm.h"
#include "newinst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetTimer(1,g_iDlgCloseMillisec,NULL);

/*
	CVersionInfo VersionInfo;
	CString Name    = VersionInfo.InternalName();
	CString Version = VersionInfo.FileVersion();
	CString PrivateBuild = VersionInfo.PrivateBuild();

	Version.Remove(' ');
	Version.Replace(',','.');

	//Version.Format("%c.%c.%c.%c",Version[0],Version[3],Version[6],Version[9]);
	//CString cs = Name + " " + Version + " - " + PrivateBuild +" - " + __DATE__;
	CString cs = CString(Name) +" " + Version + " "  + __DATE__;
	SetDlgItemText(IDC_STATIC_VERSION, cs);
*/
	SetDlgItemText(IDC_STATIC_VERSION, g_csNameVersionDate);  //SCR 118

#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG, "");
#endif

	CNewComm cNewComm(this);									//SCR000118
	CNewInst cNewInst(this);									//SCR000118
	CString cs;													//SCR000118
	CString csOut("REGISTERED MIC COMPONENTS\r\n\r\nCSOs (communications):\r\n");//SCR000118
	bool bDoGUI = false;										//SCR000118

	bool bOK = cNewComm.DoDynamicDiscovery(bDoGUI, &cs);		//SCR000118
	if (bOK)													//SCR000118
		csOut += cs;											//SCR000118

	csOut += "\r\nISOs (instruments):\r\n";									//SCR000118

	cs.Empty();													//SCR000118
	bOK = cNewInst.DoDynamicDiscovery(bDoGUI, &cs);				//SCR000118
	if (bOK)													//SCR000118
		csOut += cs;											//SCR000118

	csOut += "\r\nOTHERS:\r\n";

	csOut += g_csColorButtonVersionString;						//SCR000118

	SetDlgItemText(IDC_EDIT1,csOut);							//SCR000118

	return TRUE;
}

void CAboutDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnOK();
	}
	CDialog::OnTimer(nIDEvent);
}