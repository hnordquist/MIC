// EventInstrument.cpp

#include "stdafx.h"
#include "resource.h"
#include "ISO_definitions.h"
#include "CSO_Definitions.h"
#include "ccbutton.h"
#include "EventInstrument.h"
#include "EventSafePS.h"
#include "EventDataStat.h"
#include "BinaryDataStat.h"
#include "GPSDataStat.h"
#include "VACOSSDataStat.h"
#include "EventModParam.h"
#include "ISO_definitions.h"
#include "INI_definitions.h"
#include "colors.h"
#include "TPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//windows registered message values
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;






/*
#include <process.h>
#define TIMER_TEST 5849
*/







//Update Params gets called if initializing an instrument and after modifying it
bool CEventInstrument::UpdateParams(bool reseting)
{
	//get the file id to be used
	GetPrivateProfileString(m_pName,FILE_ID,"",m_pID,sizeof(m_pID),m_szIniFile);

	//get the file location to save data at
	GetPrivateProfileString(m_pName,SAVE_LOC,"C:\\DATA",m_szSaveLoc,sizeof(m_szSaveLoc),m_szIniFile);
	
	//get the DO_DUMP from the ini file
	m_bDoDump = GetPrivateProfileInt(m_pName,"DO_DUMP",0,m_szIniFile)==1;

	//get the LOG_MII from the ini file
	m_bLogMII = GetPrivateProfileInt(m_pName,"LOG_MII",1,m_szIniFile)==1;

//	//get do binary dump bbm
//	m_bDoBinaryBBM = GetPrivateProfileInt(m_pName,"BINARY_BBM",0,m_szIniFile)==1;
//
//	//get the LOG_FILTER from the ini file
//	m_bLogFilter = GetPrivateProfileInt(m_pName,"LOG_FILTER",1,m_szIniFile)==1;

	//get the allow auto time set flag from the ini file
	m_bAllowAutoTime = GetPrivateProfileInt(m_pName,"AUTOTIMESET",1,m_szIniFile)==1;

	//get the SLOWRESET from the ini file
	m_dSlowReset = GetPrivateProfileInt(m_pName,"SLOW_RESET",900,m_szIniFile);
	m_dSlowReset *= 1000;

	//get the port string from the ini file
	//if we are resetting and it has changed then we must unregister
	//and reregister
	char tempport[32];
	if (reseting) strcpy(tempport,m_pPort);
	bool portchange;
	GetPrivateProfileString(m_pName,"PORT","",m_pPort,sizeof(m_pPort)-1,m_szIniFile);
	if (strcmp(tempport,m_pPort)==0)
		portchange = false;
	else
		portchange = true;

	//get the node string from the ini file
	//if we are resetting and it has changed then we must unregister
	//and reregister
	int tempnode;
	tempnode = m_iNode;
	bool nodechange;
	m_iNode = GetPrivateProfileInt(m_pName,"NODE",-1,m_szIniFile);
	if (tempnode == m_iNode)
		nodechange = false;
	else
		nodechange = true;
	
	//get the maximum pause of this instrument
	int maxpause = GetPrivateProfileInt(m_pName,"MAXPAUSE",30000,m_szIniFile);
	if (maxpause < 0) maxpause = 0;
	m_iPauseMax = maxpause * 1000;	//dgp 18 Oct 2006

	//get the data cycle time
	m_dCycleInt = GetPrivateProfileInt(m_pName,"MAXCYCLE",1000,m_szIniFile);
	if (m_dCycleInt < 500) m_dCycleInt = 500;

	//get the maxinqurie interval
	m_dInquireInt = GetPrivateProfileInt(m_pName,"MAXINQUIRE",10,m_szIniFile);
	if (m_dInquireInt < 1) m_dInquireInt = 1;

	//get the maxstatus interval
	m_dStatusInt = GetPrivateProfileInt(m_pName,"MAXSTATUS",0,m_szIniFile);
	if (m_dStatusInt < 0) m_dStatusInt = 0;

	//get the maximum accumulated bbm byte count 
	m_dMaxBBM = GetPrivateProfileInt(m_pName,"MAXBBM",1000,m_szIniFile);
	if (m_dMaxBBM < 0) m_dMaxBBM = 0;

	//get the comm fail NUMBER OF RETRANSMITS prior to COMMFAIL
	m_dCommFail = GetPrivateProfileInt(m_pName,"COMMFAIL",10,m_szIniFile);
	if (m_dCommFail <= 1) m_dCommFail = 10;

	//get the 
	m_dTimeOut = GetPrivateProfileInt(m_pName,"TIMEOUT",5,m_szIniFile);
	if (m_dTimeOut <= 1) m_dTimeOut = 5;
	
	m_dtime_err = GetPrivateProfileInt(m_pName,"TIME_ERR",60,m_szIniFile);

	if (reseting)
	{
		UpdateStrings();

		m_pPropertySheet->SetLimits(m_dtime_err, m_pID);

		m_pModParams->OnModparamsReset();  //<<QA>> does this go away?

		if (!m_bPause)
		{
			KillTimer(TIMER_TICK);
			SetTimer(TIMER_TICK,m_dCycleInt,NULL);
		}
		else
		{
			KillTimer(TIMER_PAUSE);
			SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);	//dgp 18 Oct 2006
		}
	}
	return true;
}

void CEventInstrument::UpdateStrings()
{
	//create the string for the title bar
	CreateFullNameString();  //in ISO base class
	m_pPropertySheet->SetWindowText(m_pFullName);
}


void CEventInstrument::DriveToZero(bool bDriveToZero, bool bInPwrFail)
{
	if (bInPwrFail)
	{
		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, 
			"41545 EVENT COLLECT Power Fail Drive BBM to Zero.");
	}
	PauseInstrument(false);
	m_bDriveToZero = bDriveToZero;
	m_dTickToInquire = m_dInquireInt;
}

