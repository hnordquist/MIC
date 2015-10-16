// WatchComDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WatchCom.h"
#include "WatchComDlg.h"
#include "Select.h"
#include "serial.h"
#include "DirDialog.h"

#include  <io.h>
#include <afxdisp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	WM_WSARECVA						(WM_USER+2000)
#define	WM_WSARECVB						(WM_USER+2001)
#define	WM_WSARECVC						(WM_USER+2002)
#define	WM_WSARECVD						(WM_USER+2003)
#define IDC_RECEIVE                     10100
#define IDC_RECEIVEA					10101
#define IDC_RECEIVEB					10102
#define IDC_RECEIVEC					10103
#define IDC_RECEIVED					10104

#define RESTARTTIMERTIME				15000

#define RESTARTATIMER					1100
#define RESTARTBTIMER					1101
#define RESTARTCTIMER					1102
#define RESTARTDTIMER					1103

#define CONNECTTIMERTIME				15000
#define CONNECTATIMER					2000
#define CONNECTBTIMER					2001
#define CONNECTCTIMER					2002
#define CONNECTDTIMER					2003

#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

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
		// No message handlers

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchComDlg dialog

CWatchComDlg::CWatchComDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWatchComDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWatchComDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	ComA = NULL;
	ComB = NULL;
	ComC = NULL;
	ComD = NULL;

	m_SocketA = INVALID_SOCKET;
	m_SocketB = INVALID_SOCKET;
	m_SocketC = INVALID_SOCKET;
	m_SocketD = INVALID_SOCKET;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iPreviousDay = 32;

	m_pDataWatchDlg1 = NULL;
	m_pDataWatchDlg2 = NULL;
	m_pDataWatchDlg3 = NULL;
	m_pDataWatchDlg4 = NULL;
	m_bPauseLog = false;
}

void CWatchComDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWatchComDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWatchComDlg, CDialog)
	//{{AFX_MSG_MAP(CWatchComDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(IDC_RECEIVEA,OnReceiveA)
	ON_MESSAGE(IDC_RECEIVEB,OnReceiveB)
	ON_MESSAGE(IDC_RECEIVEC,OnReceiveC)
	ON_MESSAGE(IDC_RECEIVED,OnReceiveD)
	ON_WM_CTLCOLOR()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_STATIC_COMA, OnStaticComa)
	ON_BN_CLICKED(IDC_STATIC_COMC, OnStaticComc)
	ON_BN_CLICKED(IDC_STATIC_COMB, OnStaticComb)
	ON_BN_CLICKED(IDC_STATIC_COMD, OnStaticComd)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_WSARECVA,OnReceiveNA)
	ON_MESSAGE(WM_WSARECVB,OnReceiveNB)
	ON_MESSAGE(WM_WSARECVC,OnReceiveNC)
	ON_MESSAGE(WM_WSARECVD,OnReceiveND)
END_MESSAGE_MAP()
//	ON_BN_CLICKED(IDC_BUTTON_A, OnButtonA)
//	ON_BN_CLICKED(IDC_BUTTON_B, OnButtonB)
//	ON_BN_CLICKED(IDC_BUTTON_C, OnButtonA)
//	ON_BN_CLICKED(IDC_BUTTON_D, OnButtonB)
//	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
//	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)

/////////////////////////////////////////////////////////////////////////////
// CWatchComDlg message handlers
LRESULT CWatchComDlg::OnReceiveNA(WPARAM wParam, LPARAM lParam)
{
	char szSentData[1024];
	if (wParam != m_SocketA) return 0;
	int iRc;
	switch (lParam) {
	case FD_READ:
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0) return 0;
		do {
			iRc = recv(m_SocketA,szSentData,sizeof(szSentData), NULL);
			if (iRc == SOCKET_ERROR) return 0;
			if (iRc)
			{
				char* pszBuffer = (char*)malloc(iRc);
				memcpy(pszBuffer,szSentData,iRc);
				OnReceiveA(iRc,(LPARAM)pszBuffer);
			}
		} while (iRc);
		break;

	case FD_CONNECT:
		sprintf(szSentData,"%s connected: %s on %d",m_szAPort,m_szANet,m_uiANetPort);
		SetDlgItemText(IDC_STATIC_COMA,szSentData);
		KillTimer(CONNECTATIMER);
		break;

	case FD_CLOSE:
		//need to set a timer to reopen the connection
		if (m_SocketA != INVALID_SOCKET)
		{
			shutdown(m_SocketA,SD_BOTH);
			closesocket(m_SocketA);
			m_SocketA = INVALID_SOCKET;
			SetDlgItemText(IDC_STATIC_COMA,"Closed by server, RESTARTING");
			SetTimer(RESTARTATIMER,RESTARTTIMERTIME,NULL);
//TRACE("Restart timer A started CLOSE\r\n");
		}
		break;
	}
}

LRESULT CWatchComDlg::OnReceiveNB(WPARAM wParam, LPARAM lParam)
{
	char szSentData[1024];
	if (wParam != m_SocketB) return 0;
	int iRc;
	switch (lParam) {
	case FD_READ:
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0) return 0;
		do {
			iRc = recv(m_SocketB,szSentData,sizeof(szSentData), NULL);
			if (iRc == SOCKET_ERROR) return 0;
			if (iRc)
			{
				char* pszBuffer = (char*)malloc(iRc);
				memcpy(pszBuffer,szSentData,iRc);
				OnReceiveB(iRc,(LPARAM)pszBuffer);
			}
		} while (iRc);
		break;

	case FD_CONNECT:
		sprintf(szSentData,"%s connected: %s on %d",m_szBPort,m_szBNet,m_uiBNetPort);
		SetDlgItemText(IDC_STATIC_COMB,szSentData);
		KillTimer(CONNECTBTIMER);
		break;

	case FD_CLOSE:
		//need to set a timer to reopen the connection
		if (m_SocketD != INVALID_SOCKET)
		{
			shutdown(m_SocketB,SD_BOTH);
			closesocket(m_SocketB);
			m_SocketB = INVALID_SOCKET;
			SetDlgItemText(IDC_STATIC_COMB,"Closed by server, RESTARTING");
			SetTimer(RESTARTBTIMER,RESTARTTIMERTIME,NULL);
//TRACE("Restart timer B started CLOSE\r\n");
		}
		break;
	}
}

LRESULT CWatchComDlg::OnReceiveNC(WPARAM wParam, LPARAM lParam)
{
	char szSentData[1024];
	if (wParam != m_SocketC) return 0;
	int iRc;
	switch (lParam) {
	case FD_READ:
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0) return 0;
		do {
			iRc = recv(m_SocketC,szSentData,sizeof(szSentData), NULL);
			if (iRc == SOCKET_ERROR) return 0;
			if (iRc)
			{
				char* pszBuffer = (char*)malloc(iRc);
				memcpy(pszBuffer,szSentData,iRc);
				OnReceiveC(iRc,(LPARAM)pszBuffer);
			}
		} while (iRc);
		break;

	case FD_CONNECT:
		sprintf(szSentData,"%s connected: %s on %d",m_szCPort,m_szCNet,m_uiCNetPort);
		SetDlgItemText(IDC_STATIC_COMC,szSentData);
		KillTimer(CONNECTCTIMER);
		break;

	case FD_CLOSE:
		//need to set a timer to reopen the connection
		if (m_SocketC != INVALID_SOCKET)
		{
			shutdown(m_SocketC,SD_BOTH);
			closesocket(m_SocketC);
			m_SocketC = INVALID_SOCKET;
			SetDlgItemText(IDC_STATIC_COMC,"Closed by server, RESTARTING");
			SetTimer(RESTARTCTIMER,RESTARTTIMERTIME,NULL);
//TRACE("Restart timer C started CLOSE\r\n");
		}
		break;
	}
}

LRESULT CWatchComDlg::OnReceiveND(WPARAM wParam, LPARAM lParam)
{
	char szSentData[1024];
	if (wParam != m_SocketD) return 0;
	int iRc;
	switch (lParam) {
	case FD_READ:
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0) return 0;
		do {
			iRc = recv(m_SocketD,szSentData,sizeof(szSentData), NULL);
			if (iRc == SOCKET_ERROR) return 0;
			if (iRc)
			{
				char* pszBuffer = (char*)malloc(iRc);
				memcpy(pszBuffer,szSentData,iRc);
				OnReceiveD(iRc,(LPARAM)pszBuffer);
			}
		} while (iRc);
		break;

	case FD_CONNECT:
		sprintf(szSentData,"%s connected: %s on %d",m_szDPort,m_szDNet,m_uiDNetPort);
		SetDlgItemText(IDC_STATIC_COMD,szSentData);
		KillTimer(CONNECTDTIMER);
		break;

	case FD_CLOSE:
		//need to set a timer to reopen the connection
		if (m_SocketD != INVALID_SOCKET)
		{
			shutdown(m_SocketD,SD_BOTH);
			closesocket(m_SocketD);
			m_SocketD = INVALID_SOCKET;
			SetDlgItemText(IDC_STATIC_COMD,"Closed by server, RESTARTING");
			SetTimer(RESTARTDTIMER,RESTARTTIMERTIME,NULL);
////TRACE("Restart timer D started CLOSE\r\n");
		}
		break;
	}
}

