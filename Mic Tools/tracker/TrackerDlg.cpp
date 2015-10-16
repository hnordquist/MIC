// TrackerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tracker.h"
#include "TrackerDlg.h"
#include "Legend.h"
#include "Config.h"
#include "Config2.h"
#include "timedmessagebox.h"
#include "io.h"
#include "version.h"
#include <afxdisp.h>
#include <process.h>
#include <afxinet.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define VERSION "1.8.1.0"

#define CONFIGURATION	"CONFIGURATION"
#define NAME			"NAME"
#define ADDRESS			"ADDRESS"
#define PORT			"PORT"
#define DOHTML			"DOHTML"
#define HTMLTIME		"HTMLTIME"
#define HTMLNAME		"HTMLNAME"
#define MAILUPDATE		"MAILUPDATE"
#define MAILSLOT		"MAILSLOT"
#define DOMAILSLOT		"DOMAILSLOT"
#define LAST_X			"LAST_X"
#define LAST_Y			"LAST_Y"
#define ALLOWFORWARDING	"ALLOWFORWARDING"
#define PUSH			"PUSH"

#define TRIGGER_CMD			(WM_APP + 12)
#define TRIGGER_CLR			(TRIGGER_CMD+1)
#define TRIGGER_EXCLUDE		(WM_USER + 42)

#undef DEBUGXML

#define ALARM			0
#define STATUS			1
#define IMAGE			2
#define INHIBIT			3
#define HEARTBEAT		4

typedef enum {eHTML = 1, eMailSlot = 2, eTrackToTrack = 3, eTrackToTrackLong = 4, eBeepAlarm = 5, eFTPImages=6, ePushXML=7, eTimerMax} TimerType;
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
// CTrackerDlg dialog

CTrackerDlg::CTrackerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrackerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrackerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	int i;
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	for (i = 0; i < MAX_BUTTONS; i++)
	{
		Buttons[i] = NULL;
	}
	for (i = 0; i < MAX_FORWARD; i++)
	{
		m_sForwardTime[i].SetStatus(COleDateTime::invalid);
	}
	m_dNumButtons = 0;
	m_dDefPort = 1028;
	m_sd_dg = INVALID_SOCKET;
	m_bAllowForward = false;
	m_bInSendImages = false;
	m_bToMIC = true;
	m_cIniFile[0] = NULL;
	m_bAcked = true;
	m_hMailSlot = INVALID_HANDLE_VALUE;
}

CTrackerDlg::~CTrackerDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	for (int i=0; i < MAX_BUTTONS; i++)
	{
		if (Buttons[i])
			delete Buttons[i];
	}
	if (m_bDoingMailSlot)
	{
		//close the mailslot
		if (m_hMailSlot != INVALID_HANDLE_VALUE)
			CloseHandle(m_hMailSlot);
	}
	else
	{
		closesocket(m_sd_dg);
		WSACleanup();
	}
}

void CTrackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrackerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTrackerDlg, CDialog)
	//{{AFX_MSG_MAP(CTrackerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_WSARECV,OnReceive)
	ON_CONTROL_RANGE( BN_CLICKED, BUTTONBASE, BUTTONBASE+MAX_BUTTONS, OnInstrumentButton )
	ON_MESSAGE(TRIGGER_CMD,OnTrigger)
	ON_MESSAGE(TRIGGER_CLR,OnTriggerClr)
	ON_MESSAGE(TRIGGER_EXCLUDE,OnTriggerExclude)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrackerDlg message handlers
void 
CTrackerDlg::OnInstrumentButton( UINT nID )
{
//	if (Buttons[nID - BUTTONBASE]->GetFaceColor() == RED)
	m_bAcked = true;
	CLegend Legend;
	GetDlgItemText(nID,Legend.m_cName);
	Legend.DoModal();
}

