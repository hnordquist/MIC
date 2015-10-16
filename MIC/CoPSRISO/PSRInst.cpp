// PSRInst.cpp 

#include "stdafx.h"
#include "resource.h"
#include "ISO_definitions.h"
#include "CSO_definitions.h"
#include "INI_definitions.h"
#include "PSRSafePS.h"
#include "PSRInst.h"
#include "PSRDataStat.h"
#include "PSRModParam.h"
#include "PSRConfig2.h"
#include "colors.h"
#include <math.h>

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

//Update Params gets called if initializing an instrument and after modifying it
bool CPSRInst::UpdateParams(bool reseting)
{
	//get the file id to be used
	GetPrivateProfileString(m_pName,FILE_ID,"",m_pID,sizeof(m_pID),m_szIniFile);

	//get the file location to save data at
	GetPrivateProfileString(m_pName,SAVE_LOC,"C:\\DATA\\PSR01",m_szSaveLoc,sizeof(m_szSaveLoc),m_szIniFile);
	
	//get the DO_DUMP from the ini file
	m_bDoDump = GetPrivateProfileInt(m_pName,"DO_DUMP",0,m_szIniFile)==1;

//	Get the LOG_MII from the ini file
//	m_bLogMII = GetPrivateProfileInt(m_pName,"LOG_MII",1,m_szIniFile)==1;

	//get the LOG_FILTER from the ini file
	m_bLogFilter = GetPrivateProfileInt(m_pName,"LOG_FILTER",1,m_szIniFile)==1;

	//get the SLOWRESET from the ini file
	m_dSlowReset = GetPrivateProfileInt(m_pName,"SLOW_RESET",900,m_szIniFile);
	m_dSlowReset *= 1000;

	//get the port string from the ini file
	//if we are resetting and it has changed then we must unregister
	//and reregister
	char tempport[32];
	if (reseting) strcpy(tempport,m_pPort);
	bool portchange;
	GetPrivateProfileString(m_pName,PORT,"",m_pPort,sizeof(m_pPort),m_szIniFile);
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
	m_iNode = GetPrivateProfileInt(m_pName,NODE,-1,m_szIniFile);
	if (tempnode == m_iNode)
		nodechange = false;
	else
		nodechange = true;

	//get the maximum pause of this instrument
	int maxpause = GetPrivateProfileInt(m_pName,"MAXPAUSE",30000,m_szIniFile);
	if (maxpause < 0) maxpause = 0;
	m_iPauseMax = maxpause * 1000;

	//get the data cycle time
	m_dCycleInt = GetPrivateProfileInt(m_pName,"MAXCYCLE",1000,m_szIniFile);
	if (m_dCycleInt < 100) m_dCycleInt = 100;
	if (m_dCycleInt > 1000) m_dCycleInt = 1000;

	//m_dCommFail = 20;//should this be settable
	m_dCommFail = GetPrivateProfileInt(m_pName,"COMMFAIL",20,m_szIniFile);
	if (m_dCommFail <= 1) m_dCommFail = 20;

	//set to approximately 1 second
	m_dTimeOut = 1000 / m_dCycleInt;
	
	//get the high voltage high limit
	m_dHighVoltH = GetPrivateProfileInt(m_pName,"HIGHVOLTH",2000,m_szIniFile);

	//get the high voltage low limit
	m_dHighVoltL = GetPrivateProfileInt(m_pName,"HIGHVOLTL",1000,m_szIniFile);

	//get the sigma threshold for starting a new data record
	char tempvalue[32];
	GetPrivateProfileString(m_pName,"SIGMATHRESH","5.00",tempvalue,sizeof(tempvalue),m_szIniFile);
	m_lSigmaThresh = atof(tempvalue)/10.0;

	//get the totals threshold for starting a new data record
	GetPrivateProfileString(m_pName,"REALSTHRESH","50.00",tempvalue,sizeof(tempvalue),m_szIniFile);
	m_lRealThresh = atof(tempvalue);

	//get the totals threshold for starting a new data record
	GetPrivateProfileString(m_pName,"TOTALSTHRESH","50.00",tempvalue,sizeof(tempvalue),m_szIniFile);
	m_lTotalThresh = atof(tempvalue);

	//get the totals threshold for starting a new data record
	GetPrivateProfileString(m_pName,"TOTALS1THRESH","50.00",tempvalue,sizeof(tempvalue),m_szIniFile);
	m_lTotal1Thresh = atof(tempvalue);

	//get the low totals threshold for starting a new data record
	GetPrivateProfileString(m_pName,"TOTALSLOWTHRESH","1.00",tempvalue,sizeof(tempvalue),m_szIniFile);
	m_lTotalLThresh = atof(tempvalue);

	//get the camera trigger type (should be Real or Totals)
	GetPrivateProfileString(m_pName,"CAMERA_TRIGGER_TYPE","RT1",m_cCameraRealorTotals,sizeof(m_cCameraRealorTotals),m_szIniFile);
	for (unsigned ui = 0; ui < strlen(m_cCameraRealorTotals); ui++) 
		m_cCameraRealorTotals[ui] = (char)toupper(m_cCameraRealorTotals[ui]);

	GetPrivateProfileString(m_pName,"CAMERA_PORT","NONE",tempvalue,sizeof(tempvalue),m_szIniFile);
	for (unsigned int j = 0; j < strlen(tempvalue); j++)
		tempvalue[j] = (char)toupper(tempvalue[j]);
	bool bCameraDo = (tempvalue[0] == 'P');

	SetCamera(reseting);

	//get the maximum number of datapoints to compress
	m_dMaxCompress = GetPrivateProfileInt(m_pName,"MAXCOMPRESS",25,m_szIniFile);

	//get the count time units
	m_dCountTimeUnit = GetPrivateProfileInt(m_pName,"COUNTTIMEUNIT",10,m_szIniFile);

	//get the count time exp
	m_dCountTimeExp = GetPrivateProfileInt(m_pName,"COUNTTIMEEXP", 1, m_szIniFile);

	//get the high voltage set
	m_dHighV = GetPrivateProfileInt(m_pName,"HIGHV",1800,m_szIniFile);

	//get the gate width
	m_dGateWidth = GetPrivateProfileInt(m_pName,"GATEWIDTH",1800,m_szIniFile);

	//get the predelay
	char temp[8];
	GetPrivateProfileString(m_pName,"PREDELAY","1",temp,sizeof(temp),m_szIniFile);
	sscanf(temp,"%x",&m_dPreDelay);

	if (reseting)
	{
		UpdateStrings();
		m_pPropertySheet->SetLimits(
			m_lRealThresh,
			m_lTotalThresh,
			m_lTotal1Thresh,
			m_lTotalLThresh,
			m_lSigmaThresh,
			m_dMaxCompress,
			m_dCountTimeUnit,
			m_dCountTimeExp,
			m_dHighV,
			m_dGateWidth,
			m_dPreDelay,
			m_dHighVoltH, m_dHighVoltL,
			m_cCameraRealorTotals,
//			m_lCameraThreshold,
			bCameraDo,
			m_pID);
		m_pModParams->OnModparamsReset();

		if (!m_bPause)
		{
			KillTimer(TIMER_TICK);
			SetTimer(TIMER_TICK,m_dCycleInt,NULL);
		}
		else
		{
			KillTimer(TIMER_PAUSE);
			SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		}
	}
	return true;
}