void CEventInstrument::IssuePrint(FILE* outfile)
{
	if (m_pPropertySheet)
		m_pPropertySheet->IssuePrint(outfile);

	fprintf(outfile,"MIC Event Instrument Status:\n");
	fprintf(outfile,"  Registered with communications object:");
	fprintf(outfile,m_bGoodRegister?"Yes\n":"No\n");
	fprintf(outfile,"  Checksum Error Count: %d\n",m_dChecksumCount);
	fprintf(outfile,"  Communications Error Count: %d\n",m_dCommFail);
	fprintf(outfile,"  Break Message filter on:");
	fprintf(outfile,(m_dBreakSentCount > m_iMaxRepetition)?"Yes\n":"No\n");
	fprintf(outfile,"  Timeout Message filter on:");
	fprintf(outfile,(m_dTimeOutCount > m_iMaxRepetition)?"Yes\n":"No\n");
	fprintf(outfile,"  Invalid Time Count filter on:");
	fprintf(outfile,(m_dInvalidTimeCount > m_iMaxRepetition)?"Yes\n":"No\n");
	CString Text;
	switch(m_dCurrent_State){
	case 1000: Text = "About to send first COMSTATUSDE. (1000)"; break;
	case 1010: Text = "First COMSTATUSDE sent.  Waiting for response. (1010)"; break;
	case 1030: Text = "Sending second COMSTATUSDE. (1030)"; break;
	case 1040: Text = "Second COMSTATUSDE sent.  Waiting for response. (1040)"; break;
	case 1062:
	case 1061:
	case 1060: Text = "Didn't understand COMSTATUSDE response.  Sending END, USER, YES. (1060-2)"; break;
	case 1070: Text = "Received a ??.  Sending 90.  (1070)"; break;
	case 1100: Text = "Sending DUMPLAST. (1100)"; break;
	case 1110: Text = "DUMPLAST sent.  Waiting for response. (1110)"; break;
	case 1200: Text = "Sending DUMPSTAT. (1200)"; break;
	case 1210: Text = "DUMPSTAT sent.  Waiting for response. (1210)"; break;
	case 1300: Text = "Sending ANALYZE. (1300)"; break;
	case 1310: Text = "ANALYZE sent.  Waiting for response. (1310)"; break;
	case 2000: Text = "About to send INQUIRE2, DUMPBBM, or DUMPSTAT. (2000)"; break;
	case 2100: Text = "INQUIRE2 sent.  Waiting for response. (2100)"; break;
	case 2200: Text = "DUMPBBM sent.  Waiting for response. (2200)"; break;
	case 2210: Text = "Received ENDDUMP record. If no checksum errors about to send DUMPOK. (2210)";break;
	case 2220: Text = "DUMPOK sent.  Waiting for response. (2220)"; break;
	case 2300: Text = "DUMPSTAT sent.  Waiting for response.  (2300)"; break;
	case 8000:
	case 8001:
	case 8009:
	case 8010:
	case 8020: 
//	case 8500:
//	case 8510:
//	case 8520:
//	case 8530: 
//	case 8540: 
	case 8550: Text = "Forcing Instrument to Computer's Time. (8000 - 8020)"; break;
	case 9000: Text = "About to send LONG BREAK SET. (9000)"; break;
	case 9010: Text = "About to clear LONG BREAK SET. (9010)"; break;
	case 9500: Text = "COMMUNICATIONS FAILURE -- In slow (15 minute) reset.  (9500)"; break;
	case 9800:
	case 9810: Text = "WRITE FAILURE on BID file (9800 - 9810)"; break;
	case 9900: Text = "COMMUNICATIONS FAILURE -- Register instrument error or cannot write to disk. (9900)"; break;
	default:
		Text = "UNKNOWN STATE";
	}
	fprintf(outfile,"  %s\n",Text);
	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
	fprintf(outfile,"========================================================\n");
}

void CEventInstrument::Reset()
{
	if (IsPaused())
		m_dCurrent_State = 1000;
}

CEventInstrument::CEventInstrument()
{
//	TRACE("CEventInstrument::CTOR #1\n");
	m_pParent = NULL;
	FinishConstruction();
}

CEventInstrument::CEventInstrument(CWnd* Parent, char *inifile) : CISO(inifile)
{
//	TRACE("CEventInstrument::CTOR #2\n");
	m_pParent = Parent;
	FinishConstruction();
}

void CEventInstrument::FinishConstruction()
{
	m_eCategory = STANDARD_ISO;

	strncpy(m_pType, EVENT_DISCOVERY_TYPE, sizeof(m_pType));

	CString csComma(",");
	CString csExtensions = 
		CString(BNY_SUFFIX) + csComma + 
		GPS_SUFFIX + csComma + 
		VCS_SUFFIX + csComma +
		EVT_SUFFIX + csComma +
		PFM_SUFFIX + csComma +	//DR 353
		SOH_SUFFIX + csComma +	//DR 353
		CEV_SUFFIX;				//DR 353

	strncpy(m_szFileExtensionsList,(LPCTSTR)csExtensions,sizeof(m_szFileExtensionsList));

	m_pSummaryPage		= NULL;
	m_pDataStatus		= NULL;
	m_pBinaryDataStatus = NULL;
	m_pGPSDataStatus	= NULL;
	m_pVACOSSDataStatus = NULL;
	m_pModParams		= NULL;
	m_pLegend			= NULL;
	m_pPropertySheet	= NULL;

	m_bDriveToZero		= false;
	m_bDoAnalyzeAfterDT0= false;
	m_bInHardFail		= false;
	m_bGoodRegister		= false;
	m_dHardFailCount	= 0;

	m_dSlowReset		= SLOWRESET;
	m_dPrevBIDTime		= 0;
	m_dTimeOutCount		= 0;
	m_dInvalidTimeCount = 0;

	m_bIsPaused = true;
	m_dBreakSentCount = 0;

	m_dRetransmit		= 0;		//no retransmitions yet
	m_dLongBreak		= 0;		//not in long break
	m_bdoingBBM			= false;	//start by doing dump bbm //DGP 2007 Sept 4
	m_bdumpOK			= false;	//don't have a good dumpbbm yet
	m_bbbmVal			= false;	//don't have a bbm value yet
	m_bChecksumFail		= false;	//not checksum fail yet
	m_dChecksumCount	= 0;		//no checksum fails yet
	m_bInHardFail		= false;	//not in hard fail of any type yet
	m_bInitInitTimeOut	= true;		//need to initialize initial time out
	m_bInDumpBBM		= false;	//not in dumplast sequence yet
}