LRESULT CTrackerDlg::OnTriggerExclude(WPARAM wParam, LPARAM lParam)
{
	//PUSH XML
	char szXML[1400];
	char szTemp1[MAX_PATH];
	char szTemp2[MAX_PATH];
	char szForwardAddress[MAX_PATH];
	char szPort[MAX_PATH];
	szTemp1[0] = NULL;
	szTemp2[0] = NULL;
	GetPrivateProfileString(PUSH_XML,"LOCATION3","",szTemp1,sizeof(szTemp1),m_cIniFile);
	int iSizeXML = FormatXML(szXML, szTemp1, szTemp2, INHIBIT, 0);

	int i = 0;
	char szI[16];
	sprintf_s(szI,"A%d",i);
	GetPrivateProfileString(PUSH_XML,szI,"",
		szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
	while (strlen(szForwardAddress))
	{
		sprintf_s(szPort,"P%d",i);
		int iPort = GetPrivateProfileInt(PUSH_XML,szPort,0,m_cIniFile);
		m_sAddr_Tmp.sin_family = PF_INET;
		m_sAddr_Tmp.sin_addr.s_addr = inet_addr(LPCSTR(szForwardAddress));
		m_sAddr_Tmp.sin_port= htons(iPort);
		try
		{
			int iError = 0;
			if (sendto(m_sd_dg,(char*)&szXML,iSizeXML,0,
				(struct sockaddr FAR *) &m_sAddr_Tmp,sizeof(m_sAddr_Tmp)) == SOCKET_ERROR)
			{
				iError = WSAGetLastError();
			}
			if (strlen(m_cLogFileName))
			{
				CString cString;
				if (iError)
					cString.Format("Error on send (trigger exclude) to: %s\n",szForwardAddress);
				else
					cString.Format("Ok on send (trigger exclude) to: %s\n",szForwardAddress);
				Log(false,(LPSTR)LPCSTR(cString));
			}
		}
		catch(...)
		{
		}
		i++;
		sprintf_s(szI,"A%d",i);
		szForwardAddress[0] = NULL;
		GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
	}
	return 0;
	///*
	//int iLocation = (int)lParam;
	//if (iLocation <= EXCLUDE_SIZE)
	//{
	//	m_bTriggerExclude[iLocation] = true;
	//	CString cString;
	//	cString.Format("LOCATION%d",iLocation);
	//	char szLocation[64];
	//	int iDuration = GetPrivateProfileInt(PUSH_XML,"EXCLUDEDURATION_MS",2000,m_cIniFile);
	//	GetPrivateProfileString(PUSH_XML,cString,"",szLocation,sizeof(szLocation),m_cIniFile);
	//	SetTimer(LOCATION_BASE+iLocation,iDuration,NULL);
	//	CString cTemp;
	//	cTemp.Format("Trigger Exclusion: %d %s\n",iLocation,szLocation);
	//	Log(false,(LPSTR)LPCSTR(cTemp));
	//}
	//else
	//	Log(false,"Location index too large -- exclude command ignored\n");
	//*/
}

LRESULT CTrackerDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	SendDataType SendData;

	if (wParam != m_sd_dg) return 0;

	if (WSAGETSELECTEVENT(lParam) == FD_READ)
	{
		int iRc;
		if ((iRc = WSAGETSELECTERROR(lParam))!= 0)
		{
//			MessageBox("Error on FD_READ event message.","Error");
			return 0;
		}
		int iAddrLen = sizeof(m_sAddr_Rem);
		
		iRc = recvfrom(m_sd_dg,(char *)&SendData,sizeof(SendData),
			0,(struct sockaddr FAR *)&m_sAddr_Rem,&iAddrLen);

		if (iRc == SOCKET_ERROR)
		{
//			MessageBox("Error on recvfrom().","Error");
			return 0;
		}

		if	(
			(SendData.BBM == -1) &&
			(SendData.Index == -1) &&
			(SendData.NumButtons == -1) &&
			(SendData.Base == -1)
			)
		{
			if (m_bAllowForward)
			{
				//add address and time to requester list
				int i = 0;
				bool found = false;
				CString temp;
				temp.Format("%d.%d.%d.%d",
					m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b1,
					m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b2,
					m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b3,
					m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b4);
				//don't register myself
				if (temp == m_szIPAddress)
					return 0;
				while ((i < MAX_FORWARD) && !found)
				{
					if (m_sForwardAddress[i])
						if (temp == m_sForwardAddress[i]) 
							found = true;
					if (!found)
						i++;
				}

				if (!found)
				{
					i = 0;
					while ((i < MAX_FORWARD) && (m_sForwardTime[i].GetStatus()==COleDateTime::valid))
						i++;
				}

				if (i < MAX_FORWARD)
				{
					m_sForwardTime[i] = COleDateTime::GetCurrentTime();
					m_sForwardTime[i].SetStatus(COleDateTime::valid);
					m_sForwardAddress[i] = temp;
				}
			}
		}
		else
		{
			//if first packet and we don't have one set then
			//save and use this address as the qualified sender
			if ((m_b1==0) && (m_b2==0) &&(m_b3==0) && (m_b4==0))
			{
			   char szTemp[32];
			   m_b1 = m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b1;
			   m_b2 = m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b2;
			   m_b3 = m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b3;
			   m_b4 = m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b4;
			   sprintf_s(szTemp,"%d.%d.%d.%d",m_b1,m_b2,m_b3,m_b4);
			   WritePrivateProfileString("CONFIGURATION","ADDRESS",szTemp,m_cIniFile);
			}

			//is message from un-expected source
			if ((m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b1 != m_b1) ||
				(m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b2 != m_b2) ||
				(m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b3 != m_b3) ||
				(m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b4 != m_b4))
			{
				char szTemp[32];
				sprintf_s(szTemp,"%d.%d.%d.%d",
					m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b1,
					m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b2,
					m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b3,
					m_sAddr_Rem.sin_addr.S_un.S_un_b.s_b4);
				WritePrivateProfileString("CONFIGURATION","BADADDRESS",szTemp,m_cIniFile);
				return 0;
			}

			if (SendData.NumButtons != m_dNumButtons)
			{
				//delete all existing buttons
				for (int i = 0; i < m_dNumButtons; i++)
				{
					if (Buttons[i])
					{
						delete Buttons[i];
						Buttons[i] = NULL;
					}
				}
				m_dNumButtons = 0;

				//create new buttons
				for (int i = 0; i < SendData.NumButtons; i++)
				{
					//create button
					Buttons[i] = new CColorButton(false);
					CRect Rect(0,0,0,0);
					ScreenToClient(&Rect);
					Buttons[i]->Create("Unknown",
						WS_CHILD | WS_TABSTOP | WS_VISIBLE |
						BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
						Rect,this,BUTTONBASE+i);
					Buttons[i]->SetBase(0,false);
					m_dNumButtons++;
				}
				Reorg();
			}

			if ((SendData.Index >=0 ) && (SendData.Index < m_dNumButtons))
			{
//				m_dTimeSinceLastMsg = 0;
				//Log(false,"");

				if (Buttons[SendData.Index]->GetFaceColor() != RED &&
					SendData.FaceColor == RED)
					m_bAcked = false;

				m_sLastGoodMessageTime = COleDateTime::GetCurrentTime();
				Buttons[SendData.Index]->SetWindowText(SendData.Title);
				Buttons[SendData.Index]->SetPreNum(SendData.PreNum,false);
				Buttons[SendData.Index]->SetNumeric(SendData.BBM,false);
				Buttons[SendData.Index]->SetFaceColor(SendData.FaceColor,false);
				Buttons[SendData.Index]->SetFlash(SendData.Flash);
				Buttons[SendData.Index]->SetFlag(SendData.Flag,false);
				Buttons[SendData.Index]->SetBell(SendData.Bell,false);
				Buttons[SendData.Index]->SetTime(SendData.Time,false);
				Buttons[SendData.Index]->SetOthr(SendData.Other,false);
				Buttons[SendData.Index]->SetDisk(SendData.Disk,false);
				Buttons[SendData.Index]->SetQuest(SendData.Quest,false);
				Buttons[SendData.Index]->SetBellChar(SendData.BellC,false);
				Buttons[SendData.Index]->SetTimeChar(SendData.TimeC,false);
				Buttons[SendData.Index]->SetOthrChar(SendData.OtherC,false);
				Buttons[SendData.Index]->SetDiskChar(SendData.DiskC,false);
				Buttons[SendData.Index]->SetFlagChar(SendData.FlagC,false);
				Buttons[SendData.Index]->SetQuestChar(SendData.QuestC,false);
				Buttons[SendData.Index]->SetBase(SendData.Base,true);
			}

			//step through forward list and send packet to each
			if (m_bAllowForward)
			{
				m_sAddr_Tmp.sin_family = PF_INET;
				m_sAddr_Tmp.sin_port = (unsigned short)(htons((unsigned short)m_dDefPort));
				int i = 0;
				while (i < MAX_FORWARD)
				{
					if (m_sForwardTime[i].GetStatus()==COleDateTime::valid)
					{
						m_sAddr_Tmp.sin_addr.s_addr = inet_addr(LPCSTR(m_sForwardAddress[i]));
						try
						{
							sendto(m_sd_dg,(char*)&SendData,sizeof(SendData),0,
								(struct sockaddr FAR *) &m_sAddr_Tmp,sizeof(m_sAddr_Tmp));
						}
						catch(...)
						{
						}
						if ((COleDateTime::GetCurrentTime() - m_sForwardTime[i]) > COleDateTimeSpan(0,0,5,0))
							m_sForwardTime[i].SetStatus(COleDateTime::invalid);
						//CString sToAddress;
					}
					i++;
				}

				i = 0;
				char szI[8];
				char szForwardAddress[32];
				_itoa_s(i,szI,10);
				szForwardAddress[0] = NULL;
				GetPrivateProfileString(PUSH,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
				while (strlen(szForwardAddress))
				{
					m_sAddr_Tmp.sin_addr.s_addr = inet_addr(LPCSTR(szForwardAddress));
					try
					{
						if (sendto(m_sd_dg,(char*)&SendData,sizeof(SendData),0,
							(struct sockaddr FAR *) &m_sAddr_Tmp,sizeof(m_sAddr_Tmp))==SOCKET_ERROR)
						{
							int  iError = WSAGetLastError();
						}
					}
					catch(...)
					{
					}
				
					_itoa_s(++i,szI,10);
					szForwardAddress[0] = NULL;
					GetPrivateProfileString(PUSH,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
				}
/*
				if (GetPrivateProfileInt(PUSH_XML,"SENDSTATUS",0,m_cIniFile)==1)
				{
					i = 0;
					int iPort = 0;
					char szPort[16];
					sprintf_s(szI,"A%d",i);
					szForwardAddress[0] = NULL;
					GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
					while (strlen(szForwardAddress))
					{
						sprintf_s(szPort,"P%d",i);
						iPort = GetPrivateProfileInt(PUSH_XML,szPort,0,m_cIniFile);
						for (int i=0; i < m_dNumButtons; i++)
						{
							m_sAddr_Tmp.sin_family = PF_INET;
							m_sAddr_Tmp.sin_addr.s_addr = inet_addr(LPCSTR(szForwardAddress));
							m_sAddr_Tmp.sin_port= htons(iPort);
							try
							{
								char szXML[1400];
								char cTemp[64];
								Buttons[i]->GetWindowText(cTemp,sizeof(cTemp));
								int iSizeXML = FormatXML(szXML,cTemp, i, i);
								if (sendto(m_sd_dg,(char*)&szXML,iSizeXML+1,0,
									(struct sockaddr FAR *) &m_sAddr_Tmp,sizeof(m_sAddr_Tmp))==SOCKET_ERROR)
								{
									int  iError = WSAGetLastError();
								}
								if (strlen(m_cLogFileName))
									Log(false,"Status Sent");
		#ifdef DEBUGXML
								//test to demonstrate we are sending 
								FILE*temp=fopen("C:\\XML.xml","wt");
								if (temp)
								{
									fprintf(temp,szXML);
									fclose(temp);
								}
		#endif				
							}
							catch(...)
							{
							}
							i++;
							sprintf_s(szI,"A%d",i);
							szForwardAddress[0] = NULL;
							GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
						}
					}
				}
*/
			}
		}
	}
	if (m_bMoved)
	{
		m_bMoved = false;
		Reorg();
	}

	return 0;
}

LRESULT CTrackerDlg::OnTrigger(WPARAM, LPARAM LParam)
{
	int iPort = 0;
	char szPort[16];
	char szI[16];
	char szForwardAddress[32];
	szForwardAddress[0] = NULL;
	char szTemp[64];
	char szSource[64];

	sprintf_s(szTemp,"LOCATION%d",(int)LParam);
	GetPrivateProfileString(PUSH_XML,szTemp,"UNKNOWN",szSource,sizeof(szSource),m_cIniFile);
	char szXML[1400];
	int iSizeXML = FormatXML(szXML,szSource,"",ALARM,0);
#ifdef DEBUGXML
	//test to demonstrate we are sending 
	FILE*temp=fopen("C:\\XML.xml","wt");
	if (temp)
	{
		fprintf(temp,szXML);
		fclose(temp);
	}
#endif
	if (strlen(m_cLogFileName))
	{
		Log(false,szXML);
	}
				
	if (m_bPushXML)
	{
		int i = 0;
		sprintf_s(szI,"A%d",i);
		GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
		while (strlen(szForwardAddress))
		{
			sprintf_s(szPort,"P%d",i);
			iPort = GetPrivateProfileInt(PUSH_XML,szPort,0,m_cIniFile);
			m_sAddr_Tmp.sin_family = PF_INET;
			m_sAddr_Tmp.sin_addr.s_addr = inet_addr(LPCSTR(szForwardAddress));
			m_sAddr_Tmp.sin_port= htons(iPort);
			try
			{
				int iError = 0;
				if (sendto(m_sd_dg,(char*)&szXML,iSizeXML,0,
					(struct sockaddr FAR *) &m_sAddr_Tmp,sizeof(m_sAddr_Tmp)) == SOCKET_ERROR)
				{
					iError = WSAGetLastError();
				}
				if (strlen(m_cLogFileName))
				{
					//CString cString;
					//cString = "Alarm from ";
					//cString += szTemp;
					//Log(false,(LPSTR)LPCSTR(cString));
					CString cString;
					if (iError)
						cString.Format("Error on send (Alarm Trigger) to: %s\n",szForwardAddress);
					else
						cString.Format("Ok on send (Alarm Trigger) to: %s\n",szForwardAddress);
					Log(false,(LPSTR)LPCSTR(cString));
				}
			}
			catch(...)
			{
			}
			i++;
			sprintf_s(szI,"A%d",i);
			szForwardAddress[0] = NULL;
			GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
		}
	}
	else
		Log(false,"Alarm Received but PUSH XML=No in ini file.\n");

	return 0;
}

LRESULT CTrackerDlg::OnTriggerClr(WPARAM, LPARAM)
{
		return 0;
}

int
CTrackerDlg::FormatXML(char * szXML, char* szSource, char* szMediaName, int iType, int iButtonIndex)
{
	char szString[64];
	CString cString;
	strcpy_s(szString,"Short");
	GetPrivateProfileString(PUSH_XML,"FORM","Short",szString,sizeof(szString),m_cIniFile);
	if (toupper(szString[0])=='S')//short form
	{
		switch (iType) {
		case ALARM:
			cString = "LOCATION\t";
			cString += szSource;
			break;
		case STATUS:
			cString = "STATUS\t";
			cString += szSource;
			break;
		case IMAGE:
			cString = "CAMERA\t";
			cString += szMediaName;
			break;
		case INHIBIT:
			cString = "INHIBIT\t";
			cString += szSource;
			break;
		case HEARTBEAT:
			{
				cString = "HEARTBEAT\t";
				cString += szSource;
				cString += "\t";
				for (int i = 0; i < m_dNumButtons; i++)
				{
					Buttons[i]->GetWindowText(szString,sizeof(szString)-1);
					cString += szString;
					cString += "=";
					COLORREF sFaceColor = Buttons[i]->GetFaceColor();
					if (sFaceColor == RGB(255,0,0))
						cString += "RED";
					Buttons[i]->GetFlags(szSource);
					cString += "{";
					cString += szSource;
					cString += "};";
				}
			}
			break;
		default:
			cString = "UNKNOWN";
		}
		cString += "\n";
	}
	else if (toupper(szString[0]) == 'K')//king's bay
	{
		cString = "<?xml version=\"1.0\"?>\n";
		cString += "<EventReport>\n";
		cString += "\t<ReportDate>";
		SYSTEMTIME SystemTime;

		GetSystemTime(&SystemTime);   // system time
		COleDateTime cNow(SystemTime);
		cString += cNow.Format("%Y%b%d");
		cString += "</ReportDate>\n";

		cString += "\t<ReportTime>\n";
		cString += cNow.Format("%H%M%S");
		cString += "</ReportTime>\n";

		cString += "<\t<EventSite>\n";

		cString += "\t\t<Name1>";
		GetPrivateProfileString(PUSH_XML,"FACILITY","FACILITY",szString,sizeof(szString),m_cIniFile);
		cString += szString;
		cString += "</Name1>\n";

		cString += "\t\t<Name2>";
		GetPrivateProfileString(PUSH_XML,"SITE","SITE",szString,sizeof(szString),m_cIniFile);
		cString += szString;
		cString += "</Name2>\n";
/*
		cString += "\t\t<Lat>";
		GetPrivateProfileString(PUSH_XML,"LAT","000.000000",szString,sizeof(szString),m_cIniFile);
		cString += szString;
		cString += "</Lat>\n";

		cString += "\t\t<Lon>";
		GetPrivateProfileString(PUSH_XML,"Lon","00.000000",szString,sizeof(szString),m_cIniFile);
		cString += szString;
		cString += "</Lon>\n";

		cString += "\t\t<LatLonUnits>";
		GetPrivateProfileString(PUSH_XML,"Lon","degree",szString,sizeof(szString),m_cIniFile);
		cString += szString;
		cString += "</LatLonUnits>\n";
*/
		cString += "\t</EventSite>\n";

		cString += "\t<Type>";
		switch (iType) {
		case ALARM:
			cString += "ALARM";
			break;
		case STATUS:
			cString += "STATUS";
			break;
		case IMAGE:
			cString += "ALARM";
			break;
		case INHIBIT:
			cString += "INHIBIT ALARM";
			break;
		default:
			cString += "UNKNOWN";
		}
		cString += "</Type>\n";


		cString += "\t<Action>";
		switch (iType) {
		case ALARM:
			cString += "ALARM";
			break;
		case STATUS:
			cString += "STATUS";
			break;
		case IMAGE:
			cString += "ALARM";
			break;
		case INHIBIT:
			cString += "INHIBIT ALARM";
			break;
		default:
			cString += "UNKNOWN";
		}
		cString += "</Action>\n";


		switch(iType) {
		case ALARM:
			cString += "\t<SensorReports>1</SensorReports>\n";
			cString += "\t<SensorData>\n";
			cString == "\t</SensorData>\n";
			break;
		case STATUS:
			cString += "\t<SensorReports>1</SensorReports>\n";
			cString += "\t<SensorData>\n";
			cString == "\t</SensorData>\n";
			break;
		case IMAGE:
			cString += "\t<SensorReports>1</SensorReports>\n";
			cString += "\t<SensorData>\n";
			cString == "\t</SensorData>\n";
			break;
		case INHIBIT:
			cString += "\t<SensorReports>1</SensorReports>\n";
			cString += "\t<SensorData>\n";
			cString == "\t</SensorData>\n";
			break;
		}
	}
	else if (toupper(szString[0]) == 'F')//fort lajune
	{
	}
	else if (toupper(szString[0]) == 'C')//camp lennard wood
	{

		SYSTEMTIME SystemTime;

		GetSystemTime(&SystemTime);   // system time
		COleDateTime cNow(SystemTime);
		cString += cNow.Format("\n\tdateTime=\"%Y-%m-%dT%H:%M:%SZ\"");
		szString[0] = NULL;
		GetPrivateProfileString(PUSH_XML,"FACILITY","FACILITY",szString,sizeof(szString),m_cIniFile);
		cString += "\n\tfacility=\"";
		cString += szString;
		cString += "\"";

		szString[0] = NULL;
		GetPrivateProfileString(PUSH_XML,"SITE","SITE",szString,sizeof(szString),m_cIniFile);
		cString += "\n\tsite=\"";
		cString += szString;
		cString += "\"";

		cString += ">";

		cString += "\n\t<SensorData";
		cString += cNow.Format("\n\t\tdateTime=\"%Y-%m-%dT%H:%M:%SZ\"");

		szString[0] = NULL;
		GetPrivateProfileString(PUSH_XML,"OEMID","OEMID",szString,sizeof(szString),m_cIniFile);
		cString += "\n\t\toemid=\"";
		cString += szString;
		cString += "\"";

		szString[0] = NULL;
		GetPrivateProfileString(PUSH_XML,"MODEL","MODEL",szString,sizeof(szString),m_cIniFile);
		cString += "\n\t\tmodel=\"";
		cString += szString;
		cString += "\"";

		int iInt = GetPrivateProfileInt(PUSH_XML,"SERIALNUM",0,m_cIniFile);
		sprintf_s(szString,"%d",iInt);
		cString += "\n\t\tserial=\"";
		cString += szString;
		cString += "\"";

		cString += "\n\t\tlocation=\"";
		cString += szSource;
		cString += "\"";
		cString += ">";

		cString += "\n\t\t<Result";
		szString[0] = NULL;
		GetPrivateProfileString(PUSH_XML,"CONFIDENCE","1.00",szString,sizeof(szString),m_cIniFile);
		cString += " confidence=\"";
		cString += szString;
		cString += "\"";

		cString += " status=\"";
		switch (iType) {
		case ALARM:
			cString += "ALARM\">";
			cString += "\n\t\t\tRadiation Detected";
			break;
		case STATUS:
			cString += "STATUS\">";
			break;
		case IMAGE:
			cString += "ALARM\">";
			cString += "\n\t\t\tImage Triggered";
			break;
		case INHIBIT:
			cString += "INHIBIT ALARM\">";
			break;
		default:
			cString += "UNKNOWN\">";
			cString += "\n\t\tUNKNOWN";
		}
		 
		cString += "\n\t\t</Result>";
		
		if (iType == IMAGE)
		{
			cString += "\n\t\t\t<Media mimetype=\"image.jpg\" description=\"";
			szString[0] = NULL;
			GetPrivateProfileString(PUSH_XML,"MEDIA_DISCRIPTION","description",szString,sizeof(szString),m_cIniFile);
			cString += szString;
			cString += "\">";
			cString += "\n\t\t\t\t<Location type=\"HTTP\" server=\"";
			szString[0] = NULL;
			GetPrivateProfileString(PUSH_XML,"SERVERNAME","SERVERNAME",szString,sizeof(szString),m_cIniFile);
			cString += szString;
			cString += "\" path=\"";
			szString[0] = NULL;
			GetPrivateProfileString(PUSH_XML,"SERVERPATH","SERVERPATH",szString,sizeof(szString),m_cIniFile);
			cString += szString;
			if ((szMediaName[0] != '\\') && (szMediaName[0] != '/'))
				if ((szString[strlen(szString)-1] != '\\') && (szString[strlen(szString)-1] != '/'))
					cString += "/";
			cString += szMediaName;
			cString += "\" />";
			cString += "\n\t\t\t</Media>";
		}
		else if (iType == STATUS)
		{
			
		}

		cString += "\n\t</SensorData>";
		cString += "\n</SensorEvent>";
		cString += "\n";
	}
	strcpy(szXML,cString.GetBuffer());
	return cString.GetLength();
}

void
CTrackerDlg::Reconfigure()
{
	//get rid of any of the timers that are running
	KillTimer(eHTML);
	KillTimer(eMailSlot);
	KillTimer(eTrackToTrack);
	KillTimer(eTrackToTrackLong);

	//get rid of existing buttons
	for (int i=0; i < MAX_BUTTONS; i++)
	{
		if (Buttons[i])
		{
			delete Buttons[i];
			Buttons[i] = NULL;
		}
	}
	m_dNumButtons = 0;

	//get rid of mailslot and socket if we are using it
	if (m_bDoingMailSlot)
	{
		//close the mailslot
		if (m_hMailSlot != INVALID_HANDLE_VALUE)
			CloseHandle(m_hMailSlot);
	}
	else
	{
		closesocket(m_sd_dg);
		WSACleanup();
		//give NT time to really close the socket
		Sleep(500);
	}

	//get mailslot name
	GetPrivateProfileString(CONFIGURATION,"MAILSLOT","",
		m_szSlotName,sizeof(m_szSlotName),m_cIniFile);
	//are we doing mailslot
	m_bDoingMailSlot = GetPrivateProfileInt(CONFIGURATION,"DOMAILSLOT",0,m_cIniFile)==1;

	//get how often to check 
	m_dMSTime = GetPrivateProfileInt(CONFIGURATION,"MAILUPDATE",250,m_cIniFile);
	if (m_dMSTime < 100) m_dMSTime = 100;

	//doing html?
	m_bDoHTML = GetPrivateProfileInt(CONFIGURATION,"DOHTML",0,m_cIniFile)==1;

	//get html cycle time
	m_dHTMLTime = GetPrivateProfileInt(CONFIGURATION,"HTMLTIME",60,m_cIniFile);

	//get the html file name 
	GetPrivateProfileString(CONFIGURATION,"HTMLNAME",m_cName,m_szHTMLName,sizeof(m_szHTMLName),m_cIniFile);
	
	//get the width of each button
	m_dButtonWidth = GetPrivateProfileInt(CONFIGURATION,"BUTTONWIDTH",BUTTONWIDTH,m_cIniFile);

	//get the height of each button
	m_dButtonHeight= GetPrivateProfileInt(CONFIGURATION,"BUTTONHEIGHT",BUTTONHEIGHT,m_cIniFile);

	//get the flag size of each button
	m_dButtonFlagSize = GetPrivateProfileInt(CONFIGURATION,"BUTTONFLAGSIZE",BUTTONFLAGSIZE,m_cIniFile);

	//get the number of columns of buttons
	m_dButtonsWide = GetPrivateProfileInt(CONFIGURATION,"BUTTONSWIDE",BUTTONCOLUMNS,m_cIniFile);

	//get the udp port number
	m_dDefPort = GetPrivateProfileInt(CONFIGURATION,"PORT",IPORT,m_cIniFile);

	//get the facility name
	GetPrivateProfileString(CONFIGURATION,"NAME","Unknown",m_cName,sizeof(m_cName),m_cIniFile);

	//get the logfile name
	GetPrivateProfileString(CONFIGURATION,"LOGFILENAME","c:\\LogFile.txt",m_cLogFileName,sizeof(m_cLogFileName),m_cIniFile);

	char szTemp[32];
	GetPrivateProfileString(CONFIGURATION,"DOALARM","No",szTemp,sizeof(szTemp),m_cIniFile);
	m_bDoAlarm = toupper(szTemp[0]) == 'Y';

	//get the source address
	char *pTemp;
	szTemp[0] = NULL;
	GetPrivateProfileString(CONFIGURATION,"ADDRESS","0.0.0.0",szTemp,sizeof(szTemp),m_cIniFile);
	char *next_token = NULL;
	pTemp = strtok_s(szTemp,".", &next_token);
	m_b1 = atoi(pTemp);
	pTemp = strtok_s(NULL,".", &next_token);
	m_b2 = atoi(pTemp);
	pTemp = strtok_s(NULL,".", &next_token);
	m_b3 = atoi(pTemp);
	pTemp = strtok_s(NULL,".", &next_token);
	m_b4 = atoi(pTemp);

	CString temp;
	temp = "Tracker ";
	temp += m_cName;
	temp += " ";
	temp += m_cVERSION;
	SetWindowText(temp);

	bool bFail = false;
	if (m_bDoingMailSlot)
	{
		//open mailslot
		m_MSName = "\\\\.\\mailslot\\";
		m_MSName += m_szSlotName;
		m_hMailSlot = CreateMailslot(m_MSName,sizeof(SendDataType),0,NULL);
		if (m_hMailSlot == INVALID_HANDLE_VALUE)
			bFail = true;
		SetTimer(eMailSlot,m_dMSTime,NULL);
	}
	else
	{
		m_wVer = MAKEWORD(1,1);
		int iRc = WSAStartup(m_wVer, &m_WSAData);
		if (iRc != 0)
		{
			MessageBox("Error occurred on WSAStartup().","Tracker Error");
			bFail = true;
		}
		if (!bFail)
		{
			if (LOBYTE (m_WSAData.wVersion) != 1 ||
				HIBYTE (m_WSAData.wVersion) != 1) 
			{
				MessageBox("WINSOCK.DLL wrong version.","Tracker Error");
				bFail = true;
			}
		}

		if (!bFail)
		{
			m_sd_dg = socket(PF_INET,SOCK_DGRAM,0);
			if (m_sd_dg == INVALID_SOCKET)
			{
				MessageBox("Error occurred on socket().","Tracker Error");
				bFail = true;
			}
		}

		if (!bFail)
		{
			m_sAddr_Loc.sin_family=PF_INET;
			m_sAddr_Loc.sin_port=htons(m_dDefPort);
			m_sAddr_Loc.sin_addr.s_addr=htonl(INADDR_ANY);
		
			iRc = bind(m_sd_dg, (struct sockaddr far *) &m_sAddr_Loc,sizeof(m_sAddr_Loc));
			if (iRc == SOCKET_ERROR)
			{
				char buff[256];
				sprintf(buff, "Error occurred on bind() to port %d.", m_dDefPort);
				MessageBox(buff,"Tracker Error oxo");
				bFail = true;
			}
		}

		if (!bFail)
		{
			if (!m_bToMIC)
				SetTimer(eTrackToTrack,1000,NULL); // 1 seconds

			iRc = WSAAsyncSelect(m_sd_dg, m_hWnd, WM_WSARECV, FD_READ);
			if (iRc == SOCKET_ERROR)
			{
				MessageBox("Error occurred on WSAAsyncSelect().","Tracker Error");
				bFail = true;
			}
		}
	}

	int dLastX = GetPrivateProfileInt(CONFIGURATION,LAST_X,0,m_cIniFile);
	int dLastY = GetPrivateProfileInt(CONFIGURATION,LAST_Y,0,m_cIniFile);
	
	m_sInitialRect = m_sStartRect;

	m_sInitialRect.left = min(dLastX,
		::GetSystemMetrics(SM_CXSCREEN)-GetSystemMetrics(SM_CXICON));
	m_sInitialRect.top  = min(dLastY,
		::GetSystemMetrics(SM_CYSCREEN)-GetSystemMetrics(SM_CYICON));

	SetWindowPos(&wndTop,
		m_sInitialRect.left,m_sInitialRect.top,
		m_sInitialRect.right,m_sInitialRect.bottom,
		0);

	if (!bFail)
	{
		SetTimer(eHTML,m_dHTMLTime*1000,NULL);
//		m_dTimeSinceLastMsg = 0;

		//tell the user we are waiting for something from mic
		if (m_bDoingMailSlot)
		{
			temp.Format("Waiting for MailSlot Message from MIC on %s",m_szSlotName);
		}
		else
		{
			temp.Format(m_bToMIC?"Waiting for UDP message from MIC on port %d":"Waiting for UDP message from TRACKER on port %d",m_dDefPort);
		}
		SetDlgItemText(IDC_STATIC_WAIT,temp);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_WAIT,"Configuration Failed");
	}
	GetDlgItem(IDC_STATIC_WAIT)->ShowWindow(SW_SHOW);
}

void CTrackerDlg::Log(bool bFlash, char* szText)
{
/*
	if (bFlash != m_bAllFlashing)
	{
		FILE *file;
		file = fopen(m_cLogFileName,"at");
		if(file)
		{
			COleDateTime Now = COleDateTime::GetCurrentTime();
			if (bFlash)
				fprintf(file,"%s: DEAD\n",Now.Format("%A %B %d, %H:%M:%S"));
			else
			{
				fprintf(file,"%s: OK",Now.Format("%A %B %d, %H:%M:%S"));
				if (szText)
					fprintf(file," %s",szText);
				fprintf(file,"\n");
			}
			fclose(file);
		}
		m_bAllFlashing = bFlash;
	}
*/
	errno_t err;
	FILE *file;
	err = fopen_s(&file, m_cLogFileName,"at");
	if(file)
	{
		if (szText)
			fprintf(file," %s\n",szText);
		fclose(file);
	}
}	
	
BOOL CTrackerDlg::OnInitDialog()
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
		CString strConfigMenu("Configure");
		CString strConfig2Menu("Setup Display");
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_CONFIGURE, strConfigMenu);
			pSysMenu->AppendMenu(MF_STRING, IDM_CONFIGURE2, strConfig2Menu);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	m_bAllFlashing = false;
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	for (int i = 0; i < EXCLUDE_SIZE; i++)
		m_bTriggerExclude[i] = false;
	//build the ini file name
	//get full path and name for this instance of application
	GetModuleFileName(AfxGetInstanceHandle(),m_cIniFile,sizeof(m_cIniFile));
	if (m_cCommandLine[0] != NULL)
	{
		char cDrive[_MAX_DRIVE];
		char cPath[_MAX_PATH];
		char cName[_MAX_FNAME];
		char cExt[_MAX_EXT];
		_splitpath_s(m_cIniFile,cDrive,cPath,cName,cExt);
		for (char *p = m_cCommandLine; p < m_cCommandLine + strlen(m_cCommandLine);p++)
			if (islower(*p)) p[0] = _toupper(*p);
		if (strstr(m_cCommandLine,".INI"))
			_makepath_s(m_cIniFile,cDrive,cPath,m_cCommandLine,NULL);
		else
			_makepath_s(m_cIniFile,cDrive,cPath,m_cCommandLine,".INI");
	} 
	else
	{
		char *c = strrchr(m_cIniFile,'.');//find the . in the .exe
		*c = '\0';						//make it null
		strcat_s(m_cIniFile,".INI");
	}
	//if the ini file isn't there create a "default" one
	if (_access(m_cIniFile,0)!=0)
	{
#ifndef SINGLERUN
		errno_t err;
		FILE* temp = NULL;
		err = fopen_s(&temp, m_cIniFile,"wt");
		if (temp)
		{
			fprintf(temp,"[CONFIGURATION]\n");
			fprintf(temp,"NAME=Location\n");
			fprintf(temp,"VERSION=\n");
			fprintf(temp,"ADDRESS=0.0.0.0\n");
			fprintf(temp,"PORT=2020\n");
			fprintf(temp,"ALLOWFORWARDING=N\n");
			fprintf(temp,"TO_MIC=Y\n");
			fprintf(temp,"DOMAILSLOT=0\n");
			fprintf(temp,"MAILSLOT=\n");
			fprintf(temp,"LOGFILENAME=C:\\LogFile.txt\n");
			fprintf(temp,"MAILUPDATE=100\n");
			fprintf(temp,"DOHTML=0\n");
			fprintf(temp,"HTMLNAME=FACILITY\n");
			fprintf(temp,"HTMLTIME=60\n");
			fprintf(temp,"LAST_X=0\n");
			fprintf(temp,"LAST_Y=0\n");
			fprintf(temp,"BUTTONSWIDE=1\n");
			fprintf(temp,"BUTTONWIDTH=200\n");
			fprintf(temp,"BUTTONFLAGSIZE=36\n");
			fprintf(temp,"BUTTONHEIGHT=70\n");
			fprintf(temp,"DOALARM=No\n");
			fprintf(temp,"PUSHXML=No\n");
			fprintf(temp,"\n");

			fprintf(temp,"[BAD]\n");
			fprintf(temp,"ADDRESS=\n");
			fprintf(temp,"SIZE=\n");
			fprintf(temp,"BBM=\n");
			fprintf(temp,"INDEX=\n");
			fprintf(temp,"NUMBUTTONS=\n");
			fprintf(temp,"BASE=\n");
			fprintf(temp,"\n");

			fprintf(temp,"[PUSH XML]\n");
			fprintf(temp,"FORM=Short\n");
//			fprintf(temp,";FORM=Fort Lennard Wood\n");
//			fprintf(temp,";FORM=Camp Lajune\n");
//			fprintf(temp,";FORM=King's Bay\n");
			fprintf(temp,"FTP_SERVER=0.0.0.0\n");
			fprintf(temp,"FTP_USERNAME=\n");
			fprintf(temp,"FTP_PASSWORD=\n");
			fprintf(temp,"FTP_DIR=/home/test\n");
			fprintf(temp,"EXCLUDEDURATION_MS=2000\n");
			fprintf(temp,"GEMCONVERTPATH=c:\\aquila\\gemCNVRT.exe\n");
			fprintf(temp,"IMAGENAME=*.jpg\n");
			fprintf(temp,"IMAGEPATHIN=C:\\images\n");
			fprintf(temp,"IMAGEPATHOUT=c:\\imagesOUT\n");
			fprintf(temp,"SENDSTATUS=0\n");
			fprintf(temp,"FACILITY=Facility Name\n");
			fprintf(temp,"SITE=Site Name\n");
			fprintf(temp,"OEMID=LANL N-4\n");
			fprintf(temp,"MODEL=MiniGRAND\n");
			fprintf(temp,"SERIALNUM=0\n");
			fprintf(temp,"CONFIDENCE=1.00\n");
			fprintf(temp,"MEDIA_DESCRIPTION=Vehicle Picture\n");
			fprintf(temp,"SERVERNAME=0.0.0.0\n");
			fprintf(temp,"SERVERPATH=/home/unwd/test\n");
			fprintf(temp,"CAMERALOCATION=2\n");
			fprintf(temp,"LOCATION1=location1\n");
			fprintf(temp,"LOCATION2=location2\n");
			fprintf(temp,"LOCATION3=location3\n");
			fprintf(temp,"LOCATION4=location4\n");
			fprintf(temp,"A0=10.2.10.1\n");
			fprintf(temp,"P0=2020\n");
			fprintf(temp,"A1=10.2.20.3\n");
			fprintf(temp,"P1=2020\n");
			fprintf(temp,"A2=10.1.20.3\n");
			fprintf(temp,"P2=2020\n");
			fprintf(temp,"A3=10.253.20.50\n");
			fprintf(temp,"P3=2020\n");
			fclose(temp);
		}
#else
		EndDialog(-1);
		m_cIniFile[0] = NULL;
		return false;
#endif
	}

	CVersionInfo VersionInfo;
	CString Name = VersionInfo.InternalName();
	m_cVERSION = VersionInfo.FileVersion(); // he hardcoded the 1, 9, 0, 1 with  ', ' in it as indices into the raw string: incredible!
	// m_cVERSION.Format("%c.%c.%c.%c",m_cVERSION[0],m_cVERSION[3],m_cVERSION[6],m_cVERSION[9]);
	// ^ not necessary because I changed version block to use '.' chars

	char szName[MAX_PATH];
	strcpy_s(m_szDir,"c:\\image");
	GetPrivateProfileString(PUSH_XML,"IMAGEPATHIN","C:\\IMAGEIN",m_szDir,sizeof(m_szDir),m_cIniFile);
	if (m_szDir[strlen(m_szDir)-1] != '\\')
		strcat_s(m_szDir,"\\");
	strcat_s(szName,"*.jpg");
	GetPrivateProfileString(PUSH_XML,"IMAGENAME","*.jpg",szName,sizeof(szName),m_cIniFile);

	strcpy_s(m_szPathName,m_szDir);
	strcat_s(m_szPathName,"*.jpg");

	char szTemp[64];