void CPSRInst::UpdateStrings()
{
	CreateFullNameString();

	m_pPropertySheet->SetWindowText(m_pFullName);
}

void CPSRInst::ShowInstrument(bool show)
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

void CPSRInst::DriveToZero(bool bDriveToZero, bool bInPwrFail)
{
	if (bInPwrFail)
	{
		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, 
			"53335 PSR COLLECT Power Fail Drive BBM to Zero.\n\r");
	}
	PauseInstrument(true);//opposite from the other instruments
	m_bDriveToZero = bDriveToZero;
}

void CPSRInst::IssuePrint(FILE* outfile)
{
	if (m_pPropertySheet)
		m_pPropertySheet->IssuePrint(outfile);

	fprintf(outfile,"MIC Instrument Status:\n");
	fprintf(outfile,"  Registered with communications object:");
	fprintf(outfile,m_bGoodRegister?"Yes\n":"No\n");
	CString Text;
	switch(m_dCurrent_State){
	case 1000: Text = "About to send initialization. (1000)"; break;
	case 1010: Text = "Waiting for response to PSR initialization. (1010)"; break;
	case 1020: Text = "About to send HX to find PSR mode (1020)"; break;
	case 1030: Text = "Waiting for response to HX. (1030)"; break;
	case 1040: Text = "About to send ^ to force PSR to computer mode. (1040)"; break;
	case 1050: Text = "Waiting for response to ^. (1050)"; break;
	case 1060: Text = "About to send <esc> to force PSR to reset. (1060)"; break;
	case 1100: Text = "About to send J to get PSR setup values. (1100)"; break;
	case 1110: Text = "Waiting for response to J. (1110)"; break;
	case 1120: Text = "About to send Y to get PSR high voltage. (1120)"; break;
	case 1130: Text = "Waiting for response to Y. (1130)"; break;
	case 1140: Text = "About to send Z& to start PSR. (1140)"; break;
	case 2000: Text = "Waiting for end of measurement period. (2000)"; break;
	case 2100: Text = "About to send X to see if PSR finished. (2100)"; break;
	case 3000: Text = "About to send C to get Aux value. (3000)"; break;
	case 3100: Text = "Waiting for response to C. (3100)"; break;
	case 3200: Text = "About to send R to read. (3200)"; break;
	case 3300: Text = "Waiting for response to R and about to send Z&. (3300)"; break;
	case 3401: Text = "Z& sent waiting approximately 1 second. (3401)"; break;
	case 3400: Text = "About to send X to verify running. (3400)"; break;
	case 3410: Text = "Waiting for response to X. (3410)"; break;
	case 3500: Text = "About to send J to read current setup. (3500)"; break;
	case 3510: Text = "Waiting for response to J. (3510)"; break;
	case 3600: Text = "About to send Y to read HV. (3600)"; break;
	case 3610: Text = "Waiting for response to Y. (3610)"; break;
	case 9000: Text = "Communications Failure -- In slow reset. (9000)"; break;
	case 9800:
	case 9810: Text = "WRITE FAILURE on PSR file (9800 - 9810)"; break;
	case 9900: Text = "SETUP FAILURE -- Register instrument error. (9900)"; break;
	default:
		Text = "UNKNOWN STATE";
	}
	fprintf(outfile,"  %s\n",Text);
		COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");

	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
	fprintf(outfile,"========================================================\n");
}

