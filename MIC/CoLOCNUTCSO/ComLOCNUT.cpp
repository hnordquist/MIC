//$History: ComLOCNUT.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Pelowitz     Date: 4/29/08    Time: 2:14p
 * Updated in $/MIC 2004/CoLOCNUTCSO
 * final check in prior to retirement
 * 
 * *****************  Version 1  *****************
 * User: Pelowitz     Date: 3/12/08    Time: 2:52p
 * Created in $/MIC 2004/CoLOCNUTCSO
 * At or near retirement (EOSS still being tested)
// 
// *****************  Version 2  *****************
// User: 149047       Date: 10/23/06   Time: 1:29p
// Updated in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 9/13/06    Time: 12:39p
// Created in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 3  *****************
// User: Pelowitz     Date: 3/02/06    Time: 2:10p
// Updated in $/CoLOCNUTCSO
// Cleaned up problem with socket blocking on call to rec and added code
// to issue TIMESET messages to node 0 watcher (if one exists) and to
// watcher window (if one exists).
// 
// *****************  Version 2  *****************
// User: Pelowitz     Date: 2/27/06    Time: 1:55p
// Updated in $/CoLOCNUTCSO
// Fix minor problem when echoing binary data to watch0 window.
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 1/11/06    Time: 7:47a
// Created in $/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Moore        Date: 8/09/05    Time: 4:12p
// Created in $/MIC 2.000/CoLOCNUTCSO
// 
// ComLOCNUT.cpp : implementation file

#include "stdafx.h"
#include "CSO_definitions.h"
#include "INI_definitions.h"
#include "ComLOCNUT.h"
#include "Resource.h"
#include "TimedMessageDialog.h"
#include "NewLOCNUTComm.h"
#include <math.h>
#include "md5.h"
	
#define CLOSEPORT	"CLOSEPORT"
#define OPENPORT	"OPENPORT"
#define TIMESYNCPC	"TIMESYNCPC"
#define BASEIP		"BaseIP"
#define INI_PORT	"Port"
#define PASSPHRASE	"system passphrase"

//dgp -- these need to be done peggy's way
#define UDPWSARECV		(WM_USER+6000)
#define UDPWSARECVTS	(WM_USER+6001)
#define INITSOCKET		(WM_USER+6002)
#define SENDSOCKET		(WM_USER+6003)

//windows registered message values
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CComLOCNUT

IMPLEMENT_DYNCREATE(CComLOCNUT, CWinThread)

CComLOCNUT::CComLOCNUT()
{
	m_pReceiver = NULL;		//object to send incoming text to
	m_pWatcher = NULL;		//secondary object to send text to
	m_bAutoDelete = TRUE;	//set the CWinThread to automatically delete
	memset(NodeInfo,NULL,sizeof(NodeInfo));//zero out the node info structure
	m_bTimeSyncPC = true;
	AfxSocketInit();
	m_cLastTimeSync = COleDateTime(1990,1,1,0,0,0);
	m_iTimeSyncCount = 0;
	m_ReceivingSocket = INVALID_SOCKET;

	m_bLastDUMPBBMBinary = false;
	m_bStopped = false;

	strncpy(m_pType, LOCNUT_DISCOVERY_TYPE, sizeof(m_pType));
}