CEventInstrument::~CEventInstrument()
{
//	TRACE("CEventInstrument::DTOR\n");

	//tell the world we are shutting down
	char str[512];		//dr00115
	char str2[128];

	if (m_pPropertySheet)
	{
		strcpy(str,"41570 EVENT COLLECT stopped:");
		unsigned long size = sizeof(str2);
		if (::GetUserName(str2,&size))
		{
			strcat(str," Logon ID:");
			strcat(str,str2);
		}
		strcat(str," MIC User ID:");		//dr00115
		GetPrivateProfileString(CONFIGURATION,LAST_VALIDATION,"UNKNOWN",str2,sizeof(str2),m_szIniFile);
		strcat(str,str2);														//dr00115
		strcat(str,"\r\n");														//dr00115

		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, str);
	}

	//destroy each of the pages in the dialog box for this instrument
	if (m_pSummaryPage)
	{
		::DestroyWindow(m_pSummaryPage->m_hWnd);
		delete m_pSummaryPage;
	}
	if (m_pDataStatus)
	{
		::DestroyWindow(m_pDataStatus->m_hWnd);
		delete m_pDataStatus;
	}
	if (m_pBinaryDataStatus)
	{
		::DestroyWindow(m_pBinaryDataStatus->m_hWnd);
		delete m_pBinaryDataStatus;
	}
	if (m_pGPSDataStatus)
	{
		::DestroyWindow(m_pGPSDataStatus->m_hWnd);
		delete m_pGPSDataStatus;
	}
	if (m_pVACOSSDataStatus)
	{
		::DestroyWindow(m_pVACOSSDataStatus->m_hWnd);
		delete m_pVACOSSDataStatus;
	}
	if (m_pModParams)
	{
		::DestroyWindow(m_pModParams->m_hWnd);
		delete m_pModParams;
	}
	if (m_pLegend)
	{
		::DestroyWindow(m_pLegend->m_hWnd);
		delete m_pLegend;
	}
	//destroy the property sheet itself
	if (m_pPropertySheet)
	{
		::DestroyWindow(m_pPropertySheet->m_hWnd);
		delete m_pPropertySheet;
	}
}


BEGIN_MESSAGE_MAP(CEventInstrument, CWnd)
	//{{AFX_MSG_MAP(CEventInstrument)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE,OnReceive)
END_MESSAGE_MAP()































#define DESKTOP_ALL (DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW | \
DESKTOP_CREATEMENU | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALRECORD | \
DESKTOP_JOURNALPLAYBACK | DESKTOP_ENUMERATE | DESKTOP_WRITEOBJECTS | \
DESKTOP_SWITCHDESKTOP | STANDARD_RIGHTS_REQUIRED)

#define WINSTA_ALL (WINSTA_ENUMDESKTOPS | WINSTA_READATTRIBUTES | \
WINSTA_ACCESSCLIPBOARD | WINSTA_CREATEDESKTOP | \
WINSTA_WRITEATTRIBUTES | WINSTA_ACCESSGLOBALATOMS | \
WINSTA_EXITWINDOWS | WINSTA_ENUMERATE | WINSTA_READSCREEN | \
STANDARD_RIGHTS_REQUIRED)

#define GENERIC_ACCESS (GENERIC_READ | GENERIC_WRITE | \
GENERIC_EXECUTE | GENERIC_ALL)

BOOL AddAceToWindowStation(HWINSTA hwinsta, PSID psid);

BOOL AddAceToDesktop(HDESK hdesk, PSID psid);


BOOL GetLogonSID (HANDLE hToken, PSID *ppsid) 
{
   BOOL bSuccess = FALSE;
   DWORD dwIndex;
   DWORD dwLength = 0;
   PTOKEN_GROUPS ptg = NULL;

// Verify the parameter passed in is not NULL.
    if (NULL == ppsid)
        goto Cleanup;

// Get required buffer size and allocate the TOKEN_GROUPS buffer.

   if (!GetTokenInformation(
         hToken,         // handle to the access token
         TokenGroups,    // get information about the token's groups 
         (LPVOID) ptg,   // pointer to TOKEN_GROUPS buffer
         0,              // size of buffer
         &dwLength       // receives required buffer size
      )) 
   {
      if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
         goto Cleanup;

      ptg = (PTOKEN_GROUPS)HeapAlloc(GetProcessHeap(),
         HEAP_ZERO_MEMORY, dwLength);

      if (ptg == NULL)
         goto Cleanup;
   }

// Get the token group information from the access token.

   if (!GetTokenInformation(
         hToken,         // handle to the access token
         TokenGroups,    // get information about the token's groups 
         (LPVOID) ptg,   // pointer to TOKEN_GROUPS buffer
         dwLength,       // size of buffer
         &dwLength       // receives required buffer size
         )) 
   {
      goto Cleanup;
   }

// Loop through the groups to find the logon SID.

   for (dwIndex = 0; dwIndex < ptg->GroupCount; dwIndex++) 
      if ((ptg->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID)
             ==  SE_GROUP_LOGON_ID) 
      {
      // Found the logon SID; make a copy of it.

         dwLength = GetLengthSid(ptg->Groups[dwIndex].Sid);
         *ppsid = (PSID) HeapAlloc(GetProcessHeap(),
                     HEAP_ZERO_MEMORY, dwLength);
         if (*ppsid == NULL)
             goto Cleanup;
         if (!CopySid(dwLength, *ppsid, ptg->Groups[dwIndex].Sid)) 
         {
             HeapFree(GetProcessHeap(), 0, (LPVOID)*ppsid);
             goto Cleanup;
         }
         break;
      }

   bSuccess = TRUE;

Cleanup: 

// Free the buffer for the token groups.

   if (ptg != NULL)
      HeapFree(GetProcessHeap(), 0, (LPVOID)ptg);

   return bSuccess;
}

