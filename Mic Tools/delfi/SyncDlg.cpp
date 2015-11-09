/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-040. This software was exported from the United States in accordance with the Export
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
// SyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sync.h"
#include "SystemTray.h"
#include "SyncDlg.h"
#include "Configure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char g_szInDir[_MAX_PATH];
COleDateTimeSpan g_dCycleTime;

#define VERSION "1.7.0.0"
#define MYCALLBACK (WM_APP+100)
#define HIDEME	30003
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncDlg dialog

CSyncDlg::CSyncDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSyncDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSyncDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyncDlg)
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_pConfigButton);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSyncDlg, CDialog)
	//{{AFX_MSG_MAP(CSyncDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, OnPause)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, OnButtonConfigure)
	ON_BN_CLICKED(IDC_BUTTON_HIDENOW, OnButtonHidenow)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(MYCALLBACK,MsgFromTray)
	ON_MESSAGE(TIMER_MESSAGE,DoWork)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncDlg message handlers

void CSyncDlg::MsgFromTray(WPARAM WParam, LPARAM LParam)
{
  if (LParam == 513)
  {
	  DoShow(true);
	  BringWindowToTop();
	  SetFocus();
//	  m_SystemTray.HideIcon();
  }
}

BOOL CSyncDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	// TODO: Add extra initialization here
	m_SystemTray.Create(this, MYCALLBACK, NAME, 
		m_hIcon, MYCALLBACK, FALSE);

	char temp[_MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),m_szIniFile,sizeof(m_szIniFile));
	if (m_cCommandLine[0] != NULL)
	{
		char cDrive[_MAX_DRIVE];
		char cPath[_MAX_PATH];
		char cName[_MAX_FNAME];
		char cExt[_MAX_EXT];
		_splitpath(m_szIniFile,cDrive,cPath,cName,cExt);
		for (char *p = m_cCommandLine; p < m_cCommandLine + strlen(m_cCommandLine);p++)
			if (islower(*p)) p[0] = _toupper(*p);
		if (strstr(m_cCommandLine,".INI"))
			_makepath(m_szIniFile,cDrive,cPath,m_cCommandLine,NULL);
		else
			_makepath(m_szIniFile,cDrive,cPath,m_cCommandLine,".INI");
		strcpy(temp,NAME);
		strcat(temp," - ");
		strcat(temp,m_cCommandLine);
		SetWindowText(temp);
	} 
	else
	{
		char *c = strrchr(m_szIniFile,'.');//find the . in the .exe
		*c = '\0';						//make it null
		strcat(m_szIniFile,".INI");
	}
	FILE* inifile;
	m_bNewIni = false;
	if ((inifile = fopen(m_szIniFile,"rt"))==NULL)
	{
		inifile = fopen(m_szIniFile,"wt");
		if (inifile)
		{
			fprintf(inifile,"[CONFIGURATION]\n");
			fprintf(inifile,";how often to check dirs in seconds\n");
			fprintf(inifile,";or CYCLETIME=DAYS:HOURS:MINUTES:SECONDS\n");
			fprintf(inifile,"CYCLETIME=0:0:1:0\n");
			fprintf(inifile,";maximum pause in seconds\n");
			fprintf(inifile,"MAXPAUSE=60\n");
			fprintf(inifile,";show or don't show dialog\n");
			fprintf(inifile,"SHOW=1\n");
			fprintf(inifile,";age in seconds at which to delete\n");
			fprintf(inifile,";or AGE=DAYS:HOURS:MINUTES:SECONDS\n");
			fprintf(inifile,"AGE=90:0:0:0\n");
			fprintf(inifile,"VERSION=%s\n",VERSION);
			fprintf(inifile,"\n");
			fprintf(inifile,"[STATUS]\n");
			fprintf(inifile,"LAST FILE=\n");
			fprintf(inifile,"LAST FAIL=\n");
			fprintf(inifile,"\n");
			fprintf(inifile,"[WATCHES]\n");
			fprintf(inifile,";directories to watch\n");
			fprintf(inifile,";1=C:\\INDIR1\\\n");
			fprintf(inifile,";1USEARCHIVEFLAG=N\n");
			fprintf(inifile,";1DOSUBDIRECTORY=N\n");
			fprintf(inifile,";1DELETE_AT_AGE=180:0:0:0\n\n");
			fprintf(inifile,";2=C:\\INDIR2\\\n");
			fprintf(inifile,";2USEARCHIVEFLAG=N\n");
			fprintf(inifile,";2DOSUBDIRECTORY=N\n");
			fprintf(inifile,";2DELETE_AT_AGE=180:0:0:0\n\n");

			MessageBox("INI file not found.  DelFi created a new one! Configure DelFi before continuing!",
				"DelFi: Error",MB_OK | MB_ICONINFORMATION);
			fclose(inifile);
		}
		else
		{
			MessageBox("INI file not found and cannot be created! Terminating DelFi.",
				"DelFi: Error",MB_OK | MB_ICONSTOP);
			EndDialog(-1);
		}
		m_bNewIni = true;
	}
	else
	{
		fclose(inifile);
	}

	WritePrivateProfileString(CONFIG,"VERSION",VERSION,m_szIniFile);
	WritePrivateProfileString("STATUS","LAST FAIL","",m_szIniFile);

	GetPrivateProfileString(CONFIG,CYCLETIME,"0:0:1:0",temp,sizeof(temp),m_szIniFile);
	if (strchr(temp,':'))
	{
		int Days, Hours, Minutes, Seconds;
		if (sscanf(temp,"%d:%d:%d:%d",&Days,&Hours,&Minutes,&Seconds)!= 4)
		{
			Days = 0; Hours = 0; Minutes = 1; Seconds = 0;
		}
		//correct for COleDateTimeSpan bug
		g_dCycleTime = COleDateTimeSpan(Days,Hours,Minutes,Seconds);
	}
	else
		g_dCycleTime = COleDateTimeSpan(0,0,0,atoi(temp));
	g_dCycleTime += 5.7870370370370370e-6;
	
	sprintf(temp,"%d Days, %2d hrs : %2d min : %2d sec",
		g_dCycleTime.GetDays(),
		g_dCycleTime.GetHours(),
		g_dCycleTime.GetMinutes(),
		g_dCycleTime.GetSeconds());
	SetDlgItemText(IDC_STATIC_CYCLETIME,temp);

	GetPrivateProfileString(CONFIG,AGE,"90:0:0:0",temp,sizeof(temp),m_szIniFile);
	if (strchr(temp,':'))
	{
		int Days, Hours, Minutes, Seconds;
		if (sscanf(temp,"%d:%d:%d:%d",&Days,&Hours,&Minutes,&Seconds)!= 4)
		{
			Days = 90; Hours = 0; Minutes = 0; Seconds = 0;
		}
		m_Age = COleDateTimeSpan(Days,Hours,Minutes,Seconds);
	}
	else
		m_Age = COleDateTimeSpan(0,0,0,atoi(temp));
	m_Age += 5.7870370370370370e-6;
	
	m_dMaxPause = GetPrivateProfileInt(CONFIG,MAXPAUSE,10,m_szIniFile);

	m_bShow = GetPrivateProfileInt(CONFIG,SHOW,1,m_szIniFile)==1;

	m_SystemTray.SetIconList(IDI_ICON1,IDI_ICON18);

	m_bPause = false;
	m_dPauseCnt = 0;

	if (m_bNewIni)
		OnButtonConfigure();
	else
	{
		SetTimer(CYCLETIMER,(UINT)g_dCycleTime.GetTotalSeconds()*1000,NULL);
		//added to force immediate run when program is started
//		PostMessage(TIMER_MESSAGE,0,0);
	}

	SetTimer(HIDEME,3000,NULL);	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSyncDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void CSyncDlg::DoWork(WPARAM WParam, LPARAM LParam)
{
	m_pConfigButton.EnableWindow(false);
	m_SystemTray.Animate(250);

	int DirIndex = 1;
	char INIIndex[8];
	char InDirectory[_MAX_PATH];
	char szTemp1[_MAX_PATH];
	char szTemp2[_MAX_PATH];

	bool bConsiderArchiveBit;
	bool bDoSubDirectories;

	//build first INI entry name
	sprintf(INIIndex,"%d",DirIndex);
	//get the first name from the INI file
	COleDateTime TimeNow = COleDateTime::GetCurrentTime();
	COleDateTimeSpan Age;
	SetDlgItemText(IDC_STATIC_LASTRUN,TimeNow.Format("%c"));

	GetPrivateProfileString("WATCHES",INIIndex,"",InDirectory,sizeof(InDirectory),m_szIniFile);
	//while we have a name do
	while (strlen(InDirectory) > 0)
	{
		//get use archive flag
		sprintf(szTemp1,"%dUSEARCHIVEFLAG",DirIndex);
		GetPrivateProfileString("WATCHES",szTemp1,"",szTemp2,sizeof(szTemp2),m_szIniFile);
		if (strlen(szTemp2)==0)
		{
			WritePrivateProfileString("WATCHES",szTemp1,"N",m_szIniFile);
			strcpy(szTemp2,"N");
		}

		if (toupper(szTemp2[0])=='Y')
			bConsiderArchiveBit = true;
		else
			bConsiderArchiveBit = false;

		sprintf(szTemp1, "%dDOSUBDIRECTORY",DirIndex);
		GetPrivateProfileString("WATCHES",szTemp1,"",szTemp2,sizeof(szTemp2),m_szIniFile);
		if (strlen(szTemp2)==0)
		{
			WritePrivateProfileString("WATCHES",szTemp1,"N",m_szIniFile);
			strcpy(szTemp2,"N");
		}

		if (toupper(szTemp2[0])=='Y')
			bDoSubDirectories = true;
		else
			bDoSubDirectories = false;

		sprintf(szTemp1, "%dDELETE_AT_AGE",DirIndex);
		GetPrivateProfileString("WATCHES",szTemp1,"",szTemp2,sizeof(szTemp2),m_szIniFile);
		if (strlen(szTemp2)==0)
		{
			GetPrivateProfileString("CONFIGURATION","AGE","",szTemp2,sizeof(szTemp2),m_szIniFile);
			if (strlen(szTemp2) == 0)
				strcpy(szTemp2,"180:0:0:0");
			WritePrivateProfileString("WATCHES",szTemp1,szTemp2,m_szIniFile);
		}

		if (strchr(szTemp2,':'))
		{
			int Days, Hours, Minutes, Seconds;
			if (sscanf(szTemp2,"%d:%d:%d:%d",&Days,&Hours,&Minutes,&Seconds)!= 4)
			{
				Days = 180; Hours = 0; Minutes = 0; Seconds = 0;
			}
			Age = COleDateTimeSpan(Days,Hours,Minutes,Seconds);
		}
		else
			Age = COleDateTimeSpan(0,0,0,atoi(szTemp2));
		Age += 5.7870370370370370e-6;
		
		CheckFiles(InDirectory, Age, TimeNow, bConsiderArchiveBit, bDoSubDirectories);

		//	build next directory name
		DirIndex++;
		sprintf(INIIndex,"%d",DirIndex);
		//	get the next dir name from the INI file
		GetPrivateProfileString("WATCHES",INIIndex,"",InDirectory,sizeof(InDirectory),m_szIniFile);
	//end while
	}
	m_pConfigButton.EnableWindow(true);
	m_SystemTray.StopAnimation();
}

