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
// treeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tree.h"
#include "treeDlg.h"
#include "FileSpec.h"
#include "Schedule.h"
#include "DirDialog.h"
#include "IPAddressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NAME	"MicXfer"
#define VERSION "1.8.0.5"
enum Types {OTHER,DOZIP,IGNOREARCH,DOSUB,FROM,TO,GROUP,CONTROL,
			WHEN,FILESPEC,DOLOG,MAXLOG,LOGPATHNAME,VERBOSE,VERBOSEUDP,VALIDATEPATH,CYCLE,ZIPPATH,NEXT,PREVIOUS,
			USERNAME,DOMAINNAME,PORT,DOUDP,DOUDPSELF,STARTPAUSE,DOUDPADDRESSES,XFERVERSION};
#define NOT_SET	"Not Set"

#define INVALIDATETIMER 10001
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
// CTreeDlg dialog

CTreeDlg::CTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreeDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIconCheck = AfxGetApp()->LoadIcon(IDI_ICON3);
	m_hIconQuest = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_hIconWait  = AfxGetApp()->LoadIcon(IDI_ICON2);
	m_bInitialized = false;
}

CTreeDlg::~CTreeDlg()
{
//	WritePrivateProfileString("CONFIGURATION","HWND",NULL,m_szIniFile);
}

void CTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeDlg)
	DDX_Control(pDX, IDC_BUTTON_ADD, m_ButtonAdd);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_ButtonDelete);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_StaticImage);
	DDX_Control(pDX, IDC_BUTTON1, m_ButtonEdit);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTreeDlg, CDialog)
	//{{AFX_MSG_MAP(CTreeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnDblclkTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnClickTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON2, OnButtonReload)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeDlg message handlers

BOOL CTreeDlg::OnInitDialog()
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

	char temp[_MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),m_szIniFile,sizeof(m_szIniFile));

	char cDrive[_MAX_DRIVE];
	char cPath[_MAX_PATH];
	char cName[_MAX_FNAME];
	char cExt[_MAX_EXT];
	_splitpath(m_szIniFile,cDrive,cPath,cName,cExt);
	if (m_cCommandLine[0] != NULL)
	{
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
		if (cName[strlen(cName)-1]=='2')
			_makepath(m_szIniFile,cDrive,cPath,"MicXfer2",".ini");
		else
			_makepath(m_szIniFile,cDrive,cPath,"MicXfer",".ini");
	}
	CString cTemp("Using: ");
	cTemp += m_szIniFile;
	SetDlgItemText(IDC_STATIC_INI,cTemp);
	FILE* inifile;
	if ((inifile = fopen(m_szIniFile,"rt"))==NULL)
	{
		inifile = fopen(m_szIniFile,"wt");
		if (inifile)
		{
			fprintf(inifile,"[CONFIGURATION]\n");
			fprintf(inifile,";how often to check dirs in seconds\n");
			fprintf(inifile,"CYCLE=30\n");
			fprintf(inifile,";path to validation program\n");
			fprintf(inifile,"VALIDATEPATH=C:\\MIC\\GemAuth.exe\n");
			fprintf(inifile,"ZIPPATH=C:\\mic\\pkzip25.exe\n");
			fprintf(inifile,"USERNAME=\n");
			fprintf(inifile,"DOMAINNAME=\n");
			fprintf(inifile,"VERSION=\n");
			fprintf(inifile,"INITIALPAUSE_SEC=60\n");
			fprintf(inifile,";make entries in log file\n");
			fprintf(inifile,"DOLOG=Yes\n");
			fprintf(inifile,"LOGPATHNAME=C:\\mic\\MICXferLog.txt\n");
			fprintf(inifile,"MAXLOG=0\n");
			fprintf(inifile,"VERBOSE=Yes\n");
			//fprintf(inifile,";comport to activate link - NOT YET IMPLEMENTED\n");
			//fprintf(inifile,";COMPORT=COM2\n");
			fprintf(inifile,"DO_UDP=Yes\n");
			fprintf(inifile,"PORT=5250\n");
			fprintf(inifile,"VERBOSEUDP=Yes\n");
			fprintf(inifile,"1=192.168.1.1\n");
			fprintf(inifile,"2=192.168.1.2\n");
			fprintf(inifile,"\n");

			fprintf(inifile,";CONTROL=<NEVER,MINUTE,HOURLY,DAILY,WEEKLY,MONTHLY,PROGRAM>\n");
			fprintf(inifile,";NEVER   reads nothing\n");
			fprintf(inifile,";MINUTE  reads only TIMEMINUTE and repeats that often\n");
			fprintf(inifile,";HOURLY  reads only TIMEMINUTE and repeats on MINUTES after hour\n");
			fprintf(inifile,";DAILY   reads only TIME and repeats on TIME each day\n");
			fprintf(inifile,";WEEKLY  reads DAYOFWEEK and TIME and repeats each occurance\n");
			fprintf(inifile,";MONTHLY reads DATEDAY and TIME and repeats each occurance\n");
			fprintf(inifile,";SOURCEDIRn directory to get files from\n");
			fprintf(inifile,";DESTINDIRn directory to send files to\n");
			fprintf(inifile,";DESTINDIRna\n");
			fprintf(inifile,";DESTINDIRnb\n");
			fprintf(inifile,"\n");

			fprintf(inifile,"[CALENDAR]\n");
			fprintf(inifile,"CONTROL=NEVER\n");
			fprintf(inifile,"DATEYEAR=2000\n");
			fprintf(inifile,"DATEMONTH=6\n");
			fprintf(inifile,"DATEDAY=1\n");
			fprintf(inifile,"TIMEHOURS=6\n");
			fprintf(inifile,"TIMEMINUTES=30\n");
			fprintf(inifile,"TIMESECONDS=0\n");
			fprintf(inifile,"DAYOFWEEK=0\n");
			fprintf(inifile,"DOSUB=N\n");
			fprintf(inifile,"DOZIP=NI\n");
			fprintf(inifile,"FILESPEC=CALENDAR.CAL\n");
			fprintf(inifile,"PREVIOUS=\n");
			fprintf(inifile,"NEXT=\n");
			fprintf(inifile,"SOURCEDIR1=c:\\newfile\n");
			fprintf(inifile,"DESTINDIR1=c:\\mic\n");
			fprintf(inifile,"\n");

			fprintf(inifile,"[ACQUIRE]\n");
			fprintf(inifile,"CONTROL=NEVER\n");
			fprintf(inifile,"DATEYEAR=2000\n");
			fprintf(inifile,"DATEMONTH=6\n");
			fprintf(inifile,"DATEDAY=1\n");
			fprintf(inifile,"TIMEHOURS=6\n");
			fprintf(inifile,"TIMEMINUTES=30\n");
			fprintf(inifile,"TIMESECONDS=0\n");
			fprintf(inifile,"DAYOFWEEK=0\n");
			fprintf(inifile,"DOSUB=N\n");
			fprintf(inifile,"DOZIP=NI\n");
			fprintf(inifile,"FILESPEC=*.BIN,*.JSR,*.ISR,*.MCA,*.BNY,*.ESS\n");
			fprintf(inifile,"PREVIOUS=\n");
			fprintf(inifile,"NEXT=\n");
			fprintf(inifile,"SOURCEDIR1=c:\\dataoriginal\\RadData\n");
			fprintf(inifile,"DESTINDIR1=c:\\datarealtime\\RadData\n");

			fprintf(inifile,"[PFM]\n");
			fprintf(inifile,"CONTROL=NEVER\n");
			fprintf(inifile,"DATEYEAR=2000\n");
			fprintf(inifile,"DATEMONTH=6\n");
			fprintf(inifile,"DATEDAY=1\n");
			fprintf(inifile,"TIMEHOURS=6\n");
			fprintf(inifile,"TIMEMINUTES=30\n");
			fprintf(inifile,"TIMESECONDS=0\n");
			fprintf(inifile,"DAYOFWEEK=0\n");
			fprintf(inifile,"DOSUB=N\n");
			fprintf(inifile,"DOZIP=NI\n");
			fprintf(inifile,"FILESPEC=*.PFM,*.SOH\n");
			fprintf(inifile,"PREVIOUS=\n");
			fprintf(inifile,"NEXT=\n");
			fprintf(inifile,"SOURCEDIR1=c:\\dataoriginal\\RadData\n");
			fprintf(inifile,"DESTINDIR1=c:\\datarealtime\\RadData\n");

			fprintf(inifile,"[CEV]\n");
			fprintf(inifile,"CONTROL=NEVER\n");
			fprintf(inifile,"DATEYEAR=2000\n");
			fprintf(inifile,"DATEMONTH=6\n");
			fprintf(inifile,"DATEDAY=1\n");
			fprintf(inifile,"TIMEHOURS=6\n");
			fprintf(inifile,"TIMEMINUTES=30\n");
			fprintf(inifile,"TIMESECONDS=0\n");
			fprintf(inifile,"DAYOFWEEK=0\n");
			fprintf(inifile,"DOSUB=N\n");
			fprintf(inifile,"DOZIP=NI\n");
			fprintf(inifile,"FILESPEC=*.CEV\n");
			fprintf(inifile,"PREVIOUS=\n");
			fprintf(inifile,"NEXT=\n");
			fprintf(inifile,"SOURCEDIR1=c:\\dataoriginal\\RadData\n");
			fprintf(inifile,"DESTINDIR1=c:\\datarealtime\\RadData\n");

			fprintf(inifile,"[JPG]\n");
			fprintf(inifile,"CONTROL=NEVER\n");
			fprintf(inifile,"DATEYEAR=2000\n");
			fprintf(inifile,"DATEMONTH=6\n");
			fprintf(inifile,"DATEDAY=1\n");
			fprintf(inifile,"TIMEHOURS=6\n");
			fprintf(inifile,"TIMEMINUTES=30\n");
			fprintf(inifile,"TIMESECONDS=0\n");
			fprintf(inifile,"DAYOFWEEK=0\n");
			fprintf(inifile,"DOSUB=Y\n");
			fprintf(inifile,"DOZIP=YI\n");
			fprintf(inifile,"FILESPEC=*.JPG\n");
			fprintf(inifile,"PREVIOUS=\n");
			fprintf(inifile,"NEXT=\n");
			fprintf(inifile,"SOURCEDIR1=c:\\dataoriginal\\images\n");
			fprintf(inifile,"DESTINDIR1=c:\\datarealtime\\images\n");

			fprintf(inifile,"[OTHER]\n");
			fprintf(inifile,"CONTROL=NEVER\n");
			fprintf(inifile,"DATEYEAR=2000\n");
			fprintf(inifile,"DATEMONTH=6\n");
			fprintf(inifile,"DATEDAY=1\n");
			fprintf(inifile,"TIMEHOURS=6\n");
			fprintf(inifile,"TIMEMINUTES=30\n");
			fprintf(inifile,"TIMESECONDS=0\n");
			fprintf(inifile,"DAYOFWEEK=0\n");
			fprintf(inifile,"DOSUB=N\n");
			fprintf(inifile,"DOZIP=NI\n");
			fprintf(inifile,"FILESPEC=*.*\n");
			fprintf(inifile,"PREVIOUS=\n");
			fprintf(inifile,"NEXT=\n");
			fprintf(inifile,"SOURCEDIR1=c:\\dataoriginal\\RadData\n");
			fprintf(inifile,"DESTINDIR1=c:\\datarealtime\\RadData\n");

			MessageBox("INI file not found.  MicXferC created a new one!",
				"MicXferC: Error",MB_OK | MB_ICONINFORMATION);
			fclose(inifile);
		}
		else
		{
			MessageBox("INI file not found and cannot be created! Terminating MicXferC.",
				"MicXferC: Error",MB_OK | MB_ICONSTOP);
			EndDialog(-1);
		}
	}
	else
	{
		fclose(inifile);
	}


	// TODO: Add extra initialization here
	UpdateDisplay();
	m_Tree.SelectItem(m_CONFIG);
	GetClientRect(&m_OldRect);
	
	CWnd* item = GetDlgItem(IDC_TREE1);
	item->GetWindowRect(&m_OldEdit);
	ScreenToClient(&m_OldEdit);