VOID FreeLogonSID (PSID *ppsid) 
{
    HeapFree(GetProcessHeap(), 0, (LPVOID)*ppsid);
}

BOOL StartInteractiveClientProcess (
    LPTSTR lpszUsername,    // client to log on
    LPTSTR lpszDomain,      // domain of client's account
    LPTSTR lpszPassword,    // client's password
    LPTSTR lpCommandLine    // command line to execute
) 
{
   HANDLE      hToken;
   HDESK       hdesk = NULL;
   HWINSTA     hwinsta = NULL, hwinstaSave = NULL;
   PROCESS_INFORMATION pi;
   PSID pSid = NULL;
   STARTUPINFO si;
   BOOL bResult = FALSE;

// Log the client on to the local computer.

   if (!LogonUser(
           lpszUsername,
           lpszDomain,
           lpszPassword,
           LOGON32_LOGON_INTERACTIVE,
           LOGON32_PROVIDER_DEFAULT,
           &hToken) ) 
   {
      goto Cleanup;
   }

// Save a handle to the caller's current window station.

   if ( (hwinstaSave = GetProcessWindowStation() ) == NULL)
      goto Cleanup;

// Get a handle to the interactive window station.

   hwinsta = OpenWindowStation(
       "winsta0",                   // the interactive window station 
       FALSE,                       // handle is not inheritable
       READ_CONTROL | WRITE_DAC);   // rights to read/write the DACL

   if (hwinsta == NULL) 
      goto Cleanup;

// To get the correct default desktop, set the caller's 
// window station to the interactive window station.

   if (!SetProcessWindowStation(hwinsta))
      goto Cleanup;

// Get a handle to the interactive desktop.

   hdesk = OpenDesktop(
      "default",     // the interactive window station 
      DF_ALLOWOTHERACCOUNTHOOK,             // no interaction with other desktop processes
      FALSE,         // handle is not inheritable
      READ_CONTROL | // request the rights to read and write the DACL
      WRITE_DAC | 
      DESKTOP_WRITEOBJECTS | 
      DESKTOP_READOBJECTS);

// Restore the caller's window station.

   if (!SetProcessWindowStation(hwinstaSave)) 
      goto Cleanup;

   if (hdesk == NULL) 
      goto Cleanup;

// Get the SID for the client's logon session.

   if (!GetLogonSID(hToken, &pSid)) 
      goto Cleanup;

// Allow logon SID full access to interactive window station.

   if (! AddAceToWindowStation(hwinsta, pSid) ) 
      goto Cleanup;

// Allow logon SID full access to interactive desktop.

   if (! AddAceToDesktop(hdesk, pSid) ) 
      goto Cleanup;

// Impersonate client to ensure access to executable file.

   if (! ImpersonateLoggedOnUser(hToken) ) 
      goto Cleanup;

// Initialize the STARTUPINFO structure.
// Specify that the process runs in the interactive desktop.

   ZeroMemory(&si, sizeof(STARTUPINFO));
   si.cb= sizeof(STARTUPINFO);
   si.lpDesktop = TEXT("winsta0\\default");

// Launch the process in the client's logon session.
   ZeroMemory(&si, sizeof(STARTUPINFO));
   si.cb= sizeof(STARTUPINFO);
   si.lpDesktop = TEXT("winsta0\\default");

   bResult = CreateProcessAsUser(
      hToken,            // client's access token
      NULL,              // file to execute
      lpCommandLine,     // command line
      NULL,              // pointer to process SECURITY_ATTRIBUTES
      NULL,              // pointer to thread SECURITY_ATTRIBUTES
      FALSE,             // handles are not inheritable
      NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,   // creation flags
      NULL,              // pointer to new environment block 
      "C:\\Program Files\\Dr. Neumann\\EOSS",              // name of current directory 
      &si,               // pointer to STARTUPINFO structure
      &pi                // receives information about new process
   ); 

// End impersonation of client.

   RevertToSelf();

//   if (bResult && pi.hProcess != INVALID_HANDLE_VALUE) 
//   { 
//      WaitForSingleObject(pi.hProcess, INFINITE); 
//      CloseHandle(pi.hProcess); 
//   } 

   if (pi.hThread != INVALID_HANDLE_VALUE)
      CloseHandle(pi.hThread);  

Cleanup: 

   if (hwinstaSave != NULL)
      SetProcessWindowStation (hwinstaSave);

// Free the buffer for the logon SID.

   if (pSid)
      FreeLogonSID(&pSid);

// Close the handles to the interactive window station and desktop.

   if (hwinsta)
      CloseWindowStation(hwinsta);

   if (hdesk)
      CloseDesktop(hdesk);

// Close the handle to the client's access token.

   if (hToken != INVALID_HANDLE_VALUE)
      CloseHandle(hToken);  

   return bResult;
}