void CSyncDlg::DoShow(bool bShow)
{
	if (bShow)
	{
//		m_SystemTray.HideIcon();
		ShowWindow(SW_SHOW);
	}
	else
	{
		ShowWindow(SW_HIDE);
//		m_SystemTray.ShowIcon();
	}
}

void CSyncDlg::CheckFiles(char *InDirectory, COleDateTimeSpan Age, COleDateTime TimeNow, 
						  bool bConsiderArchiveBit, bool bDoSubDirectories)
{
	char InFile[_MAX_PATH];
	char InDrive[_MAX_DRIVE];
	char InPath[_MAX_PATH];
	char InFName[_MAX_FNAME];
	char InExt[_MAX_EXT];
	HANDLE hFileFind;
	WIN32_FIND_DATA FindData;
	//	find firstfile
	if ((InDirectory[strlen(InDirectory)-1] != '\\') &&
		(InDirectory[strlen(InDirectory)-1] != '/'))
		strcat(InDirectory,"\\");
	_splitpath(InDirectory,InDrive,InPath,InFName,InExt);
	_makepath(InFile,InDrive,InPath,"*","*");
	if ((hFileFind = FindFirstFile(InFile,&FindData)) != INVALID_HANDLE_VALUE)
	{
		//if not a directory and file is older than age at seconds then delete it
		BOOL bDone = false;
		while (!bDone)
		{
			if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				//if we must consider archive bit then
				//only delete file if archive bit has been 
				//cleared (e.g. is not on)
				bool bArchiveBitOk = true;
				if (bConsiderArchiveBit)
					if (FindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
						bArchiveBitOk = false;

				COleDateTime FileTime(FindData.ftLastWriteTime);
				COleDateTimeSpan FileAge(TimeNow - FileTime);
				if ((FileAge > Age) && bArchiveBitOk)
				{
					//build file name
					_makepath(InFile,InDrive,InPath,FindData.cFileName,NULL);
					//delete file
//if (strstr(InFile,"\\Program Files\\Microsoft Visual Studio\\MyProjects\\"))
//{
//	if (strstr(InFile,"\\Release\\") || strstr(InFile,"\\Debug\\"))
//	{
//		if (strstr(InFile,".pdb") || 
//			strstr(InFile,".ilk") || 
//			strstr(InFile,".pch") || 
//			strstr(InFile,".obj") || 
//			strstr(InFile,".res") || 
//			strstr(InFile,".idb") )

					if (DeleteFile(InFile)==0)
					{
						SetDlgItemText(IDC_STATIC_ERROR,InFile);
						WritePrivateProfileString("STATUS","LAST FAIL",InFile,m_szIniFile);
					}
					else
					{
						SetDlgItemText(IDC_STATIC_DELETED,InFile);
						WritePrivateProfileString("STATUS","LAST DELETE",InFile,m_szIniFile);
					}
//	}
//}
				}
			}
			else
			{//else is a directory
				if (FindData.cFileName[0] != '.')
				{
					if (bDoSubDirectories)
					{
						char subDir[_MAX_PATH];
						strcpy(subDir,InDirectory);
						strcat(subDir,FindData.cFileName);
						CheckFiles(subDir, Age, TimeNow, bConsiderArchiveBit, bDoSubDirectories);
					}
				}
			}
			bDone = (FindNextFile(hFileFind,&FindData)==0);
		}
		FindClose(hFileFind);	
	}
	else
	{
		WritePrivateProfileString("STATUS","LAST FAIL",InFile,m_szIniFile);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSyncDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSyncDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSyncDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == CYCLETIMER)
	{
		char temp[64];
		if (!m_bPause)
		{
			m_dPauseCnt = 0;
			//trigger the required work 
			PostMessage(TIMER_MESSAGE,0,0);
		}
		else
		{
			m_dPauseCnt++;
			sprintf(temp,"Continue Now or in %d sec.",m_dMaxPause - m_dPauseCnt);
			SetDlgItemText(IDC_BUTTON1,temp);

			if (m_dPauseCnt > m_dMaxPause)
			{
				m_dPauseCnt = 0;
				m_bPause = false;
				SetDlgItemText(IDC_BUTTON1,"Pause");
				KillTimer(CYCLETIMER);
				SetTimer(CYCLETIMER,(UINT)g_dCycleTime.GetTotalSeconds()*1000,NULL);
			}
		}
	}
	if (nIDEvent == HIDEME)
	{
		DoShow(m_bShow);
		KillTimer(HIDEME);
	}
	CDialog::OnTimer(nIDEvent);
}