//	char szTemp[16];
//	WritePrivateProfileString("CONFIGURATION","HWND",itoa((int)m_hWnd,szTemp,10),m_szIniFile);

	CRect initRect;
	GetClientRect(initRect);
	initRect.left = initRect.right - GetSystemMetrics(SM_CXHSCROLL);
	initRect.top  = initRect.bottom - GetSystemMetrics(SM_CYVSCROLL);
	m_Grip.Create(WS_CHILD | SBS_SIZEBOX | SBS_SIZEBOXBOTTOMRIGHTALIGN | SBS_SIZEGRIP |
		WS_VISIBLE,initRect, this, AFX_IDW_SIZE_BOX);

	m_bInitialized = true;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

HTREEITEM CTreeDlg::InsertItem(char *szGroup,char *szItem, 
			   char *szPreText, HTREEITEM hItem, DWORD dwType)
{
	char szTemp[_MAX_PATH];
	CString cTemp;
	HTREEITEM hTemp;
	GetPrivateProfileString(szGroup,szItem,NOT_SET,szTemp,sizeof(szTemp),m_szIniFile);
	cTemp = szPreText;

	if (dwType == DOUDPADDRESSES)
	{
		if (strcmp(szTemp,NOT_SET)==0)
			return NULL;
		else
		{
			cTemp += szTemp;
			hTemp = m_Tree.InsertItem(cTemp,hItem);
		}
	}
	else if (dwType == DOUDPSELF)
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	else if (dwType == IGNOREARCH)
	{
		if (strlen(szTemp) > 1)
		{
			if (toupper(szTemp[1]=='I'))
				cTemp += "Yes";
			else
				cTemp += "No";
		}
		else
			cTemp += "No";
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	}
	else if (dwType == DOLOG)
	{
		if (strcmp(szTemp,NOT_SET)==0)
			szTemp[0] = 'N';
		if (toupper(szTemp[0] == 'Y'))
			cTemp += "Yes";
		else
			cTemp += "No";
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	}
	else if (dwType == VERBOSE)
	{
		if (strcmp(szTemp,NOT_SET)==0)
			szTemp[0] = 'N';
		if (toupper(szTemp[0] == 'Y'))
			cTemp += "Yes";
		else
			cTemp += "No";
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	}
	else if (dwType == DOSUB)
	{
		if (strcmp(szTemp,NOT_SET)==0)
			szTemp[0] = 'N';
		if (toupper(szTemp[0] == 'Y'))
			cTemp += "Yes";
		else
			cTemp += "No";
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	}
	else if (dwType == DOUDP)
	{
		if (strcmp(szTemp,NOT_SET)==0)
			szTemp[0] = 'N';
		if (toupper(szTemp[0] == 'Y'))
			cTemp += "Yes";
		else
			cTemp += "No";
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	}
	else if (dwType == DOZIP)
	{
		if (strcmp(szTemp,NOT_SET)==0)
			szTemp[0] = 'N';
		if (toupper(szTemp[0] == 'Y'))
			cTemp += "Yes";
		else
			cTemp += "No";
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	}
	else if (dwType == WHEN)
	{
		int iYr = GetPrivateProfileInt(szGroup,"DATEYEAR",0,m_szIniFile);
		if (iYr < 1999 || iYr > 3000)
			iYr = 1999;
		sprintf(szTemp, "%d", iYr);
		cTemp += szTemp;

		int iMon = GetPrivateProfileInt(szGroup,"DATEMONTH",0,m_szIniFile);
		if (iMon < 1 || iMon > 12)
			iMon = 1;
		sprintf(szTemp, "%d", iMon);
		cTemp += ",";
		cTemp += szTemp;

		int iDay = GetPrivateProfileInt(szGroup,"DATEDAY",0,m_szIniFile);
		if (iDay < 1 || iDay > 31)
			iDay = 1;
		sprintf(szTemp, "%d", iDay);
		cTemp += ",";
		cTemp += szTemp;
	
		int iHr = GetPrivateProfileInt(szGroup,"TIMEHOURS",0,m_szIniFile);
		if (iHr < 0 || iHr > 23)
			iHr = 0;
		sprintf(szTemp, "%d", iHr);
		cTemp += " - ";
		cTemp += szTemp;

		int iMin = GetPrivateProfileInt(szGroup,"TIMEMINUTES",0,m_szIniFile);
		if (iMin < 0 || iMin > 59)
			iMin = 0;
		sprintf(szTemp, "%d", iMin);
		cTemp += ":";
		cTemp += szTemp;

		int iSec = GetPrivateProfileInt(szGroup,"TIMESECONDS",0,m_szIniFile);
		if (iSec < 0 || iSec > 59)
			iSec = 0;
		sprintf(szTemp, "%d", iSec);
		cTemp += ":";
		cTemp += szTemp;

		int i = GetPrivateProfileInt(szGroup,"DAYOFWEEK",0,m_szIniFile);
		cTemp += " - ";
		switch (i)
		{
		case 0: cTemp += "none";break;
		case 1: cTemp += "Sunday";break;
		case 2: cTemp += "Monday";break;
		case 3: cTemp += "Tuesday";break;
		case 4: cTemp += "Wednesday";break;
		case 5: cTemp += "Thursday";break;
		case 6: cTemp += "Friday";break;
		case 7: cTemp += "Saturday";break;
		default:
			cTemp += "not set";
		}
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	}
	else if (dwType == CYCLE)
	{
		int iCycle = atoi(szTemp);
		if (iCycle < 2)
			iCycle = 2;
		sprintf(szTemp, "%d", iCycle);
		cTemp += szTemp;
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	}
	else
	{
		cTemp += szTemp;
		hTemp = m_Tree.InsertItem(cTemp,hItem);
	}

	m_Tree.SetItemData(hTemp,dwType);
	return hTemp;
}