BOOL AddAceToWindowStation(HWINSTA hwinsta, PSID psid)
{
   ACCESS_ALLOWED_ACE   *pace;
   ACL_SIZE_INFORMATION aclSizeInfo;
   BOOL                 bDaclExist;
   BOOL                 bDaclPresent;
   BOOL                 bSuccess = FALSE;
   DWORD                dwNewAclSize;
   DWORD                dwSidSize = 0;
   DWORD                dwSdSizeNeeded;
   PACL                 pacl;
   PACL                 pNewAcl;
   PSECURITY_DESCRIPTOR psd = NULL;
   PSECURITY_DESCRIPTOR psdNew = NULL;
   PVOID                pTempAce;
   SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
   unsigned int         i;

   __try
   {
      // Obtain the DACL for the window station.

      if (!GetUserObjectSecurity(
             hwinsta,
             &si,
             psd,
             dwSidSize,
             &dwSdSizeNeeded)
      )
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
         psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
               GetProcessHeap(),
               HEAP_ZERO_MEMORY,
               dwSdSizeNeeded);

         if (psd == NULL)
            __leave;

         psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
               GetProcessHeap(),
               HEAP_ZERO_MEMORY,
               dwSdSizeNeeded);

         if (psdNew == NULL)
            __leave;

         dwSidSize = dwSdSizeNeeded;

         if (!GetUserObjectSecurity(
               hwinsta,
               &si,
               psd,
               dwSidSize,
               &dwSdSizeNeeded)
         )
            __leave;
      }
      else
         __leave;

      // Create a new DACL.

      if (!InitializeSecurityDescriptor(
            psdNew,
            SECURITY_DESCRIPTOR_REVISION)
      )
         __leave;

      // Get the DACL from the security descriptor.

      if (!GetSecurityDescriptorDacl(
            psd,
            &bDaclPresent,
            &pacl,
            &bDaclExist)
      )
         __leave;

      // Initialize the ACL.

      ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
      aclSizeInfo.AclBytesInUse = sizeof(ACL);

      // Call only if the DACL is not NULL.

      if (pacl != NULL)
      {
         // get the file ACL size info
         if (!GetAclInformation(
               pacl,
               (LPVOID)&aclSizeInfo,
               sizeof(ACL_SIZE_INFORMATION),
               AclSizeInformation)
         )
            __leave;
      }

      // Compute the size of the new ACL.

      dwNewAclSize = aclSizeInfo.AclBytesInUse +
            (2*sizeof(ACCESS_ALLOWED_ACE)) + (2*GetLengthSid(psid)) -
            (2*sizeof(DWORD));

      // Allocate memory for the new ACL.

      pNewAcl = (PACL)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            dwNewAclSize);

      if (pNewAcl == NULL)
         __leave;

      // Initialize the new DACL.

      if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
         __leave;

      // If DACL is present, copy it to a new DACL.

      if (bDaclPresent)
      {
         // Copy the ACEs to the new ACL.
         if (aclSizeInfo.AceCount)
         {
            for (i=0; i < aclSizeInfo.AceCount; i++)
            {
               // Get an ACE.
               if (!GetAce(pacl, i, &pTempAce))
                  __leave;

               // Add the ACE to the new ACL.
               if (!AddAce(
                     pNewAcl,
                     ACL_REVISION,
                     MAXDWORD,
                     pTempAce,
                    ((PACE_HEADER)pTempAce)->AceSize)
               )
                  __leave;
            }
         }
      }

      // Add the first ACE to the window station.

      pace = (ACCESS_ALLOWED_ACE *)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) -
                  sizeof(DWORD));

      if (pace == NULL)
         __leave;

      pace->Header.AceType  = ACCESS_ALLOWED_ACE_TYPE;
      pace->Header.AceFlags = CONTAINER_INHERIT_ACE |
                   INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE;
      pace->Header.AceSize  = sizeof(ACCESS_ALLOWED_ACE) +
                   GetLengthSid(psid) - sizeof(DWORD);
      pace->Mask            = GENERIC_ACCESS;

      if (!CopySid(GetLengthSid(psid), &pace->SidStart, psid))
         __leave;

      if (!AddAce(
            pNewAcl,
            ACL_REVISION,
            MAXDWORD,
            (LPVOID)pace,
            pace->Header.AceSize)
      )
         __leave;

      // Add the second ACE to the window station.

      pace->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
      pace->Mask            = WINSTA_ALL;

      if (!AddAce(
            pNewAcl,
            ACL_REVISION,
            MAXDWORD,
            (LPVOID)pace,
            pace->Header.AceSize)
      )
         __leave;

      // Set a new DACL for the security descriptor.

      if (!SetSecurityDescriptorDacl(
            psdNew,
            TRUE,
            pNewAcl,
            FALSE)
      )
         __leave;

      // Set the new security descriptor for the window station.

      if (!SetUserObjectSecurity(hwinsta, &si, psdNew))
         __leave;

      // Indicate success.

      bSuccess = TRUE;
   }
   __finally
   {
      // Free the allocated buffers.

      if (pace != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)pace);

      if (pNewAcl != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

      if (psd != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

      if (psdNew != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
   }

   return bSuccess;

}

BOOL AddAceToDesktop(HDESK hdesk, PSID psid)
{
   ACL_SIZE_INFORMATION aclSizeInfo;
   BOOL                 bDaclExist;
   BOOL                 bDaclPresent;
   BOOL                 bSuccess = FALSE;
   DWORD                dwNewAclSize;
   DWORD                dwSidSize = 0;
   DWORD                dwSdSizeNeeded;
   PACL                 pacl;
   PACL                 pNewAcl;
   PSECURITY_DESCRIPTOR psd = NULL;
   PSECURITY_DESCRIPTOR psdNew = NULL;
   PVOID                pTempAce;
   SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
   unsigned int         i;

   __try
   {
      // Obtain the security descriptor for the desktop object.

      if (!GetUserObjectSecurity(
            hdesk,
            &si,
            psd,
            dwSidSize,
            &dwSdSizeNeeded))
      {
         if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
         {
            psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
                  GetProcessHeap(),
                  HEAP_ZERO_MEMORY,
                  dwSdSizeNeeded );

            if (psd == NULL)
               __leave;

            psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
                  GetProcessHeap(),
                  HEAP_ZERO_MEMORY,
                  dwSdSizeNeeded);

            if (psdNew == NULL)
               __leave;

            dwSidSize = dwSdSizeNeeded;

            if (!GetUserObjectSecurity(
                  hdesk,
                  &si,
                  psd,
                  dwSidSize,
                  &dwSdSizeNeeded)
            )
               __leave;
         }
         else
            __leave;
      }

      // Create a new security descriptor.

      if (!InitializeSecurityDescriptor(
            psdNew,
            SECURITY_DESCRIPTOR_REVISION)
      )
         __leave;

      // Obtain the DACL from the security descriptor.

      if (!GetSecurityDescriptorDacl(
            psd,
            &bDaclPresent,
            &pacl,
            &bDaclExist)
      )
         __leave;

      // Initialize.

      ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
      aclSizeInfo.AclBytesInUse = sizeof(ACL);

      // Call only if NULL DACL.

      if (pacl != NULL)
      {
         // Determine the size of the ACL information.

         if (!GetAclInformation(
               pacl,
               (LPVOID)&aclSizeInfo,
               sizeof(ACL_SIZE_INFORMATION),
               AclSizeInformation)
         )
            __leave;
      }

      // Compute the size of the new ACL.

      dwNewAclSize = aclSizeInfo.AclBytesInUse +
            sizeof(ACCESS_ALLOWED_ACE) +
            GetLengthSid(psid) - sizeof(DWORD);

      // Allocate buffer for the new ACL.

      pNewAcl = (PACL)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            dwNewAclSize);

      if (pNewAcl == NULL)
         __leave;

      // Initialize the new ACL.

      if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
         __leave;

      // If DACL is present, copy it to a new DACL.

      if (bDaclPresent)
      {
         // Copy the ACEs to the new ACL.
         if (aclSizeInfo.AceCount)
         {
            for (i=0; i < aclSizeInfo.AceCount; i++)
            {
               // Get an ACE.
               if (!GetAce(pacl, i, &pTempAce))
                  __leave;

               // Add the ACE to the new ACL.
               if (!AddAce(
                  pNewAcl,
                  ACL_REVISION,
                  MAXDWORD,
                  pTempAce,
                  ((PACE_HEADER)pTempAce)->AceSize)
               )
                  __leave;
            }
         }
      }

      // Add ACE to the DACL.

      if (!AddAccessAllowedAce(
            pNewAcl,
            ACL_REVISION,
            DESKTOP_ALL,
            psid)
      )
         __leave;

      // Set new DACL to the new security descriptor.

      if (!SetSecurityDescriptorDacl(
            psdNew,
            TRUE,
            pNewAcl,
            FALSE)
      )
         __leave;

      // Set the new security descriptor for the desktop object.

      if (!SetUserObjectSecurity(hdesk, &si, psdNew))
         __leave;

      // Indicate success.

      bSuccess = TRUE;
   }
   __finally
   {
      // Free buffers.

      if (pNewAcl != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

      if (psd != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

      if (psdNew != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
   }

   return bSuccess;
}










void CEventInstrument::OnTimer(UINT nIDEvent) 
{
	//if we have a timer tick then notify the state machine
	//if we have a pause timer rundown then UN-pause this instrument
	switch (nIDEvent)
	{
	//max pause time ran out so we need to un-pause this instrument
	case TIMER_PAUSE:	
		PauseInstrument(false);
		break;

	//normal timer increment -- need to send this to the state machine
	case TIMER_TICK:
		PostMessage(uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
		break;






/*

	case TIMER_TEST:

		{
			//_spawnl(_P_NOWAIT,"C:\\Program Files\\Dr. Neumann\\EOSS\\eoss.exe","eoss.exe",NULL);
			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			ZeroMemory( &pi, sizeof(pi) );

			// Start the child process. 
			if( !CreateProcess( NULL, // No module name (use command line). 
				"\"C:\\Program Files\\Dr. Neumann\\EOSS\\eoss.exe\"", // Command line. 
				NULL,             // Process handle not inheritable. 
				NULL,             // Thread handle not inheritable. 
				FALSE,            // Set handle inheritance to FALSE. 
				0,                // No creation flags. 
				NULL,             // Use parent's environment block. 
				"C:\\Program Files\\Dr. Neumann\\EOSS\\",// Use starting directory. 
				&si,              // Pointer to STARTUPINFO structure.
				&pi )             // Pointer to PROCESS_INFORMATION structure.
			) 
			{
				Beep(1000,1000);
				TRACE( "CreateProcess failed." );
			}
			
			break;
		}

		
			Beep(1000,1000);
//			StartInteractiveClientProcess (
//				"mictest",
//				NULL,
//				"mictest.",
//				"\"C:\\Program Files\\Dr. Neumann\\EOSS\\TokCom.exe\"");    // command line to execute

			StartInteractiveClientProcess (
				"mictest",    // client to log on
				NULL,      // domain of client's account
				"mictest.",    // client's password
				"\"C:\\Program Files\\Dr. Neumann\\EOSS\\eoss.exe\"");    // command line to execute

//			_spawnl(_P_NOWAIT,"C:\\Program Files\\Dr. Neumann\\EOSS\\eoss.exe","eoss.exe",NULL);

		break;
*/





















	}
	//let the system handle all other timer messages 
	CWnd::OnTimer(nIDEvent);
}

bool CEventInstrument::CreateInstrument()
{
//	TRACE("CEventInstrument::CreateInstrument()\n");
//	ASSERT(m_pName != NULL);
//	ASSERT(m_pButton != NULL);

	m_bPause = true;

	if (!UpdateParams(false))	//This no longer check for the port
		return false;			// And may not be needed.

	//create CEventInstrument object -- we won't actually have a window
	//the window will be in a non-modal dialog box 

	CString cs = AfxRegisterWndClass(NULL, NULL, NULL, NULL);

	BOOL result = CreateEx(
			0,               //DWORD dwExStyle, 
			(LPCTSTR)cs,     //LPCTSTR lpszClassName, 
			m_pName,         //LPCTSTR lpszWindowName, 
			0,               //DWORD dwStyle, 
			CRect(0,0,0,0),  //const RECT& rect, 
			NULL,            //CWnd* pParentWnd, 
			0                //UINT nID, 
			);				//LPVOID lpParam = NULL);

	if (result) 
	{
		m_pButton->put_DisplayNumericString(VARIANT_TRUE,10);
		m_pButton->put_FaceColor(COLOR_PAUSE);
		m_pButton->put_Flash(VARIANT_FALSE);
		m_pButton->put_Numeric(0);

		BSTR bstr;
		Convert_Character_String_to_BSTR("BBM: ",bstr);
		m_pButton->put_PreNumericString(bstr);

		//get the time flag status
		m_iFlagTime = GetPrivateProfileInt(m_pName,FLAGTIME,-1,m_szIniFile);
		if (m_iFlagTime == 1)
			m_pButton->ShowIcon_Clock(VARIANT_TRUE);

		//get the other flag status
		m_iFlagOthr = GetPrivateProfileInt(m_pName,FLAGOTHR,-1,m_szIniFile);
		if (m_iFlagOthr == 1)
			m_pButton->ShowIcon_Frown(VARIANT_TRUE);

		//get the comm flag status
		m_iFlagComm = GetPrivateProfileInt(m_pName,FLAGCOMM,-1,m_szIniFile);
		if (m_iFlagComm == 1)
			m_pButton->ShowIcon_Error(VARIANT_TRUE);

		//create each of the pages we'll need in the property sheet
		m_pSummaryPage     = new CEventSummary(IDD_EVENTSUMMARY_PAGE,this);
		m_pDataStatus      = new CEventDataStat(IDD_EVENTDATASTATUS_PAGE);
		m_pBinaryDataStatus= new CBinaryDataStat(IDD_BINARYEVENTDATASTATUS_PAGE, this);
		m_pGPSDataStatus   = new CGPSDataStat(IDD_GPSDATASTATUS_PAGE);
		m_pVACOSSDataStatus= new CVACOSSDataStat(IDD_VACOSSDATASTATUS_PAGE);
		m_pModParams       = new CEventModParam(IDD_EVENTMODPARAMS_PAGE,this,false);
		m_pLegend		   = new CLegend(/*IDD_PROPPAGE_LEGEND*/);

		//create the user interface dialog box for this instrument
		m_pPropertySheet   = new CEventPropertySheet(LPCTSTR(m_pName),this);

		m_pPropertySheet->AddPage(m_pSummaryPage);
		m_pPropertySheet->AddPage(m_pDataStatus);
		m_pPropertySheet->AddPage(m_pGPSDataStatus);
		m_pPropertySheet->AddPage(m_pBinaryDataStatus);
		m_pPropertySheet->AddPage(m_pVACOSSDataStatus);
		m_pPropertySheet->AddPage(m_pModParams);
		m_pPropertySheet->AddPage(m_pLegend);

		DWORD dwStyle = 
			WS_POPUP | WS_CAPTION | DS_MODALFRAME |
			WS_MINIMIZEBOX|WS_OVERLAPPED|WS_SYSMENU;

		m_pPropertySheet->Create(this,dwStyle,0);

		m_pSummaryPage->SetPropertySheet(m_pPropertySheet);
		m_pDataStatus->SetPropertySheet(m_pPropertySheet);
		m_pBinaryDataStatus->SetPropertySheet(m_pPropertySheet);
		m_pGPSDataStatus->SetPropertySheet(m_pPropertySheet);
		m_pVACOSSDataStatus->SetPropertySheet(m_pPropertySheet);
		m_pModParams->SetPropertySheet(m_pPropertySheet);

		//necessary because PropertySheet doesn't creat a page until 
		//  it needs to display it.  So force it to create them all
		for (int i = m_pPropertySheet->GetPageCount();i>=0;i--)
			m_pPropertySheet->SetActivePage(i-1);

		char szTemp[MAX_PATH];
		GetModuleFileName(AfxGetInstanceHandle(),szTemp,sizeof(szTemp));
		char *c = strrchr(szTemp,'\\');//find the last "\"
		*c = '\0';						//make it null
		strcat(szTemp,"\\");
		
		CString cTemp(szTemp);
		cTemp += m_pName;
		cTemp += ".rtf";
		if (_access(cTemp,4))
		{
			cTemp = szTemp;
			cTemp += "EVENT.rtf";
			if (_access(cTemp,4))
			{
				cTemp = 
				"Instrument Button Colors...\n"
				"GREEN_ICON\nNormal state.\n"
				"---------------------------\n"
				"YELLOW_ICON\nA command has been sent from MIC to the instrument and MIC is waiting for a response.\n"
				"---------------------------\n"
				"RED_ICON\nCommunications Failure! A command has been sent and resent but no acceptable response has been received back from the instrument.\n"
				"---------------------------\n"
				"WHITE_ICON\nFLASHING to previous color. The disk is full or the instrument has a failure which is causing data loss\n"
				"---------------------------\n"
				"GRAY_ICON\nMIC has been instructed not to talk to the instrument.  In all cases this state will eventuall revert to a normal operational state.\n"
				"===========================\n"
				"\n"
				"Instrument Button ICONs...\n"
				"SQUARE_ICON Square: One or more of the tabbed pages have a data item out of tolerance or a configuration mismatch.\n"
				"---------------------------\n"
				"CLOCK_ICON Clock: The difference between MIC's time and the instrument's time is greater than the user set tolerance.\n"
				"---------------------------\n"
				"FLAG_ICON Flag:  MIC has sent and resent a command with no acceptable response and consequently commanded a long break reset in an attempt to reinitialize the instrument or a Hard Checksum Error occurred.\n"
				"---------------------------\n"
				"BELL_ICON Bell: The instrument is currently reporting a \"Measurement Interval of Interest\" or equivalent.\n"	
				"---------------------------\n"
				"FACE_ICON Face: The instrument has reported either a power problem or a battery backed up memory problem.\n"	
				"---------------------------\n"
				"FLOPPY_ICON Floppy Disk & Flashing: MIC has stopped writing to disk and talking to the instrument.  This can be caused by the hard drive being too full, the maximum use of the hard drive set too low, or disk write failure.\n"
				"---------------------------\n"
				"SKULL_ICON Skull and Cross Bones: The instrument has repeatedly failed to respond to commands and long break resets.  MIC will attempt to reset it every 15 minutes or instrument is in a state which is causing DATA LOSS.\n"
				"---------------------------\n"
				"BOMB_ICON Bomb: Configuration Error.  MIC is not configured correctly. Edit the MIC.INI file or delete the instrument object and recreate it to correct this problem.\n";
			}	
		}
		m_pLegend->SetText((LPSTR)LPCSTR(cTemp));

		UpdateStrings();

		//set the time to start data collecting 
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);	//dgp 18 Oct 2006

		m_pPropertySheet->SetLimits(m_dtime_err, m_pID);

		m_pModParams->OnModparamsReset();

		m_dCurrent_State = 1000;
		m_dTick_Count = 0;
		m_dTickToInquire = 0;

		//register this instument with the com object 
		//note that the DIALOGBOX gets registered -- so that it will receive 
		//the data coming from the port.  We may want to change this to a 
		//seperate CWndThread window later on if the interface gets sluggish

		VARIANT_BOOL vbResult;

		m_pComm->RegisterISO(
			(ULONG)m_pPropertySheet->m_hWnd,
			m_iNode,
			STANDARD_ISO,
			VARIANT_TRUE,  //is OOB so EventISO can talk directly to ILON
			&vbResult);		//instead of passthrough to serial instrument

		m_bGoodRegister = (vbResult == VARIANT_TRUE);

		m_pNow = COleDateTime::GetCurrentTime();
		m_dPreviousDay = m_pNow.GetDay();

		//SCR 118
		//EstablishVersionString();

		//tell the files that mic is starting for this instrument
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, 
			(m_bNormalShutdown)?TYPE_START:TYPE_ABNORMAL);







/*
		SetTimer(TIMER_TEST,1000*60,NULL);
*/











	}  //if result

	return m_bGoodRegister;
}


bool CEventInstrument::CollectParametersForNewISO(CString& csNameOut)
{
//	TRACE("CEventInstrument::CollectParametersForNewISO()\n");

	//This ISO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new ISO from that data.

	bool bReturn;

	//Collect the data for the new instrument.
	//
	CTPropertySheet tps("",m_iDlgCloseMilliseconds,this);
	//CEventModParam *pModParam = new CEventModParam(IDD_EVENTMODPARAMS_PAGE,this,true);
	CEventModParam modparam(IDD_EVENTMODPARAMS_PAGE,this,true);
	modparam.SetPropertySheet((CEventPropertySheet*)&tps,m_pName);
	tps.AddPage(&modparam);
	tps.SetWizardMode();

	if (tps.DoModal()==ID_WIZFINISH)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(INSTRUMENTS,m_pName,EVENT_DISCOVERY_TYPE,m_szIniFile);
		bReturn = true;
	}
	else
	{
		csNameOut.Empty();
		bReturn = false;
	}

	return bReturn;
}