void CComLOCNUT::InitSocket(WPARAM,LPARAM)
{

   #ifndef _AFXDLL
   #define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE
   #define _afxSockThreadState AfxGetModuleThreadState()

      _AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
      if (pState->m_pmapSocketHandle == NULL)
         pState->m_pmapSocketHandle = new CMapPtrToPtr;
      if (pState->m_pmapDeadSockets == NULL)
         pState->m_pmapDeadSockets = new CMapPtrToPtr;
      if (pState->m_plistSocketNotifications == NULL)
         pState->m_plistSocketNotifications = new CPtrList;

   #endif
	bool bResult = true;
	//get port info from ini file
//	m_iPortNumber = GetPrivateProfileInt(m_pName,INI_PORT,6000,m_szIniFile);
//	GetPrivateProfileString(m_pName,BASEIP,"192.168.1",m_szBaseIP,sizeof(m_szBaseIP),m_szIniFile);
//	sscanf(m_szBaseIP,"%d.%d.%d",&m_iBaseIP[0],&m_iBaseIP[1],&m_iBaseIP[2]);
//	char szTemp[16];
//	GetPrivateProfileString(m_pName,TIMESYNCPC,"Yes",szTemp,sizeof(szTemp),m_szIniFile);
//	m_bTimeSyncPC = (toupper(szTemp[0]) == 'Y');
	
//	GetPrivateProfileString(m_pName,PASSPHRASE,"system passphrase",m_szPassphrase,sizeof(m_szPassphrase),m_szIniFile);
	if ((m_ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		bResult = false;
	else 
	{
		sockaddr_in ReceiverAddr;
		ReceiverAddr.sin_family = AF_INET;
		ReceiverAddr.sin_port = htons((unsigned short)m_iPortNumber);
		ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(m_ReceivingSocket, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR)
		{
			bResult = false;
			closesocket(m_ReceivingSocket);
		}
	}

	if (m_bTimeSyncPC && bResult)
	{
		if ((m_TimeSyncSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
			bResult = false;
		else 
		{
			sockaddr_in ReceiverAddr;
			ReceiverAddr.sin_family = AF_INET;
			ReceiverAddr.sin_port = htons((unsigned short)(m_iPortNumber+1));
			ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(m_TimeSyncSocket, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR)
			{
				bResult = false;
				closesocket(m_TimeSyncSocket);
			}
		}
	}

	//these change both sockets to be non-blocking
//	unsigned long argp = 1;
//	int iResult = ioctlsocket(m_ReceivingSocket,FIONBIO,&argp);
//	argp = 1;
//	iResult = ioctlsocket(m_TimeSyncSocket,FIONBIO,&argp);

	fd_set fd_set1;

	timeval timeval1;

	int iReturn; 
		
//	char buffer[1024];

	m_bWorking = true;
		
	while (m_bWorking)
	{
		if (m_bTimeSyncPC)
		{
			fd_set1.fd_count    = 2;
			fd_set1.fd_array[0] = m_ReceivingSocket;
			fd_set1.fd_array[1] = m_TimeSyncSocket;
		}
		else
		{
			fd_set1.fd_count    = 1;
			fd_set1.fd_array[0] = m_ReceivingSocket;
			fd_set1.fd_array[1] = NULL;
		}


		timeval1.tv_sec = 1;
		timeval1.tv_usec = 0;

		iReturn = select(0,&fd_set1,NULL,NULL,&timeval1);//timeout
		if (iReturn == SOCKET_ERROR)
		{

			switch (WSAGetLastError()) {
			case WSANOTINITIALISED:
				break;
			case WSAEFAULT:
				break;
			case WSAENETDOWN:
				break;
			case WSAEINVAL:
				break;
			case WSAEINTR:
				break;
			case WSAEINPROGRESS:
				break;
			case WSAENOTSOCK:
				break;
			default:;
			}
		}
		else if (iReturn > 0) 
		{
			unsigned long argp = 0;
			ioctlsocket(m_ReceivingSocket,FIONREAD,&argp);
			if (argp)
				OnReceive((WPARAM)m_ReceivingSocket,(LPARAM)FD_READ);
			argp = 0;
			ioctlsocket(m_TimeSyncSocket,FIONREAD,&argp);
			if (argp)
				OnReceiveTS((WPARAM)m_TimeSyncSocket,(LPARAM)FD_READ);
		}
	}
	closesocket(m_ReceivingSocket);
	if (m_TimeSyncSocket)
		closesocket(m_TimeSyncSocket);

}

//return the name of this com object
char * CComLOCNUT::GetName()
{
	return &m_pName[0];
}

//register a node watcher for a specific node
bool CComLOCNUT::RegisterWatch(HWND watcher_handle, int inode)
{
	//is this one empty
	if (NodeInfo[inode].hWatcher == NULL)
	{
		//yes, so use it
		NodeInfo[inode].hWatcher = watcher_handle;
		return true;
	}
	m_csLastError = CString("RegisterWatch::Watcher already using this port");
	return false;
}

//unregister a node watcher
bool CComLOCNUT::UnRegisterWatch(int inode)
{
	if (NodeInfo[inode].hWatcher != NULL)
	{
		//remove the watcher
		NodeInfo[inode].hWatcher = NULL;
		//success
		return true;
	}
	//not found return failure
	m_csLastError = CString("UnRegisterWatch::Node not found");
	return false;
}

//register the node receiver
bool CComLOCNUT::Register(
	HWND receiver_handle, 
	bool out_of_band,
	int inode)
{
	if (!out_of_band && (inode < MAX_UDPNODE_ARRAY))
	{
		if (NodeInfo[inode].hReceiver == NULL)
		{
			NodeInfo[inode].hReceiver = receiver_handle;
			return true;
		}
	}
	else  //out of band
	{
		inode &= 0x0000ffff;

		if ((inode < MAX_UDPNODE_ARRAY) && (NodeInfo[inode].hOOB == NULL))
		{
			NodeInfo[inode].hOOB = receiver_handle;
			return true;
		}
	}
	m_csLastError = CString("Register::Node already registered");
	return false;
}

//unregister the receiver
bool CComLOCNUT::UnRegister(int inode)
{
	if (!(inode & 0xffff0000))
	{
		if (NodeInfo[inode].hReceiver != NULL)
		{
			NodeInfo[inode].hReceiver = NULL;
			return true;
		}
	}
	else
	{
		inode &= 0x0000ffff;
		if (NodeInfo[inode].hOOB != NULL)
		{
			NodeInfo[inode].hOOB = NULL;
			return true;
		}
	}

	//not found return failure
	m_csLastError = CString("UnRegister::Node not registered");

	return false;
}

bool CComLOCNUT::SetUpPort(char* name)
{
	strcpy(m_pName,name);
//   #ifndef _AFXDLL
//   #define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE
//   #define _afxSockThreadState AfxGetModuleThreadState()

//      _AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
//      if (pState->m_pmapSocketHandle == NULL)
//         pState->m_pmapSocketHandle = new CMapPtrToPtr;
//      if (pState->m_pmapDeadSockets == NULL)
//         pState->m_pmapDeadSockets = new CMapPtrToPtr;
//      if (pState->m_plistSocketNotifications == NULL)
//         pState->m_plistSocketNotifications = new CPtrList;
//
   //#endif

	bool bResult = true;
	//get port info from ini file
	m_iPortNumber = GetPrivateProfileInt(m_pName,INI_PORT,6000,m_szIniFile);
	GetPrivateProfileString(m_pName,BASEIP,"192.168.1",m_szBaseIP,sizeof(m_szBaseIP),m_szIniFile);
	sscanf(m_szBaseIP,"%d.%d.%d",&m_iBaseIP[0],&m_iBaseIP[1],&m_iBaseIP[2]);
	char szTemp[16];
	GetPrivateProfileString(m_pName,TIMESYNCPC,"Yes",szTemp,sizeof(szTemp),m_szIniFile);
	m_bTimeSyncPC = (toupper(szTemp[0]) == 'Y');
	
	GetPrivateProfileString(m_pName,PASSPHRASE,"system passphrase",m_szPassphrase,sizeof(m_szPassphrase),m_szIniFile);

	//post message to set up the socket
	if (::PostThreadMessage(m_nThreadID,INITSOCKET,NULL,NULL)==0)
		::PostThreadMessage(m_nThreadID,INITSOCKET,NULL,NULL);
	return true;
}

//<<QA>> are both of these "SetUpPort" functions needed???
//initialize the serial port from parameter information
bool CComLOCNUT::SetUpPort(
	char* /*szBaseIP*//*szPort*/, 
	unsigned uPort/*BaudRate*/, 
	unsigned /*DataBits*/, 
	unsigned /*StopBits*/, 
	char* /*szParity*/, 
	bool /*bLongBreak*/)
{
/*
	bool bResult = true;

	//get port info from ini file
	m_iPortNumber = uPort;
	strncpy(m_szBaseIP,szBaseIP,sizeof(m_szBaseIP));

	if ((m_ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		bResult = false;
	else 
	{
		sockaddr_in ReceiverAddr;
		ReceiverAddr.sin_family = AF_INET;
		ReceiverAddr.sin_port = htons((unsigned short)m_iPortNumber);
		ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(m_ReceivingSocket, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR)
		{
			bResult = false;
			closesocket(m_ReceivingSocket);
		}
	}
	if (bResult)
	{
//		WSAAsyncSelect(m_ReceivingSocket, m_pMainWnd->m_hWnd, UDPWSARECV, FD_READ);
//		WSAAsyncSelect(m_ReceivingSocket, (struct HWND__ *)m_nThreadID, UDPWSARECV, FD_READ);
	}

	return bResult;
*/
	return false;
}

void CComLOCNUT::DoShutdown(WPARAM, LPARAM)
{
//	TRACE("CComLOCNUT::DoShutdown()\n");
	m_bWorking = false;
	closesocket(m_ReceivingSocket);
	if (m_TimeSyncSocket)
		closesocket(m_TimeSyncSocket);

	AfxEndThread(0);  //This calls the DTOR
}

void CComLOCNUT::DoStart(WPARAM, LPARAM)
{
	m_bStopped = false;
}

BOOL  CComLOCNUT::InitInstance()
{
	//TRACE("CComLOCNUT::InitInstance() - NOOP\n");
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CComLOCNUT::ExitInstance()
{
	//TRACE("CComLOCNUT::ExitInstance\n");
	closesocket(m_ReceivingSocket);
	if (m_TimeSyncSocket)
		closesocket(m_TimeSyncSocket);
	return CCSO::ExitInstance();
}

//on receive time sync message
void CComLOCNUT::OnReceiveTS(WPARAM wParam,LPARAM lParam)
{
	if (wParam != m_TimeSyncSocket) return;
	if (WSAGETSELECTEVENT(lParam) == FD_READ)
	{
		int iRc;
		char sBuffer[4096];
		if ((iRc = WSAGETSELECTERROR(lParam))!= 0)
			return;
		int iCount;
		sockaddr_in sReceiverAddr;
		int size = sizeof(sReceiverAddr);
		if ((iCount = recvfrom(m_TimeSyncSocket,sBuffer, sizeof(sBuffer), 0,
			(SOCKADDR *)&sReceiverAddr,&size)) != SOCKET_ERROR)
		{
			if (iCount != 0)
			{
				if	(
					(sReceiverAddr.sin_addr.S_un.S_un_b.s_b1 == m_iBaseIP[0]) &&
					(sReceiverAddr.sin_addr.S_un.S_un_b.s_b2 == m_iBaseIP[1]) &&
					(sReceiverAddr.sin_addr.S_un.S_un_b.s_b3 == m_iBaseIP[2])
					)
				{

					//authenticate
					bool bAuthenticated;
					bAuthenticated = true;
					if (iCount > 16)
					{
						//char szPassPhrase[32];
						//strcpy(szPassPhrase,"system passphrase");
						md5 md;	
						md.Init();
						md.Update((unsigned char*)m_szPassphrase,strlen(m_szPassphrase));
						md.Update((unsigned char*)sBuffer,iCount - 16);
						md.Finalize();
						if (memcmp((const char *)md.Digest(),&(sBuffer[iCount-16]),16) != 0)
							bAuthenticated = false;
					}
					if (bAuthenticated)
					{
						if (strstr(sBuffer,"TIMESET")==sBuffer)
						{
							unsigned long ulTimeRem;
      						memcpy(&ulTimeRem,&sBuffer[11],4);
							CheckAndSetTime(ulTimeRem,sReceiverAddr.sin_addr.S_un.S_un_b.s_b4);
						}
					}
				}
			}
		}

		//is there a node 0 watcher registered
		if (NodeInfo[0].hWatcher)
		{
			//malloc a copy
			char *Data = (char*)CoTaskMemAlloc(28+iCount+3);
			//add some info so the user knows where is from
			sprintf(Data,"Rec'd from %03d.%03d.%03d.%03d: ",sReceiverAddr.sin_addr.S_un.S_un_b.s_b1,
				sReceiverAddr.sin_addr.S_un.S_un_b.s_b2,sReceiverAddr.sin_addr.S_un.S_un_b.s_b3,sReceiverAddr.sin_addr.S_un.S_un_b.s_b4);
			if (iCount >= 20)
				memcpy(&Data[28],sBuffer,iCount-20);
			//be polite to receiver by adding cr and lf
			//strcat(Data,"\r\n");
			memcpy(&Data[28] + iCount - 20,"\r\n",2);
			//send it
			::PostMessage(NodeInfo[0].hWatcher,m_uIDC_RECEIVE,(WPARAM)(28+iCount+2-20),(LPARAM)Data);
		}
	}
}

int CComLOCNUT::CheckAndSetTime(unsigned int ulTimeRem, BYTE bFrom)
{
	int iReturn;
	COleDateTime cNow = COleDateTime::GetCurrentTime();

	COleDateTime cNew= COleDateTime(1980,1,1,0,0,0);
	double dTemp= (double)ulTimeRem/(60.0*60.0*24.0);
	
	cNew.m_dt = cNew.m_dt + dTemp;

	CString tmpBuff;
	//don't change this -- event is looking for this string to
	//differentiate between data and true OOB information.
	tmpBuff.Format("TIME SET - COLLECTs TIME:%s MESSAGEs TIME: %s ",cNow.Format("%Y,%m,%d %H:%M:%S"),cNew.Format("%Y,%m,%d %H:%M:%S"));
	tmpBuff.TrimRight();
	WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME_COMMAND",tmpBuff,m_szIniFile);

	bool bSendToInst = true;

	COleDateTimeSpan cSpan;
	cSpan = cNew - cNow;
	double Seconds;
	Seconds = fabs(cSpan.GetTotalSeconds());

	iReturn = 0;

	//if difference is > 2 seconds and < 5 minutes
	double dMin = (double)GetPrivateProfileInt(m_pName,"MIN_TIME_SET",2,m_szIniFile);
	double dMax = (double)GetPrivateProfileInt(m_pName,"MAX_TIME_SET",300,m_szIniFile);
	if (dMin < 1) dMin = 1;
	if (dMax < 0) dMax = 0;

	if ((Seconds > dMin && Seconds < dMax) || (Seconds > dMin && dMax == 0))
	{
		//then reset the system clock
		SYSTEMTIME SysTime;
		GetSystemTime(&SysTime);
		cNow.SetDateTime(
			SysTime.wYear,
			SysTime.wMonth,
			SysTime.wDay,
			SysTime.wHour,
			SysTime.wMinute,
			SysTime.wSecond);
		cNow = cNow + cSpan;
		SysTime.wYear	= (unsigned short)(cNow.GetYear());		
		SysTime.wMonth	= (unsigned short)(cNow.GetMonth());
		SysTime.wDay	= (unsigned short)(cNow.GetDay());
		SysTime.wHour	= (unsigned short)(cNow.GetHour());
		SysTime.wMinute = (unsigned short)(cNow.GetMinute());
		SysTime.wSecond = (unsigned short)(cNow.GetSecond());
									
		if (SetSystemTime(&SysTime))
		{
			iReturn = 1;
			WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME",cNow.Format("%Y,%m,%d %H:%M:%S Done"),m_szIniFile);
			tmpBuff += " Done";
			m_iTimeSyncCount = 0;
//dgp - this message needs to be done correctly!!!!
//micmgr then can send to all instruments that can receive oob 
//when we do this we can also remove the section below that sends
//to all LOCNUTS known instruments (with oob registration)
//			m_pMainWnd->PostMessage(WM_TIMECHANGE,LOCNUT,0);
		}
		else
		{
			WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME",cNow.Format("%Y,%m,%d %H:%M:%S Failed"),m_szIniFile);
			tmpBuff += " FAILED";
			m_iTimeSyncCount++;
		}
	}
	else
	{
		m_iTimeSyncCount++;
		if (Seconds >= dMax) 
		{
			WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME",cNow.Format("%Y,%m,%d %H:%M:%S Time Diff. Too Large"),m_szIniFile);
			tmpBuff += " Not Done (time diff. too large)";
		}
		else
		{
			WritePrivateProfileString(m_pName,"LAST_SET_DATE_TIME",cNow.Format("%Y,%m,%d %H:%M:%S Not Required"),m_szIniFile);
			tmpBuff += " Not Required";
			bSendToInst = false;
		}
	}

	//send tmpBuff to all instruments 
	//we are changing the systems time so
	//all instruments that maintain a clock may need to know

	if (cNow.GetDay() != m_cLastTimeSync.GetDay() && cNow.GetYear() != m_cLastTimeSync.GetYear())
		m_iTimeSyncCount = 0;
	m_cLastTimeSync = cNow;

//dgp -- this needs to be moved up to the micmgr level and let it do its thing to all instruments
	//this sends to this com objects registered instruments with oob only
	//int iCount = 0;
	if (m_iTimeSyncCount <= 5)
	{
		for (int iInstrument = 0, iCount=0 ; iInstrument < MAX_UDPNODE_ARRAY; iInstrument++)
		{
			if (NodeInfo[iInstrument].hOOB != NULL)
			{
				//count the instruments
				iCount++;
				//get memory
				char* szData = (char*)CoTaskMemAlloc((ULONG)(tmpBuff.GetLength()+1));
				//fill memory
				strcpy(szData,LPCSTR(tmpBuff));
				szData[tmpBuff.GetLength()] = NULL;
				//post message
				::PostMessage(NodeInfo[iInstrument].hOOB,m_uIDC_RECEIVE_OOB,
					(WPARAM)strlen(szData)+1,(LPARAM)szData);
			}
//			if (iCount >= g_dCount)
//				iInstrument = MAX_UDPNODE_ARRAY;
		}
	}
/*
	//this sends to all instruments and won't work here any longer 'cause it doesn't know 
	//about the g_pInstruments
	if (bSendToInst)
	{
		for (int iInstrument = 0; iInstrument < g_dCount; iInstrument++)
		{
			if ((g_pInstruments[iInstrument] != NULL))
			{
				//get memory
				char* szData = (char*)CoTaskMemAlloc((ULONG)(tmpBuff.GetLength()+1));
				//fill memory
				strcpy(szData,LPCSTR(tmpBuff));
				szData[tmpBuff.GetLength()] = NULL;
				//post message
				g_pInstruments[iInstrument]::PostMessage(IDC_RECEIVE_OOB,
					(WPARAM)strlen(szData)+1,(LPARAM)szData);
			}
		}
	}
*/	
	//if watcher registered
	if (NodeInfo[bFrom].hWatcher)
	{
		//malloc a copy
		tmpBuff += "\r\n";
		char *Data = (char*)CoTaskMemAlloc(tmpBuff.GetLength());
		memcpy(Data,tmpBuff,tmpBuff.GetLength());
		//send copy to watcher
		::PostMessage(NodeInfo[bFrom].hWatcher,m_uIDC_RECEIVE,(WPARAM)tmpBuff.GetLength(),(LPARAM)Data);
	}


	return iReturn;
}

//Receive loop
//run until done 
void CComLOCNUT::OnReceive(WPARAM wParam, LPARAM lParam)
{
	if (wParam != m_ReceivingSocket || m_bStopped) return;
//Beep(2000,2000);
	if (WSAGETSELECTEVENT(lParam) == FD_READ)
	{
		int iRc;
		char sBuffer[4096];
		if ((iRc = WSAGETSELECTERROR(lParam))!= 0)
			return;
		int iCount;
		sockaddr_in sReceiverAddr;
		int size = sizeof(sReceiverAddr);
		if ((iCount = recvfrom(m_ReceivingSocket,sBuffer, sizeof(sBuffer), 0,
			(SOCKADDR *)&sReceiverAddr,&size)) != SOCKET_ERROR)
		{
			if (iCount != 0)
			{
				//else ignore

				//if high three bytes of sender address match and 
				if	(
					(sReceiverAddr.sin_addr.S_un.S_un_b.s_b1 == m_iBaseIP[0]) &&
					(sReceiverAddr.sin_addr.S_un.S_un_b.s_b2 == m_iBaseIP[1]) &&
					(sReceiverAddr.sin_addr.S_un.S_un_b.s_b3 == m_iBaseIP[2])
					)
				{

					//authenticate
					bool bAuthenticated;
					bAuthenticated = false;
					if (iCount > 16)
					{
						//char szPassPhrase[32];
						//strcpy(szPassPhrase,"system passphrase");
						char szTemp[16];
						md5 md;	
						md.Init();
						md.Update((unsigned char*)m_szPassphrase,strlen(m_szPassphrase));
						md.Update((unsigned char*)sBuffer,iCount - 16);
						md.Finalize();
						memcpy(szTemp,md.Digest(),16);
						//if (memcmp((const char *)md.Digest(),&(sBuffer[iCount-16]),16) == 0)
						if (memcmp(szTemp,&(sBuffer[iCount-16]),16) == 0)
							bAuthenticated = true;
/*
else
{
   TRACE("post %s\n",bAuthenticated?"AUTH":"FAIL");
   for (int k = 0; k < iCount; k++)
   {
      if (k % 16 == 0) TRACE("\n");
      TRACE("%02x ",(unsigned)sBuffer[k]);
   }
   TRACE("\n");
}						
*/

					}
					if (bAuthenticated)
					{
						//someone is registered with the low byte
						if (NodeInfo[sReceiverAddr.sin_addr.S_un.S_un_b.s_b4].hReceiver)
						{
							//malloc a copy
							char *Data = (char*)CoTaskMemAlloc((ULONG)(iCount-20));
							memcpy(Data,sBuffer,iCount-20);
							//send copy to receiver
							::PostMessage(NodeInfo[sReceiverAddr.sin_addr.S_un.S_un_b.s_b4].hReceiver,m_uIDC_RECEIVE,(WPARAM)(iCount-20),(LPARAM)Data);
						}
						else if (NodeInfo[sReceiverAddr.sin_addr.S_un.S_un_b.s_b4].hOOB)
						{
							//malloc a copy
							char *Data = (char*)CoTaskMemAlloc((ULONG)(iCount-20));
							memcpy(Data,sBuffer,iCount-20);
							//send copy to receiver
							::PostMessage(NodeInfo[sReceiverAddr.sin_addr.S_un.S_un_b.s_b4].hOOB,m_uIDC_RECEIVE,(WPARAM)(iCount-20),(LPARAM)Data);
						}

						//if watcher registered
						if (NodeInfo[sReceiverAddr.sin_addr.S_un.S_un_b.s_b4].hWatcher)
						{
							//malloc a copy
							char *Data = (char*)CoTaskMemAlloc((ULONG)(iCount-20));
							memcpy(Data,sBuffer,iCount-20);
							//send copy to watcher
							::PostMessage(NodeInfo[sReceiverAddr.sin_addr.S_un.S_un_b.s_b4].hWatcher,m_uIDC_RECEIVE,(WPARAM)iCount-20,(LPARAM)Data);
						}
					}

				}

				//is there a node 0 watcher registered
				if (NodeInfo[0].hWatcher)
				{
					//malloc a copy
					char *Data = (char*)CoTaskMemAlloc((ULONG)(28+iCount+3));
					//add some info so the user knows where is from
					sprintf(Data,"Rec'd from %03d.%03d.%03d.%03d: ",sReceiverAddr.sin_addr.S_un.S_un_b.s_b1,
						sReceiverAddr.sin_addr.S_un.S_un_b.s_b2,sReceiverAddr.sin_addr.S_un.S_un_b.s_b3,sReceiverAddr.sin_addr.S_un.S_un_b.s_b4);
					if (iCount >= 20)
//						strncat(Data,sBuffer,iCount-20);
						memcpy(&Data[28],sBuffer,iCount-20);
					//be polite to receiver by adding cr and lf
//					strcat(Data,"\r\n");
					memcpy(&Data[28]+iCount-20,"\r\n",2);
					//send it
					::PostMessage(NodeInfo[0].hWatcher,m_uIDC_RECEIVE,(WPARAM)(28+iCount+2-20),(LPARAM)Data);
				}
			}
		}
	}
}

BOOL CComLOCNUT::EscapeCommFunction(DWORD dwFunction)
{
	return 0;
}

void CComLOCNUT::DoStop()
{
	//flag dostart to exit now
	m_bStopped = true;
	m_bWorking = false;
	closesocket(m_ReceivingSocket);
	if (m_TimeSyncSocket)
		closesocket(m_TimeSyncSocket);
}

void CComLOCNUT::WatchDogOk(bool /*set*/)
{
}

void CComLOCNUT::WatchDogSend()
{
}

void CComLOCNUT::SendToWatch(int Length, char* Output, int Node)
{
	Node &= 0x0000ffff;
	if (NodeInfo[Node].hWatcher != NULL)
	{
		int dlength = (Length==ISZSTRING)?strlen(Output):Length;
		//char*Data = (char*)malloc(dlength+2);
		char*Data = (char*)CoTaskMemAlloc(dlength+2);
		if (Data != NULL)
		{
			memmove(Data,Output,dlength);
			if (Data[0]==0x0d) Data[0]= NULL;
			Data[dlength] = 0x0a;
			Data[dlength+1]=NULL;
			//NodeInfo[Node].Watcher->PostMessage(IDC_RECEIVE,dlength+1,(LPARAM)Data);
			::PostMessage(NodeInfo[Node].hWatcher, m_uIDC_RECEIVE,dlength+1,(LPARAM)Data);
		}
	}
}

void CComLOCNUT::SendSocket(WPARAM wParam, LPARAM lParam)
{
	if (m_ReceivingSocket == INVALID_SOCKET)
	{
		::PostThreadMessage(m_nThreadID,INITSOCKET,NULL,NULL);
		return;
	}


	int iPortNumber = (wParam >> 16);
	int iLength = (wParam >> 8) & 0x000000ff;
	int iNode = wParam & 0x000000ff;
	char szIP[16];
	char szTemp[33];
	strcpy(szIP,m_szBaseIP);
	strcat(szIP,".");
	strcat(szIP,itoa(iNode,szTemp,10));
	struct sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = inet_addr(szIP);
	to.sin_port = htons((unsigned short)iPortNumber);


//	char szPassPhrase[32];
	char szBuff[1024];
//	strcpy(szPassPhrase,"system passphrase");

	md5 md;	
	md.Init();
	md.Update((unsigned char*)m_szPassphrase,strlen(m_szPassphrase));
	memcpy(szBuff,(char*)lParam,iLength);

	COleDateTime cNow = COleDateTime::GetCurrentTime();
	COleDateTime cStart = COleDateTime(1980,1,1,0,0,0);
	COleDateTimeSpan cSpan;
	cSpan = cNow-cStart;
	unsigned int uiSpan = (unsigned int)cSpan.GetTotalSeconds();
	memcpy(&szBuff[iLength],&uiSpan,4);
	md.Update((unsigned char*)szBuff,iLength+4);
	md.Finalize();
	memcpy(&szBuff[iLength+4],md.Digest(),16);
	iLength += 20;
	int ret = sendto(m_ReceivingSocket,szBuff,iLength,0,(sockaddr*)&to,sizeof(struct sockaddr_in));
//	delete [] ((char*)lParam);
	CoTaskMemFree((void*)lParam);
}

//called to send a buffer of data to the port
int CComLOCNUT::Transmit(int iLength, char* sOutput, int iNode, bool bQuiet)
{
	//find length if not given
	if (iLength == -1)
		iLength = strlen(sOutput);

	//send a long break as a control message on port+1
	if ((strcmp(sOutput,"LONGBREAKSET\r")==0) ||
		(strcmp(sOutput,"LOCALLONGBREAK\r")==0))
	{
		WPARAM wParam = m_iPortNumber+1;
		wParam = (wParam<<8) | (iLength & 0x000000ff);
		wParam = (wParam<<8) | (iNode   & 0x000000ff);
		char* lParam = (char*)CoTaskMemAlloc((ULONG)iLength);
		memcpy(lParam,sOutput,iLength);
		::PostThreadMessage(m_nThreadID,SENDSOCKET,wParam,(LPARAM)lParam);
	}
	//ignore lb clear
	else if (strcmp(sOutput,"LONGBREAKCLR\r")==0)
	{
//		return m_iRegisterCount;
	}
	//send the actual message
	else
	{
		WPARAM wParam = m_iPortNumber;
		wParam = (wParam<<8) | (iLength & 0x000000ff);
		wParam = (wParam<<8) | (iNode   & 0x000000ff);
		char* lParam = (char*)CoTaskMemAlloc((ULONG)iLength);
		memcpy(lParam,sOutput,iLength);
//		::PostThreadMessage(m_nThreadID,SENDSOCKET,wParam,(LPARAM)lParam);
//		PostMessage(NULL,SENDSOCKET,wParam,(LPARAM)lParam);
		SendSocket((WPARAM)wParam,(LPARAM)lParam);
	}

	//send to node watcher
	if (!bQuiet)
	{
		//if we have a registered watcher then send to it
		if ((NodeInfo[iNode & 0x000000ff].hWatcher != NULL) && (iNode != 0))
		{
			//if the message is dumpbbm then turn off binary
			if (strcmp(sOutput,"DUMPBBM")==0)
				m_bLastDUMPBBMBinary = false;
			//if the message is dumpbbbm then turn on binary
			else if (strcmp(sOutput,"DUMPBBBM")==0)
				m_bLastDUMPBBMBinary = true;
			//insert a nl if we were doing binary and this is a dumpok or dump
			if ((strcmp(sOutput,"DUMPOK")==0) && m_bLastDUMPBBMBinary)
			{
				char* Data = (char*)CoTaskMemAlloc(2);
				Data[0] = '\n';
				Data[1] = NULL;
				::PostMessage(NodeInfo[iNode & 0x000000ff].hWatcher,m_uIDC_RECEIVE,2,(LPARAM)Data);
			}
			//send the actual data to the watcher
			char* Data = (char*)CoTaskMemAlloc(ULONG(iLength+2));
			memcpy(Data,sOutput,iLength);
			Data[iLength] = '\n';
			Data[iLength+1] = NULL;											
			::PostMessage(NodeInfo[iNode & 0x000000ff].hWatcher,m_uIDC_RECEIVE,iLength+1,(LPARAM)Data);
		}
	}

	//send to node 0 watcher
	if (NodeInfo[0].hWatcher != NULL)
	{
		//prepend with who we are sending to
		char *Data = (char*)CoTaskMemAlloc((ULONG)(13+iLength+1));
		sprintf(Data,"Sent to %03d: ",iNode);
		strncat(Data,sOutput,iLength);
		//send the actual message to the node 0 watcher
		::PostMessage(NodeInfo[0].hWatcher,m_uIDC_RECEIVE,(WPARAM)strlen(Data),(LPARAM)Data);
	}

	return 1;//m_iRegisterCount;//(int)iLength;
}

void CComLOCNUT::DoReset(WPARAM /*WWord*/, LPARAM LWord)
{
	m_bStopped = true;
	m_bWorking = false;
	closesocket(m_ReceivingSocket);
	if (m_TimeSyncSocket)
		closesocket(m_TimeSyncSocket);

	if (SetUpPort(m_pName))
	{
		::PostThreadMessage(m_nThreadID,m_uIDC_COM_START,0,0);
		SendToWatch(14,"Socket RESTARTED",LWord);
		m_bStopped = false;
	}
	else
	{
		SendToWatch(19,"Socket RESTART FAILED",LWord);
	}
}

CComLOCNUT::~CComLOCNUT()
{
}

BEGIN_MESSAGE_MAP(CComLOCNUT, CWinThread)
	//{{AFX_MSG_MAP(CComLOCNUT)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_START,DoStart)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_STOP,DoShutdown)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_RESET,DoReset)
	ON_MESSAGE(UDPWSARECV,OnReceive)
	ON_MESSAGE(UDPWSARECVTS,OnReceiveTS)
	ON_THREAD_MESSAGE(INITSOCKET,InitSocket)
	ON_MESSAGE(SENDSOCKET,SendSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CComLOCNUT::SetLocalRegisteredMessageValues()
{
//	TRACE("CComLOCNUT::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}



//////////////////////////////////////////////////////////////////////////////
//
//                      CollectParametersForNewCSO
//
//////////////////////////////////////////////////////////////////////////////
bool CComLOCNUT::CollectParametersForNewCSO(CString& csNameOut)
{
//	TRACE("CComLOCNUT::CollectParametersForNewCSO()\n");

	//This CSO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new CSO from that data.

	//Collect the data for the new communications object.
	CNewLOCNUTComm dlg(m_szIniFile,m_iDlgClose,m_pName);
	int result = dlg.DoModal();
	if (result == IDOK)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(COMMUNICATIONS,m_pName,LOCNUT_DISCOVERY_TYPE,m_szIniFile);
		return true;
	}
	else
	{
		csNameOut.Empty();
		return false;
	}
}