HTREEITEM CTreeDlg::InsertSet(char *szSetName)
{
	HTREEITEM hItem;
	HTREEITEM hTemp;
	hItem = m_Tree.InsertItem(szSetName, TVI_ROOT, TVI_LAST );
	m_Tree.SetItemData(hItem,GROUP);
	hTemp = InsertItem(szSetName,"CONTROL",		"Schedule: ",				hItem,CONTROL);
	hTemp = InsertItem(szSetName,"DATEYEAR",	"Y,M,D - H:M:S - WeekDay: ",hTemp,WHEN);
	hTemp = InsertItem(szSetName,"FILESPEC",	"File Specifications: ",	hItem,FILESPEC);
	hTemp = InsertItem(szSetName,"DOSUB",		"Do Sub-Directories: ",		hItem,DOSUB);
	hTemp = InsertItem(szSetName,"DOZIP",		"Move into ZIP file: ",		hItem,DOZIP);
	hTemp = InsertItem(szSetName,"DOZIP",		"Ignore Archive Bit: ",		hTemp,IGNOREARCH);
	hTemp = InsertItem(szSetName,"PREVIOUS",	"Previous run: ",			hItem,PREVIOUS);
	hTemp = InsertItem(szSetName,"NEXT",		"Next run: ",				hItem,NEXT);
	InsertItemFromTos(szSetName,hItem);
	return hItem;
}

void CTreeDlg::InsertItemFromTos(char *szSetName, HTREEITEM hItem)
{
	bool done = false;
	char szTemp1[_MAX_PATH];
	char szTemp2[_MAX_PATH];
	char szTemp3[_MAX_PATH];
	char szTemp4[_MAX_PATH];
	CString cTemp;
	HTREEITEM hTempItem;
	bool bToDone;
	int  iFromIndex;
	int  iToIndex;
	iFromIndex = 1;
	while (!done)
	{
		sprintf(szTemp1,"SOURCEDIR%d",iFromIndex);
		GetPrivateProfileString(szSetName,szTemp1,"",szTemp2,sizeof(szTemp2),m_szIniFile);
		if (strlen(szTemp2))
		{
			iToIndex = 0;
			bToDone = false;

			cTemp = "From: ";
			cTemp += szTemp2;
			hTempItem = m_Tree.InsertItem(cTemp,hItem);
			m_Tree.SetItemData(hTempItem,FROM);

			while (!bToDone)
			{
				if (iToIndex)
					sprintf(szTemp3,"DESTINDIR%d%c",iFromIndex,'a' + iToIndex - 1);
				else
					sprintf(szTemp3,"DESTINDIR%d",iFromIndex);
				GetPrivateProfileString(szSetName,szTemp3,"",szTemp4,sizeof(szTemp4),m_szIniFile);
				if (strlen(szTemp4))
				{
					cTemp = "To: ";
					cTemp += szTemp4;
					m_Tree.SetItemData(m_Tree.InsertItem(cTemp,hTempItem),TO);
				}
				else
				{
					bToDone = true;
				}
				iToIndex++;
			}
		}
		else
		{
			done = true;
		}
		iFromIndex++;
	}
}

void CTreeDlg::UpdateDisplay()
{
	char  cTemp[_MAX_PATH];
	HTREEITEM m_Temp;
	HTREEITEM m_Temp2;
	strcpy(cTemp,"CONFIGURATION");
	m_CONFIG = m_Tree.InsertItem(cTemp);
	m_Tree.SetItemData(m_CONFIG,OTHER);
	m_Temp = InsertItem(cTemp,"CYCLE",		"Cycle Period: ",m_CONFIG,CYCLE);
	m_Temp = InsertItem(cTemp,"VALIDATEPATH","Path\\Name of Calendar File Validator: ",m_CONFIG,VALIDATEPATH);
	m_Temp = InsertItem(cTemp,"ZIPPATH",	"Path\\Name of ZIP utility: ",m_CONFIG,ZIPPATH);
	m_Temp = InsertItem(cTemp,"USERNAME",	"Current Account: ",m_CONFIG,USERNAME);
	m_Temp = InsertItem(cTemp,"DOMAINNAME",	"Current Machine: ",m_CONFIG,DOMAINNAME);
	m_Temp = InsertItem(cTemp,"VERSION",	"Version: ",m_CONFIG,XFERVERSION);
	m_Temp = InsertItem(cTemp,"INITIALPAUSE_SEC","Pause on startup (sec): ",m_CONFIG,STARTPAUSE);
	m_Temp2 = InsertItem(cTemp,"DOLOG",		"Do Log File: ",m_CONFIG,DOLOG);
	m_Temp = InsertItem(cTemp,"LOGPATHNAME","Path and name of Log File: ",m_Temp2,LOGPATHNAME);
	m_Temp = InsertItem(cTemp,"MAXLOG",		"Maximum Log Size: ",m_Temp2,MAXLOG);
	m_Temp = InsertItem(cTemp,"VERBOSE",	"Be Verbose in Log: ",m_Temp2,VERBOSE);
	m_Temp2 = m_Temp = InsertItem(cTemp,"DO_UDP",		"Do UDP Logging: ",m_CONFIG,DOUDP);
	m_Temp = InsertItem(cTemp,"PORT",		"UDP Port Number: ",m_Temp2,PORT);
	m_Temp = InsertItem(cTemp,"VERBOSEUDP","Be Verbose in UDP: ", m_Temp2,VERBOSEUDP);
	m_Temp2 = InsertItem(cTemp,"UDP_SELF",	"Send to this Computer and to...",m_Temp2,DOUDPSELF);
	char szTemp[8];
	int iIndex = 1;
	do 
	{
		sprintf(szTemp,"%d ",iIndex);
		m_Temp = InsertItem(cTemp,szTemp,	"",m_Temp2,DOUDPADDRESSES);
		iIndex++;
	}
	while (m_Temp != NULL);

	strcpy(cTemp,"CALENDAR");
	m_CALENDAR = m_Tree.InsertItem(cTemp);
	m_Tree.SetItemData(m_CALENDAR,OTHER);
	m_Temp = InsertItem(cTemp,"CONTROL",	"Schedule: ",m_CALENDAR,CONTROL);
	m_Temp = InsertItem(cTemp,"DATEYEAR",	"Y,M,D - H:M:S - WeekDay: ",m_Temp,WHEN);
	m_Temp = InsertItem(cTemp,"FILESPEC",	"File Specifications: ", m_CALENDAR,FILESPEC);
	m_Temp = InsertItem(cTemp,"PREVIOUS",   "Previous run: ",m_CALENDAR,PREVIOUS);
	m_Temp = InsertItem(cTemp,"NEXT",		"Next run: ",m_CALENDAR,NEXT);
	InsertItemFromTos(cTemp,m_CALENDAR);
	
	m_ACQUIRE	= InsertSet("ACQUIRE");
	m_CEV		= InsertSet("CEV");
	m_PFM		= InsertSet("PFM");
	m_JPG		= InsertSet("JPG");
	m_OTHER		= InsertSet("OTHER");

	if (GetFileAttributes(m_szIniFile) & FILE_ATTRIBUTE_ARCHIVE)
	{
		//set icon to check
		m_StaticImage.SetIcon(m_hIconWait);
		//set text to micxfer has read
		SetDlgItemText(IDC_STATIC_MICXFERSTAT,"MicXfer has NOT read these values.");
		//kill timer
		SetTimer(1000,500,NULL);
	}
	else
	{
		m_StaticImage.SetIcon(m_hIconCheck);
		SetDlgItemText(IDC_STATIC_MICXFERSTAT,"MicXfer apparently HAS read these values.");
		KillTimer(1000);
	}
}	


void CTreeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTreeDlg::OnPaint() 
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
HCURSOR CTreeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTreeDlg::OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CTreeDlg::OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
//	HTREEITEM Selected = m_Tree.GetSelectedItem();
//	m_Tree.Expand(Selected,TVE_TOGGLE);
	*pResult = 0;
}

void CTreeDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM Selected = m_Tree.GetSelectedItem();
	DWORD data = m_Tree.GetItemData(Selected);
	
	switch (data) {
	case DOUDPADDRESSES:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(true);
		m_ButtonAdd.EnableWindow(false);
		m_ButtonAdd.SetWindowText("Add New TO");
		break;
	case DOUDPSELF:
		m_ButtonEdit.EnableWindow(false);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(true);
		m_ButtonAdd.SetWindowText("Add New TO");
		break;
	case VERBOSEUDP:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case LOGPATHNAME:
	case DOUDP:
	case PORT:
	case ZIPPATH:
	case CYCLE:
	case DOZIP:
	case IGNOREARCH:
	case DOSUB:
	case STARTPAUSE:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case FROM:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(true);
		m_ButtonAdd.EnableWindow(true);
		m_ButtonAdd.SetWindowText("Add New TO");
		break;
	case TO:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(true);
		m_ButtonAdd.EnableWindow(false);
		break;
	case GROUP:
		m_ButtonEdit.EnableWindow(false);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(true);
		m_ButtonAdd.SetWindowText("Add New FROM");
		break;
	case WHEN:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case CONTROL:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case FILESPEC:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case NEXT:
		m_ButtonEdit.EnableWindow(false);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case PREVIOUS:
		m_ButtonEdit.EnableWindow(false);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case DOLOG:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case VERBOSE:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case MAXLOG:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	case VALIDATEPATH:
		m_ButtonEdit.EnableWindow(true);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
		break;
	default:
		m_ButtonEdit.EnableWindow(false);
		m_ButtonDelete.EnableWindow(false);
		m_ButtonAdd.EnableWindow(false);
	}
	*pResult = 0;
}