void CPSRInst::Reset()
{
	if (IsPaused())
		m_dCurrent_State = 1000;
}

void CPSRInst::PauseInstrument(bool pause)
{
	//kill the timer so that it won't unpause us when we don't want
	KillTimer(TIMER_PAUSE);
	KillTimer(TIMER_TICK);

	//tell the PSR to stop taking readings
	if (!m_bPause && pause)
	{
		COM_Transmit(ISZSTRING,"H",m_iNode);//^
		m_dCurrent_State = 1000;
		m_bInHardFail = false;
	}

	//if we are changing the pause state then change the button face 
	//and the member variable
	m_pButton->put_FaceColor((pause)?COLOR_PAUSE:COLOR_OK);
	m_pPropertySheet->SetButtonColor((pause)?COLOR_PAUSE:COLOR_OK);
	if (m_bPause != pause)
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, 
			(pause)?"53340 PSR COLLECT take data stopped.\n":
					"53350 PSR COLLECT take data started.\n");
	m_bPause = pause;
//	m_dTimeOutCount = 0;
//	m_dBreakSentCount = 0;

	//The button to apply values to change settings in the PSR instrument
	//toggles as the pause/unpause state of the instrument.
	if(m_pModParams)
		m_pModParams->m_pApplyPSRButton->EnableWindow(m_bPause);//SCR00114

	//if we are now paused then set the timer to un-pause us when
	//it expires at end of PauseMax
	if (pause) 
	{
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		//SetTimer(TIMER_UPDATE,60000,NULL);
	}
	else
	{
		if (m_dCurrent_State == 9500) 
		{
			m_dTick_Count = 0;
//			m_dHardFailCount = 0;
			m_dCurrent_State = 1000;
		}
		else
		{
			m_dTick_Count = 0;
//			m_dHardFailCount = 0;
		}
		SetTimer(TIMER_TICK,m_dCycleInt,NULL);
		m_bIsPaused = false;
	}
	if (m_hButtonKeeperHandle)
		::PostMessage(m_hButtonKeeperHandle,m_uWM_CHECKDATABUTTON,0,0);
}