//	GetPrivateProfileString(PUSH_XML,"GEMCONVERTPATH","C:\\mic\\gemCNVRT.exe",m_szConvertPath,sizeof(m_szConvertPath),m_cIniFile);
	m_szConvertPath[0] = NULL;
	GetPrivateProfileString(PUSH_XML,"GEMCONVERTPATH","",m_szConvertPath,sizeof(m_szConvertPath),m_cIniFile);
	int m_iCameraLocation = GetPrivateProfileInt(PUSH_XML,"CAMERALOCATION",1,m_cIniFile);
	sprintf_s(szTemp,"LOCATION%d",m_iCameraLocation);
	GetPrivateProfileString(PUSH_XML,szTemp,"UNKNOWN",m_szCameraLocation,sizeof(m_szCameraLocation),m_cIniFile);

	m_szPathNameOut[0] = NULL;
	GetPrivateProfileString(PUSH_XML,"IMAGEPATHOUT","C:\\IMAGEOUT",m_szPathNameOut,sizeof(m_szPathNameOut),m_cIniFile);
//	if (strlen(m_szPathNameOut)>0)
	if (m_szPathNameOut[strlen(m_szPathNameOut)-1] != '\\')
		strcat_s(m_szPathNameOut,"\\");

	//tell the world which version we are running
	WritePrivateProfileString(CONFIGURATION,"VERSION",m_cVERSION,m_cIniFile);

	//get mailslot name
	GetPrivateProfileString(CONFIGURATION,MAILSLOT,"",
		m_szSlotName,sizeof(m_szSlotName),m_cIniFile);

	//get what we are connecting to MIC or another TRACKER
	GetPrivateProfileString(CONFIGURATION,"TO_MIC","Y",szTemp,sizeof(szTemp),m_cIniFile);
	m_bToMIC = toupper(szTemp[0])!='N';
	szTemp[0] = NULL;

	GetPrivateProfileString(CONFIGURATION,"PUSHXML","No",szTemp,sizeof(szTemp),m_cIniFile);
	m_bPushXML = toupper(szTemp[0]) == 'Y';
	szTemp[0] = NULL;

	//are we doing mailslot
	m_bDoingMailSlot = GetPrivateProfileInt(CONFIGURATION,DOMAILSLOT,0,m_cIniFile)==1;
	//can only do mailslot if connecting to mic
	if (m_bDoingMailSlot)//!m_bToMIC)
	{
		if (!m_bToMIC)
		{
			m_bToMIC = true;
			WritePrivateProfileString(CONFIGURATION,"TO_MIC","Y",m_cIniFile);
		}
	}

	//get how often to check 
	m_dMSTime = GetPrivateProfileInt(CONFIGURATION,MAILUPDATE,250,m_cIniFile);
	if (m_dMSTime < 100) m_dMSTime = 100;

	//doing html?
	m_bDoHTML = GetPrivateProfileInt(CONFIGURATION,DOHTML,0,m_cIniFile)==1;

	//get html cycle time
	m_dHTMLTime = GetPrivateProfileInt(CONFIGURATION,HTMLTIME,60,m_cIniFile);

	//get the html file name 
	GetPrivateProfileString(CONFIGURATION,HTMLNAME,m_cName,m_szHTMLName,sizeof(m_szHTMLName),m_cIniFile);
	
	//get the width of each button
	m_dButtonWidth = GetPrivateProfileInt(CONFIGURATION,"BUTTONWIDTH",BUTTONWIDTH,m_cIniFile);

	//get the height of each button
	m_dButtonHeight= GetPrivateProfileInt(CONFIGURATION,"BUTTONHEIGHT",BUTTONHEIGHT,m_cIniFile);

	//get the flag size of each button
	m_dButtonFlagSize = GetPrivateProfileInt(CONFIGURATION,"BUTTONFLAGSIZE",BUTTONFLAGSIZE,m_cIniFile);

	//get the number of columns of buttons
	m_dButtonsWide = GetPrivateProfileInt(CONFIGURATION,"BUTTONSWIDE",BUTTONCOLUMNS,m_cIniFile);

	//get the udp port number (mic will send to us on this port)
	m_dDefPort = GetPrivateProfileInt(CONFIGURATION,PORT,IPORT,m_cIniFile);

	//get the ok to do audio alarm
	GetPrivateProfileString(CONFIGURATION,"DOALARM","No",szTemp,sizeof(szTemp),m_cIniFile);
	m_bDoAlarm = toupper(szTemp[0])=='Y';

	//get the allow forwarding flag
	GetPrivateProfileString(CONFIGURATION,ALLOWFORWARDING,"N",szTemp,sizeof(szTemp),m_cIniFile);
	m_bAllowForward = toupper(szTemp[0])=='Y';
	szTemp[0] = NULL;

	//get the facility name
	GetPrivateProfileString(CONFIGURATION,NAME,"Unknown",m_cName,sizeof(m_cName),m_cIniFile);

	//get the log file name
	GetPrivateProfileString(CONFIGURATION,"LOGFILENAME","C:\\LogFile.txt",
		m_cLogFileName, sizeof(m_cLogFileName), m_cIniFile);

	//get the source address
	char *pTemp;
	char *next_token = NULL;

	GetPrivateProfileString(CONFIGURATION,ADDRESS,"0.0.0.0",szTemp,sizeof(szTemp),m_cIniFile);
	m_b1 = m_b2 = m_b3 = m_b4 = 0;
	pTemp = strtok_s(szTemp,".", &next_token);
	if (pTemp)
	{
		m_b1 = atoi(pTemp);
		pTemp = strtok_s(NULL,".", &next_token);
		if (pTemp)
		{
			m_b2 = atoi(pTemp);
			pTemp = strtok_s(NULL,".", &next_token);
			if (pTemp)
			{
				m_b3 = atoi(pTemp);
				pTemp = strtok_s(NULL,".", &next_token);
				if (pTemp)
					m_b4 = atoi(pTemp);
			}
		}
	}