void CTreeDlg::OnButtonEdit() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM Selected = m_Tree.GetSelectedItem();
	HTREEITEM SelectedChild;
	DWORD data = m_Tree.GetItemData(Selected);
	CString cTemp;
	UINT uResult;
	int dSched,dWeekd,dYear,dMonth,dDay,dHour,dMinute,dSecond;
	switch (data) {
	case LOGPATHNAME:
		{
			cTemp = m_Tree.GetItemText(Selected);
			//m_Tree.SetItemText(Selected,"Path and name of log file: C:\\mic\\%y_%m_%d_log.txt");
			CFileSpec cFileSpec;
			cFileSpec.m_Message = "Enter path and name of log file.\n\n"
				"%y = 2 char year, %Y = 4 char year, %m = month, %B = text month,"
				" %b abbr. month, %d = day, %A = text day, %a = abbr. day\n\n"
				"Example:\n"
				"C:\\mic\\LOG_%Y_%m_%d.txt => c:\\mic\\LOG_2003_10_28.txt\n"
				"C:\\mic\\LOG_%y%b%d.txt => c:\\mic\\LOG_03Oct28.txt";
			cFileSpec.m_bGetForm = true;
			if ((uResult = cFileSpec.DoModal()) == IDOK)
			{
				CString cString("Path and name of Log File: ");
				cString += cFileSpec.m_cPath;
				m_Tree.SetItemText(Selected,cString);
			}
			else
				uResult = IDCANCEL;
		}
		break;

	case DOUDPADDRESSES:
		{
			cTemp = m_Tree.GetItemText(Selected);
			CIPAddressDlg cIPAddress;
			strcpy(cIPAddress.szAddress,cTemp);
			strcpy(cIPAddress.szMsg,"Set the desired address and click on Ok");
			if ((uResult = cIPAddress.DoModal()) == IDOK)
				m_Tree.SetItemText(Selected,cIPAddress.szAddress);
			else
				uResult = IDCANCEL;
		}
		break;

	case STARTPAUSE:
		cTemp = m_Tree.GetItemText(Selected);
		cTemp = cTemp.Right(cTemp.GetLength()-14);
		m_GetNumber.m_dValue = atoi(LPCSTR(cTemp));
		m_GetNumber.m_cMessage = "Enter the number of seconds to pause after a restart of MICXfer.";
		m_GetNumber.m_cTitle = "Initial Start Pause";
		if ((uResult = m_GetNumber.DoModal()) == IDOK)
		{
			if (m_GetNumber.m_dValue >= 0) 
			{
				cTemp.Format("Pause on startup (sec): %d",m_GetNumber.m_dValue);
				m_Tree.SetItemText(Selected,cTemp);
			}
			else
				uResult = IDCANCEL;
		}
		break;

	case PORT:
		cTemp = m_Tree.GetItemText(Selected);
		cTemp = cTemp.Right(cTemp.GetLength()-14);
		m_GetNumber.m_dValue = atoi(LPCSTR(cTemp));
		m_GetNumber.m_cMessage = "Enter the UDP port number (1-65000). MICXfer will send log messages on this port number.";
		m_GetNumber.m_cTitle = "UDP Port Number";
		if ((uResult = m_GetNumber.DoModal()) == IDOK)
		{
			if ((m_GetNumber.m_dValue >= 1) && (m_GetNumber.m_dValue <= 65000))
			{
				cTemp.Format("UDP Port Number: %d",m_GetNumber.m_dValue);
				m_Tree.SetItemText(Selected,cTemp);
			}
			else
				uResult = IDCANCEL;
		}
		break;
	case DOUDP:
		uResult = MessageBox("Send messages to a UDP receiver (e.g. MIC)?","MicXfer Config",MB_YESNOCANCEL | MB_ICONQUESTION);
		if (uResult == IDYES)
			m_Tree.SetItemText(Selected,"Do UDP Logging: Yes");
		else if (uResult == IDNO)
			m_Tree.SetItemText(Selected,"Do UDP Logging: No");
		else
			uResult = IDCANCEL;
		break;
	case DOZIP:
		uResult = MessageBox("Move files into a ZIP file?","MicXfer Config",MB_YESNOCANCEL | MB_ICONQUESTION);
		if (uResult == IDYES)
			m_Tree.SetItemText(Selected,"Move into ZIP file: Yes");
		else if (uResult == IDNO)
			m_Tree.SetItemText(Selected,"Move into ZIP file: No");
		else
			uResult = IDCANCEL;
		break; 
	case IGNOREARCH:
		uResult = MessageBox("Ignore Archive Bit?","MicXfer Config",MB_YESNOCANCEL | MB_ICONQUESTION);
		if (uResult == IDYES)
			m_Tree.SetItemText(Selected,"Ignore Archive Bit: Yes");
		else if (uResult == IDNO)
			m_Tree.SetItemText(Selected,"Ignore Archive Bit: No");
		else
			uResult = IDCANCEL;
		break;
	case DOSUB:
		uResult = MessageBox("Step into sub-directories?","MicXfer Config",MB_YESNOCANCEL | MB_ICONQUESTION);
		if (uResult == IDYES)
			m_Tree.SetItemText(Selected,"Do Sub-Directories: Yes");
		else if (uResult == IDNO)
			m_Tree.SetItemText(Selected,"Do Sub-Directories: No");
		else
			uResult = IDCANCEL;
		break;
	case FROM:
		m_FileSpec.m_Name = "From...";
		m_FileSpec.m_bAddGemAuth = false;
		m_FileSpec.m_bAllowBrowse = true;
		cTemp = m_Tree.GetItemText(Selected);
		m_FileSpec.m_cPath = cTemp.Right(cTemp.GetLength()-6);
		m_FileSpec.m_FileSpec = m_FileSpec.m_cPath;
		if ((uResult = m_FileSpec.DoModal()) == IDOK)
		{
			if (m_FileSpec.m_cPath.GetLength())
			{
				cTemp = "From: ";
				cTemp += m_FileSpec.m_cPath;
				m_Tree.SetItemText(Selected,cTemp);
			}
			else
				uResult = IDCANCEL;
		}
		break;
	case TO:
		m_FileSpec.m_bAddGemAuth = false;
		m_FileSpec.m_bAllowBrowse = true;
		m_FileSpec.m_Name = "To...";
		m_FileSpec.m_Message = "Set to destination directory.  Use \"NULL\" if no destination (ZIP only).";
		cTemp = m_Tree.GetItemText(Selected);
		m_FileSpec.m_cPath = cTemp.Right(cTemp.GetLength()-4);
		m_FileSpec.m_FileSpec = m_FileSpec.m_cPath;
		if ((uResult = m_FileSpec.DoModal()) == IDOK)
		{
			cTemp = "To: ";
			if (m_FileSpec.m_cPath.GetLength() == 0)
				cTemp += "NULL";
			else
				cTemp += m_FileSpec.m_cPath;
			m_Tree.SetItemText(Selected,cTemp);
		}
		break;
	case GROUP:
		//can't get here
		uResult = IDCANCEL;
		break;
	case WHEN:
		Selected = m_Tree.GetParentItem(Selected);
		//deliberately fall through and do the control
	case CONTROL:
		cTemp = m_Tree.GetItemText(Selected);
		cTemp.MakeUpper();
		if (cTemp.Find("NEVER")>=0)
			dSched = 0;
		else if (cTemp.Find("PROGRAM")>=0)
			dSched = 1;
		else if (cTemp.Find("MONTHLY")>=0)
			dSched = 2;
		else if (cTemp.Find("WEEKLY")>=0)
			dSched = 3;
		else if (cTemp.Find("DAILY")>=0)
			dSched = 4;
		else if (cTemp.Find("HOURLY")>=0)
			dSched = 5;
		else if (cTemp.Find("MINUTE")>=0)
			dSched = 6;
		else if (cTemp.Find("SECOND")>=0)
			dSched = 7;
		else 
			dSched = 0;
		if (m_Tree.ItemHasChildren(Selected))
		{
			SelectedChild = m_Tree.GetChildItem(Selected);
			cTemp = m_Tree.GetItemText(SelectedChild);
			char temp[32];
			sscanf(LPCSTR(cTemp),"Y,M,D - H:M:S - WeekDay: %d,%d,%d - %d:%d:%d - %s",
				&dYear,&dMonth,&dDay,
				&dHour,&dMinute,&dSecond,temp);
			temp[0] = toupper(temp[0]);
			temp[1] = toupper(temp[1]);
			switch (temp[0]) {
			case 'S':
				switch (temp[1]) {
				case 'U':
					dWeekd = 1;
					break;
				case 'A':
					dWeekd = 7;
					break;
				default:
					dWeekd = 0;
					break;
				}
				break;
			case 'M':
				dWeekd = 2;
				break;
			case 'T':
				switch (temp[1]) {
				case 'U':
					dWeekd = 3;
					break;
				case 'H':
					dWeekd = 5;
					break;
				default:
					dWeekd = 0;
					break;
				}
				break;
				break;
			case 'W':
				dWeekd = 4;
				break;
			case 'F':
				dWeekd = 6;
				break;
			default:
				dWeekd = 0;
				break;
			}
		}
		else
		{
			dYear  = 1999;
			dMonth = dDay  = 1;
			dWeekd = dHour = dMinute = dSecond = 0;
		}
		m_Schedule.Setup(dSched,dWeekd,dYear,dMonth,dDay,dHour,dMinute,dSecond);
		if ((uResult = m_Schedule.DoModal()) == IDOK)
		{
			//get and set results
			cTemp = "Schedule: " + m_Schedule.m_cSchedule;
			m_Tree.SetItemText(Selected,cTemp);
			//selected
			//selectedchild
			cTemp.Format("Y,M,D - H:M:S - WeekDay: %d,%d,%d - %d:%d:%d - %d",
				m_Schedule.m_iYear,
				m_Schedule.m_iMonth,
				m_Schedule.m_iDay,
				m_Schedule.m_iHours,
				m_Schedule.m_iMinutes,
				m_Schedule.m_iSeconds,
				m_Schedule.m_cWeekDay);
			m_Tree.SetItemText(SelectedChild,cTemp);
		}
		break;
	case FILESPEC:
		m_FileSpec.m_bAllowBrowse = false;
		m_FileSpec.m_bAddGemAuth = false;
		m_FileSpec.m_Name = "File Specifications";
		m_FileSpec.m_Message = "Enter file specifications. \nFor example: *.BIN, *.ISR, *.MCA, *.JSR, *.CEV, *.PFM, *.BNY, *.GPS, *.VCS, *.ESS";
		cTemp = m_Tree.GetItemText(Selected);
		m_FileSpec.m_FileSpec = cTemp.Right(cTemp.GetLength()-21);
		m_FileSpec.m_cPath = m_FileSpec.m_FileSpec;
		if ((uResult = m_FileSpec.DoModal()) == IDOK)
		{
			if (m_FileSpec.m_cPath.GetLength())
			{
				cTemp = "File Specifications: ";
				cTemp += m_FileSpec.m_cPath;
				m_Tree.SetItemText(Selected,cTemp);
			}
			else
				uResult = IDCANCEL;
		}
		
		break;
	case DOLOG:
		uResult = MessageBox("Make entries in a log file?","MicXfer Config",MB_YESNOCANCEL | MB_ICONQUESTION);
		if (uResult == IDYES)
			m_Tree.SetItemText(Selected,"Do Log File: Yes");
		else if (uResult == IDNO)
			m_Tree.SetItemText(Selected,"Do Log File: No");
		else
			uResult = IDCANCEL;
		break;
	case VERBOSE:
		uResult = MessageBox("Make VERBOSE entries in the log file?\n"
			"Select \"No\" to log only GOOD file transfers.\n"
			"Select \"Yes\" to log as much as possible.",
			"MicXfer Config",MB_YESNOCANCEL | MB_ICONQUESTION);
		if (uResult == IDYES)
			m_Tree.SetItemText(Selected,"Be Verbose in Log: Yes");
		else if (uResult == IDNO)
			m_Tree.SetItemText(Selected,"Be Verbose in Log: No");
		else
			uResult = IDCANCEL;
		break;

	case VERBOSEUDP:
		uResult = MessageBox("Send VERBOSE entries in the UDP data?\n"
			"Select \"No\" to send only GOOD file transfers."
			"Select \"Yes\" to send as much as possible.",
			"MicXfer Config",MB_YESNOCANCEL | MB_ICONQUESTION);
		if (uResult == IDYES)
			m_Tree.SetItemText(Selected,"Be Verbose in UDP: Yes");
		else if (uResult == IDNO)
			m_Tree.SetItemText(Selected,"Be Verbose in UDP: No");
		else
			uResult = IDCANCEL;
		break;

	case VALIDATEPATH:
		m_FileSpec.m_bAllowBrowse = true;
		m_FileSpec.m_bAddGemAuth = true;
		m_FileSpec.m_Name = "Calendar Validation Program";
		m_FileSpec.m_Message = "Enter full path to GEMAUTH.EXE.";
		m_FileSpec.m_AddOn = "GemAuth.exe";
		cTemp = m_Tree.GetItemText(Selected);
		cTemp = cTemp.Right(cTemp.GetLength()-38);
		m_FileSpec.m_FileSpec = cTemp;
		m_FileSpec.m_cPath = cTemp.Right(cTemp.GetLength()-4);
		if ((uResult = m_FileSpec.DoModal()) == IDOK)
		{
			if (m_FileSpec.m_cPath.GetLength())
			{
				cTemp = "Path\\Name of Calendar File Validator: ";
				cTemp += m_FileSpec.m_cPath;
				m_Tree.SetItemText(Selected,cTemp);
			}
			else
				uResult = IDCANCEL;
		}
		break;

	case ZIPPATH:
		m_FileSpec.m_bAllowBrowse = true;
		m_FileSpec.m_bAddGemAuth = true;
		m_FileSpec.m_Name = "ZIP Program";
		m_FileSpec.m_AddOn = "PKZIP25.EXE";
		m_FileSpec.m_Message = "Enter full path and name to PKZIP25.EXE.";
		cTemp = m_Tree.GetItemText(Selected);
		cTemp = cTemp.Right(cTemp.GetLength()-26);
		m_FileSpec.m_FileSpec = cTemp;
		m_FileSpec.m_cPath = cTemp.Right(cTemp.GetLength()-4);
		if ((uResult = m_FileSpec.DoModal()) == IDOK)
		{
			if (m_FileSpec.m_cPath.GetLength())
			{
				cTemp = "Path\\Name of ZIP utility: ";
				cTemp += m_FileSpec.m_cPath;
				m_Tree.SetItemText(Selected,cTemp);
			}
			else
				uResult = IDCANCEL;
		}
		break;

	case MAXLOG:
		cTemp = m_Tree.GetItemText(Selected);
		cTemp = cTemp.Right(cTemp.GetLength()-18);
		m_GetNumber.m_dValue = atoi(LPCSTR(cTemp));
		m_GetNumber.m_cMessage = "Enter the maximum size in bytes"
			" the log file will be allowed to be."
			"  Zero (0) means NO limit.";
		m_GetNumber.m_cTitle = "Max Log Size";
		if ((uResult = m_GetNumber.DoModal()) == IDOK)
		{
			if (m_GetNumber.m_dValue >= 0)
			{
				cTemp.Format("Maximum Log Size: %d",m_GetNumber.m_dValue);
				m_Tree.SetItemText(Selected,cTemp);
			}
			else
				uResult = IDCANCEL;
		}
		break;

	case CYCLE:
		cTemp = m_Tree.GetItemText(Selected);
		cTemp = cTemp.Right(cTemp.GetLength()-14);
		m_GetNumber.m_dValue = atoi(LPCSTR(cTemp));
		m_GetNumber.m_cMessage = "Enter the cycle period in seconds."
			"  MicXfer will check all directories this often.";
		m_GetNumber.m_cTitle = "Cycle Period";
		if ((uResult = m_GetNumber.DoModal()) == IDOK)
		{
			if (m_GetNumber.m_dValue >= 2)
			{
				cTemp.Format("Cycle Period: %d",m_GetNumber.m_dValue);
				m_Tree.SetItemText(Selected,cTemp);
			}
			else
				uResult = IDCANCEL;
		}
		break;
	default:;
	}
	if (uResult != IDCANCEL)
	{
		m_StaticImage.SetIcon(m_hIconQuest);
		SetDlgItemText(IDC_STATIC_MICXFERSTAT,"Changed values not saved to INI yet.");
		KillTimer(1000);
	}
}