// CPSRInst
CPSRInst::CPSRInst()
{
//	TRACE("CPSRInst::CTOR #1\n");
	m_pParent = NULL;
	FinishConstruction();
}

CPSRInst::CPSRInst(CWnd* Parent, char *inifile) : CISO(inifile)
{
//	TRACE("CPSRInst::CTOR #2\n");
	m_pParent = Parent;
	FinishConstruction();
}

void CPSRInst::FinishConstruction()
{
	m_eCategory = STANDARD_ISO;

	strncpy(m_pType,PSR_DISCOVERY_TYPE, sizeof(m_pType));

	CString csComma(",");
	CString csExtensions = 
		CString(PSR_SUFFIX) + csComma +
		SOH_SUFFIX + csComma +
		PFM_SUFFIX + csComma +	
		CEV_SUFFIX;				

	strncpy(m_szFileExtensionsList,(LPCTSTR)csExtensions,sizeof(m_szFileExtensionsList));
	//strncpy(m_szFileExtensionsList,PSR_SUFFIX,sizeof(m_szFileExtensionsList));

	m_pSummaryPage		= NULL;
	m_pDataStatus		= NULL;
	m_pModParams		= NULL;
	m_pCamera			= NULL;
	m_pLegend			= NULL;
	m_pPropertySheet	= NULL;

	m_bDriveToZero		= false;
	m_bInHardFail		= false;
	m_bGoodRegister		= false;
	m_dPrevPSRTime		= 0;
	m_bCameraBlocked	= false;

	m_dInquireInt = 50;
	m_dInquireIntAdjust = 0;
	
	m_bCameraHoldingOff = false;
}


CPSRInst::~CPSRInst()
{
	//tell the world we are shutting down
	char str[512];
	char str2[256];

	if (m_pPropertySheet)
	{
		strcpy(str,"53360 PSR COLLECT stopped:");
		unsigned long size = sizeof(str2);
		if (::GetUserName(str2,&size))
		{
			strcat(str," Logon ID:");
			strcat(str,str2);
		}
		strcat(str," MIC User ID:");		
		GetPrivateProfileString(CONFIGURATION,LAST_VALIDATION,"UNKNOWN",str2,sizeof(str2),m_szIniFile);
		strcat(str,str2);														
		strcat(str,"\r\n");														
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, str);
	}

	//destroy each of the pages in the dialog box for this instrument
	if (m_pSummaryPage)
	{
		m_pSummaryPage->DestroyWindow();
		delete m_pSummaryPage;
	}
	if (m_pDataStatus)
	{
		m_pDataStatus->DestroyWindow();
		delete m_pDataStatus;
	}
	if (m_pModParams)
	{
		m_pModParams->DestroyWindow();
		delete m_pModParams;
	}
	if (m_pCamera)
	{
		m_pCamera->DestroyWindow();
		delete m_pCamera;
	}
	if (m_pLegend)
	{
		m_pLegend->DestroyWindow();
		delete m_pLegend;
	}
	//destroy the property sheet itself
	if (m_pPropertySheet)
	{
		m_pPropertySheet->DestroyWindow();
		delete m_pPropertySheet;
	}
}

BEGIN_MESSAGE_MAP(CPSRInst, CWnd)
	//{{AFX_MSG_MAP(CPSRInst)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE,OnReceive)
END_MESSAGE_MAP()