//	}



	CString temp;
	temp = "Tracker ";
	temp += m_cName;
	temp += " ";
	temp += m_cVERSION;
	SetWindowText(temp);

	GetWindowRect(&m_sInitialRect);
	GetWindowRect(&m_sStartRect);

	m_sLastGoodMessageTime = COleDateTime::GetCurrentTime();

	bool bFail = false;
	if (m_bDoingMailSlot)
	{
		//open mailslot
		m_MSName = "\\\\.\\mailslot\\";
		m_MSName += m_szSlotName;
		m_hMailSlot = CreateMailslot(m_MSName,sizeof(SendDataType),0,NULL);
		if (m_hMailSlot != INVALID_HANDLE_VALUE)
			SetTimer(eMailSlot,m_dMSTime,NULL);
		else
		{
			CTimedMessageBox cTimedMB("Tracker Error","Invalid Mailslot name!",20,this);
//			MessageBox("Invalid Mailslot name!","Tracker Error");
			bFail = true;
		}
	}
	
	{
		m_wVer = MAKEWORD(1,1);
		int iRc = WSAStartup(m_wVer, &m_WSAData);
		if (iRc != 0)
		{

			CTimedMessageBox cTimedMB("Tracker Error","Error occurred on WSAStartup().",20,this);
//			MessageBox("Error occurred on WSAStartup().","Tracker Error");
			bFail = true;
		}
		if (!bFail)
		{
			if (LOBYTE (m_WSAData.wVersion) != 1 ||
				HIBYTE (m_WSAData.wVersion) != 1) 
			{
				CTimedMessageBox cTimedMB("Tracker Error","WINSOCK.DLL wrong version.",20,this);
//				MessageBox("WINSOCK.DLL wrong version.","Tracker Error");
				bFail = true;
			}
		}

		if (!bFail)
		{
			m_sd_dg = socket(PF_INET,SOCK_DGRAM,0);
			if (m_sd_dg == INVALID_SOCKET)
			{
				CTimedMessageBox cTimedMB("Tracker Error","Error occurred on socket().",20,this);
//				MessageBox("Error occurred on socket().","Tracker Error");
				bFail = true;
			}
		}

		if (!bFail)
		{
			m_sAddr_Loc.sin_family=PF_INET;
			m_sAddr_Loc.sin_port=htons(m_dDefPort);
			m_sAddr_Loc.sin_addr.s_addr=htonl(INADDR_ANY);
		
			iRc = bind(m_sd_dg, (struct sockaddr far *) &m_sAddr_Loc,sizeof(m_sAddr_Loc));
			if (iRc == SOCKET_ERROR)
			{
				char buff[256];
				sprintf(buff, "Error occurred on bind() to port %d.", m_dDefPort);
				CTimedMessageBox cTimedMB("Tracker Error",buff,20,this);
//				MessageBox("Error occurred on bind().","Tracker Error");
				bFail = true;
			}
		}
		if (!bFail)
		{
			iRc = WSAAsyncSelect(m_sd_dg, m_hWnd, WM_WSARECV, FD_READ);
			if (iRc == SOCKET_ERROR)
			{
				CTimedMessageBox cTimedMB("Tracker Error","Error occurred on WSAAsyncSelect().",20,this);
//				MessageBox("Error occurred on WSAAsyncSelect().","Tracker Error");
				bFail = true;
			}
		}
	}
	m_bMoved = false;

	int iRc;
	char szLocName[255];
	try 
	{
		iRc = gethostname(szLocName,sizeof(szLocName));
	}
	catch(...)
	{
		iRc = SOCKET_ERROR;
	}
	if (iRc == SOCKET_ERROR)
		strcpy_s(m_szIPAddress,"0.0.0.0");
	else
	{
		LPHOSTENT lphp;
		struct in_addr in_addrIP;
		lphp = gethostbyname(szLocName);
		if (lphp)
		{
			in_addrIP = *(struct in_addr far *)(lphp->h_addr);
			strcpy_s(m_szIPAddress,inet_ntoa(in_addrIP));
		}
		else
			strcpy_s(m_szIPAddress,"0.0.0.0");
	}

	int dLastX = GetPrivateProfileInt(CONFIGURATION,LAST_X,0,m_cIniFile);
	int dLastY = GetPrivateProfileInt(CONFIGURATION,LAST_Y,0,m_cIniFile);
	m_sInitialRect.left = min(dLastX,
		::GetSystemMetrics(SM_CXSCREEN)-GetSystemMetrics(SM_CXICON));
	m_sInitialRect.top  = min(dLastY,
		::GetSystemMetrics(SM_CYSCREEN)-GetSystemMetrics(SM_CYICON));
	SetWindowPos(&wndTop,
		m_sInitialRect.left,m_sInitialRect.top,
		0,0,
		SWP_NOSIZE);

	if (!bFail)
	{
		SetTimer(eHTML,m_dHTMLTime*1000,NULL);

		//tell the user we are waiting for something from mic
		if (m_bDoingMailSlot)
			temp.Format("Waiting for MailSlot Message from MIC on %s",m_szSlotName);
		else
		{
			temp.Format(m_bToMIC?"Waiting for UDP message from MIC on port %d":"Waiting for UDP message from TRACKER on port %d",m_dDefPort);
		}
		SetDlgItemText(IDC_STATIC_WAIT,temp);

		//if we aren't talking to MIC then we need to remind 
		//tracker-server every 1 minute that we need it to 
		//send us packets.  timer 3 sends now and starts
		//timer 4 on 1 min cycle
		if (!m_bToMIC)
			SetTimer(eTrackToTrack,1000,NULL); // 1 second

		SetTimer(eBeepAlarm,2000,NULL); // 2 seconds

		SetTimer(eFTPImages,1200,NULL);  // 1.2 seconds

		SetTimer(ePushXML,60000,NULL); // 60 seconds
	}
	else
	{
		SetDlgItemText(IDC_STATIC_WAIT,"Configuration Failed");
		CTimedMessageBox cTimedMB("Tracker Error","Configuration Failed, Ok to continue...",20,this);
		if (cTimedMB.m_iResult != MB_OK)
			EndDialog(-1);
	}