void CTreeDlg::OnButtonReload() 
{
	// TODO: Add your control notification handler code here
	m_Tree.DeleteAllItems();
	UpdateDisplay();
	m_Tree.SelectItem(m_CONFIG);
//	m_StaticImage.SetIcon(m_hIconCheck);
//	SetDlgItemText(IDC_STATIC_MICXFERSTAT,"MicXfer Has These Values.");
}

void CTreeDlg::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM Selected = m_Tree.GetSelectedItem();
	HTREEITEM SelectedChild;
	DWORD data = m_Tree.GetItemData(Selected);
	switch (data)
	{
	case DOUDPADDRESSES:
	case TO:
		m_Tree.DeleteItem(Selected);
		break;
	case FROM:
		while ((SelectedChild = m_Tree.GetChildItem(Selected)) != NULL)
			m_Tree.DeleteItem(SelectedChild);
		m_Tree.DeleteItem(Selected);
		break;
	}
	m_StaticImage.SetIcon(m_hIconQuest);
	SetDlgItemText(IDC_STATIC_MICXFERSTAT,"Changed values not saved to INI yet.");
	KillTimer(1000);
}

void CTreeDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM Selected = m_Tree.GetSelectedItem();
	DWORD data = m_Tree.GetItemData(Selected);
	CString cTemp;
	switch (data)
	{
	case DOUDPSELF:
		{
			CIPAddressDlg cIPAddress;
			strcpy(cIPAddress.szAddress,"0.0.0.0");
			strcpy(cIPAddress.szMsg,"Set the desired address and click on Ok");
			if (cIPAddress.DoModal() == IDOK)
			{
				Selected = m_Tree.InsertItem(cIPAddress.szAddress,Selected);
				m_Tree.SetItemData(Selected,DOUDPADDRESSES);
			}
		}
		break;

	case GROUP:
		//add a from
		m_FileSpec.m_Name = "From...";
		m_FileSpec.m_bAllowBrowse = true;
		m_FileSpec.m_bAddGemAuth = false;
		m_FileSpec.m_FileSpec.Empty();
		cTemp = m_Tree.GetItemText(Selected);
		cTemp += "\nFrom:";
		m_FileSpec.m_cPath = "";
		m_FileSpec.m_Message = cTemp;
		if (m_FileSpec.DoModal() == IDOK)
		{
			if (m_FileSpec.m_cPath.GetLength())
			{
				cTemp = "From: ";
				cTemp += m_FileSpec.m_cPath;
				Selected = m_Tree.InsertItem(cTemp,Selected);
				m_Tree.SetItemData(Selected,FROM);
				m_Tree.Invalidate();
				m_StaticImage.SetIcon(m_hIconQuest);
				SetDlgItemText(IDC_STATIC_MICXFERSTAT,"Changed values not saved to INI yet.");
				KillTimer(1000);
			}
		}
		break;

	case FROM:
		//add a TO
		m_FileSpec.m_Name = "To...";
		m_FileSpec.m_bAllowBrowse = true;
		m_FileSpec.m_bAddGemAuth = false;
		m_FileSpec.m_FileSpec.Empty();
		m_FileSpec.m_cPath = "";
		cTemp = m_Tree.GetItemText(Selected);
		cTemp += "\nSet to destination directory.\nUse \"NULL\" if no destination (ZIP only).\nTo:";
		m_FileSpec.m_Message = cTemp;
		if (m_FileSpec.DoModal() == IDOK)
		{
			cTemp = "To: ";
			if (m_FileSpec.m_cPath.GetLength()==0)
				cTemp += "NULL";
			else
				cTemp += m_FileSpec.m_cPath;
			Selected = m_Tree.InsertItem(cTemp,Selected);
			m_Tree.SetItemData(Selected,TO);
			m_Tree.Invalidate();
			m_StaticImage.SetIcon(m_hIconQuest);
			SetDlgItemText(IDC_STATIC_MICXFERSTAT,"Changed values not saved to INI yet.");
			KillTimer(1000);
		}
		break;
	}
}