void CEventInstrument::SetLocalRegisteredMessageValues()
{
//	TRACE("CEventInstrument::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}


//disconnect this instrument from the communications object
//it is registered with
bool CEventInstrument::UnRegInstrument()
{
	//This is different from the base class version
	//of UnRegInstrument();

	//don't attempt to disconnect if it wasn't
	//registered in the first place
	if (m_pComm && m_bGoodRegister)
		m_pComm->UnRegisterISO(m_iNode+OOBFLAG,m_eCategory);
	return true;
}

void CEventInstrument::PostToWatch(char *pMessage)
{
	if ((NULL != pMessage) && (NULL != m_pComm))
	{
		BSTR bstr_out;
		Convert_Character_String_to_BSTR(pMessage, bstr_out);
		m_pComm->SendToWatch(bstr_out, ULONG(m_iNode + OOBFLAG));
	}
}
	
void CEventInstrument::ShowInstrument(bool show)
{
	//if attempting to show instrument then do it and bring it to top
	if (show) 
	{
		m_pPropertySheet->ShowWindow(SW_SHOW);
		m_pPropertySheet->BringWindowToTop();
	} 
	else
		m_pPropertySheet->ShowWindow(SW_HIDE);
}

void CEventInstrument::PauseInstrument(bool pause)
{
	//kill the timer so that it won't unpause us when we don't want
	KillTimer(TIMER_PAUSE);
	KillTimer(TIMER_TICK);

	//if we are changing the pause state then change the button face 
	//and the member variable
	m_pButton->put_FaceColor((pause)?COLOR_PAUSE:COLOR_OK);
	m_pPropertySheet->SetButtonColor((pause)?COLOR_PAUSE:COLOR_OK);
	if (m_bPause != pause)
	{
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, 
			(pause)?"41550 EVENT COLLECT take data stopped.":
					"41560 EVENT COLLECT take data started.");
	}
	m_bPause = pause;
	m_dTimeOutCount = 0;
	m_dBreakSentCount = 0;
	m_dInvalidTimeCount = 0;

	//if we are now paused then set the timer to un-pause us when
	//it expires at end of PauseMax
	if (pause) 
	{
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);	//dgp 18 Oct 2006
		m_pModParams->GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->EnableWindow(TRUE);
	}
	else
	{
		m_pModParams->GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->EnableWindow(FALSE);
		if (m_dCurrent_State == 9500) 
		{
			m_dTick_Count = 0;
			m_dHardFailCount = 0;
			m_dCurrent_State = 1000;
		} else if (m_dCurrent_State == 2000)//force an immediate inq2 if comming out of pause
			m_dTickToInquire = m_dInquireInt;//may want to do this on automatic exit from pause also
		SetTimer(TIMER_TICK,m_dCycleInt,NULL);
		m_bIsPaused = false;
	}
	// Have the MICDlg review the status of the buttons
	if (m_hButtonKeeperHandle)
		::PostMessage(m_hButtonKeeperHandle,m_uWM_CHECKDATABUTTON,0,0);
}