void CPSRInst::OnTimer(UINT nIDEvent) 
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

	//start camera trigger
	case TIMER_CAMERA_START:
		KillTimer(TIMER_CAMERA_START);
		PostMessage(uIDC_RECEIVE,TRIGGER_CAMERA_START);
		break;

	//end camera trigger
	case TIMER_CAMERA_END:
		KillTimer(TIMER_CAMERA_END);
		PostMessage(uIDC_RECEIVE,TRIGGER_CAMERA_END);
		break;
	}

	//let the system handle all other timer messages 
	CWnd::OnTimer(nIDEvent);
}



bool CPSRInst::CreateInstrument(void)
{
//	TRACE("CPSRInst::CreateInstrument()\n");
//	ASSERT(m_pName != NULL);
//	ASSERT(m_pButton != NULL);

	BOOL result = FALSE;

	//find if port exists if it doesn't return false;
	if (!UpdateParams(false))
		return false;

	//char port[32];
	//bool bCameraDo = false;

	//create CPSRInst object -- we won't actually have a window
	//the window will be in a non-modal dialog box 

	CString cs = AfxRegisterWndClass(NULL, NULL, NULL, NULL);

	result = CreateEx(
			0,               //DWORD dwExStyle, 
			(LPCTSTR)cs,     //LPCTSTR lpszClassName, 
			m_pName,         //LPCTSTR lpszWindowName, 
			0,               //DWORD dwStyle, 
			CRect(0,0,0,0),  //const RECT& rect, 
			NULL,            //CWnd* pParentWnd, 
			0                //UINT nID, 
			);

	bool bParallelPortSetupStatusOK = true;

	if (result) 
	{
		//PSRs don't have bbm so don't display numeric
		//Don't care on the radix input....
		m_pButton->put_DisplayNumericString(VARIANT_FALSE,0);
		m_pButton->put_FaceColor(COLOR_PAUSE);
		m_pButton->put_Flash(VARIANT_FALSE);

		//no time flag status for PSR

		//get the other flag status
		m_iFlagOthr = GetPrivateProfileInt(m_pName,FLAGOTHR,-1,m_szIniFile);
		if (m_iFlagOthr == 1)
			m_pButton->ShowIcon_Frown(VARIANT_TRUE);

		//get the comm flag status
		m_iFlagComm = GetPrivateProfileInt(m_pName,FLAGCOMM,-1,m_szIniFile);
		if (m_iFlagComm == 1)
			m_pButton->ShowIcon_Error(VARIANT_TRUE);

		//create each of the pages we'll need in the property sheet
		m_pSummaryPage     = new CPSRSummary(IDD_PSRSUMMARY, this);
		m_pDataStatus      = new CPSRDataStat(IDD_PSRDATASTATUS_PAGE, this);//m_szIniFile);
		m_pModParams       = new CPSRModParam(IDD_PSRMODPARAMS_PAGE,this);
		m_pLegend		   = new CLegend();
		m_pCamera		   = new CPSRConfig2(IDD_PSRCONFIG,this);

		//create the user interface dialog box for this instrument
		m_pPropertySheet   = new CPSRPropertySheet(LPCTSTR(m_pName),this);//, m_szIniFile);

		m_pPropertySheet->AddPage(m_pSummaryPage);
		m_pPropertySheet->AddPage(m_pDataStatus);
		m_pPropertySheet->AddPage(m_pModParams);
		m_pPropertySheet->AddPage(m_pCamera);
		m_pPropertySheet->AddPage(m_pLegend);

		DWORD dwStyle = 
			WS_POPUP | WS_CAPTION | DS_MODALFRAME |
			WS_MINIMIZEBOX|WS_OVERLAPPED|WS_SYSMENU;

		m_pPropertySheet->Create(this,dwStyle,0);

		m_pPropertySheet->RestartAutoCloseTimer();

		m_pSummaryPage->SetPropertySheet(m_pPropertySheet);
		m_pDataStatus->SetPropertySheet(m_pPropertySheet);
		m_pModParams->SetPropertySheet((CPSRPropertySheet*)m_pPropertySheet);
		m_pCamera->SetPropertySheet(m_pPropertySheet,false);

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
			cTemp += "PSR.rtf";
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
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);

		char port[32];
		bool bCameraDo = false;
		GetPrivateProfileString(m_pName,"CAMERA_PORT","NONE",port,sizeof(port),m_szIniFile);
		for (unsigned int ui = 0; ui < strlen(port); ui++)
				port[ui]=(char)toupper(port[ui]);

		m_pPropertySheet->SetLimits(
				m_lRealThresh,
				m_lTotalThresh,
				m_lTotal1Thresh,
				m_lTotalLThresh,
				m_lSigmaThresh,
				m_dMaxCompress,
				m_dCountTimeUnit,
				m_dCountTimeExp,
				m_dHighV,
				m_dGateWidth,
				m_dPreDelay,
				m_dHighVoltH, m_dHighVoltL,
				m_cCameraRealorTotals,
//				m_lCameraThreshold,
				bCameraDo = (port[0] == 'P'),
				m_pID);

			//reset the mod params page
			m_pModParams->OnModparamsReset();

		m_dCurrent_State = 1000;
		m_dTick_Count = 0;
		m_dTickToInquire = 0;

		bParallelPortSetupStatusOK = SetupParallelPort(true);

		//register this instument with the com object 
		//note that the DIALOGBOX gets registered -- so that it will receive 
		//the data coming from the port.  We may want to change this to a 
		//separate CWndThread window later on if the interface gets sluggish

		VARIANT_BOOL vbResult;

		m_pComm->RegisterISO(
			(ULONG)m_pPropertySheet->m_hWnd,
			m_iNode,
			STANDARD_ISO,
			VARIANT_FALSE,  //not OOB
			&vbResult);

		m_bGoodRegister = (vbResult == VARIANT_TRUE);

		m_dRetransmit		= 0;		//no retransmitions yet
		m_dLongBreak		= 0;		//not in long break

		//SCR 118
		//EstablishVersionString();

		//tell the files that mic is starting for this instrument
		m_pPropertySheet->WriteToFile(
			TO_CEV | TO_PFM, 
			(m_bNormalShutdown)?TYPE_START:TYPE_ABNORMAL);

	}  //if(result)
	
	return (m_bGoodRegister && bParallelPortSetupStatusOK);
}