//		CTimedMessageBox cTimedMB("Tracker Error","Configuration Failed, Ok to continue...",20,this);
//		if (cTimedMB.m_iResult != MB_OK)
//			EndDialog(-1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTrackerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == IDM_CONFIGURE2)
	{
		CConfig2 dlgConfig(this);
		dlgConfig.SetValues(m_dButtonWidth,m_dButtonHeight,m_dButtonsWide,m_dButtonFlagSize);
		if (dlgConfig.DoModal()==IDOK)
		{
			char temp[16];
			dlgConfig.GetValues(&m_dButtonWidth,&m_dButtonHeight,&m_dButtonsWide,&m_dButtonFlagSize);
			_itoa_s(m_dButtonWidth,temp,10);
			WritePrivateProfileString(CONFIGURATION,"BUTTONWIDTH",temp,m_cIniFile);
			_itoa_s(m_dButtonHeight,temp,10);
			WritePrivateProfileString(CONFIGURATION,"BUTTONHEIGHT",temp,m_cIniFile);
			_itoa_s(m_dButtonsWide,temp,10);
			WritePrivateProfileString(CONFIGURATION,"BUTTONSWIDE",temp,m_cIniFile);
			_itoa_s(m_dButtonFlagSize,temp,10);
			WritePrivateProfileString(CONFIGURATION,"BUTTONFLAGSIZE",temp,m_cIniFile);
			Reorg();
		}
	}
	else if (nID == IDM_CONFIGURE)
	{
		CConfig dlgConfig(m_cIniFile,this);
		if (dlgConfig.DoModal()==IDOK)
		{
			CString temp;
			
			//save name from dlg
			WritePrivateProfileString(CONFIGURATION,NAME,dlgConfig.m_sName,m_cIniFile);

			//save logfile
			WritePrivateProfileString(CONFIGURATION,"LOGFILENAME",dlgConfig.m_cLogName,m_cIniFile);
			strcpy_s(m_cLogFileName,dlgConfig.m_cLogName);

			//save allow forwarding or not
			WritePrivateProfileString(CONFIGURATION,ALLOWFORWARDING,dlgConfig.m_bAllowForward?"Y":"N",m_cIniFile);
			m_bAllowForward = dlgConfig.m_bAllowForward == TRUE;

			//save talk to MIC or to another TRACKER
			WritePrivateProfileString(CONFIGURATION,"TO_MIC",dlgConfig.m_bToMIC?"Y":"N",m_cIniFile);
			if (!m_bToMIC)
				KillTimer(eTrackToTrack);
			m_bToMIC = (dlgConfig.m_bToMIC == TRUE);

			//udp factors
			temp.Format("%d.%d.%d.%d",dlgConfig.m_dUDP1,dlgConfig.m_dUDP2,dlgConfig.m_dUDP3,dlgConfig.m_dUDP4);
			WritePrivateProfileString(CONFIGURATION,ADDRESS,temp,m_cIniFile);
			temp.Format("%d",dlgConfig.m_dUDPPort);
			WritePrivateProfileString(CONFIGURATION,PORT,temp,m_cIniFile);

			//html factors
			WritePrivateProfileString(CONFIGURATION,DOHTML,(dlgConfig.m_bDoHTML)?"1":"0",m_cIniFile);
			temp.Format("%d",dlgConfig.m_dHTMLTime);
			WritePrivateProfileString(CONFIGURATION,HTMLTIME,temp,m_cIniFile);
			WritePrivateProfileString(CONFIGURATION,HTMLNAME,dlgConfig.m_szHTMLName,m_cIniFile);
		
			//mailslot factors
			temp.Format("%d",dlgConfig.m_dMSTime);
			WritePrivateProfileString(CONFIGURATION,MAILUPDATE,temp,m_cIniFile);
			WritePrivateProfileString(CONFIGURATION,MAILSLOT,dlgConfig.m_szMailSlotName,m_cIniFile);
			WritePrivateProfileString(CONFIGURATION,DOMAILSLOT,(dlgConfig.m_bDoUDP)?"0":"1",m_cIniFile);
			if (!dlgConfig.m_bDoUDP)
			{
				if (!m_bToMIC)
				{
					m_bToMIC = true;
					WritePrivateProfileString(CONFIGURATION,"TO_MIC","Y",m_cIniFile);
				}
			}

			//audio alarm
			temp.Format("%s",dlgConfig.m_bDoAlarm?"Yes":"No");
			WritePrivateProfileString(CONFIGURATION,"DOALARM",temp,m_cIniFile);

			Reconfigure();

		}
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTrackerDlg::OnPaint() 
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
HCURSOR CTrackerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//reorganize the instrument buttons in the dialog
void CTrackerDlg::Reorg() 
{
	//for each existing button move it where it now belongs
	WINDOWPLACEMENT lpwndpl;	
	GetWindowPlacement( &lpwndpl );
	CRect Rect = m_sInitialRect;

	//get the width of each button
	m_dButtonWidth = GetPrivateProfileInt(CONFIGURATION,"BUTTONWIDTH",BUTTONWIDTH,m_cIniFile);

	//get the height of each button
	m_dButtonHeight= GetPrivateProfileInt(CONFIGURATION,"BUTTONHEIGHT",BUTTONHEIGHT,m_cIniFile);

	//get the flag size of each button
	m_dButtonFlagSize = GetPrivateProfileInt(CONFIGURATION,"BUTTONFLAGSIZE",BUTTONFLAGSIZE,m_cIniFile);

	//get the number of columns of buttons
	m_dButtonsWide = GetPrivateProfileInt(CONFIGURATION,"BUTTONSWIDE",BUTTONCOLUMNS,m_cIniFile);

	if (m_dButtonWidth == 0) m_dButtonWidth = Rect.right - Rect.left + 8;
	if (m_dButtonHeight== 0) m_dButtonHeight= Rect.bottom- Rect.top;

	//figure where each button must go
	if (m_dNumButtons > 0)
	{

	  GetDlgItem(IDC_STATIC_WAIT)->ShowWindow(SW_HIDE);

	  CRect current_rect = m_sInitialRect;	
	  
      int row = 0;
	  int column = 0;
	  int maxcolumn = 0;
	  int maxrow = 0;
	  //step through list of instruments
	  for (int instrument = 0; instrument < m_dNumButtons; instrument ++, column++ ) 
	  {
        //if at the first in a row make room for a row of buttons  
		if ((instrument % m_dButtonsWide)==0) 
		{
			if (instrument != 0) row++;	//bump the row if we aren't on row 0
			column = 0;					//always start in column 0 
		}

		//move the buttons and show them
		if (Buttons[instrument])
		{
			Buttons[instrument]->MoveWindow(
			CRect(column*(m_dButtonWidth+1)+5,
				    row*(m_dButtonHeight+1)+5,
					m_dButtonWidth+column*(m_dButtonWidth+1)+5,
					m_dButtonHeight+row*(m_dButtonHeight+1)),false);
			Buttons[instrument]->SetFlagSize(m_dButtonFlagSize,false);
			Buttons[instrument]->ShowWindow(SW_SHOW);
		}

		//increase width and height of dialog box if we need to
		maxcolumn = max(maxcolumn, column);
		maxrow    = max(maxrow,    row);
		current_rect.right = (m_dButtonWidth+1) * (maxcolumn+1) + 15;
		current_rect.bottom = (m_dButtonHeight+1)* (maxrow   +1) + 30;

		//do all this so that the upper left corner of windows stays put
	    lpwndpl.rcNormalPosition.bottom = lpwndpl.rcNormalPosition.top + current_rect.bottom;
		lpwndpl.rcNormalPosition.right  = lpwndpl.rcNormalPosition.left+ current_rect.right;
		SetWindowPlacement(&lpwndpl);
		//redraw the dialog box
		Invalidate();
	  }

  } //if g_dCount > 0
  else 
  {
		// don't have any instruments to play with yet
	  lpwndpl.rcNormalPosition.bottom = lpwndpl.rcNormalPosition.top+m_sInitialRect.Height();//m_sInitialRect.bottom-m_sInitialRect.Top;
	  lpwndpl.rcNormalPosition.right  = lpwndpl.rcNormalPosition.left+m_sInitialRect.Width();//m_sInitialRect.right-m_sInitialRect.Left;
	  SetWindowPlacement(&lpwndpl);
	  SetDlgItemText(IDC_STATIC_WAIT,"No Instruments Defined");
  }
}

void CTrackerDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
	GetWindowRect(&m_sInitialRect);
	if (strlen(m_cIniFile))
	{
		char temp[16];
		_itoa_s(m_sInitialRect.left,temp,10);
		WritePrivateProfileString(CONFIGURATION,LAST_X,temp,m_cIniFile);
		_itoa_s(m_sInitialRect.top,temp,10);
		WritePrivateProfileString(CONFIGURATION,LAST_Y,temp,m_cIniFile);
	}
	m_bMoved = true;
}