void CTreeDlg::OnOK() 
{
	// TODO: Add extra validation here
	//do configuration section
	WriteSection("CONFIGURATION",m_CONFIG);
	//do calendar section
	WriteSection("CALENDAR",m_CALENDAR);
	//do cev section
	WriteSection("CEV",m_CEV);
	//do pfm Section
	WriteSection("PFM",m_PFM);
	//do acquire section
	WriteSection("ACQUIRE",m_ACQUIRE);
	//do JPG secton
	WriteSection("JPG",m_JPG);
	//do other section
	WriteSection("OTHER",m_OTHER);
	//flush the ini cache
	WritePrivateProfileString(NULL,NULL,NULL,m_szIniFile);
	//change the icon to wait
	m_StaticImage.SetIcon(m_hIconWait);
	//tell the user we are waiting
	SetDlgItemText(IDC_STATIC_MICXFERSTAT,"MicXfer has NOT read these values.");
	//start watching for archive bit to change
	SetTimer(1000,500,NULL);

//	CDialog::OnOK();
}

void CTreeDlg::WriteSection(char* szName, HTREEITEM hItem)
{
	HTREEITEM Selected;
	HTREEITEM SelectedChild;
	HTREEITEM SelectedChild2;
	DWORD data;
	CString cTemp;
	CString cTempA;
	char szTemp[16];
	int dYear;
	int dMonth;
	int dDay;
	int dHour;
	int dMinute;
	int dSecond;
	int dFromIndex;
	int dToIndex;
	char szWeekd[4];
	int dIndex;
	Selected = m_Tree.GetChildItem(hItem);
	dFromIndex = 1;
	WritePrivateProfileString(szName,NULL,NULL,m_szIniFile);
	dIndex = 0;
	//char szIndex[16];
	while (Selected)
	{
		data = m_Tree.GetItemData(Selected);
		cTemp = m_Tree.GetItemText(Selected);
		switch (data)
		{
		case XFERVERSION:
			cTemp = cTemp.Right(cTemp.GetLength()-9);
			WritePrivateProfileString(szName,"VERSION",cTemp,m_szIniFile);
			break;
		case USERNAME:
			cTemp = cTemp.Right(cTemp.GetLength()-17);
			WritePrivateProfileString(szName,"USERNAME",cTemp,m_szIniFile);
			break;

		case DOMAINNAME:
			cTemp = cTemp.Right(cTemp.GetLength()-17);
			WritePrivateProfileString(szName,"DOMAINNAME",cTemp,m_szIniFile);
			break;

		case DOUDP:
			cTemp = cTemp.Right(cTemp.GetLength()-16);
			if (cTemp == NOT_SET)
				cTemp = "No";
			WritePrivateProfileString(szName,"DO_UDP",cTemp,m_szIniFile);
			SelectedChild = m_Tree.GetChildItem(Selected);
			while (SelectedChild)
			{
				cTemp = m_Tree.GetItemText(SelectedChild);
//				if (m_Tree.GetItemData(SelectedChild)==DOUDPADDRESSES)
//				{
//					cTemp = m_Tree.GetItemText(SelectedChild);
//					dIndex++;
//					sprintf(szIndex,"%d",dIndex);
//					WritePrivateProfileString(szName,szIndex,cTemp,m_szIniFile);
//				} 
//				else 
				if (m_Tree.GetItemData(SelectedChild)==VERBOSEUDP)
				{
//					cTemp = m_Tree.GetItemText(SelectedChild);
					WritePrivateProfileString(szName,"VERBOSEUDP",cTemp.Mid(19),m_szIniFile);
				}
				else if (m_Tree.GetItemData(SelectedChild)==PORT)
				{
//					cTemp = m_Tree.GetItemText(SelectedChild);
					WritePrivateProfileString(szName,"PORT",cTemp.Mid(17),m_szIniFile);
				}
				else if (m_Tree.GetItemData(SelectedChild)==DOUDPSELF)
				{
					SelectedChild2 = m_Tree.GetChildItem(SelectedChild);
					CString cTemp;
					int iIndex = 0;
					char szTemp[8];
					while (SelectedChild2)
					{
						if (m_Tree.GetItemData(SelectedChild2)==DOUDPADDRESSES)
						{
							cTemp = m_Tree.GetItemText(SelectedChild2);
							sprintf(szTemp,"%d",++dIndex);
							WritePrivateProfileString(szName,szTemp,cTemp,m_szIniFile);
						} 
						SelectedChild2 = m_Tree.GetNextSiblingItem(SelectedChild2);
					}
				}
				SelectedChild = m_Tree.GetNextSiblingItem(SelectedChild);
			}
			break;

		case STARTPAUSE:
			cTemp = cTemp.Right(cTemp.GetLength()-24);
			if (cTemp == NOT_SET)
				cTemp = "0";
			WritePrivateProfileString(szName,"INITIALPAUSE_SEC",cTemp,m_szIniFile);
			break;

		case CYCLE:
			cTemp = cTemp.Right(cTemp.GetLength()-14);
			WritePrivateProfileString(szName,"CYCLE",cTemp,m_szIniFile);
			break;

		case OTHER:
			//previous, next, all use type OTHER
//			cTemp = cTemp.Right(cTemp.GetLength()-9);
//			WritePrivateProfileString(szName,"VERSION",cTemp,m_szIniFile);
			break;
		case DOLOG:
			cTemp = cTemp.Right(cTemp.GetLength()-13);
			cTemp.MakeUpper();
			WritePrivateProfileString(szName,"DOLOG",(cTemp[0] == 'Y')?"Yes":"No",m_szIniFile);
			// do MAXLOG also
			SelectedChild = m_Tree.GetChildItem(Selected);

			while (SelectedChild)
			{
				cTemp = m_Tree.GetItemText(SelectedChild);
				if (m_Tree.GetItemData(SelectedChild)==MAXLOG)
				{
					cTemp = m_Tree.GetItemText(SelectedChild);
					WritePrivateProfileString(szName,"MAXLOG",cTemp.Mid(18),m_szIniFile);
				} 
				else if (m_Tree.GetItemData(SelectedChild)==LOGPATHNAME)
				{
					cTemp = m_Tree.GetItemText(SelectedChild);
					WritePrivateProfileString(szName,"LOGPATHNAME",cTemp.Mid(27),m_szIniFile);
				} 
				else if (m_Tree.GetItemData(SelectedChild)==VERBOSE)
				{
					cTemp = m_Tree.GetItemText(SelectedChild);
					WritePrivateProfileString(szName,"VERBOSE",cTemp.Mid(19),m_szIniFile);
				}
				SelectedChild = m_Tree.GetNextSiblingItem(SelectedChild);
			}

			break;
		case VALIDATEPATH:
			cTemp = cTemp.Right(cTemp.GetLength()-38);
			WritePrivateProfileString(szName,"VALIDATEPATH",cTemp,m_szIniFile);
			break;
		case NEXT:
			cTemp = cTemp.Right(cTemp.GetLength()-10);
			WritePrivateProfileString(szName,"NEXT",cTemp,m_szIniFile);
			break;
		case PREVIOUS:
			cTemp = cTemp.Right(cTemp.GetLength()-14);
			WritePrivateProfileString(szName,"PREVIOUS",cTemp,m_szIniFile);
			break;
		case ZIPPATH:
			cTemp = cTemp.Right(cTemp.GetLength()-26);
			WritePrivateProfileString(szName,"ZIPPATH",cTemp,m_szIniFile);
			break;
		case CONTROL:
			cTemp = cTemp.Right(cTemp.GetLength()-10);
			WritePrivateProfileString(szName,"CONTROL",cTemp,m_szIniFile);
			if ((SelectedChild = m_Tree.GetChildItem(Selected))!= NULL)
			{
				cTemp = m_Tree.GetItemText(SelectedChild);
				sscanf(LPCSTR(cTemp),"Y,M,D - H:M:S - WeekDay: %d,%d,%d - %d:%d:%d - %s",
					&dYear,&dMonth,&dDay,
					&dHour,&dMinute,&dSecond,szTemp);
				szTemp[0] = toupper(szTemp[0]);
				szTemp[1] = toupper(szTemp[1]);
				switch (szTemp[0]) {
				case 'S':
					switch (szTemp[1]) {
					case 'U':
						szWeekd[0] = '1';
						break;
					case 'A':
						szWeekd[0] = '7';
						break;
					default:
						szWeekd[0] = '0';
						break;
					}
					break;
				case 'M':
					szWeekd[0] = '2';
					break;
				case 'T':
					switch (szTemp[1]) {
					case 'U':
						szWeekd[0] = '3';
						break;
					case 'H':
						szWeekd[0] = '5';
						break;
					default:
						szWeekd[0] = '0';
						break;
					}
					break;
					break;
				case 'W':
					szWeekd[0] = '4';
					break;
				case 'F':
					szWeekd[0] = '6';
					break;
				default:
					szWeekd[0] = '0';
					break;
				}
				szWeekd[1] = NULL;
			}
			else
			{
				dYear =	dMonth = dDay = dHour = dMinute = dSecond = 0;
				strcpy(szWeekd,"0");
			}

			WritePrivateProfileString(szName,"DATEYEAR",itoa(dYear,szTemp,10),m_szIniFile);
			WritePrivateProfileString(szName,"DATEMONTH",itoa(dMonth,szTemp,10),m_szIniFile);
			WritePrivateProfileString(szName,"DATEDAY",itoa(dDay,szTemp,10),m_szIniFile);
			WritePrivateProfileString(szName,"TIMEHOURS",itoa(dHour,szTemp,10),m_szIniFile);
			WritePrivateProfileString(szName,"TIMEMINUTES",itoa(dMinute,szTemp,10),m_szIniFile);
			WritePrivateProfileString(szName,"TIMESECONDS",itoa(dSecond,szTemp,10),m_szIniFile);
			WritePrivateProfileString(szName,"DAYOFWEEK",szWeekd,m_szIniFile);
			break;

		case FILESPEC:
			cTemp = cTemp.Right(cTemp.GetLength()-21);
			WritePrivateProfileString(szName,"FILESPEC",cTemp,m_szIniFile);
			break;

		case DOZIP:
			cTemp = cTemp.Right(cTemp.GetLength()-20);
			szTemp[0] = (toupper(cTemp[0])=='Y')?'Y':'N';
			if ((SelectedChild = m_Tree.GetChildItem(Selected))!= NULL)
			{
				cTemp = m_Tree.GetItemText(SelectedChild);
				cTemp = cTemp.Right(cTemp.GetLength()-20);
				if (toupper(cTemp[0])=='Y')
					szTemp[1] = 'I';
				else
					szTemp[1] = 'N';
			}
			else
				szTemp[1] = 'N';
			szTemp[2] = NULL;
			WritePrivateProfileString(szName,"DOZIP",szTemp,m_szIniFile);
			break;

		case DOSUB:
			cTemp = cTemp.Right(cTemp.GetLength()-20);
			WritePrivateProfileString(szName,"DOSUB",(toupper(cTemp[0])=='Y')?"Y":"N",m_szIniFile);
			break;
		case FROM:
			cTemp = cTemp.Right(cTemp.GetLength()-6);
			cTempA.Format("SOURCEDIR%d",dFromIndex);
			WritePrivateProfileString(szName,cTempA,cTemp,m_szIniFile);
			//do TOs also
			dToIndex = 1;
			SelectedChild = m_Tree.GetChildItem(Selected);
			while (SelectedChild)
			{
				cTemp = m_Tree.GetItemText(SelectedChild);
				cTemp = cTemp.Right(cTemp.GetLength()-4);
				cTempA.Format("DESTINDIR%d",dFromIndex);
				if (dToIndex > 1)
					cTempA += ('a'+dToIndex-2);
				WritePrivateProfileString(szName,cTempA,cTemp,m_szIniFile);
				SelectedChild = m_Tree.GetNextSiblingItem(SelectedChild);
				dToIndex++;
			}
			dFromIndex++;
			break;
		}
		Selected = m_Tree.GetNextSiblingItem(Selected);
	}
}

void CTreeDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == INVALIDATETIMER)
	{
		Invalidate(TRUE);
		KillTimer(INVALIDATETIMER);
	}
	else if (nIDEvent == 1000)
	{
		//check archive bit
		//if bit is off then
		if ((GetFileAttributes(m_szIniFile) & FILE_ATTRIBUTE_ARCHIVE)==NULL)
		{
			//set icon to check
			m_StaticImage.SetIcon(m_hIconCheck);
			//set text to micxfer has read
			SetDlgItemText(IDC_STATIC_MICXFERSTAT,"MicXfer has read these values.");
			//kill timer
			KillTimer(1000);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString cTemp("Version: "VERSION);
	SetDlgItemText(IDC_STATIC_VERSION,cTemp);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTreeDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_bInitialized && cx > 0 && cy > 130)
	{
		KillTimer(INVALIDATETIMER);
		int iY_Offset = cy - m_OldRect.bottom;
		int iX_Offset = cx - m_OldRect.right;

		CRect rect;	
		CWnd * item;
		
		item = GetDlgItem(IDC_TREE1);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top,
			rect.Width()+ iX_Offset,rect.Height() + iY_Offset,true);

		item = GetDlgItem(IDC_BUTTON1);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset, 
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDOK);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_BUTTON_DELETE);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_BUTTON2);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_BUTTON_ADD);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDCANCEL);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_STATIC_IMAGE);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_STATIC_MICXFERSTAT);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		CWnd *resizeWnd = GetDlgItem(AFX_IDW_SIZE_BOX);
		if (resizeWnd) 
		{
			CRect rect, rect2;
			resizeWnd->GetWindowRect(&rect);
			GetClientRect(rect2);
			rect2.left = rect2.right - rect.Width();
			rect2.top  = rect2.bottom - rect.Height();
			resizeWnd->MoveWindow(&rect2);
		}
		GetClientRect(&m_OldRect);
		SetTimer(INVALIDATETIMER,1000,NULL);
	}

}