BOOL CWatchComDlg::OnInitDialog()
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
		CString strComA;
		CString strComB;
		CString strComC;
		CString strComD;
		CString strClearLog;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		strComA.LoadString(IDS_COMA);
		strComB.LoadString(IDS_COMB);
		strComC.LoadString(IDS_COMC);
		strComD.LoadString(IDS_COMD);
		strClearLog.LoadString(IDS_CLEARLOG);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_COMA,strComA);
			pSysMenu->AppendMenu(MF_STRING, IDM_COMB,strComB);
			pSysMenu->AppendMenu(MF_STRING, IDM_COMC,strComC);
			pSysMenu->AppendMenu(MF_STRING, IDM_COMD,strComD);
			pSysMenu->AppendMenu(MF_STRING, IDM_CLEARLOG,strClearLog);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_hButtonIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
	((CButton*)GetDlgItem(IDC_BUTTON1))->SetIcon(m_hButtonIcon);
	((CButton*)GetDlgItem(IDC_BUTTON2))->SetIcon(m_hButtonIcon);
	((CButton*)GetDlgItem(IDC_BUTTON3))->SetIcon(m_hButtonIcon);
	((CButton*)GetDlgItem(IDC_BUTTON4))->SetIcon(m_hButtonIcon);

	GetWindowRect(&m_WindowRect);
	ClientToScreen(&m_WindowRect);

	m_bANewLine = true;
	m_bBNewLine = true;
	m_bCNewLine = true;
	m_bDNewLine = true;

	//build the ini file name
	//get full path and name for this instance of application
	GetModuleFileName(AfxGetInstanceHandle(),m_szIniFile,sizeof(m_szIniFile));
	char *c = strrchr(m_szIniFile,'.');//find the last "."
	*c = '\0';						//make it null
	strcat(m_szIniFile,".INI");
	//if the ini file isn't there create a "default" one
	if (access(m_szIniFile,0)!=0)
	{
		FILE* temp;
		if ((temp = fopen(m_szIniFile,"wt"))!=NULL)
		{
			fprintf(temp,"[COM_A]\n");
			fprintf(temp,"DEACTIVATE=Yes\n");
			fprintf(temp,"TCPADDRESS=127.0.0.1\n");
			fprintf(temp,"TCPPORT=23\n");
			fprintf(temp,"BAUD=9600\n");
			fprintf(temp,"PORT=\n");
			fprintf(temp,"DATABITS=8\n");
			fprintf(temp,"STOPBITS=1\n");
			fprintf(temp,"PARITY=NONE\n");
			fprintf(temp,"FILENAME=AA%%P_%%y%%m%%d.log\n");
			fprintf(temp,"FILELOCATION=C:\\\n");
			fprintf(temp,"COLOR_RGB=0,0,0\n\n");

			fprintf(temp,"[COM_B]\n");
			fprintf(temp,"DEACTIVATE=Yes\n");
			fprintf(temp,"TCPADDRESS=127.0.0.1\n");
			fprintf(temp,"TCPPORT=23\n");
			fprintf(temp,"BAUD=9600\n");
			fprintf(temp,"PORT=\n");
			fprintf(temp,"DATABITS=8\n");
			fprintf(temp,"STOPBITS=1\n");
			fprintf(temp,"PARITY=NONE\n");
			fprintf(temp,"FILENAME=AA%%P_%%y%%m%%d.log\n");
			fprintf(temp,"FILELOCATION=C:\\\n");
			fprintf(temp,"COLOR_RGB=191,0,0\n\n");

			fprintf(temp,"[COM_C]\n");
			fprintf(temp,"DEACTIVATE=Yes\n");
			fprintf(temp,"TCPADDRESS=127.0.0.1\n");
			fprintf(temp,"TCPPORT=23\n");
			fprintf(temp,"BAUD=9600\n");
			fprintf(temp,"PORT=\n");
			fprintf(temp,"DATABITS=8\n");
			fprintf(temp,"STOPBITS=1\n");
			fprintf(temp,"PARITY=NONE\n");
			fprintf(temp,"FILENAME=AA%%P_%%y%%m%%d.log\n");
			fprintf(temp,"FILELOCATION=C:\\\n");
			fprintf(temp,"COLOR_RGB=0,191,0\n\n");

			fprintf(temp,"[COM_D]\n");
			fprintf(temp,"DEACTIVATE=Yes\n");
			fprintf(temp,"TCPADDRESS=127.0.0.1\n");
			fprintf(temp,"TCPPORT=23\n");
			fprintf(temp,"BAUD=9600\n");
			fprintf(temp,"PORT=\n");
			fprintf(temp,"DATABITS=8\n");
			fprintf(temp,"STOPBITS=1\n");
			fprintf(temp,"PARITY=NONE\n");
			fprintf(temp,"FILENAME=AA%%P_%%y%%m%%d.log\n");
			fprintf(temp,"FILELOCATION=C:\\\n");
			fprintf(temp,"COLOR_RGB=0,0,255\n");

			fclose(temp);
		}
	}

	m_wVer = MAKEWORD(1,1);
	m_iRc = WSAStartup(m_wVer, &m_WSAData);
	int iRc;
	int iRed,iGreen,iBlue;
	char szTemp[64];
	bool bDeactivate = true;

	GetPrivateProfileString("COM_A","DEACTIVATE","Yes",szTemp,sizeof(szTemp),m_szIniFile);
	bDeactivate = toupper(szTemp[0])=='Y';

	GetPrivateProfileString("COM_A","PORT","",m_szAPort,sizeof(m_szAPort),m_szIniFile);
	GetPrivateProfileString("COM_A","TCPADDRESS","",m_szANet,sizeof(m_szANet),m_szIniFile);
	m_uiANetPort = GetPrivateProfileInt("COM_A","TCPPORT",23,m_szIniFile);
	GetPrivateProfileString("COM_A","PARITY","NONE",m_szAParity,sizeof(m_szAParity),m_szIniFile);
	m_dABaudRate = GetPrivateProfileInt("COM_A","BAUD",9600,m_szIniFile);
	m_dADataBits = GetPrivateProfileInt("COM_A","DATABITS",8,m_szIniFile);
	m_dAStopBits = GetPrivateProfileInt("COM_A","STOPBITS",1,m_szIniFile);
	GetPrivateProfileString("COM_A","FILENAME","",m_szAFileName,sizeof(m_szAFileName),m_szIniFile);
	GetPrivateProfileString("COM_A","FILELOCATION","C:\\",m_szADestination,sizeof(m_szADestination),m_szIniFile);
	GetPrivateProfileString("COM_A","COLOR_RGB","0,0,0",szTemp,sizeof(szTemp),m_szIniFile);
	sscanf(szTemp,"%d,%d,%d",&iRed,&iGreen,&iBlue);
	m_iAColor = RGB(iRed,iGreen,iBlue);

	if (!bDeactivate)
	{
		if (strstr(m_szAPort,"TCP/IP") == NULL)
		{
			if (strlen(m_szAPort) > 0)
			{

				ComA = (CSerial*)AfxBeginThread(RUNTIME_CLASS(CSerial));
				//set it up
				sprintf(szTemp,"Com A: %s, %d, %d, %d, %s",
					m_szAPort,
					m_dABaudRate,
					m_dADataBits,
					m_dAStopBits,
					m_szAParity);

				if (ComA->SetUpPort(m_szAPort,m_dABaudRate,
						m_dADataBits, m_dAStopBits, m_szAParity))
				{
					//connect to it
					ComA->DoSetMessage(1);
					ComA->Register(this);
					ComA->PostThreadMessage(IDC_COM_START,0,0);
					SetDlgItemText(IDC_STATIC_COMA,szTemp);
				}
				else
				{
					CString st("Could not set up port\n");
					st += szTemp;

					//shutdown the com object's receiver
					ComA->DoStop();
					//get the com object to kill itself
					ComA->PostThreadMessage(IDC_COM_STOP,0,0);
					::WaitForSingleObject(ComA->m_hThread,1000);
			//		TerminateThread(ComA->m_hThread,0);
					delete ComA;
					ComA = NULL;

					SetDlgItemText(IDC_STATIC_COMA,"Com A: FAILED");
				}
			}
		}
		else
		{
			if (m_iRc == 0)
			{
				m_SocketA = socket(PF_INET,SOCK_STREAM,0);
				if (m_SocketA != INVALID_SOCKET)
				{
					int iResult = WSAAsyncSelect(m_SocketA, m_hWnd, WM_WSARECVA, FD_READ | FD_CONNECT | FD_CLOSE);
					if (iResult == SOCKET_ERROR)
					{
						SetDlgItemText(IDC_STATIC_COMA,"Socket Error");
					}
					else
					{
						m_sAddrA.sin_family=PF_INET;
						m_sAddrA.sin_port=htons(m_uiANetPort);
						m_sAddrA.sin_addr.s_addr = inet_addr(m_szANet);
						iRc = connect(m_SocketA,(sockaddr*)&m_sAddrA,sizeof(m_sAddrA));
						DWORD dwResult = GetLastError();
						if (dwResult == WSAEWOULDBLOCK)
						{
							SetDlgItemText(IDC_STATIC_COMA,"Connecting");
							SetTimer(CONNECTATIMER,CONNECTTIMERTIME,NULL);
						}
						else
						{
							char szError[256];
							sprintf(szError, "Connect error: %d", dwResult);
							SetDlgItemText(IDC_STATIC_COMA,szError);
							SetTimer(RESTARTATIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer A started INITIAL\r\n");
						}
					}
				}
				else
				{
					char szError[256];
					DWORD dwResult = GetLastError();
					sprintf(szError, "Socket error: %d", dwResult);
					SetDlgItemText(IDC_STATIC_COMA,szError);
					SetTimer(RESTARTATIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer A started INITIAL\r\n");
				}
			}
			else
				SetDlgItemText(IDC_STATIC_COMA,"Cannot start network.");
		}
	}


	GetPrivateProfileString("COM_B","DEACTIVATE","Yes",szTemp,sizeof(szTemp),m_szIniFile);
	bDeactivate = toupper(szTemp[0])=='Y';

	GetPrivateProfileString("COM_B","PORT","",m_szBPort,sizeof(m_szBPort),m_szIniFile);
	GetPrivateProfileString("COM_B","TCPADDRESS","",m_szBNet,sizeof(m_szBNet),m_szIniFile);
	m_uiBNetPort = GetPrivateProfileInt("COM_B","TCPPORT",23,m_szIniFile);
	GetPrivateProfileString("COM_B","PARITY","NONE",m_szBParity,sizeof(m_szBParity),m_szIniFile);
	m_dBBaudRate = GetPrivateProfileInt("COM_B","BAUD",9600,m_szIniFile);
	m_dBDataBits = GetPrivateProfileInt("COM_B","DATABITS",8,m_szIniFile);
	m_dBStopBits = GetPrivateProfileInt("COM_B","STOPBITS",1,m_szIniFile);
	GetPrivateProfileString("COM_B","FILENAME","",m_szBFileName,sizeof(m_szBFileName),m_szIniFile);
	GetPrivateProfileString("COM_B","FILELOCATION","C:\\",m_szBDestination,sizeof(m_szBDestination),m_szIniFile);
	GetPrivateProfileString("COM_B","COLOR_RGB","191,0,0",szTemp,sizeof(szTemp),m_szIniFile);
	sscanf(szTemp,"%d,%d,%d",&iRed,&iGreen,&iBlue);
	m_iBColor = RGB(iRed,iGreen,iBlue);

	if (!bDeactivate)
	{
		if (strstr(m_szBPort,"TCP/IP") == NULL)
		{
			if (strlen(m_szBPort) > 0)
			{

				ComB = (CSerial*)AfxBeginThread(RUNTIME_CLASS(CSerial));
				//set it up
				sprintf(szTemp,"Com B: %s, %d, %d, %d, %s",
					m_szBPort,
					m_dBBaudRate,
					m_dBDataBits,
					m_dBStopBits,
					m_szBParity);

				if (ComB->SetUpPort(m_szBPort,m_dBBaudRate,
						m_dBDataBits, m_dBStopBits, m_szBParity))
				{	
					//connect to it
					ComB->DoSetMessage(2);
					ComB->Register(this);
					ComB->PostThreadMessage(IDC_COM_START,0,0);
					SetDlgItemText(IDC_STATIC_COMB,szTemp);
				}
				else
				{
					CString st("Could not set up port\n");
					st += szTemp;

					//shutdown the com object's receiver
					ComB->DoStop();
					//get the com object to kill itself
					ComB->PostThreadMessage(IDC_COM_STOP,0,0);
					::WaitForSingleObject(ComB->m_hThread,1000);
					delete ComB;
					ComB = NULL;

					SetDlgItemText(IDC_STATIC_COMB,"Com B: FAILED");
				}
			}
		}
		else
		{
			if (m_iRc == 0)
			{
				m_SocketB = socket(PF_INET,SOCK_STREAM,0);
				if (m_SocketB != INVALID_SOCKET)
				{
					int iResult = WSAAsyncSelect(m_SocketB, m_hWnd, WM_WSARECVB, FD_READ | FD_CONNECT | FD_CLOSE);
					if (iResult == SOCKET_ERROR)
					{
						SetDlgItemText(IDC_STATIC_COMB,"Socket Error");
					}
					else
					{
						m_sAddrB.sin_family=PF_INET;
						m_sAddrB.sin_port=htons(m_uiBNetPort);
						m_sAddrB.sin_addr.s_addr = inet_addr(m_szBNet);
						iRc = connect(m_SocketB,(sockaddr*)&m_sAddrB,sizeof(m_sAddrB));
						DWORD dwResult = GetLastError();
						if (dwResult == WSAEWOULDBLOCK)
						{
							SetDlgItemText(IDC_STATIC_COMB,"Connecting");
							SetTimer(CONNECTBTIMER,CONNECTTIMERTIME,NULL);
						}
						else
						{
							char szError[256];
							sprintf(szError, "Connect error: %d", dwResult);
							SetDlgItemText(IDC_STATIC_COMB,szError);
							SetTimer(RESTARTBTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer B started INITIAL\r\n");
						}
					}
				}
				else
				{
					char szError[256];
					DWORD dwResult = GetLastError();
					sprintf(szError, "Socket error: %d", dwResult);
					SetDlgItemText(IDC_STATIC_COMB,szError);
					SetTimer(RESTARTBTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer B started INITIAL\r\n");
				}
			}
			else
				SetDlgItemText(IDC_STATIC_COMB,"Cannot start network.");
		}
	}

	GetPrivateProfileString("COM_C","DEACTIVATE","Yes",szTemp,sizeof(szTemp),m_szIniFile);
	bDeactivate = toupper(szTemp[0])=='Y';

	GetPrivateProfileString("COM_C","PORT","",m_szCPort,sizeof(m_szCPort),m_szIniFile);
	GetPrivateProfileString("COM_C","TCPADDRESS","",m_szCNet,sizeof(m_szCNet),m_szIniFile);
	m_uiCNetPort = GetPrivateProfileInt("COM_C","TCPPORT",23,m_szIniFile);
	GetPrivateProfileString("COM_C","PARITY","NONE",m_szCParity,sizeof(m_szCParity),m_szIniFile);
	m_dCBaudRate = GetPrivateProfileInt("COM_C","BAUD",9600,m_szIniFile);
	m_dCDataBits = GetPrivateProfileInt("COM_C","DATABITS",8,m_szIniFile);
	m_dCStopBits = GetPrivateProfileInt("COM_C","STOPBITS",1,m_szIniFile);
	GetPrivateProfileString("COM_C","FILENAME","",m_szCFileName,sizeof(m_szCFileName),m_szIniFile);
	GetPrivateProfileString("COM_C","FILELOCATION","C:\\",m_szCDestination,sizeof(m_szCDestination),m_szIniFile);
	GetPrivateProfileString("COM_C","COLOR_RGB","0,191,0",szTemp,sizeof(szTemp),m_szIniFile);
	sscanf(szTemp,"%d,%d,%d",&iRed,&iGreen,&iBlue);
	m_iCColor = RGB(iRed,iGreen,iBlue);

	if (!bDeactivate)
	{
		if (strstr(m_szCPort,"TCP/IP") == NULL)
		{

			if (strlen(m_szCPort) > 0)
			{

				ComC = (CSerial*)AfxBeginThread(RUNTIME_CLASS(CSerial));
				//set it up
				sprintf(szTemp,"Com C: %s, %d, %d, %d, %s",
					m_szCPort,
					m_dCBaudRate,
					m_dCDataBits,
					m_dCStopBits,
					m_szCParity);

				if (ComC->SetUpPort(m_szCPort,m_dCBaudRate,
						m_dCDataBits, m_dCStopBits, m_szCParity))
				{
					//connect to it
					ComC->DoSetMessage(3);
					ComC->Register(this);
					ComC->PostThreadMessage(IDC_COM_START,0,0);
					SetDlgItemText(IDC_STATIC_COMC,szTemp);
				}
				else
				{
					CString st("Could not set up port\n");
					st += szTemp;

					//shutdown the com object's receiver
					ComC->DoStop();
					//get the com object to kill itself
					ComC->PostThreadMessage(IDC_COM_STOP,0,0);
					::WaitForSingleObject(ComC->m_hThread,1000);
					delete ComC;
					ComC = NULL;

					SetDlgItemText(IDC_STATIC_COMC,"Com C: FAILED");
				}
			}
		}
		else
		{
			if (m_iRc == 0)
			{
				m_SocketC = socket(PF_INET,SOCK_STREAM,0);
				if (m_SocketC != INVALID_SOCKET)
				{
					int iResult = WSAAsyncSelect(m_SocketC, m_hWnd, WM_WSARECVC, FD_READ | FD_CONNECT | FD_CLOSE);
					if (iResult == SOCKET_ERROR)
					{
						SetDlgItemText(IDC_STATIC_COMC,"Socket Error");
					}
					else
					{
						m_sAddrC.sin_family=PF_INET;
						m_sAddrC.sin_port=htons(m_uiCNetPort);
						m_sAddrC.sin_addr.s_addr = inet_addr(m_szCNet);
						iRc = connect(m_SocketC,(sockaddr*)&m_sAddrC,sizeof(m_sAddrC));
						DWORD dwResult = GetLastError();
						if (dwResult == WSAEWOULDBLOCK)
						{
							SetDlgItemText(IDC_STATIC_COMC,"Connecting");
							SetTimer(CONNECTCTIMER,CONNECTTIMERTIME,NULL);
						}
						else
						{
							char szError[256];
							sprintf(szError, "Connect error: %d", dwResult);
							SetDlgItemText(IDC_STATIC_COMC,szError);
							SetTimer(RESTARTCTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer C started INITIAL\r\n");
						}
					}
				}
				else
				{
					char szError[256];
					DWORD dwResult = GetLastError();
					sprintf(szError, "Socket error: %d", dwResult);
					SetDlgItemText(IDC_STATIC_COMC,szError);
					SetTimer(RESTARTCTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer C started INITIAL\r\n");
				}
			}
			else
				SetDlgItemText(IDC_STATIC_COMC,"Cannot start network.");
		}
	}

	GetPrivateProfileString("COM_D","DEACTIVATE","Yes",szTemp,sizeof(szTemp),m_szIniFile);
	bDeactivate = toupper(szTemp[0])=='Y';

	GetPrivateProfileString("COM_D","PORT","",m_szDPort,sizeof(m_szDPort),m_szIniFile);
	GetPrivateProfileString("COM_D","TCPADDRESS","",m_szDNet,sizeof(m_szDNet),m_szIniFile);
	m_uiDNetPort = GetPrivateProfileInt("COM_D","TCPPORT",23,m_szIniFile);
	GetPrivateProfileString("COM_D","PARITY","NONE",m_szDParity,sizeof(m_szDParity),m_szIniFile);
	m_dDBaudRate = GetPrivateProfileInt("COM_D","BAUD",9600,m_szIniFile);
	m_dDDataBits = GetPrivateProfileInt("COM_D","DATABITS",8,m_szIniFile);
	m_dDStopBits = GetPrivateProfileInt("COM_D","STOPBITS",1,m_szIniFile);
	GetPrivateProfileString("COM_D","FILENAME","",m_szDFileName,sizeof(m_szDFileName),m_szIniFile);
	GetPrivateProfileString("COM_D","FILELOCATION","C:\\",m_szDDestination,sizeof(m_szDDestination),m_szIniFile);
	GetPrivateProfileString("COM_D","COLOR_RGB","0,0,255",szTemp,sizeof(szTemp),m_szIniFile);
	sscanf(szTemp,"%d,%d,%d",&iRed,&iGreen,&iBlue);
	m_iDColor = RGB(iRed,iGreen,iBlue);

	if (!bDeactivate)
	{
		if (strstr(m_szDPort,"TCP/IP") == NULL)
		{

			if (strlen(m_szDPort) > 0)
			{

				ComD = (CSerial*)AfxBeginThread(RUNTIME_CLASS(CSerial));
				//set it up
				sprintf(szTemp,"Com D: %s, %d, %d, %d, %s",
					m_szDPort,
					m_dDBaudRate,
					m_dDDataBits,
					m_dDStopBits,
					m_szDParity);

				if (ComD->SetUpPort(m_szDPort,m_dDBaudRate,
						m_dDDataBits, m_dDStopBits, m_szDParity))
				{
					//connect to it
					ComD->DoSetMessage(4);
					ComD->Register(this);
					ComD->PostThreadMessage(IDC_COM_START,0,0);
					SetDlgItemText(IDC_STATIC_COMD,szTemp);
				}
				else
				{
					CString st("Could not set up port\n");
					st += szTemp;

					//shutdown the com object's receiver
					ComD->DoStop();
					//get the com object to kill itself
					ComD->PostThreadMessage(IDC_COM_STOP,0,0);
					::WaitForSingleObject(ComD->m_hThread,1000);
					delete ComD;
					ComD = NULL;

					SetDlgItemText(IDC_STATIC_COMD,"Com D: FAILED");
				}
			}
		}
		else
		{
			if (m_iRc == 0)
			{
				m_SocketD = socket(PF_INET,SOCK_STREAM,0);
				if (m_SocketD != INVALID_SOCKET)
				{
					int iResult = WSAAsyncSelect(m_SocketD, m_hWnd, WM_WSARECVD, FD_READ | FD_CONNECT | FD_CLOSE);
					if (iResult == SOCKET_ERROR)
					{
						SetDlgItemText(IDC_STATIC_COMD,"Socket Error");
					}
					else
					{
						m_sAddrD.sin_family=PF_INET;
						m_sAddrD.sin_port=htons(m_uiDNetPort);
						m_sAddrD.sin_addr.s_addr = inet_addr(m_szDNet);
						iRc = connect(m_SocketD,(sockaddr*)&m_sAddrD,sizeof(m_sAddrD));
						DWORD dwResult = GetLastError();
						if (dwResult == WSAEWOULDBLOCK)
						{
							SetDlgItemText(IDC_STATIC_COMD,"Connecting");
							SetTimer(CONNECTDTIMER,CONNECTTIMERTIME,NULL);
						}
						else
						{
							char szError[256];
							sprintf(szError, "Connect error: %d", dwResult);
							SetDlgItemText(IDC_STATIC_COMD,szError);
							SetTimer(RESTARTDTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer D started INITIAL\r\n");
						}
					}
				}
				else
				{
					char szError[256];
					DWORD dwResult = GetLastError();
					sprintf(szError, "Socket error: %d", dwResult);
					SetDlgItemText(IDC_STATIC_COMD,szError);
					SetTimer(RESTARTDTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer D started INITIAL\r\n");

				}
			}
			else
				SetDlgItemText(IDC_STATIC_COMD,"Cannot start network.");
		}
	}

	m_bDataWatchDlg1Open = false;
	m_bDataWatchDlg2Open = false;
	m_bDataWatchDlg3Open = false;
	m_bDataWatchDlg4Open = false;
	
	GetDlgItem(IDC_RICHEDIT1)->SetFocus();
	SetDlgItemInt(IDC_EDIT1,m_iSeq=0);

//test timer
//SetTimer(2020,100,NULL);

	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CWatchComDlg::OnReceive(WPARAM WParam, LPARAM LParam)
{
	char hex[] = "0123456789ABCDEF";
	//make tbuff point to LParam
	char* tbuff = (char*)LParam;
	//create an "exchange" buffer to receive the data
	char  xbuff[2048];
	//j is index into xbuff
	int j = 0;
	
	//convert to displayable in CEdit
	for (unsigned int i = 0; i < WParam; i++)
	{
//		xbuff[j++] = tbuff[i];
		
		xbuff[j++] = hex[(tbuff[i] & 0xf0)>>4];
		xbuff[j++] = hex[tbuff[i] & 0x0f];
		xbuff[j++] = ' ';

//		if ( ( (tbuff[i]&0x7f) == 0x0d) || ((tbuff[i]&0x7f) == 0x0a))
//		{
//				xbuff[j++] = 0x0d;
//				xbuff[j++] = 0x0a;
//		}
//		else 
//		{
//			if (isprint(tbuff[i]&0x7f))
//				xbuff[j++] = tbuff[i] & 0x7f;
//			else
//			{
//				xbuff[j++] = '{';
//				xbuff[j++] = hex[(tbuff[i] & 0xf0)>>4];
//				xbuff[j++] = hex[tbuff[i] & 0x0f];
//				xbuff[j++] = '}';
//			}
//		}
	}
	//terminate the character buffer 
	xbuff[j] = NULL;
	//increment the count of number of char in the cedit

	//select the end of the cedit (insertion point)
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(-2,-1);
	//append the new data
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->ReplaceSel(xbuff,false);

	delete [] (char*)LParam;
}
/*
void
CWatchComDlg::OnReceiveA(WPARAM WParam, LPARAM LParam)
{
	CString cResult;
	CString cTemp;
	CString cFormat;
	//static CString cPrevious;
	
	bool bShowIt = false;
	COleDateTime cNow = COleDateTime::GetCurrentTime();
	if (cNow.GetDay() < m_iPreviousDay)
	{
		OnButtonClear();
		m_iPreviousDay = cNow.GetDay();
	}

//	if (m_bDataWatchDlg1Open)
	{
		char* pAt1 = (char*)LParam;
		char* pAt2 = pAt1+1;
		if (*pAt1 == 'N') 
		{
			if ((*pAt2 == 'B') || (*pAt2 == 'S') || (*pAt2 == 'A'))
			{
				pAt1[WParam] = NULL;
				m_cPrevious = CString(pAt1);
				m_cPrevious.TrimRight("\r\n");
				pAt2 += 2;;
				sscanf(pAt2,"%f,%f,%f,%f",
					&sAcquireRecordA.NeutronChA,
					&sAcquireRecordA.NeutronChB,
					&sAcquireRecordA.NeutronChC,
					&sAcquireRecordA.NeutronChD);
			}
		}
		else if (*pAt1 == 'G')
		{
			if ((*pAt2 == 'B') || (*pAt2 == 'S') || (*pAt2 == 'A'))
			{
				bShowIt = true;
				pAt2 += 2;
				sscanf(pAt2,"%f,%f,%f,%f",
					&sAcquireRecordA.GammaCh1,
					&sAcquireRecordA.GammaCh2,
					&sAcquireRecordA.GammaCh3,
					&sAcquireRecordA.GammaCh4);
				COleDateTime cBase(1952,1,1,0,0,0);
				COleDateTimeSpan cSpan = cNow - cBase;

	if (m_bDataWatchDlg1Open)
				m_pDataWatchDlg1->SetData(cSpan.GetTotalSeconds(),
					sAcquireRecordA.NeutronChA,
					sAcquireRecordA.NeutronChB,
					sAcquireRecordA.NeutronChC,
					sAcquireRecordA.NeutronChD,
					sAcquireRecordA.GammaCh1,
					sAcquireRecordA.GammaCh2, 
					sAcquireRecordA.GammaCh3, 
					sAcquireRecordA.GammaCh4, 
					true);
			}
		}
	}

	if (bShowIt)
	{
		cFormat = m_szAFileName;
		bool bHaveEscape;
		bHaveEscape = false;
		int i;
		if (strlen(m_szAFileName) && !m_bPauseLog)
		{
			for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
			{
				if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
					bHaveEscape = true;
				else if (bHaveEscape)
				{
					bHaveEscape = false;
					switch (cFormat[iSourceIndex]) {
					case 'P':
						{
							if (strstr(m_szAPort,"TCP/IP"))
								cResult += m_szANet;
							else
								for (i = 0; i < (int)strlen(m_szAPort); i++)
									if ((m_szAPort[i] >= '0')  && (m_szAPort[i] <= '9')) cResult += m_szAPort[i];
						}
						break;
					case 'y':
						cTemp.Format("%04d",cNow.GetYear());
						cResult += cTemp;
						break;
					case 'm':
						cTemp.Format("%02d",cNow.GetMonth());
						cResult += cTemp;
						break;
					case 'd':
						cTemp.Format("%02d",cNow.GetDay());
						cResult += cTemp;
						break;
					case 'H':
						cTemp.Format("%02d",cNow.GetHour());
						cResult += cTemp;
						break;
					case 'M':
						cTemp.Format("%02d",cNow.GetMinute());
						cResult += cTemp;
						break;
					case 'S':
						cTemp.Format("%02d",cNow.GetSecond());
						cResult += cTemp;
						break;
					case 's':
						cTemp.Format("%04d",m_iSeq);
						cResult += cTemp;
						break;
					default:
						cResult += cFormat[iSourceIndex];
					}
				}
				else
					cResult += cFormat[iSourceIndex];
			}

			if (cResult.FindOneOf("\\/?*:;><\"")<0)
			{
				CString cFilePathName;
				cFilePathName = m_szADestination;
				if ((cFilePathName.Right(1) != '\\') && (cResult.Left(1) != '\\'))
					cFilePathName += '\\';
				cFilePathName += cResult;
				FILE * outfile = fopen(cFilePathName,"at");
				if (outfile)
				{
					char szBuffer[1024];
					strcpy(szBuffer,m_cPrevious);
					strcat(szBuffer,",");
					strncat(szBuffer,(const char*)LParam,WParam);
					szBuffer[m_cPrevious.GetLength()+1+WParam] = NULL;

					for (int i = 0; i < strlen(szBuffer); i++)
					{
						if (m_bANewLine)
						{
							m_bANewLine = false;
							fprintf(outfile,"%s ",cNow.Format("%Y\\%m\\%d %H:%M:%S"));
						}
						char ch = szBuffer[i];//(*((char*)LParam+i));
						if ((m_cAPreviousChar == 0x0d) && (ch != 0x0a)) 
							fputc(0x0a,outfile);
						fputc(ch,outfile);
						if (ch == 0x0A) m_bANewLine = true;
						m_cAPreviousChar = ch;
					}
					fclose(outfile);
				}
			}
		}

		CHARFORMAT cf;
		cf.cbSize=sizeof(cf);
		cf.dwEffects = CFE_BOLD;
		cf.dwMask = CFM_COLOR | CFM_BOLD;
		cf.crTextColor = m_iAColor;
		long start = ((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->GetTextLength();
		long end = start + WParam + m_cPrevious.GetLength();
		char* pAt2 = (char*)malloc(WParam + m_cPrevious.GetLength()+2);
		strcpy(pAt2,m_cPrevious);
		strcat(pAt2,",");
		strncat(pAt2,(const char*)LParam,WParam);
		OnReceive(strlen(pAt2), (LPARAM)pAt2);
		delete [] (void*)LParam;
		((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(start,-1);
		((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSelectionCharFormat(cf);
		((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(-1,-1);

	}
}
*/
LRESULT
CWatchComDlg::OnReceiveA(WPARAM WParam, LPARAM LParam)
{
	CString cResult;
	CString cTemp;
	CString cFormat;
	COleDateTime cNow = COleDateTime::GetCurrentTime();
	if (cNow.GetDay() < m_iPreviousDay)
	{
		OnButtonClear();
		m_iPreviousDay = cNow.GetDay();
	}
	if (m_bDataWatchDlg1Open)
	{
		char* pAt = (char*)LParam;
		if (*pAt++ == 'N')
		{
			if (*pAt++ == 'B')
			{
				pAt++;
				sscanf(pAt,"%f,%f,%f,%f",
					&sAcquireRecordA.NeutronChA,
					&sAcquireRecordA.NeutronChB,
					&sAcquireRecordA.NeutronChC,
					&sAcquireRecordA.NeutronChD);
			}
		}
		else if (*pAt++ == 'G')
		{
			if (*pAt++ == 'B')
			{
				pAt++;
				sscanf(pAt,"%f,%f,%f,%f",
					&sAcquireRecordA.GammaCh1,
					&sAcquireRecordA.GammaCh2,
					&sAcquireRecordA.GammaCh3,
					&sAcquireRecordA.GammaCh4);
				COleDateTime cBase(1952,1,1,0,0,0);
				COleDateTimeSpan cSpan = cNow - cBase;

				m_pDataWatchDlg1->SetData(cSpan.GetTotalSeconds(),
					sAcquireRecordA.NeutronChA,
					sAcquireRecordA.NeutronChB,
					sAcquireRecordA.NeutronChC,
					sAcquireRecordA.NeutronChD,
					sAcquireRecordA.GammaCh1,
					sAcquireRecordA.GammaCh2, 
					sAcquireRecordA.GammaCh3, 
					sAcquireRecordA.GammaCh4, 
					true);
			}
		}
	}

	cFormat = m_szAFileName;
	bool bHaveEscape;
	bHaveEscape = false;
	int i;
	if (strlen(m_szAFileName))
	{

		for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
		{
			if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
				bHaveEscape = true;
			else if (bHaveEscape)
			{
				bHaveEscape = false;
				switch (cFormat[iSourceIndex]) {
				case 'P':
					{
						if (strstr(m_szAPort,"TCP/IP"))
							cResult += m_szANet;
						else
							for (i = 0; i < (int)strlen(m_szAPort); i++)
								if ((m_szAPort[i] >=0)  && (m_szAPort[i] <= '9')) cResult += m_szAPort[i];
					}
					break;
				case 'y':
					cTemp.Format("%04d",cNow.GetYear());
					cResult += cTemp;
					break;
				case 'm':
					cTemp.Format("%02d",cNow.GetMonth());
					cResult += cTemp;
					break;
				case 'd':
					cTemp.Format("%02d",cNow.GetDay());
					cResult += cTemp;
					break;
				case 'H':
					cTemp.Format("%02d",cNow.GetHour());
					cResult += cTemp;
					break;
				case 'M':
					cTemp.Format("%02d",cNow.GetMinute());
					cResult += cTemp;
					break;
				case 'S':
					cTemp.Format("%02d",cNow.GetSecond());
					cResult += cTemp;
					break;
				default:
					cResult += cFormat[iSourceIndex];
				}
			}
			else
				cResult += cFormat[iSourceIndex];
		}
		if (cResult.FindOneOf("\\/?*:;><\"")<0)
		{
			CString cFilePathName;
			cFilePathName = m_szBDestination;
			if ((cFilePathName.Right(1) != '\\') && (cResult.Left(1) != '\\'))
				cFilePathName += '\\';
			cFilePathName += cResult;
			FILE * outfile = fopen(cFilePathName,"at");
			if (outfile)
			{
				for (int i = 0; i < (int)WParam; i++)
				{
					if (m_bANewLine)
					{
						m_bANewLine = false;
						fprintf(outfile,"%s ",cNow.Format("%Y\\%m\\%d %H:%M:%S"));
					}
					char ch = (*((char*)LParam+i));
					if ((m_cAPreviousChar == 0x0d) && (ch != 0x0a)) 
						fputc(0x0a,outfile);
					fputc(ch,outfile);
					if (ch == 0x0A) m_bANewLine = true;
					m_cAPreviousChar = ch;
				}
				fclose(outfile);
			}
		}
	}

	CHARFORMAT cf;
	cf.cbSize=sizeof(cf);
	cf.dwEffects = CFE_BOLD;
	cf.dwMask = CFM_COLOR | CFM_BOLD;
	cf.crTextColor = m_iAColor;
	long start = ((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->GetTextLength();
	long end = start + WParam;
	OnReceive(WParam, LParam);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(start,-1);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSelectionCharFormat(cf);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(-1,-1);
}

LRESULT
CWatchComDlg::OnReceiveB(WPARAM WParam, LPARAM LParam)
{
	CString cResult;
	CString cTemp;
	CString cFormat;
	COleDateTime cNow = COleDateTime::GetCurrentTime();
	if (cNow.GetDay() < m_iPreviousDay)
	{
		OnButtonClear();
		m_iPreviousDay = cNow.GetDay();
	}
	if (m_bDataWatchDlg2Open)
	{
		char* pAt = (char*)LParam;
		if (*pAt++ == 'N')
		{
			if (*pAt++ == 'B')
			{
				pAt++;
				sscanf(pAt,"%f,%f,%f,%f",
					&sAcquireRecordB.NeutronChA,
					&sAcquireRecordB.NeutronChB,
					&sAcquireRecordB.NeutronChC,
					&sAcquireRecordB.NeutronChD);
			}
		}
		else if (*pAt++ == 'G')
		{
			if (*pAt++ == 'B')
			{
				pAt++;
				sscanf(pAt,"%f,%f,%f,%f",
					&sAcquireRecordB.GammaCh1,
					&sAcquireRecordB.GammaCh2,
					&sAcquireRecordB.GammaCh3,
					&sAcquireRecordB.GammaCh4);
				COleDateTime cBase(1952,1,1,0,0,0);
				COleDateTimeSpan cSpan = cNow - cBase;

				m_pDataWatchDlg1->SetData(cSpan.GetTotalSeconds(),
					sAcquireRecordB.NeutronChA,
					sAcquireRecordB.NeutronChB,
					sAcquireRecordB.NeutronChC,
					sAcquireRecordB.NeutronChD,
					sAcquireRecordB.GammaCh1,
					sAcquireRecordB.GammaCh2, 
					sAcquireRecordB.GammaCh3, 
					sAcquireRecordB.GammaCh4, 
					true);
			}
		}
	}

	cFormat = m_szBFileName;
	bool bHaveEscape;
	bHaveEscape = false;
	int i;
	if (strlen(m_szBFileName))
	{

		for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
		{
			if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
				bHaveEscape = true;
			else if (bHaveEscape)
			{
				bHaveEscape = false;
				switch (cFormat[iSourceIndex]) {
				case 'P':
					{
						if (strstr(m_szBPort,"TCP/IP"))
							cResult += m_szBNet;
						else
							for (i = 0; i < (int)strlen(m_szBPort); i++)
								if ((m_szBPort[i] >=0)  && (m_szBPort[i] <= '9')) cResult += m_szBPort[i];
					}
					break;
				case 'y':
					cTemp.Format("%04d",cNow.GetYear());
					cResult += cTemp;
					break;
				case 'm':
					cTemp.Format("%02d",cNow.GetMonth());
					cResult += cTemp;
					break;
				case 'd':
					cTemp.Format("%02d",cNow.GetDay());
					cResult += cTemp;
					break;
				case 'H':
					cTemp.Format("%02d",cNow.GetHour());
					cResult += cTemp;
					break;
				case 'M':
					cTemp.Format("%02d",cNow.GetMinute());
					cResult += cTemp;
					break;
				case 'S':
					cTemp.Format("%02d",cNow.GetSecond());
					cResult += cTemp;
					break;
				default:
					cResult += cFormat[iSourceIndex];
				}
			}
			else
				cResult += cFormat[iSourceIndex];
		}
		if (cResult.FindOneOf("\\/?*:;><\"")<0)
		{
			CString cFilePathName;
			cFilePathName = m_szBDestination;
			if ((cFilePathName.Right(1) != '\\') && (cResult.Left(1) != '\\'))
				cFilePathName += '\\';
			cFilePathName += cResult;
			FILE * outfile = fopen(cFilePathName,"at");
			if (outfile)
			{
				for (int i = 0; i < (int)WParam; i++)
				{
					if (m_bBNewLine)
					{
						m_bBNewLine = false;
						fprintf(outfile,"%s ",cNow.Format("%Y\\%m\\%d %H:%M:%S"));
					}
					char ch = (*((char*)LParam+i));
					if ((m_cBPreviousChar == 0x0d) && (ch != 0x0a)) 
						fputc(0x0a,outfile);
					fputc(ch,outfile);
					if (ch == 0x0A) m_bBNewLine = true;
					m_cBPreviousChar = ch;
				}
				fclose(outfile);
			}
		}
	}

	CHARFORMAT cf;
	cf.cbSize=sizeof(cf);
	cf.dwEffects = CFE_BOLD;
	cf.dwMask = CFM_COLOR | CFM_BOLD;
	cf.crTextColor = m_iBColor;
	long start = ((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->GetTextLength();
	long end = start + WParam;
	OnReceive(WParam, LParam);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(start,-1);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSelectionCharFormat(cf);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(-1,-1);
}

LRESULT
CWatchComDlg::OnReceiveC(WPARAM WParam, LPARAM LParam)
{
	CString cResult;
	CString cTemp;
	CString cFormat;
	COleDateTime cNow = COleDateTime::GetCurrentTime();
	if (cNow.GetDay() < m_iPreviousDay)
	{
		OnButtonClear();
		m_iPreviousDay = cNow.GetDay();
	}
	if (m_bDataWatchDlg3Open)
	{
		char* pAt = (char*)LParam;
		if (*pAt++ == 'N')
		{
			if (*pAt++ == 'B')
			{
				pAt++;
				sscanf(pAt,"%f,%f,%f,%f",
					&sAcquireRecordC.NeutronChA,
					&sAcquireRecordC.NeutronChB,
					&sAcquireRecordC.NeutronChC,
					&sAcquireRecordC.NeutronChD);
			}
		}
		else if (*pAt++ == 'G')
		{
			if (*pAt++ == 'B')
			{
				pAt++;
				sscanf(pAt,"%f,%f,%f,%f",
					&sAcquireRecordC.GammaCh1,
					&sAcquireRecordC.GammaCh2,
					&sAcquireRecordC.GammaCh3,
					&sAcquireRecordC.GammaCh4);
				COleDateTime cBase(1952,1,1,0,0,0);
				COleDateTimeSpan cSpan = cNow - cBase;

				m_pDataWatchDlg1->SetData(cSpan.GetTotalSeconds(),
					sAcquireRecordC.NeutronChA,
					sAcquireRecordC.NeutronChB,
					sAcquireRecordC.NeutronChC,
					sAcquireRecordC.NeutronChD,
					sAcquireRecordC.GammaCh1,
					sAcquireRecordC.GammaCh2, 
					sAcquireRecordC.GammaCh3, 
					sAcquireRecordC.GammaCh4, 
					true);
			}
		}
	}

	cFormat = m_szBFileName;
//	GetDlgItemText(IDC_EDIT_LONGNAME,cFormat);
	bool bHaveEscape;
	bHaveEscape = false;
	int i;
	if (strlen(m_szCFileName))
	{
		for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
		{
			if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
				bHaveEscape = true;
			else if (bHaveEscape)
			{
				bHaveEscape = false;
				switch (cFormat[iSourceIndex]) {
				case 'P':
					{
						if (strstr(m_szCPort,"TCP/IP"))
							cResult += m_szCNet;
						else
							for (i = 0; i < (int)strlen(m_szCPort); i++)
								if ((m_szCPort[i] >=0)  && (m_szCPort[i] <= '9')) cResult += m_szCPort[i];
					}
					break;
				case 'y':
					cTemp.Format("%04d",cNow.GetYear());
					cResult += cTemp;
					break;
				case 'm':
					cTemp.Format("%02d",cNow.GetMonth());
					cResult += cTemp;
					break;
				case 'd':
					cTemp.Format("%02d",cNow.GetDay());
					cResult += cTemp;
					break;
				case 'H':
					cTemp.Format("%02d",cNow.GetHour());
					cResult += cTemp;
					break;
				case 'M':
					cTemp.Format("%02d",cNow.GetMinute());
					cResult += cTemp;
					break;
				case 'S':
					cTemp.Format("%02d",cNow.GetSecond());
					cResult += cTemp;
					break;
				default:
					cResult += cFormat[iSourceIndex];
				}
			}
			else
				cResult += cFormat[iSourceIndex];
		}
		if (cResult.FindOneOf("\\/?*:;><\"")<0)
		{
			CString cFilePathName;
			cFilePathName = m_szCDestination;
			if ((cFilePathName.Right(1) != '\\') && (cResult.Left(1) != '\\'))
				cFilePathName += '\\';
			cFilePathName += cResult;
			FILE * outfile = fopen(cFilePathName,"at");
			if (outfile)
			{
				for (int i = 0; i < (int)WParam; i++)
				{
					if (m_bCNewLine)
					{
						m_bCNewLine = false;
						fprintf(outfile,"%s ",cNow.Format("%Y\\%m\\%d %H:%M:%S"));
					}
					char ch = (*((char*)LParam+i));
					if ((m_cCPreviousChar == 0x0d) && (ch != 0x0a)) 
						fputc(0x0a,outfile);
					fputc(ch,outfile);
					if (ch == 0x0A) m_bCNewLine = true;
					m_cCPreviousChar = ch;
				}
				fclose(outfile);
			}
		}
	}
	CHARFORMAT cf;
	cf.cbSize=sizeof(cf);
	cf.dwEffects = CFE_BOLD;
	cf.dwMask = CFM_COLOR | CFM_BOLD;
	cf.crTextColor = m_iCColor;
	long start = ((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->GetTextLength();
	long end = start + WParam;
	OnReceive(WParam, LParam);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(start,-1);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSelectionCharFormat(cf);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(-1,-1);
}

LRESULT
CWatchComDlg::OnReceiveD(WPARAM WParam, LPARAM LParam)
{
	CString cResult;
	CString cTemp;
	CString cFormat;
	COleDateTime cNow = COleDateTime::GetCurrentTime();
	if (cNow.GetDay() < m_iPreviousDay)
	{
		OnButtonClear();
		m_iPreviousDay = cNow.GetDay();
	}
	if (m_bDataWatchDlg4Open)
	{
		char* pAt = (char*)LParam;
		if (*pAt++ == 'N')
		{
			if (*pAt++ == 'B')
			{
				pAt++;
				sscanf(pAt,"%f,%f,%f,%f",
					&sAcquireRecordD.NeutronChA,
					&sAcquireRecordD.NeutronChB,
					&sAcquireRecordD.NeutronChC,
					&sAcquireRecordD.NeutronChD);
			}
		}
		else if (*pAt++ == 'G')
		{
			if (*pAt++ == 'B')
			{
				pAt++;
				sscanf(pAt,"%f,%f,%f,%f",
					&sAcquireRecordD.GammaCh1,
					&sAcquireRecordD.GammaCh2,
					&sAcquireRecordD.GammaCh3,
					&sAcquireRecordD.GammaCh4);
				COleDateTime cBase(1952,1,1,0,0,0);
				COleDateTimeSpan cSpan = cNow - cBase;

				m_pDataWatchDlg1->SetData(cSpan.GetTotalSeconds(),
					sAcquireRecordD.NeutronChA,
					sAcquireRecordD.NeutronChB,
					sAcquireRecordD.NeutronChC,
					sAcquireRecordD.NeutronChD,
					sAcquireRecordD.GammaCh1,
					sAcquireRecordD.GammaCh2, 
					sAcquireRecordD.GammaCh3, 
					sAcquireRecordD.GammaCh4, 
					true);
			}
		}
	}

	cFormat = m_szDFileName;
//	GetDlgItemText(IDC_EDIT_LONGNAME,cFormat);
	bool bHaveEscape;
	bHaveEscape = false;
	int i;
	if (strlen(m_szDFileName))
	{

		for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
		{
			if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
				bHaveEscape = true;
			else if (bHaveEscape)
			{
				bHaveEscape = false;
				switch (cFormat[iSourceIndex]) {
				case 'P':
					{
						if (strstr(m_szDPort,"TCP/IP"))
							cResult += m_szDNet;
						else
						for (i = 0; i < (int)strlen(m_szDPort); i++)
							if ((m_szDPort[i] >=0)  && (m_szDPort[i] <= '9')) cResult += m_szDPort[i];
					}
					break;
				case 'y':
					cTemp.Format("%04d",cNow.GetYear());
					cResult += cTemp;
					break;
				case 'm':
					cTemp.Format("%02d",cNow.GetMonth());
					cResult += cTemp;
					break;
				case 'd':
					cTemp.Format("%02d",cNow.GetDay());
					cResult += cTemp;
					break;
				case 'H':
					cTemp.Format("%02d",cNow.GetHour());
					cResult += cTemp;
					break;
				case 'M':
					cTemp.Format("%02d",cNow.GetMinute());
					cResult += cTemp;
					break;
				case 'S':
					cTemp.Format("%02d",cNow.GetSecond());
					cResult += cTemp;
					break;
				default:
					cResult += cFormat[iSourceIndex];
				}
			}
			else
				cResult += cFormat[iSourceIndex];
		}
		if (cResult.FindOneOf("\\/?*:;><\"")<0)
		{
			CString cFilePathName;
			cFilePathName = m_szDDestination;
			if ((cFilePathName.Right(1) != '\\') && (cResult.Left(1) != '\\'))
				cFilePathName += '\\';
			cFilePathName += cResult;
			FILE * outfile = fopen(cFilePathName,"at");
			if (outfile)
			{
				for (int i = 0; i < (int)WParam; i++)
				{
					if (m_bDNewLine)
					{
						m_bDNewLine = false;
						fprintf(outfile,"%s ",cNow.Format("%Y\\%m\\%d %H:%M:%S"));
					}
					char ch = (*((char*)LParam+i));
					if ((m_cDPreviousChar == 0x0d) && (ch != 0x0a)) 
						fputc(0x0a,outfile);
					fputc(ch,outfile);
					if (ch == 0x0A) m_bDNewLine = true;
					m_cDPreviousChar = ch;
				}
				fclose(outfile);
			}
		}
	}
	CHARFORMAT cf;
	cf.cbSize=sizeof(cf);
	cf.dwEffects = CFE_BOLD;
	cf.dwMask = CFM_COLOR | CFM_BOLD;
	cf.crTextColor = m_iDColor;
	long start = ((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->GetTextLength();
	long end = start + WParam;
	OnReceive(WParam, LParam);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(start,-1);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSelectionCharFormat(cf);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(-1,-1);
}

CWatchComDlg::~CWatchComDlg()
{
	if(ComA)
	{
		ComA->UnRegister(0);
		//shutdown the com object's receiver
		ComA->DoStop();
		//get the com object to kill itself
		ComA->PostThreadMessage(IDC_COM_STOP,0,0);
		TerminateThread(ComA->m_hThread,0);
		::WaitForSingleObject(ComA->m_hThread,1000);
		delete ComA;
		//remove it from the com array
		ComA = NULL;

	}
	if (m_SocketA != INVALID_SOCKET)
	{
		shutdown(m_SocketA,SD_BOTH);
		closesocket(m_SocketA);
		m_SocketA = INVALID_SOCKET;
	}

	if(ComB)
	{
		ComB->UnRegister(0);
		//shutdown the com object's receiver
		ComB->DoStop();
		//get the com object to kill itself
		ComB->PostThreadMessage(IDC_COM_STOP,0,0);
		TerminateThread(ComB->m_hThread,0);
		::WaitForSingleObject(ComB->m_hThread,1000);
		delete ComB;
		//remove it from the com array
		ComB = NULL;
	}
	if (m_SocketB != INVALID_SOCKET)
	{
		shutdown(m_SocketB,SD_BOTH);
		closesocket(m_SocketB);
		m_SocketB = INVALID_SOCKET;
	}

	if(ComC)
	{
		ComC->UnRegister(0);
		//shutdown the com object's receiver
		ComC->DoStop();
		//get the com object to kill itself
		ComC->PostThreadMessage(IDC_COM_STOP,0,0);
		TerminateThread(ComC->m_hThread,0);
		::WaitForSingleObject(ComC->m_hThread,1000);
		delete ComC;
		//remove it from the com array
		ComC = NULL;
	}
	if (m_SocketC != INVALID_SOCKET)
	{
		shutdown(m_SocketC,SD_BOTH);
		closesocket(m_SocketC);
		m_SocketC = INVALID_SOCKET;
	}

	if(ComD)
	{
		ComD->UnRegister(0);
		//shutdown the com object's receiver
		ComD->DoStop();
		//get the com object to kill itself
		ComD->PostThreadMessage(IDC_COM_STOP,0,0);
		TerminateThread(ComD->m_hThread,0);
		::WaitForSingleObject(ComD->m_hThread,1000);
		delete ComD;
		//remove it from the com array
		ComD = NULL;
	}
	if (m_SocketD != INVALID_SOCKET)
	{
		shutdown(m_SocketD,SD_BOTH);
		closesocket(m_SocketD);
		m_SocketD = INVALID_SOCKET;
	}

	delete m_pDataWatchDlg1;
	delete m_pDataWatchDlg2;
	delete m_pDataWatchDlg3;
	delete m_pDataWatchDlg4;
}

void CWatchComDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == IDM_COMA)
	{
		OnButtonA();
	}
	else if (nID == IDM_COMB)
	{
		OnButtonB();
	}
	else if (nID == IDM_COMC)
	{
		OnButtonC();
	}
	else if (nID == IDM_COMD)
	{
		OnButtonD();
	}
	else if (nID == IDM_CLEARLOG)
	{
		OnButtonClear();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWatchComDlg::OnPaint() 
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
HCURSOR CWatchComDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWatchComDlg::OnButtonA() 
{
	// TODO: Add your control notification handler code here
	CSelect Select;
	Select.m_dBaudRate = m_dABaudRate;
	Select.m_dDataBits = m_dADataBits;
	Select.m_dStopBits = m_dAStopBits;
	Select.m_uiTCPPort = m_uiANetPort;

	strcpy(Select.m_szDestination, m_szADestination);
	strcpy(Select.m_szFileName, m_szAFileName);
	strcpy(Select.m_szPort,m_szAPort);
	strcpy(Select.m_szIPAddress,m_szANet);

	switch (m_szAParity[0]) {
	case 'o'://ODD
	case 'O':
		Select.m_dParity = 0;
		break;
	case 'e'://EVEN
	case 'E':
		Select.m_dParity = 1;
		break;
	case 'm'://MARK
	case 'M':
		Select.m_dParity = 2;
		break;
	default://NONE
		Select.m_dParity = 3;
	}

	KillTimer(RESTARTATIMER);
	KillTimer(CONNECTATIMER);

	if (Select.DoModal() == IDOK) 
	{
		if (ComA)
		{
			ComA->UnRegister(0);
			//shutdown the com object's receiver
			ComA->DoStop();
			//get the com object to kill itself
			::WaitForSingleObject(ComA->m_hThread,5000);
			ComA->PostThreadMessage(IDC_COM_STOP,0,0);
			TerminateThread(ComA->m_hThread,0);
			delete ComA;
			//remove it from the com array
			ComA = NULL;
		}
		if (m_SocketA != INVALID_SOCKET)
		{
			shutdown(m_SocketA,SD_BOTH);
			closesocket(m_SocketA);
			m_SocketA = INVALID_SOCKET;
		}
		SetDlgItemText(IDC_STATIC_COMA,"Com A: Not set.");
	
		//set it up
		char szTemp[64];

		WritePrivateProfileString("COM_A","DEACTIVATE",Select.m_bDeactivate?"Yes":"No",m_szIniFile);

		strcpy(m_szANet,Select.m_szIPAddress);
		WritePrivateProfileString("COM_A","TCPADDRESS",m_szANet,m_szIniFile);

		m_uiANetPort = Select.m_uiTCPPort;
		WritePrivateProfileString("COM_A","TCPPORT",itoa(Select.m_uiTCPPort,szTemp,10),m_szIniFile);

		strcpy(m_szADestination,Select.m_szDestination);
		WritePrivateProfileString("COM_A","FILELOCATION",m_szADestination,m_szIniFile);

		strcpy(m_szAFileName,Select.m_szFileName);
		WritePrivateProfileString("COM_A","FILENAME",m_szAFileName,m_szIniFile);

		strcpy(m_szAPort,Select.m_szPort);
		WritePrivateProfileString("COM_A","PORT",m_szAPort,m_szIniFile);

		switch (Select.m_dParity) {
		case 0:
			strcpy(m_szAParity,"ODD");
			break;
		case 1:
			strcpy(m_szAParity,"EVEN");
			break;
		case 2:
			strcpy(m_szAParity,"MARK");
			break;
		default:
			strcpy(m_szAParity,"NONE");
		}
		WritePrivateProfileString("COM_A","PARITY",m_szAParity,m_szIniFile);

		m_dABaudRate = Select.m_dBaudRate;
		WritePrivateProfileString("COM_A","BAUD",itoa(m_dABaudRate,szTemp,10),m_szIniFile);

		m_dADataBits = Select.m_dDataBits;
		WritePrivateProfileString("COM_A","DATABITS",itoa(m_dADataBits,szTemp,10),m_szIniFile);

		m_dAStopBits = Select.m_dStopBits;
		WritePrivateProfileString("COM_A","STOPBITS",itoa(m_dAStopBits,szTemp,10),m_szIniFile);

		if (Select.m_bDeactivate == false)
		{
			if (strstr(m_szAPort,"TCP/IP") == NULL)
			{
				if (strlen(m_szAPort) > 0)
				{

					ComA = (CSerial*)AfxBeginThread(RUNTIME_CLASS(CSerial));
					//set it up
					sprintf(szTemp,"Com A: %s, %d, %d, %d, %s",
						m_szAPort,
						m_dABaudRate,
						m_dADataBits,
						m_dAStopBits,
						m_szAParity);

					if (ComA->SetUpPort(m_szAPort,m_dABaudRate,
							m_dADataBits, m_dAStopBits, m_szAParity))
					{
						//connect to it
						ComA->DoSetMessage(1);
						ComA->Register(this);
						ComA->PostThreadMessage(IDC_COM_START,0,0);
						SetDlgItemText(IDC_STATIC_COMA,szTemp);
					}
					else
					{
						CString st("Could not set up port\n");
						st += szTemp;

						//shutdown the com object's receiver
						ComA->DoStop();
						//get the com object to kill itself
						ComA->PostThreadMessage(IDC_COM_STOP,0,0);
						::WaitForSingleObject(ComA->m_hThread,5000);
						TerminateThread(ComA->m_hThread,0);
						delete ComA;
						ComA = NULL;

						SetDlgItemText(IDC_STATIC_COMA,"Com A: FAILED");
					}
				}
			}
			else
			{
				if (m_iRc == 0)
				{
					m_SocketA = socket(PF_INET,SOCK_STREAM,0);
					if (m_SocketA != INVALID_SOCKET)
					{
						int iResult = WSAAsyncSelect(m_SocketA, m_hWnd, WM_WSARECVA, FD_READ | FD_CONNECT | FD_CLOSE);
						if (iResult == SOCKET_ERROR)
						{
							SetDlgItemText(IDC_STATIC_COMA,"Socket Error");
						}
						else
						{
							m_sAddrA.sin_family=PF_INET;
							m_sAddrA.sin_port=htons(m_uiANetPort);
							m_sAddrA.sin_addr.s_addr = inet_addr(m_szANet);
							int iRc = connect(m_SocketA,(sockaddr*)&m_sAddrA,sizeof(m_sAddrA));
							DWORD dwResult = GetLastError();
							if (dwResult == WSAEWOULDBLOCK)
							{
								SetDlgItemText(IDC_STATIC_COMA,"Connecting");
								SetTimer(CONNECTATIMER,CONNECTTIMERTIME,NULL);
							}
							else
							{
								char szError[256];
								sprintf(szError, "Connect error: %d", dwResult);
								SetDlgItemText(IDC_STATIC_COMA,szError);
								SetTimer(RESTARTATIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer A started BUTTONA\r\n");
							}
						}
					}
					else
					{
						char szError[256];
						DWORD dwResult = GetLastError();
						sprintf(szError, "Socket error: %d", dwResult);
						SetDlgItemText(IDC_STATIC_COMA,szError);
						SetTimer(RESTARTATIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer A started BUTTONA\r\n");
					}
				}
				else
					SetDlgItemText(IDC_STATIC_COMA,"Cannot start network.");
			}
		}
	}
}

void CWatchComDlg::OnButtonB() 
{
	// TODO: Add your control notification handler code here
	CSelect Select;
	Select.m_dBaudRate = m_dBBaudRate;
	Select.m_dDataBits = m_dBDataBits;
	Select.m_dStopBits = m_dBStopBits;
	Select.m_uiTCPPort = m_uiBNetPort;

	strcpy(Select.m_szDestination, m_szBDestination);
	strcpy(Select.m_szFileName, m_szBFileName);
	strcpy(Select.m_szPort,m_szBPort);
	strcpy(Select.m_szIPAddress,m_szBNet);

	switch (m_szBParity[0]) {
	case 'o':
	case 'O':
		Select.m_dParity = 0;
		break;
	case 'e':
	case 'E':
		Select.m_dParity = 1;
		break;
	case 'm':
	case 'M':
		Select.m_dParity = 2;
		break;
	default:
		Select.m_dParity = 3;
	}

	KillTimer(RESTARTBTIMER);
	KillTimer(CONNECTBTIMER);

	if (Select.DoModal() == IDOK)
	{
		if (ComB)
		{
			ComB->UnRegister(0);
			//shutdown the com object's receiver
			ComB->DoStop();
			//get the com object to kill itself
			::WaitForSingleObject(ComB->m_hThread,5000);
			ComB->PostThreadMessage(IDC_COM_STOP,0,0);
			TerminateThread(ComB->m_hThread,0);
			delete ComB;
			//remove it from the com array
			ComB = NULL;
		}
		if (m_SocketB != INVALID_SOCKET)
		{
			shutdown(m_SocketB,SD_BOTH);
			closesocket(m_SocketB);
			m_SocketB = INVALID_SOCKET;
		}
		SetDlgItemText(IDC_STATIC_COMB,"Com B: Not Set.");

		//set it up
		char szTemp[64];

		WritePrivateProfileString("COM_B","DEACTIVATE",Select.m_bDeactivate?"Yes":"No",m_szIniFile);

		strcpy(m_szBNet,Select.m_szIPAddress);
		WritePrivateProfileString("COM_B","TCPADDRESS",m_szBNet,m_szIniFile);

		m_uiBNetPort = Select.m_uiTCPPort;
		WritePrivateProfileString("COM_B","TCPPORT",itoa(Select.m_uiTCPPort,szTemp,10),m_szIniFile);

		strcpy(m_szBDestination,Select.m_szDestination);
		WritePrivateProfileString("COM_B","FILELOCATION",m_szBDestination,m_szIniFile);

		strcpy(m_szBFileName,Select.m_szFileName);
		WritePrivateProfileString("COM_B","FILENAME",m_szBFileName,m_szIniFile);

		strcpy(m_szBPort,Select.m_szPort);
		WritePrivateProfileString("COM_B","PORT",m_szBPort,m_szIniFile);

		switch (Select.m_dParity) {
		case 0:
			strcpy(m_szBParity,"ODD");
			break;
		case 1:
			strcpy(m_szBParity,"EVEN");
			break;
		case 2:
			strcpy(m_szBParity,"MARK");
			break;
		default:
			strcpy(m_szBParity,"NONE");
		}
		WritePrivateProfileString("COM_B","PARITY",m_szBParity,m_szIniFile);

		m_dBBaudRate = Select.m_dBaudRate;
		WritePrivateProfileString("COM_B","BAUD",itoa(m_dBBaudRate,szTemp,10),m_szIniFile);

		m_dBDataBits = Select.m_dDataBits;
		WritePrivateProfileString("COM_B","DATABITS",itoa(m_dBDataBits,szTemp,10),m_szIniFile);

		m_dBStopBits = Select.m_dStopBits;
		WritePrivateProfileString("COM_B","STOPBITS",itoa(m_dBStopBits,szTemp,10),m_szIniFile);

		if (Select.m_bDeactivate == false)
		{
			if (strstr(m_szBPort,"TCP/IP") == NULL)
			{
				if (strlen(m_szBPort) > 0)
				{

					ComB = (CSerial*)AfxBeginThread(RUNTIME_CLASS(CSerial));
					//set it up
					sprintf(szTemp,"Com B: %s, %d, %d, %d, %s",
						m_szBPort,
						m_dBBaudRate,
						m_dBDataBits,
						m_dBStopBits,
						m_szBParity);

					if (ComB->SetUpPort(m_szBPort,m_dBBaudRate,
							m_dBDataBits, m_dBStopBits, m_szBParity))
					{
						//connect to it
						ComB->DoSetMessage(2);
						ComB->Register(this);
						ComB->PostThreadMessage(IDC_COM_START,0,0);
						SetDlgItemText(IDC_STATIC_COMB,szTemp);
					}
					else
					{
						CString st("Could not set up port\n");
						st += szTemp;

						//shutdown the com object's receiver
						ComB->DoStop();
						//get the com object to kill itself
						ComB->PostThreadMessage(IDC_COM_STOP,0,0);
						::WaitForSingleObject(ComB->m_hThread,5000);
						TerminateThread(ComB->m_hThread,0);
						delete ComB;
						ComB = NULL;

						SetDlgItemText(IDC_STATIC_COMB,"Com B: FAILED");
					}
				}
			}
			else
			{
				if (m_iRc == 0)
				{
					m_SocketB = socket(PF_INET,SOCK_STREAM,0);
					if (m_SocketB != INVALID_SOCKET)
					{
						int iResult = WSAAsyncSelect(m_SocketB, m_hWnd, WM_WSARECVB, FD_READ | FD_CONNECT | FD_CLOSE);
						if (iResult == SOCKET_ERROR)
						{
							SetDlgItemText(IDC_STATIC_COMB,"Socket Error");
						}
						else
						{
							m_sAddrB.sin_family=PF_INET;
							m_sAddrB.sin_port=htons(m_uiBNetPort);
							m_sAddrB.sin_addr.s_addr = inet_addr(m_szBNet);
							int iRc = connect(m_SocketB,(sockaddr*)&m_sAddrB,sizeof(m_sAddrB));
							DWORD dwResult = GetLastError();
							if (dwResult == WSAEWOULDBLOCK)
							{
								SetDlgItemText(IDC_STATIC_COMB,"Connecting");
								SetTimer(CONNECTBTIMER,CONNECTTIMERTIME,NULL);
							}
							else
							{
								char szError[256];
								sprintf(szError, "Connect error: %d", dwResult);
								SetDlgItemText(IDC_STATIC_COMB,szError);
								SetTimer(RESTARTBTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer B started BUTTONB\r\n");
							}
						}
					}
					else
					{
						char szError[256];
						DWORD dwResult = GetLastError();
						sprintf(szError, "Socket error: %d", dwResult);
						SetDlgItemText(IDC_STATIC_COMB,szError);
						SetTimer(RESTARTBTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer B started BUTTONB\r\n");
					}
				}
				else
					SetDlgItemText(IDC_STATIC_COMB,"Cannot start network.");
			}
		}
	}
}

void CWatchComDlg::OnButtonC() 
{
	// TODO: Add your control notification handler code here
	CSelect Select;
	Select.m_dBaudRate = m_dCBaudRate;
	Select.m_dDataBits = m_dCDataBits;
	Select.m_dStopBits = m_dCStopBits;
	Select.m_uiTCPPort = m_uiCNetPort;

	strcpy(Select.m_szDestination, m_szCDestination);
	strcpy(Select.m_szFileName, m_szCFileName);
	strcpy(Select.m_szPort,m_szCPort);
	strcpy(Select.m_szIPAddress,m_szCNet);

	switch (m_szCParity[0]) {
	case 'o':
	case 'O':
		Select.m_dParity = 0;
		break;
	case 'e':
	case 'E':
		Select.m_dParity = 1;
		break;
	case 'm':
	case 'M':
		Select.m_dParity = 2;
		break;
	default:
		Select.m_dParity = 3;
	}

	KillTimer(RESTARTCTIMER);
	KillTimer(CONNECTCTIMER);

	if (Select.DoModal() == IDOK)
	{
		if (ComC)
		{
			ComC->UnRegister(0);
			//shutdown the com object's receiver
			ComC->DoStop();
			//get the com object to kill itself
			::WaitForSingleObject(ComC->m_hThread,5000);
			ComC->PostThreadMessage(IDC_COM_STOP,0,0);
			TerminateThread(ComC->m_hThread,0);
			delete ComC;
			//remove it from the com array
			ComC = NULL;
		}
		if (m_SocketC != INVALID_SOCKET)
		{
			shutdown(m_SocketC,SD_BOTH);
			closesocket(m_SocketC);
			m_SocketC = INVALID_SOCKET;
		}
		SetDlgItemText(IDC_STATIC_COMC,"Com C: Not Set.");

		//set it up
		char szTemp[64];

		WritePrivateProfileString("COM_C","DEACTIVATE",Select.m_bDeactivate?"Yes":"No",m_szIniFile);

		strcpy(m_szCNet,Select.m_szIPAddress);
		WritePrivateProfileString("COM_C","TCPADDRESS",m_szCNet,m_szIniFile);

		m_uiCNetPort = Select.m_uiTCPPort;
		WritePrivateProfileString("COM_C","TCPPORT",itoa(Select.m_uiTCPPort,szTemp,10),m_szIniFile);

		strcpy(m_szCDestination,Select.m_szDestination);
		WritePrivateProfileString("COM_C","FILELOCATION",m_szCDestination,m_szIniFile);

		strcpy(m_szCFileName,Select.m_szFileName);
		WritePrivateProfileString("COM_C","FILENAME",m_szCFileName,m_szIniFile);

		strcpy(m_szCPort,Select.m_szPort);
		WritePrivateProfileString("COM_C","PORT",m_szCPort,m_szIniFile);

		switch (Select.m_dParity) {
		case 0:
			strcpy(m_szCParity,"ODD");
			break;
		case 1:
			strcpy(m_szCParity,"EVEN");
			break;
		case 2:
			strcpy(m_szCParity,"MARK");
			break;
		default:
			strcpy(m_szCParity,"NONE");
		}
		WritePrivateProfileString("COM_C","PARITY",m_szCParity,m_szIniFile);

		m_dCBaudRate = Select.m_dBaudRate;
		WritePrivateProfileString("COM_C","BAUD",itoa(m_dCBaudRate,szTemp,10),m_szIniFile);

		m_dCDataBits = Select.m_dDataBits;
		WritePrivateProfileString("COM_C","DATABITS",itoa(m_dCDataBits,szTemp,10),m_szIniFile);

		m_dBStopBits = Select.m_dStopBits;
		WritePrivateProfileString("COM_C","STOPBITS",itoa(m_dCStopBits,szTemp,10),m_szIniFile);

		if (Select.m_bDeactivate == false)
		{
			if (strstr(m_szCPort,"TCP/IP") == NULL)
			{
				if (strlen(m_szCPort) > 0)
				{

					ComC = (CSerial*)AfxBeginThread(RUNTIME_CLASS(CSerial));
					//set it up
					sprintf(szTemp,"Com C: %s, %d, %d, %d, %s",
						m_szCPort,
						m_dCBaudRate,
						m_dCDataBits,
						m_dCStopBits,
						m_szCParity);

					if (ComC->SetUpPort(m_szCPort,m_dCBaudRate,
							m_dCDataBits, m_dCStopBits, m_szCParity))
					{
						//connect to it
						ComC->DoSetMessage(3);
						ComC->Register(this);
						ComC->PostThreadMessage(IDC_COM_START,0,0);
						SetDlgItemText(IDC_STATIC_COMC,szTemp);
					}
					else
					{
						CString st("Could not set up port\n");
						st += szTemp;

						//shutdown the com object's receiver
						ComC->DoStop();
						//get the com object to kill itself
						ComC->PostThreadMessage(IDC_COM_STOP,0,0);
						::WaitForSingleObject(ComC->m_hThread,5000);
						TerminateThread(ComC->m_hThread,0);
						delete ComC;
						ComC = NULL;

						SetDlgItemText(IDC_STATIC_COMC,"Com C: FAILED");
					}
				}
			}
			else
			{
				if (m_iRc == 0)
				{
					m_SocketC = socket(PF_INET,SOCK_STREAM,0);
					if (m_SocketC != INVALID_SOCKET)
					{
						int iResult = WSAAsyncSelect(m_SocketC, m_hWnd, WM_WSARECVC, FD_READ | FD_CONNECT | FD_CLOSE);
						if (iResult == SOCKET_ERROR)
						{
							SetDlgItemText(IDC_STATIC_COMC,"Socket Error");
						}
						else
						{
							m_sAddrC.sin_family=PF_INET;
							m_sAddrC.sin_port=htons(m_uiCNetPort);
							m_sAddrC.sin_addr.s_addr = inet_addr(m_szCNet);
							int iRc = connect(m_SocketC,(sockaddr*)&m_sAddrC,sizeof(m_sAddrC));
							DWORD dwResult = GetLastError();
							if (dwResult == WSAEWOULDBLOCK)
							{
								SetDlgItemText(IDC_STATIC_COMC,"Connecting");
								SetTimer(CONNECTCTIMER,CONNECTTIMERTIME,NULL);
							}
							else
							{
								char szError[256];
								sprintf(szError, "Connect error: %d", dwResult);
								SetDlgItemText(IDC_STATIC_COMC,szError);
								SetTimer(RESTARTCTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer C started BUTTONC\r\n");
							}
						}
					}
					else
					{
						char szError[256];
						DWORD dwResult = GetLastError();
						sprintf(szError, "Socket error: %d", dwResult);
						SetDlgItemText(IDC_STATIC_COMC,szError);
						SetTimer(RESTARTCTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer C started BUTTONC\r\n");
					}
				}
				else
					SetDlgItemText(IDC_STATIC_COMC,"Cannot start network.");
			}
		}
	}
}

void CWatchComDlg::OnButtonD() 
{
	// TODO: Add your control notification handler code here
	CSelect Select;
	Select.m_dBaudRate = m_dDBaudRate;
	Select.m_dDataBits = m_dDDataBits;
	Select.m_dStopBits = m_dDStopBits;
	Select.m_uiTCPPort = m_uiDNetPort;

	strcpy(Select.m_szDestination, m_szDDestination);
	strcpy(Select.m_szFileName, m_szDFileName);
	strcpy(Select.m_szPort,m_szDPort);
	strcpy(Select.m_szIPAddress,m_szDNet);

	switch (m_szDParity[0]) {
	case 'o':
	case 'O':
		Select.m_dParity = 0;
		break;
	case 'e':
	case 'E':
		Select.m_dParity = 1;
		break;
	case 'm':
	case 'M':
		Select.m_dParity = 2;
		break;
	default:
		Select.m_dParity = 3;
	}

	KillTimer(RESTARTDTIMER);
	KillTimer(CONNECTDTIMER);

	if (Select.DoModal() == IDOK)
	{
		if (ComD)
		{
			ComD->UnRegister(0);
			//shutdown the com object's receiver
			ComD->DoStop();
			//get the com object to kill itself
			::WaitForSingleObject(ComD->m_hThread,5000);
			ComD->PostThreadMessage(IDC_COM_STOP,0,0);
			TerminateThread(ComD->m_hThread,0);
			delete ComD;
			//remove it from the com array
			ComD = NULL;
		}
		if (m_SocketD != INVALID_SOCKET)
		{
			shutdown(m_SocketD,SD_BOTH);
			closesocket(m_SocketD);
			m_SocketD = INVALID_SOCKET;
		}
		SetDlgItemText(IDC_STATIC_COMD,"Com D: Not Set.");

		//set it up
		char szTemp[64];

		WritePrivateProfileString("COM_D","DEACTIVATE",Select.m_bDeactivate?"Yes":"No",m_szIniFile);

		strcpy(m_szDNet,Select.m_szIPAddress);
		WritePrivateProfileString("COM_D","TCPADDRESS",m_szDNet,m_szIniFile);

		m_uiDNetPort = Select.m_uiTCPPort;
		WritePrivateProfileString("COM_D","TCPPORT",itoa(Select.m_uiTCPPort,szTemp,10),m_szIniFile);

		strcpy(m_szDDestination,Select.m_szDestination);
		WritePrivateProfileString("COM_D","FILELOCATION",m_szDDestination,m_szIniFile);

		strcpy(m_szDFileName,Select.m_szFileName);
		WritePrivateProfileString("COM_D","FILENAME",m_szDFileName,m_szIniFile);

		strcpy(m_szDPort,Select.m_szPort);
		WritePrivateProfileString("COM_D","PORT",m_szDPort,m_szIniFile);

		switch (Select.m_dParity) {
		case 0:
			strcpy(m_szDParity,"ODD");
			break;
		case 1:
			strcpy(m_szDParity,"EVEN");
			break;
		case 2:
			strcpy(m_szDParity,"MARK");
			break;
		default:
			strcpy(m_szDParity,"NONE");
		}
		WritePrivateProfileString("COM_D","PARITY",m_szDParity,m_szIniFile);

		m_dDBaudRate = Select.m_dBaudRate;
		WritePrivateProfileString("COM_D","BAUD",itoa(m_dDBaudRate,szTemp,10),m_szIniFile);

		m_dDDataBits = Select.m_dDataBits;
		WritePrivateProfileString("COM_D","DATABITS",itoa(m_dDDataBits,szTemp,10),m_szIniFile);

		m_dDStopBits = Select.m_dStopBits;
		WritePrivateProfileString("COM_D","STOPBITS",itoa(m_dDStopBits,szTemp,10),m_szIniFile);

		if (Select.m_bDeactivate == false)
		{
			if (strstr(m_szDPort,"TCP/IP") == NULL)
			{
				if (strlen(m_szDPort) > 0)
				{

					ComD = (CSerial*)AfxBeginThread(RUNTIME_CLASS(CSerial));
					//set it up
					sprintf(szTemp,"Com D: %s, %d, %d, %d, %s",
						m_szDPort,
						m_dDBaudRate,
						m_dDDataBits,
						m_dDStopBits,
						m_szDParity);

					if (ComD->SetUpPort(m_szDPort,m_dDBaudRate,
							m_dDDataBits, m_dDStopBits, m_szDParity))
					{
						//connect to it
						ComD->DoSetMessage(4);
						ComD->Register(this);
						ComD->PostThreadMessage(IDC_COM_START,0,0);
						SetDlgItemText(IDC_STATIC_COMD,szTemp);
					}
					else
					{
						CString st("Could not set up port\n");
						st += szTemp;

						//shutdown the com object's receiver
						ComD->DoStop();
						//get the com object to kill itself
						ComD->PostThreadMessage(IDC_COM_STOP,0,0);
						::WaitForSingleObject(ComD->m_hThread,5000);
						TerminateThread(ComD->m_hThread,0);
						delete ComD;
						ComD = NULL;

						SetDlgItemText(IDC_STATIC_COMD,"Com D: FAILED");
					}
				}
			}
			else
			{
				if (m_iRc == 0)
				{
					m_SocketD = socket(PF_INET,SOCK_STREAM,0);
					if (m_SocketD != INVALID_SOCKET)
					{
						int iResult = WSAAsyncSelect(m_SocketD, m_hWnd, WM_WSARECVD, FD_READ | FD_CONNECT | FD_CLOSE);
						if (iResult == SOCKET_ERROR)
						{
							SetDlgItemText(IDC_STATIC_COMD,"Socket Error");
						}
						else
						{
							m_sAddrD.sin_family=PF_INET;
							m_sAddrD.sin_port=htons(m_uiDNetPort);
							m_sAddrD.sin_addr.s_addr = inet_addr(m_szDNet);
							int iRc = connect(m_SocketD,(sockaddr*)&m_sAddrD,sizeof(m_sAddrD));
							DWORD dwResult = GetLastError();
							if (dwResult == WSAEWOULDBLOCK)
							{
								SetDlgItemText(IDC_STATIC_COMD,"Connecting");
								SetTimer(CONNECTDTIMER,CONNECTTIMERTIME,NULL);
							}
							else
							{
								char szError[256];
								sprintf(szError, "Connect error: %d", dwResult);
								SetDlgItemText(IDC_STATIC_COMD,szError);
								SetTimer(RESTARTDTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer D started BUTTOND\r\n");
							}
						}
					}
					else
					{
						char szError[256];
						DWORD dwResult = GetLastError();
						sprintf(szError, "Socket error: %d", dwResult);
						SetDlgItemText(IDC_STATIC_COMD,szError);
						SetTimer(RESTARTDTIMER,RESTARTTIMERTIME,NULL);
	//TRACE("Restart timer D started BUTTOND\r\n");
					}
				}
				else
					SetDlgItemText(IDC_STATIC_COMD,"Cannot start network.");
			}
		}
	}
}

HBRUSH CWatchComDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if (index == IDC_STATIC_COMA)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_iAColor);
		pDC->SetBkColor(COLOR_BKGROUND);
	}
	if (index == IDC_STATIC_COMB)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_iBColor);
		pDC->SetBkColor(COLOR_BKGROUND);
	}
	if (index == IDC_STATIC_COMC)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_iCColor);
		pDC->SetBkColor(COLOR_BKGROUND);
	}
	if (index == IDC_STATIC_COMD)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_iDColor);
		pDC->SetBkColor(COLOR_BKGROUND);
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CWatchComDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(0,-1);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->ReplaceSel("",false);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT1))->SetSel(0,0);
}

void CWatchComDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	CString text;
	GetDlgItemText(IDC_RICHEDIT1,text);
	FILE *outfile;
	CFileDialog FileDialog(FALSE,"rtf","",
	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
	"Rich Text Files (*.rtf)|*.rtf|All Files (*.*)|*.*||",this);
	if (FileDialog.DoModal()!=IDCANCEL)
	{
		CString filename = FileDialog.GetPathName();
		outfile = fopen(filename,"w");
		for (int i = 0; i < text.GetLength(); i++)
			fputc(text[i],outfile);
		fclose(outfile);
	}
}
void CWatchComDlg::OnSizing(UINT nSide, LPRECT lpRect)
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

void CWatchComDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	SetTimer(RESIZETIMER,200,NULL);
	
}

void CWatchComDlg::ResizeChildren()
{
	CRect rectClient,rectChart;
	GetClientRect(&rectClient);
	GetDlgItem(IDC_RICHEDIT1)->GetWindowRect(&rectChart);
	ScreenToClient(&rectChart);
	GetDlgItem(IDC_RICHEDIT1)->MoveWindow(
		rectChart.left,
		rectChart.top,
		rectClient.right - rectChart.left - rectChart.left,
		rectClient.bottom - rectChart.top - 10, true);
}

void CWatchComDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == RESIZETIMER)
	{
		ResizeChildren();
		KillTimer(RESIZETIMER);
		Invalidate();
	}
	else if (nIDEvent == CONNECTATIMER)
	{
		KillTimer(CONNECTATIMER);
		if (m_SocketA != INVALID_SOCKET)
		{
			shutdown(m_SocketA,SD_BOTH);
			closesocket(m_SocketA);
			m_SocketA = INVALID_SOCKET;
			SetTimer(RESTARTATIMER,RESTARTTIMERTIME,NULL);
			SetDlgItemText(IDC_STATIC_COMA,"Timeout, Restarting");
//TRACE("Restart timer A started CONNECTA\r\n");
		}
	}
	else if (nIDEvent == CONNECTBTIMER)
	{
		KillTimer(CONNECTBTIMER);
		if (m_SocketB != INVALID_SOCKET)
		{
			shutdown(m_SocketB,SD_BOTH);
			closesocket(m_SocketB);
			m_SocketB = INVALID_SOCKET;
			SetTimer(RESTARTBTIMER,RESTARTTIMERTIME,NULL);
			SetDlgItemText(IDC_STATIC_COMB,"Timeout, Restarting");
//TRACE("Restart timer B started CONNECTB\r\n");
		}
	}
	else if (nIDEvent == CONNECTCTIMER)
	{
		KillTimer(CONNECTCTIMER);
		if (m_SocketC != INVALID_SOCKET)
		{
			shutdown(m_SocketC,SD_BOTH);
			closesocket(m_SocketC);
			m_SocketC = INVALID_SOCKET;
			SetTimer(RESTARTCTIMER,RESTARTTIMERTIME,NULL);
			SetDlgItemText(IDC_STATIC_COMC,"Timeout, Restarting");
//TRACE("Restart timer C started CONNECTC\r\n");
		}
	}
	else if (nIDEvent == CONNECTDTIMER)
	{
		KillTimer(CONNECTDTIMER);
		if (m_SocketD != INVALID_SOCKET)
		{
			shutdown(m_SocketD,SD_BOTH);
			closesocket(m_SocketD);
			m_SocketD = INVALID_SOCKET;
			SetTimer(RESTARTDTIMER,RESTARTTIMERTIME,NULL);
			SetDlgItemText(IDC_STATIC_COMD,"Timeout, Restarting");
//TRACE("Restart timer D started CONNECTD\r\n");
		}
	}
	else if (nIDEvent == RESTARTATIMER)
	{
//TRACE("Restart timer A started RESTARTATIMER\r\n");
		if (m_SocketA == INVALID_SOCKET)
		{
			m_SocketA = socket(PF_INET,SOCK_STREAM,0);
			if (m_SocketA != INVALID_SOCKET)
			{
				int iResult = WSAAsyncSelect(m_SocketA, m_hWnd, WM_WSARECVA, FD_READ | FD_CONNECT | FD_CLOSE);
				if (iResult == SOCKET_ERROR)
				{
					SetDlgItemText(IDC_STATIC_COMA,"Socket Error");
				}
				else
				{
					m_sAddrA.sin_family=PF_INET;
					m_sAddrA.sin_port=htons(m_uiANetPort);
					m_sAddrA.sin_addr.s_addr = inet_addr(m_szANet);
					int iRc = connect(m_SocketA,(sockaddr*)&m_sAddrA,sizeof(m_sAddrA));
					DWORD dwResult = GetLastError();
					if (dwResult == WSAEWOULDBLOCK)
					{
						SetDlgItemText(IDC_STATIC_COMA,"Connecting");
						KillTimer(RESTARTATIMER);
						SetTimer(CONNECTATIMER,CONNECTTIMERTIME,NULL);
					}
					else
					{
						char szError[256];
						sprintf(szError, "Connect error: %d", dwResult);
						SetDlgItemText(IDC_STATIC_COMA,szError);
					}
				}
			}
			else
			{
				char szError[256];
				DWORD dwResult = GetLastError();
				sprintf(szError, "Socket error: %d", dwResult);
				SetDlgItemText(IDC_STATIC_COMA,szError);
			}
		}
	}
	else if (nIDEvent == RESTARTBTIMER)
	{
//TRACE("Restart timer B started RESTARTBTIMER\r\n");
		if (m_SocketB == INVALID_SOCKET)
		{
			m_SocketB = socket(PF_INET,SOCK_STREAM,0);
			if (m_SocketB != INVALID_SOCKET)
			{
				int iResult = WSAAsyncSelect(m_SocketB, m_hWnd, WM_WSARECVB, FD_READ | FD_CONNECT | FD_CLOSE);
				if (iResult == SOCKET_ERROR)
				{
					SetDlgItemText(IDC_STATIC_COMB,"Socket Error");
				}
				else
				{
					m_sAddrB.sin_family=PF_INET;
					m_sAddrB.sin_port=htons(m_uiBNetPort);
					m_sAddrB.sin_addr.s_addr = inet_addr(m_szBNet);
					int iRc = connect(m_SocketB,(sockaddr*)&m_sAddrB,sizeof(m_sAddrB));
					DWORD dwResult = GetLastError();
					if (dwResult == WSAEWOULDBLOCK)
					{
						SetDlgItemText(IDC_STATIC_COMB,"Connecting");
						KillTimer(RESTARTBTIMER);
						SetTimer(CONNECTBTIMER,CONNECTTIMERTIME,NULL);
					}
					else
					{
						char szError[256];
						sprintf(szError, "Connect error: %d", dwResult);
						SetDlgItemText(IDC_STATIC_COMB,szError);
					}
				}
			}
			else
			{
				char szError[256];
				DWORD dwResult = GetLastError();
				sprintf(szError, "Socket error: %d", dwResult);
				SetDlgItemText(IDC_STATIC_COMB,szError);
			}
		}
	}
	else if (nIDEvent == RESTARTCTIMER)
	{
//TRACE("Restart timer C started RESTARTCTIMER\r\n");
		if (m_SocketC == INVALID_SOCKET)
		{
			m_SocketC = socket(PF_INET,SOCK_STREAM,0);
			if (m_SocketC != INVALID_SOCKET)
			{
				int iResult = WSAAsyncSelect(m_SocketC, m_hWnd, WM_WSARECVC, FD_READ | FD_CONNECT | FD_CLOSE);
				if (iResult == SOCKET_ERROR)
				{
					SetDlgItemText(IDC_STATIC_COMC,"Socket Error");
				}
				else
				{
					m_sAddrC.sin_family=PF_INET;
					m_sAddrC.sin_port=htons(m_uiCNetPort);
					m_sAddrC.sin_addr.s_addr = inet_addr(m_szCNet);
					int iRc = connect(m_SocketC,(sockaddr*)&m_sAddrC,sizeof(m_sAddrC));
					DWORD dwResult = GetLastError();
					if (dwResult == WSAEWOULDBLOCK)
					{
						SetDlgItemText(IDC_STATIC_COMC,"Connecting");
						KillTimer(RESTARTCTIMER);
						SetTimer(CONNECTCTIMER,CONNECTTIMERTIME,NULL);
					}
					else
					{
						char szError[256];
						sprintf(szError, "Connect error: %d", dwResult);
						SetDlgItemText(IDC_STATIC_COMC,szError);
					}
				}
			}
			else
			{
				char szError[256];
				DWORD dwResult = GetLastError();
				sprintf(szError, "Socket error: %d", dwResult);
				SetDlgItemText(IDC_STATIC_COMC,szError);
			}
		}
	}
	else if (nIDEvent == RESTARTDTIMER)
	{
//TRACE("Restart timer D started RESTARTDTIMER\r\n");
		if (m_SocketD == INVALID_SOCKET)
		{
			m_SocketD = socket(PF_INET,SOCK_STREAM,0);
			if (m_SocketD != INVALID_SOCKET)
			{
				int iResult = WSAAsyncSelect(m_SocketD, m_hWnd, WM_WSARECVD, FD_READ | FD_CONNECT | FD_CLOSE);
				if (iResult == SOCKET_ERROR)
				{
					SetDlgItemText(IDC_STATIC_COMD,"Socket Error");
				}
				else
				{
					m_sAddrD.sin_family=PF_INET;
					m_sAddrD.sin_port=htons(m_uiDNetPort);
					m_sAddrD.sin_addr.s_addr = inet_addr(m_szDNet);
					int iRc = connect(m_SocketD,(sockaddr*)&m_sAddrD,sizeof(m_sAddrD));
					DWORD dwResult = GetLastError();
					if (dwResult == WSAEWOULDBLOCK)
					{
						SetDlgItemText(IDC_STATIC_COMD,"Connecting");
						KillTimer(RESTARTDTIMER);
						SetTimer(CONNECTDTIMER,CONNECTTIMERTIME,NULL);
					}
					else
					{
						char szError[256];
						sprintf(szError, "Connect error: %d", dwResult);
						SetDlgItemText(IDC_STATIC_COMD,szError);
					}
				}
			}
			else
			{
				char szError[256];
				DWORD dwResult = GetLastError();
				sprintf(szError, "Socket error: %d", dwResult);
				SetDlgItemText(IDC_STATIC_COMD,szError);
			}
		}
	}
/*
	else if (nIDEvent == 2020)
	{
		char szTemp[64];
		static int iG1 = 0;
		static int iG2 = 0;
		static int iG3 = 0;
		static int iG4 = 0;
		static int iN1 = 0;
		static int iN2 = 0;
		static int iN3 = 0;
		static int iN4 = 0;
		sprintf(szTemp,"NB,%05d,%05d,%05d,%05d\r\n",iN1,iN2,iN3,iN4);
		char* pszBuffer = (char*)malloc(strlen(szTemp));
		memcpy(pszBuffer,szTemp,strlen(szTemp));
		OnReceiveA(strlen(szTemp),(LPARAM)pszBuffer);
		sprintf(szTemp,"GB,%05d,%05d,%05d,%05d\r\n",iG1,iG2,iG3,iG4);
		pszBuffer = (char*)malloc(strlen(szTemp));
		memcpy(pszBuffer,szTemp,strlen(szTemp));
		OnReceiveA(strlen(szTemp),(LPARAM)pszBuffer);
		iN1++;iN2++;iN3++;iN4++;
		iG1++;iG2++;iG3++;iG4++;
	}
*/
	
	CDialog::OnTimer(nIDEvent);
}

void CWatchComDlg::OnStaticComa() 
{
	// TODO: Add your control notification handler code here
	OnButtonA();
}

void CWatchComDlg::OnStaticComc() 
{
	// TODO: Add your control notification handler code here
	OnButtonC();
}

void CWatchComDlg::OnStaticComb() 
{
	// TODO: Add your control notification handler code here
	OnButtonB();	
}

void CWatchComDlg::OnStaticComd() 
{
	// TODO: Add your control notification handler code here
	OnButtonD();
}

void CWatchComDlg::GraphIsRunning(int i)
{
	if (i == 1) 
	{
		m_bDataWatchDlg1Open = false;
	}
	else if (i == 2) 
	{
		m_bDataWatchDlg2Open = false;
	}
	else if (i == 3) 
	{
		m_bDataWatchDlg3Open = false;
	}
	else if (i == 4) 
	{
		m_bDataWatchDlg4Open = false;
	}
}

void CWatchComDlg::GraphIsDestroyed(int i)
{
	if (i == 1) 
	{
//		m_pDataWatchDlg1->EndDialog(0);
//		m_pDataWatchDlg1->DestroyWindow();
//		delete m_pDataWatchDlg1;
		m_pDataWatchDlg1 = NULL;
	}
	else if (i == 2) 
	{
//		m_pDataWatchDlg2->EndDialog(0);
//		delete m_pDataWatchDlg2;
		m_pDataWatchDlg2 = NULL;
	}
	else if (i == 3) 
	{
//		m_pDataWatchDlg3->EndDialog(0);
//		delete m_pDataWatchDlg3;
		m_pDataWatchDlg3 = NULL;
	}
	else if (i == 4) 
	{
//		m_pDataWatchDlg4->EndDialog(0);
//		delete m_pDataWatchDlg4;
		m_pDataWatchDlg4 = NULL;
	}
}

void CWatchComDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	if (m_bDataWatchDlg1Open)
		return;
	m_pDataWatchDlg1 = new CDataWatchDlg();
	m_pDataWatchDlg1->SetDlgTitle("TSA Data A", this, 1);
	m_pDataWatchDlg1->SetGraphTitles(
		"Neutron A", "Neutron B", "Neutron C", "Neutron D", 
		"Gamma 1", "Gamma 2", "Gamma 3","Gamma 4");
	m_pDataWatchDlg1->Create(IDD_DATAWATCH_DIALOG,NULL);
	m_pDataWatchDlg1->ShowWindow(SW_SHOW);
	m_bDataWatchDlg1Open = true;
}

void CWatchComDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	if (m_bDataWatchDlg2Open)
		return;
	m_pDataWatchDlg2 = new CDataWatchDlg();
	m_pDataWatchDlg2->SetDlgTitle("TSA Data B", this, 2);
	m_pDataWatchDlg2->SetGraphTitles(
		"Neutron A", "Neutron B", "Neutron C", "Neutron D", 
		"Gamma 1", "Gamma 2", "Gamma 3","Gamma 4");
	m_pDataWatchDlg2->Create(IDD_DATAWATCH_DIALOG,NULL);
	m_pDataWatchDlg2->ShowWindow(SW_SHOW);
	m_bDataWatchDlg2Open = true;
	
}

void CWatchComDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	if (m_bDataWatchDlg3Open)
		return;
	m_pDataWatchDlg3 = new CDataWatchDlg();
	m_pDataWatchDlg3->SetDlgTitle("TSA Data C", this, 3);
	m_pDataWatchDlg3->SetGraphTitles(
		"Neutron A", "Neutron B", "Neutron C", "Neutron D", 
		"Gamma 1", "Gamma 2", "Gamma 3","Gamma 4");
	m_pDataWatchDlg3->Create(IDD_DATAWATCH_DIALOG,NULL);
	m_pDataWatchDlg3->ShowWindow(SW_SHOW);
	m_bDataWatchDlg3Open = true;
	
}

void CWatchComDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	if (m_bDataWatchDlg4Open)
		return;
	m_pDataWatchDlg4 = new CDataWatchDlg();
	m_pDataWatchDlg4->SetDlgTitle("TSA Data D", this, 4);
	m_pDataWatchDlg4->SetGraphTitles(
		"Neutron A", "Neutron B", "Neutron C", "Neutron D", 
		"Gamma 1", "Gamma 2", "Gamma 3","Gamma 4");
	m_pDataWatchDlg4->Create(IDD_DATAWATCH_DIALOG,NULL);
	m_pDataWatchDlg4->ShowWindow(SW_SHOW);
	m_bDataWatchDlg4Open = true;

}

void CWatchComDlg::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	m_bPauseLog = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == 1;	
	if (!m_bPauseLog)
	{
		m_iSeq = GetDlgItemInt(IDC_EDIT1) + 1;
		SetDlgItemInt(IDC_EDIT1,m_iSeq);
	}
}