UINT ThreadFunc(LPVOID pParam)
{
	SREQUEST* sRequest = (SREQUEST*)pParam;
	int iResult;
	int iContext = sRequest->iContext;
	char szPathNameToSend[MAX_PATH];
	char szNameOut[MAX_PATH];
	char szFTPServer[MAX_PATH];
	char szFTPUserName[MAX_PATH];
	char szFTPPassword[MAX_PATH];
	char szFTPDir[MAX_PATH];
	DWORD dwAttrs;
	strcpy_s(szPathNameToSend,sRequest->pszPathNameToSend);
	strcpy_s(szNameOut,sRequest->pszNameOut);
	strcpy_s(szFTPServer,sRequest->pszFTPServer);
	strcpy_s(szFTPUserName,sRequest->pszFTPUserName);
	strcpy_s(szFTPPassword,sRequest->pszFTPPassword);
	strcpy_s(szFTPDir,sRequest->pszFTPDir);
	dwAttrs = sRequest->dwAttrs;
	sRequest->bGotData = true;

	iResult = 0;
   try
   {
	   CInternetSession sess(_T("LANL Tracker/1.9.0.0"),iContext);

	   CFtpConnection* pConnect = NULL;

	  if (strlen(szFTPServer))
	  {
			pConnect = sess.GetFtpConnection(szFTPServer,szFTPUserName,szFTPPassword);
	  
			if (strlen(szFTPDir))
				pConnect->SetCurrentDirectory(szFTPDir);
			if (pConnect->PutFile( szPathNameToSend, szNameOut, 
				FTP_TRANSFER_TYPE_BINARY, iContext )==0)
				iResult = -1;
			else
				iResult = 0;

			// if the connection is open, close it
			if (pConnect != NULL)
				pConnect->Close();
			delete pConnect;
	  }
		sess.Close();
   }
   catch(...)
   {
   }
   if (iResult==0)
		SetFileAttributes(szPathNameToSend, dwAttrs & !FILE_ATTRIBUTE_ARCHIVE);

   return iResult;
}

void CTrackerDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if ((nIDEvent >= LOCATION_BASE) && (nIDEvent < (EXCLUDE_SIZE + LOCATION_BASE)))
	{
		m_bTriggerExclude[nIDEvent - LOCATION_BASE] = false;
	}
	else if (nIDEvent == eMailSlot)
	{
		//do mailslot stuff here
		DWORD MaxMessageSize; 
		DWORD NextSize;
		DWORD MessageCount;
		DWORD ReadTimeout;
		if (m_hMailSlot == INVALID_HANDLE_VALUE)
			m_hMailSlot = CreateMailslot(m_MSName,sizeof(SendDataType),0,NULL);

		if (m_hMailSlot != INVALID_HANDLE_VALUE)
		{
			GetMailslotInfo(m_hMailSlot,&MaxMessageSize,&NextSize,&MessageCount,&ReadTimeout);
			//if there is data then read it into SendData
			while (MessageCount > 0)
			{
				if (NextSize == sizeof(SendDataType) )
				{
					SendDataType SendData;
					unsigned long NumberofBytesRead;
					//get the next message and place it into SendData
					ReadFile(m_hMailSlot,&SendData,sizeof(SendDataType),
						&NumberofBytesRead,NULL);

					if (SendData.NumButtons != m_dNumButtons)
					{
						//delete all existing buttons
						for (int i = 0; i < m_dNumButtons; i++)
						{
							if (Buttons[i])
							{
								delete Buttons[i];
								Buttons[i] = NULL;
							}
						}
						m_dNumButtons = 0;

						//create new buttons
						for (int i = 0; i < SendData.NumButtons; i++)
						{
							//create button
							Buttons[i] = new CColorButton(false);
							CRect Rect(0,0,0,0);
							ScreenToClient(&Rect);
							Buttons[i]->Create("Unknown",
								WS_CHILD | WS_TABSTOP | WS_VISIBLE |
								BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
								Rect,this,BUTTONBASE+i);
							Buttons[i]->SetBase(0,false);
							m_dNumButtons++;
						}
						Reorg();
					}
					if ((SendData.Index >=0 ) && (SendData.Index < m_dNumButtons))
					{

						m_sLastGoodMessageTime = COleDateTime::GetCurrentTime();
						Buttons[SendData.Index]->SetWindowText(SendData.Title);
						Buttons[SendData.Index]->SetPreNum(SendData.PreNum,false);
						Buttons[SendData.Index]->SetNumeric(SendData.BBM,false);
						Buttons[SendData.Index]->SetFaceColor(SendData.FaceColor,false);
						Buttons[SendData.Index]->SetFlash(SendData.Flash);
						Buttons[SendData.Index]->SetFlag(SendData.Flag,false);
						Buttons[SendData.Index]->SetBell(SendData.Bell,false);
						Buttons[SendData.Index]->SetTime(SendData.Time,false);
						Buttons[SendData.Index]->SetOthr(SendData.Other,false);
						Buttons[SendData.Index]->SetDisk(SendData.Disk,false);
						Buttons[SendData.Index]->SetFlagChar(SendData.FlagC,false);
						Buttons[SendData.Index]->SetBellChar(SendData.BellC,false);
						Buttons[SendData.Index]->SetTimeChar(SendData.TimeC,false);
						Buttons[SendData.Index]->SetOthrChar(SendData.OtherC,false);
						Buttons[SendData.Index]->SetDiskChar(SendData.DiskC,false);
						Buttons[SendData.Index]->SetFlagChar(SendData.FlagC,false);
						Buttons[SendData.Index]->SetBase(SendData.Base,true);
					}

					if (GetPrivateProfileInt(PUSH_XML,"SENDSTATUS",0,m_cIniFile)==1)
					{
						int i;
						i = 0;
						int iPort = 0;
						char szPort[16];
						char szI[32];
						char szForwardAddress[32];
						szI[0] = NULL;
						szPort[0] = NULL;
						sprintf_s(szI,"A%d",i);
						szForwardAddress[0] = NULL;
						GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
						while (strlen(szForwardAddress))
						{
							
							sprintf_s(szPort,"P%d",i);
							iPort = GetPrivateProfileInt(PUSH_XML,szPort,0,m_cIniFile);
							for (int j = 0; j < m_dNumButtons; j++)
							{
								m_sAddr_Tmp.sin_family = PF_INET;
								m_sAddr_Tmp.sin_addr.s_addr = inet_addr(LPCSTR(szForwardAddress));
								m_sAddr_Tmp.sin_port= (unsigned short)(htons((unsigned short)iPort));
								try
								{
									char szXML[1400];
									char cTemp[64];
									Buttons[j]->GetWindowText(cTemp,sizeof(cTemp));

									int iSizeXML = FormatXML(szXML, cTemp, "", STATUS, j);
									//int iSizeXML = FormatXML(szXML,cTemp,j,j);
									int iError;
									iError = 0;
									if (sendto(m_sd_dg,(char*)&szXML,iSizeXML+1,0,
										(struct sockaddr FAR *) &m_sAddr_Tmp,sizeof(m_sAddr_Tmp)) == SOCKET_ERROR)
									{
										iError = WSAGetLastError();
									}

									if (strlen(m_cLogFileName))
									{
										CString cString;
										if (iError)
											cString.Format("Error on send (Status) to: %s\n",szForwardAddress);
										else
											cString.Format("Ok on send (Status) to: %s\n",szForwardAddress);
										Log(false,(LPSTR)LPCSTR(cString));
									}

		#ifdef DEBUGXML
									//test to demonstrate we are sending 
									FILE*temp=fopen("C:\\XML.xml","wt");
									if (temp)
									{
										fprintf(temp,szXML);
										fclose(temp);
									}
		#endif				
								}
								catch(...)
								{
								}
							}
							i++;
							sprintf_s(szI,"A%d",i);
							szForwardAddress[0] = NULL;
							GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
//							TRACE(szI);
						}
					}
				}
				else
				{
					for (int count = 0; count < (int)NextSize; count++)
					{
						unsigned long BytesRead;
						char dummy;
						ReadFile(m_hMailSlot,&dummy,sizeof(dummy),
							&BytesRead, NULL);
					}
				}
				GetMailslotInfo(m_hMailSlot,&MaxMessageSize,&NextSize,&MessageCount,&ReadTimeout);
			}
		}
		//sometimes we miss a reorg when dragging so correct here
		if ((m_bMoved) && (m_dNumButtons > 0))
		{
			m_bMoved = false;
			Reorg();
		}
		
	}
	else if (nIDEvent == eBeepAlarm)
	{
		if (m_bDoAlarm && !m_bAcked)
			Beep(750,50);
	}

	else if (nIDEvent == ePushXML)
	{
		if (m_bPushXML)
		{

			//PUSH XML
			char szXML[1400];
			char szI[8];
			char szForwardAddress[32];
			char szPort[8];
			char szTemp[64];
			GetPrivateProfileString(PUSH_XML,"LOCATION3","UNKNOWN",
				szTemp,sizeof(szTemp),m_cIniFile);
			int iPort;
			int iSizeXML = FormatXML(szXML, szTemp, 
				NULL, HEARTBEAT, 0);

			int i = 0;
			sprintf_s(szI,"A%d",i);
			GetPrivateProfileString(PUSH_XML,szI,"",
				szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
			while (strlen(szForwardAddress))
			{
				sprintf_s(szPort,"P%d",i);
				iPort = GetPrivateProfileInt(PUSH_XML,szPort,0,m_cIniFile);
				m_sAddr_Tmp.sin_family = PF_INET;
				m_sAddr_Tmp.sin_addr.s_addr = inet_addr(LPCSTR(szForwardAddress));
				m_sAddr_Tmp.sin_port= htons(iPort);
				int iError = 0;
				if (sendto(m_sd_dg,(char*)&szXML,iSizeXML,0,
					(struct sockaddr FAR *) &m_sAddr_Tmp,sizeof(m_sAddr_Tmp)) == SOCKET_ERROR)
					iError = WSAGetLastError();

				if (strlen(m_cLogFileName))
				{
					CString cString;
					if (iError)
						cString.Format("Error on send to: %s Data: %s",szForwardAddress,szXML);
					else
						cString.Format("Ok on send to: %s Data: %s",szForwardAddress,szXML);
					Log(false,(LPSTR)LPCSTR(cString));
				}
				i++;
				sprintf_s(szI,"A%d",i);
				szForwardAddress[0] = NULL;
				GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
			}
		}
	}

	//here if we are going to look for images to send
	else if ((nIDEvent == eFTPImages) && !m_bInSendImages)
	{
		m_bInSendImages = true;
		if (m_bPushXML)
		{
			if (strlen(m_szPathName))
			{

				WIN32_FIND_DATA sFileData;
				HANDLE hFind;
				if ((hFind = FindFirstFile(m_szPathName,&sFileData)) != INVALID_HANDLE_VALUE)
				{
					bool bFinished = false;
					char szI[16];
					char szForwardAddress[32];
					char szPort[8];
					int iPort;

					int iContext = 0;
					struct SREQUEST  sRequest;

					char szFTPServer[MAX_PATH];
					char szFTPUserName[MAX_PATH];
					char szFTPPassword[MAX_PATH];
					char szFTPDir[MAX_PATH];
					szFTPServer[0] = NULL;
					szFTPUserName[0] = NULL;
					szFTPPassword[0] = NULL;
					szFTPDir[0] = NULL;
					sRequest.bGotData = true;
					GetPrivateProfileString(PUSH_XML,"FTP_SERVER",  "",szFTPServer,sizeof(szFTPServer),m_cIniFile);
					GetPrivateProfileString(PUSH_XML,"FTP_USERNAME","",szFTPUserName,sizeof(szFTPUserName),m_cIniFile);
					GetPrivateProfileString(PUSH_XML,"FTP_PASSWORD","",szFTPPassword,sizeof(szFTPPassword),m_cIniFile);
					GetPrivateProfileString(PUSH_XML,"FTP_DIR","",szFTPDir,sizeof(szFTPDir),m_cIniFile);
					sRequest.pszFTPServer = szFTPServer;
					sRequest.pszFTPUserName = szFTPUserName;
					sRequest.pszFTPPassword = szFTPPassword;
					sRequest.pszFTPDir = szFTPDir;
					
					char szPreviousFileName[MAX_PATH];
					szPreviousFileName[0] = NULL;

					int iCrosscheck;
					CString cCrosscheck[100];

					iCrosscheck = 0;

					while (!bFinished) 
					{
						iContext++;
						char szTempPathName[MAX_PATH];
						strcpy_s(szTempPathName, m_szDir); 
						strcat_s(szTempPathName, sFileData.cFileName); 

						strcpy_s(szPreviousFileName,sFileData.cFileName);

						char szTempPathNameOut[MAX_PATH];
						char szTempPathNameOutFTP[MAX_PATH];
						strcpy_s(szTempPathNameOut,"-o");
						strcat_s(szTempPathNameOut, m_szPathNameOut);
						strcat_s(szTempPathNameOut, sFileData.cFileName);
						strcpy_s(szTempPathNameOutFTP,m_szPathNameOut);
						strcat_s(szTempPathNameOutFTP,sFileData.cFileName);

						DWORD dwAttrs = GetFileAttributes(szTempPathName);
						if (dwAttrs & FILE_ATTRIBUTE_ARCHIVE)
						{
							//convert if needed
							int iResult;
							if (strlen(m_szConvertPath))
							{
								if (strcmp(m_szDir,m_szPathNameOut)==0)
									iResult = _spawnl(_P_WAIT,m_szConvertPath,m_szConvertPath,
										"-j", szTempPathName, NULL);
								else
									iResult = _spawnl(_P_WAIT,m_szConvertPath,m_szConvertPath,
										"-j", szTempPathNameOut, szTempPathName, NULL);
							}
							else
								iResult = 0;
							//clear archive
							if (iResult != -1)
							{

								// create a session object to initialize WININET library
								// Default parameters mean the access method in the registry
								// (that is, set by the "Internet" icon in the Control Panel)
								// will be used.

								//spin until the last thread to be kicked off gets its data
								if (strlen(szFTPServer))
									while (sRequest.bGotData == false);

								//set up data to send to thread
								sRequest.iContext = iContext;
								sRequest.bGotData = false;
								sRequest.pszPathNameToSend = szTempPathNameOutFTP;
								sRequest.pszNameOut = sFileData.cFileName;
								sRequest.dwAttrs = dwAttrs;

								//kick off thread to do the ftp
								//and wait a moment before sending another 
								if (strlen(szFTPServer))
								{
									AfxBeginThread(ThreadFunc, (void*)&sRequest);
									//so that we don't hit the ftp server too fast
									//may be necessary for the clear archive bit also
									Sleep(50);
								}
								else //just clear the archive bit
								{
									if (SetFileAttributes(szTempPathName, 
										dwAttrs & !FILE_ATTRIBUTE_ARCHIVE)==0)
									{

										if (strlen(m_cLogFileName))
										{
											CString cString;
											cString.Format("Error on clear archive bit: %s",szTempPathName);
											Log(false,(LPSTR)LPCSTR(cString));
										}

									}
									else
										if (strlen(m_cLogFileName))
										{
											CString cString;
											cString.Format("Ok on clear archive bit: %s",szTempPathName);
											Log(false,(LPSTR)LPCSTR(cString));
										}
									{
									}
								}

							}
							//PUSH XML
							char szXML[1400];
							int iSizeXML = FormatXML(szXML, m_szCameraLocation, 
								sFileData.cFileName, IMAGE, 0);

							int i = 0;
							sprintf_s(szI,"A%d",i);
							GetPrivateProfileString(PUSH_XML,szI,"",
								szForwardAddress,sizeof(szForwardAddress),m_cIniFile);


							bool bSend;
							bSend = true;
							
							for (int iCross = 0; iCross < iCrosscheck; iCross++)
								if (cCrosscheck[iCross].Compare(sFileData.cFileName)==0)
									bSend = false;
							
							if (bSend)
							{
								cCrosscheck[iCrosscheck] = sFileData.cFileName;
								iCrosscheck++;
								if (iCrosscheck >= 100) iCrosscheck = 99;
								while (strlen(szForwardAddress))
								{
									sprintf_s(szPort,"P%d",i);
									iPort = GetPrivateProfileInt(PUSH_XML,szPort,0,m_cIniFile);
									m_sAddr_Tmp2.sin_family = PF_INET;
									m_sAddr_Tmp2.sin_addr.s_addr = inet_addr(LPCSTR(szForwardAddress));
									m_sAddr_Tmp2.sin_port= htons(iPort);
									int iError = 0;
									if (sendto(m_sd_dg,(char*)&szXML,iSizeXML,0,
										(struct sockaddr FAR *) &m_sAddr_Tmp2,sizeof(m_sAddr_Tmp2)) == SOCKET_ERROR)
									{
										iError = WSAGetLastError();
									}
									if (strlen(m_cLogFileName))
									{
										CString cString;
										if (iError)
											cString.Format("Error on send filename to: %s Data: %s",szForwardAddress,szXML);
										else
											cString.Format("Ok on send filename to: %s Data: %s",szForwardAddress,szXML);
										Log(false,(LPSTR)LPCSTR(cString));
									}
									i++;
									sprintf_s(szI,"A%d",i);
									szForwardAddress[0] = NULL;
									GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
								}
							}
						}

						if (FindNextFile(hFind, &sFileData)==0) 
						{
						//	if (GetLastError() == ERROR_NO_MORE_FILES) 
							bFinished = TRUE; 
						}
					} 
					FindClose(hFind);
				}
			}
		}
		m_bInSendImages = false;
	}

	else if (nIDEvent == eHTML)
	{
		bool bDoingFlash = false;
		COleDateTime now = COleDateTime::GetCurrentTime();
		if ((now - m_sLastGoodMessageTime) > COleDateTimeSpan(0,0,5,0)) // 5 minutes!
		{
			bDoingFlash = true;
			for (int i=0; i < m_dNumButtons; i++)
				if (Buttons[i])
					Buttons[i]->SetFlash(true);
			//Log(true,"");
			now = m_sLastGoodMessageTime;
		}
		
		//build .html file
		if (m_bDoHTML)
		{
			GetPrivateProfileString(CONFIGURATION,HTMLNAME,m_cName,m_szHTMLName,sizeof(m_szHTMLName),m_cIniFile);
			int dHTMLTime=GetPrivateProfileInt(CONFIGURATION,HTMLTIME,60,m_cIniFile);
			if (dHTMLTime != m_dHTMLTime)
			{
				m_dHTMLTime = dHTMLTime;
				KillTimer(eHTML);
				SetTimer(eHTML,m_dHTMLTime*1000,NULL);
			}
			if (strstr(m_szHTMLName,".HTML")==NULL)
				strcat_s(m_szHTMLName,".HTML");
			errno_t err;
			FILE *outfile = NULL;
			err = fopen_s(&outfile, m_szHTMLName, "wt");
			if ((err == 0) && outfile) {
				fprintf(outfile,"<HTML>\n");
				fprintf(outfile,"<HEAD><TITLE>%s</TITLE>\n",m_cName);
				fprintf(outfile,"<meta http-equiv=\"expires\" content=\"Tue, 01 Jan 1981 01:00:00 GMT\">\n");
				fprintf(outfile,"<meta http-equiv=\"refresh\" content=\"%d\">\n",m_dHTMLTime);
				fprintf(outfile,"</HEAD>\n");
				fprintf(outfile,"<BODY BGCOLOR=\"#FFFFFF\">\n");
				fprintf(outfile,"<P><H2>%s</H2>\n",m_cName);
				fprintf(outfile,"<P>%s\n",now.Format("%H:%M:%S %A %B %d, %Y"));
				if (bDoingFlash)
					fprintf(outfile,"<P><H3>WARNING: NO UPDATE RECEIVED FROM MIC IN OVER 5 MINUTES</H3>\n");
				fprintf(outfile,"<ALIGN=CENTER>\n");
				fprintf(outfile,"<TR><TD>\n");
				fprintf(outfile,"<TABLE CELLPADDING=5>\n");
				fprintf(outfile,"<TR>\n");
				CString temp;
				for (int i = 0; i < m_dNumButtons; i++)
				{
					fprintf(outfile,"<TR ALIGN=CENTER>\n");
					Buttons[i]->GetWindowText(temp);
					UINT color = Buttons[i]->GetFaceColor();
					UINT finalcolor = color & 0x0000ff;
					finalcolor = (finalcolor<<8) | ((color >> 8) & 0x0000ff);
					finalcolor = (finalcolor<<8) | ((color >>16) & 0x0000ff);
					char flags[16];
					char numeric[32];
					Buttons[i]->GetFlags(flags);
					Buttons[i]->GetNumericLine(numeric, 32);
					if (strlen(flags))
						fprintf(outfile,"<TD BGCOLOR=\"#%06X\">%s<br>%s<br><FONT SIZE=5 FACE=\"WINGDINGS\"><B>%s</B></FONT></A></TD>\n",
							finalcolor,temp,numeric,flags);
					else
						fprintf(outfile,"<TD BGCOLOR=\"#%06X\">%s<br>%s<br></A></TD>\n",
							finalcolor,temp,numeric);
				}
				fprintf(outfile,"</TABLE>\n");
				temp.Format("<P>Status is updated every %d seconds.<br>"
					"Click on \"Reload\" or \"Refresh\"to update.</P>\n",m_dHTMLTime);
				fprintf(outfile,temp);
				fprintf(outfile,"</BODY></HTML>\n");
				fclose(outfile);
			}
		}

	}
	//if we are talking to another tracker then we need to
	//remind it every 1 minute that we are still here
	bool doFour = false;
	if (nIDEvent == eTrackToTrack)
	{
		KillTimer(eTrackToTrack);
		SetTimer(eTrackToTrackLong,60000,NULL); // 60 seconds
		doFour = true;
	}

	if ((nIDEvent == eTrackToTrackLong) || doFour)
	{
		SendDataType SendData;
		memchr(&SendData,NULL,sizeof(SendData));
		SendData.BBM = -1;
		SendData.Index = -1;
		SendData.NumButtons = -1;
		SendData.Base = -1;
		m_sAddr_Tmp.sin_family = PF_INET;
		m_sAddr_Tmp.sin_port = (unsigned short)(htons((unsigned short)m_dDefPort));
	
		CString sToAddress;
		sToAddress.Format("%d.%d.%d.%d",m_b1,m_b2,m_b3,m_b4);
		m_sAddr_Tmp.sin_addr.s_addr = inet_addr(LPCSTR(sToAddress));
		try
		{
			if (sendto(m_sd_dg,(char*)&SendData,sizeof(SendData),0,
				(struct sockaddr FAR *) &m_sAddr_Tmp,sizeof(m_sAddr_Tmp)) == SOCKET_ERROR)
			{
				int  iError = WSAGetLastError();
			}
		}
		catch(...)
		{
		}
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CVersionInfo VersionInfo;
//	CString Name    = VersionInfo.InternalName();
	CString Version = VersionInfo.FileVersion();
	//Version.Format("Version %c.%c.%c.%c",Version[0],Version[3],Version[6],Version[9]);
	// ^ not necessary because I changed version block to use '.' chars
	Version = "Version " + Version;
	SetDlgItemText(IDC_STATIC_VERSION,Version);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