void CSyncDlg::OnPause() 
{
	// TODO: Add your control notification handler code here
	char temp[40];
	m_dMaxPause = GetPrivateProfileInt(CONFIG,MAXPAUSE,10,m_szIniFile);

	KillTimer(CYCLETIMER);

	m_dPauseCnt = 0;
	m_bPause = !m_bPause;

	if (m_bPause)
	{
		SetTimer(CYCLETIMER,1000,NULL);
		sprintf(temp,"Continue Now or in %d sec.",m_dMaxPause - m_dPauseCnt);
	}
	else
	{
		SetTimer(CYCLETIMER,(UINT)g_dCycleTime.GetTotalSeconds()*1000,NULL);
		strcpy(temp,"Pause");
	}

	SetDlgItemText(IDC_BUTTON1,temp);
}


void CSyncDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	char temp[40];
	KillTimer(CYCLETIMER);
	m_dPauseCnt = 0;
	if (m_bPause)
	{
		SetTimer(CYCLETIMER,1000,NULL);
		sprintf(temp,"Continue Now or in %d sec.",m_dMaxPause - m_dPauseCnt);
	}
	else
	{
		SetTimer(CYCLETIMER,(UINT)g_dCycleTime.GetTotalSeconds()*1000,NULL);
		strcpy(temp,"Pause");
	}
	SetDlgItemText(IDC_BUTTON1,temp);
	PostMessage(TIMER_MESSAGE,0,0);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	char temp[64];
	strcpy(temp,NAME);
	strcat(temp," Version: ");
	strcat(temp,VERSION);
	SetDlgItemText(IDC_STATIC_NAME,temp);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSyncDlg::OnButtonConfigure() 
{
	// TODO: Add your control notification handler code here
	KillTimer(CYCLETIMER);
	KillTimer(HIDEME);
	CConfigure Config;
	//setup config
	Config.m_IniFile = m_szIniFile;
	if (Config.DoModal()==IDOK)
	{
		char temp[32];
		GetPrivateProfileString(CONFIG,CYCLETIME,"0:0:1:0",temp,sizeof(temp),m_szIniFile);
		if (strchr(temp,':'))
		{
			int Days, Hours, Minutes, Seconds;
			if (sscanf(temp,"%d:%d:%d:%d",&Days,&Hours,&Minutes,&Seconds)!= 4)
			{
				Days = 0; Hours = 0; Minutes = 1; Seconds = 0;
			}
			//correct for COleDateTimeSpan bug
			g_dCycleTime = COleDateTimeSpan(Days,Hours,Minutes,Seconds);
		}
		else
			g_dCycleTime = COleDateTimeSpan(0,0,0,atoi(temp));
		//add a half second to the 
		g_dCycleTime += 5.7870370370370370e-6;
		
		sprintf(temp,"%d Days, %2d hrs : %2d min : %2d sec",
			g_dCycleTime.GetDays(),
			g_dCycleTime.GetHours(),
			g_dCycleTime.GetMinutes(),
			g_dCycleTime.GetSeconds());
		SetDlgItemText(IDC_STATIC_CYCLETIME,temp);
		m_bShow = (GetPrivateProfileInt(CONFIG,SHOW,1,m_szIniFile)==1);
		DoShow(m_bShow);
	}
	if (m_bPause)
		SetTimer(CYCLETIMER,1000,NULL);
	else
		SetTimer(CYCLETIMER,(UINT)g_dCycleTime.GetTotalSeconds()*1000,NULL);
}


void CSyncDlg::OnButtonHidenow() 
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_HIDE);
}

void CSyncDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_SystemTray.HideIcon();
	CDialog::OnClose();
}