bool CPSRInst::CollectParametersForNewISO(CString& csNameOut)
{
//	TRACE("CPSRInst::CollectParametersForNewISO()\n");

	//This ISO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new ISO from that data.

	bool bReturn;

	//Collect the data for the new instrument.
	//
	CTPropertySheet tps("",m_iDlgCloseMilliseconds,this);

	CPSRModParam modparam_page(IDD_PSRMODPARAMS_PAGE,this,true);
	CPSRConfig2 camera_page(IDD_PSRCONFIG,this,true);

	modparam_page.SetPropertySheet(&tps);
	camera_page.SetPropertySheet(((CPSRPropertySheet *)&tps),true);

	tps.AddPage(&modparam_page);
	tps.AddPage(&camera_page);
	tps.SetWizardMode();

	if (tps.DoModal()==ID_WIZFINISH)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(INSTRUMENTS,m_pName,PSR_DISCOVERY_TYPE,m_szIniFile);
		bReturn = true;
	}
	else
	{
		csNameOut.Empty();
		bReturn = false;
	}

	return bReturn;
}

void CPSRInst::SetLocalRegisteredMessageValues()
{
//	TRACE("CPSRInst::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}

int CPSRInst::CalculateDiff(SYSTEMTIME *Time2, SYSTEMTIME *Time1)
{
//	It is not recommended that you add and subtract values from the 
//	SYSTEMTIME structure to obtain relative times. Instead, you should 
//
//Convert the SYSTEMTIME structure to a FILETIME structure. 
//Copy the resulting FILETIME structure to a ULARGE_INTEGER structure. 
//Use normal 64-bit arithmetic on the ULARGE_INTEGER value. 
	union myTime {
		FILETIME FileTime;
		__int64 ulTime;
	};
	
	myTime myTime1, myTime2, myTime3;
	SYSTEMTIME Time3;
	
	SystemTimeToFileTime(Time1,&myTime1.FileTime);
	SystemTimeToFileTime(Time2,&myTime2.FileTime);
	myTime3.ulTime = myTime2.ulTime - myTime1.ulTime;
	FileTimeToSystemTime(&myTime3.FileTime,&Time3);
	return (Time3.wMilliseconds + 1000*(Time3.wSecond + 60*(Time3.wMinute + 60*Time3.wHour)));
}
			
void
CPSRInst::SetCamera(bool reseting)
{
	char tempvalue[64];
	char port[32];

	//get the camera trigger suppression time
	GetPrivateProfileString(m_pName,"CAMERA_TRIGGER_SUPPRESS","0:0:5:0",tempvalue,sizeof(tempvalue),m_szIniFile);
	long days = 0;
	int hours = 0, minutes = 0, seconds = 0;
	sscanf(tempvalue,"%ld:%d:%d:%d",&days,&hours,&minutes,&seconds);
	m_pCameraSuppress = COleDateTimeSpan(days,hours,minutes,seconds);
	m_pCameraSuppress += HALF_SECOND;

	//get the camera trigger holdoff time
	GetPrivateProfileString(m_pName,"CAMERA_TRIGGER_HOLDOFF","0:0:0:10",tempvalue,sizeof(tempvalue),m_szIniFile);
	sscanf(tempvalue,"%ld:%d:%d:%d",&days,&hours,&minutes,&seconds);
	m_pCameraHoldOff = COleDateTimeSpan(days,hours,minutes,seconds);
	m_pCameraHoldOff += HALF_SECOND;
	if (m_dParallelPortPin && !m_bCameraBlocked)
	{
		m_cParallelPort.DisconnectDevice();
	}

	m_dParallelPortPin = GetPrivateProfileInt(m_pName,"CAMERA_PIN",4,m_szIniFile);
	GetPrivateProfileString(m_pName,"CAMERA_PORT","NONE",port,sizeof(port),m_szIniFile);
	if ((reseting) && (port[0] == 'P'))
	{
		bool result = true;
		if (m_cParallelPort.ConfigureDriver())
		{
			try {
				m_cParallelPort.ConfigureDevice("PORT1",0x378);
				m_cParallelPort.ConfigureDevice("PORT2",0x278);
				m_cParallelPort.ConfigureDevice("PORT3",0x3bc);
				if (m_cParallelPort.ConnectDevice(port)==FALSE)
				{
					m_cParallelPort.StartDriver();
					if (m_cParallelPort.ConnectDevice(port)==FALSE)
						result = false;
				}
			}
			catch(...)
			{
				m_bCameraBlocked = true;
				result = false;
			}
		}
		else
			result = false;

		if (result)
		{
			m_cParallelPort.UnSetPin(m_dParallelPortPin);
			m_bCameraBlocked = false;
		}
		else
		{
			m_dParallelPortPin = 0;
			m_bCameraBlocked = true;
		}
	}
}

bool CPSRInst::SetupParallelPort(bool reseting)
{
	bool bParallelPortSetupStatusOK = true;

	m_dParallelPortPin = GetPrivateProfileInt(m_pName,"CAMERA_PIN",4,m_szIniFile);

	if (reseting)
	{
		char port[32];

		GetPrivateProfileString(m_pName,"CAMERA_PORT","NONE",port,sizeof(port),m_szIniFile);
		port[0]=(char)toupper(port[0]);

		if (port[0] == 'P')
		{
			if (m_cParallelPort.ConfigureDriver())
			{
				m_cParallelPort.ConfigureDevice("PORT1",0x378);
				m_cParallelPort.ConfigureDevice("PORT2",0x278);
				m_cParallelPort.ConfigureDevice("PORT3",0x3bc);
				if (m_cParallelPort.ConnectDevice(port)==FALSE)
				{
					m_cParallelPort.StartDriver();
					if (m_cParallelPort.ConnectDevice(port)==FALSE)
					{
						bParallelPortSetupStatusOK = false;
						m_csLastError = CString("Could not connect to Parallel port device.");
					}
				}
			}
			else
			{
				bParallelPortSetupStatusOK = false;
			}

			if (bParallelPortSetupStatusOK)
				m_cParallelPort.UnSetPin(m_dParallelPortPin);
			else
				m_dParallelPortPin = 0;
		}
	}

	return bParallelPortSetupStatusOK;
}


