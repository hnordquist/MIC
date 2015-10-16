// instrument.cpp
// LAST WriteToFileEx() is 224
#include "stdafx.h"
#include "resource.h"
#include "ISO_definitions.h"
#include "CSO_definitions.h"
#include "INI_definitions.h"
#include "SafePS.h"
#include "instrument.h"
#include "Genl4Stat.h"
#include "DataStat.h"
#include "DataIStat.h"
#include "Data2Stat.h"
#include "User4Prog.h"
#include "CDData.h"
#include "ModParam.h"
#include "SetupData.h"
#include "UGrandSetupData.h"
#include "colors.h"
#include "UGInfo.h"
#include "PSBInfo.h"
#include "ESBInfo.h"
#include "BMBInfo.h"
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

//////////////////////////////////////////////////////////////////////////////
//
//               UpdateParams
//
//////////////////////////////////////////////////////////////////////////////
//Update Params gets called if initializing an instrument and after modifying it
bool CInstrument::UpdateParams(bool reseting)
{
//	TRACE("CInstrument::UpdateParams()\n");

	//get the file id to be used
	GetPrivateProfileString(m_pName,FILE_ID,"",m_pID,sizeof(m_pID),m_szIniFile);

	//get the file location to save data at
	GetPrivateProfileString(m_pName,SAVE_LOC,"C:\\DATA",m_szSaveLoc,sizeof(m_szSaveLoc),m_szIniFile);
	
	//get the DO_DUMP from the ini file
	m_bDoDump = GetPrivateProfileInt(m_pName,"DO_DUMP",0,m_szIniFile)==1;

	//get the LOG_MII from the ini file
	m_bLogMII = GetPrivateProfileInt(m_pName,"LOG_MII",1,m_szIniFile)==1;

	//get do binary dump bbm
	m_bDoBinaryBBM = GetPrivateProfileInt(m_pName,"BINARY_BBM",0,m_szIniFile)==1;

	//get the LOG_FILTER from the ini file
	m_bLogFilter = GetPrivateProfileInt(m_pName,"LOG_FILTER",1,m_szIniFile)==1;

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
	GetPrivateProfileString(m_pName,PORT,"",m_pPort,sizeof(m_pPort)-1,m_szIniFile);
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

	//get the duration to wait for a response to a command
	m_dTimeOut = GetPrivateProfileInt(m_pName,"TIMEOUT",5,m_szIniFile);
	if (m_dTimeOut <= 1) m_dTimeOut = 5;

	char temp[32];
	//get battery and supply limits
	GetPrivateProfileString(m_pName,"BATVOLTH","3.0",temp,sizeof(temp),m_szIniFile);
	m_fbatvolth = (float)atof(temp);

	GetPrivateProfileString(m_pName,"BATVOLTL","2.0",temp,sizeof(temp),m_szIniFile);
	m_fbatvoltl = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P25H","2.6",temp,sizeof(temp),m_szIniFile);
	m_fsup_p25h = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P25L","2.4",temp,sizeof(temp),m_szIniFile);
	m_fsup_p25l = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P33H","3.4",temp,sizeof(temp),m_szIniFile);
	m_fsup_p33h = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P33L","3.2",temp,sizeof(temp),m_szIniFile);
	m_fsup_p33l = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P05H","6.0",temp,sizeof(temp),m_szIniFile);
	m_fsup_p05h = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P05L","4.0",temp,sizeof(temp),m_szIniFile);
	m_fsup_p05l = (float)atof(temp);
	
	m_dtime_err = GetPrivateProfileInt(m_pName,"TIME_ERR",60,m_szIniFile);

	UpdateStrings();
	if (reseting)
	{
		SetLimits(
			m_fbatvolth, m_fbatvoltl,
			m_fsup_p25h, m_fsup_p25l,
			m_fsup_p33h, m_fsup_p33l,
			m_fsup_p05h, m_fsup_p05l,
			m_dtime_err, m_pID);
//		OnModparamsReset();
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

void CInstrument::UpdateStrings()
{
	//create the string for the delete box
	char temp[32];
	strcpy(m_pFullNameSpaces,m_pName);
	strcat(m_pFullNameSpaces,"  Port:");
	strcat(m_pFullNameSpaces,m_pPort);
	strcat(m_pFullNameSpaces,"  Node:");
	if (m_iNode == -1)
		strcat(m_pFullNameSpaces,"N/A");
	else
		strcat(m_pFullNameSpaces,itoa(m_iNode,temp,10));

	//create the string for the title bar
	CreateFullNameString();  //in ISO base class
}

//ShowInstrument has been rewritten and is below...


void CInstrument::DriveToZero(bool bDriveToZero, bool bInPwrFail)
{
	if (bInPwrFail)
	{
		WriteToFile(TO_PFM, TYPE_COMP, NULL, 
			"00000 µGRAND COLLECT Power Fail Drive BBM to Zero.\r\n");
	}
	PauseInstrument(false);
	m_bDriveToZero = bDriveToZero;
	m_dTickToInquire = m_dInquireInt;
	if (bDriveToZero)
		m_bDoAnalyzeAfterDT0 = true;
}

void CInstrument::IssuePrint(FILE* outfile)
{
	IssuePrint2(outfile);
	fprintf(outfile,"MIC Instrument Status:\n");
	fprintf(outfile,"  Registered with communications object: %s\n",
		m_bGoodRegister?"Yes":"No");
	fprintf(outfile,"  Checksum Error Count: %d\n",m_dChecksumCount);
	fprintf(outfile,"  Communications Error Count: %d\n",m_dCommFail);
//	fprintf(outfile,"  Break Message filter on:");
//	fprintf(outfile,(m_dBreakSentCount > g_dMaxRepetition)?"Yes\n":"No\n");
//	fprintf(outfile,"  Timeout Message filter on:");
//	fprintf(outfile,(m_dTimeOutCount > g_dMaxRepetition)?"Yes\n":"No\n");
//	fprintf(outfile,"  Invalid Time Count filter on:");
//	fprintf(outfile,(m_dInvalidTimeCount > g_dMaxRepetition)?"Yes\n":"No\n");
	CString Text;
	switch(m_dCurrent_State){
	case 1000: Text = "About to send first ECHO. (1000)"; break;
	case 1010: Text = "First ECHO sent.  Waiting for response. (1010)"; break;
	case 1020: Text = "Second ECHO sent.  Waiting for response. (1020)"; break;
	case 1030: Text = "Sending second ECHO. (1030)"; break;
	case 1040: Text = "Third ECHO sent.  Waiting for response. (1040)"; break;
	case 1062:
	case 1061:
	case 1060: Text = "Didn't understand ECHO response.  Sending END, USER, YES. (1060-2)"; break;
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
	case 8010:
	case 8020: 
	case 8500:
	case 8510:
	case 8520:
	case 8530: 
	case 8540: 
	case 8550: Text = "Forcing Instrument to Computer's Time. (8000 - 8530)"; break;
	case 9000: Text = "About to send LONG BREAK SET. (9000)"; break;
	case 9010: Text = "About to clear LONG BREAK SET. (9010)"; break;
	case 9500: Text = "COMMUNICATIONS FAILURE -- In slow (15 minute) reset.  (9500)"; break;
	case 9800:
	case 9810: Text = "WRITE FAILURE on BID file (9800 - 9810)"; break;
	case 9900: Text = "COMMUNICATIONS FAILURE -- Register instrument error or cannot write to disk. (9900)"; break;
	default:
			Text.Format("CURRENT STATE (%d)");
	}
	fprintf(outfile,"  %s\n",Text);
	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
	fprintf(outfile,"========================================================\n");
}

void CInstrument::Reset()
{
	if (IsPaused())
	{
		m_dRetransmit = 0;
		m_bInHardFail = false;
		m_dTimeOutCount = 0;
		m_dCurrent_State = 1000;
#ifdef ALLOWTRIGGERHOLDOFF
		DelExclusion();
#endif
	}
}
///////////////////////////////////////////////////////
//
//            ReadExclusion
//            WriteExclusion
//            AddExclusion
//            DelExclusion
//
//    ALL CONDITIONALLY COMPILED
//
//////////////////////////////////////////////////////
#ifdef ALLOWTRIGGERHOLDOFF
//read the exclusion list from the ini file 
//and put them in the CList -- only called
//during instrument creation
void 
CInstrument::ReadExclusion()
{
	int iIndex;
	char szIndex[8];
	char szExclusionValues[32];
	char szExclusionName[_MAX_PATH];
	strcpy(szExclusionName,m_pName);
	strcat(szExclusionName,"_EXCLUSIONS");
	iIndex = 1;
	EXCLUSIONREC ExclusionRec;
	do
	{
		itoa(iIndex,szIndex,10);
		szExclusionValues[0] = NULL;
		GetPrivateProfileString(szExclusionName,szIndex,"",szExclusionValues,sizeof(szExclusionValues),m_szIniFile);
		if (strlen(szExclusionValues))
		{
			if (sscanf(szExclusionValues,"%d,%d",&ExclusionRec.uiStartTime,&ExclusionRec.uiEndTime)==2)
				m_ExclusionList.AddTail(ExclusionRec);
		}
		iIndex++;
	}while (strlen(szExclusionValues));
}

//write the exclusion list from the CList
//out to the ini file -- only called
//during instrument destruction
void 
CInstrument::WriteExclusion()
{
	int iIndex;
	char szIndex[8];
	char szValue[16];
	char szExclusionName[_MAX_PATH];
	char szExclusionValues[32];

	//get the name of the exclusions section in the ini file
	strcpy(szExclusionName,m_pName);
	strcat(szExclusionName,"_EXCLUSIONS");
	iIndex = 1;
	//get the default minutes
	int iExcludeMin = GetPrivateProfileInt(szExclusionName,"MINUTES",-1,m_szIniFile);
	//clear out the old section
	WritePrivateProfileString(szExclusionName,NULL,NULL,m_szIniFile);
	itoa(iExcludeMin,szValue,10);
	//start a new copy of the section and save the minutes
	WritePrivateProfileString(szExclusionName,"MINUTES",szValue,m_szIniFile);
	//see if we have any exclusion periods
	POSITION pos = m_ExclusionList.GetHeadPosition();
	//if we do then write them to the new section
	if (pos)
	{
		for (int i=0;i < m_ExclusionList.GetCount();i++)
		{
			itoa(iIndex,szIndex,10);
			EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
			sprintf(szExclusionValues,"%d,%d",sExclusionRec.uiStartTime,sExclusionRec.uiEndTime);
			WritePrivateProfileString(szExclusionName,szIndex,szExclusionValues,m_szIniFile);
			iIndex++;
		}
	}
}

//add an exclusion period into the CList
//that stores them -- called when ever
//the signal(message) is received
void
CInstrument::AddExclusion(LPARAM lParam)
{
	m_pButton->SetPreNum("Ex BBM: ");
	char szExclusionName[_MAX_PATH];
	strcpy(szExclusionName,m_pName);
	strcat(szExclusionName,"_EXCLUSIONS");
	int iExcludeMin;
	if (lParam >= 0)
	{
		if (lParam == 0)
			iExcludeMin = GetPrivateProfileInt(szExclusionName,"MINUTES",1,m_szIniFile);
		else
			iExcludeMin = lParam;
		SetTimer(TIMER_EXCLUSION,iExcludeMin*60*1000,NULL);
		COleDateTime cOleStart = COleDateTime::GetCurrentTime();
		COleDateTime cOleBase(1952,1,1,0,0,0);
		COleDateTimeSpan cOleJulian;
		cOleJulian = cOleStart - cOleBase;
		EXCLUSIONREC ExclusionRec;
		ExclusionRec.uiStartTime = (unsigned int)cOleJulian.GetTotalSeconds();
		ExclusionRec.uiEndTime = ExclusionRec.uiStartTime + iExcludeMin*60;
		m_ExclusionList.AddTail(ExclusionRec);
		CString cString;
		cString.Format("Exclusion Message: %d to %d, %d min",
			ExclusionRec.uiStartTime,ExclusionRec.uiEndTime,iExcludeMin);
		char szString[128];
		strcpy(szString,cString);
		PostToWatch(szString);
		UpdateSummary(szString);
	}
	else
	{
		char szString[64];
		sprintf(szString,"Bad Exclusion Message, %d min", lParam);
		PostToWatch(szString);
		UpdateSummary(szString);
	}
}

void
CInstrument::DelExclusion()
{
	//remove all of the exclusion periods
	m_ExclusionList.RemoveAll();
	//update the INI file
	WriteExclusion();
	//update the button
	m_pButton->SetPreNum("BBM: ");
	//stop the exclusion timer
	KillTimer(TIMER_EXCLUSION);
	//tell the world
	char szString[128];
	strcpy(szString,"Exclusion Clear Message Received");
	PostToWatch(szString);
	UpdateSummary(szString);
}
#endif

void CInstrument::PauseInstrument(bool pause)
{
//	TRACE("CInstrument::PauseInstrument(%s)\n", pause?"true":"false");

	//kill the timer so that it won't unpause us when we don't want
	KillTimer(TIMER_PAUSE);
	KillTimer(TIMER_TICK);
	//KillTimer(TIMER_UPDATE);

	//if we are changing the pause state then change the button face 
	//and the member variable
	m_pButton->put_FaceColor((pause)?COLOR_PAUSE:COLOR_OK);
	if (m_pPropertySheet)
		m_pPropertySheet->SetButtonColor((pause)?COLOR_PAUSE:COLOR_OK);
	if (m_bPause != pause)
		WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, 
			(pause)?"00000 µGRAND COLLECT take data stopped.\r\n":
					"00000 µGRAND COLLECT take data started.\r\n");
	m_bPause = pause;
	m_dTimeOutCount = 0;
	m_dBreakSentCount = 0;
	m_dInvalidTimeCount = 0;

	//if we are now paused then set the timer to un-pause us when
	//it expires at end of PauseMax
	if (pause) 
	{
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		//SetTimer(TIMER_UPDATE,60000,NULL);
//		m_pModParams->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(true);
	}
	else
	{
//		m_pModParams->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(false);
		if (m_dCurrent_State == 9500) 
		{
			m_dTick_Count = 0;
			m_dHardFailCount = 0;
			m_dCurrent_State = 1000;
		}
		SetTimer(TIMER_TICK,m_dCycleInt,NULL);
		m_bIsPaused = false;
	}

	// Have the MICDlg review the status of the buttons
	if (m_hButtonKeeperHandle)
		::PostMessage(m_hButtonKeeperHandle,m_uWM_CHECKDATABUTTON,0,0);
}


// CInstrument
CInstrument::CInstrument()
{
//	TRACE("CInstrument::CTOR #1\n");
	m_pParent = NULL;
	FinishConstruction();
}
CInstrument::CInstrument(CWnd* Parent, char *inifile) : CISO(inifile)
{
//	TRACE("CInstrument::CTOR #2\n");
	m_pParent = Parent;
	FinishConstruction();
}

void CInstrument::FinishConstruction()
{
	m_eCategory = STANDARD_ISO;

	strncpy(m_pType, UGRAND_DISCOVERY_TYPE, sizeof(m_pType));
	strcpy(m_szFileExtensionsList,"CEV,");
	strcat(m_szFileExtensionsList,"PFM,");
	strcat(m_szFileExtensionsList,"SOH,");
	strcat(m_szFileExtensionsList,MGD_SUFFIX);

	m_pSummaryPage		= NULL;
///	m_pGeneral4StatPage = NULL;
	m_pDataStatus		= NULL;
	m_pDataIStatus		= NULL;
	m_pData2Status		= NULL;
	m_pUser4ProgStatus	= NULL;
	m_pCDData			= NULL;
	m_pModParams		= NULL;
	m_pSetupData		= NULL;
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

	m_pComm = NULL;

//	m_eMonitorVers = UNKNOWN;

///	m_iGeneral4_Tab	= 0;
	m_iData_Tab		= 0;
	m_iDataI_Tab		= 0;
	m_iData2_Tab		= 0;
	m_iUser4Prog_Tab	= 0;
	m_iInst4Setup_Tab	= 0;
	m_iUGInfo_Tab		= 0;
	m_iBMBInfo_Tab		= 0;
	m_iPSBInfo_Tab		= 0;
	m_iESBInfo_Tab		= 0;

	m_fTNSRHigh		= 0.0;
	m_fTNSRLow		= 0.0;
	m_dTNSRFirst	= true;

	m_fDIRHigh		= 0.0;
	m_fDIRLow		= 0.0;
	m_dDIRFirst		= true;

	m_fFBHigh		= 0.0;
	m_fFBLow		= 0.0;
	m_bFBFirst		= true;

	m_bBatteryOk	= true;
	m_bBatteryTolOk	= true;
	m_bP5VoltOk		= true;
	m_bP25VoltOk	= true;
	m_bP33VoltOk	= true;
	m_bACPowerOk	= true;
	m_bACPowerLoss	= false;
	m_bBBMOk		= true;
	m_bCountRateTamperOk = true;
	m_bRMSSigmaTamperOk = true;
	m_bInMII		= false;
	m_bNewFileName	= false;
	m_bDataFiltered	= false;
	m_bTimeError	= false;

	m_bFirst15Rec	= true;

	m_fLowBV = m_fCurBV = m_fHighBV = 0.0;
	m_bFirstBV = true;
	m_fLowP25 = m_fCurP25 = m_fHighP25 = 0.0;
	m_bFirstP25 = true;
	m_fLowP33 = m_fCurP33 = m_fHighP33 = 0.0;
	m_bFirstP33 = true;
	m_fLowP5 = m_fCurP5 = m_fHighP5 = 0.0;
	m_bFirstP5 = true;

	m_bStartOfFile			= true;
	m_dCWStart				= -1; //neither COLD START (0) nor WARM START (1)

	m_szCurrentFileName[0]	= NULL;
	m_szPreviousGTime[0]	= NULL;
	m_dCurrentDay			= -1;
	m_dPreviousBIDTime		= 0xa5a5a5a5;

	m_dAt					= 0;
	m_dBinaryAt				= 0;
	m_dAcquireRecordCountX	= 0;
	m_dRecordCountX			= 0;

	m_dBinaryChecksum		= 0;
	m_dBinaryState			= 0;
	m_dBinaryAT				= 0;
	m_usLength				= 0;
	m_usMessageType			= 0;
	m_dRecLengthCount		= 0;
	m_dGivenChecksum		= 0;
	m_dChecksum				= 0;
	m_bCouldNotOpenBinaryFile = false;

	m_bFCHVError			= false;
	m_bICHVError			= false;
	m_iFCHVError			= 0;
	m_iICHVError			= 0;
	m_dCommFail				= 0;

	m_uiuGConfigRecordTime	= 0;
	m_uiuGChConfigRecordTime= 0;	
	m_uiESBConfigRecordTime	= 0;	
	m_uiPSBConfigRecordTime	= 0;	
	m_uiBMBConfigRecordTime	= 0;	
	m_duGStatusRecordTime	= 0.0;	
	m_duPSBStatusRecordTime	= 0.0;	
	m_duBMBStatusRecordTime	= 0.0;	
	m_uiuGInfoRecordTime	= 0;		
	m_uiESBInfoRecordTime	= 0;		
	m_uiPSBInfoRecordTime	= 0;		
	m_uiBMBInfoRecordTime	= 0;		
	m_uiInformationRecordTime= 0;	
	m_uiTriggerConfigureRecordTime= 0;







	m_cStatusRec.Empty();
	m_cOffsetRec.Empty();
	m_cIDRec.Empty();
	m_cBatteryRec.Empty();
	m_cTripNSRec.Empty();
//	m_cDualIonRec.Empty();
	m_cGenStatRec.Empty();
	m_cMon_A_Rec.Empty();
	m_cTripN2Rec.Empty();
	m_cID2_Record.Empty();
	m_cID3_Record.Empty();
	m_cAnalyzeOkRec.Empty();
	m_cAnalyzeOk3Rec.Empty();
	m_cInformRec.Empty();
	m_cMon_B_Rec.Empty();
	m_cuGConfigRecord.Empty();
	m_cUGStatusRecord.Empty();
	m_cUGInfoRecord.Empty();
	m_cESBInfoRecord1.Empty();
	m_cESBInfoRecord2.Empty();
	m_cESBInfoRecord3.Empty();
	m_cESBInfoRecord4.Empty();
	m_cPSBInfoRecord1.Empty();
	m_cPSBInfoRecord2.Empty();
	m_cPSBInfoRecord3.Empty();
	m_cPSBInfoRecord4.Empty();
	m_cBMBInfoRecord1.Empty();
	m_cBMBInfoRecord2.Empty();
	m_cBMBInfoRecord3.Empty();
	m_cBMBInfoRecord4.Empty();
	m_cInstrumentStatusRecord.Empty();
	m_cInstrumentInfoRecord.Empty();
//	m_cDualCurrentModeConfigRecord.Empty();
	m_cTriggerConfigRecord.Empty();
	m_cChannel0ConfigRecord.Empty();
	m_cChannel1ConfigRecord.Empty();
	m_cChannel2ConfigRecord.Empty();
	m_cChannel3ConfigRecord.Empty();
	m_cChannel4ConfigRecord.Empty();
	m_cChannel5ConfigRecord.Empty();
	m_cChannel6ConfigRecord.Empty();

	int i;
	for (i = 0; i < 6; i++)
		m_cSummaryRecent[i].Empty();
	for (i = 0; i < NUMBEROFACQUIRERECS1DISPLAYED; i++) 
		m_cAcquireRec1[i].Empty();
	for (i = 0; i < NUMBEROFACQUIRERECS2DISPLAYED; i++)
		m_cAcquireRec2[i].Empty();
	m_cGENERAL_CHECKSUM_text.Empty();
	m_cSetParamMatchText.Empty();

	for (i = 0; i < sizeof(SummaryStaticColors)/sizeof(COLORREF); i++)
		SummaryStaticColors[i] = COLOR_DEFAULT_TEXT;

///	for (i = 0; i < sizeof(General4StaticColors)/sizeof(COLORREF); i++)
///		General4StaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(User4StaticColors)/sizeof(COLORREF); i++)
		User4StaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(DataStaticColors)/sizeof(COLORREF); i++)
		DataStaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(DataIStaticColors)/sizeof(COLORREF); i++)
		DataIStaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(Data2StaticColors)/sizeof(COLORREF); i++)
		Data2StaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(Setup4StaticColors)/sizeof(COLORREF); i++)
		Setup4StaticColors[i] = COLOR_DEFAULT_TEXT;
	
	for (i = 0; i < (sizeof(m_iMessageLimiter)/sizeof(int)); i++)
		m_iMessageLimiter[i] = m_iMaxRepetition;//0-302 used, see WriteToFileEx

}

CInstrument::~CInstrument()
{
//	TRACE("CInstrument::DTOR\n");

	//tell the world we are shutting down
#ifdef ALLOWTRIGGERHOLDOFF
	WriteExclusion();
#endif
	char szString[512];											//dr00115
	char szString2[128];
	strcpy(szString,"00000 µGRAND COLLECT stopped:");
	unsigned long ulSize = sizeof(szString2);
	if (::GetUserName(szString2,&ulSize))
	{
		strcat(szString," Logon ID:");
		strcat(szString,szString2);
	}
	strcat(szString," MIC User ID:");							//dr00115
//	strcat(szString,g_pVal_Name);  //<<QA>> put back in.		//dr00115
	GetPrivateProfileString(CONFIGURATION,LAST_VALIDATION,"UNKNOWN",szString2,sizeof(szString2),m_szIniFile);
	strcat(szString,szString2);									//dr00115
	strcat(szString,"\r\n");										//dr00115
	WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, szString);

	//destroy the button on the main screen dialog
	//if (m_pButton) delete m_pButton;
	
	//destroy the property sheet itself
	if (m_pPropertySheet)
	{
		m_pPropertySheet->EndDialog(0);
		delete m_pPropertySheet; //PropertySheet
		m_pPropertySheet = NULL;
	}
}

BEGIN_MESSAGE_MAP(CInstrument, CWnd)
	//{{AFX_MSG_MAP(CInstrument)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE           (IDC_RECEIVE_RESULT, OnReceive)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE_OOB, OnReceiveOOB)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE,OnReceiveData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstrument message handlers

void CInstrument::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//if we have a timer tick then notify the state machine
	//if we have a pause timer rundown then UN-pause this instrument
	switch (nIDEvent)
	{
	//max pause time ran out so we need to un-pause this instrument
	case TIMER_PAUSE:	
		PauseInstrument(false);
		break;

	//during pause update the network information 1 per minute
	//case TIMER_UPDATE:
	//	m_pButton->Redraw(false);
	//	break;

		//<<QA>> need to find out about this.
	//normal timer increment -- need to send this to the state machine
	case TIMER_TICK:
		PostMessage(IDC_RECEIVE_RESULT,TIMER_TICK,CHECKSUM_GOOD);
		break;

#ifdef ALLOWTRIGGERHOLDOFF
	//turn off the EXCLUSION flag
	case TIMER_EXCLUSION:
		m_pButton->SetPreNum("BBM: ");
		KillTimer(TIMER_EXCLUSION);
		break;
#endif
	}
	//let the system handle all other timer messages 
	CWnd::OnTimer(nIDEvent);
}

bool CInstrument::CreateInstrument()
{
//	TRACE("CInstrument::CreateInstrument()\n");

//	ASSERT(m_pName != NULL);
//	ASSERT(m_pButton != NULL);

	m_bPause = true;
	m_dBreakSentCount = 0;
	m_bIsPaused = true;
	m_iCurrentBBM = 0;

	m_bGoodRegister = false;

	if (!UpdateParams(false))  //<<QA>> is this needed?  Comm detection is different.
		return false;

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
		char bbmstring[] = {"BBM: "};
		BSTR bstr;
		Convert_Character_String_to_BSTR(bbmstring, bstr);
		m_pButton->put_PreNumericString(bstr);

		m_pButton->put_DisplayNumericString(VARIANT_TRUE, 10);
		m_pButton->put_FaceColor(COLOR_PAUSE);
		m_pButton->put_Flash(VARIANT_FALSE);
		m_pButton->put_Numeric(0);

		//get the time flag status
		m_iFlagTime = GetPrivateProfileInt(m_pName,FLAGTIME,-1,m_szIniFile);
		if (m_iFlagTime == 1)
		{
			WriteToFileEx(307,TO_PFM,TYPE_COMP,NULL,"00000 Icon turned on from previous execution of MIC [TIME icon on]\r\n");
			m_pButton->ShowIcon_Clock(VARIANT_TRUE);
		}
		//get the other flag status (frown face is "other")
		m_iFlagOthr = GetPrivateProfileInt(m_pName,FLAGOTHR,-1,m_szIniFile);
		if (m_iFlagOthr == 1)
		{
			WriteToFileEx(308,TO_PFM,TYPE_COMP,NULL,"00000 Icon turned on from previous execution of MIC [FROWN FACE icon on]\r\n");
			m_pButton->ShowIcon_Frown(VARIANT_TRUE);
		}
		//get the comm flag status
		m_iFlagComm = GetPrivateProfileInt(m_pName,"FLAGCOMM",-1,m_szIniFile);
		if (m_iFlagComm == 1)
		{
			WriteToFileEx(309,TO_PFM,TYPE_COMP,NULL,"00000 Icon turned on from previous execution of MIC [FLAG icon on]\r\n");
			m_pButton->ShowIcon_Error(VARIANT_TRUE);
		}

#ifdef ALLOWTRIGGERHOLDOFF
		ReadExclusion();
#endif

	//set the time to start data collecting 
	SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);

	SetLimits(m_fbatvolth, m_fbatvoltl,
				m_fsup_p25h, m_fsup_p25l,
				m_fsup_p33h, m_fsup_p33l,
				m_fsup_p05h, m_fsup_p05l,
				m_dtime_err, m_pID);
//			OnModparamsReset();

	m_dCurrent_State = 1000;
	m_dTick_Count = 0;
	m_dTickToInquire = 0;

	VARIANT_BOOL vbResult;

	m_pComm->RegisterISO(
			(ULONG)m_hWnd,
			m_iNode,
			STANDARD_ISO,
			VARIANT_FALSE,  // ~OOB
			&vbResult);

	m_bGoodRegister = (vbResult == VARIANT_TRUE);

	m_pComm->RegisterISO(
			(ULONG)m_hWnd,
			m_iNode+OOBFLAG,
			STANDARD_ISO,
			VARIANT_TRUE,	// OOB
			NULL);			//don't care about the return value here

	m_dRetransmit		= 0;		//no retransmitions yet
	m_dLongBreak		= 0;		//not in long break
	m_bdoingBBM			= true;		//start by doing dump bbm
	m_bdumpOK			= false;	//don't have a good dumpbbm yet
	m_bbbmVal			= false;	//don't have a bbm value yet
	m_bChecksumFail		= false;	//not checksum fail yet
	m_dChecksumCount	= 0;		//no checksum fails yet
	m_bInHardFail		= false;	//not in hard fail of any type yet
	m_bInitInitTimeOut	= true;		//need to initialize initial time out
	m_bInDumpLast		= false;	//not in dumplast sequence yet

	m_pNow = COleDateTime::GetCurrentTime();
	m_dPreviousDay = m_pNow.GetDay();

	EstablishVersionString();

	//tell the files that mic is starting for this instrument

	WriteToFile(TO_CEV | TO_PFM, 
		(m_bNormalShutdown)?TYPE_START:TYPE_ABNORMAL);// UGRAND COLLECT started

	}  //if m_bGoodRegister
	
	return m_bGoodRegister;
}

//////////////////////////////////////////////////////////////////////////////
//
//                  CollectParametersForNewISO
//
//////////////////////////////////////////////////////////////////////////////
bool CInstrument::CollectParametersForNewISO(CString& csNameOut)
{
//	TRACE("CInstrument::CollectParametersForNewISO()\n");

	//This ISO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new ISO from that data.

	bool bReturn;

	//Collect the data for the new instrument.
	//
	CTPropertySheet tps("",m_iDlgCloseMilliseconds,this);
	CModParam modparam_page(IDD_MODPARAMS_PAGE,
	                             this,        //parent
								 m_szIniFile, 
								 true);       //Temporary ISO = true

	modparam_page.SetPropertySheet(&tps,m_pName);
	tps.AddPage(&modparam_page);
	tps.SetWizardMode();

	if (tps.DoModal()==ID_WIZFINISH)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(INSTRUMENTS,m_pName,UGRAND_DISCOVERY_TYPE,m_szIniFile);
		bReturn = true;
	}
	else
	{
		csNameOut.Empty();
		bReturn = false;
	}

	return bReturn;
}

void CInstrument::SetLocalRegisteredMessageValues()
{
//	TRACE("CInstrument::SetLocalRegisteredMessageValues()\n");
	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}

void CInstrument::SetLimits(
	float batvolth, float batvoltl,
	float sup_p25h, float sup_p25l,
	float sup_p33h, float sup_p33l,
	float sup_p05h, float sup_p05l,
	int time_err,	char* Id)
{	
	m_fBatVoltH=batvolth;
	m_fBatVoltL=batvoltl;
	m_fSup_P25H=sup_p25h;
	m_fSup_P25L=sup_p25l;
	m_fSup_P33H=sup_p33h;
	m_fSup_P33L=sup_p33l;
	m_fSup_P05H=sup_p05h;
	m_fSup_P05L=sup_p05l;
	m_dTime_Err=(long)time_err;
	strcpy(m_szCurrentID,Id);
}

void CInstrument::UpdateSummary(char *Message)
{
	CString cString(Message);
	cString.TrimRight("\r\n");
	if (m_pPropertySheet)
		m_pPropertySheet->UpdateSummary((LPSTR)LPCSTR(cString));

	m_cSummaryRecent[5] = m_cSummaryRecent[4];
	m_cSummaryRecent[4] = m_cSummaryRecent[3];
	m_cSummaryRecent[3] = m_cSummaryRecent[2];
	m_cSummaryRecent[2] = m_cSummaryRecent[1];
	m_cSummaryRecent[1] = m_cSummaryRecent[0];
	m_cSummaryRecent[0] = cString;
}

void CInstrument::SetChecksumDisplay()
{
	CString cString;
	cString = m_cCheckSumMsg;
	cString += " ";
	cString += m_cCommStatus;
///	SetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,cString);
	SetString(DATAIPAGE,IDC_DATAI_CHECKSUM,cString);
	if (m_pPropertySheet)
	{
///		if (m_pGeneral4StatPage) 
///			m_pGeneral4StatPage->SetDlgItemText(IDC_GENERAL4_CHECKSUM,cString);
		if (m_pDataIStatus) 
			m_pDataIStatus->SetDlgItemText(IDC_DATAI_CHECKSUM,cString);
	}
}

void CInstrument::OnReceiveOOB(WPARAM WParam, LPARAM LParam)
{
//	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;
	char* cbuff = (char*)LParam;
	char cbuff2[1024];
	char cbuff3[1024];
	cbuff2[1023] = NULL;
	//int length = min(sizeof(cbuff2)-1,(int)WParam);

	try
	{
		strcpy(cbuff2,"00000 OOB: ");
		int length2 = strlen(cbuff2) + (int)WParam;
		strncat(cbuff2,cbuff,(int)WParam);
		cbuff2[length2] = NULL;

		strncpy(cbuff3,cbuff,(int)WParam);
		cbuff3[(int)WParam] = NULL;
		m_cCommStatus = cbuff3;
		SetChecksumDisplay();
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP,NULL,"00000 Internal Error -- Bad LParam Address Received (OOB)");
		if (m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"Internal Error -- Bad LParam Address Received (OOB)");
		CoTaskMemFree((void *)LParam);
		return;
	}

	UpdateSummary(cbuff2);
	strcat(cbuff2,"\r\n");

	WriteToFile(TO_PFM, TYPE_COMP, NULL, cbuff2);

	if (m_bDoDump)
		WriteToFile(TO_DMP,TYPE_DUMP, NULL, cbuff2);

	//delete [] (char*)LParam;
	CoTaskMemFree((void *)LParam);
}


CString CInstrument::GetString(PAGETYPE ePageType, UINT uiDlgItem)
{
	CString cString("");
	switch (ePageType)
	{
	case SUMMARYPAGE:
		if ((uiDlgItem >= IDC_SUMMARY_FIRST) && (uiDlgItem <= IDC_SUMMARY_LAST)) 
		  cString = SummaryStrings[uiDlgItem-IDC_SUMMARY_FIRST];
		break;
///	case GENERAL4PAGE:
///		if ((uiDlgItem >= IDC_GENERAL4_FIRST) && (uiDlgItem <= IDC_GENERAL4_LAST)) 
///		  cString = General4Strings[uiDlgItem-IDC_GENERAL4_FIRST];
///		break;
	case USER4PAGE:
		if ((uiDlgItem >= IDC_USER4PROG_FIRST) && (uiDlgItem <= IDC_USER4PROG_LAST)) 
		  return User4Strings[uiDlgItem-IDC_USER4PROG_FIRST];
		break;
	case DATAPAGE:
		if ((uiDlgItem >= IDC_DATA_FIRST) && (uiDlgItem <= IDC_DATA_LAST)) 
		  cString = DataStrings[uiDlgItem-IDC_DATA_FIRST];
		break;
	case DATAIPAGE:
		if ((uiDlgItem >= IDC_DATAI_FIRST) && (uiDlgItem <= IDC_DATAI_LAST)) 
		  cString = DataIStrings[uiDlgItem-IDC_DATAI_FIRST];
		break;
	case DATA2PAGE:
		if ((uiDlgItem >= IDC_DATA2_FIRST) && (uiDlgItem <= IDC_DATA2_LAST)) 
		  cString = Data2Strings[uiDlgItem-IDC_DATA2_FIRST];
		break;
	case ESBINFOPAGE:
		if ((uiDlgItem >= IDC_ESBINFO_FIRST) && (uiDlgItem <= IDC_ESBINFO_LAST)) 
		  cString = ESBInfoStrings[uiDlgItem-IDC_ESBINFO_FIRST];
		break;
	case PSBINFOPAGE:
		if ((uiDlgItem >= IDC_PSBINFO_FIRST) && (uiDlgItem <= IDC_PSBINFO_LAST)) 
		  cString = PSBInfoStrings[uiDlgItem-IDC_PSBINFO_FIRST];
		break;
	case BMBINFOPAGE:
		if ((uiDlgItem >= IDC_BMBINFO_FIRST) && (uiDlgItem <= IDC_BMBINFO_LAST)) 
		  cString = BMBInfoStrings[uiDlgItem-IDC_BMBINFO_FIRST];
		break;
	}
	return cString;
}


//set string functions are only needed to assist in the Issue Print function
void CInstrument::SetString(PAGETYPE ePageType,UINT uiDlgItem,unsigned char* szText,int iLength)
{
	char szTemp[1024];
	if (iLength < 1024)
	{
		strncpy(szTemp,(const char*)szText,iLength);
		szTemp[iLength] = NULL;
	}
	else
	{
		strncpy(szTemp,(const char*)szText,1023);
		szTemp[1023] = NULL;
	}
	SetString(ePageType,uiDlgItem,szTemp);
}

//set string functions are only needed to assist in the Issue Print function
void CInstrument::SetString(PAGETYPE ePageType,UINT uiDlgItem,CString cString)
{
	switch (ePageType)
	{
	case SUMMARYPAGE:
		if ((uiDlgItem >= IDC_SUMMARY_FIRST) && (uiDlgItem <= IDC_SUMMARY_LAST)) 
		  SummaryStrings[uiDlgItem-IDC_SUMMARY_FIRST] = cString;
		break;
///	case GENERAL4PAGE:
///		if ((uiDlgItem >= IDC_GENERAL4_FIRST) && (uiDlgItem <= IDC_GENERAL4_LAST)) 
///		  General4Strings[uiDlgItem-IDC_GENERAL4_FIRST] = cString;
///		break;
	case USER4PAGE:
		if ((uiDlgItem >= IDC_USER4PROG_FIRST) && (uiDlgItem <= IDC_USER4PROG_LAST)) 
		  User4Strings[uiDlgItem-IDC_USER4PROG_FIRST] = cString;
		break;
	case DATAPAGE:
		if ((uiDlgItem >= IDC_DATA_FIRST) && (uiDlgItem <= IDC_DATA_LAST)) 
		  DataStrings[uiDlgItem-IDC_DATA_FIRST] = cString;
		break;
	case DATAIPAGE:
		if ((uiDlgItem >= IDC_DATAI_FIRST) && (uiDlgItem <= IDC_DATAI_LAST)) 
		  DataIStrings[uiDlgItem-IDC_DATAI_FIRST] = cString;
		break;
	case UGINFOPAGE:
		if ((uiDlgItem >= IDC_UGINFO_FIRST) && (uiDlgItem <= IDC_UGINFO_LAST)) 
		  UGInfoStrings[uiDlgItem-IDC_UGINFO_FIRST] = cString;
		break;
	case ESBINFOPAGE:
		if ((uiDlgItem >= IDC_ESBINFO_FIRST) && (uiDlgItem <= IDC_ESBINFO_LAST)) 
		  ESBInfoStrings[uiDlgItem-IDC_ESBINFO_FIRST] = cString;
		break;
	case PSBINFOPAGE:
		if ((uiDlgItem >= IDC_PSBINFO_FIRST) && (uiDlgItem <= IDC_PSBINFO_LAST)) 
		  PSBInfoStrings[uiDlgItem-IDC_PSBINFO_FIRST] = cString;
		break;
	case BMBINFOPAGE:
		if ((uiDlgItem >= IDC_BMBINFO_FIRST) && (uiDlgItem <= IDC_BMBINFO_LAST)) 
		  BMBInfoStrings[uiDlgItem-IDC_BMBINFO_FIRST] = cString;
		break;
	}
}

void CInstrument::SetColor(PAGETYPE ePageType,UINT uiDlgItem,COLORREF rgbColor)
{
	if ((rgbColor == COLOR_RED) || (rgbColor == COLOR_YELLOW))
	{
//		WriteToFileEx(306,TO_PFM,TYPE_COMP,NULL,"00000 SQUARE icon on\r\n");
		//m_pButton->SetQuestChar(CHICK_CHAR,false);
		//m_pButton->SetQuest(true,true);
		m_pButton->ShowIcon_Square(VARIANT_TRUE);
	}

	switch (ePageType)
	{
	case SUMMARYPAGE:
		if ((uiDlgItem >= IDC_SUMMARY_FIRST) && (uiDlgItem <= IDC_SUMMARY_LAST)) 
		  SummaryStaticColors[uiDlgItem-IDC_SUMMARY_FIRST] = rgbColor;
		break;
///	case GENERAL4PAGE:
///		if ((uiDlgItem >= IDC_GENERAL4_FIRST) && (uiDlgItem <= IDC_GENERAL4_LAST)) 
///		  General4StaticColors[uiDlgItem-IDC_GENERAL4_FIRST] = rgbColor;
///		break;
	case USER4PAGE:
		if ((uiDlgItem >= IDC_USER4PROG_FIRST) && (uiDlgItem <= IDC_USER4PROG_LAST)) 
		  User4StaticColors[uiDlgItem-IDC_USER4PROG_FIRST] = rgbColor;
		break;
	case DATAPAGE:
		if ((uiDlgItem >= IDC_DATA_FIRST) && (uiDlgItem <= IDC_DATA_LAST)) 
		  DataStaticColors[uiDlgItem-IDC_DATA_FIRST] = rgbColor;
		break;
	case DATA2PAGE:
		if ((uiDlgItem >= IDC_DATA2_FIRST) && (uiDlgItem <= IDC_DATA2_LAST)) 
		  Data2StaticColors[uiDlgItem-IDC_DATA2_FIRST] = rgbColor;
		break;
	case DATAIPAGE:
		if ((uiDlgItem >= IDC_DATAI_FIRST) && (uiDlgItem <= IDC_DATAI_LAST)) 
		  DataIStaticColors[uiDlgItem-IDC_DATAI_FIRST] = rgbColor;
		break;
	case ESBINFOPAGE:
		if ((uiDlgItem >= IDC_ESBINFO_FIRST) && (uiDlgItem <= IDC_ESBINFO_LAST)) 
		  ESBInfoStaticColors[uiDlgItem-IDC_ESBINFO_FIRST] = rgbColor;
		break;
	case PSBINFOPAGE:
		if ((uiDlgItem >= IDC_PSBINFO_FIRST) && (uiDlgItem <= IDC_PSBINFO_LAST)) 
		  PSBInfoStaticColors[uiDlgItem-IDC_PSBINFO_FIRST] = rgbColor;
		break;
	case BMBINFOPAGE:
		if ((uiDlgItem >= IDC_BMBINFO_FIRST) && (uiDlgItem <= IDC_BMBINFO_LAST)) 
		  BMBInfoStaticColors[uiDlgItem-IDC_BMBINFO_FIRST] = rgbColor;
		break;
	}
}

void CInstrument::OnReceiveData(WPARAM WParam, LPARAM LParam)
{
	char* cbuff =(char*)LParam;
	char* temp_at;
	char c;
	if (WParam == 0)
		return;
	try
	{
		c = cbuff[0];
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP, NULL,"00000 Internal Error -- Bad LParam Address Received ()");
		if (m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"00000 Internal Error -- Bad LParam Address Received ()");
		CoTaskMemFree((void *)LParam);
		return;
	}
	for (unsigned int index = 0; index < WParam; index ++)
	{
		m_pAccumulator[m_dAt] = (char)(cbuff[index] & 0x7f);

		//m_dAt can get reset externally
		if (m_dAt == 0) 
		{
			m_dBinaryAt = 0;
			m_dBinaryChecksum = 0;
			m_dBinaryState = 0;
		}

		//do we have a "special" record containing multiple binary records
		if ((m_dAt > 1) && (m_pAccumulator[0] == '3') && (m_pAccumulator[1] == '3'))
		{
			//compute checksum
			m_dBinaryChecksum += (BYTE)cbuff[index];
			
			//protect the rest of memory
			if (m_dBinaryAt >= BINARYACCUMULATOR_SIZE)
			{
				m_dBinaryAt = 0;
				m_dBinaryState = 0;
				m_dBinaryChecksum = 0;
			}

			//save the current byte
			m_pBinaryAccumulator[m_dBinaryAt] = (BYTE)cbuff[index];
			m_dAt++;
			switch (m_dBinaryState)
			{
			case 0:
			case 1://get individual record length
				m_usLength = m_pBinaryAccumulator[m_dBinaryAt];
				m_dBinaryState = 2;
				break;
			case 2://get record type
				m_usMessageType = m_pBinaryAccumulator[m_dBinaryAt];

				if	(

					((m_usMessageType == RADACQUIRERECORD_OPCODE		) && (m_usLength == sizeof(RADAcquireRecord	)))||
					((m_usMessageType == ESBACQUIRERECORD_OPCODE		) && (m_usLength == sizeof(ESBAcquireRecord	)))||
					((m_usMessageType == UGCONFIGRECORD_OPCODE			) && (m_usLength == sizeof(uGConfigRecord	)))||
					((m_usMessageType == UGCHCONFIGRECORD_OPCODE		) && (m_usLength == sizeof(uGChConfigRecord	)))||
					((m_usMessageType == ESBCONFIGRECORD_OPCODE			) && (m_usLength == sizeof(ESBConfigRecord	)))||
					((m_usMessageType == PSBCONFIGRECORD_OPCODE			) && (m_usLength == sizeof(PSBConfigRecord	)))||
					((m_usMessageType == BMBCONFIGRECORD_OPCODE			) && (m_usLength == sizeof(BMBConfigRecord	)))||
					((m_usMessageType == UGSTATUSRECORD_OPCODE			) && (m_usLength == sizeof(uGStatusRecord	)))||
					((m_usMessageType == PSBSTATUSRECORD_OPCODE			) && (m_usLength == sizeof(PSBStatusRecord	)))||
					((m_usMessageType == BMBSTATUSRECORD_OPCODE			) && (m_usLength == sizeof(BMBStatusRecord	)))||
					((m_usMessageType == UGINFORECORD_OPCODE			) && (m_usLength == sizeof(uGInfoRecord		)))||
					((m_usMessageType == ESBINFORECORD_OPCODE			) && (m_usLength == sizeof(ESBInfoRecord	)))||
					((m_usMessageType == PSBINFORECORD_OPCODE			) && (m_usLength == sizeof(PSBInfoRecord	)))||
					((m_usMessageType == BMBINFORECORD_OPCODE			) && (m_usLength == sizeof(BMBInfoRecord	)))||
					((m_usMessageType == INFORMATIONRECORD_OPCODE		) && (m_usLength == sizeof(InformationRecord)))||
					((m_usMessageType == TRIGGERCONFIGURERECORD_OPCODE	) && (m_usLength == sizeof(TriggerConfigureRecord)))
					)
				{
					m_dBinaryState = 3;
					m_dRecLengthCount = 0;
				}
				else if (m_usMessageType == 0xbe)
					m_usLength = m_usMessageType;
				else
				{
					m_dBinaryState = 0;
					m_dAt = 0;
					m_dBinaryAt = 0;
					m_dBinaryState = 0;
					m_dBinaryChecksum = 0;
					CString cString;
					cString.Format("00000 Apparent Comm Fault During DUMPBBBM MsgType=%02x MsgLength=%02x\r\n",m_usMessageType,m_usLength);
					WriteToFile(TO_PFM, TYPE_COMP, NULL, (LPSTR)LPCSTR(cString));
					PostToWatch((LPSTR)LPCSTR(cString));
				}
				break;
			case 3://get full record and first checksum byte
				m_dRecLengthCount++;
				//where's the beef
				if ((m_usLength == 0xBE) && (m_usMessageType == 0xEF))
				{
					m_dBinaryState = 4;//done go get checksum
								//compute checksum
					m_dBinaryChecksum -= (BYTE)cbuff[index];

					m_dGivenChecksum = m_pBinaryAccumulator[m_dBinaryAt];
				}
				else if (m_dRecLengthCount >= m_usLength)
					m_dBinaryState = 0;
				break;
			case 4://get checksum byte 2
				//compute checksum
				m_dBinaryChecksum -= (BYTE)cbuff[index];
				m_dBinaryChecksum += '3';
				m_dBinaryChecksum += '3';

				//combine the two bytes of checksum
				m_dGivenChecksum = (m_dGivenChecksum << 8) + m_pBinaryAccumulator[m_dBinaryAt];

				//get ready for a new message
				m_dBinaryState = 0;
				m_pAccumulator[0] = ' ';//cause a failure of the "33" check
				m_pAccumulator[1] = ' ';

				//did we get a good 33 message
				if (m_dBinaryChecksum == m_dGivenChecksum)
				{

					if ((m_dBinaryAt >= 4) &&
						(m_pBinaryAccumulator[m_dBinaryAt-3]==0xBE) &&
						(m_pBinaryAccumulator[m_dBinaryAt-2]==0xEF) 
						)
					{
						bool bDone = false;
						//call each message handler for type
						int dBinaryAt=0;
						while (!bDone)
						{
							short message_type = UNKNOWN_REC;
							short message_length = 0;
							message_type = (short)(m_pBinaryAccumulator[dBinaryAt+1]);
							message_length = (short)(m_pBinaryAccumulator[dBinaryAt]);

							switch(message_type)
							{
							case RADACQUIRERECORD_OPCODE:			
								dBinaryAt += OnBinaryRADAcquireRec(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case ESBACQUIRERECORD_OPCODE:		
								dBinaryAt += OnBinaryESBAcquireRec(&m_pBinaryAccumulator[dBinaryAt]);
								break;
							case UGCONFIGRECORD_OPCODE:
								dBinaryAt += OnBinaryuGConfigRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;
							case UGCHCONFIGRECORD_OPCODE:		
								dBinaryAt += OnBinaryuGChConfigRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case ESBCONFIGRECORD_OPCODE:			
								dBinaryAt += OnBinaryESBConfigRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case PSBCONFIGRECORD_OPCODE:			
								dBinaryAt += OnBinaryPSBConfigRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case BMBCONFIGRECORD_OPCODE:			
								dBinaryAt += OnBinaryBMBConfigRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case UGSTATUSRECORD_OPCODE:			
								dBinaryAt += OnBinaryuGStatusRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case PSBSTATUSRECORD_OPCODE:			
								dBinaryAt += OnBinaryPSBStatusRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case BMBSTATUSRECORD_OPCODE:			
								dBinaryAt += OnBinaryBMBStatusRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case UGINFORECORD_OPCODE:			
								dBinaryAt += OnBinaryuGInfoRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case ESBINFORECORD_OPCODE:			
								dBinaryAt += OnBinaryESBInfoRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case PSBINFORECORD_OPCODE:			
								dBinaryAt += OnBinaryPSBInfoRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case BMBINFORECORD_OPCODE:			
								dBinaryAt += OnBinaryBMBInfoRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case INFORMATIONRECORD_OPCODE:		
								dBinaryAt += OnBinaryInfoRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case TRIGGERCONFIGURERECORD_OPCODE:	
								dBinaryAt += OnBinaryTriggerConfigRecord(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							default:
								{
									CString cString;
									cString.Format("00000 Message Type Unknown: Message Type=%02x\r\n",message_type);
									WriteToFile(TO_PFM, TYPE_COMP, NULL, (LPSTR)LPCSTR(cString));
									PostToWatch((LPSTR)LPCSTR(cString));
									bDone = true;
									m_dBinaryAt = 0;
									m_dAt = 0;
								}
							}
							//account for beef and checksum
							//if we are that close then we are done
							if ((dBinaryAt+4) >= m_dBinaryAt)
							{
								bDone = true;
								m_dBinaryAt = 0;
								m_dAt=0;
							}
						}
					}

					else if ((m_dBinaryAt < 4) &&
						(m_pBinaryAccumulator[m_dBinaryAt-3]==0xBE) &&
						(m_pBinaryAccumulator[m_dBinaryAt-2]==0xEF) 
						)
					{
						m_dBinaryAt = 0;
						m_dAt = 0;
						m_dBinaryState = 0;
						m_dBinaryChecksum = 0;
					}

					PostMessage(IDC_RECEIVE_RESULT,END_DUMP2_RECORD,true);
					m_dNumberOfAcquires = -1;
				}
				else  
				{
					//bad check sum on binary record
					char Record[] = "Binary 33 record\r\n";
		
					COleDateTime Now = COleDateTime::GetCurrentTime();
					CString DateTime = Now.Format("CheckSum: %d/%H:%M:%S\n");
					DateTime += Record;

					m_cGENERAL_CHECKSUM_text = DateTime;
///					m_iGeneral4_Tab |= 0x0001;
					m_iDataI_Tab |= 0x0001;
///					SetColor(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,COLOR_RED);
					SetColor(DATAIPAGE,IDC_DATAI_CHECKSUM,COLOR_RED);
///					SetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,m_cGENERAL_CHECKSUM_text);
					SetString(DATAIPAGE,IDC_DATAI_CHECKSUM,m_cGENERAL_CHECKSUM_text);

					if (m_pPropertySheet)
					{
///						m_pGeneral4StatPage->SetDlgItemText(IDC_GENERAL4_CHECKSUM,(char*)LPCTSTR(DateTime));
///						m_pGeneral4StatPage->SetColor(IDC_GENERAL4_CHECKSUM,COLOR_RED);
///						m_pPropertySheet->m_iGeneral4_Tab |= 0x0001;

						m_pDataIStatus->SetDlgItemText(IDC_DATAI_CHECKSUM,(char*)LPCTSTR(DateTime));
						m_pDataIStatus->SetColor(IDC_DATAI_CHECKSUM,COLOR_RED);
						m_pPropertySheet->m_iDataI_Tab |= 0x0001;
							
///						if (m_pPropertySheet->m_iGeneral4_Tab == 0x0003)
///							m_pPropertySheet->SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
///						else if (m_pPropertySheet->m_iGeneral4_Tab == 0x0002)
///							m_pPropertySheet->SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
///						else if (m_pPropertySheet->m_iGeneral4_Tab == 0x0001)
///							m_pPropertySheet->SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
///						else if (m_pPropertySheet->m_iGeneral4_Tab == 0x0000)
///							m_pPropertySheet->SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);

						if (m_pPropertySheet->m_iDataI_Tab == 0x0003)
							m_pPropertySheet->SetTabColor(IDC_DATAI_TAB,COLOR_YELLOWRED);
						else if (m_pPropertySheet->m_iDataI_Tab == 0x0002)
							m_pPropertySheet->SetTabColor(IDC_DATAI_TAB,COLOR_YELLOW);
						else if (m_pPropertySheet->m_iDataI_Tab == 0x0001)
							m_pPropertySheet->SetTabColor(IDC_DATAI_TAB,COLOR_RED);
						else if (m_pPropertySheet->m_iDataI_Tab == 0x0000)
							m_pPropertySheet->SetTabColor(IDC_DATAI_TAB,COLOR_GREEN);
					}


					WriteToFile(TO_PFM, TYPE_COMP, NULL, "00000 CHECKSUM ERROR\r\n");
					char cBuff[1024];
					strcpy(cBuff,"00000 ");
					strcat(cBuff,Record);
					WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

					if (m_bDoDump)
					{
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, "CHECKSUM ERROR\r\n");
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
					}
					m_dBinaryAt = 0;
					m_dAt=0;
				}
			}	
			m_dBinaryAt++;
			//clean up data pointers // get them ready for another message
			//m_dBinaryAt = 0;
			//m_dAt = 0;
		}
		else if (
			(m_dAt>=1) &&
			((m_pAccumulator[m_dAt]==EOL1)&&(m_pAccumulator[m_dAt-1]==EOL2)) ||
			((m_pAccumulator[m_dAt]==EOL2)&&(m_pAccumulator[m_dAt-1]==EOL1)) 
		   )
		{
			//clean up from backward instrument
			if ((m_pAccumulator[m_dAt]==EOL2)&&(m_pAccumulator[m_dAt-1]==EOL1)) 
			{
				m_pAccumulator[m_dAt]=EOL1;
				m_pAccumulator[m_dAt-1]=EOL2;
			}

			//*test here if length is correct for particular type of message
			//*if it is ok then continue below 
			//*if it is < correct length then assume 
			//*	we got a \r\n or \r\n in binary and continue with the for loop
			//*if it is > correct length then assume
			//*	 we dropped a packet -- send to the handler function and let it
			//*  tell everyone we have an error on this message
			short message_type = UNKNOWN_REC;
			int iStart;
			char* cbuff2;// = (char*)malloc(m_dAt+2);
			iStart = 0;
			while (!isprint(m_pAccumulator[iStart]) && (iStart < m_dAt)) iStart++;

			cbuff2 = (char*)malloc(m_dAt-iStart+2);
			memmove(cbuff2,&m_pAccumulator[iStart],m_dAt-iStart+1);
			cbuff2[m_dAt-iStart+1] = NULL;

			//save for the summary page display			
			UpdateSummary(cbuff2);

			//get the message type
			if (strchr("0123456789ABCDEFabcdef",cbuff2[0]) &&	//protect from collision
				strchr("0123456789ABCDEFabcdef",cbuff2[1]))		//with a text message
				sscanf(cbuff2,"%2hx",&message_type);			//such as "MICROGRAND..."

			//* add new message handlers for new binary messages which are now in ascii
			switch(message_type)
			{

		
			//reorganize these by relative occurrence 

//remove these when done testing
case ACQUIRE_RECORD:			AcquirRec(cbuff2);					break;	//15	remove when done
case MONITOR_ID2_RECORD:		ID2_Record(cbuff2);					break;	//1e	remove when done
case ANALYZE_OK_RECORD:			AnalyzeOk(cbuff2);					break;
case DUMP_OK_RECORD:			DumpOk(cbuff2);						break;

			//text only crackers
			case END_DUMP2_RECORD:			DumpEnd2(cbuff2);					break;	//61
			case ANALYZE_OK3_RECORD:		AnalyzeOk3Record(cbuff2);			break;	//4d
			case MONITOR_ID3_RECORD:		ID3_Record(cbuff2);					break;	//4e
			case DUMP_OK3_RECORD:			DumpOk3_Record(cbuff2);				break;	//4f

			//binary or text -- text crackers build binary record and send to binary handler
			case RADACQUIRERECORD_OPCODE:	OnRADAcquireRecord(cbuff2);			break;
			case ESBACQUIRERECORD_OPCODE:	OnESBAcquireRecord(cbuff2);			break;
			case UGCONFIGRECORD_OPCODE:		OnuGConfigRecord(cbuff2);			break;
			case UGCHCONFIGRECORD_OPCODE:	OnuGChConfigRecord(cbuff2);			break;
			case ESBCONFIGRECORD_OPCODE:	OnESBConfigRecord(cbuff2);			break;
			case PSBCONFIGRECORD_OPCODE:	OnPSBConfigRecord(cbuff2);			break;
			case BMBCONFIGRECORD_OPCODE:	OnBMBConfigRecord(cbuff2);			break;
			case UGSTATUSRECORD_OPCODE:		OnuGStatusRecord(cbuff2);			break;
			case PSBSTATUSRECORD_OPCODE:	OnPSBStatusRecord(cbuff2);			break;
			case BMBSTATUSRECORD_OPCODE:	OnBMBStatusRecord(cbuff2);			break;
			case UGINFORECORD_OPCODE:		OnuGInfoRecord(cbuff2);				break;
			case ESBINFORECORD_OPCODE:		OnESBInfoRecord(cbuff2);			break;
			case PSBINFORECORD_OPCODE:		OnPSBInfoRecord(cbuff2);			break;
			case BMBINFORECORD_OPCODE:		OnBMBInfoRecord(cbuff2);			break;
			case INFORMATIONRECORD_OPCODE:	OnInfoRecord(cbuff2);				break;
			case TRIGGERCONFIGURERECORD_OPCODE:	OnTriggerConfigRecord(cbuff2);	break;

			default:
				if ((temp_at = strstr(cbuff2,"MICROGRAND"))!=NULL)
					Echo_Rec(cbuff2);

//'remove when we get a true uGRAND to work with
else if (strstr(cbuff2,"TIME")!=NULL)
	Echo_Rec(cbuff2);

				else if (strstr(cbuff2,"??")!=NULL)
					Echo_3_Rec(cbuff2);
				else if (strstr(cbuff2,"uG Setup")!=NULL)
					Echo_2_Rec(cbuff2);
				else if (strstr(cbuff2,"µG Setup")!=NULL)
					Echo_2_Rec(cbuff2);
				else if (strlen(cbuff2)==0)
				{
					//do nothing we received a blank line or an echo
				}
				else if (strstr(cbuff2,"uG Monitor")!=NULL)
					Echo_5_Rec(cbuff2);				
				else if (strstr(cbuff2,"µG Monitor")!=NULL)
					Echo_5_Rec(cbuff2);				
			}							
			delete [] cbuff2;
			m_dAt = 0;
		}
		else
		{
			m_dAt++;
			if (m_dAt >= ACCUMULATOR_SIZE) m_dAt = 0;
		}
	}

	//free( (BYTE*)LParam);
	CoTaskMemFree((void *)LParam);
}


void CInstrument::Echo_Rec(char *Record)
{
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	PostMessage(IDC_RECEIVE_RESULT,ECHO_RECORD,CHECKSUM_GOOD);
}

void CInstrument::Echo_5_Rec(char *Record)				
{																	
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,ECHO_5_REC,CHECKSUM_GOOD);	
}																	

void CInstrument::Echo_3_Rec(char *Record)
{
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,ECHO_3_REC,CHECKSUM_GOOD);
}

void CInstrument::Echo_2_Rec(char *Record)
{
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,ECHO_2_REC,CHECKSUM_GOOD);
}

void CInstrument::Reverse(BYTE *sz, int iLength)
{
	BYTE cTemp;
	for (int i = 0; i < (iLength/2); i++)
	{
		cTemp = sz[i];
		sz[i] = sz[iLength - i - 1];
		sz[iLength - i - 1] = cTemp;
	}
}

int CInstrument::AppendCheckSum(char *pBinaryAt)
{
    unsigned short sum;		/* sum of all bytes in checksum */
	sum = 0;
	for (unsigned int i=0; i < strlen(pBinaryAt); i++)
		sum = (unsigned short)(sum + pBinaryAt[i]);
	sum &= 0x00ff;	/* keep only relevant part of checksum */
	char cTemp[4];
	sprintf(cTemp,"%02hx",sum);
	strcat(pBinaryAt,cTemp);
	return sum;
}

//this is the state machine which controls 
//	what commands to send, 
//	when to send them, and
//	what to expect in return.
//It receives a message for each message returned from an instrument
//as well as a "timer" message on intervals selected in "MIC:Configuration"
void  CInstrument::OnReceive(WPARAM WParam, LPARAM LParam)
{
//TRACE("OnReceive::m_dCurrent_State = %d\n",m_dCurrent_State);
	
	//wparam is the type of message and lparam is the checksum good or bad
	//this is true for all messages EXCEPT the battery backed value message
	//in this one case lparam contains the number of bytes in bbm

	//throw away any message which has a bad checksum -- it can't be
	//trusted.  the tick timer will sum up a counter to force a 
	//retransmition if necessary.  The checksum routine will record
	//the message with an error to the proper files

	int i,temp;

	m_pNow = COleDateTime::GetCurrentTime();
	int dCurrentDay = m_pNow.GetDay();
	if (dCurrentDay != m_dPreviousDay)
	{
		m_dPreviousDay = dCurrentDay;
		m_dBreakSentCount = 0;
		m_dTimeOutCount = 0;
		m_dInvalidTimeCount= 0;
		WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,"00000 µGRAND Start new day\r\n");
	}

	//if we couldn't register this instrument 
	//or if disk is full so we are blocking write
	//then lock it up in state 9900 -- don't send anything
	if ((m_bGoodRegister == false) || (m_bBlockWrite == true))
		m_dCurrent_State = 9900;

	if (
		(WParam == IDC_BADYEAR_VALUE)&&
		m_bGoodRegister &&
		((m_dCurrent_State < 8000)||(m_dCurrent_State > 8999))
	   )
	{
		m_bDoingAutoTimeSet = true;
		m_dTickToInquire = m_dInquireInt;
	}

	//if checksum result in LParam then was it bad
	if ((WParam != IDC_BBM_VALUE) && (LParam == CHECKSUM_BAD)) 
	{
		m_bChecksumFail = true;
		return;
	}

	if (WParam == IDC_BBM_VALUE)
		m_iCurrentBBM = LParam;

	//count the time since the last send of any kind
	//      the time since the last inquire2
	//      the time since the last dumpstat (iff != 0)
	if (WParam == TIMER_TICK)
	{
		//bump time increments to next inquire
		m_dTickToInquire++;
		//bump time increments to next status
		if (m_dStatusInt)m_dTickToStatus++;
		//bump time from last command
		m_dTick_Count++;
		if ((m_dRetransmit > m_dCommFail) &&
			(m_dCurrent_State != 9010) &&
			(m_dCurrent_State != 9500))
		{		
			if (m_dCurrent_State == 1210)
			{
				m_dRetransmit = 0;
				m_dTick_Count = 0;
				m_bChecksumFail = false;

				//set next state to "send analyze command"
				m_dCurrent_State = 1300;
			}
			else
			{
				m_dRetransmit = 0;
				m_dTick_Count =	0;
				m_dTimeOutCount++;
				m_dCurrent_State = 9000; //hard fail
			}
		}
	}

	if (
		(WParam == FORCEDATETIME)&&
		(m_bGoodRegister)&&
		((m_dCurrent_State < 8000)||(m_dCurrent_State > 8999))
	   )
	{
		m_dSave_State = m_dCurrent_State;
		m_dCurrent_State = 8000;
	}

	if (m_dCurrent_State < 9000)
		m_bInShutDown = false;
	else
		m_bInShutDown = true;

	if (m_dCurrent_State < 1100)
		m_bInStartUp = true;
	else
		m_bInStartUp = false;

	//this is the virtual instrument state machine
	switch (m_dCurrent_State)
	{
	//start by sending echo,echo,echo
	case 1000:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}

		if (WParam == TIMER_TICK)
		{
			m_pButton->SetErrorIcon_Flag();
			m_pButton->put_FaceColor(m_bInHardFail?COLOR_ERROR:COLOR_WAIT);
			if (m_pPropertySheet)
	 			m_pPropertySheet->m_pCloseButton->SetFaceColor(m_bInHardFail?COLOR_ERROR:COLOR_WAIT,true);
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"\rECHO\r",m_iNode);
			UpdateSummary("ECHO");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ECHO\r\n");
			m_dCurrent_State = 1010;
		}
		break;

	//wait for response 
	case 1010:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//something came back but didn't understand anything that came back
//		if (WParam == ECHO_2_REC)
//		{
//			m_dCurrent_State = 1060;
//			m_dTimeOutCount = 0;
//		}

		//got a "??" back (mini-Ugrand)
		if (WParam == ECHO_3_REC)
		{
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//got the expected "TIME = <space>" record back
		//this must be a MiniUGRAND
		if (WParam == ECHO_RECORD)
		{
			m_dRetransmit = 0;
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dTimeOutCount = 0;
		}

		//got a "uG" response							
		if (WParam == ECHO_5_REC)									
		{
			m_dRetransmit = 0;							
			m_bInHardFail = false;						
			m_dCurrent_State = 1100;//good response		
			m_dTimeOutCount = 0;						
		}												

		//if we didn't get anything back go to transmit second ECHO
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dCurrent_State = 1030;//second transmit
		}
		break;

	case 1015:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		if (WParam == TIMER_TICK)
		{
			m_dTick_Count++ ;
			if (m_dTick_Count > 5)
				m_dCurrent_State = 1000;
		}
		break;

	case 1020:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send second echo command
		if (WParam == TIMER_TICK)
		{
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"\rECHO\r",m_iNode);
			UpdateSummary("ECHO");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ECHO\r\n");
			m_dRetransmit++;
			m_dCurrent_State = 1025;
		}
		break;

	//wait for response 
	case 1025:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//something came back but didn't understand anything that came back
//		if (WParam == ECHO_2_REC)
//		{
//			m_dCurrent_State = 1060;
//			m_dTimeOutCount = 0;
//		}

		//got a "??" back (mini-Ugrand)
		if (WParam == ECHO_3_REC)
		{
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//got the expected "TIME =" record back
		if (WParam == ECHO_RECORD)
		{
			m_dRetransmit = 0;
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dTimeOutCount = 0;
		}

		//got a "uG" response							
		if (WParam == ECHO_5_REC)									
		{												
			m_dRetransmit = 0;							
			m_bInHardFail = false;						
			m_dCurrent_State = 1100;//good response		
			m_dTimeOutCount = 0;						
		}												

		//if we didn't get anything back go to transmit second ECHO
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dCurrent_State = 1030;//third transmit
		}
		break;

	case 1030:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send second echo command
		if (WParam == TIMER_TICK)
		{
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"\rECHO\r",m_iNode);
			UpdateSummary("ECHO");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ECHO\r\n");
			m_dRetransmit++;
			m_dCurrent_State = 1040;
		}
		break;

	case 1040:
		//something came back but didn't understand anything that came back
//		if (WParam == ECHO_2_REC)
//		{
//			m_dCurrent_State = 1060;
//			m_dRetransmit = 0;
//			m_dTimeOutCount = 0;
//			break;
//		}

		if (WParam == ECHO_5_REC)
		{
			m_dRetransmit = 0;							
			m_bInHardFail = false;						
			m_dCurrent_State = 1100;//good response		
			m_dTimeOutCount = 0;						
		}

		//got a "??" back (mini-Ugrand)
		if (WParam == ECHO_3_REC)
		{
			m_dCurrent_State = 1070;
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			break;
		}

		//got the expected "TIME =" record back
		if (WParam == ECHO_RECORD)
		{
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			break;
		}

		//if we didn't get anything back go directly to hard fail
		if (m_dTick_Count > m_dTimeOut)
		{
//			if (m_dBreakSentCount >= m_dCommFail)
			if (m_dRetransmit >= m_dCommFail)
			{
				m_dRetransmit = 0;
				m_dCurrent_State = 9500; //long hard fail
				m_dTick_Count = 0;
				m_dTimeOutCount++;
			}
			else
			{
				m_dCurrent_State = 9000; //hard fail
				m_dTimeOutCount++;
			}
		}
		break;

	//didn't understand what came back so try to reset a grand3
	case 1060:
		//wait here if paused
		if (m_bPause) 
		{
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}

		//send the string which should bring a Ugrand3 back to monitor mode
		COM_Transmit(ISZSTRING,"END\r",m_iNode);
		UpdateSummary("END");
		m_dCurrent_State = 1061;
		break;

	case 1061:
		COM_Transmit(ISZSTRING,"USER\r",m_iNode);
		UpdateSummary("USER");
		m_dCurrent_State = 1062;
		break;

	case 1062:
		COM_Transmit(ISZSTRING,"YES\r",m_iNode);
		UpdateSummary("YES");
		if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "END\r\nUSER\r\nYES\r\n");
		m_dTick_Count = 0;
		m_dCurrent_State = 1000;//restart
		break;

	//got a ?? so we are working on a mini and can get into monitor with a "90"
	case 1070:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
//			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		COM_Transmit(ISZSTRING,"90\r",m_iNode);
		UpdateSummary("90");
		if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "90\r\n");
		m_dTick_Count = 0;
		m_dCurrent_State = 1071;//restart
		break;

	case 1071:
		if (m_bPause)
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		if (m_dTick_Count >= 3)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1000;
		}
		break;

	case 1111:
		if (m_bPause)
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		if (m_dTick_Count >= 2)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
		}
		break;
	
	//got past the echo,echo,response sequence, now need to send a dumplast
	case 1100:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = true;//why was this changed to false
//		m_bInDumpLast = false;
		//send the command
		COM_Transmit(ISZSTRING,"DUMPLAST\r",m_iNode);
		UpdateSummary("DUMPLAST");
		if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPLAST\r\n");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1110;
		break;

	//wait here for a response to the dumplast
	case 1110:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
			WriteToFileEx(292,TO_PFM, TYPE_COMP, NULL,"00000 µGRAND Timeout on receive DUMPLAST response\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL,"µGRAND Timeout on receive DUMPLAST response\r\n");
			m_dTimeOutCount++;
		} 
		//beak out and wait for correct response
		if ((WParam != END_DUMP_RECORD) && (WParam != END_DUMP2_RECORD))
		{
			//
			// Jim Halberg and Peggy Moore 8/22/2002
			//
			if (WParam == ECHO_3_REC) // got "??" ?
			{
				COM_Transmit(ISZSTRING,"90\r",m_iNode);
				UpdateSummary("90");
				if (m_bDoDump)
					WriteToFile(TO_DMP, TYPE_DUMP, NULL, "90\r\n");
				m_dCurrent_State = 1111;
			}
			//
			// end JH and PM changes
			//
			break;
		}
		//got it 
		m_dTimeOutCount = 0;
		//do we need to save these records -- the only way we need to is if
		//we sent a dumpok and then didn't get a dumpok response back and 
		//consequently are resetting this instrument
		temp = m_dAcquireRecordCountX;
		if (
			//we are not in initial startup and
			(m_dPrevBIDTime) &&
			//we did get some records in the dumplast and
			(m_dAcquireRecordCountX) &&
			//those records date time are after the latest recorded
/*need to remove this check*/
			(m_dPrevBIDTime < AcquireRecX[0].uiJulianSeconds)
		   )
		{
			for (i = 0;i<temp;i++)
			{
				WriteToFile(TO_PFM, TYPE_INST, &AcquireTime[i],"00000 Acquire Record from DUMPLAST written in MGD file (15)\r\n");
				if (m_bDoDump)
					WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Acquire Record from DUMPLAST written in MGD file (15)\r\n");
				//issue warning if out of sequence
				if (m_dPrevBIDTime > AcquireRecX[i].uiJulianSeconds)
					WriteToFile(TO_PFM, TYPE_INST, &AcquireTime[i],"00000 Acquire Record Out Of Order (15)\r\n");
				//update the "most recent" acquire record time
				m_dPrevBIDTime = AcquireRecX[i].uiJulianSeconds;

				bool bAccept = true;
#ifdef ALLOWTRIGGERHOLDOFF
				POSITION pos = m_ExclusionList.GetHeadPosition();
				if (pos)
				{
					for (int i=0;i < m_ExclusionList.GetCount();i++)
					{
						EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
						if (m_dPrevBIDTime > sExclusionRec.uiStartTime && 
							m_dPrevBIDTime < sExclusionRec.uiEndTime)
						{
							bAccept = false;
						}
					}
				}
#endif
				if (bAccept)
				{
					//write this record in the bid file
					if (WriteToMGDFileX(
//						&(AcquireTime[i]),
						&(AcquireRecX[i]),						
						AcquireCS[i]) == false)
//						AcquireAUTH[i],
//						AcquireAUTHIgnore[i]) == false)
					{
						UpdateSummary("Could not open MGD file on DUMPLAST");
					}
				}
#ifdef ALLOWTRIGGERHOLDOFF
				else
				{
					CString cMessage;
					cMessage.Format("Excluded Record: %d",m_dPrevBIDTime);
					UpdateSummary((LPSTR)LPCSTR(cMessage));
					PostToWatch((LPSTR)LPCSTR(cMessage));
					WriteToFile(TO_PFM, TYPE_INST, &(AcquireTime[i]),"00000 Acquire Record from DUMPLAST in an EXCLUSION time (15)\r\n");
					if (m_bDoDump)
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Acquire Record from DUMPLAST in an EXCLUSION time (15)\r\n");
				}
#endif
			}
		}
////		else
////		{
////			//save the time of the latest received  
////			//acquire record for authentication
////			if (temp)
////				m_pPropertySheet->m_dLastAcquireTime = m_pPropertySheet->AcquireRec[temp-1].JulianSeconds;
////		}
		if (temp) 
			m_dPreviousBIDTime = AcquireRecX[temp-1].uiJulianSeconds;

		m_dAcquireRecordCountX = 0;
		m_dRecordCountX = 0;

		//don't save any more acquire records -- until next dumpbbm
		m_bInDumpLast = true;

		//so... we are no longer in dump last sequence
////		m_bInDumpLast = false;
		//prepare for the dumpstat command
		m_dRetransmit = 0;
		//go to that state
		m_dCurrent_State = 1200;
		//fall through to do the next transmit iff end dump received

	//transmit the dumpstat command
	case 1200:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send it
		COM_Transmit(ISZSTRING,"DUMPSTAT\r",m_iNode);
		UpdateSummary("DUMPSTAT");
		//update the dmp file
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPSTAT\r\n");
		//count the number of times we've sent this command
		m_dRetransmit++;
		//reset the tick count so that we can time out
		m_dTick_Count = 0;
		//set the next state to wait for a dumpstat response
		m_dCurrent_State = 1210;
		//start with no checksum failures
		m_bChecksumFail = false;
		break;

	//wait for a good response for the dumpstat command
	case 1210:

		//timeout if we've waited here too long
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1200;
			m_dTimeOutCount++;
			break;
		}

		//wait here if we don't have a end dump record
		if ((WParam != END_DUMP_RECORD) && (WParam != END_DUMP2_RECORD))
			break;
		
		//got a good end dump record
		m_dTimeOutCount = 0;
		if (m_bChecksumFail)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1200;
			break;
		}

		//got a good response 
		//so prepare for the next state
		m_dRetransmit = 0;
		m_dTick_Count = 0;
		m_bChecksumFail = false;

		//set next state to "send analyze command"
		m_dCurrent_State = 1300;
		//fall through to immediately transmit the command

	//send the analyze command
	case 1300:
		//wait here if pausing
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send the command
		COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
		UpdateSummary("ANALYZE");
		//update the dmp file
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ANALYZE\r\n");
		//count the number of times analyze has been sent
		m_dRetransmit++;
		//go to the wait for response state
		m_dCurrent_State = 1310;
		//reset check sum fail to see if we get one on the response
		m_bChecksumFail = false;
		break;

	//wait for a response to the analyze command
	case 1310:
		//time out if need be
		if (m_dTick_Count >= m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1300;
			m_dAt = 0;
			m_dTimeOutCount++;
			WriteToFileEx(293,TO_PFM, TYPE_COMP, NULL, "00000 µGRAND Timeout on receive ANALYZE response\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "µGRAND Timeout on receive ANALYZE response\r\n");
		}
		// received a ?? == somehow a mini-Ugrand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//wait here
		if (WParam != ANALYZE_OK_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		m_dTimeOutCount = 0;
		//"normal" operations
		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_bChecksumFail = false;
		m_dCurrent_State = 2000;
		if (m_bDoAnalyzeAfterDT0)
		{
			m_bDoAnalyzeAfterDT0 = false;
			m_dTickToInquire = m_dInquireInt;
////			break;
		}
////		else
////			m_dCurrent_State = 1400;
		//else fall through to next state

	//state 2000 is the "normal" resting state
	case 2000:

		//we can't have an initial initialization time out any longer
		m_bInitInitTimeOut = false;
		m_dHardFailCount = 0;
		m_dBreakSentCount = 0;
		m_dTimeOutCount = 0;
		//if we are paused then don't let state change, just
		//wait for next message to arrive by exiting
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}

		//if we are driving to zero (power fail or copy data) then 
		//we need to empty the immediate buffer on the instrument
		//by sending an analyze command
		if (m_bDoAnalyzeAfterDT0)
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
			UpdateSummary("ANALYZE");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ANALYZE\r\n");

			m_dRetransmit = 0;
			m_dTick_Count = 0;
			m_dCurrent_State = 1310;
			break;
		}

		//if enough time has passed that we need to do
		//another inquire2 then do it
		if (m_dTickToInquire >= m_dInquireInt)  
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			if (m_pPropertySheet)
					m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"INQUIRE2\r",m_iNode);
			UpdateSummary("INQUIRE2");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "INQUIRE2\r\n");

			m_dRetransmit++;
			m_dCurrent_State = 2100; 
			m_dTick_Count = 0;
			break;
			
		} 

		//if StatusInterval is greater than 0 (e.g. we do want to do dumpstats)
		//and enough time has passed then do it.
		if ((m_dStatusInt > 0)&&(m_dTickToStatus >= m_dStatusInt))
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"DUMPSTAT\r",m_iNode);
			UpdateSummary("DUMPSTAT");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPSTAT\r\n");
			m_dRetransmit++;
			m_dCurrent_State = 2300; 
			m_dTick_Count = 0;
			break;
		}

		//if we aren't going to do an inquire2 or a dumpstat then are we 
		//in the process of doing a dumpbbm--if so then continue with another
		//if the bytes in bbm has been driven to 0 then doingbbm will be false
		if (m_bdoingBBM) 
		{
			CString cCommand;
			if (m_bDoBinaryBBM)
				cCommand = "DUMPBBBM\r";
			else
				cCommand = "DUMPBBM\r";
			m_pButton->put_FaceColor(COLOR_WAIT);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,(char*)LPCSTR(cCommand),m_iNode);
			UpdateSummary(m_bDoBinaryBBM?"DUMPBBBM":"DUMPBBM");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, m_bDoBinaryBBM?"DUMPBBBM\r\n":"DUMPBBM\r\n");
			m_dAcquireRecordCountX = 0;
			m_dRecordCountX = 0;
			m_bInDumpLast = false;
			m_dRetransmit++;
			m_dCurrent_State = 2200; 
			m_dTick_Count = 0;
			break;//redundant
		} 
		break;

	case 2100:
		//inquire2 has just been sent 
		//normally two messages will be received in response
		//the first is the bbm value and the second is the id2 record
		//in the case of a mini-Ugrand a "??" could also be receive iff
		//the Ugrand has been taken out of monitor mode
		//all other messages are ignored and the state will eventually 
		//time out

		//have we timed out yet--we can timeout on the message we are waiting for
		//so DON'T break just incase
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 2000;
			m_dTimeOutCount++;

//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(294,TO_PFM, TYPE_COMP, NULL, "00000 µGRAND Timeout on INQUIRE2 command\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "00000 UGRAND Timeout on INQUIRE2 command (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "µGRAND Timeout on INQUIRE2 command\r\n");
			m_dAt = 0;
		}

		if ((WParam == IDC_BADYEAR_VALUE) && m_bAllowAutoTime)
		{
			m_bDoingAutoTimeSet = true;
			m_dTickToInquire = m_dInquireInt;
			m_dSave_State = 2000;
//			m_dCurrent_State = (m_IsMiniUGrand)?8000:8500;
			m_dCurrent_State = 8000;
			break;
		}

		//got the expected bbm value message -- we should receive the 
		//MONITOR_ID2_RECORD message very shortly
		if (WParam == IDC_BBM_VALUE)
		{
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			if ((LParam > (m_bdoingBBM?0:m_dMaxBBM)) ||
				((LParam > 0) && m_bDriveToZero))
			{
				m_bdoingBBM=true;
				PostMessage(IDC_RECEIVE_RESULT,TIMER_TICK,CHECKSUM_GOOD);
			}
			else
			{
				m_bdoingBBM = false;
				if ((m_bDriveToZero) && (!m_bDoAnalyzeAfterDT0))
				{
					m_bPause = true;
					m_bDriveToZero = false;
				}
			}
			m_dTimeOutCount = 0;
		}

		//we got a "??" from a mini-Ugrand so try to reset it
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//received the expected monitor_id2 record so go back to "rest" state
		if ((WParam == MONITOR_ID2_RECORD)||(WParam == MONITOR_ID3_RECORD))
		{
			m_dRetransmit = 0;
			m_dTickToInquire = 0;
			m_dTick_Count = 0;
			m_dCurrent_State = 2000;
			m_dTimeOutCount = 0;
		}

		break;

	case 2200:
		//we just sent a DUMPBBM command
		//time out?  we may time out on the specific message we need so
		//don't break--instead continue to see if we got it
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_bInDumpLast = true;
			m_dCurrent_State = 2000;
			m_dAt = 0;
			m_dAcquireRecordCountX = 0;
			m_dRecordCountX = 0;
			m_dTimeOutCount++;

//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(295,TO_PFM, TYPE_COMP, NULL, 
				m_bDoBinaryBBM?"00000 µGRAND Timeout on DUMPBBBM command\r\n":"00000 µGRAND Timeout on DUMPBBM command\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, 
//				m_bDoBinaryBBM?"00000 UGRAND Timeout on DUMPBBBM command (msg filter on)\r\n":"00000 UGRAND Timeout on DUMPBBM command (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, 
				m_bDoBinaryBBM?"00000 µGRAND Timeout on DUMPBBBM command\r\n":"00000 µGRAND Timeout on DUMPBBM command\r\n");
		}

		// received a ?? == somehow a mini-Ugrand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_bInDumpLast = true;
			m_dAcquireRecordCountX = 0;
			m_dRecordCountX = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			m_dCurrent_State = 1070;

			//caution here -- we'll allow the next if to 
			//break us out
		}

		//wait for end dumpbbm record
		if ((WParam != END_DUMP_RECORD) && (WParam != END_DUMP2_RECORD))
			break;

		//got an end dump record so we can continue with the dumpbbm sequence
		m_dTimeOutCount = 0;
		m_dCurrent_State = 2210;
		m_bInDumpLast = true;
		//fall through iff end_dump_record received
		//this allows "quick" looping on the expected dumpbbm/

	case 2210:
		//just received a end dump record and possibly a bunch of other records
		//one or more of which may have a bad checksum
		//if we are paused in the middle of a dumpbbm sequence this is 
		//NOT a good state to wait in.   

		//if the dumpbbm terminated in the "new" end of dump then 
		//we MUST have same number of acquire records that the 
		//instrument thinks it sent
//		if (((m_dNumberOfAcquires >=0) && (m_dNumberOfAcquires != m_dAcquireRecordCountX)) &&
//			((m_dNumberOfAcquires >=0) && (m_dNumberOfAcquires != (m_dAcquireRecordCountX + m_dRecordCountX))))
		if ((m_dNumberOfAcquires >=0) && (m_dNumberOfAcquires != m_dAcquireRecordCountX))
		{
			m_dCurrent_State = 2000;
			m_bChecksumFail = false;
			m_dAcquireRecordCountX = 0;
			m_dRecordCountX = 0;
			m_bInDumpLast = true;
			WriteToFile(TO_PFM, TYPE_INST, NULL, "00000 INCORRECT NUMBER OF ACQUIRE RECORDS RECEIVED\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "INCORRECT NUMBER OF ACQUIRE RECORDS RECEIVED\r\n");
			m_dRetransmit = 0;
			break;
		}

		//if we received ANY checksum errors during the dumpbbm then
		//we want to break out and ask for a retransmit via another dumpbbm
		//but if the number of checksum fails is at hard-coded value then
		//record the hard fail and continue with the next dump
		if (m_bChecksumFail) 
		{ 
			//count this failure
			m_dChecksumCount++;

			//hard coded if more than 5 checksum failures 
			//then give up and move on
			if (m_dChecksumCount <= 5)
			{
				//go back to get another dumpbbm
				m_dCurrent_State = 2000;
				//reset the checksum failure flag
				m_bChecksumFail = false;
				//we are going to ask for a new dumpbbm so 
				//get rid of all the existing acquire records
				m_dAcquireRecordCountX = 0;
				m_dRecordCountX = 0;
				//indumplast actually means "not" in dumpbbm
				//the next dumpbbm will set this to false
				m_bInDumpLast = true;
				//jump out of state machine
				break;
			}
			else 
			{
				m_bChecksumFail = false;
				FlagComm(1,true);
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, "00000 HARD CHECKSUM ERROR\r\n");
			}
		}

		//if count was <= 5 we break out of case and don't do the 
		//following stuff
		//reset the checksum count in preparation for the next hard checksum error
		m_dChecksumCount = 0;

		//either we got a good BBMDUMP or we had excessive checksum errors
		//in both cases we need to continue
		m_pButton->put_FaceColor(COLOR_WAIT);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"DUMPOK\r",m_iNode);
		UpdateSummary("DUMPOK");
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPOK\r\n");
		m_dRetransmit++;
		m_dCurrent_State = 2220;
		//set these to prime the next state
		m_bdumpOK = false;
		m_bbbmVal = false;
		break;

	case 2220:
		//we just sent a DUMPOK message so we need the dumpok response record
		//we should receive two messages here m_bdumpOK and bbm value

		//got a bbm value so test the bbm value for empty and
		//set the flag to continue the dump and the got bbm value flag
		if ((WParam == IDC_BBM_VALUE))
		{
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			if (LParam > 0)
				m_bdoingBBM = true;
			else
			{
				m_bdoingBBM = false;
				if ((m_bDriveToZero) && (!m_bDoAnalyzeAfterDT0))
				{
					m_bPause = true;
					m_bDriveToZero = false;
				}
			}
			m_bbbmVal = true;
		}

		//don't fail on waiting for dump ok response
		//assume that the instrument got at least one
		//of the dumpok messages -- need the check for
		//bbm value message first so we don't loose it
		if (m_dRetransmit > m_dCommFail)
		{
			WParam = DUMP_OK_RECORD;
			WriteToFile(TO_PFM|TO_CEV, TYPE_COMP, NULL, 
				"00000 µGRAND Multiple Timeout on DUMPOK -- assuming ok\r\n");
			UpdateSummary("DUMPOK -- assuming ok");
////			if (m_bDoDump)
////				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPOK -- assuming ok\r\n");
		}

		//got a good dumpok but need to receive the bbm value before we 
		//continue.  so just set one of the flags
		if (WParam == DUMP_OK_RECORD) 
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			m_bdumpOK = true;
			int iTemp = m_dAcquireRecordCountX;
			bool bDidWriteThemAll = true;
			unsigned int uLastBIDTime;
			uLastBIDTime = 0;
			m_dTimeOutCount = 0;
			for (i = 0;i<iTemp;i++)
			{
				//issue warning if out of sequence
				if (m_dPrevBIDTime && 
					(m_dPrevBIDTime > AcquireRecX[i].uiJulianSeconds))
				{
					WriteToFile(TO_PFM, TYPE_INST, &(AcquireTime[i]),"00000 Acquire Record Out Of Order (15)\r\n");
				}
				//update the "most recent" acquire record time
				//this is local only NOT the PropertySheet value used in WriteToBIDFile
				m_dPrevBIDTime = AcquireRecX[i].uiJulianSeconds;
				uLastBIDTime = m_dPrevBIDTime;

				//record we are attempting to write
				m_iLastRecordWritten = i;

				bool bAccept = true;
#ifdef ALLOWTRIGGERHOLDOFF
				if (!m_ExclusionList.IsEmpty())
				{
					EXCLUSIONREC sExclusionRec = m_ExclusionList.GetHead();
					if (sExclusionRec.uiEndTime < m_dPrevBIDTime)
						m_ExclusionList.RemoveHead();
					POSITION pos = m_ExclusionList.GetHeadPosition();
					if (pos)
					{
						for (int i=0; i < m_ExclusionList.GetCount(); i++)
						{
							EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
							if (m_dPrevBIDTime > sExclusionRec.uiStartTime &&
								m_dPrevBIDTime < sExclusionRec.uiEndTime)
							{
								bAccept = false;
							}
						}
					}
				}
#endif
				if (bAccept)
				{
//TRACE("Write Record I:%d\r\n", i);

					//write this record in the MGD file
					if (WriteToMGDFileX(
							&AcquireRecX[i],
							AcquireCS[i])==false)
					{
						//we failed on this write		
						UpdateSummary("Could not open MGD file");
						m_dCurrent_State = 2221;
						m_pButton->put_FaceColor(COLOR_CYAN);
						if (m_pPropertySheet)
							m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN,true);
						m_dTick_Count = 0;
						m_dTimeOutCount = 0;
						bDidWriteThemAll = false;
						break;
					}
				}
#ifdef ALLOWTRIGGERHOLDOFF
				else
				{
					CString cMessage;
					cMessage.Format("Excluded Record: %d",m_dPrevBIDTime);
					PostToWatch((LPSTR)LPCSTR(cMessage));
					UpdateSummary((LPSTR)LPCSTR(cMessage));
					WriteToFile(TO_PFM, TYPE_INST, &AcquireTime[i],"00000 Acquire Record in an EXCLUSION time (15)\r\n");
					if (m_bDoDump)
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Acquire Record in an EXCLUSION time (15)\r\n");
				}
#endif
			}

			//if we did manage to write them and 
			//we did have some record 15s to write 
			//then reset the propertysheets info
			if (bDidWriteThemAll && (iTemp > 0))
			{
				m_dAcquireRecordCountX = 0;
				m_dPreviousBIDTime = uLastBIDTime;
			}

			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			m_bdumpOK = true;
			iTemp = m_dRecordCountX;
			bDidWriteThemAll = true;
			m_dTimeOutCount = 0;
			for (i = 0;i<iTemp;i++)
			{

				//record we are attempting to write
				m_iLastRecordWritten = i;

				//write this record in the bid file
				if (WriteToSOHFileX(&RecordX[i])==false)
				{
					//we failed on this write		
					UpdateSummary("Could not open SOH file");
					m_dCurrent_State = 2222;
					m_pButton->put_FaceColor(COLOR_CYAN);
					if (m_pPropertySheet)
						m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN,true);
					m_dTick_Count = 0;
					m_dTimeOutCount = 0;
					bDidWriteThemAll = false;
					break;
				}
			}

			if ((m_dCurrent_State == 2221)||(m_dCurrent_State == 2222)) break;
		}

		// received a ?? == somehow a ugrand 
		// went back into setup mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_bInDumpLast = true;
			m_dAcquireRecordCountX = 0;
			m_dTimeOutCount = 0;
			m_dCurrent_State = 1070;
			break;
		}

		//time out if we need to but check this message anyway by
		//not doing a break in the if above
		if ((m_dTick_Count > m_dTimeOut) && (WParam != DUMP_OK_RECORD))
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 2230;//retransmit as necessary
			m_dTimeOutCount++;

			WriteToFileEx(296,TO_PFM, TYPE_COMP, NULL, "00000 µGRAND Timeout on DUMPOK command\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "µGRAND Timeout on DUMPOK command\r\n");
			break;
		}

		//iff we received both messages then go back to the "rest" state
		if (m_bdumpOK && m_bbbmVal) 
		{
			m_dCurrent_State = 2000;
			if (m_bdoingBBM)
				PostMessage(IDC_RECEIVE_RESULT,TIMER_TICK,CHECKSUM_GOOD);
		}

		break;

		//could not open bid file state handler
	case 2221:
		if (m_dTick_Count > m_dTimeOut)
		{
			bool bThisTimeFailed = false;
			m_dTimeOutCount++;
			m_dTick_Count = 0;

			for (int i = m_iLastRecordWritten; i < m_dAcquireRecordCountX; i++)
			{
				//issue warning if out of sequence
				if (m_dPrevBIDTime && 
					(m_dPrevBIDTime > AcquireRecX[i].uiJulianSeconds))
				{
					WriteToFile(TO_PFM, TYPE_INST, &AcquireTime[i],"00000 Acquire Record Out Of Order (15)\r\n");
				}
				//update the "most recent" acquire record time
				m_dPrevBIDTime = AcquireRecX[i].uiJulianSeconds;
				//record we are attempting to write
				m_iLastRecordWritten = i;
				//write this record in the bid file
				if (WriteToMGDFileX(
						&AcquireRecX[i],
						AcquireCS[i])==false)
				{
					//still can't write records so break out of the for loop and try again
					bThisTimeFailed = true;
					UpdateSummary("Still can not open BID file");
					break;
				}
			}
			//if we failed and we've retried as many as possible then
			//bail out of trying and lock up the 
			if ((m_dTimeOutCount >= m_dCommFail) && bThisTimeFailed)
			{
				UpdateSummary("Still can not open MGD file -- giving up");
				m_pButton->SetDiskIcon_Floppy();
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 9800;
			}
			//got a good write this time
			if (!bThisTimeFailed)
			{
				//get ready for the next block of records
				m_dPreviousBIDTime = m_dPrevBIDTime;
				//setup to add new records in the acquirerecord array
				m_dAcquireRecordCountX = 0;
				//set the main button color back to something appropriate
				m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
				//set the property sheet's button color back
				if (m_pPropertySheet)
					m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
				//turn off flash
				m_pButton->put_Flash(VARIANT_FALSE);
				//set the disk icon back to the normal disk
				m_pButton->SetDiskIcon_Floppy();
				//turn off the disk icon
				m_pButton->ShowIcon_Disk(VARIANT_FALSE);
				//go back to the normal state
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 2000;
				//if we still have bbm then post a message to trigger 
				//getting more bbm immediately
				if (m_bdoingBBM)
					PostMessage(IDC_RECEIVE_RESULT,TIMER_TICK,CHECKSUM_GOOD);
			}
		}
		break;

		//could not open soh file state handler
	case 2222:
		if (m_dTick_Count > m_dTimeOut)
		{
			bool bThisTimeFailed = false;
			m_dTimeOutCount++;
			m_dTick_Count = 0;

			for (int i = m_iLastRecordWritten; i < m_dRecordCountX; i++)
			{
				//record we are attempting to write
				m_iLastRecordWritten = i;
				//write this record in the SOH file
				if (WriteToSOHFileX(&RecordX[i])==false)
				{
					//still can't write records so break out of the for loop and try again
					bThisTimeFailed = true;
					UpdateSummary("Still can not open SOH file");
					break;
				}
			}
			//if we failed and we've retried as many as possible then
			//bail out of trying and lock up the 
			if ((m_dTimeOutCount >= m_dCommFail) && bThisTimeFailed)
			{
				UpdateSummary("Still can not open SOH file -- giving up");
				m_pButton->SetDiskIcon_Floppy();
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 9800;
			}
			//got a good write this time
			if (!bThisTimeFailed)
			{
				//setup to add new records in the acquirerecord array
				m_dRecordCountX = 0;
				//set the main button color back to something appropriate
				m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
				//set the property sheet's button color back
				if (m_pPropertySheet)
					m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
				//turn off flash
				m_pButton->put_Flash(VARIANT_FALSE);
				//set the disk icon back to the normal disk
				m_pButton->SetDiskIcon_Floppy();
				//turn off the disk icon
				m_pButton->ShowIcon_Disk(VARIANT_FALSE);
				//go back to the normal state
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 2000;
				//if we still have bbm then post a message to trigger 
				//getting more bbm immediately
				if (m_bdoingBBM)
					PostMessage(IDC_RECEIVE_RESULT,TIMER_TICK,CHECKSUM_GOOD);
			}
		}
		break;

	case 2230:

		//if we received ANY checksum errors during the dumpbbm then
		//we want to break out and ask for a retransmit via another dumpbbm
		//but if the number of checksum fails is at hard-coded value then
		//record the hard fail and continue with the next dump
		if (m_bChecksumFail) 
		{ 
			//count this failure
			m_dChecksumCount++;
			//reset the checksum failure flag
			m_bChecksumFail = false;
			//hard coded if more than 5 checksum failures 
			//then give up and move on
			if (m_dChecksumCount > 5)
			{
				FlagComm(1,true);
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, "00000 HARD CHECKSUM ERROR ON DUMPBBMOK\r\n");
			}
		}

		//either we got a good BBMDUMP or we had excessive checksum errors
		//in both cases we need to continue
		m_pButton->put_FaceColor(COLOR_WAIT);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"DUMPOK\r",m_iNode);
		UpdateSummary("DUMPOK Retransmit");
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPOK Retransmit\r\n");
		m_dRetransmit++;
		m_dCurrent_State = 2220;
		//set these to prime the next state
		m_bdumpOK = false;
		m_bbbmVal = false;
		break;

	case 2300:
		//we just sent a dumpstat command

		//got the end dump record from the dumpstat command
		//so return to the "rest" state
		if ((WParam == END_DUMP_RECORD) || (WParam == END_DUMP2_RECORD))
		{
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			m_dRetransmit = 0;
			m_dTickToStatus = 0;
			m_bInDumpLast = true;
			m_dAcquireRecordCountX = 0;
			m_dCurrent_State = 2000;
			m_dTimeOutCount = 0;
			break;
		}

		// received a ?? == somehow a mini-Ugrand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_bInDumpLast = true;
			m_dAcquireRecordCountX = 0;
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
			break;
		}

		//time out if we need to but check this record just in case
		//if timeout then return to the "rest" state -- next timer tick will trigger retransmit
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dAt = 0;
			m_bInDumpLast = true;
			m_dAcquireRecordCountX = 0;
			m_dTimeOutCount++;

//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(297,TO_PFM, TYPE_COMP, NULL, "00000 µGRAND Timeout on DUMPSTAT command\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "00000 UGRAND Timeout on DUMPSTAT command (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "µGRAND Timeout on DUMPSTAT command\r\n");
			m_dCurrent_State = 2000;
		}
		break;


	case 8000:
		//send the command
		COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
		UpdateSummary("ANALYZE");
		//update the dmp file
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ANALYZE\r\n");
		//count the number of times analyze has been sent
		m_dRetransmit++;
		//go to the wait for response state
		m_dCurrent_State = 8001;
		//reset check sum fail to see if we get one on the response
		m_bChecksumFail = false;
		//reset tick counter
		m_dTick_Count = 0;
		break;

	//wait for a response to the analyze command
	case 8001:
		//time out if need be
		if (m_dTick_Count >= m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 8000;
			m_dAt = 0;
			m_dTimeOutCount++;
//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(298,TO_PFM, TYPE_COMP, NULL, "00000 µGRAND Timeout on receive ANALYZE response\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "00000 UGRAND Timeout on receive ANALYZE response (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "µGRAND Timeout on receive ANALYZE response\r\n");
		}
		// received a ?? == somehow a mini-Ugrand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			///////////////////////////////////////////////////////////////
			//
			// Change by Jim Halbig and Peggy Moore 8/22/2002.
			//
			// Kick the miniUGRAND out of setup mode and back into
			// monitor mode.  Then go transmit ECHO...
			//
			m_dCurrent_State = 1070; //1000;
			//
			//////////////////////////////////////////////////////////////
			m_dTimeOutCount = 0;
		}

//		// received someting we can't understand
//		// so assume it is a Ugrand3 and restart
//		if (WParam == ECHO_2_REC)
//		{
//			m_dTick_Count = 0;
//			m_dRetransmit = 0;
//			m_dCurrent_State = 1000;
//			m_dTimeOutCount = 0;
//		}
//
		//wait here
		if (WParam != ANALYZE_OK_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		m_dTimeOutCount = 0;
		//"normal" operations
		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_bChecksumFail = false;
		m_dCurrent_State = 8009;
		//fall through to send end

	case 8009:
////		if (m_bPause) 
////		{
			KillTimer(TIMER_TICK);
			SetTimer(TIMER_TICK,1000,NULL);
			if (!m_bDoingAutoTimeSet)
				m_dTickToInquire=0;
			if (m_dStatusInt)m_dTickToStatus=0;
			m_dTick_Count=0;
////		}

		m_dCurrent_State = 8010;
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"END\r",m_iNode);
		UpdateSummary("END");
		WriteToFile(TO_PFM|TO_CEV, TYPE_COMP, NULL,"00000 Forcing Instrument to Computer Time\r\n");
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL,"END\r\n");
		break;

	case 8010:
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8020;
			m_dTick_Count = 0;
			m_pToday = COleDateTime::GetCurrentTime();
////			m_pPropertySheet->UpdateSummary((char*)LPCSTR(m_pToday.Format("2f %m %d %y %H %M %S")));
			COM_Transmit(ISZSTRING,(char*)LPCSTR(m_pToday.Format("2f %m %d %y %H %M %S\r")),m_iNode);
			if (m_bDoDump)
				WriteToFile(TO_DMP,TYPE_DUMP,NULL,(char*)LPCSTR(m_pToday.Format("2f %m %d %y %H %M %S\r\n")));
		}
		break;

	case 8020:
		if (WParam == TIMER_TICK)
		{
			COM_Transmit(ISZSTRING,"90\r",m_iNode);
			UpdateSummary("90");
			if (m_bDoDump)
				WriteToFile(TO_DMP,TYPE_DUMP,NULL,"90\r\n");
			m_dTick_Count = 0;
////			if (m_bWasPaused)
////			{
				if (!m_bDoingAutoTimeSet)
					m_dTickToInquire=0;
				if (m_dStatusInt) m_dTickToStatus=0;
				KillTimer(TIMER_TICK);
////			}
			if (!m_bPause || m_bDoingAutoTimeSet)
			{
				SetTimer(TIMER_TICK,m_dCycleInt,NULL);
				m_bIsPaused = false;
			}

			m_dCurrent_State = 8021;//m_dSave_State;
			//no matter what we don't need
			//to do another auto time set
			m_bDoingAutoTimeSet = false;

		}
		break;

	case 8021:
		if (m_dTick_Count >= 2)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = m_dSave_State;
		}
		break;

	case 9000:
		//hard fail trying to communicate with instrument
		m_bInHardFail = true;
		m_bInDumpLast = true;
		m_dAcquireRecordCountX = 0;
		//if initialization and hard fail then send the message
		if (m_bInitInitTimeOut)
		{
//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(300,TO_PFM, TYPE_COMP, NULL, "00000 µGRAND Timeout on µGRAND initialization\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "00000 uGRAND Timeout on uGRAND initialization (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "µGRAND Timeout on µGRAND initialization\r\n");
			m_bInitInitTimeOut = false;
		}

		//start trying to reset things with a long break 
		//if the comm object won't allow a hard break then 
		//the comm object must catch the command and not do it
		//this command turns on break at the comm object
		if (WParam == TIMER_TICK)
		{
			if (m_dLongBreak == 0)
			{
				m_pButton->put_FaceColor(COLOR_ERROR);
				if (m_pPropertySheet)
					m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
				FlagComm(1,true);			
				bool Local = false;
				bool HeldOffLocal = false;
				if (m_dRetransmit < (m_dCommFail-1))
				{
					COM_Transmit(ISZSTRING,LONGBREAKSET"\r",m_iNode);
					UpdateSummary("LONG BREAK SET");

					m_dBreakSentCount++;
////					if (m_bDoDump)
////						WriteToFile(TO_DMP, TYPE_DUMP, NULL,"LONGBREAKSET\r\n");
				}
				else
				{
					if (COM_Transmit(ISZSTRING,LOCALLONGBREAK"\r",m_iNode)==-1)
					{
						UpdateSummary("LOCAL LONG BREAK held off");
						HeldOffLocal = true;
////						if (m_bDoDump)
////							WriteToFile(TO_DMP, TYPE_DUMP, NULL,"LOCAL LONG BREAK held off\r\n");
					}
					else
					{
						UpdateSummary("LOCAL LONG BREAK");
						m_dBreakSentCount++;
////						if (m_bDoDump)
////							WriteToFile(TO_DMP, TYPE_DUMP, NULL,"LOCAL LONG BREAK\r\n");
					}
					Local = true;
				}

//				if (m_dBreakSentCount < g_dMaxRepetition)
//				{
					if (HeldOffLocal)
						WriteToFileEx(301,TO_CEV | TO_PFM, TYPE_COMP, NULL, "00000 Local Break held off (start)\r\n");
					else
						WriteToFileEx(301,TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"00000 Local Break sent (start)\r\n":"00000 Break sent (start)\r\n");
//				}
//				if (m_dBreakSentCount == g_dMaxRepetition)
//				{
//					if (HeldOffLocal)
//						WriteToFileEx(302,TO_CEV | TO_PFM, TYPE_COMP, NULL, "00000 Local Break held off (start) (msg filter on)\r\n");
//					else
//						WriteToFileEx(302,TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"00000 Local Break sent (start) (msg filter on)\r\n":"00000 Break sent (start) (msg filter on)\r\n");
//				}

				if (m_bDoDump)
				{
					if (HeldOffLocal)
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Local LONG BREAK SET held off\r\n");
					else
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, (Local)?"Local LONG BREAK SET\r\n":"LONG BREAK SET\r\n");
				}

			}
			m_dLongBreak++;
		}

		//wait here for a couple of timer ticks to make 
		//sure a long break is minimum 3 seconds
		if ((m_dLongBreak * m_dCycleInt) > 3000) 
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 9010;
		}
		break;

	case 9010:
		//still in hard fail
		m_bInHardFail = true;

		//send the command to clear the long break
		//then wait in this state for a couple timer cycles
		//to give the instrument time to reset after the long break
		if (WParam == TIMER_TICK)
		{
			if (m_dLongBreak)
			{
				m_dTick_Count = 0;
				COM_Transmit(ISZSTRING,LONGBREAKCLR"\r",m_iNode,false);
				UpdateSummary("LONG BREAK CLEAR");
//				if (m_dBreakSentCount < g_dMaxRepetition)
				WriteToFileEx(302,TO_CEV | TO_PFM, TYPE_COMP, NULL, "00000 Break sent (end)\r\n");
//				if (m_dBreakSentCount == g_dMaxRepetition)
//					WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "00000 Break sent (end) (msg filter on)\r\n");
				if (m_bDoDump)
					WriteToFile(TO_DMP, TYPE_DUMP, NULL, "LONG BREAK CLEAR\r\n");
			}
			m_dLongBreak = 0;
			//this is how many timer ticks to wait until we continue with the reset
			bool bLBRExtend = (GetPrivateProfileInt(m_pName,"LBREXTEND",0,m_szIniFile) != 0);
			int iWait;
			if (bLBRExtend) 
				iWait = 60000;
			else
				iWait = 3000;
			if ((m_dTick_Count * m_dCycleInt) > iWait)//wait a minimum 3 or 60 seconds
			{
				m_dTick_Count = 0;
				m_dHardFailCount++;
				m_dCurrent_State = 1000;
			}
		}
		break;

	//long hard fail recovery
	case 9500:							//	900sec/15min
		if ((m_dTick_Count * m_dCycleInt) > m_dSlowReset)//wait 15 minutes before we retry
		{
			m_dTick_Count = 0;
			m_dHardFailCount = 0;
			//m_pButton->SetFlagChar(FLAG_CHAR,true);
			WriteToFileEx(310, TO_PFM, TYPE_COMP, NULL, "00000 Attempting recovery from communications failure.\r\n");
			m_dCurrent_State = 1000;
		}
		//Tickle Tracker every 10 counts
		if (m_dTick_Count % 10 == 0)
		{
			m_pButton->SetErrorIcon_Skull();
			m_pButton->put_FaceColor(COLOR_ERROR);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);

			if (m_dTick_Count <= 10) //first time through have FlagComm write msg 30001 to PFM
				FlagComm(1,true);
			// <<QA>> The redraw is done in the OnTimer function now.
			//else
			//	m_pButton->Redraw(TRUE);  //other times, just trigger network send
		}
		break;

	case 9800:
		m_dCurrent_State = 9810;
		//shut down this instrument
		PauseInstrument(true);	
		//but set it to attempt again in the future
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		m_pButton->put_FaceColor(COLOR_ERROR);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
		WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "00000 WRITE FAILURE on BID file, SUPPORT TERMINATED\r\n");
		break;

	case 9810:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			}
		else
		{
			m_bIsPaused = false;
			PauseInstrument(false);
			m_dTick_Count = INT_MAX-1;
			m_dTimeOutCount = 0;
			WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "00000 Attempt restart from WRITE FAILURE on BID file\r\n");
			m_dCurrent_State = 2221;
		}
		break;

	case 9900:
		if (m_bGoodRegister)
			m_dCurrent_State = 1000;
		else
		{
			m_dCurrent_State = 9900;
			WriteToFileEx(311,TO_PFM,TYPE_COMP,NULL,"00000 Configuration Failure [BOMB icon on]\r\n");
			m_pButton->SetErrorIcon_Bomb();
			m_pButton->ShowIcon_Error(VARIANT_TRUE);
			m_pButton->put_FaceColor(COLOR_ERROR);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
		}
		break;
	default:
		m_dCurrent_State = 1000;
	}
}

int CInstrument::OnBinaryuGConfigRecord(BYTE *pBinaryAt)
{
	//will need a few work areas
	CString cTemp;
	char szTemp1[64];
	char szTemp2[256];
	char szTemp3[16];

	//create a binary record pointer
	uGConfigRecord* uGConfigRec = (uGConfigRecord*)pBinaryAt;

	//make the multi-bytes all little endian
	Reverse((BYTE*)&uGConfigRec->uiJulianSeconds,sizeof(uGConfigRec->uiJulianSeconds));
	Reverse((BYTE*)&uGConfigRec->uiAcqInterval10ths,sizeof(uGConfigRec->uiAcqInterval10ths));
	Reverse((BYTE*)&uGConfigRec->fPHDVSetPoint,sizeof(uGConfigRec->fPHDVSetPoint));
	Reverse((BYTE*)&uGConfigRec->usStatusRecInterv,sizeof(uGConfigRec->usStatusRecInterv));
	Reverse((BYTE*)&uGConfigRec->uiInspectorID,sizeof(uGConfigRec->uiInspectorID));
	Reverse((BYTE*)&uGConfigRec->fCh0KalmanR,sizeof(uGConfigRec->fCh0KalmanR));
	Reverse((BYTE*)&uGConfigRec->fCh1KalmanR,sizeof(uGConfigRec->fCh1KalmanR));
	Reverse((BYTE*)&uGConfigRec->fCh2KalmanR,sizeof(uGConfigRec->fCh2KalmanR));
	Reverse((BYTE*)&uGConfigRec->fCh3KalmanR,sizeof(uGConfigRec->fCh3KalmanR));
	Reverse((BYTE*)&uGConfigRec->fCh0KalmanQ,sizeof(uGConfigRec->fCh0KalmanQ));
	Reverse((BYTE*)&uGConfigRec->fCh1KalmanQ,sizeof(uGConfigRec->fCh1KalmanQ));
	Reverse((BYTE*)&uGConfigRec->fCh2KalmanQ,sizeof(uGConfigRec->fCh2KalmanQ));
	Reverse((BYTE*)&uGConfigRec->fCh3KalmanQ,sizeof(uGConfigRec->fCh3KalmanQ));

	//get a temporary GeneralConfigTextRecord
	uGConfigTextRecord sUGConfigTextRecord;

	//get opcode and place in text record
	sprintf(szTemp1,"%02x",uGConfigRec->bOpcode);
	memcpy(sUGConfigTextRecord.chOpcode,szTemp1,sizeof(sUGConfigTextRecord.chOpcode));

	//get julian time and put it in the binary record
	sprintf(szTemp1,"%10u",uGConfigRec->uiJulianSeconds);
	memcpy(sUGConfigTextRecord.cdJulianSeconds,szTemp1,sizeof(sUGConfigTextRecord.cdJulianSeconds));

	//set the date & time
	CJulianTime cTime;
	cTime.Init(uGConfigRec->uiJulianSeconds);

	bool bStale = true;

	if (m_uiuGConfigRecordTime < cTime.TotalSeconds())	
	{	
		//newer than the last received
		bStale = false;

		//update the time of the latest record
		m_uiuGConfigRecordTime = cTime.TotalSeconds();	

		//set date and time
		SetString(UGINFOPAGE,IDC_UGINFO_DATADATEC,cTime.Date());
		SetString(UGINFOPAGE,IDC_UGINFO_DATATIMEC,cTime.Time());
		cTemp = cTime.Date();
		cTemp += " ";
		cTemp += cTime.Time();
		CurrentSetParam(SET_UGGCR_TIME,"SET_UGGCR_TIME",cTemp,false);
	}

	//set the acquire time
	sprintf(szTemp1,"%10u",uGConfigRec->uiAcqInterval10ths);
	sprintf(szTemp2,"%10.1f",(float)uGConfigRec->uiAcqInterval10ths/10.0);
	memcpy(&sUGConfigTextRecord.cdAcqInterval10ths,szTemp1,sizeof(sUGConfigTextRecord.cdAcqInterval10ths));
	if (!bStale) 
	{
		SetString(UGINFOPAGE,IDC_UGINFO_ACQINTC,szTemp2);
		if (!CurrentSetParam(SET_UGGCR_COUNTTIME,"SET_UGGCR_COUNTTIME",szTemp1,true))
		{
			WriteToFileEx(1,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - COUNT_TIME (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_ACQINTC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set the baud and frame
	sprintf(szTemp1,"%02x",uGConfigRec->bBaudFrame);
	memcpy(sUGConfigTextRecord.cdBaudFrame,szTemp1,sizeof(sUGConfigTextRecord.cdBaudFrame));

	if (!bStale) 
	{
		switch(szTemp1[0]) {
		case '0':
			strcpy(szTemp2 ,"300");
			break;
		case '1':
			strcpy(szTemp2 ,"600");
			break;
		case '2':
			strcpy(szTemp2 ,"1200");
			break;
		case '3':
			strcpy(szTemp2 ,"2400");
			break;
		case '4':
			strcpy(szTemp2 ,"4800");
			break;
		case '5':
			strcpy(szTemp2 ,"9600");
			break;
		case '6':
			strcpy(szTemp2 ,"19200");
			break;
		case '7':
			strcpy(szTemp2 ,"38400");
			break;
		case '8':
			strcpy(szTemp2 ,"57600");
			break;
		case '9':
			strcpy(szTemp2 ,"115200");
			break;
		default:
			strcpy(szTemp2 ,"ERR");
		}
		switch(szTemp1[1]) {
		case '0':
			strcpy(szTemp3,"7:Odd");
			break;
		case '1':
			strcpy(szTemp3,"7:Even");
			break;
		case '2':
			strcpy(szTemp3,"7:None");
			break;
		case '3':
			strcpy(szTemp3,"8:Odd");
			break;
		case '4':
			strcpy(szTemp3,"8:Even");
			break;
		case '5':
			strcpy(szTemp3,"8:None");
			break;
		default:
			strcpy(szTemp3,"ERR");
		}

		if (!CurrentSetParam(SET_UGGCR_BAUDRATE,"SET_UGGCR_BAUDRATE",szTemp2,true))
		{
			WriteToFileEx(2,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BAUD RATE (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_BAUDFRAMEC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
		if (!CurrentSetParam(SET_UGGCR_FRAME,"SET_UGGCR_FRAME",szTemp3,true))
		{
			WriteToFileEx(3,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - FRAME (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_BAUDFRAMEC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
		strcat(szTemp2,":");
		strcat(szTemp2,szTemp3);
		SetString(UGINFOPAGE,IDC_UGINFO_BAUDFRAMEC,szTemp2);
	}

	//set PHD V. Setpoint
	sprintf(szTemp1,"%4.2f",uGConfigRec->fPHDVSetPoint);
	memcpy(sUGConfigTextRecord.cdPHDVSetPoint,szTemp1,sizeof(sUGConfigTextRecord.cdPHDVSetPoint));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_PHDSETPTC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_PHDV,"SET_UGGCR_PHDV",szTemp1,true))
		{
			WriteToFileEx(4,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PHD V. Setpoint (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_PHDSETPTC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set CPU Speed
	sprintf(szTemp1,"%02x",uGConfigRec->bCPUSpeed);
	memcpy(sUGConfigTextRecord.chCPUSpeed,szTemp1,sizeof(sUGConfigTextRecord.chCPUSpeed));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CPUSPEEDC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_OSCSET,"SET_UGGCR_OSCSET",szTemp1,true))
		{
			WriteToFileEx(5,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CPU Speed Select (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_CPUSPEEDC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set Mode Flags
	sprintf(szTemp1,"%08x",uGConfigRec->uiModeFlags);
	memcpy(sUGConfigTextRecord.chModeFlags,szTemp1,sizeof(sUGConfigTextRecord.chModeFlags));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_MODEFLAGC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_MODEFLAGS,"SET_UGGCR_MODEFLAGS",szTemp1,true))
		{
			WriteToFileEx(6,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Mode Flags (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_MODEFLAGC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set Filter Control
	sprintf(szTemp1,"%02x",uGConfigRec->bFilterControl);
	memcpy(sUGConfigTextRecord.chFilterControl,szTemp1,sizeof(sUGConfigTextRecord.chFilterControl));
	if (!bStale)
	{
		szTemp2[0] = NULL;
		if (uGConfigRec->bFilterControl & 0x00000080) strcat(szTemp2,"Kal ");
		if (uGConfigRec->bFilterControl & 0x00000004) strcat(szTemp2,"f/a ");
		if (uGConfigRec->bFilterControl & 0x00000002) strcat(szTemp2,"s ");
		if (uGConfigRec->bFilterControl & 0x00000001) strcat(szTemp2,"e");
		SetString(UGINFOPAGE,IDC_UGINFO_FILTERCNTC,szTemp2);
		if (!CurrentSetParam(SET_UGGCR_FILTERCONT,"SET_UGGCR_FILTERCONT",szTemp1,true))
		{
			WriteToFileEx(7,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Filter Control (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_MODEFLAGC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}
	
	//set immediate buffer size
	sprintf(szTemp1,"%2d",uGConfigRec->bImmBuffSize);
	memcpy(sUGConfigTextRecord.cdImmBuffSize,szTemp1,sizeof(sUGConfigTextRecord.cdImmBuffSize));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_IMMBUFSIZEC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_IMMBUFSIZE,"SET_UGGCR_IMMBUFSIZE",szTemp1,true))
		{
			WriteToFileEx(8,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Immediate Buffer Size (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_IMMBUFSIZEC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set immediate buffer save size
	sprintf(szTemp1,"%2d",uGConfigRec->bImmBuffSaveSize);
	memcpy(sUGConfigTextRecord.cdImmBuffSaveSize,szTemp1,sizeof(sUGConfigTextRecord.cdImmBuffSaveSize));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_IMMBUFSAVEC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_IMMBUFFSAVESIZE,"SET_UGGCR_IMMBUFFSAVESIZE",szTemp1,true))
		{
			WriteToFileEx(9,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Immediate Buffer Save Size (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_IMMBUFSAVEC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set local background size
	sprintf(szTemp1,"%2d",uGConfigRec->bLocalBGSize);
	memcpy(sUGConfigTextRecord.cdLocalBGSize,szTemp1,sizeof(sUGConfigTextRecord.cdLocalBGSize));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_LOCBGSIZEC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_LOCBACKSIZE,"SET_UGGCR_LOCBACKSIZE",szTemp1,true))
		{
			WriteToFileEx(10,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Local Background Size (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_LOCBGSIZEC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set local background end
	sprintf(szTemp1,"%2d",uGConfigRec->bLocalBGEnd);
	memcpy(sUGConfigTextRecord.cdLocalBGEnd,szTemp1,sizeof(sUGConfigTextRecord.cdLocalBGEnd));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_LOCBGENDC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_LOCBACKEND,"SET_UGGCR_LOCBACKEND",szTemp1,true))
		{
			WriteToFileEx(11,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Local Background End (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_LOCBGENDC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set Status record interval
	sprintf(szTemp1,"%5d",uGConfigRec->usStatusRecInterv);
	memcpy(sUGConfigTextRecord.cdStatusRecInterv,szTemp1,sizeof(sUGConfigTextRecord.cdStatusRecInterv));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_STATUSRECINTC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_STATUSINT,"SET_UGGCR_STATUSINT",szTemp1,true))
		{
			WriteToFileEx(12,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Status Record Interval (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_STATUSRECINTC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set time sync state
	sprintf(szTemp1,"%1x",uGConfigRec->bTimeSyncState);
	memcpy(&sUGConfigTextRecord.chTimeSyncState,szTemp1,sizeof(sUGConfigTextRecord.chTimeSyncState));
	if (!bStale)
	{
		szTemp2[0]=NULL;
		if (szTemp1[0]=='1') strcpy(szTemp2,"Daily");
		if (szTemp1[0]=='2') strcpy(szTemp2,"Hourly");
		if (szTemp1[0]=='3') strcpy(szTemp2,"Daily,Hourly");
		SetString(UGINFOPAGE,IDC_UGINFO_TIMESYNCSTC,szTemp2);
		if (!CurrentSetParam(SET_UGGCR_TIMESYNC_STATE,"SET_UGGCR_TIMESYNC_STATE",szTemp1,true))
		{
			WriteToFileEx(13,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Time Sync State (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_TIMESYNCSTC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set hourly time sync time
	sprintf(szTemp1,"%02d%02d",uGConfigRec->bHourlyTimeSyncMM,uGConfigRec->bHourlyTimeSyncSS);
	memcpy(sUGConfigTextRecord.cdHourlyTimeSync,szTemp1,sizeof(sUGConfigTextRecord.cdHourlyTimeSync));
	if (!bStale)
	{
		strncpy(szTemp2,szTemp1,2);
		strcat(szTemp2,":");
		strcat(szTemp2,&szTemp1[2]);
		SetString(UGINFOPAGE,IDC_UGINFO_HOURLYTSC,szTemp2);
		if (!CurrentSetParam(SET_UGGCR_TIMESYNC_HOUR,"SET_UGGCR_TIMESYNC_HOUR",szTemp1,true))
		{
			WriteToFileEx(14,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Hourly Time Sync (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_HOURLYTSC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set daily time sync time
	sprintf(szTemp1,"%02d%02d%02d",uGConfigRec->bDailyTimeSyncHH,uGConfigRec->bDailyTimeSyncMM,uGConfigRec->bDailyTimeSyncSS);
	memcpy(sUGConfigTextRecord.cdDailyTimeSync,szTemp1,sizeof(sUGConfigTextRecord.cdDailyTimeSync));
	if (!bStale)
	{
		sprintf(szTemp2,"%02d:%02d:%02d",uGConfigRec->bDailyTimeSyncHH,uGConfigRec->bDailyTimeSyncMM,uGConfigRec->bDailyTimeSyncSS);
		SetString(UGINFOPAGE,IDC_UGINFO_DAILYTSC,szTemp2);
		if (!CurrentSetParam(SET_UGGCR_TIMESYNC_DAILY,"SET_UGGCR_TIMESYNC_DAILY",szTemp1,true))
		{
			WriteToFileEx(15,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Daily Time Sync (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_DAILYTSC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set time sync lower tolerance
	sprintf(szTemp1,"%2d",uGConfigRec->bTimeSyncLowerTol);
	memcpy(sUGConfigTextRecord.cdTimeSyncLowerTol,szTemp1,sizeof(sUGConfigTextRecord.cdTimeSyncLowerTol));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_TSLOWTOLC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_TIMESYNC_LOWTOL,"SET_UGGCR_TIMESYNC_LOWTOL",szTemp1,true))
		{
			WriteToFileEx(16,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Time Sync Lower Tolerance (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_TSLOWTOLC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set time sync upper tolerance
	sprintf(szTemp1,"%2d",uGConfigRec->bTimeSyncUpperTol);
	memcpy(sUGConfigTextRecord.cdTimeSyncUpperTol,szTemp1,sizeof(sUGConfigTextRecord.cdTimeSyncUpperTol));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_TSUPTOLC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_TIMESYNC_UPPTOL,"SET_UGGCR_TIMESYNC_UPPTOL",szTemp1,true))
		{
			WriteToFileEx(17,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Time Sync Upper Tolerance (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_TSUPTOLC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set inspector ID
	sprintf(szTemp1,"%8d",uGConfigRec->uiInspectorID);
	memcpy(sUGConfigTextRecord.cdInspectorID,szTemp1,sizeof(sUGConfigTextRecord.cdInspectorID));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_INSPECTORC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_INSPECTOR_ID,"SET_UGGCR_INSPECTOR_ID",szTemp1,true))
		{
			WriteToFileEx(18,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Inspector ID (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_INSPECTORC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set unit ID
	sprintf(szTemp1,"%2x",uGConfigRec->bUnitID);
	memcpy(sUGConfigTextRecord.chUnitID,szTemp1,sizeof(sUGConfigTextRecord.chUnitID));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_UNITIDC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_UNIT_ID,"SET_UGGCR_UNIT_ID",szTemp1,true))
		{
			WriteToFileEx(19,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Unit ID (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_UNITIDC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set impedance settings
	sprintf(szTemp1,"%1x",uGConfigRec->bImpedanceSet);
	memcpy(&sUGConfigTextRecord.chImpedanceSet,szTemp1,sizeof(sUGConfigTextRecord.chImpedanceSet));
	if (!bStale)
	{
		szTemp2[0]=NULL;
		if (uGConfigRec->bImpedanceSet & 0x01) strcat(szTemp2,"Ch0:50 "); else strcat(szTemp2,"Ch0:10K ");
		if (uGConfigRec->bImpedanceSet & 0x02) strcat(szTemp2,"Ch1:50 "); else strcat(szTemp2,"Ch1:10K ");
		if (uGConfigRec->bImpedanceSet & 0x04) strcat(szTemp2,"Ch2:50 "); else strcat(szTemp2,"Ch2:10K ");
		if (uGConfigRec->bImpedanceSet & 0x08) strcat(szTemp2,"Ch3:50");  else strcat(szTemp2,"Ch3:10K");
		SetString(UGINFOPAGE,IDC_UGINFO_IMPEDSETSC,szTemp2);
		if (!CurrentSetParam(SET_UGGCR_IMPEDANCE_SET,"SET_UGGCR_IMPEDANCE_SET",szTemp1,true))
		{
			WriteToFileEx(20,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Impedance Settings (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_IMPEDSETSC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set ch 0 kalman r
	sprintf(szTemp1,"%10f",uGConfigRec->fCh0KalmanR);
	memcpy(&sUGConfigTextRecord.cfCh0KalmanR,szTemp1,sizeof(sUGConfigTextRecord.cfCh0KalmanR));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CH0KALRC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_CH0KALR,"SET_UGGCR_CH0KALR",szTemp1,true))
		{
			WriteToFileEx(21,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 0 Kalman R (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_CH0KALRC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set ch 1 kalman r
	sprintf(szTemp1,"%10f",uGConfigRec->fCh1KalmanR);
	memcpy(&sUGConfigTextRecord.cfCh1KalmanR,szTemp1,sizeof(sUGConfigTextRecord.cfCh1KalmanR));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CH1KALRC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_CH1KALR,"SET_UGGCR_CH1KALR",szTemp1,true))
		{
			WriteToFileEx(22,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 1 Kalman R (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_CH1KALRC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set ch 2 kalman r
	sprintf(szTemp1,"%10f",uGConfigRec->fCh2KalmanR);
	memcpy(&sUGConfigTextRecord.cfCh2KalmanR,szTemp1,sizeof(sUGConfigTextRecord.cfCh2KalmanR));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CH2KALRC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_CH2KALR,"SET_UGGCR_CH2KALR",szTemp1,true))
		{
			WriteToFileEx(23,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 2 Kalman R (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_CH2KALRC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set ch 3 kalman r
	sprintf(szTemp1,"%10f",uGConfigRec->fCh3KalmanR);
	memcpy(&sUGConfigTextRecord.cfCh3KalmanR,szTemp1,sizeof(sUGConfigTextRecord.cfCh3KalmanR));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CH3KALRC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_CH3KALR,"SET_UGGCR_CH3KALR",szTemp1,true))
		{
			WriteToFileEx(24,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 3 Kalman R (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_CH3KALRC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set ch 0 kalman Q
	sprintf(szTemp1,"%10f",uGConfigRec->fCh0KalmanQ);
	memcpy(&sUGConfigTextRecord.cfCh0KalmanQ,szTemp1,sizeof(sUGConfigTextRecord.cfCh0KalmanQ));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CH0KALQC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_CH0KALQ,"SET_UGGCR_CH0KALQ",szTemp1,true))
		{
			WriteToFileEx(25,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 0 Kalman Q (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_CH0KALQC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set ch 1 kalman Q
	sprintf(szTemp1,"%10f",uGConfigRec->fCh1KalmanQ);
	memcpy(&sUGConfigTextRecord.cfCh1KalmanQ,szTemp1,sizeof(sUGConfigTextRecord.cfCh1KalmanQ));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CH1KALQC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_CH1KALQ,"SET_UGGCR_CH1KALQ",szTemp1,true))
		{
			WriteToFileEx(26,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 1 Kalman Q (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_CH1KALQC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set ch 2 kalman Q
	sprintf(szTemp1,"%10f",uGConfigRec->fCh2KalmanQ);
	memcpy(&sUGConfigTextRecord.cfCh2KalmanQ,szTemp1,sizeof(sUGConfigTextRecord.cfCh2KalmanQ));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CH2KALQC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_CH2KALQ,"SET_UGGCR_CH2KALQ",szTemp1,true))
		{
			WriteToFileEx(27,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 2 Kalman Q (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_CH2KALQC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//set ch 3 kalman Q
	sprintf(szTemp1,"%10f",uGConfigRec->fCh3KalmanQ);
	memcpy(&sUGConfigTextRecord.cfCh3KalmanQ,szTemp1,sizeof(sUGConfigTextRecord.cfCh3KalmanQ));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CH3KALQC,szTemp1);
		if (!CurrentSetParam(SET_UGGCR_CH3KALQ,"SET_UGGCR_CH3KALQ",szTemp1,true))
		{
			WriteToFileEx(28,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 3 Kalman Q (50)\r\n");
			SetColor(UGINFOPAGE,IDC_UGINFO_CH3KALQC,COLOR_YELLOW);
			m_iUGInfo_Tab |= 0x0002;//turn on yellow
		}
	}

	//got a binary file so don't have a message checksum
	sUGConfigTextRecord.chCheckSum[0] = '-';
	sUGConfigTextRecord.chCheckSum[1] = '-';

	//set the cr and lf in the message
	sUGConfigTextRecord.cCR = '\r';
	sUGConfigTextRecord.cLF = '\n';

	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&sUGConfigTextRecord,sizeof(uGConfigTextRecord));
	cBuff[sizeof(uGConfigTextRecord) + 6] = NULL;

	//send it off to be displayed if possible
	if (!bStale)
	{
		m_cuGConfigRecord = cBuff+6;
		if (m_pPropertySheet)
			m_pPropertySheet->OnuGConfigRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)&uGConfigRec);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,UGCONFIGRECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(uGConfigRecord	);
}

int CInstrument::OnBinaryuGChConfigRecord(BYTE *pBinaryAt)
{
	//some temp buffers
	char szTemp1[32];
	char szJulianTime[16];
	char szUsed[2];
	char szThresh1TD[4];
	char szThresh2TD[4];
	char szCh0Thresh1Value[16];
	char szCh1Thresh1Value[16];
	char szCh2Thresh1Value[16];
	char szCh3Thresh1Value[16];
	char szCh0Thresh2Value[16];
	char szCh1Thresh2Value[16];
	char szCh2Thresh2Value[16];
	char szCh3Thresh2Value[16];
	char szCh0T1EE[4];
	char szCh1T1EE[4];
	char szCh2T1EE[4];
	char szCh3T1EE[4];
	char szCh0T2EE[4];
	char szCh1T2EE[4];
	char szCh2T2EE[4];
	char szCh3T2EE[4];
	char szCh0CSM[8];
	char szCh1CSM[8];
	char szCh2CSM[8];
	char szCh3CSM[8];
	char szCh0CSEE[4];
	char szCh1CSEE[4];
	char szCh2CSEE[4];
	char szCh3CSEE[4];
	char szCh0FilterLim[8];
	char szCh1FilterLim[8];
	char szCh2FilterLim[8];
	char szCh3FilterLim[8];
	char szCh0Hystersis[4];
	char szCh1Hystersis[4];
	char szCh2Hystersis[4];
	char szCh3Hystersis[4];
	CString cTemp;
	unsigned short usTemp;
	unsigned short usTemp1;

	//create a binary record pointer
	uGChConfigRecord* uGChConfigRec = (uGChConfigRecord*)pBinaryAt;

	//make the multipytes all little endian
	Reverse((BYTE*)&uGChConfigRec->fCh0Thresh1Value,sizeof(uGChConfigRec->fCh0Thresh1Value));
	Reverse((BYTE*)&uGChConfigRec->fCh1Thresh1Value,sizeof(uGChConfigRec->fCh1Thresh1Value));
	Reverse((BYTE*)&uGChConfigRec->fCh2Thresh1Value,sizeof(uGChConfigRec->fCh2Thresh1Value));
	Reverse((BYTE*)&uGChConfigRec->fCh3Thresh1Value,sizeof(uGChConfigRec->fCh3Thresh1Value));
	Reverse((BYTE*)&uGChConfigRec->fCh0Thresh2Value,sizeof(uGChConfigRec->fCh0Thresh2Value));
	Reverse((BYTE*)&uGChConfigRec->fCh1Thresh2Value,sizeof(uGChConfigRec->fCh1Thresh2Value));
	Reverse((BYTE*)&uGChConfigRec->fCh2Thresh2Value,sizeof(uGChConfigRec->fCh2Thresh2Value));
	Reverse((BYTE*)&uGChConfigRec->fCh3Thresh2Value,sizeof(uGChConfigRec->fCh3Thresh2Value));
	Reverse((BYTE*)&uGChConfigRec->fCh0ChangSigMultip,sizeof(uGChConfigRec->fCh0ChangSigMultip));
	Reverse((BYTE*)&uGChConfigRec->fCh1ChangSigMultip,sizeof(uGChConfigRec->fCh1ChangSigMultip));
	Reverse((BYTE*)&uGChConfigRec->fCh2ChangSigMultip,sizeof(uGChConfigRec->fCh2ChangSigMultip));
	Reverse((BYTE*)&uGChConfigRec->fCh3ChangSigMultip,sizeof(uGChConfigRec->fCh3ChangSigMultip));
	Reverse((BYTE*)&uGChConfigRec->fCh0FilterLimit,sizeof(uGChConfigRec->fCh0FilterLimit));
	Reverse((BYTE*)&uGChConfigRec->fCh1FilterLimit,sizeof(uGChConfigRec->fCh1FilterLimit));
	Reverse((BYTE*)&uGChConfigRec->fCh2FilterLimit,sizeof(uGChConfigRec->fCh2FilterLimit));
	Reverse((BYTE*)&uGChConfigRec->fCh3FilterLimit,sizeof(uGChConfigRec->fCh3FilterLimit));

	//get a text record
	uGChConfigTextRecord suGChConfigTextRecord;

	//get opcode and place in text record
	sprintf(szTemp1,"%02x",uGChConfigRec->bOpcode);
	memcpy(suGChConfigTextRecord.chOpcode,szTemp1,sizeof(suGChConfigTextRecord.chOpcode));

	//get julian time and put it in the binary record
	sprintf(szJulianTime,"%10u",uGChConfigRec->uiJulianSeconds);
	memcpy(suGChConfigTextRecord.cdJulianSeconds,szJulianTime,sizeof(suGChConfigTextRecord.cdJulianSeconds));

	//get the used for analysis
	sprintf(szUsed,"%1x",uGChConfigRec->bUsedForAnalysis);
	memcpy(&suGChConfigTextRecord.chUsedForAnalysis,szUsed,sizeof(suGChConfigTextRecord.chUsedForAnalysis));

	//get the threshold 1 type and direction
	sprintf(szThresh1TD,"%02x",uGChConfigRec->bThresh1TypeDir);
	memcpy(suGChConfigTextRecord.chThresh1TypeDir,szThresh1TD,sizeof(suGChConfigTextRecord.chThresh1TypeDir));

	//get the threshold 2 type and direction
	sprintf(szThresh2TD,"%02x",uGChConfigRec->bThresh2TypeDir);
	memcpy(suGChConfigTextRecord.chThresh2TypeDir,szThresh2TD,sizeof(suGChConfigTextRecord.chThresh2TypeDir));

	//get the ch 0 threshold 1 value
	sprintf(szCh0Thresh1Value,"%9.1f",uGChConfigRec->fCh0Thresh1Value);
	memcpy(suGChConfigTextRecord.cdfCh0Thresh1Value,szCh0Thresh1Value,sizeof(suGChConfigTextRecord.cdfCh0Thresh1Value));

	//get the ch 1 threshold 1 value
	sprintf(szCh1Thresh1Value,"%9.1f",uGChConfigRec->fCh1Thresh1Value);
	memcpy(suGChConfigTextRecord.cdfCh1Thresh1Value,szCh1Thresh1Value,sizeof(suGChConfigTextRecord.cdfCh1Thresh1Value));

	//get the ch 2 threshold 1 value
	sprintf(szCh2Thresh1Value,"%9.1f",uGChConfigRec->fCh2Thresh1Value);
	memcpy(suGChConfigTextRecord.cdfCh2Thresh1Value,szCh2Thresh1Value,sizeof(suGChConfigTextRecord.cdfCh2Thresh1Value));

	//get the ch 3 threshold 1 value
	sprintf(szCh3Thresh1Value,"%9.1f",uGChConfigRec->fCh3Thresh1Value);
	memcpy(suGChConfigTextRecord.cdfCh3Thresh1Value,szCh3Thresh1Value,sizeof(suGChConfigTextRecord.cdfCh3Thresh1Value));

	//get the ch 0 threshold 2 value
	sprintf(szCh0Thresh2Value,"%9.1f",uGChConfigRec->fCh0Thresh2Value);
	memcpy(suGChConfigTextRecord.cdfCh0Thresh2Value,szCh0Thresh2Value,sizeof(suGChConfigTextRecord.cdfCh0Thresh2Value));

	//get the ch 1 threshold 2 value
	sprintf(szCh1Thresh2Value,"%9.1f",uGChConfigRec->fCh1Thresh2Value);
	memcpy(suGChConfigTextRecord.cdfCh1Thresh2Value,szCh1Thresh2Value,sizeof(suGChConfigTextRecord.cdfCh1Thresh2Value));

	//get the ch 2 threshold 2 value
	sprintf(szCh2Thresh2Value,"%9.1f",uGChConfigRec->fCh2Thresh2Value);
	memcpy(suGChConfigTextRecord.cdfCh2Thresh2Value,szCh2Thresh2Value,sizeof(suGChConfigTextRecord.cdfCh2Thresh2Value));

	//get the ch 3 threshold 2 value
	sprintf(szCh3Thresh2Value,"%9.1f",uGChConfigRec->fCh3Thresh2Value);
	memcpy(suGChConfigTextRecord.cdfCh3Thresh2Value,szCh3Thresh2Value,sizeof(suGChConfigTextRecord.cdfCh3Thresh2Value));

	//get the ch 0 threshold 1 entry exit counts
	sprintf(szCh0T1EE,"%02x",uGChConfigRec->bCh0Thresh1EntryExit);
	memcpy(suGChConfigTextRecord.chCh0Thresh1EntryExit,szCh0T1EE,sizeof(suGChConfigTextRecord.chCh0Thresh1EntryExit));

	//get the ch 1 threshold 1 entry exit counts
	sprintf(szCh1T1EE,"%02x",uGChConfigRec->bCh1Thresh1EntryExit);
	memcpy(suGChConfigTextRecord.chCh1Thresh1EntryExit,szCh1T1EE,sizeof(suGChConfigTextRecord.chCh1Thresh1EntryExit));

	//get the ch 2 threshold 1 entry exit counts
	sprintf(szCh2T1EE,"%02x",uGChConfigRec->bCh2Thresh1EntryExit);
	memcpy(suGChConfigTextRecord.chCh2Thresh1EntryExit,szCh2T1EE,sizeof(suGChConfigTextRecord.chCh2Thresh1EntryExit));

	//get the ch 3 threshold 1 entry exit counts
	sprintf(szCh3T1EE,"%02x",uGChConfigRec->bCh3Thresh1EntryExit);
	memcpy(suGChConfigTextRecord.chCh3Thresh1EntryExit,szCh3T1EE,sizeof(suGChConfigTextRecord.chCh3Thresh1EntryExit));

	//get the ch 0 threshold 2 entry exit counts
	sprintf(szCh0T2EE,"%02x",uGChConfigRec->bCh0Thresh2EntryExit);
	memcpy(suGChConfigTextRecord.chCh0Thresh2EntryExit,szCh0T2EE,sizeof(suGChConfigTextRecord.chCh0Thresh2EntryExit));

	//get the ch 1 threshold 2 entry exit counts
	sprintf(szCh1T2EE,"%02x",uGChConfigRec->bCh1Thresh2EntryExit);
	memcpy(suGChConfigTextRecord.chCh1Thresh2EntryExit,szCh1T2EE,sizeof(suGChConfigTextRecord.chCh1Thresh2EntryExit));

	//get the ch 2 threshold 2 entry exit counts
	sprintf(szCh2T2EE,"%02x",uGChConfigRec->bCh2Thresh2EntryExit);
	memcpy(suGChConfigTextRecord.chCh2Thresh2EntryExit,szCh2T2EE,sizeof(suGChConfigTextRecord.chCh2Thresh2EntryExit));

	//get the ch 3 threshold 2 entry exit counts
	sprintf(szCh3T2EE,"%02x",uGChConfigRec->bCh3Thresh2EntryExit);
	memcpy(suGChConfigTextRecord.chCh3Thresh2EntryExit,szCh3T2EE,sizeof(suGChConfigTextRecord.chCh3Thresh2EntryExit));

	//get the ch 0 changing signal multiplier
	sprintf(szCh0CSM,"%4.1f",uGChConfigRec->fCh0ChangSigMultip);
	memcpy(suGChConfigTextRecord.cdfCh0ChangSigMultip,szCh0CSM,sizeof(suGChConfigTextRecord.cdfCh0ChangSigMultip));

	//get the ch 1 changing signal multiplier
	sprintf(szCh1CSM,"%4.1f",uGChConfigRec->fCh1ChangSigMultip);
	memcpy(suGChConfigTextRecord.cdfCh1ChangSigMultip,szCh1CSM,sizeof(suGChConfigTextRecord.cdfCh1ChangSigMultip));

	//get the ch 2 changing signal multiplier
	sprintf(szCh2CSM,"%4.1f",uGChConfigRec->fCh2ChangSigMultip);
	memcpy(suGChConfigTextRecord.cdfCh2ChangSigMultip,szCh2CSM,sizeof(suGChConfigTextRecord.cdfCh2ChangSigMultip));

	//get the ch 3 changing signal multiplier
	sprintf(szCh3CSM,"%4.1f",uGChConfigRec->fCh3ChangSigMultip);
	memcpy(suGChConfigTextRecord.cdfCh3ChangSigMultip,szCh3CSM,sizeof(suGChConfigTextRecord.cdfCh3ChangSigMultip));

	//get the ch 0 changing signal entry exit
	sprintf(szCh0CSEE,"%02x",uGChConfigRec->bCh0ChangSigEntryExit);
	memcpy(suGChConfigTextRecord.chCh0ChangSigEntryExit,szCh0CSEE,sizeof(suGChConfigTextRecord.chCh0ChangSigEntryExit));

	//get the ch 1 changing signal entry exit
	sprintf(szCh1CSEE,"%02x",uGChConfigRec->bCh1ChangSigEntryExit);
	memcpy(suGChConfigTextRecord.chCh1ChangSigEntryExit,szCh1CSEE,sizeof(suGChConfigTextRecord.chCh1ChangSigEntryExit));

	//get the ch 2 changing signal entry exit
	sprintf(szCh2CSEE,"%02x",uGChConfigRec->bCh2ChangSigEntryExit);
	memcpy(suGChConfigTextRecord.chCh2ChangSigEntryExit,szCh2CSEE,sizeof(suGChConfigTextRecord.chCh2ChangSigEntryExit));

	//get the ch 3 changing signal entry exit
	sprintf(szCh3CSEE,"%02x",uGChConfigRec->bCh3ChangSigEntryExit);
	memcpy(suGChConfigTextRecord.chCh3ChangSigEntryExit,szCh3CSEE,sizeof(suGChConfigTextRecord.chCh3ChangSigEntryExit));

	//get the ch 0 filter limit
	sprintf(szCh0FilterLim,"%7.4f",uGChConfigRec->fCh0FilterLimit);
	memcpy(suGChConfigTextRecord.cdfCh0FilterLimit,szCh0FilterLim,sizeof(suGChConfigTextRecord.cdfCh0FilterLimit));

	//get the ch 1 filter limit
	sprintf(szCh0FilterLim,"%7.4f",uGChConfigRec->fCh0FilterLimit);
	memcpy(suGChConfigTextRecord.cdfCh1FilterLimit,szCh0FilterLim,sizeof(suGChConfigTextRecord.cdfCh1FilterLimit));

	//get the ch 2 filter limit
	sprintf(szCh0FilterLim,"%7.4f",uGChConfigRec->fCh0FilterLimit);
	memcpy(suGChConfigTextRecord.cdfCh2FilterLimit,szCh0FilterLim,sizeof(suGChConfigTextRecord.cdfCh2FilterLimit));

	//get the ch 3 filter limit
	sprintf(szCh0FilterLim,"%7.4f",uGChConfigRec->fCh0FilterLimit);
	memcpy(suGChConfigTextRecord.cdfCh3FilterLimit,szCh0FilterLim,sizeof(suGChConfigTextRecord.cdfCh3FilterLimit));

	//get the ch 0 Hysteresis
	sprintf(szCh0Hystersis,"%7.4f",uGChConfigRec->bCh0Hysteresis);
	memcpy(suGChConfigTextRecord.cdCh0Hysteresis,szCh0Hystersis,sizeof(suGChConfigTextRecord.cdCh0Hysteresis));

	//get the ch 1 Hysteresis
	sprintf(szCh1Hystersis,"%7.4f",uGChConfigRec->bCh1Hysteresis);
	memcpy(suGChConfigTextRecord.cdCh0Hysteresis,szCh1Hystersis,sizeof(suGChConfigTextRecord.cdCh1Hysteresis));

	//get the ch 2 Hysteresis
	sprintf(szCh2Hystersis,"%7.4f",uGChConfigRec->bCh2Hysteresis);
	memcpy(suGChConfigTextRecord.cdCh0Hysteresis,szCh2Hystersis,sizeof(suGChConfigTextRecord.cdCh2Hysteresis));

	//get the ch 3 Hysteresis
	sprintf(szCh3Hystersis,"%7.4f",uGChConfigRec->bCh3Hysteresis);
	memcpy(suGChConfigTextRecord.cdCh0Hysteresis,szCh3Hystersis,sizeof(suGChConfigTextRecord.cdCh3Hysteresis));

	//got a binary file so don't have a message checksum
	suGChConfigTextRecord.chCheckSum[0] = '-';
	suGChConfigTextRecord.chCheckSum[1] = '-';

	//set the cr and lf in the message
	suGChConfigTextRecord.cCR = '\r';
	suGChConfigTextRecord.cLF = '\n';

	//we now have the text image of the record built and each of the text values
	//set the date & time
	CJulianTime cTime;
	cTime.Init(uGChConfigRec->uiJulianSeconds);

	bool bStale = true;

	if (m_uiuGChConfigRecordTime < cTime.TotalSeconds())	
	{	
		//newer than the last received
		bStale = false;

		//update the time of the latest record
		m_uiuGChConfigRecordTime = cTime.TotalSeconds();	

		//save a copy to send to the display dialog
		//when it initially gets displayed
		m_cuGConfigRecord = pBinaryAt;

		//set date and time
		cTemp = cTime.Date();
		cTemp += " ";
		cTemp += cTime.Time();
		SetString(USER4PAGE,IDC_USER4PROG_TIME2,cTemp);
		CurrentSetParam(SET_CCR_TIME,"SET_CCR_TIME",cTemp,false);

		//set used for analysis in each of the columns
		sscanf(szUsed,"%hx",&usTemp);
		if (!CurrentSetParam(SET_CCR_USED,"SET_CCR_USED",szUsed,false))
		{
			GetPrivateProfileString(m_pName,"SET_CCR_USED","",szTemp1,sizeof(szTemp1),m_szIniFile);
			sscanf(szTemp1,"%x",&usTemp1);
			usTemp1 = usTemp1 ^ usTemp; 
			if (usTemp1 & 0x0001)
			{
				SetColor(USER4PAGE,IDC_USER4PROG_CH0USED,COLOR_YELLOW);
				WriteToFileEx(29,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 (NOT)USED (51)\r\n");
			}
			if (usTemp1 & 0x0002)
			{
				SetColor(USER4PAGE,IDC_USER4PROG_CH1USED,COLOR_YELLOW);
				WriteToFileEx(30,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 (NOT)USED (51)\r\n");
			}
			if (usTemp1 & 0x0004)
			{
				SetColor(USER4PAGE,IDC_USER4PROG_CH2USED,COLOR_YELLOW);
				WriteToFileEx(31,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 (NOT)USED (51)\r\n");
			}
			if (usTemp1 & 0x0008) 
			{
				SetColor(USER4PAGE,IDC_USER4PROG_CH3USED,COLOR_YELLOW);
				WriteToFileEx(32,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 (NOT)USED (51)\r\n");
			}
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		SetString(USER4PAGE,IDC_USER4PROG_CH0USED,(usTemp & 0x0001)?"Chan. 0":"Not Used");
		SetString(USER4PAGE,IDC_USER4PROG_CH1USED,(usTemp & 0x0002)?"Chan. 1":"Not Used");
		SetString(USER4PAGE,IDC_USER4PROG_CH2USED,(usTemp & 0x0004)?"Chan. 2":"Not Used");
		SetString(USER4PAGE,IDC_USER4PROG_CH3USED,(usTemp & 0x0008)?"Chan. 3":"Not Used");

		//set threshold 1 type and direction
		sscanf(szThresh1TD,"%hx",&usTemp);
		if (!CurrentSetParam(SET_CCR_T1TYPEDIR,"SET_CCR_T1TYPEDIR",szThresh1TD,true))
		{
			GetPrivateProfileString(m_pName,"SET_CCR_T1TYPEDIR","",szTemp1,sizeof(szTemp1),m_szIniFile);
			sscanf(szTemp1,"%x",&usTemp1);
			usTemp1 = usTemp1 ^ usTemp; 
			if (usTemp1 & 0x00c0)
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C00,COLOR_YELLOW);
				WriteToFileEx(33, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 1 Type or Direction (51)\r\n");
			}
			if (usTemp1 & 0x0030) 
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C10,COLOR_YELLOW);
				WriteToFileEx(34, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 1 Type or Direction (51)\r\n");
			}
			if (usTemp1 & 0x000c) 
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C20,COLOR_YELLOW);
				WriteToFileEx(35, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 1 Type or Direction (51)\r\n");
			}
			if (usTemp1 & 0x0003)
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C30,COLOR_YELLOW);
				WriteToFileEx(36, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 1 Type or Direction (51)\r\n");
			}
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		strcpy(szTemp1,(usTemp & 0x0080)?"Rel/":"Abs/");
		strcat(szTemp1,(usTemp & 0x0040)?"+":"-");
		SetString(USER4PAGE,IDC_USER4PROG_C00,szTemp1);
		strcpy(szTemp1,(usTemp & 0x0020)?"Rel/":"Abs/");
		strcat(szTemp1,(usTemp & 0x0010)?"+":"-");
		SetString(USER4PAGE,IDC_USER4PROG_C10,szTemp1);
		strcpy(szTemp1,(usTemp & 0x0008)?"Rel/":"Abs/");
		strcat(szTemp1,(usTemp & 0x0004)?"+":"-");
		SetString(USER4PAGE,IDC_USER4PROG_C20,szTemp1);
		strcpy(szTemp1,(usTemp & 0x0002)?"Rel/":"Abs/");
		strcat(szTemp1,(usTemp & 0x0001)?"+":"-");
		SetString(USER4PAGE,IDC_USER4PROG_C30,szTemp1);

		//set threshold 2 type and direction
		sscanf(szThresh2TD,"%hx",&usTemp);
		if (!CurrentSetParam(SET_CCR_T2TYPEDIR,"SET_CCR_T2TYPEDIR",szThresh2TD,true))
		{
			GetPrivateProfileString(m_pName,"SET_CCR_T2TYPEDIR","",szTemp1,sizeof(szTemp1),m_szIniFile);
			sscanf(szTemp1,"%x",&usTemp1);
			usTemp1 = usTemp1 ^ usTemp; 
			if (usTemp1 & 0x00c0)
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C03,COLOR_YELLOW);
				WriteToFileEx(37,TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 2 Type or Direction (51)\r\n");
			}
			if (usTemp1 & 0x0030) 
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C13,COLOR_YELLOW);
				WriteToFileEx(38,TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 2 Type or Direction (51)\r\n");
			}
			if (usTemp1 & 0x000c) 
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C23,COLOR_YELLOW);
				WriteToFileEx(39,TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 2 Type or Direction (51)\r\n");
			}
			if (usTemp1 & 0x0003)
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C33,COLOR_YELLOW);
				WriteToFileEx(40,TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 2 Type or Direction (51)\r\n");
			}
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		strcpy(szTemp1,(usTemp & 0x0080)?"Rel/":"Abs/");
		strcat(szTemp1,(usTemp & 0x0040)?"+":"-");
		SetString(USER4PAGE,IDC_USER4PROG_C03,szTemp1);
		strcpy(szTemp1,(usTemp & 0x0020)?"Rel/":"Abs/");
		strcat(szTemp1,(usTemp & 0x0010)?"+":"-");
		SetString(USER4PAGE,IDC_USER4PROG_C13,szTemp1);
		strcpy(szTemp1,(usTemp & 0x0008)?"Rel/":"Abs/");
		strcat(szTemp1,(usTemp & 0x0004)?"+":"-");
		SetString(USER4PAGE,IDC_USER4PROG_C23,szTemp1);
		strcpy(szTemp1,(usTemp & 0x0002)?"Rel/":"Abs/");
		strcat(szTemp1,(usTemp & 0x0001)?"+":"-");
		SetString(USER4PAGE,IDC_USER4PROG_C33,szTemp1);

		//ch 0 thresh 1 value
		if (!CurrentSetParam(SET_CCR_C0T1VALUE,"SET_CCR_C0T1VALUE",szCh0Thresh1Value,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C01,COLOR_YELLOW);
			WriteToFileEx(41, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 1 Value (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C01,szCh0Thresh1Value);

		//ch 1 thresh 1 value
		if (!CurrentSetParam(SET_CCR_C1T1VALUE,"SET_CCR_C1T1VALUE",szCh1Thresh1Value,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C11,COLOR_YELLOW);
			WriteToFileEx(42, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 1 Value (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C11,szCh1Thresh1Value);

		//ch 2 thres 1 value
		if (!CurrentSetParam(SET_CCR_C2T1VALUE,"SET_CCR_C2T1VALUE",szCh2Thresh1Value,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C21,COLOR_YELLOW);
			WriteToFileEx(43, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 1 Value (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C21,szCh2Thresh1Value);

		//ch 3 thres 1 value
		if (!CurrentSetParam(SET_CCR_C3T1VALUE,"SET_CCR_C3T1VALUE",szCh3Thresh1Value,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C31,COLOR_YELLOW);
			WriteToFileEx(44, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 1 Value (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C31,szCh3Thresh1Value);

		//ch 0 thresh 2 value
		if (!CurrentSetParam(SET_CCR_C0T2VALUE,"SET_CCR_C0T2VALUE",szCh0Thresh2Value,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C04,COLOR_YELLOW);
			WriteToFileEx(45, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 2 Value (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C04,szCh0Thresh2Value);

		//ch 1 thresh 2 value
		if (!CurrentSetParam(SET_CCR_C1T2VALUE,"SET_CCR_C1T2VALUE",szCh1Thresh2Value,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C14,COLOR_YELLOW);
			WriteToFileEx(46, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 2 Value (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C14,szCh1Thresh2Value);

		//ch 2 thres 2 value
		if (!CurrentSetParam(SET_CCR_C2T2VALUE,"SET_CCR_C2T2VALUE",szCh2Thresh2Value,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C24,COLOR_YELLOW);
			WriteToFileEx(47, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 2 Value (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C24,szCh2Thresh2Value);

		//ch 3 thres 2 value
		if (!CurrentSetParam(SET_CCR_C3T2VALUE,"SET_CCR_C3T2VALUE",szCh3Thresh2Value,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C34,COLOR_YELLOW);
			WriteToFileEx(48, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 2 Value (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C34,szCh3Thresh2Value);

		//ch 0 thresh 1 entry/exit
		if (!CurrentSetParam(SET_CCR_C0T1EE,"SET_CCR_C0T1EE",szCh0T1EE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C02,COLOR_YELLOW);
			WriteToFileEx(49, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 1 Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh0T1EE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C02,szTemp1);

		//ch 1 thresh 1 entry/exit
		if (!CurrentSetParam(SET_CCR_C1T1EE,"SET_CCR_C1T1EE",szCh1T1EE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C12,COLOR_YELLOW);
			WriteToFileEx(50, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 1 Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh1T1EE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C12,szTemp1);

		//ch 2 thresh 1 entry/exit
		if (!CurrentSetParam(SET_CCR_C2T1EE,"SET_CCR_C2T1EE",szCh2T1EE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C22,COLOR_YELLOW);
			WriteToFileEx(51, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 1 Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh2T1EE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C22,szTemp1);

		//ch 3 thresh 1 entry/exit
		if (!CurrentSetParam(SET_CCR_C3T1EE,"SET_CCR_C3T1EE",szCh3T1EE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C32,COLOR_YELLOW);
			WriteToFileEx(52, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 1 Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh3T1EE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C32,szTemp1);

		//ch 0 thresh 2 entry/exit
		if (!CurrentSetParam(SET_CCR_C0T2EE,"SET_CCR_C0T2EE",szCh0T2EE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C05,COLOR_YELLOW);
			WriteToFileEx(53, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 2 Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh0T2EE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C05,szTemp1);

		//ch 1 thresh 2 entry/exit
		if (!CurrentSetParam(SET_CCR_C1T2EE,"SET_CCR_C1T2EE",szCh1T2EE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C15,COLOR_YELLOW);
			WriteToFileEx(54, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 2 Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh1T2EE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C15,szTemp1);

		//ch 2 thresh 2 entry/exit
		if (!CurrentSetParam(SET_CCR_C2T2EE,"SET_CCR_C2T2EE",szCh2T2EE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C25,COLOR_YELLOW);
			WriteToFileEx(55, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 2 Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh2T2EE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C25,szTemp1);

		//ch 3 thresh 2 entry/exit
		if (!CurrentSetParam(SET_CCR_C3T2EE,"SET_CCR_C3T2EE",szCh3T1EE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C35,COLOR_YELLOW);
			WriteToFileEx(56, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 2 Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh3T1EE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C35,szTemp1);

		//ch 0 changing signal multiplier
		if (!CurrentSetParam(SET_CCR_C0CCM,"SET_CCR_C0CCM",szCh0CSM,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C06,COLOR_YELLOW);
			WriteToFileEx(57, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Changing Signal multiplier (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C06,szCh0CSM);

		//ch 1 changing signal multiplier
		if (!CurrentSetParam(SET_CCR_C1CCM,"SET_CCR_C1CCM",szCh1CSM,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C16,COLOR_YELLOW);
			WriteToFileEx(58, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Changing Signal multiplier (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C16,szCh1CSM);

		//ch 2 changing signal multiplier
		if (!CurrentSetParam(SET_CCR_C2CCM,"SET_CCR_C2CCM",szCh2CSM,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C26,COLOR_YELLOW);
			WriteToFileEx(59, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Changing Signal multiplier (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C26,szCh2CSM);

		//ch 3 changing signal multiplier
		if (!CurrentSetParam(SET_CCR_C3CCM,"SET_CCR_C3CCM",szCh3CSM,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C36,COLOR_YELLOW);
			WriteToFileEx(60, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Changing Signal multiplier (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C36,szCh3CSM);

		//ch 0 changing signal entry exit
		if (!CurrentSetParam(SET_CCR_C0CSEE,"SET_CCR_C0CSEE",szCh0CSEE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C07,COLOR_YELLOW);
			WriteToFileEx(61, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Changing Signal Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh0CSEE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C07,szTemp1);

		//ch 1 changing signal entry exit
		if (!CurrentSetParam(SET_CCR_C0CSEE,"SET_CCR_C1CSEE",szCh1CSEE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C17,COLOR_YELLOW);
			WriteToFileEx(62, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Changing Signal Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh1CSEE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C17,szTemp1);

		//ch 2 changing signal entry exit
		if (!CurrentSetParam(SET_CCR_C2CSEE,"SET_CCR_C2CSEE",szCh2CSEE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C27,COLOR_YELLOW);
			WriteToFileEx(63, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Changing Signal Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh2CSEE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C27,szTemp1);

		//ch 3 changing signal entry exit
		if (!CurrentSetParam(SET_CCR_C3CSEE,"SET_CCR_C3CSEE",szCh3CSEE,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C37,COLOR_YELLOW);
			WriteToFileEx(64, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Changing Signal Entry/Exit(51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		sscanf(szCh3CSEE,"%x",&usTemp);
		sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
		SetString(USER4PAGE,IDC_USER4PROG_C37,szTemp1);

		//ch 0 filter limit
		if (!CurrentSetParam(SET_CCR_FILTERLIM0,"SET_CCR_FILTERLIM0",szCh0FilterLim,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C08,COLOR_YELLOW);
			WriteToFileEx(65, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Filter Limit (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C08,szCh0FilterLim);

		//ch 1 filter limit
		if (!CurrentSetParam(SET_CCR_FILTERLIM0,"SET_CCR_FILTERLIM1",szCh1FilterLim,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C18,COLOR_YELLOW);
			WriteToFileEx(66, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Filter Limit (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C18,szCh1FilterLim);

		//ch 2 filter limit
		if (!CurrentSetParam(SET_CCR_FILTERLIM2,"SET_CCR_FILTERLIM2",szCh2FilterLim,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C28,COLOR_YELLOW);
			WriteToFileEx(67, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Filter Limit (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C28,szCh2FilterLim);

		//ch 3 filter limit
		if (!CurrentSetParam(SET_CCR_FILTERLIM3,"SET_CCR_FILTERLIM3",szCh3FilterLim,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C38,COLOR_YELLOW);
			WriteToFileEx(68, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Filter Limit (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C38,szCh3FilterLim);

		//ch 0 hysteresis
		if (!CurrentSetParam(SET_CCR_HYSTER0,"SET_CCR_HYSTER0",szCh0Hystersis,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C09,COLOR_YELLOW);
			WriteToFileEx(69, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Hysteresis (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C09,szCh0Hystersis);

		//ch 1 hysteresis
		if (!CurrentSetParam(SET_CCR_HYSTER1,"SET_CCR_HYSTER1",szCh1Hystersis,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C19,COLOR_YELLOW);
			WriteToFileEx(70, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Hysteresis (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C19,szCh1Hystersis);

		//ch 2 hysteresis
		if (!CurrentSetParam(SET_CCR_HYSTER2,"SET_CCR_HYSTER2",szCh2Hystersis,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C29,COLOR_YELLOW);
			WriteToFileEx(71, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Hysteresis (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C29,szCh2Hystersis);

		//ch 0 hysteresis
		if (!CurrentSetParam(SET_CCR_HYSTER3,"SET_CCR_HYSTER3",szCh3Hystersis,true))
		{
			SetColor(USER4PAGE,IDC_USER4PROG_C39,COLOR_YELLOW);
			WriteToFileEx(72, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Hysteresis (51)\r\n");
			m_iUser4Prog_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		SetString(USER4PAGE,IDC_USER4PROG_C39,szCh3Hystersis);

	}

	//send it off to be displayed if possible
	if (m_pPropertySheet && !bStale)
		m_pPropertySheet->OnuGChConfigRecord((char*)&suGChConfigTextRecord);

	char cBuff[sizeof(uGChConfigTextRecord)+7];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&suGChConfigTextRecord,sizeof(suGChConfigTextRecord));
	cBuff[sizeof(suGChConfigTextRecord) + 6] = NULL;

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)&uGChConfigRec);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,UGCHCONFIGRECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(uGChConfigRecord	);
}

int CInstrument::OnBinaryESBConfigRecord(BYTE *pBinaryAt)
{
	//some temp buffers
	char szTemp1[32];
	char szTempModeFlags[8];
	char szNomAcqInterv[8];
	char szAnomAcqInterv[8];
	char szLPAcqInterv[8];
	char szTempSN[16];
	CString cTemp;

	//create a binary record pointer
	ESBConfigRecord* ESBConfigRec = (ESBConfigRecord*)pBinaryAt;

	//make the multibytes all little endian
	Reverse((BYTE*)&ESBConfigRec->uiJulianSeconds,sizeof(ESBConfigRec->uiJulianSeconds));
	Reverse((BYTE*)&ESBConfigRec->chModeFlags,sizeof(ESBConfigRec->chModeFlags));
	Reverse((BYTE*)&ESBConfigRec->usNominalAcqInterval,sizeof(ESBConfigRec->usNominalAcqInterval));
	Reverse((BYTE*)&ESBConfigRec->usAnomalousAcqInterval,sizeof(ESBConfigRec->usAnomalousAcqInterval));
	Reverse((BYTE*)&ESBConfigRec->usLowPowerAcqInterval,sizeof(ESBConfigRec->usLowPowerAcqInterval));
	//Reverse((BYTE*)&ESBConfigRec->bSerialNumber,sizeof(ESBConfigRec->bSerialNumber));

	//get a text record
	ESBConfigTextRecord sESBConfigTextRecord;

	//get opcode and place in text record
	sprintf(szTemp1,"%02x",ESBConfigRec->bOpcode);
	memcpy(sESBConfigTextRecord.chOpcode,szTemp1,sizeof(sESBConfigTextRecord.chOpcode));

	//get julian time and put it in the binary record
	sprintf(szTemp1,"%10u",ESBConfigRec->uiJulianSeconds);
	memcpy(sESBConfigTextRecord.cdJulianSeconds,szTemp1,sizeof(sESBConfigTextRecord.cdJulianSeconds));

	//get mode bytes
	sprintf(szTempModeFlags,"%04x",ESBConfigRec->chModeFlags);
	memcpy(sESBConfigTextRecord.chModeFlags,szTempModeFlags,sizeof(sESBConfigTextRecord.chModeFlags));

	//get nominal acquisition interval
	sprintf(szNomAcqInterv,"%5hu",ESBConfigRec->usAnomalousAcqInterval);
	memcpy(sESBConfigTextRecord.cdNominalAcqInterval,szNomAcqInterv,sizeof(sESBConfigTextRecord.cdNominalAcqInterval));

	//get anomalous acquisition interval
	sprintf(szAnomAcqInterv,"%5hu",ESBConfigRec->usAnomalousAcqInterval);
	memcpy(&sESBConfigTextRecord.cdAnomalousAcqInterval,szAnomAcqInterv,sizeof(sESBConfigTextRecord.cdAnomalousAcqInterval));

	//get low power acquisition interval
	sprintf(szLPAcqInterv,"%5hu",ESBConfigRec->usLowPowerAcqInterval);
	memcpy(&sESBConfigTextRecord.cdLowPowerAcqInterval,szLPAcqInterv,sizeof(sESBConfigTextRecord.cdLowPowerAcqInterval));

	//got a binary file so don't have a message checksum
	sESBConfigTextRecord.chCheckSum[0] = '-';
	sESBConfigTextRecord.chCheckSum[1] = '-';

	//set the cr and lf in the message
	sESBConfigTextRecord.cCR = '\r';
	sESBConfigTextRecord.cLF = '\n';

	//build the serial number and use it to find which column to put this data in
	sprintf(szTempSN,"%02x%02x%02x%02x%02x%02x",
		ESBConfigRec->bSerialNumber[0],
		ESBConfigRec->bSerialNumber[1],
		ESBConfigRec->bSerialNumber[2],
		ESBConfigRec->bSerialNumber[3],
		ESBConfigRec->bSerialNumber[4],
		ESBConfigRec->bSerialNumber[5]);
	memcpy(sESBConfigTextRecord.chSerialNumber,szTempSN,sizeof(sESBConfigTextRecord.chSerialNumber));

	//text version should be complete at this point

	//set the date & time
	CJulianTime cTime;
	cTime.Init(ESBConfigRec->uiJulianSeconds);

	bool bStale = true;

	int iColumn = 0;
	if      (strncmp((char*)&sESBConfigTextRecord.chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1),sizeof(sESBConfigTextRecord.chSerialNumber)) == 0) iColumn = 1;
	else if (strncmp((char*)&sESBConfigTextRecord.chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2),sizeof(sESBConfigTextRecord.chSerialNumber)) == 0) iColumn = 2;
	else if (strncmp((char*)&sESBConfigTextRecord.chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3),sizeof(sESBConfigTextRecord.chSerialNumber)) == 0) iColumn = 3;
	else if (strncmp((char*)&sESBConfigTextRecord.chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4),sizeof(sESBConfigTextRecord.chSerialNumber)) == 0) iColumn = 4;
	else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1))==0) iColumn = 5;
	else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2))==0) iColumn = 6;
	else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3))==0) iColumn = 7;
	else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4))==0) iColumn = 8;

	if (m_uiESBConfigRecordTime < cTime.TotalSeconds())
	{
		//newer than the last received
		bStale = false;

		//update the time of the latest record
		m_uiESBConfigRecordTime = cTime.TotalSeconds();	

		//set date and time
		switch (iColumn) {
		case 5:
			SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1,sESBConfigTextRecord.chSerialNumber,sizeof(sESBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_ESBIR_SERIALNUM1,"SET_ESBIR_SERIALNUM1",szTempSN,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL1,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
		//fall through to process rest
		case 1:
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEC1,cTime.Date());
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEC1,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_ESBCR_TIME1,"SET_ESBCR_TIME1",cTemp,false);

			if (!CurrentSetParam(SET_ESBCR_MODEFLAGS1,"SET_ESBCR_MODEFLAGS1",szTempModeFlags,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_MODEFLAGS1,COLOR_YELLOW);
				WriteToFileEx(73, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Mode Flags (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_NORMACQINTERV1,"SET_ESBCR_NORMACQINTERV1",szNomAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_NOMACQINT1,COLOR_YELLOW);
				WriteToFileEx(74, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Nominal Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_ANOMACQINTERV1,"SET_ESBCR_ANOMACQINTERV1",szAnomAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_ANOMACQINT1,COLOR_YELLOW);
				WriteToFileEx(75, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Anomalous Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_LOPWACQINTERV1,"SET_ESBCR_LOPWACQINTERV1",szLPAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_LPACQINT1,COLOR_YELLOW);
				WriteToFileEx(76, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Low Power Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}
			break;

		case 6:
			SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2,sESBConfigTextRecord.chSerialNumber,sizeof(sESBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_ESBIR_SERIALNUM2,"SET_ESBIR_SERIALNUM2",szTempSN,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL2,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
		//fall through to process rest
		case 2:
			if (!CurrentSetParam(SET_ESBCR_MODEFLAGS2,"SET_ESBCR_MODEFLAGS2",szTempModeFlags,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_MODEFLAGS2,COLOR_YELLOW);
				WriteToFileEx(77, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Mode Flags (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_NORMACQINTERV2,"SET_ESBCR_NORMACQINTERV2",szNomAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_NOMACQINT2,COLOR_YELLOW);
				WriteToFileEx(78, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Nominal Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_ANOMACQINTERV2,"SET_ESBCR_ANOMACQINTERV2",szAnomAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_ANOMACQINT2,COLOR_YELLOW);
				WriteToFileEx(79, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Anomalous Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_LOPWACQINTERV2,"SET_ESBCR_LOPWACQINTERV2",szLPAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_LPACQINT2,COLOR_YELLOW);
				WriteToFileEx(80, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Low Power Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}
			break;

		case 7:
			SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3,sESBConfigTextRecord.chSerialNumber,sizeof(sESBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_ESBIR_SERIALNUM3,"SET_ESBIR_SERIALNUM3",szTempSN,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL3,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
		//fall through to process rest
		case 3:
			if (!CurrentSetParam(SET_ESBCR_MODEFLAGS3,"SET_ESBCR_MODEFLAGS3",szTempModeFlags,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_MODEFLAGS3,COLOR_YELLOW);
				WriteToFileEx(81, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Mode Flags (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_NORMACQINTERV3,"SET_ESBCR_NORMACQINTERV3",szNomAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_NOMACQINT3,COLOR_YELLOW);
				WriteToFileEx(82, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Nominal Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_ANOMACQINTERV3,"SET_ESBCR_ANOMACQINTERV3",szAnomAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_ANOMACQINT3,COLOR_YELLOW);
				WriteToFileEx(83, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Anomalous Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_LOPWACQINTERV3,"SET_ESBCR_LOPWACQINTERV3",szLPAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_LPACQINT3,COLOR_YELLOW);
				WriteToFileEx(84, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Low Power Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}
			break;

		case 8:
			SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4,sESBConfigTextRecord.chSerialNumber,sizeof(sESBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_ESBIR_SERIALNUM4,"SET_ESBIR_SERIALNUM4",szTempSN,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL4,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
		//fall through to process rest
		case 4:
			if (!CurrentSetParam(SET_ESBCR_MODEFLAGS4,"SET_ESBCR_MODEFLAGS4",szTempModeFlags,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_MODEFLAGS4,COLOR_YELLOW);
				WriteToFileEx(85, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Mode Flags (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_NORMACQINTERV4,"SET_ESBCR_NORMACQINTERV4",szNomAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_NOMACQINT4,COLOR_YELLOW);
				WriteToFileEx(86, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Nominal Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_ANOMACQINTERV4,"SET_ESBCR_ANOMACQINTERV4",szAnomAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_ANOMACQINT4,COLOR_YELLOW);
				WriteToFileEx(87, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Anomalous Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_ESBCR_LOPWACQINTERV4,"SET_ESBCR_LOPWACQINTERV4",szLPAcqInterv,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_LPACQINT4,COLOR_YELLOW);
				WriteToFileEx(88, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Low Power Acquisition Interval (52)\r\n");
				m_iESBInfo_Tab |= 0x0002;
			}
			break;
		}
		}	
	//send it off to be displayed if possible
	//and save it for later
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&sESBConfigTextRecord,sizeof(sESBConfigTextRecord));
	cBuff[sizeof(sESBConfigTextRecord) + 6] = NULL;

	if (!bStale)
	{
		switch(iColumn) {
		case 1:
		case 5:
			m_cESBConfigRecord1 = cBuff+6;
			break;
		case 2:
		case 6:
			m_cESBConfigRecord2 = cBuff+6;
			break;
		case 3:
		case 7:
			m_cESBConfigRecord3 = cBuff+6;
			break;
		case 4:
		case 8:
			m_cESBConfigRecord4 = cBuff+6;
			break;
		}
		if (m_pPropertySheet)
			m_pPropertySheet->OnESBConfigRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)&ESBConfigRec);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,ESBCONFIGRECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(ESBConfigRecord	);
}

int CInstrument::OnBinaryPSBConfigRecord(BYTE *pBinaryAt)
{
	//some temp buffers
	char szTemp1[32];
	char szTempModeFlags[8];
	char szTempHVSetPoint[8];
	char szTempHVConfState[2];
	char szTempSN[16];
	CString cTemp;

	//create a binary record pointer
	PSBConfigRecord* PSBConfigRec = (PSBConfigRecord*)pBinaryAt;

	//make the multibytes all little endian
	Reverse((BYTE*)&PSBConfigRec->uiJulianSeconds,sizeof(PSBConfigRec->uiJulianSeconds));
	Reverse((BYTE*)&PSBConfigRec->usModeFlags,sizeof(PSBConfigRec->usModeFlags));
	Reverse((BYTE*)&PSBConfigRec->fHVSetPoint,sizeof(PSBConfigRec->fHVSetPoint));
	//Reverse((BYTE*)&PSBConfigRec->bSerialNumber,sizeof(PSBConfigRec->bSerialNumber));

	//get a text record
	PSBConfigTextRecord sPSBConfigTextRecord;

	//get opcode and place in text record
	sprintf(szTemp1,"%02x",PSBConfigRec->bOpcode);
	memcpy(sPSBConfigTextRecord.chOpcode,szTemp1,sizeof(sPSBConfigTextRecord.chOpcode));

	//get julian time and put it in the binary record
	sprintf(szTemp1,"%10u",PSBConfigRec->uiJulianSeconds);
	memcpy(sPSBConfigTextRecord.cdJulianSeconds,szTemp1,sizeof(sPSBConfigTextRecord.cdJulianSeconds));

	//get mode bytes
	sprintf(szTempModeFlags,"%04x",PSBConfigRec->usModeFlags);
	memcpy(sPSBConfigTextRecord.chModeFlags,szTempModeFlags,sizeof(sPSBConfigTextRecord.chModeFlags));

	//get hv set point
	sprintf(szTempHVSetPoint,"%7.1f",PSBConfigRec->fHVSetPoint);
	memcpy(sPSBConfigTextRecord.cdfHVSetPoint,szTempHVSetPoint,sizeof(sPSBConfigTextRecord.cdfHVSetPoint));

	//get hv configrued state
	sprintf(szTempHVConfState,"%1x",PSBConfigRec->bHVConfigState);
	memcpy(&sPSBConfigTextRecord.chHVConfigState,szTempHVConfState,sizeof(sPSBConfigTextRecord.chHVConfigState));

	//got a binary file so don't have a message checksum
	sPSBConfigTextRecord.chCheckSum[0] = '-';
	sPSBConfigTextRecord.chCheckSum[1] = '-';

	//set the cr and lf in the message
	sPSBConfigTextRecord.cCR = '\r';
	sPSBConfigTextRecord.cLF = '\n';

	//build the serial number and use it to find which column to put this data in
	sprintf(szTempSN,"%02x%02x%02x%02x%02x%02x",
		PSBConfigRec->bSerialNumber[0],
		PSBConfigRec->bSerialNumber[1],
		PSBConfigRec->bSerialNumber[2],
		PSBConfigRec->bSerialNumber[3],
		PSBConfigRec->bSerialNumber[4],
		PSBConfigRec->bSerialNumber[5]);
	memcpy(sPSBConfigTextRecord.chSerialNumber,szTempSN,sizeof(sPSBConfigTextRecord.chSerialNumber));

	//text version should be complete at this point

	//set the date & time
	CJulianTime cTime;
	cTime.Init(PSBConfigRec->uiJulianSeconds);

	bool bStale = true;

	int iColumn = 0;
	if      (strncmp((char*)&sPSBConfigTextRecord.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1),sizeof(sPSBConfigTextRecord.chSerialNumber)) == 0) iColumn = 1;
	else if (strncmp((char*)&sPSBConfigTextRecord.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2),sizeof(sPSBConfigTextRecord.chSerialNumber)) == 0) iColumn = 2;
	else if (strncmp((char*)&sPSBConfigTextRecord.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3),sizeof(sPSBConfigTextRecord.chSerialNumber)) == 0) iColumn = 3;
	else if (strncmp((char*)&sPSBConfigTextRecord.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4),sizeof(sPSBConfigTextRecord.chSerialNumber)) == 0) iColumn = 4;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1))==0) iColumn = 5;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2))==0) iColumn = 6;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3))==0) iColumn = 7;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4))==0) iColumn = 8;

	if (m_uiPSBConfigRecordTime < cTime.TotalSeconds())
	{
		//newer than the last received
		bStale = false;

		//update the time of the latest record
		m_uiPSBConfigRecordTime = cTime.TotalSeconds();	

		//set date and time
		switch (iColumn) {
		case 5:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,sPSBConfigTextRecord.chSerialNumber,sizeof(sPSBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM1,"SET_PSBIR_SERIALNUM1",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
		//fall through to process rest
		case 1:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEC1,cTime.Date());
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEC1,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_PSBCR_TIME1,"SET_PSBCR_TIME1",cTemp,false);

			if (!CurrentSetParam(SET_PSBCR_MODEFLAGS1,"SET_PSBCR_MODEFLAGS1",szTempModeFlags,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_MODEFLAGS1,COLOR_YELLOW);
				WriteToFileEx(89, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB Mode Flags (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBCR_HVSETPOINT1,"SET_PSBCR_HVSETPOINT1",szTempHVSetPoint,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVSETPT1,COLOR_YELLOW);
				WriteToFileEx(90, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Set Point (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBCR_CONFIGSTATE1,"SET_PSBCR_CONFIGSTATE1",szTempHVConfState,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVCONFSTATE1,COLOR_YELLOW);
				WriteToFileEx(91, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Configured State (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;

		case 6:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,sPSBConfigTextRecord.chSerialNumber,sizeof(sPSBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM2,"SET_PSBIR_SERIALNUM2",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 2:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEC2,cTime.Date());
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEC2,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_PSBCR_TIME2,"SET_PSBCR_TIME2",cTemp,false);

			if (!CurrentSetParam(SET_PSBCR_MODEFLAGS2,"SET_PSBCR_MODEFLAGS2",szTempModeFlags,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_MODEFLAGS2,COLOR_YELLOW);
				WriteToFileEx(92, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB Mode Flags (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBCR_HVSETPOINT2,"SET_PSBCR_HVSETPOINT2",szTempHVSetPoint,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVSETPT2,COLOR_YELLOW);
				WriteToFileEx(93, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Set Point (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBCR_CONFIGSTATE2,"SET_PSBCR_CONFIGSTATE2",szTempHVConfState,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVCONFSTATE2,COLOR_YELLOW);
				WriteToFileEx(94, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Configured State (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;

		case 7:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,sPSBConfigTextRecord.chSerialNumber,sizeof(sPSBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM3,"SET_PSBIR_SERIALNUM3",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 3:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEC3,cTime.Date());
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEC3,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_PSBCR_TIME3,"SET_PSBCR_TIME3",cTemp,false);

			if (!CurrentSetParam(SET_PSBCR_MODEFLAGS3,"SET_PSBCR_MODEFLAGS3",szTempModeFlags,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_MODEFLAGS3,COLOR_YELLOW);
				WriteToFileEx(95, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB Mode Flags (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBCR_HVSETPOINT3,"SET_PSBCR_HVSETPOINT3",szTempHVSetPoint,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVSETPT3,COLOR_YELLOW);
				WriteToFileEx(96, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Set Point (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBCR_CONFIGSTATE3,"SET_PSBCR_CONFIGSTATE3",szTempHVConfState,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVCONFSTATE3,COLOR_YELLOW);
				WriteToFileEx(97, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Configured State (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;

		case 8:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,sPSBConfigTextRecord.chSerialNumber,sizeof(sPSBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM4,"SET_PSBIR_SERIALNUM4",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 4:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEC4,cTime.Date());
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEC4,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_PSBCR_TIME4,"SET_PSBCR_TIME4",cTemp,false);

			if (!CurrentSetParam(SET_PSBCR_MODEFLAGS4,"SET_PSBCR_MODEFLAGS4",szTempModeFlags,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_MODEFLAGS4,COLOR_YELLOW);
				WriteToFileEx(98, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB Mode Flags (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBCR_HVSETPOINT4,"SET_PSBCR_HVSETPOINT4",szTempHVSetPoint,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVSETPT4,COLOR_YELLOW);
				WriteToFileEx(99, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Set Point (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBCR_CONFIGSTATE4,"SET_PSBCR_CONFIGSTATE4",szTempHVConfState,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVCONFSTATE4,COLOR_YELLOW);
				WriteToFileEx(100, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Configured State (53)\r\n");
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;
		}
	}
	
	//send it off to be displayed if possible
	//and save it for later
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&sPSBConfigTextRecord,sizeof(sPSBConfigTextRecord));
	cBuff[sizeof(sPSBConfigTextRecord) + 6] = NULL;

	if (!bStale)
	{
		switch(iColumn) {
		case 1:
		case 5:
			m_cPSBConfigRecord1 = cBuff+6;
			break;
		case 2:
		case 6:
			m_cPSBConfigRecord2 = cBuff+6;
			break;
		case 3:
		case 7:
			m_cPSBConfigRecord3 = cBuff+6;
			break;
		case 4:
		case 8:
			m_cPSBConfigRecord4 = cBuff+6;
			break;
		}
		if (m_pPropertySheet)
			m_pPropertySheet->OnPSBConfigRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)&PSBConfigRec);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,PSBCONFIGRECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(PSBConfigRecord	);
}

int CInstrument::OnBinaryBMBConfigRecord(BYTE *pBinaryAt)
{
	//some temp buffers
	char szTemp1[32];
	char szTempSN[16];
	char szTempPwrOn[8];
	char szTempPwrOff[8];
	char szTempModeFlags[8];
	CString cTemp;

	//create a binary record pointer
	BMBConfigRecord* BMBConfigRec = (BMBConfigRecord*)pBinaryAt;

	//make the multibytes all little endian
	Reverse((BYTE*)&BMBConfigRec->uiJulianSeconds,sizeof(BMBConfigRec->uiJulianSeconds));
	//Reverse((BYTE*)&BMBConfigRec->usModeFlags,sizeof(BMBConfigRec->usModeFlags));
	Reverse((BYTE*)&BMBConfigRec->usPowerOnReportInter,sizeof(BMBConfigRec->usPowerOnReportInter));
	Reverse((BYTE*)&BMBConfigRec->usPowerOffReportInterv,sizeof(BMBConfigRec->usPowerOffReportInterv));
	//Reverse((BYTE*)&BMBConfigRec->bSerialNumber,sizeof(BMBConfigRec->bSerialNumber));

	//get a text record
	BMBConfigTextRecord sBMBConfigTextRecord;

	//get opcode and place in text record
	sprintf(szTemp1,"%02x",BMBConfigRec->bOpcode);
	memcpy(sBMBConfigTextRecord.chOpcode,szTemp1,sizeof(sBMBConfigTextRecord.chOpcode));

	//get julian time and put it in the binary record
	sprintf(szTemp1,"%10u",BMBConfigRec->uiJulianSeconds);
	memcpy(sBMBConfigTextRecord.cdJulianSeconds,szTemp1,sizeof(sBMBConfigTextRecord.cdJulianSeconds));

	//get mode bytes
	sprintf(szTempModeFlags,"%04x",BMBConfigRec->usModeFlags);
	memcpy(sBMBConfigTextRecord.chModeFlags,szTempModeFlags,sizeof(sBMBConfigTextRecord.chModeFlags));

	//get power on status-reporting interval (sec)
	sprintf(szTempPwrOn,"%5d",BMBConfigRec->usPowerOnReportInter);
	memcpy(sBMBConfigTextRecord.cdPowerOnReportInter,szTempPwrOn,sizeof(sBMBConfigTextRecord.cdPowerOnReportInter));

	//get power off status-reporting interval (sec)
	sprintf(szTempPwrOff,"%5d",BMBConfigRec->usPowerOffReportInterv);
	memcpy(sBMBConfigTextRecord.cdPowerOffReportInterv,szTempPwrOn,sizeof(sBMBConfigTextRecord.cdPowerOffReportInterv));

	//got a binary file so don't have a message checksum
	sBMBConfigTextRecord.chCheckSum[0] = '-';
	sBMBConfigTextRecord.chCheckSum[1] = '-';

	//set the cr and lf in the message
	sBMBConfigTextRecord.cCR = '\r';
	sBMBConfigTextRecord.cLF = '\n';

	//build the serial number and use it to find which column to put this data in
	sprintf(szTempSN,"%02x%02x%02x%02x%02x%02x",
		BMBConfigRec->bSerialNumber[0],
		BMBConfigRec->bSerialNumber[1],
		BMBConfigRec->bSerialNumber[2],
		BMBConfigRec->bSerialNumber[3],
		BMBConfigRec->bSerialNumber[4],
		BMBConfigRec->bSerialNumber[5]);
	memcpy(sBMBConfigTextRecord.chSerialNumber,szTempSN,sizeof(sBMBConfigTextRecord.chSerialNumber));

	//text version should be complete at this point

	//set the date & time
	CJulianTime cTime;
	cTime.Init(BMBConfigRec->uiJulianSeconds);

	bool bStale = true;

	int iColumn = 0;
	if      (strncmp((char*)&sBMBConfigTextRecord.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1),sizeof(sBMBConfigTextRecord.chSerialNumber)) == 0) iColumn = 1;
	else if (strncmp((char*)&sBMBConfigTextRecord.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2),sizeof(sBMBConfigTextRecord.chSerialNumber)) == 0) iColumn = 2;
	else if (strncmp((char*)&sBMBConfigTextRecord.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3),sizeof(sBMBConfigTextRecord.chSerialNumber)) == 0) iColumn = 3;
	else if (strncmp((char*)&sBMBConfigTextRecord.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4),sizeof(sBMBConfigTextRecord.chSerialNumber)) == 0) iColumn = 4;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1))==0) iColumn = 5;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2))==0) iColumn = 6;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3))==0) iColumn = 7;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4))==0) iColumn = 8;

	if (m_uiBMBConfigRecordTime < cTime.TotalSeconds())
	{
		//newer than the last received
		bStale = false;

		//update the time of the latest record
		m_uiBMBConfigRecordTime = cTime.TotalSeconds();	

		//set date and time
		switch (iColumn) {
		case 5:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,sBMBConfigTextRecord.chSerialNumber,sizeof(sBMBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM1,"SET_BMBIR_SERIALNUM1",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 1:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEC1,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEC1,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_BMBCR_TIME1,"SET_BMBCR_TIME1",cTemp,false);

			if (!CurrentSetParam(SET_BMBCR_MODEFLAGS1,"SET_BMBCR_MODEFLAGS1",szTempModeFlags,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_MODEFLAGS1,COLOR_YELLOW);
				WriteToFileEx(101, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Mode Flags (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_BMBCR_PONSTATINT1,"SET_BMBCR_PONSTATINT1",szTempPwrOn,false))
			{
				SetColor(PSBINFOPAGE,IDC_BMBINFO_PONRPT1,COLOR_YELLOW);
				WriteToFileEx(102, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power On Status Reporting Interval (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_BMBCR_POFFSTATINT1,"SET_BMBCR_POFFSTATINT1",szTempPwrOff,false))
			{
				SetColor(PSBINFOPAGE,IDC_BMBINFO_POFFRPT1,COLOR_YELLOW);
				WriteToFileEx(103, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power Off Status Reporting Interval (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;

		case 6:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,sBMBConfigTextRecord.chSerialNumber,sizeof(sBMBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM2,"SET_BMBIR_SERIALNUM2",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 2:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEC2,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEC2,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_BMBCR_TIME2,"SET_BMBCR_TIME2",cTemp,false);

			if (!CurrentSetParam(SET_BMBCR_MODEFLAGS2,"SET_BMBCR_MODEFLAGS2",szTempModeFlags,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_MODEFLAGS2,COLOR_YELLOW);
				WriteToFileEx(104, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Mode Flags (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_BMBCR_PONSTATINT2,"SET_BMBCR_PONSTATINT2",szTempPwrOn,false))
			{
				SetColor(PSBINFOPAGE,IDC_BMBINFO_PONRPT2,COLOR_YELLOW);
				WriteToFileEx(105, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power On Status Reporting Interval (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_BMBCR_POFFSTATINT2,"SET_BMBCR_POFFSTATINT2",szTempPwrOff,false))
			{
				SetColor(PSBINFOPAGE,IDC_BMBINFO_POFFRPT2,COLOR_YELLOW);
				WriteToFileEx(106, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power Off Status Reporting Interval (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;

		case 7:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,sBMBConfigTextRecord.chSerialNumber,sizeof(sBMBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM3,"SET_BMBIR_SERIALNUM3",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 3:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEC3,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEC3,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_BMBCR_TIME3,"SET_BMBCR_TIME3",cTemp,false);

			if (!CurrentSetParam(SET_BMBCR_MODEFLAGS3,"SET_BMBCR_MODEFLAGS3",szTempModeFlags,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_MODEFLAGS3,COLOR_YELLOW);
				WriteToFileEx(107, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Mode Flags (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_BMBCR_PONSTATINT3,"SET_BMBCR_PONSTATINT3",szTempPwrOn,false))
			{
				SetColor(PSBINFOPAGE,IDC_BMBINFO_PONRPT3,COLOR_YELLOW);
				WriteToFileEx(108, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power On Status Reporting Interval (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_BMBCR_POFFSTATINT3,"SET_BMBCR_POFFSTATINT3",szTempPwrOff,false))
			{
				SetColor(PSBINFOPAGE,IDC_BMBINFO_POFFRPT3,COLOR_YELLOW);
				WriteToFileEx(109, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power Off Status Reporting Interval (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;

		case 8:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,sBMBConfigTextRecord.chSerialNumber,sizeof(sBMBConfigTextRecord.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM4,"SET_BMBIR_SERIALNUM4",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 4:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEC4,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEC4,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_BMBCR_TIME4,"SET_BMBCR_TIME4",cTemp,false);

			if (!CurrentSetParam(SET_BMBCR_MODEFLAGS4,"SET_BMBCR_MODEFLAGS4",szTempModeFlags,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_MODEFLAGS4,COLOR_YELLOW);
				WriteToFileEx(110, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Mode Flags (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_BMBCR_PONSTATINT4,"SET_BMBCR_PONSTATINT4",szTempPwrOn,false))
			{
				SetColor(PSBINFOPAGE,IDC_BMBINFO_PONRPT4,COLOR_YELLOW);
				WriteToFileEx(111, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power On Status Reporting Interval (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_BMBCR_POFFSTATINT4,"SET_BMBCR_POFFSTATINT4",szTempPwrOff,false))
			{
				SetColor(PSBINFOPAGE,IDC_BMBINFO_POFFRPT4,COLOR_YELLOW);
				WriteToFileEx(112, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power Off Status Reporting Interval (54)\r\n");
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;
		}
	}

	//send it off to be displayed if possible
	//and save it for later
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&sBMBConfigTextRecord,sizeof(sBMBConfigTextRecord));
	cBuff[sizeof(sBMBConfigTextRecord) + 6] = NULL;

	if (!bStale)
	{
		switch(iColumn) {
		case 1:
		case 5:
			m_cBMBConfigRecord1 = cBuff+6;
			break;
		case 2:
		case 6:
			m_cBMBConfigRecord2 = cBuff+6;
			break;
		case 3:
		case 7:
			m_cBMBConfigRecord3 = cBuff+6;
			break;
		case 4:
		case 8:
			m_cBMBConfigRecord4 = cBuff+6;
			break;
		}
		if (m_pPropertySheet)
			m_pPropertySheet->OnBMBConfigRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)&BMBConfigRec);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,BMBCONFIGRECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(BMBConfigRecord	);
}

int CInstrument::OnBinaryuGStatusRecord(BYTE *pBinaryAt)
{
	//some temporary buffers
	char szTemp1[32];
	CString cTemp;

	//create a binary record pointer
	uGStatusRecord* uGStatusRec = (uGStatusRecord*)pBinaryAt;

	//make the multibytes all little endian
	Reverse((BYTE*)&uGStatusRec->uiJulianSeconds,sizeof(uGStatusRec->uiJulianSeconds));
	Reverse((BYTE*)&uGStatusRec->f50VSupply,sizeof(uGStatusRec->f50VSupply));
	Reverse((BYTE*)&uGStatusRec->f33VSupply,sizeof(uGStatusRec->f33VSupply));
	Reverse((BYTE*)&uGStatusRec->f25VSupply,sizeof(uGStatusRec->f25VSupply));
	Reverse((BYTE*)&uGStatusRec->fPHDVoltage,sizeof(uGStatusRec->fPHDVoltage));
	Reverse((BYTE*)&uGStatusRec->uiCPUUpTime,sizeof(uGStatusRec->uiCPUUpTime));
	Reverse((BYTE*)&uGStatusRec->fCPUTemp,sizeof(uGStatusRec->fCPUTemp));
	Reverse((BYTE*)&uGStatusRec->fBOARDTemp,sizeof(uGStatusRec->fBOARDTemp));
	Reverse((BYTE*)&uGStatusRec->uiReserved,sizeof(uGStatusRec->uiReserved));

	//Get a uGStatusTextRecord
	uGStatusTextRecord sUGStatusTextRecord;

	//get opcode and place in text record
	sprintf(szTemp1,"%02x",uGStatusRec->bOpcode);
	memcpy(sUGStatusTextRecord.chOpcode,szTemp1,sizeof(sUGStatusTextRecord.chOpcode));

	//get julian time 
	sprintf(szTemp1,"%10u",uGStatusRec->uiJulianSeconds);
	memcpy(sUGStatusTextRecord.cdJulianSeconds,szTemp1,sizeof(sUGStatusTextRecord.cdJulianSeconds));

	//get interval 10th of second (szTemp1 is used below)
	sprintf(szTemp1,"%1d",uGStatusRec->bJulianSec10ths);
	sUGStatusTextRecord.cdJulianSec10ths = szTemp1[0];

	//set the date & time
	CJulianTime cTime;
	cTime.Init(uGStatusRec->uiJulianSeconds);

	bool bStale = true;

	//need to adjust for fractional time
	if (m_duGStatusRecordTime < (cTime.TotalSeconds()+uGStatusRec->bJulianSec10ths/10.0))	
	{	
		//newer than the last received
		bStale = false;

		//update the time of the latest record
		m_duGStatusRecordTime = cTime.TotalSeconds()+uGStatusRec->bJulianSec10ths/10.0;	

		//set date and time
		SetString(UGINFOPAGE,IDC_UGINFO_DATADATES,cTime.Date());
		cTemp = cTime.Time();
		cTemp += ".";
		cTemp += szTemp1;
		SetString(UGINFOPAGE,IDC_UGINFO_DATATIMES,cTemp);
		cTemp = cTime.Date();
		cTemp += " ";
		cTemp += cTime.Time();
		cTemp += ".";
		cTemp += szTemp1;
		CurrentSetParam(SET_UGSR_TIME,"SET_UGSR_TIME",cTemp,false);
	}

	//set the 5.0 volt supply
	sprintf(szTemp1,"%4.1f",uGStatusRec->f50VSupply);
	memcpy(&sUGStatusTextRecord.cdf50VSupply,szTemp1,sizeof(sUGStatusTextRecord.cdf50VSupply));
	if (!bStale)
	{
		//+5 volts
		bool bCurrentRed = false;
		bool bCurrentLow = false;
		bool bCurrentHigh= false;

		m_fCurP5 = uGStatusRec->f50VSupply;
		if (m_bFirstP5)
		{
			m_bFirstP5 = false;
			m_fLowP5 = m_fHighP5 = m_fCurP5;
		}
		if (m_fCurP5 < m_fLowP5)
		{
			bCurrentLow = true;
			m_fLowP5 = m_fCurP5;
		}
		else
			bCurrentLow = false;
		if (m_fCurP5 > m_fHighP5)
		{
			bCurrentHigh = true;
			m_fHighP5= m_fCurP5;
		}
		else
			bCurrentHigh = false;
		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		{
			bCurrentRed = true;			
			SetColor(UGINFOPAGE,IDC_UGINFO_50VSUPPLYS,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,COLOR_RED);
			if (m_bP5VoltOk)
			{
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 5 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
				FlagOthr(1,true);
			}
			m_bP5VoltOk = false;

			m_iUGInfo_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			if (!m_bP5VoltOk)
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 5 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
			m_bP5VoltOk = true;
		}

		if (bCurrentRed && bCurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP5 >= m_fSup_P05H))// || (m_fHighP5 >= m_fSup_P05H))
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fCurP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,szTemp1);
		SetString(UGINFOPAGE,IDC_UGINFO_50VSUPPLYS,szTemp1);
		CurrentSetParam(SET_UGSR_50V,"SET_UGSR_50V",szTemp1,false);

		sprintf(szTemp1,"%4.1f",m_fHighP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,szTemp1);
	}

	//set the 3.3 volt supply (uses the -15 from microGRAND)
	sprintf(szTemp1,"%4.1f",uGStatusRec->f33VSupply);
	memcpy(&sUGStatusTextRecord.cdf33VSupply,szTemp1,sizeof(sUGStatusTextRecord.cdf33VSupply));
	if (!bStale)
	{
		//+33 volts
		bool bCurrentRed = false;
		bool bCurrentLow = false;
		bool bCurrentHigh= false;

		m_fCurP33 = uGStatusRec->f33VSupply;
		if (m_bFirstP33)
		{
			m_bFirstP33 = false;
			m_fLowP33 = m_fHighP33 = m_fCurP33;
		}
		if (m_fCurP33 < m_fLowP33)
		{
			bCurrentLow = true;
			m_fLowP33 = m_fCurP33;
		}
		else
			bCurrentLow = false;
		if (m_fCurP33 > m_fHighP33)
		{
			bCurrentHigh = true;
			m_fHighP33 = m_fCurP33;
		}
		else
			bCurrentHigh = false;
		if ((m_fCurP33 <= m_fSup_P33L) || (m_fCurP33 >= m_fSup_P33H))
		{
			bCurrentRed = true;			
			SetColor(UGINFOPAGE,IDC_UGINFO_33VSUPPLYS,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P33VOLT2,COLOR_RED);
			if (m_bP33VoltOk)
			{
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 3.3 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
				FlagOthr(1,true);
			}
			m_bP33VoltOk = false;

			m_iUGInfo_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			if (!m_bP33VoltOk)
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 3.3 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
			m_bP33VoltOk = true;
		}

		if (bCurrentRed && bCurrentLow && (m_fLowP33 <= m_fSup_P33L))// || (m_fLowP5 >= m_fSup_P05H))
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P33VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP33 >= m_fSup_P33H))// || (m_fHighP5 >= m_fSup_P05H))
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P33VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP33);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P33VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fCurP25);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P33VOLT2,szTemp1);
		SetString(UGINFOPAGE,IDC_UGINFO_33VSUPPLYS,szTemp1);
		CurrentSetParam(SET_UGSR_33V,"SET_UGSR_33V",szTemp1,false);

		sprintf(szTemp1,"%4.1f",m_fHighP33);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P33VOLT3,szTemp1);
	}

	//set the 2.5 volt supply (uses the +15 from microGRAND)
	sprintf(szTemp1,"%4.1f",uGStatusRec->f25VSupply);
	memcpy(&sUGStatusTextRecord.cdf25VSupply,szTemp1,sizeof(sUGStatusTextRecord.cdf25VSupply));
	if (!bStale)
	{
		//+2.5 volts
		bool bCurrentRed = false;
		bool bCurrentLow = false;
		bool bCurrentHigh= false;

		m_fCurP25 = uGStatusRec->f25VSupply;
		if (m_bFirstP25)
		{
			m_bFirstP25 = false;
			m_fLowP25 = m_fHighP25 = m_fCurP25;
		}
		if (m_fCurP25 < m_fLowP25)
		{
			bCurrentLow = true;
			m_fLowP25 = m_fCurP25;
		}
		else
			bCurrentLow = false;
		if (m_fCurP25 > m_fHighP25)
		{
			bCurrentHigh = true;
			m_fHighP25 = m_fCurP25;
		}
		else
			bCurrentHigh = false;
		if ((m_fCurP25 <= m_fSup_P25L) || (m_fCurP25 >= m_fSup_P25H))
		{
			bCurrentRed = true;			
			SetColor(UGINFOPAGE,IDC_UGINFO_25VSUPPLYS,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P25VOLT2,COLOR_RED);
			if (m_bP25VoltOk)
			{
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 2.5 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
				FlagOthr(1,true);
			}
			m_bP25VoltOk = false;

			m_iUGInfo_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			if (!m_bP25VoltOk)
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 2.5 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
			m_bP25VoltOk = true;
		}

		if (bCurrentRed && bCurrentLow && (m_fLowP25 <= m_fSup_P25L))// || (m_fLowP5 >= m_fSup_P05H))
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P25VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP25 >= m_fSup_P25H))// || (m_fHighP5 >= m_fSup_P05H))
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P25VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP25);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P25VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fCurP25);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P25VOLT2,szTemp1);
		SetString(UGINFOPAGE,IDC_UGINFO_25VSUPPLYS,szTemp1);
		CurrentSetParam(SET_UGSR_25V,"SET_UGSR_25V",szTemp1,false);

		sprintf(szTemp1,"%4.1f",m_fHighP25);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P25VOLT3,szTemp1);
	}

	//set the PHD volt supply (uses the BAT from microGRAND)
	sprintf(szTemp1,"%4.1f",uGStatusRec->fPHDVoltage);
	memcpy(&sUGStatusTextRecord.cdfPHDVoltage,szTemp1,sizeof(sUGStatusTextRecord.cdfPHDVoltage));
	if (!bStale)
	{
		//+2.5 volts
		bool bCurrentRed = false;
		bool bCurrentLow = false;
		bool bCurrentHigh= false;

		m_fCurBV = uGStatusRec->fPHDVoltage;
		if (m_bFirstBV)
		{
			m_bFirstBV = false;
			m_fLowBV = m_fHighBV = m_fCurBV;
		}
		if (m_fCurBV < m_fLowBV)
		{
			bCurrentLow = true;
			m_fLowBV = m_fCurBV;
		}
		else
			bCurrentLow = false;
		if (m_fCurBV > m_fHighBV)
		{
			bCurrentHigh = true;
			m_fHighBV = m_fCurBV;
		}
		else
			bCurrentHigh = false;
		if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
		{
			bCurrentRed = true;			
			SetColor(UGINFOPAGE,IDC_UGINFO_PHDVS,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PHD VOLT OUT OF TOLERANCE (1b)\r\n");
				FlagOthr(1,true);
			}
			m_bBatteryOk = false;

			m_iUGInfo_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			if (!m_bBatteryOk)
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PHD VOLT NOT OUT OF TOLERANCE (1b)\r\n");
			m_bBatteryOk = true;
		}

		if (bCurrentRed && bCurrentLow && (m_fLowBV <= m_fBatVoltL))// || (m_fLowP5 >= m_fSup_P05H))
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighBV >= m_fBatVoltH))// || (m_fHighP5 >= m_fSup_P05H))
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fCurBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,szTemp1);
		SetString(UGINFOPAGE,IDC_UGINFO_PHDVS,szTemp1);
		CurrentSetParam(SET_UGSR_PHDVOLT,"SET_UGSR_PHDVOLT",szTemp1,false);

		sprintf(szTemp1,"%4.1f",m_fHighBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,szTemp1);
	}

	//set status byte 1
	sprintf(szTemp1,"%02x",uGStatusRec->bStatus1Byte);
	memcpy(&sUGStatusTextRecord.chStatus1Byte,szTemp1,sizeof(sUGStatusTextRecord.chStatus1Byte));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_STATUS1S,szTemp1);
		CurrentSetParam(SET_UGSR_STATUSBYTE1,"SET_UGSR_STATUSBYTE1",szTemp1,false);
	}

	//set status byte 2
	sprintf(szTemp1,"%02x",uGStatusRec->bStatus2Byte);
	memcpy(&sUGStatusTextRecord.chStatus2Byte,szTemp1,sizeof(sUGStatusTextRecord.chStatus2Byte));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_STATUS2S,szTemp1);
		CurrentSetParam(SET_UGSR_STATUSBYTE2,"SET_UGSR_STATUSBYTE2",szTemp1,false);
	}

	//set status byte 3
	sprintf(szTemp1,"%02x",uGStatusRec->bStatus3Byte);
	memcpy(&sUGStatusTextRecord.chStatus3Byte,szTemp1,sizeof(sUGStatusTextRecord.chStatus3Byte));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_STATUS3S,szTemp1);
		CurrentSetParam(SET_UGSR_STATUSBYTE3,"SET_UGSR_STATUSBYTE3",szTemp1,false);
	}

	//set status byte 4
	sprintf(szTemp1,"%02x",uGStatusRec->bStatus4Byte);
	memcpy(&sUGStatusTextRecord.chStatus4Byte,szTemp1,sizeof(sUGStatusTextRecord.chStatus4Byte));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_STATUS4S,szTemp1);
		CurrentSetParam(SET_UGSR_STATUSBYTE4,"SET_UGSR_STATUSBYTE4",szTemp1,false);
	}

	//set cpu up time
	sprintf(szTemp1,"%10u",uGStatusRec->uiCPUUpTime);
	memcpy(&sUGStatusTextRecord.chCPUUpTime,szTemp1,sizeof(sUGStatusTextRecord.chCPUUpTime));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CPUUPS,szTemp1);
		CurrentSetParam(SET_UGSR_CPUUPTIME,"SET_UGSR_CPUUPTIME",szTemp1,false);
	}

	//set cpu temperature
	sprintf(szTemp1,"%6.1f",uGStatusRec->fCPUTemp);
	memcpy(&sUGStatusTextRecord.cdfCPUTemp,szTemp1,sizeof(sUGStatusTextRecord.cdfCPUTemp));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_CPUTEMPS,szTemp1);
		CurrentSetParam(SET_UGSR_CPUTEMP,"SET_UGSR_CPUTEMP",szTemp1,false);
	}

	//set board temperature
	sprintf(szTemp1,"%6.1f",uGStatusRec->fBOARDTemp);
	memcpy(&sUGStatusTextRecord.cdfBOARDTemp,szTemp1,sizeof(sUGStatusTextRecord.cdfBOARDTemp));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_BOARDTEMPS,szTemp1);
		CurrentSetParam(SET_UGSR_BOARDTEMP,"SET_UGSR_BOARDTEMP",szTemp1,false);
	}

	//set reserved
	sprintf(szTemp1,"%08x",uGStatusRec->uiReserved);
	memcpy(&sUGStatusTextRecord.chReserved,szTemp1,sizeof(sUGStatusTextRecord.chReserved));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_RESERVEDS,szTemp1);
		CurrentSetParam(SET_UGSR_RESERVED,"SET_UGSR_RESERVED",szTemp1,false);
	}

	//got a binary file so don't have a message checksum
	sUGStatusTextRecord.chCheckSum[0] = '-';
	sUGStatusTextRecord.chCheckSum[1] = '-';

	//set the cr and lf in the message
	sUGStatusTextRecord.cCR = '\r';
	sUGStatusTextRecord.cLF = '\n';

	//send it off to be displayed if possible
	//and save it for later
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&sUGStatusTextRecord,sizeof(uGStatusTextRecord));
	cBuff[sizeof(uGStatusTextRecord) + 6] = NULL;

	if (!bStale)
	{
		m_cUGStatusRecord = cBuff+6;
		if (m_pPropertySheet)
			m_pPropertySheet->OnuGStatusRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)&uGStatusRec);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,UGSTATUSRECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(uGStatusRecord	);
}

int CInstrument::OnBinaryPSBStatusRecord(BYTE *pBinaryAt)
{
	//some temporary buffers
	char szTemp1[32];
	CString cTemp;
	char szTempSN[16];
	char szTempExtPSV[8];
	char szTempStatus[4];
	char szTemp50VOut[8];
	char szTemp33VOut[8];
	char szTempHVVOut[8];
	char szTempHVCOut[8];
	char szTempCPUTemp[8];
	char szTempBOARDTemp[8];
	char szTempReserved[10];

	//get a text version of the record
	PSBStatusTextRecord PSBStatusTextRec;

	//get a pointer from pBinaryAt
	PSBStatusRecord* psPSBStatusRecord = (PSBStatusRecord*)pBinaryAt;

	//convert big endian to little endian
	Reverse((BYTE*)&psPSBStatusRecord->uiJulianSeconds,sizeof(psPSBStatusRecord->uiJulianSeconds));
	Reverse((BYTE*)&psPSBStatusRecord->fExternPWRSupply,sizeof(psPSBStatusRecord->fExternPWRSupply));
	Reverse((BYTE*)&psPSBStatusRecord->f50VOutput,sizeof(psPSBStatusRecord->f50VOutput));
	Reverse((BYTE*)&psPSBStatusRecord->f33VOutput,sizeof(psPSBStatusRecord->f33VOutput));
	Reverse((BYTE*)&psPSBStatusRecord->fHVOutputV,sizeof(psPSBStatusRecord->fHVOutputV));
	Reverse((BYTE*)&psPSBStatusRecord->fHVOutputC,sizeof(psPSBStatusRecord->fHVOutputC));
	Reverse((BYTE*)&psPSBStatusRecord->fCPUTemp,sizeof(psPSBStatusRecord->fCPUTemp));
	Reverse((BYTE*)&psPSBStatusRecord->fBOARDTemp,sizeof(psPSBStatusRecord->fBOARDTemp));
//	Reverse((BYTE*)&psPSBStatusRecord->bSerialNumber,sizeof(psPSBStatusRecord->bSerialNumber)); //don't think we need this
	Reverse((BYTE*)&psPSBStatusRecord->uiReserved,sizeof(psPSBStatusRecord->uiReserved));

	//only do this if binary record is going to be 
	//entered into the binary file
//	memcpy(&(RecordX[m_dRecordCountX]),psPSBStatusRecord,sizeof(psPSBStatusRecord));

	//set opcode in text version
	sprintf(szTemp1,"%02x",psPSBStatusRecord->bOpcode);
	memcpy(&PSBStatusTextRec.chOpcode,szTemp1,sizeof(PSBStatusTextRec.chOpcode));

	//set time in text version
	sprintf(szTemp1,"%10u",psPSBStatusRecord->uiJulianSeconds);
	memcpy(&PSBStatusTextRec.cdJulianSeconds,szTemp1,sizeof(PSBStatusTextRec.cdJulianSeconds));

	//set the 10ths in text version
	sprintf(szTemp1,"%d",psPSBStatusRecord->bJulianSec10ths);
	memcpy(&PSBStatusTextRec.cdJulianSec10ths,szTemp1,sizeof(PSBStatusTextRec.cdJulianSec10ths));

	//set external power supply voltage in text version
	sprintf(szTempExtPSV,"%4.1f",psPSBStatusRecord->fExternPWRSupply);
	memcpy(&PSBStatusTextRec.cdfExternPWRSupply,szTempExtPSV,sizeof(PSBStatusTextRec.cdfExternPWRSupply));

	//set the status in text version
	sprintf(szTempStatus,"%02x",psPSBStatusRecord->bStatusByte);
	memcpy(&PSBStatusTextRec.chStatusByte,szTempStatus,sizeof(PSBStatusTextRec.chStatusByte));

	//set the 5.0v output
	sprintf(szTemp50VOut,"%4.1f",psPSBStatusRecord->f50VOutput);
	memcpy(&PSBStatusTextRec.cdf50VOutput,szTemp50VOut,sizeof(PSBStatusTextRec.cdf50VOutput));

	//set the 3.3v output
	sprintf(szTemp33VOut,"%4.1f",psPSBStatusRecord->f33VOutput);
	memcpy(&PSBStatusTextRec.cdf33VOutput,szTemp33VOut,sizeof(PSBStatusTextRec.cdf33VOutput));

	//set the HVv output
	sprintf(szTempHVVOut,"%7.1f",psPSBStatusRecord->fHVOutputV);
	memcpy(&PSBStatusTextRec.cdfHVOutputV,szTempHVVOut,sizeof(PSBStatusTextRec.cdfHVOutputV));

	//set the HVc output
	sprintf(szTempHVCOut,"%7.1f",psPSBStatusRecord->fHVOutputC);
	memcpy(&PSBStatusTextRec.cdfHVOutputC,szTempHVCOut,sizeof(PSBStatusTextRec.cdfHVOutputC));

	//set the CPU Temperature
	sprintf(szTempCPUTemp,"%6.1f",psPSBStatusRecord->fCPUTemp);
	memcpy(&PSBStatusTextRec.cdfCPUTemp,szTempCPUTemp,sizeof(PSBStatusTextRec.cdfCPUTemp));

	//set the BOARD Temperature
	sprintf(szTempBOARDTemp,"%6.1f",psPSBStatusRecord->fBOARDTemp);
	memcpy(&PSBStatusTextRec.cdfBOARDTemp,szTempBOARDTemp,sizeof(PSBStatusTextRec.cdfBOARDTemp));

	//set the reserved
	sprintf(szTempReserved,"%08x",psPSBStatusRecord->uiReserved);
	memcpy(&PSBStatusTextRec.chReserved,szTempReserved,sizeof(PSBStatusTextRec.chReserved));

	//got a binary file so don't have a message checksum
	PSBStatusTextRec.chCheckSum[0] = '-';
	PSBStatusTextRec.chCheckSum[1] = '-';

	//set the cr and lf in the message
	PSBStatusTextRec.cCR = '\r';
	PSBStatusTextRec.cLF = '\n';

	//build the serial number and use it to find which column to put this data in
	sprintf(szTempSN,"%02x%02x%02x%02x%02x%02x",
		psPSBStatusRecord->bSerialNumber[0],
		psPSBStatusRecord->bSerialNumber[1],
		psPSBStatusRecord->bSerialNumber[2],
		psPSBStatusRecord->bSerialNumber[3],
		psPSBStatusRecord->bSerialNumber[4],
		psPSBStatusRecord->bSerialNumber[5]);
	memcpy(&PSBStatusTextRec.chSerialNumber,szTempSN,sizeof(PSBStatusTextRec.chSerialNumber));

	//text version should be complete at this point

	//get the time
	CJulianTime cTime;
	cTime.Init(psPSBStatusRecord->uiJulianSeconds);

	bool bStale = true;

	int iColumn = 0;
	if      (strncmp((char*)&PSBStatusTextRec.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1),sizeof(PSBStatusTextRec.chSerialNumber)) == 0) iColumn = 1;
	else if (strncmp((char*)&PSBStatusTextRec.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2),sizeof(PSBStatusTextRec.chSerialNumber)) == 0) iColumn = 2;
	else if (strncmp((char*)&PSBStatusTextRec.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3),sizeof(PSBStatusTextRec.chSerialNumber)) == 0) iColumn = 3;
	else if (strncmp((char*)&PSBStatusTextRec.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4),sizeof(PSBStatusTextRec.chSerialNumber)) == 0) iColumn = 4;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1))==0) iColumn = 5;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2))==0) iColumn = 6;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3))==0) iColumn = 7;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4))==0) iColumn = 8;

	if (m_duPSBStatusRecordTime < (cTime.TotalSeconds() + psPSBStatusRecord->bJulianSec10ths/10.0))
	{

		//newer than the last received
		bStale = false;

		//update the time of the latest record
		m_duPSBStatusRecordTime = cTime.TotalSeconds()+psPSBStatusRecord->bJulianSec10ths/10.0;	

		//set date and time
		switch (iColumn) {
		case 5:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,PSBStatusTextRec.chSerialNumber,sizeof(PSBStatusTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM1,"SET_PSBIR_SERIALNUM1",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 1:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATES1,cTime.Date());
			cTemp = cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMES1,cTemp);
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_PSBSR_TIME1,"SET_PSBSR_TIME1",cTemp,false);

			if (!CurrentSetParam(SET_PSBSR_EXTERNPSVOLTS1,"SET_PSBSR_EXTERNPSVOLTS1",szTempExtPSV,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_EXTPSV1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_PSBSR_STATUS1,"SET_PSBSR_STATUS1",szTempStatus,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_STATUS1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_PSBSR_50VOLTOUT1,"SET_PSBSR_50VOLTOUT1",szTemp50VOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_50OUT1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_33VOLTOUT1,"SET_PSBSR_33VOLTOUT1",szTemp33VOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_33OUT1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_HVVOLTOUT1,"SET_PSBSR_HVVOLTOUT1",szTempHVVOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVOUT1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_HVCURROUT1,"SET_PSBSR_HVCURROUT1",szTempHVCOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_CPUTEMP1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_CPUTEMPERATURE1,"SET_PSBSR_CPUTEMPERATURE1",szTempCPUTemp,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_BOARDTEMPERATURE1,"SET_PSBSR_BOARDTEMPERATURE1",szTempBOARDTemp,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_RESERVED1,"SET_PSBSR_RESERVED1",szTempReserved,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_RESERVE1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			break;

		case 6:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,PSBStatusTextRec.chSerialNumber,sizeof(PSBStatusTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM2,"SET_PSBIR_SERIALNUM2",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 2:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATES2,cTime.Date());
			cTemp = cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMES2,cTemp);
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_PSBSR_TIME2,"SET_PSBSR_TIME2",cTemp,false);

			if (!CurrentSetParam(SET_PSBSR_EXTERNPSVOLTS2,"SET_PSBSR_EXTERNPSVOLTS2",szTempExtPSV,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_EXTPSV2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_STATUS2,"SET_PSBSR_STATUS2",szTempStatus,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_STATUS2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_50VOLTOUT2,"SET_PSBSR_50VOLTOUT2",szTemp50VOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_50OUT2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_33VOLTOUT2,"SET_PSBSR_33VOLTOUT2",szTemp33VOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_33OUT2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_HVVOLTOUT2,"SET_PSBSR_HVVOLTOUT2",szTempHVVOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVOUT2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_HVCURROUT2,"SET_PSBSR_HVCURROUT2",szTempHVCOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_CPUTEMP2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_CPUTEMPERATURE2,"SET_PSBSR_CPUTEMPERATURE2",szTempCPUTemp,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_BOARDTEMPERATURE2,"SET_PSBSR_BOARDTEMPERATURE2",szTempBOARDTemp,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_RESERVED2,"SET_PSBSR_RESERVED2",szTempReserved,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_RESERVE2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;

		case 7:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,PSBStatusTextRec.chSerialNumber,sizeof(PSBStatusTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM3,"SET_PSBIR_SERIALNUM3",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 3:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATES3,cTime.Date());
			cTemp = cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMES3,cTemp);
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_PSBSR_TIME3,"SET_PSBSR_TIME3",cTemp,false);

			if (!CurrentSetParam(SET_PSBSR_EXTERNPSVOLTS3,"SET_PSBSR_EXTERNPSVOLTS3",szTempExtPSV,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_EXTPSV3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_STATUS3,"SET_PSBSR_STATUS3",szTempStatus,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_STATUS3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_50VOLTOUT3,"SET_PSBSR_50VOLTOUT3",szTemp50VOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_50OUT3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_33VOLTOUT3,"SET_PSBSR_33VOLTOUT3",szTemp33VOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_33OUT3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_HVVOLTOUT3,"SET_PSBSR_HVVOLTOUT3",szTempHVVOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVOUT3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_HVCURROUT3,"SET_PSBSR_HVCURROUT3",szTempHVCOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_CPUTEMP3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_CPUTEMPERATURE3,"SET_PSBSR_CPUTEMPERATURE3",szTempCPUTemp,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_BOARDTEMPERATURE3,"SET_PSBSR_BOARDTEMPERATURE3",szTempBOARDTemp,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_RESERVED3,"SET_PSBSR_RESERVED3",szTempReserved,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_RESERVE3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;

		case 8:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,PSBStatusTextRec.chSerialNumber,sizeof(PSBStatusTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM4,"SET_PSBIR_SERIALNUM4",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process rest
		case 4:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATES4,cTime.Date());
			cTemp = cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMES4,cTemp);
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_PSBSR_TIME4,"SET_PSBSR_TIME4",cTemp,false);

			if (!CurrentSetParam(SET_PSBSR_EXTERNPSVOLTS4,"SET_PSBSR_EXTERNPSVOLTS4",szTempExtPSV,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_EXTPSV4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_STATUS4,"SET_PSBSR_STATUS4",szTempStatus,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_STATUS4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_50VOLTOUT4,"SET_PSBSR_50VOLTOUT4",szTemp50VOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_50OUT4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_33VOLTOUT4,"SET_PSBSR_33VOLTOUT4",szTemp33VOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_33OUT4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_HVVOLTOUT4,"SET_PSBSR_HVVOLTOUT4",szTempHVVOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_HVOUT4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_HVCURROUT4,"SET_PSBSR_HVCURROUT4",szTempHVCOut,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_CPUTEMP4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_CPUTEMPERATURE4,"SET_PSBSR_CPUTEMPERATURE4",szTempCPUTemp,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_BOARDTEMPERATURE4,"SET_PSBSR_BOARDTEMPERATURE4",szTempBOARDTemp,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}

			if (!CurrentSetParam(SET_PSBSR_RESERVED4,"SET_PSBSR_RESERVED4",szTempReserved,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_RESERVE4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;
		}
	}

	//send it off to be displayed if possible
	//and save it for later
	char cBuff[sizeof(PSBStatusTextRecord)+7];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const  char*)&PSBStatusTextRec,sizeof(PSBStatusTextRecord));
	cBuff[sizeof(PSBStatusTextRecord) + 6] = NULL;

	if (!bStale)
	{
		switch(iColumn) {
		case 1:
		case 5:
			m_cPSBStatusRecord1 = cBuff+6;
			break;
		case 2:
		case 6:
			m_cPSBStatusRecord2 = cBuff+6;
			break;
		case 3:
		case 7:
			m_cPSBStatusRecord3 = cBuff+6;
			break;
		case 4:
		case 8:
			m_cPSBStatusRecord4 = cBuff+6;
			break;
		}
		if (m_pPropertySheet)
			m_pPropertySheet->OnPSBStatusRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)psPSBStatusRecord);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,PSBSTATUSRECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(PSBStatusRecord	);
}

int CInstrument::OnBinaryBMBStatusRecord(BYTE *pBinaryAt)
{
	//some temporary buffers
	char szTemp1[32];
	char szTempSN[16];
	CString cTemp;
	char szTempCC[8];
	char szTempDC[8];
	char szTempCD[8];
	char szTempCPUTemp[8];
	char szTempBOARDTemp[8];
	char szTempTimeOnBat[16];
	char szTempBatTemp[8];
	char szTempBatStat[4];
	char szTempReserved[10];

	//get a text version of the record
	BMBStatusTextRecord BMBStatusTextRec;

	//get a pointer from pBinaryAt
	BMBStatusRecord* psBMBStatusRecord = (BMBStatusRecord*)pBinaryAt;

	//convert big endian to little endian
	Reverse((BYTE*)&psBMBStatusRecord->uiJulianSeconds,sizeof(psBMBStatusRecord->uiJulianSeconds));
	Reverse((BYTE*)&psBMBStatusRecord->fChargeCurrent,sizeof(psBMBStatusRecord->fChargeCurrent));
	Reverse((BYTE*)&psBMBStatusRecord->fDischargeCurr,sizeof(psBMBStatusRecord->fDischargeCurr));
	Reverse((BYTE*)&psBMBStatusRecord->usChDisCycles,sizeof(psBMBStatusRecord->usChDisCycles));
	Reverse((BYTE*)&psBMBStatusRecord->fCPUTemp,sizeof(psBMBStatusRecord->fCPUTemp));
	Reverse((BYTE*)&psBMBStatusRecord->fBOARDTemp,sizeof(psBMBStatusRecord->fBOARDTemp));
	Reverse((BYTE*)&psBMBStatusRecord->uiTimeOnBatt,sizeof(psBMBStatusRecord->uiTimeOnBatt));
	Reverse((BYTE*)&psBMBStatusRecord->fBattTemp,sizeof(psBMBStatusRecord->fBattTemp));
//	Reverse((BYTE*)&psBMBStatusRecord->bSerialNumber,sizeof()); //don't think we need this
	Reverse((BYTE*)&psBMBStatusRecord->uiReserved,sizeof(psBMBStatusRecord->uiReserved));

	//only do this if binary record is going to be 
	//entered into the binary file
//	memcpy(&(RecordX[m_dRecordCountX]),psBMBStatusRecord,sizeof(psBMBStatusRecord));

	//set opcode in text version
	sprintf(szTemp1,"%02x",psBMBStatusRecord->bOpcode);
	memcpy(&BMBStatusTextRec.chOpcode,szTemp1,sizeof(BMBStatusTextRec.chOpcode));

	//set time in text version
	sprintf(szTemp1,"%10u",psBMBStatusRecord->uiJulianSeconds);
	memcpy(&BMBStatusTextRec.cdJulianSeconds,szTemp1,sizeof(BMBStatusTextRec.cdJulianSeconds));

	//set the 10ths in text version
	sprintf(szTemp1,"%d",psBMBStatusRecord->bJulianSec10ths);
	memcpy(&BMBStatusTextRec.cdJulianSec10ths,szTemp1,sizeof(BMBStatusTextRec.cdJulianSec10ths));

	//set charge current in text version
	sprintf(szTempCC,"%7.2f",psBMBStatusRecord->fChargeCurrent);
	memcpy(&BMBStatusTextRec.cdfChargeCurrent,szTempCC,sizeof(BMBStatusTextRec.cdfChargeCurrent));

	//set the discharge current in text version
	sprintf(szTempDC,"%7.2f",psBMBStatusRecord->fDischargeCurr);
	memcpy(&BMBStatusTextRec.cdfDischargeCurr,szTempDC,sizeof(BMBStatusTextRec.cdfDischargeCurr));

	//set the charge/discharge cycles
	sprintf(szTempCD,"%4u",psBMBStatusRecord->usChDisCycles);
	memcpy(&BMBStatusTextRec.cdChDisCycles,szTempCD,sizeof(BMBStatusTextRec.cdChDisCycles));

	//set the CPU Temperature
	sprintf(szTempCPUTemp,"%6.1f",psBMBStatusRecord->fCPUTemp);
	memcpy(&BMBStatusTextRec.cdfCPUTemp,szTempCPUTemp,sizeof(BMBStatusTextRec.cdfCPUTemp));

	//set the BOARD Temperature
	sprintf(szTempBOARDTemp,"%6.1f",psBMBStatusRecord->fBOARDTemp);
	memcpy(&BMBStatusTextRec.cdfBOARDTemp,szTempBOARDTemp,sizeof(BMBStatusTextRec.cdfBOARDTemp));

	//set the time on battery
	sprintf(szTempTimeOnBat,"%10u",psBMBStatusRecord->uiTimeOnBatt);
	memcpy(&BMBStatusTextRec.cdTimeOnBatt,szTempTimeOnBat,sizeof(BMBStatusTextRec.cdTimeOnBatt));

	//set the battery temperature
	sprintf(szTempBatTemp,"%6.1f",psBMBStatusRecord->fBattTemp);
	memcpy(&BMBStatusTextRec.cdfBattTemp,szTempBatTemp,sizeof(BMBStatusTextRec.cdfBattTemp));

	//set the battery status
	sprintf(szTempBatStat,"%02x",psBMBStatusRecord->bBattStatus);
	memcpy(&BMBStatusTextRec.chBattStatus,szTempBatStat,sizeof(BMBStatusTextRec.chBattStatus));

	//set the reserved
	sprintf(szTempReserved,"%08x",psBMBStatusRecord->uiReserved);
	memcpy(&BMBStatusTextRec.chReserved,szTempReserved,sizeof(BMBStatusTextRec.chReserved));

	//got a binary file so don't have a message checksum
	BMBStatusTextRec.chCheckSum[0] = '-';
	BMBStatusTextRec.chCheckSum[1] = '-';

	//set the cr and lf in the message
	BMBStatusTextRec.cCR = '\r';
	BMBStatusTextRec.cLF = '\n';

	//text version should be complete at this point

	//get the time
	CJulianTime cTime;
	cTime.Init(psBMBStatusRecord->uiJulianSeconds);

	bool bStale = true;

	//build the serial number and use it to find which column to put this data in
	sprintf(szTempSN,"%02x%02x%02x%02x%02x%02x",
		psBMBStatusRecord->bSerialNumber[0],
		psBMBStatusRecord->bSerialNumber[1],
		psBMBStatusRecord->bSerialNumber[2],
		psBMBStatusRecord->bSerialNumber[3],
		psBMBStatusRecord->bSerialNumber[4],
		psBMBStatusRecord->bSerialNumber[5]);
	memcpy(&BMBStatusTextRec.chSerialNumber,szTempSN,sizeof(BMBStatusTextRec.chSerialNumber));

	int iColumn = 0;
	if      (strncmp((char*)&BMBStatusTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1),sizeof(BMBStatusTextRec.chSerialNumber)) == 0) iColumn = 1;
	else if (strncmp((char*)&BMBStatusTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2),sizeof(BMBStatusTextRec.chSerialNumber)) == 0) iColumn = 2;
	else if (strncmp((char*)&BMBStatusTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3),sizeof(BMBStatusTextRec.chSerialNumber)) == 0) iColumn = 3;
	else if (strncmp((char*)&BMBStatusTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4),sizeof(BMBStatusTextRec.chSerialNumber)) == 0) iColumn = 4;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1))==0) iColumn = 5;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2))==0) iColumn = 6;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3))==0) iColumn = 7;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4))==0) iColumn = 8;

	if (m_duBMBStatusRecordTime < (cTime.TotalSeconds() + psBMBStatusRecord->bJulianSec10ths/10.0))
	{
		//newer than the last one received
		bStale = false;

		//save the time of this new record
		m_duBMBStatusRecordTime = cTime.TotalSeconds() + psBMBStatusRecord->bJulianSec10ths/10.0;

		switch (iColumn) {
		case 5:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,BMBStatusTextRec.chSerialNumber,sizeof(BMBStatusTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBSR_SERIALNUM1,"SET_BMBSR_SERIALNUM1",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 1:
			//set date and time
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATES1,cTime.Date());
			cTemp = cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMES1,cTemp);
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBSR_TIME1,"SET_BMBSR_TIME1",cTemp,false);

			//do the charge current
			if (!CurrentSetParam(SET_BMBSR_CHARGECURNT1,"SET_BMBSR_CHARGECURNT1",szTempCC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CHARGECNT1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the discharge current
			if (!CurrentSetParam(SET_BMBSR_DISCHGCURNT1,"SET_BMBSR_DISCHGCURNT1",szTempDC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_DISCHARGECNT1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the charge / discharge cycles
			if (!CurrentSetParam(SET_BMBSR_CDCYCLES1,"SET_BMBSR_CDCYCLES1",szTempCD,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CDCYCLES1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the CPU Temperature
			if (!CurrentSetParam(SET_BMBSR_CPUTEMP1,"SET_BMBSR_CPUTEMP1",szTempCPUTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the BOARD Temperature
			if (!CurrentSetParam(SET_BMBSR_BOARDTEMP1,"SET_BMBSR_BOARDTEMP1",szTempBOARDTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the time on battery
			if (!CurrentSetParam(SET_BMBSR_TIMEONBAT1,"SET_BMBSR_TIMEONBAT1",szTempTimeOnBat,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_TIMEONBAT1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the battery temperature
			if (!CurrentSetParam(SET_BMBSR_BATTTEMP1,"SET_BMBSR_BATTTEMP1",szTempBatTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_BATTEMP1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the battery status
			if (!CurrentSetParam(SET_BMBSR_BATTSTATUS1,"SET_BMBSR_BATTSTATUS1",szTempBatStat,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_BATSTAT1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the reserved
			if (!CurrentSetParam(SET_BMBSR_RESERVED1,"SET_BMBSR_RESERVED1",szTempReserved,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_RESERVE1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			break;

		case 6:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,BMBStatusTextRec.chSerialNumber,sizeof(BMBStatusTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBSR_SERIALNUM2,"SET_BMBSR_SERIALNUM2",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 2:
			//set date and time
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATES2,cTime.Date());
			cTemp = cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMES2,cTemp);
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBSR_TIME2,"SET_BMBSR_TIME2",cTemp,false);

			//do the charge current
			if (!CurrentSetParam(SET_BMBSR_CHARGECURNT2,"SET_BMBSR_CHARGECURNT2",szTempCC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CHARGECNT2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the discharge current
			if (!CurrentSetParam(SET_BMBSR_DISCHGCURNT2,"SET_BMBSR_DISCHGCURNT2",szTempDC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_DISCHARGECNT2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the charge / discharge cycles
			if (!CurrentSetParam(SET_BMBSR_CDCYCLES2,"SET_BMBSR_CDCYCLES2",szTempCD,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CDCYCLES2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the CPU Temperature
			if (!CurrentSetParam(SET_BMBSR_CPUTEMP2,"SET_BMBSR_CPUTEMP2",szTempCPUTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the BOARD Temperature
			if (!CurrentSetParam(SET_BMBSR_BOARDTEMP2,"SET_BMBSR_BOARDTEMP2",szTempBOARDTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the time on battery
			if (!CurrentSetParam(SET_BMBSR_TIMEONBAT2,"SET_BMBSR_TIMEONBAT2",szTempTimeOnBat,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_TIMEONBAT2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the battery temperature
			if (!CurrentSetParam(SET_BMBSR_BATTTEMP2,"SET_BMBSR_BATTTEMP2",szTempBatTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_BATTEMP2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the battery status
			if (!CurrentSetParam(SET_BMBSR_BATTSTATUS2,"SET_BMBSR_BATTSTATUS2",szTempBatStat,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_BATSTAT2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the reserved
			if (!CurrentSetParam(SET_BMBSR_RESERVED2,"SET_BMBSR_RESERVED2",szTempReserved,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_RESERVE2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;

		case 7:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,BMBStatusTextRec.chSerialNumber,sizeof(BMBStatusTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBSR_SERIALNUM3,"SET_BMBSR_SERIALNUM3",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 3:
			//set date and time
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATES3,cTime.Date());
			cTemp = cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMES3,cTemp);
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBSR_TIME3,"SET_BMBSR_TIME3",cTemp,false);

			//do the charge current
			if (!CurrentSetParam(SET_BMBSR_CHARGECURNT3,"SET_BMBSR_CHARGECURNT3",szTempCC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CHARGECNT3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the discharge current
			if (!CurrentSetParam(SET_BMBSR_DISCHGCURNT3,"SET_BMBSR_DISCHGCURNT3",szTempDC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_DISCHARGECNT3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the charge / discharge cycles
			if (!CurrentSetParam(SET_BMBSR_CDCYCLES3,"SET_BMBSR_CDCYCLES3",szTempCD,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CDCYCLES3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the CPU Temperature
			if (!CurrentSetParam(SET_BMBSR_CPUTEMP3,"SET_BMBSR_CPUTEMP3",szTempCPUTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the BOARD Temperature
			if (!CurrentSetParam(SET_BMBSR_BOARDTEMP3,"SET_BMBSR_BOARDTEMP3",szTempBOARDTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the time on battery
			if (!CurrentSetParam(SET_BMBSR_TIMEONBAT3,"SET_BMBSR_TIMEONBAT3",szTempTimeOnBat,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_TIMEONBAT3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the battery temperature
			if (!CurrentSetParam(SET_BMBSR_BATTTEMP3,"SET_BMBSR_BATTTEMP3",szTempBatTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_BATTEMP3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the battery status
			if (!CurrentSetParam(SET_BMBSR_BATTSTATUS3,"SET_BMBSR_BATTSTATUS3",szTempBatStat,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_BATSTAT3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the reserved
			if (!CurrentSetParam(SET_BMBSR_RESERVED3,"SET_BMBSR_RESERVED3",szTempReserved,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_RESERVE3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			break;

		case 8:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,BMBStatusTextRec.chSerialNumber,sizeof(BMBStatusTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBSR_SERIALNUM4,"SET_BMBSR_SERIALNUM4",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 4:
			//set date and time
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATES4,cTime.Date());
			cTemp = cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMES4,cTemp);
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBSR_TIME4,"SET_BMBSR_TIME4",cTemp,false);

			//do the charge current
			if (!CurrentSetParam(SET_BMBSR_CHARGECURNT4,"SET_BMBSR_CHARGECURNT4",szTempCC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CHARGECNT4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the discharge current
			if (!CurrentSetParam(SET_BMBSR_DISCHGCURNT4,"SET_BMBSR_DISCHGCURNT4",szTempDC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_DISCHARGECNT4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the charge / discharge cycles
			if (!CurrentSetParam(SET_BMBSR_CDCYCLES4,"SET_BMBSR_CDCYCLES4",szTempCD,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CDCYCLES4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the CPU Temperature
			if (!CurrentSetParam(SET_BMBSR_CPUTEMP4,"SET_BMBSR_CPUTEMP4",szTempCPUTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the BOARD Temperature
			if (!CurrentSetParam(SET_BMBSR_BOARDTEMP4,"SET_BMBSR_BOARDTEMP4",szTempBOARDTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the time on battery
			if (!CurrentSetParam(SET_BMBSR_TIMEONBAT4,"SET_BMBSR_TIMEONBAT4",szTempTimeOnBat,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_TIMEONBAT4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the battery temperature
			if (!CurrentSetParam(SET_BMBSR_BATTTEMP4,"SET_BMBSR_BATTTEMP4",szTempBatTemp,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_BATTEMP4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the battery status
			if (!CurrentSetParam(SET_BMBSR_BATTSTATUS4,"SET_BMBSR_BATTSTATUS4",szTempBatStat,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_BATSTAT4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}

			//do the reserved
			if (!CurrentSetParam(SET_BMBSR_RESERVED4,"SET_BMBSR_RESERVED4",szTempReserved,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_RESERVE4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;

		}
	}

	//send it off to be displayed if possible
	//and save it for later
	char cBuff[sizeof(BMBStatusTextRecord)+7];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const  char*)&BMBStatusTextRec,sizeof(BMBStatusTextRecord));
	cBuff[sizeof(BMBStatusTextRecord) + 6] = NULL;

	if (!bStale)
	{
		switch (iColumn) {
		case 1:
		case 5:
			m_cBMBStatusRecord1 = cBuff+6;
			break;
		case 2:
		case 6:
			m_cBMBStatusRecord2 = cBuff+6;
			break;
		case 3:
		case 7:
			m_cBMBStatusRecord3 = cBuff+6;
			break;
		case 4:
		case 8:
			m_cBMBStatusRecord4 = cBuff+6;
			break;
		}
		if (m_pPropertySheet)
			m_pPropertySheet->OnBMBStatusRecord(cBuff+6);
	}

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)psBMBStatusRecord);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,BMBSTATUSRECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(BMBStatusRecord	);
}

int CInstrument::OnBinaryuGInfoRecord(BYTE *pBinaryAt)
{
	//some temporary buffers
	char szTemp1[32];
	CString cTemp;

	//get a text version of the record
	uGInfoTextRecord uGInfoTextRec;
	memset(&uGInfoTextRec,NULL, sizeof(uGInfoTextRecord));

	//get the address of a binary record
	uGInfoRecord* psuGInfoRecord = (uGInfoRecord*)pBinaryAt;

	//convert big endian to little endian 
	Reverse((BYTE*)&(psuGInfoRecord->uiJulianSeconds),sizeof(psuGInfoRecord->uiJulianSeconds));
	Reverse((BYTE*)&(psuGInfoRecord->usFirmwareChkSum),sizeof(psuGInfoRecord->usFirmwareChkSum));
	Reverse((BYTE*)&(psuGInfoRecord->usActelVersion),sizeof(psuGInfoRecord->usActelVersion));

	//only do this if binary record is going to be 
	//entered into the binary file
//	memcpy(&(RecordX[m_dRecordCountX]),psuGInfoRecord,sizeof(psuGInfoRecord));

	//set opcode in text version
	sprintf(szTemp1,"%02x",psuGInfoRecord->bOpcode);
	memcpy(&uGInfoTextRec.chOpcode,szTemp1,sizeof(uGInfoTextRec.chOpcode));

	//set time in text version
	sprintf(szTemp1,"%10u",psuGInfoRecord->uiJulianSeconds);
	memcpy(&uGInfoTextRec.cdJulianSeconds,szTemp1,sizeof(uGInfoTextRec.cdJulianSeconds));

	CJulianTime cTime;
	cTime.Init(psuGInfoRecord->uiJulianSeconds);

	bool bStale = true;

	if (m_uiuGInfoRecordTime < cTime.TotalSeconds())
	{
		//newer than the last one received
		bStale = false;

		//save the time of this new record
		m_uiuGInfoRecordTime = cTime.TotalSeconds();

		//set date and time
		SetString(UGINFOPAGE,IDC_UGINFO_DATADATEI,cTime.Date());
		cTemp = cTime.Time();
		cTemp += ".";
		cTemp += szTemp1;
		SetString(UGINFOPAGE,IDC_UGINFO_DATATIMEI,cTemp);
		cTemp = cTime.Date();
		cTemp += " ";
		cTemp += cTime.Time();
		cTemp += ".";
		cTemp += szTemp1;
		CurrentSetParam(SET_UGIR_TIME,"SET_UGIR_TIME",cTemp,false);
	}

	//set actel version
	sprintf(szTemp1,"%04x",psuGInfoRecord->usActelVersion);
	memcpy(uGInfoTextRec.chActelVersion,szTemp1,sizeof(uGInfoTextRec.chActelVersion));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_ACTELVERI,szTemp1);
		if (!CurrentSetParam(SET_UGIR_ACTELVER,"SET_UGIR_ACTELVER",szTemp1,true))
		{
			SetColor(UGINFOPAGE,IDC_UGINFO_ACTELVERI,COLOR_RED);
			m_iUGInfo_Tab |= 0x0001;
		}
	}

	//set firmware version
	sprintf(szTemp1,"%02d.%02d.%02d",
		psuGInfoRecord->b3dFirmwareVersion[0],
		psuGInfoRecord->b3dFirmwareVersion[1],
		psuGInfoRecord->b3dFirmwareVersion[2]);
	memcpy(&uGInfoTextRec.c3dFirmwareVersion,szTemp1,sizeof(uGInfoTextRec.c3dFirmwareVersion));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_FIRMWVI,szTemp1);
		if (!CurrentSetParam(SET_UGIR_FIRMWAREVER,"SET_UGIR_FIRMWAREVER",szTemp1,true))
		{
			SetColor(UGINFOPAGE,IDC_UGINFO_FIRMWVI,COLOR_RED);
			m_iUGInfo_Tab |= 0x0001;
		}
	}

	//set firmware checksum
	sprintf(szTemp1,"%04x",psuGInfoRecord->usFirmwareChkSum);
	memcpy(&uGInfoTextRec.cdFirmwareChkSum,szTemp1,sizeof(uGInfoTextRec.cdFirmwareChkSum));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_FIRMWCHKI,szTemp1);
		if (!CurrentSetParam(SET_UGIR_FIRMWARECKSUM,"SET_UGIR_FIRMWARECKSUM",szTemp1,true))
		{
			SetColor(UGINFOPAGE,IDC_UGINFO_FIRMWCHKI,COLOR_RED);
			m_iUGInfo_Tab |= 0x0001;
		}
	}
	
	//build the serial number 
	sprintf(szTemp1,"%02x%02x%02x%02x%02x%02x",
		psuGInfoRecord->bSerialNumber[0],
		psuGInfoRecord->bSerialNumber[1],
		psuGInfoRecord->bSerialNumber[2],
		psuGInfoRecord->bSerialNumber[3],
		psuGInfoRecord->bSerialNumber[4],
		psuGInfoRecord->bSerialNumber[5]);
	memcpy(&uGInfoTextRec.chSerialNumber,szTemp1,sizeof(uGInfoTextRec.chSerialNumber));
	if (!bStale)
	{
		SetString(UGINFOPAGE,IDC_UGINFO_UGRANDSNI,szTemp1);
		if (!CurrentSetParam(SET_UGIR_SERIALNUM,"SET_UGIR_SERIALNUM",szTemp1,true))
		{
			SetColor(UGINFOPAGE,IDC_UGINFO_UGRANDSNI,COLOR_RED);
			m_iUGInfo_Tab |= 0x0001;
		}
	}

	//got a binary file so don't have a message checksum
	uGInfoTextRec.chCheckSum[0] = '-';
	uGInfoTextRec.chCheckSum[1] = '-';

	//set the cr and lf in the message
	uGInfoTextRec.cCR = '\r';
	uGInfoTextRec.cLF = '\n';

	//send it off to be displayed if possible
	//and save it for later
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&uGInfoTextRec,sizeof(uGInfoTextRec));
	cBuff[sizeof(uGInfoTextRec) + 6] = NULL;

	if (!bStale)
	{
		//save the text version to send to the display
		m_cUGInfoRecord = cBuff+6;
		//send to the display now if we can
		if (m_pPropertySheet)
			m_pPropertySheet->OnuGInfoRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)psuGInfoRecord);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,UGINFORECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(uGInfoRecord		);
}

int CInstrument::OnBinaryESBInfoRecord(BYTE *pBinaryAt)
{
	//some temporary buffers
	char szTemp1[32];
	char szTempFV[32];
	char szTempFC[32];
	char szTempSN[32];
	CString cTemp;

	//Get a text version of the record
	ESBInfoTextRecord ESBInfoTextRec;
	memset(&ESBInfoTextRec,NULL, sizeof(ESBInfoTextRecord));

	//get the address of a binary record
	ESBInfoRecord* psESBInfoRecord = (ESBInfoRecord*)pBinaryAt;

	//convert big endian to little endian 
	Reverse((BYTE*)&(psESBInfoRecord->uiJulianSeconds),sizeof(psESBInfoRecord->uiJulianSeconds));
	Reverse((BYTE*)&(psESBInfoRecord->usFirmwareChkSum),sizeof(psESBInfoRecord->usFirmwareChkSum));

	//only do this if binary record is going to be 
	//entered into the binary file
	memcpy(&(RecordX[m_dRecordCountX]),psESBInfoRecord,sizeof(ESBInfoRecord));

	//set opcode in text version
	sprintf(szTemp1,"%02x",psESBInfoRecord->bOpcode);
	memcpy(&ESBInfoTextRec.chOpcode,szTemp1,sizeof(ESBInfoTextRec.chOpcode));

	//set time in text version
	sprintf(szTemp1,"%10u",psESBInfoRecord->uiJulianSeconds);
	memcpy(&ESBInfoTextRec.cdJulianSeconds,szTemp1,sizeof(ESBInfoTextRec.cdJulianSeconds));

	CJulianTime cTime;
	cTime.Init(psESBInfoRecord->uiJulianSeconds);

	bool bStale = true;

	//build the serial number and use it to find which column to put this data in
	sprintf(szTempSN,"%02x%02x%02x%02x%02x%02x",
		psESBInfoRecord->bSerialNumber[0],
		psESBInfoRecord->bSerialNumber[1],
		psESBInfoRecord->bSerialNumber[2],
		psESBInfoRecord->bSerialNumber[3],
		psESBInfoRecord->bSerialNumber[4],
		psESBInfoRecord->bSerialNumber[5]);
	memcpy(&ESBInfoTextRec.chSerialNumber,szTempSN,sizeof(ESBInfoTextRec.chSerialNumber));

	int iColumn = 0;
	if      (strncmp((char*)&ESBInfoTextRec.chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1),sizeof(ESBInfoTextRec.chSerialNumber)) == 0) iColumn = 1;
	else if (strncmp((char*)&ESBInfoTextRec.chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2),sizeof(ESBInfoTextRec.chSerialNumber)) == 0) iColumn = 2;
	else if (strncmp((char*)&ESBInfoTextRec.chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3),sizeof(ESBInfoTextRec.chSerialNumber)) == 0) iColumn = 3;
	else if (strncmp((char*)&ESBInfoTextRec.chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4),sizeof(ESBInfoTextRec.chSerialNumber)) == 0) iColumn = 4;
	else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1))==0) iColumn = 5;
	else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2))==0) iColumn = 6;
	else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3))==0) iColumn = 7;
	else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4))==0) iColumn = 8;

	//get the firmware version
	sprintf(szTempFV,"%02d.%02d.%02d",
		psESBInfoRecord->b3dFirmwareVersion[0],
		psESBInfoRecord->b3dFirmwareVersion[1],
		psESBInfoRecord->b3dFirmwareVersion[2]);
	memcpy(&ESBInfoTextRec.c3dFirmwareVersion,szTempFV,sizeof(ESBInfoTextRec.c3dFirmwareVersion));

	//get the firmware checksum
	sprintf(szTempFC,"%04x",psESBInfoRecord->usFirmwareChkSum);
	memcpy(&ESBInfoTextRec.cdFirmwareChkSum,szTempFC,sizeof(ESBInfoTextRec.cdFirmwareChkSum));

	if (m_uiESBInfoRecordTime < cTime.TotalSeconds())
	{
		//newer than the last one received
		bStale = false;

		//save the time of this new record
		m_uiuGInfoRecordTime = cTime.TotalSeconds();

		//set date and time
		switch (iColumn) {
		case 5:
			SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1,ESBInfoTextRec.chSerialNumber,sizeof(ESBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_ESBIR_SERIALNUM1,"SET_ESBIR_SERIALNUM1",szTempSN,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL1,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 1:
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEI1,cTime.Date());
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEI1,cTime.Time());
			SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV1,ESBInfoTextRec.c3dFirmwareVersion,sizeof(ESBInfoTextRec.c3dFirmwareVersion));
			SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC1,ESBInfoTextRec.cdFirmwareChkSum,sizeof(ESBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_ESBIR_TIME1,"SET_ESBIR_TIME1",cTemp,false);
			if (!CurrentSetParam(SET_ESBIR_FIRMWAREVER1,"SET_ESBIR_FIRMWAREVER1",szTempFV,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV1,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_ESBIR_FIRMWARECKSUM1,"SET_ESBIR_FIRMWARECKSUM1",szTempFC,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC1,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			break;
		case 6:
			SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2,ESBInfoTextRec.chSerialNumber,sizeof(ESBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_ESBIR_SERIALNUM2,"SET_ESBIR_SERIALNUM2",szTempSN,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL2,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 2:
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEI2,cTime.Date());
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEI2,cTime.Time());
			SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV2,ESBInfoTextRec.c3dFirmwareVersion,sizeof(ESBInfoTextRec.c3dFirmwareVersion));
			SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC2,ESBInfoTextRec.cdFirmwareChkSum,sizeof(ESBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_ESBIR_TIME2,"SET_ESBIR_TIME2",cTemp,false);
			if (!CurrentSetParam(SET_ESBIR_FIRMWAREVER2,"SET_ESBIR_FIRMWAREVER2",szTempFV,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV2,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_ESBIR_FIRMWARECKSUM2,"SET_ESBIR_FIRMWARECKSUM2",szTempFC,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC2,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			break;
		case 7:
			SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3,ESBInfoTextRec.chSerialNumber,sizeof(ESBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_ESBIR_SERIALNUM3,"SET_ESBIR_SERIALNUM3",szTempSN,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL3,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 3:
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEI3,cTime.Date());
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEI3,cTime.Time());
			SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV3,ESBInfoTextRec.c3dFirmwareVersion,sizeof(ESBInfoTextRec.c3dFirmwareVersion));
			SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC3,ESBInfoTextRec.cdFirmwareChkSum,sizeof(ESBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_ESBIR_TIME3,"SET_ESBIR_TIME3",cTemp,false);
			if (!CurrentSetParam(SET_ESBIR_FIRMWAREVER3,"SET_ESBIR_FIRMWAREVER3",szTempFV,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV3,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_ESBIR_FIRMWARECKSUM3,"SET_ESBIR_FIRMWARECKSUM3",szTempFC,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC3,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			break;
		case 8:
			SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4,ESBInfoTextRec.chSerialNumber,sizeof(ESBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_ESBIR_SERIALNUM4,"SET_ESBIR_SERIALNUM4",szTempSN,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL4,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 4:
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEI4,cTime.Date());
			SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEI4,cTime.Time());
			SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV4,ESBInfoTextRec.c3dFirmwareVersion,sizeof(ESBInfoTextRec.c3dFirmwareVersion));
			SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC4,ESBInfoTextRec.cdFirmwareChkSum,sizeof(ESBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_ESBIR_TIME4,"SET_ESBIR_TIME4",cTemp,false);
			if (!CurrentSetParam(SET_ESBIR_FIRMWAREVER4,"SET_ESBIR_FIRMWAREVER4",szTempFV,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV4,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_ESBIR_FIRMWARECKSUM4,"SET_ESBIR_FIRMWARECKSUM4",szTempFC,false))
			{
				SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC4,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			break;
		}
	}

	//got a binary file so don't have a message checksum
	ESBInfoTextRec.chCheckSum[0] = '-';
	ESBInfoTextRec.chCheckSum[1] = '-';

	//set the cr and lf in the message
	ESBInfoTextRec.cCR = '\r';
	ESBInfoTextRec.cLF = '\n';

	//send it off to be displayed if possible
	//and save it for later
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&ESBInfoTextRec,sizeof(ESBInfoTextRec));
	cBuff[sizeof(ESBInfoTextRec) + 6] = NULL;

	if (!bStale)
	{
		//save the text version to send to the display
		switch (iColumn)
		{
		case 5:
		case 1:
			m_cESBInfoRecord1 = cBuff+6;
			break;
		case 6:
		case 2:
			m_cESBInfoRecord2 = cBuff+6;
			break;
		case 7:
		case 3:
			m_cESBInfoRecord3 = cBuff+6;
			break;
		case 8:
		case 4:
			m_cESBInfoRecord4 = cBuff+6;
			break;
		}
		
		//send to the display now if we can
		if (m_pPropertySheet)
			m_pPropertySheet->OnESBInfoRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)psESBInfoRecord);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,ESBINFORECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(ESBInfoRecord	);
}


int CInstrument::OnBinaryPSBInfoRecord(BYTE *pBinaryAt)
{
	//some temporary buffers
	char szTemp1[32];
	char szTempFV[32];
	char szTempFC[32];
	char szTempSN[32];
	CString cTemp;

	//Get a text version of the record
	PSBInfoTextRecord PSBInfoTextRec;
	memset(&PSBInfoTextRec,NULL, sizeof(PSBInfoTextRecord));

	//get the address of a binary record
	PSBInfoRecord* psPSBInfoRecord = (PSBInfoRecord*)pBinaryAt;

	//convert big endian to little endian 
	Reverse((BYTE*)&(psPSBInfoRecord->uiJulianSeconds),sizeof(psPSBInfoRecord->uiJulianSeconds));
	Reverse((BYTE*)&(psPSBInfoRecord->usFirmwareChkSum),sizeof(psPSBInfoRecord->usFirmwareChkSum));

	//only do this if binary record is going to be 
	//entered into the binary file
	memcpy(&(RecordX[m_dRecordCountX]),psPSBInfoRecord,sizeof(PSBInfoRecord));

	//set opcode in text version
	sprintf(szTemp1,"%02x",psPSBInfoRecord->bOpcode);
	memcpy(&PSBInfoTextRec.chOpcode,szTemp1,sizeof(PSBInfoTextRec.chOpcode));

	//set time in text version
	sprintf(szTemp1,"%10u",psPSBInfoRecord->uiJulianSeconds);
	memcpy(&PSBInfoTextRec.cdJulianSeconds,szTemp1,sizeof(PSBInfoTextRec.cdJulianSeconds));

	CJulianTime cTime;
	cTime.Init(psPSBInfoRecord->uiJulianSeconds);

	bool bStale = true;

	//build the serial number and use it to find which column to put this data in
	sprintf(szTempSN,"%02x%02x%02x%02x%02x%02x",
		psPSBInfoRecord->bSerialNumber[0],
		psPSBInfoRecord->bSerialNumber[1],
		psPSBInfoRecord->bSerialNumber[2],
		psPSBInfoRecord->bSerialNumber[3],
		psPSBInfoRecord->bSerialNumber[4],
		psPSBInfoRecord->bSerialNumber[5]);
	memcpy(&PSBInfoTextRec.chSerialNumber,szTempSN,sizeof(PSBInfoTextRec.chSerialNumber));

	int iColumn = 0;
	if      (strncmp((char*)&PSBInfoTextRec.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1),sizeof(PSBInfoTextRec.chSerialNumber)) == 0) iColumn = 1;
	else if (strncmp((char*)&PSBInfoTextRec.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2),sizeof(PSBInfoTextRec.chSerialNumber)) == 0) iColumn = 2;
	else if (strncmp((char*)&PSBInfoTextRec.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3),sizeof(PSBInfoTextRec.chSerialNumber)) == 0) iColumn = 3;
	else if (strncmp((char*)&PSBInfoTextRec.chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4),sizeof(PSBInfoTextRec.chSerialNumber)) == 0) iColumn = 4;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1))==0) iColumn = 5;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2))==0) iColumn = 6;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3))==0) iColumn = 7;
	else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4))==0) iColumn = 8;

	//get the firmware version
	sprintf(szTempFV,"%02d.%02d.%02d",
		psPSBInfoRecord->b3dFirmwareVersion[0],
		psPSBInfoRecord->b3dFirmwareVersion[1],
		psPSBInfoRecord->b3dFirmwareVersion[2]);
	memcpy(&PSBInfoTextRec.c3dFirmwareVersion,szTempFV,sizeof(PSBInfoTextRec.c3dFirmwareVersion));

	//get the firmware checksum
	sprintf(szTempFC,"%04x",psPSBInfoRecord->usFirmwareChkSum);
	memcpy(&PSBInfoTextRec.cdFirmwareChkSum,szTempFC,sizeof(PSBInfoTextRec.cdFirmwareChkSum));

	if (m_uiPSBInfoRecordTime < cTime.TotalSeconds())
	{
		//newer than the last one received
		bStale = false;

		//save the time of this new record
		m_uiPSBInfoRecordTime = cTime.TotalSeconds();

		//set date and time
		switch (iColumn) {
		case 5:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,PSBInfoTextRec.chSerialNumber,sizeof(PSBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM1,"SET_PSBIR_SERIALNUM1",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 1:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEI1,cTime.Date());
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEI1,cTime.Time());
			SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV1,PSBInfoTextRec.c3dFirmwareVersion,sizeof(PSBInfoTextRec.c3dFirmwareVersion));
			SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC1,PSBInfoTextRec.cdFirmwareChkSum,sizeof(PSBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_PSBIR_TIME1,"SET_PSBIR_TIME1",cTemp,false);
			if (!CurrentSetParam(SET_PSBIR_FIRMWAREVER1,"SET_PSBIR_FIRMWAREVER1",szTempFV,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_PSBIR_FIRMWARECKSUM1,"SET_PSBIR_FIRMWARECKSUM1",szTempFC,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC1,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;
		case 6:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,PSBInfoTextRec.chSerialNumber,sizeof(PSBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM2,"SET_PSBIR_SERIALNUM2",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 2:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEI2,cTime.Date());
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEI2,cTime.Time());
			SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV2,PSBInfoTextRec.c3dFirmwareVersion,sizeof(PSBInfoTextRec.c3dFirmwareVersion));
			SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC2,PSBInfoTextRec.cdFirmwareChkSum,sizeof(PSBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_PSBIR_TIME2,"SET_PSBIR_TIME2",cTemp,false);
			if (!CurrentSetParam(SET_PSBIR_FIRMWAREVER2,"SET_PSBIR_FIRMWAREVER2",szTempFV,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_PSBIR_FIRMWARECKSUM2,"SET_PSBIR_FIRMWARECKSUM2",szTempFC,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC2,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;
		case 7:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,PSBInfoTextRec.chSerialNumber,sizeof(PSBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM3,"SET_PSBIR_SERIALNUM3",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 3:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEI3,cTime.Date());
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEI3,cTime.Time());
			SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV3,PSBInfoTextRec.c3dFirmwareVersion,sizeof(PSBInfoTextRec.c3dFirmwareVersion));
			SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC3,PSBInfoTextRec.cdFirmwareChkSum,sizeof(PSBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_PSBIR_TIME3,"SET_PSBIR_TIME3",cTemp,false);
			if (!CurrentSetParam(SET_PSBIR_FIRMWAREVER3,"SET_PSBIR_FIRMWAREVER3",szTempFV,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_PSBIR_FIRMWARECKSUM3,"SET_PSBIR_FIRMWARECKSUM3",szTempFC,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC3,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;
		case 8:
			SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,PSBInfoTextRec.chSerialNumber,sizeof(PSBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_PSBIR_SERIALNUM4,"SET_PSBIR_SERIALNUM4",szTempSN,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 4:
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEI4,cTime.Date());
			SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEI4,cTime.Time());
			SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV4,PSBInfoTextRec.c3dFirmwareVersion,sizeof(PSBInfoTextRec.c3dFirmwareVersion));
			SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC4,PSBInfoTextRec.cdFirmwareChkSum,sizeof(PSBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_PSBIR_TIME4,"SET_PSBIR_TIME4",cTemp,false);
			if (!CurrentSetParam(SET_PSBIR_FIRMWAREVER4,"SET_PSBIR_FIRMWAREVER4",szTempFV,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_PSBIR_FIRMWARECKSUM4,"SET_PSBIR_FIRMWARECKSUM4",szTempFC,false))
			{
				SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC4,COLOR_YELLOW);
				m_iPSBInfo_Tab |= 0x0002;
			}
			break;
		}
	}

	//got a binary file so don't have a message checksum
	PSBInfoTextRec.chCheckSum[0] = '-';
	PSBInfoTextRec.chCheckSum[1] = '-';

	//set the cr and lf in the message
	PSBInfoTextRec.cCR = '\r';
	PSBInfoTextRec.cLF = '\n';

	//send it off to be displayed if possible
	//and save it for later
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&PSBInfoTextRec,sizeof(PSBInfoTextRec));
	cBuff[sizeof(PSBInfoTextRec) + 6] = NULL;

	if (!bStale)
	{
		//save the text version to send to the display
		switch (iColumn)
		{
		case 5:
		case 1:
			m_cPSBInfoRecord1 = cBuff+6;
			break;
		case 6:
		case 2:
			m_cPSBInfoRecord2 = cBuff+6;
			break;
		case 7:
		case 3:
			m_cPSBInfoRecord3 = cBuff+6;
			break;
		case 8:
		case 4:
			m_cPSBInfoRecord4 = cBuff+6;
			break;
		}
		
		//send to the display now if we can
		if (m_pPropertySheet)
			m_pPropertySheet->OnPSBInfoRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)psPSBInfoRecord);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,PSBINFORECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(PSBInfoRecord	);
}

int CInstrument::OnBinaryBMBInfoRecord(BYTE *pBinaryAt)
{
	//some temporary buffers
	char szTemp1[32];
	char szTempFV[32];
	char szTempFC[32];
	char szTempSN[32];
	CString cTemp;

	//Get a text version of the record
	BMBInfoTextRecord BMBInfoTextRec;
	memset(&BMBInfoTextRec,NULL, sizeof(BMBInfoTextRecord));

	//get the address of a binary record
	BMBInfoRecord* psBMBInfoRecord = (BMBInfoRecord*)pBinaryAt;

	//convert big endian to little endian 
	Reverse((BYTE*)&(psBMBInfoRecord->uiJulianSeconds),sizeof(psBMBInfoRecord->uiJulianSeconds));
	Reverse((BYTE*)&(psBMBInfoRecord->usFirmwareChkSum),sizeof(psBMBInfoRecord->usFirmwareChkSum));

	//only do this if binary record is going to be 
	//entered into the binary file
	memcpy(&(RecordX[m_dRecordCountX]),psBMBInfoRecord,sizeof(BMBInfoRecord));

	//set opcode in text version
	sprintf(szTemp1,"%02x",psBMBInfoRecord->bOpcode);
	memcpy(&BMBInfoTextRec.chOpcode,szTemp1,sizeof(BMBInfoTextRec.chOpcode));

	//set time in text version
	sprintf(szTemp1,"%10u",psBMBInfoRecord->uiJulianSeconds);
	memcpy(&BMBInfoTextRec.cdJulianSeconds,szTemp1,sizeof(BMBInfoTextRec.cdJulianSeconds));

	CJulianTime cTime;
	cTime.Init(psBMBInfoRecord->uiJulianSeconds);

	bool bStale = true;

	//build the serial number and use it to find which column to put this data in
	sprintf(szTempSN,"%02x%02x%02x%02x%02x%02x",
		psBMBInfoRecord->bSerialNumber[0],
		psBMBInfoRecord->bSerialNumber[1],
		psBMBInfoRecord->bSerialNumber[2],
		psBMBInfoRecord->bSerialNumber[3],
		psBMBInfoRecord->bSerialNumber[4],
		psBMBInfoRecord->bSerialNumber[5]);
	memcpy(&BMBInfoTextRec.chSerialNumber,szTempSN,sizeof(BMBInfoTextRec.chSerialNumber));

	int iColumn = 0;
	if      (strncmp((char*)&BMBInfoTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1),sizeof(BMBInfoTextRec.chSerialNumber)) == 0) iColumn = 1;
	else if (strncmp((char*)&BMBInfoTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2),sizeof(BMBInfoTextRec.chSerialNumber)) == 0) iColumn = 2;
	else if (strncmp((char*)&BMBInfoTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3),sizeof(BMBInfoTextRec.chSerialNumber)) == 0) iColumn = 3;
	else if (strncmp((char*)&BMBInfoTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4),sizeof(BMBInfoTextRec.chSerialNumber)) == 0) iColumn = 4;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1))==0) iColumn = 5;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2))==0) iColumn = 6;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3))==0) iColumn = 7;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4))==0) iColumn = 8;

	//get the firmware version
	sprintf(szTempFV,"%02d.%02d.%02d",
		psBMBInfoRecord->b3dFirmwareVersion[0],
		psBMBInfoRecord->b3dFirmwareVersion[1],
		psBMBInfoRecord->b3dFirmwareVersion[2]);
	memcpy(&BMBInfoTextRec.c3dFirmwareVersion,szTempFV,sizeof(BMBInfoTextRec.c3dFirmwareVersion));

	//get the firmware checksum
	sprintf(szTempFC,"%04x",psBMBInfoRecord->usFirmwareChkSum);
	memcpy(&BMBInfoTextRec.cdFirmwareChkSum,szTempFC,sizeof(BMBInfoTextRec.cdFirmwareChkSum));

	if (m_uiBMBInfoRecordTime < cTime.TotalSeconds())
	{
		//newer than the last one received
		bStale = false;

		//save the time of this new record
		m_uiBMBInfoRecordTime = cTime.TotalSeconds();

		//set date and time
		switch (iColumn) {
		case 5:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,BMBInfoTextRec.chSerialNumber,sizeof(BMBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM1,"SET_BMBIR_SERIALNUM1",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 1:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI1,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI1,cTime.Time());
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV1,BMBInfoTextRec.c3dFirmwareVersion,sizeof(BMBInfoTextRec.c3dFirmwareVersion));
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC1,BMBInfoTextRec.cdFirmwareChkSum,sizeof(BMBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBIR_TIME1,"SET_BMBIR_TIME1",cTemp,false);
			if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER1,"SET_BMBIR_FIRMWAREVER1",szTempFV,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM1,"SET_BMBIR_FIRMWARECKSUM1",szTempFC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;
		case 6:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,BMBInfoTextRec.chSerialNumber,sizeof(BMBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM2,"SET_BMBIR_SERIALNUM2",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 2:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI2,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI2,cTime.Time());
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV2,BMBInfoTextRec.c3dFirmwareVersion,sizeof(BMBInfoTextRec.c3dFirmwareVersion));
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC2,BMBInfoTextRec.cdFirmwareChkSum,sizeof(BMBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBIR_TIME2,"SET_BMBIR_TIME2",cTemp,false);
			if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER2,"SET_BMBIR_FIRMWAREVER2",szTempFV,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM2,"SET_BMBIR_FIRMWARECKSUM2",szTempFC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;
		case 7:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,BMBInfoTextRec.chSerialNumber,sizeof(BMBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM3,"SET_BMBIR_SERIALNUM3",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 3:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI3,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI3,cTime.Time());
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV3,BMBInfoTextRec.c3dFirmwareVersion,sizeof(BMBInfoTextRec.c3dFirmwareVersion));
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC3,BMBInfoTextRec.cdFirmwareChkSum,sizeof(BMBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBIR_TIME3,"SET_BMBIR_TIME3",cTemp,false);
			if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER3,"SET_BMBIR_FIRMWAREVER3",szTempFV,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM3,"SET_BMBIR_FIRMWARECKSUM3",szTempFC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;
		case 8:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,BMBInfoTextRec.chSerialNumber,sizeof(BMBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM4,"SET_BMBIR_SERIALNUM4",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 4:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI4,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI4,cTime.Time());
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV4,BMBInfoTextRec.c3dFirmwareVersion,sizeof(BMBInfoTextRec.c3dFirmwareVersion));
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC4,BMBInfoTextRec.cdFirmwareChkSum,sizeof(BMBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBIR_TIME4,"SET_BMBIR_TIME4",cTemp,false);
			if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER4,"SET_BMBIR_FIRMWAREVER4",szTempFV,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM4,"SET_BMBIR_FIRMWARECKSUM4",szTempFC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;
		}
	}

	//got a binary file so don't have a message checksum
	BMBInfoTextRec.chCheckSum[0] = '-';
	BMBInfoTextRec.chCheckSum[1] = '-';

	//set the cr and lf in the message
	BMBInfoTextRec.cCR = '\r';
	BMBInfoTextRec.cLF = '\n';

	//send it off to be displayed if possible
	//and save it for later
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&BMBInfoTextRec,sizeof(BMBInfoTextRec));
	cBuff[sizeof(BMBInfoTextRec) + 6] = NULL;

	if (!bStale)
	{
		//save the text version to send to the display
		switch (iColumn)
		{
		case 1:
		case 5:
			m_cBMBInfoRecord1 = cBuff+6;
			break;
		case 2:
		case 6:
			m_cBMBInfoRecord2 = cBuff+6;
			break;
		case 3:
		case 7:
			m_cBMBInfoRecord3 = cBuff+6;
			break;
		case 4:
		case 8:
			m_cBMBInfoRecord4 = cBuff+6;
			break;
		}
		
		//send to the display now if we can
		if (m_pPropertySheet)
			m_pPropertySheet->OnBMBInfoRecord(cBuff+6);
	}

	PostToWatch(cBuff+6);
	UpdateSummary(cBuff+6);

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)psBMBInfoRecord);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,BMBINFORECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(BMBInfoRecord	);
}

//record 50
void 
CInstrument::OnuGConfigRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(uGConfigTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//will need a few work areas
		CString cTemp;
		char szTemp1[64];
		char szTemp2[256];
		char szTemp3[16];
		int iTemp;
		char chTemp, chTemp1, chTemp2;
		unsigned short usTemp;

		//do we have a change
		bool bChange = false;

		//create a binary record to send to the SOH file
		uGConfigRecord uGConfigRec;

		//get a pointer to a GeneralConfigTextRecord
		uGConfigTextRecord* pszRecord = (uGConfigTextRecord*)szRecord;

		//get length and opcode and place in binary record
		uGConfigRec.bLength = sizeof(uGConfigRecord);
		strncpy(szTemp1,(char*)pszRecord->chOpcode,sizeof(pszRecord->chOpcode));
		szTemp1[sizeof(pszRecord->chOpcode)] = NULL;
		sscanf(szTemp1,"%x",&uGConfigRec.bOpcode);

 		//get julian time and put it in the binary record
		strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
		szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&uGConfigRec.uiJulianSeconds);

		//set the date & time
		CJulianTime cTime;
		cTime.Init(uGConfigRec.uiJulianSeconds);

		bool bStale = true;

		if (m_uiuGConfigRecordTime < cTime.TotalSeconds())	
		{	
			//newer than the last received
			bStale = false;

			//update the time of the latest record
			m_uiuGConfigRecordTime = cTime.TotalSeconds();	

			//save a copy to send to the display dialog
			//when it initially gets displayed
			m_cuGConfigRecord = szRecord;

			//set date and time
			SetString(UGINFOPAGE,IDC_UGINFO_DATADATEC,cTime.Date());
			SetString(UGINFOPAGE,IDC_UGINFO_DATATIMEC,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_UGGCR_TIME,"SET_UGGCR_TIME",cTemp,false);
		}

		//set the acquire time
		strncpy(szTemp1,(char*)pszRecord->cdAcqInterval10ths,sizeof(pszRecord->cdAcqInterval10ths));
		szTemp1[sizeof(pszRecord->cdAcqInterval10ths)]=NULL;
		sscanf(szTemp1,"%d",&iTemp);
		sprintf(szTemp2,"%10.1f",(float)iTemp/10.0);
		uGConfigRec.uiAcqInterval10ths = iTemp;
		if (!bStale) 
		{
			SetString(UGINFOPAGE,IDC_UGINFO_ACQINTC,szTemp2);
			if (!CurrentSetParam(SET_UGGCR_COUNTTIME,"SET_UGGCR_COUNTTIME",szTemp1,true))
			{
				WriteToFileEx(113,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - COUNT_TIME (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_ACQINTC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set the baud and frame
		strncpy(szTemp1,(char*)pszRecord->cdBaudFrame,sizeof(pszRecord->cdBaudFrame));
		szTemp1[sizeof(pszRecord->cdBaudFrame)]=NULL;
		sscanf(szTemp1,"%2x",&uGConfigRec.bBaudFrame);

		if (!bStale) 
		{
			switch(szTemp1[0]) {
			case '0':
				strcpy(szTemp2 ,"300");
				break;
			case '1':
				strcpy(szTemp2 ,"600");
				break;
			case '2':
				strcpy(szTemp2 ,"1200");
				break;
			case '3':
				strcpy(szTemp2 ,"2400");
				break;
			case '4':
				strcpy(szTemp2 ,"4800");
				break;
			case '5':
				strcpy(szTemp2 ,"9600");
				break;
			case '6':
				strcpy(szTemp2 ,"19200");
				break;
			case '7':
				strcpy(szTemp2 ,"38400");
				break;
			case '8':
				strcpy(szTemp2 ,"57600");
				break;
			case '9':
				strcpy(szTemp2 ,"115200");
				break;
			default:
				strcpy(szTemp2 ,"ERR");
			}
			switch(szTemp1[1]) {
			case '0':
				strcpy(szTemp3,"7:Odd");
				break;
			case '1':
				strcpy(szTemp3,"7:Even");
				break;
			case '2':
				strcpy(szTemp3,"7:None");
				break;
			case '3':
				strcpy(szTemp3,"8:Odd");
				break;
			case '4':
				strcpy(szTemp3,"8:Even");
				break;
			case '5':
				strcpy(szTemp3,"8:None");
				break;
			default:
				strcpy(szTemp3,"ERR");
			}

			if (!CurrentSetParam(SET_UGGCR_BAUDRATE,"SET_UGGCR_BAUDRATE",szTemp2,true))
			{
				WriteToFileEx(114,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BAUD RATE (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_BAUDFRAMEC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
			if (!CurrentSetParam(SET_UGGCR_FRAME,"SET_UGGCR_FRAME",szTemp3,true))
			{
				WriteToFileEx(115,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - FRAME (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_BAUDFRAMEC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
			strcat(szTemp2,":");
			strcat(szTemp2,szTemp3);
			SetString(UGINFOPAGE,IDC_UGINFO_BAUDFRAMEC,szTemp2);
		}

		//set PHD V. Setpoint
		strncpy(szTemp1,(char*)pszRecord->cdPHDVSetPoint,sizeof(pszRecord->cdPHDVSetPoint));
		szTemp1[sizeof(pszRecord->cdPHDVSetPoint)]=NULL;
		sscanf(szTemp1,"%f",&uGConfigRec.fPHDVSetPoint);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_PHDSETPTC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_PHDV,"SET_UGGCR_PHDV",szTemp1,true))
			{
				WriteToFileEx(116,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PHD V. Setpoint (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_PHDSETPTC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}
		//set CPU Speed
		strncpy(szTemp1,(char*)pszRecord->chCPUSpeed,sizeof(pszRecord->chCPUSpeed));
		szTemp1[sizeof(pszRecord->chCPUSpeed)]=NULL;
		sscanf(szTemp1,"%x",&uGConfigRec.bCPUSpeed);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CPUSPEEDC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_OSCSET,"SET_UGGCR_OSCSET",szTemp1,true))
			{
				WriteToFileEx(117,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CPU Speed Select (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_CPUSPEEDC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set Mode Flags
		strncpy(szTemp1,(char*)pszRecord->chModeFlags,sizeof(pszRecord->chModeFlags));
		szTemp1[sizeof(pszRecord->chModeFlags)]=NULL;
		sscanf(szTemp1,"%x",&uGConfigRec.uiModeFlags);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_MODEFLAGC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_MODEFLAGS,"SET_UGGCR_MODEFLAGS",szTemp1,true))
			{
				WriteToFileEx(118,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Mode Flags (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_MODEFLAGC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}
		//set Filter Control
		strncpy(szTemp1,(char*)pszRecord->chFilterControl,sizeof(pszRecord->chFilterControl));
		szTemp1[sizeof(pszRecord->chFilterControl)]=NULL;
		sscanf(szTemp1,"%x",&chTemp);
		uGConfigRec.bFilterControl = chTemp;
		if (!bStale)
		{
			sscanf(szTemp1,"%x",&iTemp);
			szTemp2[0] = NULL;
			if (iTemp & 0x00000080) strcat(szTemp2,"Kal ");
			if (iTemp & 0x00000004) strcat(szTemp2,"f/a ");
			if (iTemp & 0x00000002) strcat(szTemp2,"s ");
			if (iTemp & 0x00000001) strcat(szTemp2,"e");
			SetString(UGINFOPAGE,IDC_UGINFO_FILTERCNTC,szTemp2);
			if (!CurrentSetParam(SET_UGGCR_FILTERCONT,"SET_UGGCR_FILTERCONT",szTemp1,true))
			{
				WriteToFileEx(119,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Filter Control (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_MODEFLAGC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}
		
		//set immediate buffer size
		strncpy(szTemp1,(char*)pszRecord->cdImmBuffSize,sizeof(pszRecord->cdImmBuffSize));
		szTemp1[sizeof(pszRecord->cdImmBuffSize)]=NULL;
		sscanf(szTemp1,"%d",&chTemp);
		uGConfigRec.bImmBuffSize = chTemp;
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_IMMBUFSIZEC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_IMMBUFSIZE,"SET_UGGCR_IMMBUFSIZE",szTemp1,true))
			{
				WriteToFileEx(120,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Immediate Buffer Size (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_IMMBUFSIZEC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set immediate buffer save size
		strncpy(szTemp1,(char*)pszRecord->cdImmBuffSaveSize,sizeof(pszRecord->cdImmBuffSaveSize));
		szTemp1[sizeof(pszRecord->cdImmBuffSaveSize)]=NULL;
		sscanf(szTemp1,"%d",&chTemp);
		uGConfigRec.bImmBuffSaveSize = chTemp;
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_IMMBUFSAVEC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_IMMBUFFSAVESIZE,"SET_UGGCR_IMMBUFFSAVESIZE",szTemp1,true))
			{
				WriteToFileEx(121,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Immediate Buffer Save Size (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_IMMBUFSAVEC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set local background size
		strncpy(szTemp1,(char*)pszRecord->cdLocalBGSize,sizeof(pszRecord->cdLocalBGSize));
		szTemp1[sizeof(pszRecord->cdLocalBGSize)]=NULL;
		sscanf(szTemp1,"%d",&uGConfigRec.bLocalBGSize);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_LOCBGSIZEC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_LOCBACKSIZE,"SET_UGGCR_LOCBACKSIZE",szTemp1,true))
			{
				WriteToFileEx(122,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Local Background Size (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_LOCBGSIZEC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set local background end
		strncpy(szTemp1,(char*)pszRecord->cdLocalBGEnd,sizeof(pszRecord->cdLocalBGEnd));
		szTemp1[sizeof(pszRecord->cdLocalBGEnd)]=NULL;
		sscanf(szTemp1,"%d",&chTemp);
		uGConfigRec.bLocalBGEnd = chTemp;
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_LOCBGENDC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_LOCBACKEND,"SET_UGGCR_LOCBACKEND",szTemp1,true))
			{
				WriteToFileEx(123,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Local Background End (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_LOCBGENDC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set Status record interval
		strncpy(szTemp1,(char*)pszRecord->cdStatusRecInterv,sizeof(pszRecord->cdStatusRecInterv));
		szTemp1[sizeof(pszRecord->cdStatusRecInterv)]=NULL;
		sscanf(szTemp1,"%d",&usTemp);
		uGConfigRec.usStatusRecInterv = usTemp;
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_STATUSRECINTC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_STATUSINT,"SET_UGGCR_STATUSINT",szTemp1,true))
			{
				WriteToFileEx(124,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Status Record Interval (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_STATUSRECINTC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set time sync state
		strncpy(szTemp1,(char*)&pszRecord->chTimeSyncState,sizeof(pszRecord->chTimeSyncState));
		szTemp1[sizeof(pszRecord->chTimeSyncState)]=NULL;
		sscanf(szTemp1,"%x",&chTemp);
		uGConfigRec.bTimeSyncState = chTemp;
		if (!bStale)
		{
			szTemp2[0]=NULL;
			if (szTemp1[0]=='1') strcpy(szTemp2,"Daily");
			if (szTemp1[0]=='2') strcpy(szTemp2,"Hourly");
			if (szTemp1[0]=='3') strcpy(szTemp2,"Daily,Hourly");
			SetString(UGINFOPAGE,IDC_UGINFO_TIMESYNCSTC,szTemp2);
			if (!CurrentSetParam(SET_UGGCR_TIMESYNC_STATE,"SET_UGGCR_TIMESYNC_STATE",szTemp1,true))
			{
				WriteToFileEx(125,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Time Sync State (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_TIMESYNCSTC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}
	
		//set hourly time sync time
		strncpy(szTemp1,(char*)pszRecord->cdHourlyTimeSync,sizeof(pszRecord->cdHourlyTimeSync));
		szTemp1[sizeof(pszRecord->cdHourlyTimeSync)]=NULL;
		sscanf(szTemp1,"%2d%2d",&chTemp,&chTemp1);
		uGConfigRec.bHourlyTimeSyncMM = chTemp;
		uGConfigRec.bHourlyTimeSyncSS = chTemp1;
		if (!bStale)
		{

			strncpy(szTemp2,szTemp1,2);
			strcat(szTemp2,":");
			strcat(szTemp2,&szTemp1[2]);
			SetString(UGINFOPAGE,IDC_UGINFO_HOURLYTSC,szTemp2);
			if (!CurrentSetParam(SET_UGGCR_TIMESYNC_HOUR,"SET_UGGCR_TIMESYNC_HOUR",szTemp1,true))
			{
				WriteToFileEx(126,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Hourly Time Sync (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_HOURLYTSC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set daily time sync time
		strncpy(szTemp1,(char*)pszRecord->cdDailyTimeSync,sizeof(pszRecord->cdDailyTimeSync));
		szTemp1[sizeof(pszRecord->cdDailyTimeSync)]=NULL;
		sscanf(szTemp1,"%2d%2d%2d",&chTemp,&chTemp1,&chTemp2);
		uGConfigRec.bDailyTimeSyncHH = chTemp;
		uGConfigRec.bDailyTimeSyncMM = chTemp1;
		uGConfigRec.bDailyTimeSyncSS = chTemp2;
		if (!bStale)
		{
			sprintf(szTemp2,"%02d:%02d:%02d",chTemp,chTemp1,chTemp2);
			SetString(UGINFOPAGE,IDC_UGINFO_DAILYTSC,szTemp2);
			if (!CurrentSetParam(SET_UGGCR_TIMESYNC_DAILY,"SET_UGGCR_TIMESYNC_DAILY",szTemp1,true))
			{
				WriteToFileEx(127,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Daily Time Sync (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_DAILYTSC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set time sync lower tolerance
		strncpy(szTemp1,(char*)pszRecord->cdTimeSyncLowerTol,sizeof(pszRecord->cdTimeSyncLowerTol));
		szTemp1[sizeof(pszRecord->cdTimeSyncLowerTol)]=NULL;
		sscanf(szTemp1,"%d",&chTemp);
		uGConfigRec.bTimeSyncLowerTol = chTemp;
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_TSLOWTOLC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_TIMESYNC_LOWTOL,"SET_UGGCR_TIMESYNC_LOWTOL",szTemp1,true))
			{
				WriteToFileEx(128,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Time Sync Lower Tolerance (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_TSLOWTOLC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set time sync upper tolerance
		strncpy(szTemp1,(char*)pszRecord->cdTimeSyncUpperTol,sizeof(pszRecord->cdTimeSyncUpperTol));
		szTemp1[sizeof(pszRecord->cdTimeSyncUpperTol)]=NULL;
		sscanf(szTemp1,"%d",&chTemp);
		uGConfigRec.bTimeSyncUpperTol = chTemp;
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_TSUPTOLC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_TIMESYNC_UPPTOL,"SET_UGGCR_TIMESYNC_UPPTOL",szTemp1,true))
			{
				WriteToFileEx(128,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Time Sync Upper Tolerance (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_TSUPTOLC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set inspector ID
		strncpy(szTemp1,(char*)pszRecord->cdInspectorID,sizeof(pszRecord->cdInspectorID));
		szTemp1[sizeof(pszRecord->cdInspectorID)]=NULL;
		sscanf(szTemp1,"%d",&uGConfigRec.uiInspectorID);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_INSPECTORC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_INSPECTOR_ID,"SET_UGGCR_INSPECTOR_ID",szTemp1,true))
			{
				WriteToFileEx(130,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Inspector ID (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_INSPECTORC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set unit ID
		strncpy(szTemp1,(char*)pszRecord->chUnitID,sizeof(pszRecord->chUnitID));
		szTemp1[sizeof(pszRecord->chUnitID)]=NULL;
		sscanf(szTemp1,"%x",&chTemp);
		uGConfigRec.bUnitID = chTemp;
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_UNITIDC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_UNIT_ID,"SET_UGGCR_UNIT_ID",szTemp1,true))
			{
				WriteToFileEx(131,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Unit ID (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_UNITIDC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set impedance settings
		strncpy(szTemp1,(char*)&pszRecord->chImpedanceSet,sizeof(pszRecord->chImpedanceSet));
		szTemp1[sizeof(pszRecord->chImpedanceSet)]=NULL;
		sscanf(szTemp1,"%x",&chTemp);
		uGConfigRec.bImpedanceSet = chTemp;
		if (!bStale)
		{
			sscanf(szTemp1,"%x",&iTemp);
			szTemp2[0]=NULL;
			if (iTemp & 0x01) strcat(szTemp2,"Ch0:50 "); else strcat(szTemp2,"Ch0:10K ");
			if (iTemp & 0x02) strcat(szTemp2,"Ch1:50 "); else strcat(szTemp2,"Ch1:10K ");
			if (iTemp & 0x04) strcat(szTemp2,"Ch2:50 "); else strcat(szTemp2,"Ch2:10K ");
			if (iTemp & 0x08) strcat(szTemp2,"Ch3:50");  else strcat(szTemp2,"Ch3:10K");
			SetString(UGINFOPAGE,IDC_UGINFO_IMPEDSETSC,szTemp2);
			if (!CurrentSetParam(SET_UGGCR_IMPEDANCE_SET,"SET_UGGCR_IMPEDANCE_SET",szTemp1,true))
			{
				WriteToFileEx(132,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Impedance Settings (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_IMPEDSETSC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set ch 0 kalman r
		strncpy(szTemp1,(char*)pszRecord->cfCh0KalmanR,sizeof(pszRecord->cfCh0KalmanR));
		szTemp1[sizeof(pszRecord->cfCh0KalmanR)]=NULL;
		sscanf(szTemp1,"%f",&uGConfigRec.fCh0KalmanR);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CH0KALRC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_CH0KALR,"SET_UGGCR_CH0KALR",szTemp1,true))
			{
				WriteToFileEx(133,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 0 Kalman R (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_CH0KALRC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set ch 1 kalman r
		strncpy(szTemp1,(char*)pszRecord->cfCh1KalmanR,sizeof(pszRecord->cfCh1KalmanR));
		szTemp1[sizeof(pszRecord->cfCh1KalmanR)]=NULL;
		sscanf(szTemp1,"%f",&uGConfigRec.fCh1KalmanR);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CH1KALRC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_CH1KALR,"SET_UGGCR_CH1KALR",szTemp1,true))
			{
				WriteToFileEx(134,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 1 Kalman R (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_CH1KALRC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set ch 2 kalman r
		strncpy(szTemp1,(char*)pszRecord->cfCh2KalmanR,sizeof(pszRecord->cfCh2KalmanR));
		szTemp1[sizeof(pszRecord->cfCh2KalmanR)]=NULL;
		sscanf(szTemp1,"%f",&uGConfigRec.fCh2KalmanR);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CH2KALRC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_CH2KALR,"SET_UGGCR_CH2KALR",szTemp1,true))
			{
				WriteToFileEx(135,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 2 Kalman R (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_CH2KALRC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set ch 3 kalman r
		strncpy(szTemp1,(char*)pszRecord->cfCh3KalmanR,sizeof(pszRecord->cfCh3KalmanR));
		szTemp1[sizeof(pszRecord->cfCh3KalmanR)]=NULL;
		sscanf(szTemp1,"%f",&uGConfigRec.fCh3KalmanR);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CH3KALRC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_CH3KALR,"SET_UGGCR_CH3KALR",szTemp1,true))
			{
				WriteToFileEx(136,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 3 Kalman R (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_CH3KALRC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set ch 0 kalman Q
		strncpy(szTemp1,(char*)pszRecord->cfCh0KalmanQ,sizeof(pszRecord->cfCh0KalmanQ));
		szTemp1[sizeof(pszRecord->cfCh0KalmanQ)]=NULL;
		sscanf(szTemp1,"%f",&uGConfigRec.fCh0KalmanQ);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CH0KALQC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_CH0KALQ,"SET_UGGCR_CH0KALQ",szTemp1,true))
			{
				WriteToFileEx(137,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 0 Kalman Q (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_CH0KALQC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set ch 1 kalman Q
		strncpy(szTemp1,(char*)pszRecord->cfCh1KalmanQ,sizeof(pszRecord->cfCh1KalmanQ));
		szTemp1[sizeof(pszRecord->cfCh1KalmanQ)]=NULL;
		sscanf(szTemp1,"%f",&uGConfigRec.fCh1KalmanQ);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CH1KALQC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_CH1KALQ,"SET_UGGCR_CH1KALQ",szTemp1,true))
			{
				WriteToFileEx(138,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 1 Kalman Q (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_CH1KALQC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set ch 2 kalman Q
		strncpy(szTemp1,(char*)pszRecord->cfCh2KalmanQ,sizeof(pszRecord->cfCh2KalmanQ));
		szTemp1[sizeof(pszRecord->cfCh2KalmanQ)]=NULL;
		sscanf(szTemp1,"%f",&uGConfigRec.fCh2KalmanQ);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CH2KALQC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_CH2KALQ,"SET_UGGCR_CH2KALQ",szTemp1,true))
			{
				WriteToFileEx(139,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 2 Kalman Q (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_CH2KALQC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//set ch 3 kalman Q
		strncpy(szTemp1,(char*)pszRecord->cfCh3KalmanQ,sizeof(pszRecord->cfCh3KalmanQ));
		szTemp1[sizeof(pszRecord->cfCh3KalmanQ)]=NULL;
		sscanf(szTemp1,"%f",&uGConfigRec.fCh3KalmanQ);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CH3KALQC,szTemp1);
			if (!CurrentSetParam(SET_UGGCR_CH3KALQ,"SET_UGGCR_CH3KALQ",szTemp1,true))
			{
				WriteToFileEx(140,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - Ch 3 Kalman Q (50)\r\n");
				SetColor(UGINFOPAGE,IDC_UGINFO_CH3KALQC,COLOR_YELLOW);
				bChange = true;
				m_iUGInfo_Tab |= 0x0002;//turn on yellow
			}
		}

		//no checksum in the binary version and the text version already has it in the record

		//send it off to be displayed if possible
		if (m_pPropertySheet && !bStale)
			m_pPropertySheet->OnuGConfigRecord((char*)pszRecord);

		char cBuff[sizeof(uGConfigTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(uGConfigTextRecord));
		cBuff[sizeof(uGConfigTextRecord) + 6] = NULL;

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

		WriteToSOHFileX((AcquireRecordX*)&uGConfigRec);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, szRecord,bStale);

	}

	PostMessage(IDC_RECEIVE_RESULT,UGCONFIGRECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnuGChConfigRecord(char* szRecord)		
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(uGChConfigTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//will need a few work areas
		char szTemp1[32];
		char szJulianTime[16];
		char szUsed[2];
		char szThresh1TD[4];
		char szThresh2TD[4];
		char szCh0Thresh1Value[16];
		char szCh1Thresh1Value[16];
		char szCh2Thresh1Value[16];
		char szCh3Thresh1Value[16];
		char szCh0Thresh2Value[16];
		char szCh1Thresh2Value[16];
		char szCh2Thresh2Value[16];
		char szCh3Thresh2Value[16];
		char szCh0T1EE[4];
		char szCh1T1EE[4];
		char szCh2T1EE[4];
		char szCh3T1EE[4];
		char szCh0T2EE[4];
		char szCh1T2EE[4];
		char szCh2T2EE[4];
		char szCh3T2EE[4];
		char szCh0CSM[8];
		char szCh1CSM[8];
		char szCh2CSM[8];
		char szCh3CSM[8];
		char szCh0CSEE[4];
		char szCh1CSEE[4];
		char szCh2CSEE[4];
		char szCh3CSEE[4];
		char szCh0FilterLim[8];
		char szCh1FilterLim[8];
		char szCh2FilterLim[8];
		char szCh3FilterLim[8];
		char szCh0Hystersis[4];
		char szCh1Hystersis[4];
		char szCh2Hystersis[4];
		char szCh3Hystersis[4];
		CString cTemp;
		unsigned short usTemp;
		unsigned short usTemp1;

		//do we have a change
		bool bChange = false;

		//create a binary record to send to the SOH file
		uGChConfigRecord uGChConfigRec;

		//get a pointer to a GeneralConfigTextRecord
		uGChConfigTextRecord* pszRecord = (uGChConfigTextRecord*)szRecord;

		//get length and opcode and place in binary record
		uGChConfigRec.bLength = sizeof(uGChConfigRecord);
		memcpy(szTemp1,pszRecord->chOpcode,sizeof(pszRecord->chOpcode));
		szTemp1[sizeof(pszRecord->chOpcode)] = NULL;
		sscanf(szTemp1,"%x",&uGChConfigRec.bOpcode);

 		//get julian time and put it in the binary record
		memcpy(szJulianTime,pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
		szJulianTime[sizeof(pszRecord->cdJulianSeconds)]=NULL;
		sscanf(szJulianTime,"%u",&uGChConfigRec.uiJulianSeconds);

		//set the date & time
		CJulianTime cTime;
		cTime.Init(uGChConfigRec.uiJulianSeconds);

		//get used
		memcpy(szUsed,&pszRecord->chUsedForAnalysis,sizeof(pszRecord->chUsedForAnalysis));
		szUsed[sizeof(pszRecord->chUsedForAnalysis)]=NULL;
		sscanf(szUsed,"%hx",&uGChConfigRec.bUsedForAnalysis);

		//get Threshold 1 type and direction
		memcpy(szThresh1TD,pszRecord->chThresh1TypeDir,sizeof(pszRecord->chThresh1TypeDir));
		szThresh1TD[sizeof(pszRecord->chThresh1TypeDir)]=NULL;
		sscanf(szThresh1TD,"%hx",&uGChConfigRec.bThresh1TypeDir);

		//get Threshold 2 type and direction
		memcpy(szThresh2TD,pszRecord->chThresh2TypeDir,sizeof(pszRecord->chThresh2TypeDir));
		szThresh2TD[sizeof(pszRecord->chThresh2TypeDir)]=NULL;
		sscanf(szThresh2TD,"%hx",&uGChConfigRec.bThresh2TypeDir);

		//get ch 0 threshold 1 value
		memcpy(szCh0Thresh1Value,pszRecord->cdfCh0Thresh1Value,sizeof(pszRecord->cdfCh0Thresh1Value));
		szCh0Thresh1Value[sizeof(pszRecord->cdfCh0Thresh1Value)]=NULL;
		sscanf(szCh0Thresh1Value,"%f",&uGChConfigRec.fCh0Thresh1Value);

		//get ch 1 threshold 1 value
		memcpy(szCh1Thresh1Value,pszRecord->cdfCh1Thresh1Value,sizeof(pszRecord->cdfCh1Thresh1Value));
		szCh1Thresh1Value[sizeof(pszRecord->cdfCh1Thresh1Value)]=NULL;
		sscanf(szCh1Thresh1Value,"%f",&uGChConfigRec.fCh1Thresh1Value);

		//get ch 2 threshold 1 value
		memcpy(szCh2Thresh1Value,pszRecord->cdfCh2Thresh1Value,sizeof(pszRecord->cdfCh2Thresh1Value));
		szCh2Thresh1Value[sizeof(pszRecord->cdfCh2Thresh1Value)]=NULL;
		sscanf(szCh2Thresh1Value,"%f",&uGChConfigRec.fCh2Thresh1Value);

		//get ch 3 threshold 1 value
		memcpy(szCh3Thresh1Value,pszRecord->cdfCh3Thresh1Value,sizeof(pszRecord->cdfCh3Thresh1Value));
		szCh3Thresh1Value[sizeof(pszRecord->cdfCh3Thresh1Value)]=NULL;
		sscanf(szCh3Thresh1Value,"%f",&uGChConfigRec.fCh3Thresh1Value);

		//get ch 0 threshold 2 value
		memcpy(szCh0Thresh2Value,pszRecord->cdfCh0Thresh2Value,sizeof(pszRecord->cdfCh0Thresh2Value));
		szCh0Thresh2Value[sizeof(pszRecord->cdfCh0Thresh2Value)]=NULL;
		sscanf(szCh0Thresh2Value,"%f",&uGChConfigRec.fCh0Thresh2Value);

		//get ch 1 threshold 2 value
		memcpy(szCh1Thresh2Value,pszRecord->cdfCh1Thresh2Value,sizeof(pszRecord->cdfCh1Thresh2Value));
		szCh1Thresh2Value[sizeof(pszRecord->cdfCh1Thresh2Value)]=NULL;
		sscanf(szCh1Thresh2Value,"%f",&uGChConfigRec.fCh1Thresh2Value);

		//get ch 2 threshold 2 value
		memcpy(szCh2Thresh2Value,pszRecord->cdfCh2Thresh2Value,sizeof(pszRecord->cdfCh2Thresh2Value));
		szCh2Thresh2Value[sizeof(pszRecord->cdfCh2Thresh2Value)]=NULL;
		sscanf(szCh2Thresh2Value,"%f",&uGChConfigRec.fCh2Thresh2Value);

		//get ch 3 threshold 2 value
		memcpy(szCh3Thresh2Value,pszRecord->cdfCh3Thresh2Value,sizeof(pszRecord->cdfCh3Thresh2Value));
		szCh3Thresh2Value[sizeof(pszRecord->cdfCh3Thresh2Value)]=NULL;
		sscanf(szCh3Thresh2Value,"%f",&uGChConfigRec.fCh3Thresh2Value);

		//get ch 0 threshold 1 entry/exit
		memcpy(szCh0T1EE,pszRecord->chCh0Thresh1EntryExit,sizeof(pszRecord->chCh0Thresh1EntryExit));
		szCh0T1EE[sizeof(pszRecord->chCh0Thresh1EntryExit)]=NULL;
		sscanf(szCh0T1EE,"%hx",&uGChConfigRec.bCh0Thresh1EntryExit);

		//get ch 1 threshold 1 entry/exit
		memcpy(szCh1T1EE,pszRecord->chCh1Thresh1EntryExit,sizeof(pszRecord->chCh1Thresh1EntryExit));
		szCh1T1EE[sizeof(pszRecord->chCh1Thresh1EntryExit)]=NULL;
		sscanf(szCh1T1EE,"%hx",&uGChConfigRec.bCh1Thresh1EntryExit);

		//get ch 2 threshold 1 entry/exit
		memcpy(szCh2T1EE,pszRecord->chCh2Thresh1EntryExit,sizeof(pszRecord->chCh2Thresh1EntryExit));
		szCh2T1EE[sizeof(pszRecord->chCh2Thresh1EntryExit)]=NULL;
		sscanf(szCh2T1EE,"%hx",&uGChConfigRec.bCh2Thresh1EntryExit);

		//get ch 3 threshold 1 entry/exit
		memcpy(szCh3T1EE,pszRecord->chCh3Thresh1EntryExit,sizeof(pszRecord->chCh3Thresh1EntryExit));
		szCh3T1EE[sizeof(pszRecord->chCh3Thresh1EntryExit)]=NULL;
		sscanf(szCh3T1EE,"%hx",&uGChConfigRec.bCh3Thresh1EntryExit);

		//get ch 0 threshold 2 entry/exit
		memcpy(szCh0T2EE,pszRecord->chCh0Thresh2EntryExit,sizeof(pszRecord->chCh0Thresh2EntryExit));
		szCh0T2EE[sizeof(pszRecord->chCh0Thresh2EntryExit)]=NULL;
		sscanf(szCh0T2EE,"%hx",&uGChConfigRec.bCh0Thresh2EntryExit);

		//get ch 1 threshold 2 entry/exit
		memcpy(szCh1T2EE,pszRecord->chCh2Thresh1EntryExit,sizeof(pszRecord->chCh1Thresh2EntryExit));
		szCh1T2EE[sizeof(pszRecord->chCh2Thresh1EntryExit)]=NULL;
		sscanf(szCh1T2EE,"%hx",&uGChConfigRec.bCh2Thresh1EntryExit);

		//get ch 2 threshold 2 entry/exit
		memcpy(szCh2T2EE,pszRecord->chCh2Thresh2EntryExit,sizeof(pszRecord->chCh2Thresh2EntryExit));
		szCh2T2EE[sizeof(pszRecord->chCh2Thresh2EntryExit)]=NULL;
		sscanf(szCh2T2EE,"%hx",&uGChConfigRec.bCh2Thresh2EntryExit);

		//get ch 3 threshold 2 entry/exit
		memcpy(szCh3T2EE,pszRecord->chCh3Thresh2EntryExit,sizeof(pszRecord->chCh3Thresh2EntryExit));
		szCh3T2EE[sizeof(pszRecord->chCh3Thresh2EntryExit)]=NULL;
		sscanf(szCh3T2EE,"%hx",&uGChConfigRec.bCh3Thresh2EntryExit);

		//get ch 0 Changing signal multiplier
		memcpy(szCh0CSM,pszRecord->cdfCh0ChangSigMultip,sizeof(pszRecord->cdfCh0ChangSigMultip));
		szCh0CSM[sizeof(pszRecord->cdfCh0ChangSigMultip)]=NULL;
		sscanf(szCh0CSM,"%f",&uGChConfigRec.fCh0ChangSigMultip);
		
		//get ch 1 Changing signal multiplier
		memcpy(szCh1CSM,pszRecord->cdfCh1ChangSigMultip,sizeof(pszRecord->cdfCh1ChangSigMultip));
		szCh1CSM[sizeof(pszRecord->cdfCh1ChangSigMultip)]=NULL;
		sscanf(szCh1CSM,"%f",&uGChConfigRec.fCh1ChangSigMultip);
		
		//get ch 2 Changing signal multiplier
		memcpy(szCh2CSM,pszRecord->cdfCh2ChangSigMultip,sizeof(pszRecord->cdfCh2ChangSigMultip));
		szCh2CSM[sizeof(pszRecord->cdfCh2ChangSigMultip)]=NULL;
		sscanf(szCh2CSM,"%f",&uGChConfigRec.fCh2ChangSigMultip);

		//get ch 3 Changing signal multiplier
		memcpy(szCh3CSM,pszRecord->cdfCh3ChangSigMultip,sizeof(pszRecord->cdfCh3ChangSigMultip));
		szCh0CSM[sizeof(pszRecord->cdfCh3ChangSigMultip)]=NULL;
		sscanf(szCh3CSM,"%f",&uGChConfigRec.fCh3ChangSigMultip);

		//get ch 0 Changing signal entry / exit
		memcpy(szCh0CSEE,pszRecord->chCh0ChangSigEntryExit,sizeof(pszRecord->chCh0ChangSigEntryExit));
		szCh0CSEE[sizeof(pszRecord->chCh0ChangSigEntryExit)]=NULL;
		sscanf(szCh0CSEE,"%x",&uGChConfigRec.bCh0ChangSigEntryExit);

		//get ch 1 Changing signal entry / exit
		memcpy(szCh1CSEE,pszRecord->chCh1ChangSigEntryExit,sizeof(pszRecord->chCh1ChangSigEntryExit));
		szCh1CSEE[sizeof(pszRecord->chCh1ChangSigEntryExit)]=NULL;
		sscanf(szCh1CSEE,"%x",&uGChConfigRec.bCh1ChangSigEntryExit);

		//get ch 2 Changing signal entry / exit
		memcpy(szCh2CSEE,pszRecord->chCh2ChangSigEntryExit,sizeof(pszRecord->chCh2ChangSigEntryExit));
		szCh2CSEE[sizeof(pszRecord->chCh2ChangSigEntryExit)]=NULL;
		sscanf(szCh2CSEE,"%x",&uGChConfigRec.bCh2ChangSigEntryExit);

		//get ch 3 Changing signal entry / exit
		memcpy(szCh3CSEE,pszRecord->chCh3ChangSigEntryExit,sizeof(pszRecord->chCh3ChangSigEntryExit));
		szCh3CSEE[sizeof(pszRecord->chCh3ChangSigEntryExit)]=NULL;
		sscanf(szCh3CSEE,"%x",&uGChConfigRec.bCh3ChangSigEntryExit);

		//get ch 0 filter limit
		memcpy(szCh0FilterLim,pszRecord->cdfCh0FilterLimit,sizeof(pszRecord->cdfCh0FilterLimit));
		szCh0FilterLim[sizeof(pszRecord->cdfCh0FilterLimit)]=NULL;
		sscanf(szCh0FilterLim,"%f",&uGChConfigRec.fCh0FilterLimit);

		//get ch 1 filter limit
		memcpy(szCh1FilterLim,pszRecord->cdfCh1FilterLimit,sizeof(pszRecord->cdfCh1FilterLimit));
		szCh1FilterLim[sizeof(pszRecord->cdfCh1FilterLimit)]=NULL;
		sscanf(szCh1FilterLim,"%f",&uGChConfigRec.fCh1FilterLimit);

		//get ch 2 filter limit
		memcpy(szCh2FilterLim,pszRecord->cdfCh2FilterLimit,sizeof(pszRecord->cdfCh2FilterLimit));
		szCh2FilterLim[sizeof(pszRecord->cdfCh2FilterLimit)]=NULL;
		sscanf(szCh2FilterLim,"%f",&uGChConfigRec.fCh2FilterLimit);

		//get ch 3 filter limit
		memcpy(szCh3FilterLim,pszRecord->cdfCh3FilterLimit,sizeof(pszRecord->cdfCh3FilterLimit));
		szCh3FilterLim[sizeof(pszRecord->cdfCh3FilterLimit)]=NULL;
		sscanf(szCh3FilterLim,"%f",&uGChConfigRec.fCh3FilterLimit);

		//get ch 0 hysteresis
		memcpy(szCh0Hystersis,pszRecord->cdCh0Hysteresis,sizeof(pszRecord->cdCh0Hysteresis));
		szCh0Hystersis[sizeof(pszRecord->cdCh0Hysteresis)]=NULL;
		sscanf(szCh0Hystersis,"%hd",&uGChConfigRec.bCh0Hysteresis);

		//get ch 1 hysteresis
		memcpy(szCh1Hystersis,pszRecord->cdCh1Hysteresis,sizeof(pszRecord->cdCh1Hysteresis));
		szCh1Hystersis[sizeof(pszRecord->cdCh1Hysteresis)]=NULL;
		sscanf(szCh1Hystersis,"%hd",&uGChConfigRec.bCh1Hysteresis);

		//get ch 2 hysteresis
		memcpy(szCh2Hystersis,pszRecord->cdCh2Hysteresis,sizeof(pszRecord->cdCh2Hysteresis));
		szCh2Hystersis[sizeof(pszRecord->cdCh2Hysteresis)]=NULL;
		sscanf(szCh2Hystersis,"%hd",&uGChConfigRec.bCh2Hysteresis);

		//get ch 3 hysteresis
		memcpy(szCh3Hystersis,pszRecord->cdCh3Hysteresis,sizeof(pszRecord->cdCh3Hysteresis));
		szCh3Hystersis[sizeof(pszRecord->cdCh3Hysteresis)]=NULL;
		sscanf(szCh3Hystersis,"%hd",&uGChConfigRec.bCh3Hysteresis);

		//we now have the binary image of the record built and each of the text values

		bool bStale = true;

		if (m_uiuGChConfigRecordTime < cTime.TotalSeconds())	
		{	
			//newer than the last received
			bStale = false;

			//update the time of the latest record
			m_uiuGChConfigRecordTime = cTime.TotalSeconds();	

			//save a copy to send to the display dialog
			//when it initially gets displayed
			m_cuGConfigRecord = szRecord;

			//set date and time
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			SetString(USER4PAGE,IDC_USER4PROG_TIME2,cTemp);
			CurrentSetParam(SET_CCR_TIME,"SET_CCR_TIME",cTemp,false);

			//set used for analysis in each of the columns
			sscanf(szUsed,"%hx",&usTemp);
			if (!CurrentSetParam(SET_CCR_USED,"SET_CCR_USED",szUsed,false))
			{
				GetPrivateProfileString(m_pName,"SET_CCR_USED","",szTemp1,sizeof(szTemp1),m_szIniFile);
				sscanf(szTemp1,"%x",&usTemp1);
				usTemp1 = usTemp1 ^ usTemp; 
				if (usTemp1 & 0x0001)
				{
					SetColor(USER4PAGE,IDC_USER4PROG_CH0USED,COLOR_YELLOW);
					WriteToFileEx(141,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 (NOT)USED (51)\r\n");
				}
				if (usTemp1 & 0x0002)
				{
					SetColor(USER4PAGE,IDC_USER4PROG_CH1USED,COLOR_YELLOW);
					WriteToFileEx(142,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 (NOT)USED (51)\r\n");
				}
				if (usTemp1 & 0x0004)
				{
					SetColor(USER4PAGE,IDC_USER4PROG_CH2USED,COLOR_YELLOW);
					WriteToFileEx(143,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 (NOT)USED (51)\r\n");
				}
				if (usTemp1 & 0x0008) 
				{
					SetColor(USER4PAGE,IDC_USER4PROG_CH3USED,COLOR_YELLOW);
					WriteToFileEx(144,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 (NOT)USED (51)\r\n");
				}
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}

			SetString(USER4PAGE,IDC_USER4PROG_CH0USED,(usTemp & 0x0001)?"Chan. 0":"Not Used");
			SetString(USER4PAGE,IDC_USER4PROG_CH1USED,(usTemp & 0x0002)?"Chan. 1":"Not Used");
			SetString(USER4PAGE,IDC_USER4PROG_CH2USED,(usTemp & 0x0004)?"Chan. 2":"Not Used");
			SetString(USER4PAGE,IDC_USER4PROG_CH3USED,(usTemp & 0x0008)?"Chan. 3":"Not Used");

			//set threshold 1 type and direction
			sscanf(szThresh1TD,"%hx",&usTemp);
			if (!CurrentSetParam(SET_CCR_T1TYPEDIR,"SET_CCR_T1TYPEDIR",szThresh1TD,true))
			{
				GetPrivateProfileString(m_pName,"SET_CCR_T1TYPEDIR","",szTemp1,sizeof(szTemp1),m_szIniFile);
				sscanf(szTemp1,"%x",&usTemp1);
				usTemp1 = usTemp1 ^ usTemp; 
				if (usTemp1 & 0x00c0)
				{
					SetColor(USER4PAGE,IDC_USER4PROG_C00,COLOR_YELLOW);
					WriteToFileEx(145, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 1 Type or Direction (51)\r\n");
				}
				if (usTemp1 & 0x0030) 
				{
					SetColor(USER4PAGE,IDC_USER4PROG_C10,COLOR_YELLOW);
					WriteToFileEx(146, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 1 Type or Direction (51)\r\n");
				}
				if (usTemp1 & 0x000c) 
				{
					SetColor(USER4PAGE,IDC_USER4PROG_C20,COLOR_YELLOW);
					WriteToFileEx(147, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 1 Type or Direction (51)\r\n");
				}
				if (usTemp1 & 0x0003)
				{
					SetColor(USER4PAGE,IDC_USER4PROG_C30,COLOR_YELLOW);
					WriteToFileEx(148, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 1 Type or Direction (51)\r\n");
				}
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			strcpy(szTemp1,(usTemp & 0x0080)?"Rel/":"Abs/");
			strcat(szTemp1,(usTemp & 0x0040)?"+":"-");
			SetString(USER4PAGE,IDC_USER4PROG_C00,szTemp1);
			strcpy(szTemp1,(usTemp & 0x0020)?"Rel/":"Abs/");
			strcat(szTemp1,(usTemp & 0x0010)?"+":"-");
			SetString(USER4PAGE,IDC_USER4PROG_C10,szTemp1);
			strcpy(szTemp1,(usTemp & 0x0008)?"Rel/":"Abs/");
			strcat(szTemp1,(usTemp & 0x0004)?"+":"-");
			SetString(USER4PAGE,IDC_USER4PROG_C20,szTemp1);
			strcpy(szTemp1,(usTemp & 0x0002)?"Rel/":"Abs/");
			strcat(szTemp1,(usTemp & 0x0001)?"+":"-");
			SetString(USER4PAGE,IDC_USER4PROG_C30,szTemp1);

			//set threshold 2 type and direction
			sscanf(szThresh2TD,"%hx",&usTemp);
			if (!CurrentSetParam(SET_CCR_T2TYPEDIR,"SET_CCR_T2TYPEDIR",szThresh2TD,true))
			{
				GetPrivateProfileString(m_pName,"SET_CCR_T2TYPEDIR","",szTemp1,sizeof(szTemp1),m_szIniFile);
				sscanf(szTemp1,"%x",&usTemp1);
				usTemp1 = usTemp1 ^ usTemp; 
				if (usTemp1 & 0x00c0)
				{
					SetColor(USER4PAGE,IDC_USER4PROG_C03,COLOR_YELLOW);
					WriteToFileEx(149,TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 2 Type or Direction (51)\r\n");
				}
				if (usTemp1 & 0x0030) 
				{
					SetColor(USER4PAGE,IDC_USER4PROG_C13,COLOR_YELLOW);
					WriteToFileEx(150,TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 2 Type or Direction (51)\r\n");
				}
				if (usTemp1 & 0x000c) 
				{
					SetColor(USER4PAGE,IDC_USER4PROG_C23,COLOR_YELLOW);
					WriteToFileEx(151,TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 2 Type or Direction (51)\r\n");
				}
				if (usTemp1 & 0x0003)
				{
					SetColor(USER4PAGE,IDC_USER4PROG_C33,COLOR_YELLOW);
					WriteToFileEx(152,TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 2 Type or Direction (51)\r\n");
				}
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			strcpy(szTemp1,(usTemp & 0x0080)?"Rel/":"Abs/");
			strcat(szTemp1,(usTemp & 0x0040)?"+":"-");
			SetString(USER4PAGE,IDC_USER4PROG_C03,szTemp1);
			strcpy(szTemp1,(usTemp & 0x0020)?"Rel/":"Abs/");
			strcat(szTemp1,(usTemp & 0x0010)?"+":"-");
			SetString(USER4PAGE,IDC_USER4PROG_C13,szTemp1);
			strcpy(szTemp1,(usTemp & 0x0008)?"Rel/":"Abs/");
			strcat(szTemp1,(usTemp & 0x0004)?"+":"-");
			SetString(USER4PAGE,IDC_USER4PROG_C23,szTemp1);
			strcpy(szTemp1,(usTemp & 0x0002)?"Rel/":"Abs/");
			strcat(szTemp1,(usTemp & 0x0001)?"+":"-");
			SetString(USER4PAGE,IDC_USER4PROG_C33,szTemp1);

			//ch 0 thresh 1 value
			if (!CurrentSetParam(SET_CCR_C0T1VALUE,"SET_CCR_C0T1VALUE",szCh0Thresh1Value,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C01,COLOR_YELLOW);
				WriteToFileEx(153, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 1 Value (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C01,szCh0Thresh1Value);

			//ch 1 thresh 1 value
			if (!CurrentSetParam(SET_CCR_C1T1VALUE,"SET_CCR_C1T1VALUE",szCh1Thresh1Value,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C11,COLOR_YELLOW);
				WriteToFileEx(154, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 1 Value (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C11,szCh1Thresh1Value);

			//ch 2 thres 1 value
			if (!CurrentSetParam(SET_CCR_C2T1VALUE,"SET_CCR_C2T1VALUE",szCh2Thresh1Value,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C21,COLOR_YELLOW);
				WriteToFileEx(155, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 1 Value (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C21,szCh2Thresh1Value);

			//ch 3 thres 1 value
			if (!CurrentSetParam(SET_CCR_C3T1VALUE,"SET_CCR_C3T1VALUE",szCh3Thresh1Value,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C31,COLOR_YELLOW);
				WriteToFileEx(156, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 1 Value (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C31,szCh3Thresh1Value);

			//ch 0 thresh 2 value
			if (!CurrentSetParam(SET_CCR_C0T2VALUE,"SET_CCR_C0T2VALUE",szCh0Thresh2Value,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C04,COLOR_YELLOW);
				WriteToFileEx(157, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 2 Value (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C04,szCh0Thresh2Value);

			//ch 1 thresh 2 value
			if (!CurrentSetParam(SET_CCR_C1T2VALUE,"SET_CCR_C1T2VALUE",szCh1Thresh2Value,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C14,COLOR_YELLOW);
				WriteToFileEx(158, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 2 Value (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C14,szCh1Thresh2Value);

			//ch 2 thres 2 value
			if (!CurrentSetParam(SET_CCR_C2T2VALUE,"SET_CCR_C2T2VALUE",szCh2Thresh2Value,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C24,COLOR_YELLOW);
				WriteToFileEx(159, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 2 Value (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C24,szCh2Thresh2Value);

			//ch 3 thres 2 value
			if (!CurrentSetParam(SET_CCR_C3T2VALUE,"SET_CCR_C3T2VALUE",szCh3Thresh2Value,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C34,COLOR_YELLOW);
				WriteToFileEx(160, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 2 Value (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C34,szCh3Thresh2Value);

			//ch 0 thresh 1 entry/exit
			if (!CurrentSetParam(SET_CCR_C0T1EE,"SET_CCR_C0T1EE",szCh0T1EE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C02,COLOR_YELLOW);
				WriteToFileEx(161, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 1 Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh0T1EE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C02,szTemp1);

			//ch 1 thresh 1 entry/exit
			if (!CurrentSetParam(SET_CCR_C1T1EE,"SET_CCR_C1T1EE",szCh1T1EE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C12,COLOR_YELLOW);
				WriteToFileEx(162, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 1 Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh1T1EE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C12,szTemp1);

			//ch 2 thresh 1 entry/exit
			if (!CurrentSetParam(SET_CCR_C2T1EE,"SET_CCR_C2T1EE",szCh2T1EE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C22,COLOR_YELLOW);
				WriteToFileEx(163, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 1 Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh2T1EE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C22,szTemp1);

			//ch 3 thresh 1 entry/exit
			if (!CurrentSetParam(SET_CCR_C3T1EE,"SET_CCR_C3T1EE",szCh3T1EE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C32,COLOR_YELLOW);
				WriteToFileEx(164, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 1 Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh3T1EE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C32,szTemp1);

			//ch 0 thresh 2 entry/exit
			if (!CurrentSetParam(SET_CCR_C0T2EE,"SET_CCR_C0T2EE",szCh0T2EE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C05,COLOR_YELLOW);
				WriteToFileEx(165, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Threshold 2 Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh0T2EE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C05,szTemp1);

			//ch 1 thresh 2 entry/exit
			if (!CurrentSetParam(SET_CCR_C1T2EE,"SET_CCR_C1T2EE",szCh1T2EE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C15,COLOR_YELLOW);
				WriteToFileEx(166, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Threshold 2 Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh1T2EE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C15,szTemp1);

			//ch 2 thresh 2 entry/exit
			if (!CurrentSetParam(SET_CCR_C2T2EE,"SET_CCR_C2T2EE",szCh2T2EE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C25,COLOR_YELLOW);
				WriteToFileEx(167, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Threshold 2 Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh2T2EE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C25,szTemp1);

			//ch 3 thresh 2 entry/exit
			if (!CurrentSetParam(SET_CCR_C3T2EE,"SET_CCR_C3T2EE",szCh3T1EE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C35,COLOR_YELLOW);
				WriteToFileEx(168, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Threshold 2 Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh3T1EE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C35,szTemp1);

			//ch 0 changing signal multiplier
			if (!CurrentSetParam(SET_CCR_C0CCM,"SET_CCR_C0CCM",szCh0CSM,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C06,COLOR_YELLOW);
				WriteToFileEx(169, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Changing Signal multiplier (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C06,szCh0CSM);

			//ch 1 changing signal multiplier
			if (!CurrentSetParam(SET_CCR_C1CCM,"SET_CCR_C1CCM",szCh1CSM,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C16,COLOR_YELLOW);
				WriteToFileEx(170, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Changing Signal multiplier (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C16,szCh1CSM);

			//ch 2 changing signal multiplier
			if (!CurrentSetParam(SET_CCR_C2CCM,"SET_CCR_C2CCM",szCh2CSM,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C26,COLOR_YELLOW);
				WriteToFileEx(171, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Changing Signal multiplier (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C26,szCh2CSM);

			//ch 3 changing signal multiplier
			if (!CurrentSetParam(SET_CCR_C3CCM,"SET_CCR_C3CCM",szCh3CSM,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C36,COLOR_YELLOW);
				WriteToFileEx(172, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Changing Signal multiplier (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C36,szCh3CSM);

			//ch 0 changing signal entry exit
			if (!CurrentSetParam(SET_CCR_C0CSEE,"SET_CCR_C0CSEE",szCh0CSEE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C07,COLOR_YELLOW);
				WriteToFileEx(173, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Changing Signal Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh0CSEE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C07,szTemp1);

			//ch 1 changing signal entry exit
			if (!CurrentSetParam(SET_CCR_C0CSEE,"SET_CCR_C1CSEE",szCh1CSEE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C17,COLOR_YELLOW);
				WriteToFileEx(174, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Changing Signal Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh1CSEE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C17,szTemp1);

			//ch 2 changing signal entry exit
			if (!CurrentSetParam(SET_CCR_C2CSEE,"SET_CCR_C2CSEE",szCh2CSEE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C27,COLOR_YELLOW);
				WriteToFileEx(175, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Changing Signal Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh2CSEE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C27,szTemp1);

			//ch 3 changing signal entry exit
			if (!CurrentSetParam(SET_CCR_C3CSEE,"SET_CCR_C3CSEE",szCh3CSEE,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C37,COLOR_YELLOW);
				WriteToFileEx(176, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Changing Signal Entry/Exit(51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			sscanf(szCh3CSEE,"%x",&usTemp);
			sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
			SetString(USER4PAGE,IDC_USER4PROG_C37,szTemp1);

			//ch 0 filter limit
			if (!CurrentSetParam(SET_CCR_FILTERLIM0,"SET_CCR_FILTERLIM0",szCh0FilterLim,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C08,COLOR_YELLOW);
				WriteToFileEx(177, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Filter Limit (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C08,szCh0FilterLim);

			//ch 1 filter limit
			if (!CurrentSetParam(SET_CCR_FILTERLIM0,"SET_CCR_FILTERLIM1",szCh1FilterLim,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C18,COLOR_YELLOW);
				WriteToFileEx(178, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Filter Limit (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C18,szCh1FilterLim);

			//ch 2 filter limit
			if (!CurrentSetParam(SET_CCR_FILTERLIM2,"SET_CCR_FILTERLIM2",szCh2FilterLim,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C28,COLOR_YELLOW);
				WriteToFileEx(179, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Filter Limit (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C28,szCh2FilterLim);

			//ch 3 filter limit
			if (!CurrentSetParam(SET_CCR_FILTERLIM3,"SET_CCR_FILTERLIM3",szCh3FilterLim,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C38,COLOR_YELLOW);
				WriteToFileEx(180, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Filter Limit (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C38,szCh3FilterLim);

			//ch 0 hysteresis
			if (!CurrentSetParam(SET_CCR_HYSTER0,"SET_CCR_HYSTER0",szCh0Hystersis,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C09,COLOR_YELLOW);
				WriteToFileEx(181, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 0 Hysteresis (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C09,szCh0Hystersis);

			//ch 1 hysteresis
			if (!CurrentSetParam(SET_CCR_HYSTER1,"SET_CCR_HYSTER1",szCh1Hystersis,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C19,COLOR_YELLOW);
				WriteToFileEx(182, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 1 Hysteresis (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C19,szCh1Hystersis);

			//ch 2 hysteresis
			if (!CurrentSetParam(SET_CCR_HYSTER2,"SET_CCR_HYSTER2",szCh2Hystersis,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C29,COLOR_YELLOW);
				WriteToFileEx(183, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 2 Hysteresis (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C29,szCh2Hystersis);

			//ch 0 hysteresis
			if (!CurrentSetParam(SET_CCR_HYSTER3,"SET_CCR_HYSTER3",szCh3Hystersis,true))
			{
				SetColor(USER4PAGE,IDC_USER4PROG_C39,COLOR_YELLOW);
				WriteToFileEx(184, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - CH 3 Hysteresis (51)\r\n");
				m_iUser4Prog_Tab |= 0x0002;
				m_iInst4Setup_Tab |= 0x0001;
			}
			SetString(USER4PAGE,IDC_USER4PROG_C39,szCh3Hystersis);

		}

		//send it off to be displayed if possible
		if (m_pPropertySheet && !bStale)
			m_pPropertySheet->OnuGChConfigRecord((char*)pszRecord);

		char cBuff[sizeof(uGChConfigTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(uGChConfigTextRecord));
		cBuff[sizeof(uGChConfigTextRecord) + 6] = NULL;

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

		WriteToSOHFileX((AcquireRecordX*)&uGChConfigRec);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, szRecord,bStale);
	}

	PostMessage(IDC_RECEIVE_RESULT,UGCHCONFIGRECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnESBConfigRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(ESBConfigTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//some temp buffers
		char szTemp1[32];
		char szTempModeFlags[8];
		char szNomAcqInterv[8];
		char szAnomAcqInterv[8];
		char szLPAcqInterv[8];
		char szTempSN[16];
		CString cTemp;

		//create a binary record 
		ESBConfigRecord ESBConfigRec;

		//get a pointer to the text record
		ESBConfigTextRecord* psESBConfigTextRecord = (ESBConfigTextRecord*)szRecord;

		//get the length into binary record
		ESBConfigRec.bLength = sizeof(ESBConfigRecord);

		//get opcode into binary record
		memcpy(szTemp1,psESBConfigTextRecord->chOpcode,sizeof(psESBConfigTextRecord->chOpcode));
		szTemp1[sizeof(psESBConfigTextRecord->chOpcode)] = NULL;
		sscanf(szTemp1,"%x",&ESBConfigRec.bOpcode);

		//get mode flags into binary record
		memcpy(szTempModeFlags,psESBConfigTextRecord->chModeFlags,sizeof(psESBConfigTextRecord->chModeFlags));
		szTempModeFlags[sizeof(psESBConfigTextRecord->chModeFlags)]=NULL;
		sscanf(szTempModeFlags,"%hd",&ESBConfigRec.chModeFlags);

		//get nominal acq interval
		memcpy(szNomAcqInterv,psESBConfigTextRecord->cdNominalAcqInterval,sizeof(psESBConfigTextRecord->cdNominalAcqInterval));
		szNomAcqInterv[sizeof(psESBConfigTextRecord->cdNominalAcqInterval)]=NULL;
		sscanf(szNomAcqInterv,"%hd",&ESBConfigRec.usNominalAcqInterval);

		//get anominal acq interval
		memcpy(szAnomAcqInterv,&psESBConfigTextRecord->cdAnomalousAcqInterval,sizeof(psESBConfigTextRecord->cdAnomalousAcqInterval));
		szAnomAcqInterv[sizeof(psESBConfigTextRecord->cdAnomalousAcqInterval)]=NULL;
		sscanf(szAnomAcqInterv,"%hd",&ESBConfigRec.usAnomalousAcqInterval);

		//get low power acq interval
		memcpy(szLPAcqInterv,&psESBConfigTextRecord->cdLowPowerAcqInterval,sizeof(psESBConfigTextRecord->cdLowPowerAcqInterval));
		szLPAcqInterv[sizeof(psESBConfigTextRecord->cdLowPowerAcqInterval)]=NULL;
		sscanf(szLPAcqInterv,"%hd",&ESBConfigRec.usLowPowerAcqInterval);

		//get the julian seconds
		memcpy(szTemp1,psESBConfigTextRecord->cdJulianSeconds,sizeof(psESBConfigTextRecord->cdJulianSeconds));
		szTemp1[sizeof(psESBConfigTextRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&ESBConfigRec.uiJulianSeconds);

		//get the serial number
		memcpy(szTempSN,psESBConfigTextRecord->chSerialNumber,sizeof(psESBConfigTextRecord->chSerialNumber));
		szTempSN[sizeof(psESBConfigTextRecord->chSerialNumber)]=NULL;
		sscanf(szTempSN,"%2x%2x%2x%2x%2x%2x",
			&ESBConfigRec.bSerialNumber[0],
			&ESBConfigRec.bSerialNumber[1],
			&ESBConfigRec.bSerialNumber[2],
			&ESBConfigRec.bSerialNumber[3],
			&ESBConfigRec.bSerialNumber[4],
			&ESBConfigRec.bSerialNumber[5]);

		//binary version should be complete at this point

		//set the date and time
		CJulianTime cTime;
		cTime.Init(szTemp1);

		bool bStale = false;

		int iColumn = 0;
		if      (strncmp(szTempSN,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1),sizeof(psESBConfigTextRecord->chSerialNumber)) == 0) iColumn = 1;
		else if (strncmp(szTempSN,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2),sizeof(psESBConfigTextRecord->chSerialNumber)) == 0) iColumn = 2;
		else if (strncmp(szTempSN,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3),sizeof(psESBConfigTextRecord->chSerialNumber)) == 0) iColumn = 3;
		else if (strncmp(szTempSN,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4),sizeof(psESBConfigTextRecord->chSerialNumber)) == 0) iColumn = 4;
		else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1))==0) iColumn = 5;
		else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2))==0) iColumn = 6;
		else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3))==0) iColumn = 7;
		else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4))==0) iColumn = 8;

		if (m_uiESBConfigRecordTime < cTime.TotalSeconds())
		{
			//newer than the last received
			bStale = false;

			//update the time of the latest record
			m_uiESBConfigRecordTime = cTime.TotalSeconds();	

			//set date and time
			switch (iColumn) {
			case 5:
				SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1,psESBConfigTextRecord->chSerialNumber,sizeof(psESBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_ESBIR_SERIALNUM1,"SET_ESBIR_SERIALNUM1",szTempSN,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL1,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
			//fall through to process rest
			case 1:
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEC1,cTime.Date());
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEC1,cTime.Time());
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				CurrentSetParam(SET_ESBCR_TIME1,"SET_ESBCR_TIME1",cTemp,false);

				if (!CurrentSetParam(SET_ESBCR_MODEFLAGS1,"SET_ESBCR_MODEFLAGS1",szTempModeFlags,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_MODEFLAGS1,COLOR_YELLOW);
					WriteToFileEx(185, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Mode Flags (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_NORMACQINTERV1,"SET_ESBCR_NORMACQINTERV1",szNomAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_NOMACQINT1,COLOR_YELLOW);
					WriteToFileEx(186, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Nominal Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_ANOMACQINTERV1,"SET_ESBCR_ANOMACQINTERV1",szAnomAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_ANOMACQINT1,COLOR_YELLOW);
					WriteToFileEx(187, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Anomalous Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_LOPWACQINTERV1,"SET_ESBCR_LOPWACQINTERV1",szLPAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_LPACQINT1,COLOR_YELLOW);
					WriteToFileEx(188, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Low Power Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}
				break;

			case 6:
				SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2,psESBConfigTextRecord->chSerialNumber,sizeof(psESBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_ESBIR_SERIALNUM2,"SET_ESBIR_SERIALNUM2",szTempSN,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL2,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
			//fall through to process rest
			case 2:
				if (!CurrentSetParam(SET_ESBCR_MODEFLAGS2,"SET_ESBCR_MODEFLAGS2",szTempModeFlags,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_MODEFLAGS2,COLOR_YELLOW);
					WriteToFileEx(189, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Mode Flags (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_NORMACQINTERV2,"SET_ESBCR_NORMACQINTERV2",szNomAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_NOMACQINT2,COLOR_YELLOW);
					WriteToFileEx(190, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Nominal Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_ANOMACQINTERV2,"SET_ESBCR_ANOMACQINTERV2",szAnomAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_ANOMACQINT2,COLOR_YELLOW);
					WriteToFileEx(191, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Anomalous Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_LOPWACQINTERV2,"SET_ESBCR_LOPWACQINTERV2",szLPAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_LPACQINT2,COLOR_YELLOW);
					WriteToFileEx(192, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Low Power Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}
				break;

			case 7:
				SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3,psESBConfigTextRecord->chSerialNumber,sizeof(psESBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_ESBIR_SERIALNUM3,"SET_ESBIR_SERIALNUM3",szTempSN,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL3,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
			//fall through to process rest
			case 3:
				if (!CurrentSetParam(SET_ESBCR_MODEFLAGS3,"SET_ESBCR_MODEFLAGS3",szTempModeFlags,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_MODEFLAGS3,COLOR_YELLOW);
					WriteToFileEx(193, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Mode Flags (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_NORMACQINTERV3,"SET_ESBCR_NORMACQINTERV3",szNomAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_NOMACQINT3,COLOR_YELLOW);
					WriteToFileEx(194, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Nominal Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_ANOMACQINTERV3,"SET_ESBCR_ANOMACQINTERV3",szAnomAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_ANOMACQINT3,COLOR_YELLOW);
					WriteToFileEx(195, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Anomalous Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_LOPWACQINTERV3,"SET_ESBCR_LOPWACQINTERV3",szLPAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_LPACQINT3,COLOR_YELLOW);
					WriteToFileEx(196, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Low Power Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}
				break;

			case 8:
				SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4,psESBConfigTextRecord->chSerialNumber,sizeof(psESBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_ESBIR_SERIALNUM4,"SET_ESBIR_SERIALNUM4",szTempSN,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL4,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
			//fall through to process rest
			case 4:
				if (!CurrentSetParam(SET_ESBCR_MODEFLAGS4,"SET_ESBCR_MODEFLAGS4",szTempModeFlags,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_MODEFLAGS4,COLOR_YELLOW);
					WriteToFileEx(197, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Mode Flags (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_NORMACQINTERV4,"SET_ESBCR_NORMACQINTERV4",szNomAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_NOMACQINT4,COLOR_YELLOW);
					WriteToFileEx(198, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Nominal Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_ANOMACQINTERV4,"SET_ESBCR_ANOMACQINTERV4",szAnomAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_ANOMACQINT4,COLOR_YELLOW);
					WriteToFileEx(199, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Anomalous Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_ESBCR_LOPWACQINTERV4,"SET_ESBCR_LOPWACQINTERV4",szLPAcqInterv,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_LPACQINT4,COLOR_YELLOW);
					WriteToFileEx(200, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - ESB Low Power Acquisition Interval (52)\r\n");
					m_iESBInfo_Tab |= 0x0002;
				}
				break;
			}
		}

		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(ESBConfigTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(ESBConfigTextRecord));
		cBuff[sizeof(ESBConfigTextRecord) + 6] = NULL;

		if (!bStale)
		{
			switch (iColumn) {
			case 1:
			case 5:
				m_cESBConfigRecord1 = cBuff+6;
				break;
			case 2:
			case 6:
				m_cESBConfigRecord2 = cBuff+6;
				break;
			case 3:
			case 7:
				m_cESBConfigRecord3 = cBuff+6;
				break;
			case 4:
			case 8:
				m_cESBConfigRecord4 = cBuff+6;
				break;
			}
			if (m_pPropertySheet)
				m_pPropertySheet->OnESBConfigRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

		WriteToSOHFileX((AcquireRecordX*)&ESBConfigRec);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	} //if checksum good

	PostMessage(IDC_RECEIVE_RESULT,ESBCONFIGRECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnPSBConfigRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(PSBConfigTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//some temp buffers
		char szTemp1[32];
		char szTempModeFlags[8];
		char szTempHVSetPoint[8];
		char szTempHVConfState[2];
		char szTempSN[16];
		CString cTemp;

		//create a binary record 
		PSBConfigRecord PSBConfigRec;

		//get a pointer to the text record
		PSBConfigTextRecord* psPSBConfigTextRecord = (PSBConfigTextRecord*)szRecord;

		//get the length into binary record
		PSBConfigRec.bLength = sizeof(PSBConfigRecord);

		//get opcode into binary record
		memcpy(szTemp1,psPSBConfigTextRecord->chOpcode,sizeof(psPSBConfigTextRecord->chOpcode));
		szTemp1[sizeof(psPSBConfigTextRecord->chOpcode)] = NULL;
		sscanf(szTemp1,"%x",&PSBConfigRec.bOpcode);

		//get mode flags into binary record
		memcpy(szTempModeFlags,psPSBConfigTextRecord->chModeFlags,sizeof(psPSBConfigTextRecord->chModeFlags));
		szTempModeFlags[sizeof(psPSBConfigTextRecord->chModeFlags)]=NULL;
		sscanf(szTempModeFlags,"%hd",&PSBConfigRec.usModeFlags);

		//get HV Set point
		memcpy(szTempHVSetPoint,psPSBConfigTextRecord->cdfHVSetPoint,sizeof(psPSBConfigTextRecord->cdfHVSetPoint));
		szTempHVSetPoint[sizeof(psPSBConfigTextRecord->cdfHVSetPoint)]=NULL;
		sscanf(szTempHVSetPoint,"%f",&PSBConfigRec.fHVSetPoint);

		//get HV Configured state
		memcpy(szTempHVConfState,&psPSBConfigTextRecord->chHVConfigState,sizeof(psPSBConfigTextRecord->chHVConfigState));
		szTempHVConfState[sizeof(psPSBConfigTextRecord->chHVConfigState)]=NULL;
		sscanf(szTempHVConfState,"%hd",&PSBConfigRec.bHVConfigState);

		//get the julian seconds
		memcpy(szTemp1,psPSBConfigTextRecord->cdJulianSeconds,sizeof(psPSBConfigTextRecord->cdJulianSeconds));
		szTemp1[sizeof(psPSBConfigTextRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&PSBConfigRec.uiJulianSeconds);

		//get the serial number
		memcpy(szTempSN,psPSBConfigTextRecord->chSerialNumber,sizeof(psPSBConfigTextRecord->chSerialNumber));
		szTempSN[sizeof(psPSBConfigTextRecord->chSerialNumber)]=NULL;
		sscanf(szTempSN,"%2x%2x%2x%2x%2x%2x",
			&PSBConfigRec.bSerialNumber[0],
			&PSBConfigRec.bSerialNumber[1],
			&PSBConfigRec.bSerialNumber[2],
			&PSBConfigRec.bSerialNumber[3],
			&PSBConfigRec.bSerialNumber[4],
			&PSBConfigRec.bSerialNumber[5]);

		//binary version should be complete at this point

		//set the date and time
		CJulianTime cTime;
		cTime.Init(szTemp1);

		bool bStale = false;

		int iColumn = 0;
		if      (strncmp(szTempSN,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1),sizeof(psPSBConfigTextRecord->chSerialNumber)) == 0) iColumn = 1;
		else if (strncmp(szTempSN,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2),sizeof(psPSBConfigTextRecord->chSerialNumber)) == 0) iColumn = 2;
		else if (strncmp(szTempSN,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3),sizeof(psPSBConfigTextRecord->chSerialNumber)) == 0) iColumn = 3;
		else if (strncmp(szTempSN,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4),sizeof(psPSBConfigTextRecord->chSerialNumber)) == 0) iColumn = 4;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1))==0) iColumn = 5;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2))==0) iColumn = 6;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3))==0) iColumn = 7;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4))==0) iColumn = 8;

		if (m_uiPSBConfigRecordTime < cTime.TotalSeconds())
		{
			//newer than the last received
			bStale = false;

			//update the time of the latest record
			m_uiPSBConfigRecordTime = cTime.TotalSeconds();	

			//set date and time
			switch (iColumn) {
			case 5:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,psPSBConfigTextRecord->chSerialNumber,sizeof(psPSBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM1,"SET_PSBIR_SERIALNUM1",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
			//fall through to process rest
			case 1:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEC1,cTime.Date());
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEC1,cTime.Time());
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				CurrentSetParam(SET_PSBCR_TIME1,"SET_PSBCR_TIME1",cTemp,false);

				if (!CurrentSetParam(SET_PSBCR_MODEFLAGS1,"SET_PSBCR_MODEFLAGS1",szTempModeFlags,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_MODEFLAGS1,COLOR_YELLOW);
					WriteToFileEx(201, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB Mode Flags (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBCR_HVSETPOINT1,"SET_PSBCR_HVSETPOINT1",szTempHVSetPoint,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVSETPT1,COLOR_YELLOW);
					WriteToFileEx(202, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Set Point (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBCR_CONFIGSTATE1,"SET_PSBCR_CONFIGSTATE1",szTempHVConfState,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVCONFSTATE1,COLOR_YELLOW);
					WriteToFileEx(203, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Configured State (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;

			case 6:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,psPSBConfigTextRecord->chSerialNumber,sizeof(psPSBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM2,"SET_PSBIR_SERIALNUM2",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 2:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEC2,cTime.Date());
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEC2,cTime.Time());
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				CurrentSetParam(SET_PSBCR_TIME2,"SET_PSBCR_TIME2",cTemp,false);

				if (!CurrentSetParam(SET_PSBCR_MODEFLAGS2,"SET_PSBCR_MODEFLAGS2",szTempModeFlags,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_MODEFLAGS2,COLOR_YELLOW);
					WriteToFileEx(204, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB Mode Flags (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBCR_HVSETPOINT2,"SET_PSBCR_HVSETPOINT2",szTempHVSetPoint,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVSETPT2,COLOR_YELLOW);
					WriteToFileEx(205, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Set Point (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBCR_CONFIGSTATE2,"SET_PSBCR_CONFIGSTATE2",szTempHVConfState,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVCONFSTATE2,COLOR_YELLOW);
					WriteToFileEx(206, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Configured State (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;

			case 7:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,psPSBConfigTextRecord->chSerialNumber,sizeof(psPSBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM3,"SET_PSBIR_SERIALNUM3",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 3:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEC3,cTime.Date());
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEC3,cTime.Time());
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				CurrentSetParam(SET_PSBCR_TIME3,"SET_PSBCR_TIME3",cTemp,false);

				if (!CurrentSetParam(SET_PSBCR_MODEFLAGS3,"SET_PSBCR_MODEFLAGS3",szTempModeFlags,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_MODEFLAGS3,COLOR_YELLOW);
					WriteToFileEx(207, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB Mode Flags (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBCR_HVSETPOINT3,"SET_PSBCR_HVSETPOINT3",szTempHVSetPoint,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVSETPT3,COLOR_YELLOW);
					WriteToFileEx(208, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Set Point (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBCR_CONFIGSTATE3,"SET_PSBCR_CONFIGSTATE3",szTempHVConfState,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVCONFSTATE3,COLOR_YELLOW);
					WriteToFileEx(209, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Configured State (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;

			case 8:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,psPSBConfigTextRecord->chSerialNumber,sizeof(psPSBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM4,"SET_PSBIR_SERIALNUM4",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 4:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEC4,cTime.Date());
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEC4,cTime.Time());
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				CurrentSetParam(SET_PSBCR_TIME4,"SET_PSBCR_TIME4",cTemp,false);

				if (!CurrentSetParam(SET_PSBCR_MODEFLAGS4,"SET_PSBCR_MODEFLAGS4",szTempModeFlags,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_MODEFLAGS4,COLOR_YELLOW);
					WriteToFileEx(210, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB Mode Flags (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBCR_HVSETPOINT4,"SET_PSBCR_HVSETPOINT4",szTempHVSetPoint,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVSETPT4,COLOR_YELLOW);
					WriteToFileEx(211, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Set Point (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBCR_CONFIGSTATE4,"SET_PSBCR_CONFIGSTATE4",szTempHVConfState,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVCONFSTATE4,COLOR_YELLOW);
					WriteToFileEx(212, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - PSB HV Configured State (53)\r\n");
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;
			}
		}

		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(PSBConfigTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(PSBConfigTextRecord));
		cBuff[sizeof(PSBConfigTextRecord) + 6] = NULL;

		if (!bStale)
		{
			switch (iColumn) {
			case 1:
			case 5:
				m_cPSBConfigRecord1 = cBuff+6;
				break;
			case 2:
			case 6:
				m_cPSBConfigRecord2 = cBuff+6;
				break;
			case 3:
			case 7:
				m_cPSBConfigRecord3 = cBuff+6;
				break;
			case 4:
			case 8:
				m_cPSBConfigRecord4 = cBuff+6;
				break;
			}
			if (m_pPropertySheet)
				m_pPropertySheet->OnPSBConfigRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

		WriteToSOHFileX((AcquireRecordX*)&PSBConfigRec);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	} //if checksum good

	PostMessage(IDC_RECEIVE_RESULT,PSBCONFIGRECORD_OPCODE,CheckSum);
}

void
CInstrument::OnBMBConfigRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(BMBConfigTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//some temp buffers
		char szTemp1[32];
		char szTempSN[16];
		char szTempPwrOn[8];
		char szTempPwrOff[8];
		char szTempModeFlags[8];
		CString cTemp;

		//create a binary record 
		BMBConfigRecord BMBConfigRec;

		//get a pointer to the text record
		BMBConfigTextRecord* psBMBConfigTextRecord = (BMBConfigTextRecord*)szRecord;

		//get the length into binary record
		BMBConfigRec.bLength = sizeof(BMBConfigRec);

		//get opcode into binary record
		memcpy(szTemp1,psBMBConfigTextRecord->chOpcode,sizeof(psBMBConfigTextRecord->chOpcode));
		szTemp1[sizeof(psBMBConfigTextRecord->chOpcode)] = NULL;
		sscanf(szTemp1,"%x",&BMBConfigRec.bOpcode);

		//get mode flags into binary record
		memcpy(szTempModeFlags,psBMBConfigTextRecord->chModeFlags,sizeof(psBMBConfigTextRecord->chModeFlags));
		szTempModeFlags[sizeof(psBMBConfigTextRecord->chModeFlags)]=NULL;
		sscanf(szTempModeFlags,"%hd",&BMBConfigRec.usModeFlags);

		//get the power on status-reporting interval
		memcpy(szTempPwrOn,psBMBConfigTextRecord->cdPowerOnReportInter,sizeof(psBMBConfigTextRecord->cdPowerOnReportInter));
		szTempPwrOn[sizeof(psBMBConfigTextRecord->cdPowerOnReportInter)]=NULL;
		sscanf(szTempPwrOn,"%hd",&BMBConfigRec.usPowerOnReportInter);

		//get the power off status-reporting interval
		memcpy(szTempPwrOff,psBMBConfigTextRecord->cdPowerOffReportInterv,sizeof(psBMBConfigTextRecord->cdPowerOffReportInterv));
		szTempPwrOff[sizeof(psBMBConfigTextRecord->cdPowerOffReportInterv)]=NULL;
		sscanf(szTempPwrOff,"%hd",&BMBConfigRec.usPowerOffReportInterv);

		//get the julian seconds
		memcpy(szTemp1,psBMBConfigTextRecord->cdJulianSeconds,sizeof(psBMBConfigTextRecord->cdJulianSeconds));
		szTemp1[sizeof(psBMBConfigTextRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%d",&BMBConfigRec.uiJulianSeconds);

		//get the serial number
		memcpy(szTempSN,psBMBConfigTextRecord->chSerialNumber,sizeof(psBMBConfigTextRecord->chSerialNumber));
		szTempSN[sizeof(psBMBConfigTextRecord->chSerialNumber)]=NULL;
		sscanf(szTempSN,"%2x%2x%2x%2x%2x%2x",
			&BMBConfigRec.bSerialNumber[0],
			&BMBConfigRec.bSerialNumber[1],
			&BMBConfigRec.bSerialNumber[2],
			&BMBConfigRec.bSerialNumber[3],
			&BMBConfigRec.bSerialNumber[4],
			&BMBConfigRec.bSerialNumber[5]);

		//binary version should be complete at this point

		//set the date and time
		CJulianTime cTime;
		cTime.Init(szTemp1);

		bool bStale = false;

		int iColumn = 0;
		if      (strncmp(szTempSN,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1),sizeof(psBMBConfigTextRecord->chSerialNumber)) == 0) iColumn = 1;
		else if (strncmp(szTempSN,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2),sizeof(psBMBConfigTextRecord->chSerialNumber)) == 0) iColumn = 2;
		else if (strncmp(szTempSN,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3),sizeof(psBMBConfigTextRecord->chSerialNumber)) == 0) iColumn = 3;
		else if (strncmp(szTempSN,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4),sizeof(psBMBConfigTextRecord->chSerialNumber)) == 0) iColumn = 4;
		else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1))==0) iColumn = 5;
		else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2))==0) iColumn = 6;
		else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3))==0) iColumn = 7;
		else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4))==0) iColumn = 8;


		if (m_uiBMBConfigRecordTime < cTime.TotalSeconds())
		{
			//newer than the last received
			bStale = false;

			//update the time of the latest record
			m_uiBMBConfigRecordTime = cTime.TotalSeconds();	

			//set date and time
			switch (iColumn) {
			case 5:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,psBMBConfigTextRecord->chSerialNumber,sizeof(psBMBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_BMBIR_SERIALNUM1,"SET_BMBIR_SERIALNUM1",szTempSN,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 1:
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEC1,cTime.Date());
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEC1,cTime.Time());
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				CurrentSetParam(SET_BMBCR_TIME1,"SET_BMBCR_TIME1",cTemp,false);

				if (!CurrentSetParam(SET_BMBCR_MODEFLAGS1,"SET_BMBCR_MODEFLAGS1",szTempModeFlags,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_MODEFLAGS1,COLOR_YELLOW);
					WriteToFileEx(213, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Mode Flags (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_BMBCR_PONSTATINT1,"SET_BMBCR_PONSTATINT1",szTempPwrOn,false))
				{
					SetColor(PSBINFOPAGE,IDC_BMBINFO_PONRPT1,COLOR_YELLOW);
					WriteToFileEx(214, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power On Status Reporting Interval (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_BMBCR_POFFSTATINT1,"SET_BMBCR_POFFSTATINT1",szTempPwrOff,false))
				{
					SetColor(PSBINFOPAGE,IDC_BMBINFO_POFFRPT1,COLOR_YELLOW);
					WriteToFileEx(215, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power Off Status Reporting Interval (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;

			case 6:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,psBMBConfigTextRecord->chSerialNumber,sizeof(psBMBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_BMBIR_SERIALNUM2,"SET_BMBIR_SERIALNUM2",szTempSN,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 2:
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEC2,cTime.Date());
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEC2,cTime.Time());
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				CurrentSetParam(SET_BMBCR_TIME2,"SET_BMBCR_TIME2",cTemp,false);

				if (!CurrentSetParam(SET_BMBCR_MODEFLAGS2,"SET_BMBCR_MODEFLAGS2",szTempModeFlags,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_MODEFLAGS2,COLOR_YELLOW);
					WriteToFileEx(216, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Mode Flags (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_BMBCR_PONSTATINT2,"SET_BMBCR_PONSTATINT2",szTempPwrOn,false))
				{
					SetColor(PSBINFOPAGE,IDC_BMBINFO_PONRPT2,COLOR_YELLOW);
					WriteToFileEx(217, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power On Status Reporting Interval (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_BMBCR_POFFSTATINT2,"SET_BMBCR_POFFSTATINT2",szTempPwrOff,false))
				{
					SetColor(PSBINFOPAGE,IDC_BMBINFO_POFFRPT2,COLOR_YELLOW);
					WriteToFileEx(218, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power Off Status Reporting Interval (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;

			case 7:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,psBMBConfigTextRecord->chSerialNumber,sizeof(psBMBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_BMBIR_SERIALNUM3,"SET_BMBIR_SERIALNUM3",szTempSN,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 3:
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEC3,cTime.Date());
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEC3,cTime.Time());
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				CurrentSetParam(SET_BMBCR_TIME3,"SET_BMBCR_TIME3",cTemp,false);

				if (!CurrentSetParam(SET_BMBCR_MODEFLAGS3,"SET_BMBCR_MODEFLAGS3",szTempModeFlags,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_MODEFLAGS3,COLOR_YELLOW);
					WriteToFileEx(219, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Mode Flags (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_BMBCR_PONSTATINT3,"SET_BMBCR_PONSTATINT3",szTempPwrOn,false))
				{
					SetColor(PSBINFOPAGE,IDC_BMBINFO_PONRPT3,COLOR_YELLOW);
					WriteToFileEx(220, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power On Status Reporting Interval (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_BMBCR_POFFSTATINT3,"SET_BMBCR_POFFSTATINT3",szTempPwrOff,false))
				{
					SetColor(PSBINFOPAGE,IDC_BMBINFO_POFFRPT3,COLOR_YELLOW);
					WriteToFileEx(221, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power Off Status Reporting Interval (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;

			case 8:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,psBMBConfigTextRecord->chSerialNumber,sizeof(psBMBConfigTextRecord->chSerialNumber));
				if (!CurrentSetParam(SET_BMBIR_SERIALNUM4,"SET_BMBIR_SERIALNUM4",szTempSN,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 4:
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEC4,cTime.Date());
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEC4,cTime.Time());
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				CurrentSetParam(SET_BMBCR_TIME4,"SET_BMBCR_TIME4",cTemp,false);

				if (!CurrentSetParam(SET_BMBCR_MODEFLAGS4,"SET_BMBCR_MODEFLAGS4",szTempModeFlags,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_MODEFLAGS4,COLOR_YELLOW);
					WriteToFileEx(222, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Mode Flags (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_BMBCR_PONSTATINT4,"SET_BMBCR_PONSTATINT4",szTempPwrOn,false))
				{
					SetColor(PSBINFOPAGE,IDC_BMBINFO_PONRPT4,COLOR_YELLOW);
					WriteToFileEx(223, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power On Status Reporting Interval (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_BMBCR_POFFSTATINT4,"SET_BMBCR_POFFSTATINT4",szTempPwrOff,false))
				{
					SetColor(PSBINFOPAGE,IDC_BMBINFO_POFFRPT4,COLOR_YELLOW);
					WriteToFileEx(224, TO_PFM, TYPE_INST, &cTime, "00000 CONFIGURATION MISMATCH - BMB Power Off Status Reporting Interval (54)\r\n");
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;
			}
		}

		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(BMBConfigTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(BMBConfigTextRecord));
		cBuff[sizeof(BMBConfigTextRecord) + 6] = NULL;

		if (!bStale)
		{
			switch (iColumn) {
			case 1:
			case 5:
				m_cBMBConfigRecord1 = cBuff+6;
				break;
			case 2:
			case 6:
				m_cBMBConfigRecord2 = cBuff+6;
				break;
			case 3:
			case 7:
				m_cBMBConfigRecord3 = cBuff+6;
				break;
			case 4:
			case 8:
				m_cBMBConfigRecord4 = cBuff+6;
				break;
			}
			if (m_pPropertySheet)
				m_pPropertySheet->OnBMBConfigRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

		WriteToSOHFileX((AcquireRecordX*)&BMBConfigRec);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	} //if checksum good

	PostMessage(IDC_RECEIVE_RESULT,BMBCONFIGRECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnuGStatusRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(uGStatusTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//some temporary buffers
		char szTemp1[32];
		CString cTemp;

		//create a binary record to send to the SOH file
		uGStatusRecord uGStatusRec;

		//get a pointer to a GeneralConfigTextRecord
		uGStatusTextRecord* pszRecord = (uGStatusTextRecord*)szRecord;

		//get length and opcode and place in binary record
		uGStatusRec.bLength = sizeof(uGStatusRecord);
		strncpy(szTemp1,(char*)pszRecord->chOpcode,sizeof(pszRecord->chOpcode));
		szTemp1[sizeof(pszRecord->chOpcode)] = NULL;
		sscanf(szTemp1,"%x",&uGStatusRec.bOpcode);

 		//get julian time and put it in the binary record
		strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
		szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&uGStatusRec.uiJulianSeconds);

		//set the date & time
		CJulianTime cTime;
		cTime.Init(uGStatusRec.uiJulianSeconds);

		bool bStale = true;

		if (m_duGStatusRecordTime < cTime.TotalSeconds()+uGStatusRec.bJulianSec10ths/10.0)	
		{	
			//newer than the last received
			bStale = false;

			//update the time of the latest record
			m_duGStatusRecordTime = cTime.TotalSeconds()+uGStatusRec.bJulianSec10ths/10.0;	

			//set date and time
			SetString(UGINFOPAGE,IDC_UGINFO_DATADATES,cTime.Date());
			cTemp = cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			SetString(UGINFOPAGE,IDC_UGINFO_DATATIMES,cTemp);
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_UGSR_TIME,"SET_UGSR_TIME",cTemp,false);
		}

		//set the 5.0 volt supply
		strncpy(szTemp1,(char*)pszRecord->cdf50VSupply,sizeof(pszRecord->cdf50VSupply));
		szTemp1[sizeof(pszRecord->cdf50VSupply)]=NULL;
		sscanf(szTemp1,"%f",&uGStatusRec.f50VSupply);

		if (!bStale)
		{
			//+5 volts
			bool bCurrentRed = false;
			bool bCurrentLow = false;
			bool bCurrentHigh= false;

			m_fCurP5 = uGStatusRec.f50VSupply;
			if (m_bFirstP5)
			{
				m_bFirstP5 = false;
				m_fLowP5 = m_fHighP5 = m_fCurP5;
			}
			if (m_fCurP5 < m_fLowP5)
			{
				bCurrentLow = true;
				m_fLowP5 = m_fCurP5;
			}
			else
				bCurrentLow = false;
			if (m_fCurP5 > m_fHighP5)
			{
				bCurrentHigh = true;
				m_fHighP5= m_fCurP5;
			}
			else
				bCurrentHigh = false;
			if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
			{
				bCurrentRed = true;			
				SetColor(UGINFOPAGE,IDC_UGINFO_50VSUPPLYS,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,COLOR_RED);
				if (m_bP5VoltOk)
				{
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 5 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
					FlagOthr(1,true);
				}
				m_bP5VoltOk = false;
	
				m_iUGInfo_Tab |= 0x0001;
			}
			else
			{
				bCurrentRed = false;
				if (!m_bP5VoltOk)
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 5 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
				m_bP5VoltOk = true;
			}

			if (bCurrentRed && bCurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,COLOR_RED);

			if (bCurrentRed && bCurrentHigh && (m_fHighP5 >= m_fSup_P05H))// || (m_fHighP5 >= m_fSup_P05H))
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,COLOR_RED);

			sprintf(szTemp1,"%4.1f",m_fLowP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,szTemp1);

			sprintf(szTemp1,"%4.1f",m_fCurP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,szTemp1);
			SetString(UGINFOPAGE,IDC_UGINFO_50VSUPPLYS,szTemp1);
			CurrentSetParam(SET_UGSR_50V,"SET_UGSR_50V",szTemp1,false);

			sprintf(szTemp1,"%4.1f",m_fHighP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,szTemp1);
		}
	
		//set the 3.3 volt supply (uses the -15 from microGRAND)
		strncpy(szTemp1,(char*)pszRecord->cdf33VSupply,sizeof(pszRecord->cdf33VSupply));
		szTemp1[sizeof(pszRecord->cdf33VSupply)]=NULL;
		sscanf(szTemp1,"%f",&uGStatusRec.f33VSupply);
		if (!bStale)
		{
			//+33 volts
			bool bCurrentRed = false;
			bool bCurrentLow = false;
			bool bCurrentHigh= false;

			m_fCurP33 = uGStatusRec.f33VSupply;
			if (m_bFirstP33)
			{
				m_bFirstP33 = false;
				m_fLowP33 = m_fHighP33 = m_fCurP33;
			}
			if (m_fCurP33 < m_fLowP33)
			{
				bCurrentLow = true;
				m_fLowP33 = m_fCurP33;
			}
			else
				bCurrentLow = false;
			if (m_fCurP33 > m_fHighP33)
			{
				bCurrentHigh = true;
				m_fHighP33 = m_fCurP33;
			}
			else
				bCurrentHigh = false;
			if ((m_fCurP33 <= m_fSup_P33L) || (m_fCurP33 >= m_fSup_P33H))
			{
				bCurrentRed = true;			
				SetColor(UGINFOPAGE,IDC_UGINFO_33VSUPPLYS,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P33VOLT2,COLOR_RED);
				if (m_bP33VoltOk)
				{
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 3.3 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
					FlagOthr(1,true);
				}
				m_bP33VoltOk = false;

				m_iUGInfo_Tab |= 0x0001;
			}
			else
			{
				bCurrentRed = false;
				if (!m_bP33VoltOk)
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 3.3 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
				m_bP33VoltOk = true;
			}

			if (bCurrentRed && bCurrentLow && (m_fLowP33 <= m_fSup_P33L))// || (m_fLowP5 >= m_fSup_P05H))
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P33VOLT1,COLOR_RED);

			if (bCurrentRed && bCurrentHigh && (m_fHighP33 >= m_fSup_P33H))// || (m_fHighP5 >= m_fSup_P05H))
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P33VOLT3,COLOR_RED);

			sprintf(szTemp1,"%4.1f",m_fLowP33);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P33VOLT1,szTemp1);

			sprintf(szTemp1,"%4.1f",m_fCurP25);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P33VOLT2,szTemp1);
			SetString(UGINFOPAGE,IDC_UGINFO_33VSUPPLYS,szTemp1);
			CurrentSetParam(SET_UGSR_33V,"SET_UGSR_33V",szTemp1,false);

			sprintf(szTemp1,"%4.1f",m_fHighP33);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P33VOLT3,szTemp1);
		}

		//set the 2.5 volt supply (uses the +15 from microGRAND)
		strncpy(szTemp1,(char*)pszRecord->cdf25VSupply,sizeof(pszRecord->cdf25VSupply));
		szTemp1[sizeof(pszRecord->cdf25VSupply)]=NULL;
		sscanf(szTemp1,"%f",&uGStatusRec.f25VSupply);

		if (!bStale)
		{
			//+2.5 volts
			bool bCurrentRed = false;
			bool bCurrentLow = false;
			bool bCurrentHigh= false;

			m_fCurP25 = uGStatusRec.f25VSupply;
			if (m_bFirstP25)
			{
				m_bFirstP25 = false;
				m_fLowP25 = m_fHighP25 = m_fCurP25;
			}
			if (m_fCurP25 < m_fLowP25)
			{
				bCurrentLow = true;
				m_fLowP25 = m_fCurP25;
			}
			else
				bCurrentLow = false;
			if (m_fCurP25 > m_fHighP25)
			{
				bCurrentHigh = true;
				m_fHighP25 = m_fCurP25;
			}
			else
				bCurrentHigh = false;
			if ((m_fCurP25 <= m_fSup_P25L) || (m_fCurP25 >= m_fSup_P25H))
			{
				bCurrentRed = true;			
				SetColor(UGINFOPAGE,IDC_UGINFO_25VSUPPLYS,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P25VOLT2,COLOR_RED);
				if (m_bP25VoltOk)
				{
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 2.5 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
					FlagOthr(1,true);
				}
				m_bP25VoltOk = false;

				m_iUGInfo_Tab |= 0x0001;
			}
			else
			{
				bCurrentRed = false;
				if (!m_bP25VoltOk)
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PLUS 2.5 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
				m_bP25VoltOk = true;
			}

			if (bCurrentRed && bCurrentLow && (m_fLowP25 <= m_fSup_P25L))// || (m_fLowP5 >= m_fSup_P05H))
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P25VOLT1,COLOR_RED);

			if (bCurrentRed && bCurrentHigh && (m_fHighP25 >= m_fSup_P25H))// || (m_fHighP5 >= m_fSup_P05H))
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P25VOLT3,COLOR_RED);

			sprintf(szTemp1,"%4.1f",m_fLowP25);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P25VOLT1,szTemp1);

			sprintf(szTemp1,"%4.1f",m_fCurP25);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P25VOLT2,szTemp1);
			SetString(UGINFOPAGE,IDC_UGINFO_25VSUPPLYS,szTemp1);
			CurrentSetParam(SET_UGSR_25V,"SET_UGSR_25V",szTemp1,false);

			sprintf(szTemp1,"%4.1f",m_fHighP25);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P25VOLT3,szTemp1);
		}

		//set the PHD volt supply (uses the BAT from microGRAND)
		strncpy(szTemp1,(char*)pszRecord->cdfPHDVoltage,sizeof(pszRecord->cdfPHDVoltage));
		szTemp1[sizeof(pszRecord->cdfPHDVoltage)]=NULL;
		sscanf(szTemp1,"%f",&uGStatusRec.fPHDVoltage);

		if (!bStale)
		{
			//PHD volts
			bool bCurrentRed = false;
			bool bCurrentLow = false;
			bool bCurrentHigh= false;

			m_fCurBV = uGStatusRec.fPHDVoltage;
			if (m_bFirstBV)
			{
				m_bFirstBV = false;
				m_fLowBV = m_fHighBV = m_fCurBV;
			}
			if (m_fCurBV < m_fLowBV)
			{
				bCurrentLow = true;
				m_fLowBV = m_fCurBV;
			}
			else
				bCurrentLow = false;
			if (m_fCurBV > m_fHighBV)
			{
				bCurrentHigh = true;
				m_fHighBV = m_fCurBV;
			}
			else
				bCurrentHigh = false;
			if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
			{
				bCurrentRed = true;			
				SetColor(UGINFOPAGE,IDC_UGINFO_PHDVS,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,COLOR_RED);
				if (m_bBatteryOk)
				{
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PHD VOLT OUT OF TOLERANCE (1b)\r\n");
					FlagOthr(1,true);
				}
				m_bBatteryOk = false;

				m_iUGInfo_Tab |= 0x0001;
			}
			else
			{
				bCurrentRed = false;
				if (!m_bBatteryOk)
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &cTime, "00000 STATUS CHANGE - PHD VOLT NOT OUT OF TOLERANCE (1b)\r\n");
				m_bBatteryOk = true;
			}

			if (bCurrentRed && bCurrentLow && (m_fLowBV <= m_fBatVoltL))// || (m_fLowP5 >= m_fSup_P05H))
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,COLOR_RED);

			if (bCurrentRed && bCurrentHigh && (m_fHighBV >= m_fBatVoltH))// || (m_fHighP5 >= m_fSup_P05H))
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,COLOR_RED);

			sprintf(szTemp1,"%4.1f",m_fLowBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,szTemp1);

			sprintf(szTemp1,"%4.1f",m_fCurBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,szTemp1);
			SetString(UGINFOPAGE,IDC_UGINFO_PHDVS,szTemp1);
			CurrentSetParam(SET_UGSR_PHDVOLT,"SET_UGSR_PHDVOLT",szTemp1,false);

			sprintf(szTemp1,"%4.1f",m_fHighBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,szTemp1);
		}

		//set status byte 1
		strncpy(szTemp1,(char*)pszRecord->chStatus1Byte,sizeof(pszRecord->chStatus1Byte));
		szTemp1[sizeof(pszRecord->chStatus1Byte)]=NULL;
		sscanf(szTemp1,"%x",&uGStatusRec.bStatus1Byte);

		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_STATUS1S,szTemp1);
			CurrentSetParam(SET_UGSR_STATUSBYTE1,"SET_UGSR_STATUSBYTE1",szTemp1,false);
		}
	
		//set status byte 2
		strncpy(szTemp1,(char*)pszRecord->chStatus2Byte,sizeof(pszRecord->chStatus2Byte));
		szTemp1[sizeof(pszRecord->chStatus2Byte)]=NULL;
		sscanf(szTemp1,"%x",&uGStatusRec.bStatus2Byte);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_STATUS2S,szTemp1);
			CurrentSetParam(SET_UGSR_STATUSBYTE2,"SET_UGSR_STATUSBYTE2",szTemp1,false);
		}

		//set status byte 3
		strncpy(szTemp1,(char*)pszRecord->chStatus3Byte,sizeof(pszRecord->chStatus3Byte));
		szTemp1[sizeof(pszRecord->chStatus3Byte)]=NULL;
		sscanf(szTemp1,"%x",&uGStatusRec.bStatus3Byte);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_STATUS3S,szTemp1);
			CurrentSetParam(SET_UGSR_STATUSBYTE3,"SET_UGSR_STATUSBYTE3",szTemp1,false);
		}

		//set status byte 4
		strncpy(szTemp1,(char*)pszRecord->chStatus4Byte,sizeof(pszRecord->chStatus4Byte));
		szTemp1[sizeof(pszRecord->chStatus1Byte)]=NULL;
		sscanf(szTemp1,"%x",&uGStatusRec.bStatus4Byte);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_STATUS4S,szTemp1);
			CurrentSetParam(SET_UGSR_STATUSBYTE4,"SET_UGSR_STATUSBYTE4",szTemp1,false);
		}

		//set cpu up time
		strncpy(szTemp1,(char*)pszRecord->chCPUUpTime,sizeof(pszRecord->chCPUUpTime));
		szTemp1[sizeof(pszRecord->chCPUUpTime)]=NULL;
		sscanf(szTemp1,"%10u",&uGStatusRec.uiCPUUpTime);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CPUUPS,szTemp1);
			CurrentSetParam(SET_UGSR_CPUUPTIME,"SET_UGSR_CPUUPTIME",szTemp1,false);
		}

		//set cpu temperature
		strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
		sscanf(szTemp1,"%f",&uGStatusRec.fCPUTemp);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_CPUTEMPS,szTemp1);
			CurrentSetParam(SET_UGSR_CPUTEMP,"SET_UGSR_CPUTEMP",szTemp1,false);
		}

		//set board temperature
		strncpy(szTemp1,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTemp1[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
		sscanf(szTemp1,"%f",&uGStatusRec.fBOARDTemp);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_BOARDTEMPS,szTemp1);
			CurrentSetParam(SET_UGSR_BOARDTEMP,"SET_UGSR_BOARDTEMP",szTemp1,false);
		}

		//set reserved
		strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTemp1[sizeof(pszRecord->chReserved)]=NULL;
		sscanf(szTemp1,"%8x",&uGStatusRec.uiReserved);

		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_RESERVEDS,szTemp1);
			CurrentSetParam(SET_UGSR_RESERVED,"SET_UGSR_RESERVED",szTemp1,false);
		}

		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(uGStatusTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(uGStatusTextRecord));
		cBuff[sizeof(uGStatusTextRecord) + 6] = NULL;

		if (!bStale)
		{
			m_cUGStatusRecord = cBuff+6;
			if (m_pPropertySheet)
				m_pPropertySheet->OnuGStatusRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

		WriteToSOHFileX((AcquireRecordX*)&uGStatusRec);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	} //if checksum good

	PostMessage(IDC_RECEIVE_RESULT,UGSTATUSRECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnPSBStatusRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(PSBStatusTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		//some temporary buffers
		char szTemp1[32];
		CString cTemp;

		//create a binary record to send to the SOH file
		PSBStatusRecord PSBStatusRec;

		//get a pointer to a TextRecord
		PSBStatusTextRecord* pszRecord = (PSBStatusTextRecord*)szRecord;

		//get length and opcode and place in binary record
		PSBStatusRec.bLength = sizeof(PSBStatusRecord);
		strncpy(szTemp1,(char*)pszRecord->chOpcode,sizeof(pszRecord->chOpcode));
		szTemp1[sizeof(pszRecord->chOpcode)] = NULL;
		sscanf(szTemp1,"%x",&PSBStatusRec.bOpcode);

 		//get julian time and put it in the binary record
		strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
		szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&PSBStatusRec.uiJulianSeconds);

		//get the fractional part of the time
		szTemp1[0] = pszRecord->cdJulianSec10ths;
		szTemp1[1] = NULL;
		sscanf(szTemp1,"%hu",&PSBStatusRec.bJulianSec10ths);

		//set the date & time
		CJulianTime cTime;
		cTime.Init(PSBStatusRec.uiJulianSeconds);

		bool bStale = true;

		char szTempSN[sizeof(pszRecord->chSerialNumber)+1];
		char szTempEPSV[sizeof(pszRecord->cdfExternPWRSupply)+1];
		char szTempStatusByte[sizeof(pszRecord->chStatusByte)+1];
		char szTemp50VOut[sizeof(pszRecord->cdf50VOutput)+1];
		char szTemp33VOut[sizeof(pszRecord->cdf33VOutput)+1];
		char szTempHVVOut[sizeof(pszRecord->cdfHVOutputV)+1];
		char szTempHVCOut[sizeof(pszRecord->cdfHVOutputC)+1];
		char szTempCPUTemp[sizeof(pszRecord->cdfCPUTemp)+1];
		char szTempBOARDTemp[sizeof(pszRecord->cdfBOARDTemp)+1];
		char szTempReserve[sizeof(pszRecord->chReserved)+1];

		//get the serial number aND figure which column it is for
		int iColumn = 0;
		if      (strncmp((char*)&pszRecord->chSerialNumber,
			GetString(PSBINFOPAGE,IDC_ESBINFO_SERIAL1),
			sizeof(pszRecord->chSerialNumber)) == 0) 
			iColumn = 1;
		else if (strncmp((char*)&pszRecord->chSerialNumber,
			GetString(PSBINFOPAGE,IDC_ESBINFO_SERIAL2),
			sizeof(pszRecord->chSerialNumber)) == 0) 
			iColumn = 2;
		else if (strncmp((char*)&pszRecord->chSerialNumber,
			GetString(PSBINFOPAGE,IDC_ESBINFO_SERIAL3),
			sizeof(pszRecord->chSerialNumber)) == 0) 
			iColumn = 3;
		else if (strncmp((char*)&pszRecord->chSerialNumber,
			GetString(PSBINFOPAGE,IDC_ESBINFO_SERIAL4),
			sizeof(pszRecord->chSerialNumber)) == 0) 
			iColumn = 4;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1))==0) iColumn = 5;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2))==0) iColumn = 6;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3))==0) iColumn = 7;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4))==0) iColumn = 8;

		//get the serial number
		memcpy(szTempSN,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTempSN[sizeof(pszRecord->chSerialNumber)] = NULL;
		sscanf(szTempSN,"%2hx%2hx%2hx%2hx%2hx%2hx",
			&PSBStatusRec.bSerialNumber[0],
			&PSBStatusRec.bSerialNumber[1],
			&PSBStatusRec.bSerialNumber[2],
			&PSBStatusRec.bSerialNumber[3],
			&PSBStatusRec.bSerialNumber[4],
			&PSBStatusRec.bSerialNumber[5]
			);

		//get the external ps voltage
		memcpy(szTempEPSV,pszRecord->cdfExternPWRSupply,sizeof(pszRecord->cdfExternPWRSupply));
		szTempEPSV[sizeof(pszRecord->cdfExternPWRSupply)] = NULL;
		sscanf(szTempEPSV,"%f",&PSBStatusRec.fExternPWRSupply);

		//get the status byte
		memcpy(szTempStatusByte,pszRecord->chStatusByte,sizeof(pszRecord->chStatusByte));
		szTempStatusByte[sizeof(pszRecord->chStatusByte)] = NULL;
		sscanf(szTempStatusByte,"%hx",&PSBStatusRec.bStatusByte);

		//get the 5.0 v output
		memcpy(szTemp50VOut,pszRecord->cdf50VOutput,sizeof(pszRecord->cdf50VOutput));
		szTemp50VOut[sizeof(pszRecord->cdf50VOutput)] = NULL;
		sscanf(szTemp50VOut,"%f",&PSBStatusRec.f50VOutput);

		//get the 3.3 v output
		memcpy(szTemp33VOut,pszRecord->cdf33VOutput,sizeof(pszRecord->cdf33VOutput));
		szTemp33VOut[sizeof(pszRecord->cdf33VOutput)] = NULL;
		sscanf(szTemp33VOut,"%f",&PSBStatusRec.f33VOutput);

		//get the HV output V
		memcpy(szTempHVVOut,pszRecord->cdfHVOutputV,sizeof(pszRecord->cdfHVOutputV));
		szTempHVVOut[sizeof(pszRecord->cdfHVOutputV)] = NULL;
		sscanf(szTempHVVOut,"%f",&PSBStatusRec.fHVOutputV);

		//get the HV output C
		memcpy(szTempHVCOut,pszRecord->cdfHVOutputC,sizeof(pszRecord->cdfHVOutputC));
		szTempHVCOut[sizeof(pszRecord->cdfHVOutputC)] = NULL;
		sscanf(szTempHVCOut,"%f",&PSBStatusRec.fHVOutputC);

		//get the CPU Temp
		memcpy(szTempCPUTemp,pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTempCPUTemp[sizeof(pszRecord->cdfCPUTemp)] = NULL;
		sscanf(szTempCPUTemp,"%f",&PSBStatusRec.fCPUTemp);

		//get the Board Temp
		memcpy(szTempBOARDTemp,pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTempBOARDTemp[sizeof(pszRecord->cdfBOARDTemp)] = NULL;
		sscanf(szTempBOARDTemp,"%f",&PSBStatusRec.fBOARDTemp);

		//get the Reserved
		memcpy(szTempReserve,pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTempReserve[sizeof(pszRecord->chReserved)] = NULL;
		sscanf(szTempReserve,"%x",&PSBStatusRec.uiReserved);

		if (m_duPSBStatusRecordTime < cTime.TotalSeconds()+PSBStatusRec.bJulianSec10ths/10.0)	
		{	
			//newer than the last received
			bStale = false;

			//update the time of the latest record
			m_duPSBStatusRecordTime = cTime.TotalSeconds()+PSBStatusRec.bJulianSec10ths/10.0;	

			//set date and time
			switch (iColumn) {
			case 5:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM1,"SET_PSBIR_SERIALNUM1",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 1:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATES1,cTime.Date());
				cTemp = cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMES1,cTemp);
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_PSBSR_TIME1,"SET_PSBSR_TIME1",cTemp,false);

				if (!CurrentSetParam(SET_PSBSR_EXTERNPSVOLTS1,"SET_PSBSR_EXTERNPSVOLTS1",szTempEPSV,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_EXTPSV1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_STATUS1,"SET_PSBSR_STATUS1",szTempStatusByte,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_STATUS1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_50VOLTOUT1,"SET_PSBSR_50VOLTOUT1",szTemp50VOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_50OUT1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_33VOLTOUT1,"SET_PSBSR_33VOLTOUT1",szTemp33VOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_33OUT1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_HVVOLTOUT1,"SET_PSBSR_HVVOLTOUT1",szTempHVVOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVOUT1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_HVCURROUT1,"SET_PSBSR_HVCURROUT1",szTempHVCOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_CPUTEMP1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_CPUTEMPERATURE1,"SET_PSBSR_CPUTEMPERATURE1",szTempCPUTemp,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_BOARDTEMPERATURE1,"SET_PSBSR_BOARDTEMPERATURE1",szTempBOARDTemp,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_RESERVED1,"SET_PSBSR_RESERVED1",szTempReserve,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_RESERVE1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				break;

			case 6:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM2,"SET_PSBIR_SERIALNUM2",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 2:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATES2,cTime.Date());
				cTemp = cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMES2,cTemp);
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_PSBSR_TIME2,"SET_PSBSR_TIME2",cTemp,false);

				if (!CurrentSetParam(SET_PSBSR_EXTERNPSVOLTS2,"SET_PSBSR_EXTERNPSVOLTS2",szTempEPSV,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_EXTPSV2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_STATUS2,"SET_PSBSR_STATUS2",szTempStatusByte,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_STATUS2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_50VOLTOUT2,"SET_PSBSR_50VOLTOUT2",szTemp50VOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_50OUT2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_33VOLTOUT2,"SET_PSBSR_33VOLTOUT2",szTemp33VOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_33OUT2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_HVVOLTOUT2,"SET_PSBSR_HVVOLTOUT2",szTempHVVOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVOUT2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_HVCURROUT2,"SET_PSBSR_HVCURROUT2",szTempHVCOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_CPUTEMP2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_CPUTEMPERATURE2,"SET_PSBSR_CPUTEMPERATURE2",szTempCPUTemp,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_BOARDTEMPERATURE2,"SET_PSBSR_BOARDTEMPERATURE2",szTempBOARDTemp,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_RESERVED2,"SET_PSBSR_RESERVED2",szTempReserve,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_RESERVE2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;

			case 7:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM3,"SET_PSBIR_SERIALNUM3",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 3:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATES3,cTime.Date());
				cTemp = cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMES3,cTemp);
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_PSBSR_TIME3,"SET_PSBSR_TIME3",cTemp,false);

				if (!CurrentSetParam(SET_PSBSR_EXTERNPSVOLTS3,"SET_PSBSR_EXTERNPSVOLTS3",szTempEPSV,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_EXTPSV3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_STATUS3,"SET_PSBSR_STATUS3",szTempStatusByte,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_STATUS3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_50VOLTOUT3,"SET_PSBSR_50VOLTOUT3",szTemp50VOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_50OUT3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_33VOLTOUT3,"SET_PSBSR_33VOLTOUT3",szTemp33VOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_33OUT3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_HVVOLTOUT3,"SET_PSBSR_HVVOLTOUT3",szTempHVVOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVOUT3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_HVCURROUT3,"SET_PSBSR_HVCURROUT3",szTempHVCOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_CPUTEMP3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_CPUTEMPERATURE3,"SET_PSBSR_CPUTEMPERATURE3",szTempCPUTemp,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_BOARDTEMPERATURE3,"SET_PSBSR_BOARDTEMPERATURE3",szTempBOARDTemp,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_RESERVED3,"SET_PSBSR_RESERVED3",szTempReserve,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_RESERVE3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;

			case 8:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM4,"SET_PSBIR_SERIALNUM4",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 4:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATES4,cTime.Date());
				cTemp = cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMES4,cTemp);
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_PSBSR_TIME4,"SET_PSBSR_TIME4",cTemp,false);

				if (!CurrentSetParam(SET_PSBSR_EXTERNPSVOLTS4,"SET_PSBSR_EXTERNPSVOLTS4",szTempEPSV,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_EXTPSV4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_STATUS4,"SET_PSBSR_STATUS4",szTempStatusByte,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_STATUS4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_50VOLTOUT4,"SET_PSBSR_50VOLTOUT4",szTemp50VOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_50OUT4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_33VOLTOUT4,"SET_PSBSR_33VOLTOUT4",szTemp33VOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_33OUT4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_HVVOLTOUT4,"SET_PSBSR_HVVOLTOUT4",szTempHVVOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_HVOUT4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_HVCURROUT4,"SET_PSBSR_HVCURROUT4",szTempHVCOut,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_CPUTEMP4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_CPUTEMPERATURE4,"SET_PSBSR_CPUTEMPERATURE4",szTempCPUTemp,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_BOARDTEMPERATURE4,"SET_PSBSR_BOARDTEMPERATURE4",szTempBOARDTemp,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_BOARDTEMP4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}

				if (!CurrentSetParam(SET_PSBSR_RESERVED4,"SET_PSBSR_RESERVED4",szTempReserve,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_RESERVE4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;
			}
		}

		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(PSBStatusTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(PSBStatusTextRecord));
		cBuff[sizeof(PSBStatusTextRecord) + 6] = NULL;

		if (!bStale)
		{
			switch (iColumn) {
			case 1:
			case 5:
				m_cPSBStatusRecord1 = cBuff+6;
				break;
			case 2:
			case 6:
				m_cPSBStatusRecord2 = cBuff+6;
				break;
			case 3:
			case 7:
				m_cPSBStatusRecord3 = cBuff+6;
				break;
			case 4:
			case 8:
				m_cPSBStatusRecord4 = cBuff+6;
				break;
			}
			if (m_pPropertySheet)
				m_pPropertySheet->OnPSBStatusRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

		WriteToSOHFileX((AcquireRecordX*)&PSBStatusRec);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	} //if checksum good

	PostMessage(IDC_RECEIVE_RESULT,PSBSTATUSRECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnBMBStatusRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(BMBStatusTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		//some temporary buffers
		char szTemp1[32];
		CString cTemp;

		//create a binary record to send to the SOH file
		BMBStatusRecord BMBStatusRec;

		//get a pointer to a text record
		BMBStatusTextRecord* pszRecord = (BMBStatusTextRecord*)szRecord;

		//get length and opcode and place in binary record
		BMBStatusRec.bLength = sizeof(BMBStatusRecord);
		strncpy(szTemp1,(char*)pszRecord->chOpcode,sizeof(pszRecord->chOpcode));
		szTemp1[sizeof(pszRecord->chOpcode)] = NULL;
		sscanf(szTemp1,"%x",&BMBStatusRec.bOpcode);

 		//get julian time and put it in the binary record
		strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
		szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&BMBStatusRec.uiJulianSeconds);

		//get the fractional part of the time
		szTemp1[0] = pszRecord->cdJulianSec10ths;
		szTemp1[1] = NULL;
		sscanf(szTemp1,"%hu",&BMBStatusRec.bJulianSec10ths);

		//set the date & time
		CJulianTime cTime;
		cTime.Init(BMBStatusRec.uiJulianSeconds);

		bool bStale = true;

//		char szJulianSeconds[sizeof(pszRecord->cdJulianSeconds)+1];
//		char szJulianSec10ths[sizeof(pszRecord->cdJulianSec10ths)+1];
		char szfChargeCurrent[sizeof(pszRecord->cdfChargeCurrent)+1];
		char szfDischargeCurr[sizeof(pszRecord->cdfDischargeCurr)+1];
		char szChDisCycles[sizeof(pszRecord->cdChDisCycles)+1];
		char szfCPUTemp[sizeof(pszRecord->cdfCPUTemp)+1];
		char szfBOARDTemp[sizeof(pszRecord->cdfBOARDTemp)+1];
		char szTimeOnBatt[sizeof(pszRecord->cdTimeOnBatt)+1];
		char szfBattTemp[sizeof(pszRecord->cdfBattTemp)+1];
		char szBattStatus[sizeof(pszRecord->chBattStatus)+1];
		char szSerialNumber[sizeof(pszRecord->chSerialNumber)+1];
		char szReserved[sizeof(pszRecord->chReserved)+1];

		//get the serial number aAND figure which column it is for
		int iColumn = 0;
		if      (strncmp((char*)&pszRecord->chSerialNumber,
			GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1),
			sizeof(pszRecord->chSerialNumber)) == 0) 
			iColumn = 1;
		else if (strncmp((char*)&pszRecord->chSerialNumber,
			GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2),
			sizeof(pszRecord->chSerialNumber)) == 0) 
			iColumn = 2;
		else if (strncmp((char*)&pszRecord->chSerialNumber,
			GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3),
			sizeof(pszRecord->chSerialNumber)) == 0) 
			iColumn = 3;
		else if (strncmp((char*)&pszRecord->chSerialNumber,
			GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4),
			sizeof(pszRecord->chSerialNumber)) == 0) 
			iColumn = 4;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1))==0) iColumn = 5;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2))==0) iColumn = 6;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3))==0) iColumn = 7;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4))==0) iColumn = 8;

		//get the serial number
		memcpy(szSerialNumber,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szSerialNumber[sizeof(pszRecord->chSerialNumber)] = NULL;
		sscanf(szSerialNumber,"%2hx%2hx%2hx%2hx%2hx%2hx",
			&BMBStatusRec.bSerialNumber[0],
			&BMBStatusRec.bSerialNumber[1],
			&BMBStatusRec.bSerialNumber[2],
			&BMBStatusRec.bSerialNumber[3],
			&BMBStatusRec.bSerialNumber[4],
			&BMBStatusRec.bSerialNumber[5]
			);

		//get the Charge current
		memcpy(szfChargeCurrent,pszRecord->cdfChargeCurrent,sizeof(pszRecord->cdfChargeCurrent));
		szfChargeCurrent[sizeof(pszRecord->cdfChargeCurrent)] = NULL;
		sscanf(szfChargeCurrent,"%f",&BMBStatusRec.fChargeCurrent);

		//get the Discharge Current
		memcpy(szfDischargeCurr,pszRecord->cdfDischargeCurr,sizeof(pszRecord->cdfDischargeCurr));
		szfDischargeCurr[sizeof(pszRecord->cdfDischargeCurr)] = NULL;
		sscanf(szfDischargeCurr,"%f",&BMBStatusRec.fDischargeCurr);

		//get the C/D Cycles
		memcpy(szChDisCycles,pszRecord->cdChDisCycles,sizeof(pszRecord->cdChDisCycles));
		szChDisCycles[sizeof(pszRecord->cdChDisCycles)] = NULL;
		sscanf(szChDisCycles,"%hd",&BMBStatusRec.fDischargeCurr);

		//get the CPU Temperature
		memcpy(szfCPUTemp,pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szfCPUTemp[sizeof(pszRecord->cdfCPUTemp)] = NULL;
		sscanf(szfCPUTemp,"%f",&BMBStatusRec.fCPUTemp);

		//get the BOARD Temperature
		memcpy(szfBOARDTemp,pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szfBOARDTemp[sizeof(pszRecord->cdfBOARDTemp)] = NULL;
		sscanf(szfBOARDTemp,"%f",&BMBStatusRec.fBOARDTemp);

		//get the Time on battery
		memcpy(szTimeOnBatt,pszRecord->cdTimeOnBatt,sizeof(pszRecord->cdTimeOnBatt));
		szTimeOnBatt[sizeof(pszRecord->cdTimeOnBatt)] = NULL;
		sscanf(szTimeOnBatt,"%f",&BMBStatusRec.uiTimeOnBatt);

		//get the battery temperature
		memcpy(szfBattTemp,pszRecord->cdfBattTemp,sizeof(pszRecord->cdfBattTemp));
		szfBattTemp[sizeof(pszRecord->cdfBattTemp)] = NULL;
		sscanf(szfBattTemp,"%f",&BMBStatusRec.fBattTemp);

		//get the battery status
		memcpy(szBattStatus,pszRecord->chBattStatus,sizeof(pszRecord->chBattStatus));
		szBattStatus[sizeof(pszRecord->chBattStatus)] = NULL;
		sscanf(szBattStatus,"%hx",&BMBStatusRec.bBattStatus);

		//get the reserved
		memcpy(szReserved,pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szReserved[sizeof(pszRecord->chReserved)] = NULL;
		sscanf(szReserved,"%x",&BMBStatusRec.uiReserved);

		if (m_duBMBStatusRecordTime < cTime.TotalSeconds())	
		{	
			//newer than the last received
			bStale = false;

			//update the time of the latest record
			m_duBMBStatusRecordTime = cTime.TotalSeconds()+BMBStatusRec.bJulianSec10ths/10.0;	

			switch (iColumn) {
			case 5:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
				if (!CurrentSetParam(SET_BMBSR_SERIALNUM1,"SET_BMBSR_SERIALNUM1",szSerialNumber,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 1:
				//set date and time
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATES1,cTime.Date());
				cTemp = cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMES1,cTemp);
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_BMBSR_TIME1,"SET_BMBSR_TIME1",cTemp,false);

				//do the charge current
				if (!CurrentSetParam(SET_BMBSR_CHARGECURNT1,"SET_BMBSR_CHARGECURNT1",szfChargeCurrent,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CHARGECNT1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the discharge current
				if (!CurrentSetParam(SET_BMBSR_DISCHGCURNT1,"SET_BMBSR_DISCHGCURNT1",szfDischargeCurr,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_DISCHARGECNT1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the charge / discharge cycles
				if (!CurrentSetParam(SET_BMBSR_CDCYCLES1,"SET_BMBSR_CDCYCLES1",szChDisCycles,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CDCYCLES1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the CPU Temperature
				if (!CurrentSetParam(SET_BMBSR_CPUTEMP1,"SET_BMBSR_CPUTEMP1",szfCPUTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the BOARD Temperature
				if (!CurrentSetParam(SET_BMBSR_BOARDTEMP1,"SET_BMBSR_BOARDTEMP1",szfBOARDTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the time on battery
				if (!CurrentSetParam(SET_BMBSR_TIMEONBAT1,"SET_BMBSR_TIMEONBAT1",szTimeOnBatt,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_TIMEONBAT1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the battery temperature
				if (!CurrentSetParam(SET_BMBSR_BATTTEMP1,"SET_BMBSR_BATTTEMP1",szfBattTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_BATTEMP1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the battery status
				if (!CurrentSetParam(SET_BMBSR_BATTSTATUS1,"SET_BMBSR_BATTSTATUS1",szBattStatus,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_BATSTAT1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the reserved
				if (!CurrentSetParam(SET_BMBSR_RESERVED1,"SET_BMBSR_RESERVED1",szReserved,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_RESERVE1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;

			case 6:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
				if (!CurrentSetParam(SET_BMBSR_SERIALNUM2,"SET_BMBSR_SERIALNUM2",szSerialNumber,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 2:
				//set date and time
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATES2,cTime.Date());
				cTemp = cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMES2,cTemp);
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_BMBSR_TIME2,"SET_BMBSR_TIME2",cTemp,false);

				//do the charge current
				if (!CurrentSetParam(SET_BMBSR_CHARGECURNT2,"SET_BMBSR_CHARGECURNT2",szfChargeCurrent,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CHARGECNT2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the discharge current
				if (!CurrentSetParam(SET_BMBSR_DISCHGCURNT2,"SET_BMBSR_DISCHGCURNT2",szfDischargeCurr,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_DISCHARGECNT2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the charge / discharge cycles
				if (!CurrentSetParam(SET_BMBSR_CDCYCLES2,"SET_BMBSR_CDCYCLES2",szChDisCycles,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CDCYCLES2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the CPU Temperature
				if (!CurrentSetParam(SET_BMBSR_CPUTEMP2,"SET_BMBSR_CPUTEMP2",szfCPUTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the BOARD Temperature
				if (!CurrentSetParam(SET_BMBSR_BOARDTEMP2,"SET_BMBSR_BOARDTEMP2",szfBOARDTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the time on battery
				if (!CurrentSetParam(SET_BMBSR_TIMEONBAT2,"SET_BMBSR_TIMEONBAT2",szTimeOnBatt,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_TIMEONBAT2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the battery temperature
				if (!CurrentSetParam(SET_BMBSR_BATTTEMP2,"SET_BMBSR_BATTTEMP2",szfBattTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_BATTEMP2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the battery status
				if (!CurrentSetParam(SET_BMBSR_BATTSTATUS2,"SET_BMBSR_BATTSTATUS2",szBattStatus,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_BATSTAT2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the reserved
				if (!CurrentSetParam(SET_BMBSR_RESERVED2,"SET_BMBSR_RESERVED2",szReserved,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_RESERVE2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;


			case 7:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
				if (!CurrentSetParam(SET_BMBSR_SERIALNUM3,"SET_BMBSR_SERIALNUM3",szSerialNumber,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 3:
				//set date and time
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATES3,cTime.Date());
				cTemp = cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMES3,cTemp);
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_BMBSR_TIME3,"SET_BMBSR_TIME3",cTemp,false);

				//do the charge current
				if (!CurrentSetParam(SET_BMBSR_CHARGECURNT3,"SET_BMBSR_CHARGECURNT3",szfChargeCurrent,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CHARGECNT3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the discharge current
				if (!CurrentSetParam(SET_BMBSR_DISCHGCURNT3,"SET_BMBSR_DISCHGCURNT3",szfDischargeCurr,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_DISCHARGECNT3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the charge / discharge cycles
				if (!CurrentSetParam(SET_BMBSR_CDCYCLES3,"SET_BMBSR_CDCYCLES3",szChDisCycles,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CDCYCLES3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the CPU Temperature
				if (!CurrentSetParam(SET_BMBSR_CPUTEMP3,"SET_BMBSR_CPUTEMP3",szfCPUTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the BOARD Temperature
				if (!CurrentSetParam(SET_BMBSR_BOARDTEMP3,"SET_BMBSR_BOARDTEMP3",szfBOARDTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the time on battery
				if (!CurrentSetParam(SET_BMBSR_TIMEONBAT3,"SET_BMBSR_TIMEONBAT3",szTimeOnBatt,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_TIMEONBAT3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the battery temperature
				if (!CurrentSetParam(SET_BMBSR_BATTTEMP3,"SET_BMBSR_BATTTEMP3",szfBattTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_BATTEMP3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the battery status
				if (!CurrentSetParam(SET_BMBSR_BATTSTATUS3,"SET_BMBSR_BATTSTATUS3",szBattStatus,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_BATSTAT3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the reserved
				if (!CurrentSetParam(SET_BMBSR_RESERVED3,"SET_BMBSR_RESERVED3",szReserved,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_RESERVE3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;


			case 8:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
				if (!CurrentSetParam(SET_BMBSR_SERIALNUM4,"SET_BMBSR_SERIALNUM4",szSerialNumber,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process rest
			case 4:
				//set date and time
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATES4,cTime.Date());
				cTemp = cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMES4,cTemp);
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_BMBSR_TIME1,"SET_BMBSR_TIME4",cTemp,false);

				//do the charge current
				if (!CurrentSetParam(SET_BMBSR_CHARGECURNT4,"SET_BMBSR_CHARGECURNT4",szfChargeCurrent,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CHARGECNT4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the discharge current
				if (!CurrentSetParam(SET_BMBSR_DISCHGCURNT4,"SET_BMBSR_DISCHGCURNT4",szfDischargeCurr,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_DISCHARGECNT4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the charge / discharge cycles
				if (!CurrentSetParam(SET_BMBSR_CDCYCLES4,"SET_BMBSR_CDCYCLES4",szChDisCycles,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CDCYCLES4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the CPU Temperature
				if (!CurrentSetParam(SET_BMBSR_CPUTEMP4,"SET_BMBSR_CPUTEMP4",szfCPUTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the BOARD Temperature
				if (!CurrentSetParam(SET_BMBSR_BOARDTEMP4,"SET_BMBSR_BOARDTEMP4",szfBOARDTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_CPUBOARDT4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the time on battery
				if (!CurrentSetParam(SET_BMBSR_TIMEONBAT4,"SET_BMBSR_TIMEONBAT4",szTimeOnBatt,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_TIMEONBAT4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the battery temperature
				if (!CurrentSetParam(SET_BMBSR_BATTTEMP4,"SET_BMBSR_BATTTEMP4",szfBattTemp,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_BATTEMP4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the battery status
				if (!CurrentSetParam(SET_BMBSR_BATTSTATUS4,"SET_BMBSR_BATTSTATUS4",szBattStatus,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_BATSTAT4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}

				//do the reserved
				if (!CurrentSetParam(SET_BMBSR_RESERVED4,"SET_BMBSR_RESERVED4",szReserved,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_RESERVE4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;

			}
		}
			
		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(BMBStatusTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(BMBStatusTextRecord));
		cBuff[sizeof(BMBStatusTextRecord) + 6] = NULL;

		if (!bStale)
		{
			switch (iColumn) {
			case 1:
			case 5:
				m_cBMBStatusRecord1 = cBuff+6;
				break;
			case 2:
			case 6:
				m_cBMBStatusRecord2 = cBuff+6;
				break;
			case 3:
			case 7:
				m_cBMBStatusRecord3 = cBuff+6;
				break;
			case 4:
			case 8:
				m_cBMBStatusRecord4 = cBuff+6;
				break;
			}
			if (m_pPropertySheet)
				m_pPropertySheet->OnBMBStatusRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

		WriteToSOHFileX((AcquireRecordX*)&BMBStatusRec);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	} //if checksum good

	PostMessage(IDC_RECEIVE_RESULT,BMBSTATUSRECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnuGInfoRecord(char* szRecord)				
{
	//here is where I'm currently working
	int CheckSum;
	if (LengthOk(szRecord,sizeof(uGInfoTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//will need a few work areas
		CString cTemp;
		char szTemp1[32];

//		//do we have a change
//		bool bChange = false;

		//create a binary record to send to the SOH file
		uGInfoRecord uGInfoRec;
		memset(&uGInfoRec,NULL,sizeof(uGInfoRecord));

		//get a pointer to a GeneralConfigTextRecord
		uGInfoTextRecord* pszRecord = (uGInfoTextRecord*)szRecord;

		//get length and opcode and place in binary record
		uGInfoRec.bLength = sizeof(uGInfoRecord);
		strncpy(szTemp1,(char*)pszRecord->chOpcode,sizeof(pszRecord->chOpcode));
		szTemp1[sizeof(pszRecord->chOpcode)] = NULL;
		sscanf(szTemp1,"%x",&uGInfoRec.bOpcode);

 		//get julian time and put it in the binary record
		strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
		szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&uGInfoRec.uiJulianSeconds);

		//set the date & time
		CJulianTime cTime;
		cTime.Init(uGInfoRec.uiJulianSeconds);

		bool bStale = true;

		if (m_uiuGInfoRecordTime < cTime.TotalSeconds())	
		{
			//newer than the last received
			bStale = false;

			//update the time of the latest record
			m_uiuGInfoRecordTime = cTime.TotalSeconds();	

			//save a copy to send to the display dialog
			//when it initially gets displayed
			m_cUGInfoRecord = szRecord;

			//set date and time
			SetString(UGINFOPAGE,IDC_UGINFO_DATADATEI,cTime.Date());
			SetString(UGINFOPAGE,IDC_UGINFO_DATATIMEI,cTime.Time());
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			CurrentSetParam(SET_UGGCR_TIME,"SET_UGIR_TIME",cTemp,false);
		}

		//set actel version
		strncpy(szTemp1,(char*)pszRecord->chActelVersion,sizeof(pszRecord->chActelVersion));
		szTemp1[sizeof(pszRecord->chActelVersion)]=NULL;
		sscanf(szTemp1,"%h4x",&uGInfoRec.usActelVersion);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_ACTELVERI,szTemp1);
			if (!CurrentSetParam(SET_UGIR_ACTELVER,"SET_UGIR_ACTELVER",szTemp1,true))
			{
				SetColor(UGINFOPAGE,IDC_UGINFO_ACTELVERI,COLOR_YELLOW);
				m_iUGInfo_Tab |= 0x0002;
			}
		}

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		sscanf(szTemp1,"%d.%d.%d",
			&uGInfoRec.b3dFirmwareVersion[0],
			&uGInfoRec.b3dFirmwareVersion[1],
			&uGInfoRec.b3dFirmwareVersion[2]
			);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_FIRMWVI,szTemp1);
			if (!CurrentSetParam(SET_UGIR_FIRMWAREVER,"SET_UGIR_FIRMWAREVER",szTemp1,true))
			{
				SetColor(UGINFOPAGE,IDC_UGINFO_FIRMWVI,COLOR_YELLOW);
				m_iUGInfo_Tab |= 0x0002;
			}
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		sscanf(szTemp1,"%h4x",&uGInfoRec.usFirmwareChkSum);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_FIRMWCHKI,szTemp1);
			if (!CurrentSetParam(SET_UGIR_FIRMWARECKSUM,"SET_UGIR_FIRMWARECKSUM",szTemp1,true))
			{
				SetColor(UGINFOPAGE,IDC_UGINFO_FIRMWCHKI,COLOR_YELLOW);
				m_iUGInfo_Tab |= 0x0002;
			}
		}
		
		//build the serial number 
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		sscanf(szTemp1,"%h2x%h2x%h2x%h2x%h2x%h2x",
			&uGInfoRec.bSerialNumber[0],
			&uGInfoRec.bSerialNumber[1],
			&uGInfoRec.bSerialNumber[2],
			&uGInfoRec.bSerialNumber[3],
			&uGInfoRec.bSerialNumber[4],
			&uGInfoRec.bSerialNumber[5]);
		if (!bStale)
		{
			SetString(UGINFOPAGE,IDC_UGINFO_UGRANDSNI,szTemp1);
			if (!CurrentSetParam(SET_UGIR_SERIALNUM,"SET_UGIR_SERIALNUM",szTemp1,true))
			{
				SetColor(UGINFOPAGE,IDC_UGINFO_UGRANDSNI,COLOR_YELLOW);
				m_iUGInfo_Tab |= 0x0002;
			}
		}

		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(uGInfoTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(uGInfoTextRecord));
		cBuff[sizeof(uGInfoTextRecord) + 6] = NULL;

		if (!bStale)
		{
			m_cUGInfoRecord = cBuff+6;
			if (m_pPropertySheet)
				m_pPropertySheet->OnuGInfoRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

		WriteToSOHFileX((AcquireRecordX*)&uGInfoRec);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	} //if checksum good

	PostMessage(IDC_RECEIVE_RESULT,UGINFORECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnESBInfoRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(ESBInfoTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//some temporary buffers
		char szTemp1[32];
		char szTempFV[32];
		char szTempFC[32];
		char szTempSN[32];
		CString cTemp;

		//create a binary record
		ESBInfoRecord sESBInfoRecord;
		memset(&sESBInfoRecord,NULL, sizeof(ESBInfoRecord));

		//get a pointer to the version of the record
		ESBInfoTextRecord* pESBInfoTextRec = (ESBInfoTextRecord*)szRecord;

		//set the length in the binary version
		sESBInfoRecord.bLength = sizeof(ESBInfoRecord);

		//set opcode into binary version
		strncpy(szTemp1,(char*)&pESBInfoTextRec->chOpcode,sizeof(pESBInfoTextRec->chOpcode));
		szTemp1[sizeof(pESBInfoTextRec->chOpcode)]=NULL;
		sscanf(szTemp1,"%02h",&sESBInfoRecord.bOpcode);

		//set time in binary version
		strncpy(szTemp1,(char*)pESBInfoTextRec->cdJulianSeconds,sizeof(pESBInfoTextRec->cdJulianSeconds));
		szTemp1[sizeof(pESBInfoTextRec->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&sESBInfoRecord.uiJulianSeconds);

		CJulianTime cTime;
		cTime.Init(szTemp1);

		bool bStale = true;

		//get the serial number to find which column it goes into
		strncpy(szTempSN,(char*)pESBInfoTextRec->chSerialNumber,sizeof(pESBInfoTextRec->chSerialNumber));
		szTempSN[sizeof(pESBInfoTextRec->chSerialNumber)]=NULL;
		sscanf(szTempSN,"%2x%2x%2x%2x%2x%2x",
			&sESBInfoRecord.bSerialNumber[0],
			&sESBInfoRecord.bSerialNumber[1],
			&sESBInfoRecord.bSerialNumber[2],
			&sESBInfoRecord.bSerialNumber[3],
			&sESBInfoRecord.bSerialNumber[4],
			&sESBInfoRecord.bSerialNumber[5]);

		int iColumn = 0;
		if      (strncmp((char*)&pESBInfoTextRec->chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1),sizeof(pESBInfoTextRec->chSerialNumber)) == 0) iColumn = 1;
		else if (strncmp((char*)&pESBInfoTextRec->chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2),sizeof(pESBInfoTextRec->chSerialNumber)) == 0) iColumn = 2;
		else if (strncmp((char*)&pESBInfoTextRec->chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3),sizeof(pESBInfoTextRec->chSerialNumber)) == 0) iColumn = 3;
		else if (strncmp((char*)&pESBInfoTextRec->chSerialNumber,GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4),sizeof(pESBInfoTextRec->chSerialNumber)) == 0) iColumn = 4;
		else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1))==0) iColumn = 5;
		else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2))==0) iColumn = 6;
		else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3))==0) iColumn = 7;
		else if (strlen(GetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4))==0) iColumn = 8;

		//get the firmware version
		strncpy(szTempFV,(char*)pESBInfoTextRec->c3dFirmwareVersion,sizeof(pESBInfoTextRec->c3dFirmwareVersion));
		szTempFV[sizeof(pESBInfoTextRec->c3dFirmwareVersion)]=NULL;
		sscanf(szTempFV,"%hd.%hd.%hd",
			&sESBInfoRecord.b3dFirmwareVersion[0],
			&sESBInfoRecord.b3dFirmwareVersion[1],
			&sESBInfoRecord.b3dFirmwareVersion[2]);
		
		//get the firmware checksum
		strncpy(szTempFC,(char*)pESBInfoTextRec->cdFirmwareChkSum,sizeof(pESBInfoTextRec->cdFirmwareChkSum));
		szTempFC[sizeof(pESBInfoTextRec->cdFirmwareChkSum)]=NULL;
		sscanf(szTempFC,"%hx",&sESBInfoRecord.usFirmwareChkSum);

		if (m_uiESBInfoRecordTime < cTime.TotalSeconds())
		{
			//newer than the last one received
			bStale = false;

			//save the time of this new record
			m_uiESBInfoRecordTime = cTime.TotalSeconds();

			//set date and time
			switch (iColumn) {
			case 5:
				SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL1,pESBInfoTextRec->chSerialNumber,sizeof(pESBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_ESBIR_SERIALNUM1,"SET_ESBIR_SERIALNUM1",szTempSN,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL1,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 1:
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEI1,cTime.Date());
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEI1,cTime.Time());
				SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV1,pESBInfoTextRec->c3dFirmwareVersion,sizeof(pESBInfoTextRec->c3dFirmwareVersion));
				SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC1,pESBInfoTextRec->cdFirmwareChkSum,sizeof(pESBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_ESBIR_TIME1,"SET_ESBIR_TIME1",cTemp,false);
				if (!CurrentSetParam(SET_ESBIR_FIRMWAREVER1,"SET_ESBIR_FIRMWAREVER1",szTempFV,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV1,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_ESBIR_FIRMWARECKSUM1,"SET_ESBIR_FIRMWARECKSUM1",szTempFC,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC1,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				break;
			case 6:
				SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL2,pESBInfoTextRec->chSerialNumber,sizeof(pESBInfoTextRec->chSerialNumber));
				if (CurrentSetParam(SET_ESBIR_SERIALNUM2,"SET_ESBIR_SERIALNUM2",szTempSN,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL2,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 2:
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEI2,cTime.Date());
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEI2,cTime.Time());
				SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV2,pESBInfoTextRec->c3dFirmwareVersion,sizeof(pESBInfoTextRec->c3dFirmwareVersion));
				SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC2,pESBInfoTextRec->cdFirmwareChkSum,sizeof(pESBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_ESBIR_TIME2,"SET_ESBIR_TIME2",cTemp,false);
				if (!CurrentSetParam(SET_ESBIR_FIRMWAREVER2,"SET_ESBIR_FIRMWAREVER2",szTempFV,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV2,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_ESBIR_FIRMWARECKSUM2,"SET_ESBIR_FIRMWARECKSUM2",szTempFC,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC2,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				break;
			case 7:
				SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL3,pESBInfoTextRec->chSerialNumber,sizeof(pESBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_ESBIR_SERIALNUM3,"SET_ESBIR_SERIALNUM3",szTempSN,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL3,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 3:
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEI3,cTime.Date());
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEI3,cTime.Time());
				SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV3,pESBInfoTextRec->c3dFirmwareVersion,sizeof(pESBInfoTextRec->c3dFirmwareVersion));
				SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC3,pESBInfoTextRec->cdFirmwareChkSum,sizeof(pESBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_ESBIR_TIME3,"SET_ESBIR_TIME3",cTemp,false);
				if (!CurrentSetParam(SET_ESBIR_FIRMWAREVER3,"SET_ESBIR_FIRMWAREVER3",szTempFV,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV3,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_ESBIR_FIRMWARECKSUM3,"SET_ESBIR_FIRMWARECKSUM3",szTempFC,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC3,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				break;
			case 8:
				SetString(ESBINFOPAGE,IDC_ESBINFO_SERIAL4,pESBInfoTextRec->chSerialNumber,sizeof(pESBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_ESBIR_SERIALNUM4,"SET_ESBIR_SERIALNUM4",szTempSN,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_SERIAL4,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 4:
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATADATEI4,cTime.Date());
				SetString(ESBINFOPAGE,IDC_ESBINFO_DATATIMEI4,cTime.Time());
				SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV4,pESBInfoTextRec->c3dFirmwareVersion,sizeof(pESBInfoTextRec->c3dFirmwareVersion));
				SetString(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC4,pESBInfoTextRec->cdFirmwareChkSum,sizeof(pESBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_ESBIR_TIME4,"SET_ESBIR_TIME4",cTemp,false);
				if (!CurrentSetParam(SET_ESBIR_FIRMWAREVER4,"SET_ESBIR_FIRMWAREVER4",szTempFV,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREV4,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_ESBIR_FIRMWARECKSUM4,"SET_ESBIR_FIRMWARECKSUM4",szTempFC,false))
				{
					SetColor(ESBINFOPAGE,IDC_ESBINFO_FIRMWAREC4,COLOR_YELLOW);
					m_iESBInfo_Tab |= 0x0002;
				}
				break;
			}
		}

		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(ESBInfoTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(ESBInfoTextRecord));
		cBuff[sizeof(ESBInfoTextRecord) + 6] = NULL;

		if (!bStale)
		{
			//save the text version to send to the display
			switch (iColumn) {
			case 1:
			case 5:
				m_cESBInfoRecord1 = cBuff+6;
				break;
			case 2:
			case 6:
				m_cESBInfoRecord2 = cBuff+6;
				break;
			case 3:
			case 7:
				m_cESBInfoRecord3 = cBuff+6;
				break;
			case 4:
			case 8:
				m_cESBInfoRecord4 = cBuff+6;
				break;
			}

			//send to the display now if we can
			if (m_pPropertySheet)
				m_pPropertySheet->OnESBInfoRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);
	
		WriteToSOHFileX((AcquireRecordX*)&sESBInfoRecord);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);
	}

	PostMessage(IDC_RECEIVE_RESULT,ESBINFORECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnPSBInfoRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(PSBInfoTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//some temporary buffers
		char szTemp1[32];
		char szTempFV[32];
		char szTempFC[32];
		char szTempSN[32];
		CString cTemp;

		//create a binary record
		PSBInfoRecord sPSBInfoRecord;
		memset(&sPSBInfoRecord,NULL, sizeof(PSBInfoRecord));

		//get a pointer to the version of the record
		PSBInfoTextRecord* pPSBInfoTextRec = (PSBInfoTextRecord*)szRecord;

		//set the length in the binary version
		sPSBInfoRecord.bLength = sizeof(PSBInfoRecord);

		//set opcode into binary version
		strncpy(szTemp1,(char*)&pPSBInfoTextRec->chOpcode,sizeof(pPSBInfoTextRec->chOpcode));
		szTemp1[sizeof(pPSBInfoTextRec->chOpcode)]=NULL;
		sscanf(szTemp1,"%02h",&sPSBInfoRecord.bOpcode);

		//set time in binary version
		strncpy(szTemp1,(char*)pPSBInfoTextRec->cdJulianSeconds,sizeof(pPSBInfoTextRec->cdJulianSeconds));
		szTemp1[sizeof(pPSBInfoTextRec->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&sPSBInfoRecord.uiJulianSeconds);

		CJulianTime cTime;
		cTime.Init(szTemp1);

		bool bStale = true;

		//get the serial number to find which column it goes into
		strncpy(szTemp1,(char*)pPSBInfoTextRec->chSerialNumber,sizeof(pPSBInfoTextRec->chSerialNumber));
		szTemp1[sizeof(pPSBInfoTextRec->chSerialNumber)]=NULL;
		sscanf(szTemp1,"%2x%2x%2x%2x%2x%2x",
			&sPSBInfoRecord.bSerialNumber[0],
			&sPSBInfoRecord.bSerialNumber[1],
			&sPSBInfoRecord.bSerialNumber[2],
			&sPSBInfoRecord.bSerialNumber[3],
			&sPSBInfoRecord.bSerialNumber[4],
			&sPSBInfoRecord.bSerialNumber[5]);

		int iColumn = 0;
		if      (strncmp((char*)&pPSBInfoTextRec->chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1),sizeof(pPSBInfoTextRec->chSerialNumber)) == 0) iColumn = 1;
		else if (strncmp((char*)&pPSBInfoTextRec->chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2),sizeof(pPSBInfoTextRec->chSerialNumber)) == 0) iColumn = 2;
		else if (strncmp((char*)&pPSBInfoTextRec->chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3),sizeof(pPSBInfoTextRec->chSerialNumber)) == 0) iColumn = 3;
		else if (strncmp((char*)&pPSBInfoTextRec->chSerialNumber,GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4),sizeof(pPSBInfoTextRec->chSerialNumber)) == 0) iColumn = 4;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1))==0) iColumn = 5;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2))==0) iColumn = 6;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3))==0) iColumn = 7;
		else if (strlen(GetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4))==0) iColumn = 8;

		//get the firmware version
		strncpy(szTempFV,(char*)pPSBInfoTextRec->c3dFirmwareVersion,sizeof(pPSBInfoTextRec->c3dFirmwareVersion));
		szTempFV[sizeof(pPSBInfoTextRec->c3dFirmwareVersion)]=NULL;
		sscanf(szTempFV,"%hd.%hd.%hd",
			&sPSBInfoRecord.b3dFirmwareVersion[0],
			&sPSBInfoRecord.b3dFirmwareVersion[1],
			&sPSBInfoRecord.b3dFirmwareVersion[2]);
		
		//get the firmware checksum
		strncpy(szTempFC,(char*)pPSBInfoTextRec->cdFirmwareChkSum,sizeof(pPSBInfoTextRec->cdFirmwareChkSum));
		szTempFC[sizeof(pPSBInfoTextRec->cdFirmwareChkSum)]=NULL;
		sscanf(szTempFC,"%hx",&sPSBInfoRecord.usFirmwareChkSum);

		if (m_uiPSBInfoRecordTime < cTime.TotalSeconds())
		{
			//newer than the last one received
			bStale = false;

			//save the time of this new record
			m_uiPSBInfoRecordTime = cTime.TotalSeconds();

			//set date and time
			switch (iColumn) {
			case 5:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,pPSBInfoTextRec->chSerialNumber,sizeof(pPSBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM1,"SET_PSBIR_SERIALNUM1",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 1:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEI1,cTime.Date());
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEI1,cTime.Time());
				SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV1,pPSBInfoTextRec->c3dFirmwareVersion,sizeof(pPSBInfoTextRec->c3dFirmwareVersion));
				SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC1,pPSBInfoTextRec->cdFirmwareChkSum,sizeof(pPSBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_PSBIR_TIME1,"SET_PSBIR_TIME1",cTemp,false);
				if (!CurrentSetParam(SET_PSBIR_FIRMWAREVER1,"SET_PSBIR_FIRMWAREVER1",szTempFV,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_PSBIR_FIRMWARECKSUM1,"SET_PSBIR_FIRMWARECKSUM1",szTempFC,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC1,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;
			case 6:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,pPSBInfoTextRec->chSerialNumber,sizeof(pPSBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM2,"SET_PSBIR_SERIALNUM2",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 2:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEI2,cTime.Date());
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEI2,cTime.Time());
				SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV2,pPSBInfoTextRec->c3dFirmwareVersion,sizeof(pPSBInfoTextRec->c3dFirmwareVersion));
				SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC2,pPSBInfoTextRec->cdFirmwareChkSum,sizeof(pPSBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_PSBIR_TIME2,"SET_PSBIR_TIME2",cTemp,false);
				if (!CurrentSetParam(SET_PSBIR_FIRMWAREVER2,"SET_PSBIR_FIRMWAREVER2",szTempFV,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_PSBIR_FIRMWARECKSUM2,"SET_PSBIR_FIRMWARECKSUM2",szTempFC,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC2,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;
			case 7:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,pPSBInfoTextRec->chSerialNumber,sizeof(pPSBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM3,"SET_PSBIR_SERIALNUM3",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 3:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEI3,cTime.Date());
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEI3,cTime.Time());
				SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV3,pPSBInfoTextRec->c3dFirmwareVersion,sizeof(pPSBInfoTextRec->c3dFirmwareVersion));
				SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC3,pPSBInfoTextRec->cdFirmwareChkSum,sizeof(pPSBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_PSBIR_TIME3,"SET_PSBIR_TIME3",cTemp,false);
				if (!CurrentSetParam(SET_PSBIR_FIRMWAREVER3,"SET_PSBIR_FIRMWAREVER3",szTempFV,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_PSBIR_FIRMWARECKSUM3,"SET_PSBIR_FIRMWARECKSUM3",szTempFC,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC3,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;
			case 8:
				SetString(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,pPSBInfoTextRec->chSerialNumber,sizeof(pPSBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_PSBIR_SERIALNUM4,"SET_PSBIR_SERIALNUM4",szTempSN,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_SERIAL4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 4:
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATADATEI4,cTime.Date());
				SetString(PSBINFOPAGE,IDC_PSBINFO_DATATIMEI4,cTime.Time());
				SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV4,pPSBInfoTextRec->c3dFirmwareVersion,sizeof(pPSBInfoTextRec->c3dFirmwareVersion));
				SetString(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC4,pPSBInfoTextRec->cdFirmwareChkSum,sizeof(pPSBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_PSBIR_TIME4,"SET_PSBIR_TIME4",cTemp,false);
				if (!CurrentSetParam(SET_PSBIR_FIRMWAREVER4,"SET_PSBIR_FIRMWAREVER4",szTempFV,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREV4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_PSBIR_FIRMWARECKSUM4,"SET_PSBIR_FIRMWARECKSUM4",szTempFC,false))
				{
					SetColor(PSBINFOPAGE,IDC_PSBINFO_FIRMWAREC4,COLOR_YELLOW);
					m_iPSBInfo_Tab |= 0x0002;
				}
				break;
			}
		}

		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(PSBInfoTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(PSBInfoTextRecord));
		cBuff[sizeof(PSBInfoTextRecord) + 6] = NULL;

		if (!bStale)
		{
			//save the text version to send to the display
			switch (iColumn) {
			case 1:
			case 5:
				m_cPSBInfoRecord1 = cBuff+6;
				break;
			case 2:
			case 6:
				m_cPSBInfoRecord2 = cBuff+6;
				break;
			case 3:
			case 7:
				m_cPSBInfoRecord3 = cBuff+6;
				break;
			case 4:
			case 8:
				m_cPSBInfoRecord4 = cBuff+6;
				break;
			}
			//send to the display now if we can
			if (m_pPropertySheet)
				m_pPropertySheet->OnPSBInfoRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);
	
		WriteToSOHFileX((AcquireRecordX*)&sPSBInfoRecord);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);
	}

	PostMessage(IDC_RECEIVE_RESULT,PSBINFORECORD_OPCODE,CheckSum);
}

void 
CInstrument::OnBMBInfoRecord(char* szRecord)			
{
	int CheckSum;
	if (LengthOk(szRecord,sizeof(BMBInfoTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		//some temporary buffers
		char szTemp1[32];
		char szTempFV[32];
		char szTempFC[32];
		char szTempSN[32];
		CString cTemp;

		//create a binary record
		BMBInfoRecord sBMBInfoRecord;
		memset(&sBMBInfoRecord,NULL, sizeof(BMBInfoRecord));

		//get a pointer to the version of the record
		BMBInfoTextRecord* pBMBInfoTextRec = (BMBInfoTextRecord*)szRecord;

		//set the length in the binary version
		sBMBInfoRecord.bLength = sizeof(BMBInfoRecord);

		//set opcode into binary version
		strncpy(szTemp1,(char*)&pBMBInfoTextRec->chOpcode,sizeof(pBMBInfoTextRec->chOpcode));
		szTemp1[sizeof(pBMBInfoTextRec->chOpcode)]=NULL;
		sscanf(szTemp1,"%02h",&sBMBInfoRecord.bOpcode);

		//set time in binary version
		strncpy(szTemp1,(char*)pBMBInfoTextRec->cdJulianSeconds,sizeof(pBMBInfoTextRec->cdJulianSeconds));
		szTemp1[sizeof(pBMBInfoTextRec->cdJulianSeconds)]=NULL;
		sscanf(szTemp1,"%u",&sBMBInfoRecord.uiJulianSeconds);

		CJulianTime cTime;
		cTime.Init(szTemp1);

		bool bStale = true;

		//get the serial number to find which column it goes into
		strncpy(szTemp1,(char*)pBMBInfoTextRec->chSerialNumber,sizeof(pBMBInfoTextRec->chSerialNumber));
		szTemp1[sizeof(pBMBInfoTextRec->chSerialNumber)]=NULL;
		sscanf(szTemp1,"%2x%2x%2x%2x%2x%2x",
			&sBMBInfoRecord.bSerialNumber[0],
			&sBMBInfoRecord.bSerialNumber[1],
			&sBMBInfoRecord.bSerialNumber[2],
			&sBMBInfoRecord.bSerialNumber[3],
			&sBMBInfoRecord.bSerialNumber[4],
			&sBMBInfoRecord.bSerialNumber[5]);

		int iColumn = 0;
		if      (strncmp((char*)&pBMBInfoTextRec->chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1),sizeof(pBMBInfoTextRec->chSerialNumber)) == 0) iColumn = 1;
		else if (strncmp((char*)&pBMBInfoTextRec->chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2),sizeof(pBMBInfoTextRec->chSerialNumber)) == 0) iColumn = 2;
		else if (strncmp((char*)&pBMBInfoTextRec->chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3),sizeof(pBMBInfoTextRec->chSerialNumber)) == 0) iColumn = 3;
		else if (strncmp((char*)&pBMBInfoTextRec->chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4),sizeof(pBMBInfoTextRec->chSerialNumber)) == 0) iColumn = 4;
		else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1))==0) iColumn = 5;
		else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2))==0) iColumn = 6;
		else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3))==0) iColumn = 7;
		else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4))==0) iColumn = 8;

		//get the firmware version
		strncpy(szTempFV,(char*)pBMBInfoTextRec->c3dFirmwareVersion,sizeof(pBMBInfoTextRec->c3dFirmwareVersion));
		szTempFV[sizeof(pBMBInfoTextRec->c3dFirmwareVersion)]=NULL;
		sscanf(szTempFV,"%hd.%hd.%hd",
			&sBMBInfoRecord.b3dFirmwareVersion[0],
			&sBMBInfoRecord.b3dFirmwareVersion[1],
			&sBMBInfoRecord.b3dFirmwareVersion[2]);
		
		//get the firmware checksum
		strncpy(szTempFC,(char*)pBMBInfoTextRec->cdFirmwareChkSum,sizeof(pBMBInfoTextRec->cdFirmwareChkSum));
		szTempFC[sizeof(pBMBInfoTextRec->cdFirmwareChkSum)]=NULL;
		sscanf(szTempFC,"%hx",&sBMBInfoRecord.usFirmwareChkSum);

		if (m_uiBMBInfoRecordTime < cTime.TotalSeconds())
		{
			//newer than the last one received
			bStale = false;

			//save the time of this new record
			m_uiBMBInfoRecordTime = cTime.TotalSeconds();

			//set date and time
			switch (iColumn) {
			case 5:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,pBMBInfoTextRec->chSerialNumber,sizeof(pBMBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_BMBIR_SERIALNUM1,"SET_BMBIR_SERIALNUM1",szTempSN,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 1:
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI1,cTime.Date());
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI1,cTime.Time());
				SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV1,pBMBInfoTextRec->c3dFirmwareVersion,sizeof(pBMBInfoTextRec->c3dFirmwareVersion));
				SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC1,pBMBInfoTextRec->cdFirmwareChkSum,sizeof(pBMBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_BMBIR_TIME1,"SET_BMBIR_TIME1",cTemp,false);
				if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER1,"SET_BMBIR_FIRMWAREVER1",szTempFV,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM1,"SET_BMBIR_FIRMWARECKSUM1",szTempFC,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC1,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;
			case 6:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,pBMBInfoTextRec->chSerialNumber,sizeof(pBMBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_BMBIR_SERIALNUM2,"SET_BMBIR_SERIALNUM2",szTempSN,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 2:
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI2,cTime.Date());
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI2,cTime.Time());
				SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV2,pBMBInfoTextRec->c3dFirmwareVersion,sizeof(pBMBInfoTextRec->c3dFirmwareVersion));
				SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC2,pBMBInfoTextRec->cdFirmwareChkSum,sizeof(pBMBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_BMBIR_TIME2,"SET_BMBIR_TIME2",cTemp,false);
				if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER2,"SET_BMBIR_FIRMWAREVER2",szTempFV,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM2,"SET_BMBIR_FIRMWARECKSUM2",szTempFC,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC2,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;
			case 7:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,pBMBInfoTextRec->chSerialNumber,sizeof(pBMBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_BMBIR_SERIALNUM3,"SET_BMBIR_SERIALNUM3",szTempSN,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 3:
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI3,cTime.Date());
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI3,cTime.Time());
				SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV3,pBMBInfoTextRec->c3dFirmwareVersion,sizeof(pBMBInfoTextRec->c3dFirmwareVersion));
				SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC3,pBMBInfoTextRec->cdFirmwareChkSum,sizeof(pBMBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_BMBIR_TIME3,"SET_BMBIR_TIME3",cTemp,false);
				if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER3,"SET_BMBIR_FIRMWAREVER3",szTempFV,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM3,"SET_BMBIR_FIRMWARECKSUM3",szTempFC,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC3,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;
			case 8:
				SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,pBMBInfoTextRec->chSerialNumber,sizeof(pBMBInfoTextRec->chSerialNumber));
				if (!CurrentSetParam(SET_BMBIR_SERIALNUM4,"SET_BMBIR_SERIALNUM4",szTempSN,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				//fall through to process
			case 4:
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI4,cTime.Date());
				SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI4,cTime.Time());
				SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV4,pBMBInfoTextRec->c3dFirmwareVersion,sizeof(pBMBInfoTextRec->c3dFirmwareVersion));
				SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC4,pBMBInfoTextRec->cdFirmwareChkSum,sizeof(pBMBInfoTextRec->cdFirmwareChkSum));
				cTemp = cTime.Date();
				cTemp += " ";
				cTemp += cTime.Time();
				cTemp += ".";
				cTemp += szTemp1;
				CurrentSetParam(SET_BMBIR_TIME4,"SET_BMBIR_TIME4",cTemp,false);
				if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER4,"SET_BMBIR_FIRMWAREVER4",szTempFV,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM4,"SET_BMBIR_FIRMWARECKSUM4",szTempFC,false))
				{
					SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC4,COLOR_YELLOW);
					m_iBMBInfo_Tab |= 0x0002;
				}
				break;
			}
		}

		//send it off to be displayed if possible
		//and save it for later
		char cBuff[sizeof(BMBInfoTextRecord)+7];
		strcpy(cBuff,"00000 ");
		strncat(cBuff,szRecord,sizeof(BMBInfoTextRecord));
		cBuff[sizeof(BMBInfoTextRecord) + 6] = NULL;

		if (!bStale)
		{
			//save the text version to send to the display
			switch (iColumn) {
			case 1:
			case 5:
				m_cBMBInfoRecord1 = cBuff+6;
				break;
			case 2:
			case 6:
				m_cBMBInfoRecord2 = cBuff+6;
				break;
			case 3:
			case 7:
				m_cBMBInfoRecord3 = cBuff+6;
				break;
			case 4:
			case 8:
				m_cBMBInfoRecord4 = cBuff+6;
				break;
			}

			//send to the display now if we can
			if (m_pPropertySheet)
				m_pPropertySheet->OnBMBInfoRecord(cBuff+6);
		}

		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);
	
		WriteToSOHFileX((AcquireRecordX*)&sBMBInfoRecord);

		//send it to dump file if required
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);
	}

	PostMessage(IDC_RECEIVE_RESULT,BMBINFORECORD_OPCODE,CheckSum);
}

//remove the high order byte(s) of the exponent
//usually make the exponent 2 digits
char* CInstrument::TrunkExp(char* szString, int iDigits)
{
	if ((iDigits == 1) || (iDigits == 2))
	{
		char *szAt;
		//find the exponent flag
		if ((szAt=strpbrk(szString,"gGfFeE"))!=0)
			strcpy(&(szString[strlen(szString)-3]),&(szString[strlen(szString)-iDigits]));
	}
	return szString;
}

//main board acquire record -- BINARY
int CInstrument::OnBinaryRADAcquireRec(BYTE *pBinaryAt)
{
	int AcquireRecordTarget = m_dAcquireRecordCountX;

	//change this to the real length
	char szRecord[256];
	char szTemp[32];
	char szTemp2[32];

	//get the address of a binary record
	RADAcquireRecord* psAcquireRecord1 = (RADAcquireRecord*)pBinaryAt;

	//big endian to little endian the time
	Reverse((BYTE*)&(psAcquireRecord1->uiJulianSeconds),sizeof(psAcquireRecord1->uiJulianSeconds));

	//big endian to little endian all the multibyte values 
	Reverse((BYTE*)&(psAcquireRecord1->fCh0CountRate),sizeof(psAcquireRecord1->fCh0CountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh0ShortCountRate),sizeof(psAcquireRecord1->fCh0ShortCountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh0LongCountRate),sizeof(psAcquireRecord1->fCh0LongCountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh1CountRate),sizeof(psAcquireRecord1->fCh1CountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh1ShortCountRate),sizeof(psAcquireRecord1->fCh1ShortCountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh1LongCountRate),sizeof(psAcquireRecord1->fCh1LongCountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh2CountRate),sizeof(psAcquireRecord1->fCh2CountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh2ShortCountRate),sizeof(psAcquireRecord1->fCh2ShortCountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh2LongCountRate),sizeof(psAcquireRecord1->fCh2LongCountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh3CountRate),sizeof(psAcquireRecord1->fCh3CountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh3ShortCountRate),sizeof(psAcquireRecord1->fCh3ShortCountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCh3LongCountRate),sizeof(psAcquireRecord1->fCh3LongCountRate));
	Reverse((BYTE*)&(psAcquireRecord1->fCPUTemp),sizeof(psAcquireRecord1->fCPUTemp));
	Reverse((BYTE*)&(psAcquireRecord1->fBOARDTemp),sizeof(psAcquireRecord1->fBOARDTemp));
	Reverse((BYTE*)&(psAcquireRecord1->uiCountTime10ths),sizeof(psAcquireRecord1->uiCountTime10ths));

	//save adjusted record for write to MGD file at end of dumpsegment
	//save status in binary to acquire record
	memcpy(&(AcquireRecX[AcquireRecordTarget]),psAcquireRecord1,sizeof(RADAcquireRecord));

	CJulianTime cTime;
	cTime.Init(psAcquireRecord1->uiJulianSeconds);

	//move the date displays down one
	SetString(DATAPAGE,IDC_DATA_DATADATE4,
		GetString(DATAPAGE,IDC_DATA_DATADATE3));
	SetString(DATAPAGE,IDC_DATA_DATADATE3,
		GetString(DATAPAGE,IDC_DATA_DATADATE2));
	SetString(DATAPAGE,IDC_DATA_DATADATE2,
		GetString(DATAPAGE,IDC_DATA_DATADATE1));
	SetString(DATAPAGE,IDC_DATA_DATADATE1,cTime.Date());

	//move the time displays down one
	sprintf(szTemp,"%s.%d",cTime.Time(),psAcquireRecord1->ucJulianSec10ths);
	SetString(DATAPAGE,IDC_DATA_DATATIME4,
		GetString(DATAPAGE,IDC_DATA_DATATIME3));
	SetString(DATAPAGE,IDC_DATA_DATATIME3,
		GetString(DATAPAGE,IDC_DATA_DATATIME2));
	SetString(DATAPAGE,IDC_DATA_DATATIME2,
		GetString(DATAPAGE,IDC_DATA_DATATIME1));
	SetString(DATAPAGE,IDC_DATA_DATADATE1,szTemp);

	//build the text version of the record
	sprintf(szRecord,"%02x%10u%c%02x%02x%02x%02x%01x",
		psAcquireRecord1->bOpcode,
		psAcquireRecord1->uiJulianSeconds,
		psAcquireRecord1->ucJulianSec10ths + '0',
		psAcquireRecord1->bStatusByte0,
		psAcquireRecord1->bStatusByte1,
		psAcquireRecord1->bStatusByte2,
		psAcquireRecord1->bStatusByte3,
		psAcquireRecord1->bPPInputState);
	//move the rest of the displays down one
	//and build the text version of the record

	//do the status text
	sprintf(szTemp,"%02x:%02x:%02x:%02x",
		psAcquireRecord1->bStatusByte0,
		psAcquireRecord1->bStatusByte1,
		psAcquireRecord1->bStatusByte2,
		psAcquireRecord1->bStatusByte3);

	SetString(DATAPAGE,IDC_DATA_STATUS01234,
		GetString(DATAPAGE,IDC_DATA_STATUS01233));
	SetString(DATAPAGE,IDC_DATA_STATUS01233,
		GetString(DATAPAGE,IDC_DATA_STATUS01232));
	SetString(DATAPAGE,IDC_DATA_STATUS01232,
		GetString(DATAPAGE,IDC_DATA_STATUS01231));
	SetString(DATAPAGE,IDC_DATA_STATUS01231,szTemp);

	//do the pp input state
	sprintf(szTemp,"%01x",
		psAcquireRecord1->bPPInputState);

	SetString(DATAPAGE,IDC_DATA_PPINPUT4,
		GetString(DATAPAGE,IDC_DATA_PPINPUT3));
	SetString(DATAPAGE,IDC_DATA_PPINPUT3,
		GetString(DATAPAGE,IDC_DATA_PPINPUT2));
	SetString(DATAPAGE,IDC_DATA_PPINPUT2,
		GetString(DATAPAGE,IDC_DATA_PPINPUT1));
	SetString(DATAPAGE,IDC_DATA_PPINPUT1,szTemp);

	//do the ch0 Status
	sprintf(szTemp,"%02x",
		psAcquireRecord1->bCh0Status);

	SetString(DATAPAGE,IDC_DATA_CH0STATUS4,
		GetString(DATAPAGE,IDC_DATA_CH0STATUS3));
	SetString(DATAPAGE,IDC_DATA_CH0STATUS3,
		GetString(DATAPAGE,IDC_DATA_CH0STATUS2));
	SetString(DATAPAGE,IDC_DATA_CH0STATUS2,
		GetString(DATAPAGE,IDC_DATA_CH0STATUS1));
	SetString(DATAPAGE,IDC_DATA_CH0STATUS1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch0 Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh0CountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH0CR4,
		GetString(DATAPAGE,IDC_DATA_CH0CR3));
	SetString(DATAPAGE,IDC_DATA_CH0CR3,
		GetString(DATAPAGE,IDC_DATA_CH0CR2));
	SetString(DATAPAGE,IDC_DATA_CH0CR2,
		GetString(DATAPAGE,IDC_DATA_CH0CR1));
	SetString(DATAPAGE,IDC_DATA_CH0CR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch0 Short Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh0ShortCountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH0SCR4,
		GetString(DATAPAGE,IDC_DATA_CH0SCR3));
	SetString(DATAPAGE,IDC_DATA_CH0SCR3,
		GetString(DATAPAGE,IDC_DATA_CH0SCR2));
	SetString(DATAPAGE,IDC_DATA_CH0SCR2,
		GetString(DATAPAGE,IDC_DATA_CH0SCR1));
	SetString(DATAPAGE,IDC_DATA_CH0SCR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch0 Long Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh0LongCountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH0LCR4,
		GetString(DATAPAGE,IDC_DATA_CH0LCR3));
	SetString(DATAPAGE,IDC_DATA_CH0LCR3,
		GetString(DATAPAGE,IDC_DATA_CH0LCR2));
	SetString(DATAPAGE,IDC_DATA_CH0LCR2,
		GetString(DATAPAGE,IDC_DATA_CH0LCR1));
	SetString(DATAPAGE,IDC_DATA_CH0LCR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch1 Status
	sprintf(szTemp,"%02x",
		psAcquireRecord1->bCh1Status);

	SetString(DATAPAGE,IDC_DATA_CH1STATUS4,
		GetString(DATAPAGE,IDC_DATA_CH1STATUS3));
	SetString(DATAPAGE,IDC_DATA_CH1STATUS3,
		GetString(DATAPAGE,IDC_DATA_CH1STATUS2));
	SetString(DATAPAGE,IDC_DATA_CH1STATUS2,
		GetString(DATAPAGE,IDC_DATA_CH1STATUS1));
	SetString(DATAPAGE,IDC_DATA_CH1STATUS1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch1 Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh1CountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH1CR4,
		GetString(DATAPAGE,IDC_DATA_CH1CR3));
	SetString(DATAPAGE,IDC_DATA_CH1CR3,
		GetString(DATAPAGE,IDC_DATA_CH1CR2));
	SetString(DATAPAGE,IDC_DATA_CH1CR2,
		GetString(DATAPAGE,IDC_DATA_CH1CR1));
	SetString(DATAPAGE,IDC_DATA_CH1CR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch1 Short Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh1ShortCountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH1SCR4,
		GetString(DATAPAGE,IDC_DATA_CH1SCR3));
	SetString(DATAPAGE,IDC_DATA_CH1SCR3,
		GetString(DATAPAGE,IDC_DATA_CH1SCR2));
	SetString(DATAPAGE,IDC_DATA_CH1SCR2,
		GetString(DATAPAGE,IDC_DATA_CH1SCR1));
	SetString(DATAPAGE,IDC_DATA_CH1SCR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch1 Long Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh1LongCountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH1LCR4,
		GetString(DATAPAGE,IDC_DATA_CH1LCR3));
	SetString(DATAPAGE,IDC_DATA_CH1LCR3,
		GetString(DATAPAGE,IDC_DATA_CH1LCR2));
	SetString(DATAPAGE,IDC_DATA_CH1LCR2,
		GetString(DATAPAGE,IDC_DATA_CH1LCR1));
	SetString(DATAPAGE,IDC_DATA_CH1LCR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch2 Status
	sprintf(szTemp,"%02x",
		psAcquireRecord1->bCh2Status);

	SetString(DATAPAGE,IDC_DATA_CH2STATUS4,
		GetString(DATAPAGE,IDC_DATA_CH2STATUS3));
	SetString(DATAPAGE,IDC_DATA_CH2STATUS3,
		GetString(DATAPAGE,IDC_DATA_CH2STATUS2));
	SetString(DATAPAGE,IDC_DATA_CH2STATUS2,
		GetString(DATAPAGE,IDC_DATA_CH2STATUS1));
	SetString(DATAPAGE,IDC_DATA_CH2STATUS1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch2 Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh2CountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH2CR4,
		GetString(DATAPAGE,IDC_DATA_CH2CR3));
	SetString(DATAPAGE,IDC_DATA_CH2CR3,
		GetString(DATAPAGE,IDC_DATA_CH2CR2));
	SetString(DATAPAGE,IDC_DATA_CH2CR2,
		GetString(DATAPAGE,IDC_DATA_CH2CR1));
	SetString(DATAPAGE,IDC_DATA_CH2CR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch2 Short Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh2ShortCountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH2SCR4,
		GetString(DATAPAGE,IDC_DATA_CH2SCR3));
	SetString(DATAPAGE,IDC_DATA_CH2SCR3,
		GetString(DATAPAGE,IDC_DATA_CH2SCR2));
	SetString(DATAPAGE,IDC_DATA_CH2SCR2,
		GetString(DATAPAGE,IDC_DATA_CH2SCR1));
	SetString(DATAPAGE,IDC_DATA_CH2SCR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch2 Long Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh2LongCountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH2LCR4,
		GetString(DATAPAGE,IDC_DATA_CH2LCR3));
	SetString(DATAPAGE,IDC_DATA_CH2LCR3,
		GetString(DATAPAGE,IDC_DATA_CH2LCR2));
	SetString(DATAPAGE,IDC_DATA_CH2LCR2,
		GetString(DATAPAGE,IDC_DATA_CH2LCR1));
	SetString(DATAPAGE,IDC_DATA_CH2LCR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch3 Status
	sprintf(szTemp,"%02x",
		psAcquireRecord1->bCh3Status);

	SetString(DATAPAGE,IDC_DATA_CH3STATUS4,
		GetString(DATAPAGE,IDC_DATA_CH3STATUS3));
	SetString(DATAPAGE,IDC_DATA_CH3STATUS3,
		GetString(DATAPAGE,IDC_DATA_CH3STATUS2));
	SetString(DATAPAGE,IDC_DATA_CH3STATUS2,
		GetString(DATAPAGE,IDC_DATA_CH3STATUS1));
	SetString(DATAPAGE,IDC_DATA_CH3STATUS1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch3 Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh3CountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH3CR4,
		GetString(DATAPAGE,IDC_DATA_CH3CR3));
	SetString(DATAPAGE,IDC_DATA_CH3CR3,
		GetString(DATAPAGE,IDC_DATA_CH3CR2));
	SetString(DATAPAGE,IDC_DATA_CH3CR2,
		GetString(DATAPAGE,IDC_DATA_CH3CR1));
	SetString(DATAPAGE,IDC_DATA_CH3CR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch3 Short Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh3ShortCountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH3SCR4,
		GetString(DATAPAGE,IDC_DATA_CH3SCR3));
	SetString(DATAPAGE,IDC_DATA_CH3SCR3,
		GetString(DATAPAGE,IDC_DATA_CH3SCR2));
	SetString(DATAPAGE,IDC_DATA_CH3SCR2,
		GetString(DATAPAGE,IDC_DATA_CH3SCR1));
	SetString(DATAPAGE,IDC_DATA_CH3SCR1,szTemp);

	strcat(szRecord,szTemp);

	//do the ch3 Long Count Rate
	sprintf(szTemp,"%8g",
		psAcquireRecord1->fCh3LongCountRate);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_CH3LCR4,
		GetString(DATAPAGE,IDC_DATA_CH3LCR3));
	SetString(DATAPAGE,IDC_DATA_CH3LCR3,
		GetString(DATAPAGE,IDC_DATA_CH3LCR2));
	SetString(DATAPAGE,IDC_DATA_CH3LCR2,
		GetString(DATAPAGE,IDC_DATA_CH3LCR1));
	SetString(DATAPAGE,IDC_DATA_CH3LCR1,szTemp);

	strcat(szRecord,szTemp);

	//do the CPU temperature
	//do the BOARD temperature
	sprintf(szTemp,"%+6g",
		psAcquireRecord1->fCPUTemp);
	StrFloatConvert(szTemp,6);
	strcat(szRecord,szTemp);
	sprintf(szTemp2,"%+6g",
		psAcquireRecord1->fBOARDTemp);
	StrFloatConvert(szTemp2,6);
	strcat(szRecord,szTemp2);
	strcat(szTemp," ");
	strcat(szTemp,szTemp2);
	SetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP4,
		GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP3));
	SetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP3,
		GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP2));
	SetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP2,
		GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP1));
	SetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP1,szTemp);

	//do the count time
	sprintf(szTemp,"%8.1f",
		((float)psAcquireRecord1->uiCountTime10ths)/10.0);
	StrFloatConvert(szTemp,8);
	SetString(DATAPAGE,IDC_DATA_LENGTHT4,
		GetString(DATAPAGE,IDC_DATA_LENGTHT3));
	SetString(DATAPAGE,IDC_DATA_LENGTHT3,
		GetString(DATAPAGE,IDC_DATA_LENGTHT2));
	SetString(DATAPAGE,IDC_DATA_LENGTHT2,
		GetString(DATAPAGE,IDC_DATA_LENGTHT1));
	SetString(DATAPAGE,IDC_DATA_LENGTHT1,szTemp);

	strcat(szRecord,szTemp);

	//send it off to be displayed if possible
	if (m_pPropertySheet)
		m_pPropertySheet->AcquirRec1(szRecord);

	//save a copy to update display later
	for (int i = 0; i < (NUMBEROFACQUIRERECS1DISPLAYED-1); i++)
		m_cAcquireRec1[i] = m_cAcquireRec1[i+1];
	m_cAcquireRec1[NUMBEROFACQUIRERECS1DISPLAYED-1] = szRecord;

	//save the display values
	//if not in dumplast sequence then add this record to MGD file
	if (m_bInDumpLast)
	{
		m_dPreviousBIDTime = psAcquireRecord1->uiJulianSeconds;
	}
	else
	{
		m_dAcquireRecordCountX++;
		if (m_dAcquireRecordCountX >= NUMBEROFACQUIRERECS) 
			m_dAcquireRecordCountX = NUMBEROFACQUIRERECS-1;
	}
	m_bFirst15Rec = false;

	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, szRecord);//dump file

	PostMessage(IDC_RECEIVE_RESULT,ACQUIRE_RECORD,true);

	return sizeof(RADAcquireRecord);
}

//record 0x16
//Environmental sensor board (ESB)acquire record -- BINARY
int CInstrument::OnBinaryESBAcquireRec(BYTE *pBinaryAt)
{
	int iRecordTarget = m_dRecordCountX;

	//change this to the real length
	char szRecord[1024];
	char szTemp[32];

	//get the address of a binary record
	ESBAcquireRecord* psAcquireRecord2 = (ESBAcquireRecord*)pBinaryAt;

	//big endian to little endian the time
	Reverse((BYTE*)&(psAcquireRecord2->uiJulianSeconds),sizeof(psAcquireRecord2->uiJulianSeconds));

	//big endian to little endian all the multibyte values 
	Reverse((BYTE*)&(psAcquireRecord2->usAccelerometer),sizeof(psAcquireRecord2->usAccelerometer));
	Reverse((BYTE*)&(psAcquireRecord2->usLightInt),sizeof(psAcquireRecord2->usLightInt));
	Reverse((BYTE*)&(psAcquireRecord2->usMagnetometerX),sizeof(psAcquireRecord2->usMagnetometerX));
	Reverse((BYTE*)&(psAcquireRecord2->usMagnetometerY),sizeof(psAcquireRecord2->usMagnetometerY));
	Reverse((BYTE*)&(psAcquireRecord2->usMagnetometerZ),sizeof(psAcquireRecord2->usMagnetometerZ));
	Reverse((BYTE*)&(psAcquireRecord2->usSurfWetness),sizeof(psAcquireRecord2->usSurfWetness));
	Reverse((BYTE*)&(psAcquireRecord2->fCPUTemp),sizeof(psAcquireRecord2->fCPUTemp));
	Reverse((BYTE*)&(psAcquireRecord2->fBOARDTemp),sizeof(psAcquireRecord2->fBOARDTemp));
	Reverse((BYTE*)&(psAcquireRecord2->bESBSerialNumber),sizeof(psAcquireRecord2->bESBSerialNumber));

	//big endian to little endian the elapsed time
	Reverse((BYTE*)&(psAcquireRecord2->uiLastRecIntervTime10ths),sizeof(psAcquireRecord2->uiLastRecIntervTime10ths));

	//save adjusted record for write to MGD file at end of dumpsegment
	//display the values
	//save adjusted record for write to MGD file at end of dumpsegment
	//save status in binary to acquire record
	memcpy(&(RecordX[iRecordTarget]),psAcquireRecord2,sizeof(ESBAcquireRecord));

	CJulianTime cTime;
	cTime.Init(psAcquireRecord2->uiJulianSeconds);

	//move some of the date displays down one
	SetString(DATA2PAGE,IDC_DATA2_DATADATE4,
		GetString(DATA2PAGE,IDC_DATA2_DATADATE3));
	SetString(DATA2PAGE,IDC_DATA2_DATADATE3,
		GetString(DATA2PAGE,IDC_DATA2_DATADATE2));
	SetString(DATA2PAGE,IDC_DATA2_DATADATE2,
		GetString(DATA2PAGE,IDC_DATA2_DATADATE1));
	SetString(DATA2PAGE,IDC_DATA2_DATADATE1,cTime.Date());

	//move all the date displays down one
	sprintf(szTemp,"%s.%d",cTime.Time(),psAcquireRecord2->bJulianSec10ths);
	SetString(DATAPAGE,IDC_DATA2_DATATIME4,
		GetString(DATAPAGE,IDC_DATA2_DATATIME3));
	SetString(DATAPAGE,IDC_DATA2_DATATIME3,
		GetString(DATAPAGE,IDC_DATA2_DATATIME2));
	SetString(DATAPAGE,IDC_DATA2_DATATIME2,
		GetString(DATAPAGE,IDC_DATA2_DATATIME1));
	SetString(DATAPAGE,IDC_DATA2_DATATIME1,szTemp);

	//build the text version of the record
	sprintf(szRecord,"%02x%10u%d%02x%02x%02x%02x",
		psAcquireRecord2->bOpcode,
		psAcquireRecord2->uiJulianSeconds,
		psAcquireRecord2->bJulianSec10ths,
		psAcquireRecord2->bStatusByte0,
		psAcquireRecord2->bStatusByte1,
		psAcquireRecord2->bStatusByte2,
		psAcquireRecord2->bStatusByte3);
	//move the rest of the displays down one
	//and build the text version of the record

	//do the status text
	sprintf(szTemp,"%02x:%02x:%02x:%02x",
		psAcquireRecord2->bStatusByte0,
		psAcquireRecord2->bStatusByte1,
		psAcquireRecord2->bStatusByte2,
		psAcquireRecord2->bStatusByte3);

	SetString(DATA2PAGE,IDC_DATA2_STATUS01234,
		GetString(DATA2PAGE,IDC_DATA2_STATUS01233));
	SetString(DATA2PAGE,IDC_DATA2_STATUS01233,
		GetString(DATA2PAGE,IDC_DATA2_STATUS01232));
	SetString(DATA2PAGE,IDC_DATA2_STATUS01232,
		GetString(DATA2PAGE,IDC_DATA2_STATUS01231));
	SetString(DATA2PAGE,IDC_DATA2_STATUS01231,szTemp);

	//do the Accelerometer
	sprintf(szTemp,"%d:0x%3x",
		psAcquireRecord2->usAccelerometer,psAcquireRecord2->usAccelerometer);

	SetString(DATA2PAGE,IDC_DATA2_ACCEL4,
		GetString(DATA2PAGE,IDC_DATA2_ACCEL3));
	SetString(DATA2PAGE,IDC_DATA2_ACCEL3,
		GetString(DATA2PAGE,IDC_DATA2_ACCEL2));
	SetString(DATA2PAGE,IDC_DATA2_ACCEL2,
		GetString(DATA2PAGE,IDC_DATA2_ACCEL1));
	SetString(DATA2PAGE,IDC_DATA2_ACCEL1,szTemp);
	sprintf(szTemp,"%03x",psAcquireRecord2->usAccelerometer);
	strcat(szRecord,szTemp);

	//do the Humidity
	sprintf(szTemp,"%d",
		psAcquireRecord2->ubHumidity);
	//StrFloatConvert(szTemp,8);
	SetString(DATA2PAGE,IDC_DATA2_HUMID4,
		GetString(DATA2PAGE,IDC_DATA2_HUMID3));
	SetString(DATA2PAGE,IDC_DATA2_HUMID3,
		GetString(DATA2PAGE,IDC_DATA2_HUMID2));
	SetString(DATA2PAGE,IDC_DATA2_HUMID2,
		GetString(DATA2PAGE,IDC_DATA2_HUMID1));
	SetString(DATA2PAGE,IDC_DATA2_HUMID1,szTemp);
	sprintf(szTemp,"%3d",psAcquireRecord2->ubHumidity);
	strcat(szRecord,szTemp);

	//do the Light Intensity
	sprintf(szTemp,"%d:0x%03x",
		psAcquireRecord2->usLightInt,psAcquireRecord2->usLightInt);
	//StrFloatConvert(szTemp,8);
	SetString(DATA2PAGE,IDC_DATA2_LIGHT4,
		GetString(DATA2PAGE,IDC_DATA2_LIGHT3));
	SetString(DATA2PAGE,IDC_DATA2_LIGHT3,
		GetString(DATA2PAGE,IDC_DATA2_LIGHT2));
	SetString(DATA2PAGE,IDC_DATA2_LIGHT2,
		GetString(DATA2PAGE,IDC_DATA2_LIGHT1));
	SetString(DATA2PAGE,IDC_DATA2_LIGHT1,szTemp);
	sprintf(szTemp,"%03x",psAcquireRecord2->usLightInt);
	strcat(szRecord,szTemp);

	//do the MagnetometerX
	sprintf(szTemp,"%0d:0x%03x",
		psAcquireRecord2->usMagnetometerX,psAcquireRecord2->usMagnetometerX);
	//StrFloatConvert(szTemp,8);
	SetString(DATA2PAGE,IDC_DATA2_MAGX4,
		GetString(DATA2PAGE,IDC_DATA2_MAGX3));
	SetString(DATA2PAGE,IDC_DATA2_MAGX3,
		GetString(DATA2PAGE,IDC_DATA2_MAGX2));
	SetString(DATA2PAGE,IDC_DATA2_MAGX2,
		GetString(DATA2PAGE,IDC_DATA2_MAGX1));
	SetString(DATA2PAGE,IDC_DATA2_MAGX1,szTemp);
	sprintf(szTemp,"%03x",psAcquireRecord2->usMagnetometerX);
	strcat(szRecord,szTemp);

	//do the MagnetometerY
	sprintf(szTemp,"%d:0x%03x",
		psAcquireRecord2->usMagnetometerY,psAcquireRecord2->usMagnetometerY);
	//StrFloatConvert(szTemp,8);
	SetString(DATA2PAGE,IDC_DATA2_MAGY4,
		GetString(DATA2PAGE,IDC_DATA2_MAGY3));
	SetString(DATA2PAGE,IDC_DATA2_MAGY3,
		GetString(DATA2PAGE,IDC_DATA2_MAGY2));
	SetString(DATA2PAGE,IDC_DATA2_MAGY2,
		GetString(DATA2PAGE,IDC_DATA2_MAGY1));
	SetString(DATA2PAGE,IDC_DATA2_MAGY1,szTemp);
	sprintf(szTemp,"%03x",psAcquireRecord2->usMagnetometerY);
	strcat(szRecord,szTemp);

	//do the MagnetometerZ
	sprintf(szTemp,"%d:0x%03x",
		psAcquireRecord2->usMagnetometerZ,psAcquireRecord2->usMagnetometerZ);
	//StrFloatConvert(szTemp,8);
	SetString(DATA2PAGE,IDC_DATA2_MAGZ4,
		GetString(DATA2PAGE,IDC_DATA2_MAGZ3));
	SetString(DATA2PAGE,IDC_DATA2_MAGZ3,
		GetString(DATA2PAGE,IDC_DATA2_MAGZ2));
	SetString(DATA2PAGE,IDC_DATA2_MAGZ2,
		GetString(DATA2PAGE,IDC_DATA2_MAGZ1));
	SetString(DATA2PAGE,IDC_DATA2_MAGZ1,szTemp);
	sprintf(szTemp,"%03x",psAcquireRecord2->usMagnetometerZ);
	strcat(szRecord,szTemp);

	//do the Surface Wetness
	sprintf(szTemp,"%d:0x%03x",
		psAcquireRecord2->usSurfWetness,psAcquireRecord2->usSurfWetness);
	SetString(DATA2PAGE,IDC_DATA2_SURFWET4,
		GetString(DATA2PAGE,IDC_DATA2_SURFWET3));
	SetString(DATA2PAGE,IDC_DATA2_SURFWET3,
		GetString(DATA2PAGE,IDC_DATA2_SURFWET2));
	SetString(DATA2PAGE,IDC_DATA2_SURFWET2,
		GetString(DATA2PAGE,IDC_DATA2_SURFWET1));
	SetString(DATA2PAGE,IDC_DATA2_SURFWET1,szTemp);
	sprintf(szTemp,"%03x",psAcquireRecord2->usSurfWetness);
	strcat(szRecord,szTemp);

	//do the CPU Temperature
	sprintf(szTemp,"%+6.1f",
		psAcquireRecord2->fCPUTemp);
	SetString(DATA2PAGE,IDC_DATA2_CPUTEMP4,
		GetString(DATA2PAGE,IDC_DATA2_CPUTEMP3));
	SetString(DATA2PAGE,IDC_DATA2_CPUTEMP3,
		GetString(DATA2PAGE,IDC_DATA2_CPUTEMP2));
	SetString(DATA2PAGE,IDC_DATA2_CPUTEMP2,
		GetString(DATA2PAGE,IDC_DATA2_CPUTEMP1));
	SetString(DATA2PAGE,IDC_DATA2_CPUTEMP1,szTemp);
	strcat(szRecord,szTemp);

	//do the BOARD Temperature
	sprintf(szTemp,"%+6.1f",
		psAcquireRecord2->fBOARDTemp);
	SetString(DATA2PAGE,IDC_DATA2_BOARDTEMP4,
		GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP3));
	SetString(DATA2PAGE,IDC_DATA2_BOARDTEMP3,
		GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP2));
	SetString(DATA2PAGE,IDC_DATA2_BOARDTEMP2,
		GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP1));
	SetString(DATA2PAGE,IDC_DATA2_BOARDTEMP1,szTemp);
	strcat(szRecord,szTemp);

	//do the count time
	sprintf(szTemp,"%8.1f",
		((float)psAcquireRecord2->uiLastRecIntervTime10ths)/10.0);
	StrFloatConvert(szTemp,8);
	SetString(DATA2PAGE,IDC_DATA2_INTERVAL4,
		GetString(DATA2PAGE,IDC_DATA2_INTERVAL3));
	SetString(DATA2PAGE,IDC_DATA2_INTERVAL3,
		GetString(DATA2PAGE,IDC_DATA2_INTERVAL2));
	SetString(DATA2PAGE,IDC_DATA2_INTERVAL2,
		GetString(DATA2PAGE,IDC_DATA2_INTERVAL1));
	SetString(DATA2PAGE,IDC_DATA2_INTERVAL1,szTemp);
	strcat(szRecord,szTemp);

	//do the serial number
	sprintf(szTemp,"%02x%02X%02X%02x%02X%02X",
		psAcquireRecord2->bESBSerialNumber[0],
		psAcquireRecord2->bESBSerialNumber[1],
		psAcquireRecord2->bESBSerialNumber[2],
		psAcquireRecord2->bESBSerialNumber[3],
		psAcquireRecord2->bESBSerialNumber[4],
		psAcquireRecord2->bESBSerialNumber[5]);
	SetString(DATA2PAGE,IDC_DATA2_SERIALNUM4,
		GetString(DATA2PAGE,IDC_DATA2_SERIALNUM3));
	SetString(DATA2PAGE,IDC_DATA2_SERIALNUM3,
		GetString(DATA2PAGE,IDC_DATA2_SERIALNUM2));
	SetString(DATA2PAGE,IDC_DATA2_SERIALNUM2,
		GetString(DATA2PAGE,IDC_DATA2_SERIALNUM1));
	SetString(DATA2PAGE,IDC_DATA2_SERIALNUM1,szTemp);
	strcat(szRecord,szTemp);

	//send it off to be displayed if possible
	if (m_pPropertySheet)
		m_pPropertySheet->AcquirRec2(szRecord);

	//save a copy to update display later
	for (int i = 0; i < (NUMBEROFACQUIRERECS2DISPLAYED-1); i++)
		m_cAcquireRec2[i] = m_cAcquireRec2[i+1];
	m_cAcquireRec2[NUMBEROFACQUIRERECS2DISPLAYED-1] = szRecord;

	//save the display values
	//if not in dumplast sequence then add this record to MGD file
//	if (m_bInDumpLast)
//	{
//		m_dPreviousBIDTime2 = psAcquireRecord2->uiJulianSeconds;
//	}
//	else
//	{
//		m_dRecordCountX++;
//		if (m_dRecordCountX >= NUMBEROFSOHRECS) 
//			m_dRecordCountX = NUMBEROFSOHRECS-1;
//	}
//	m_bFirst15Rec = false;

	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, szRecord);//dump file

	PostMessage(IDC_RECEIVE_RESULT,ACQUIRE_RECORD,true);

	return sizeof(ESBAcquireRecord);
}
/*
//record 15
//old acquire record -- BINARY (TO BE REMOVED WHEN NO LONGER NEEDED)
int CInstrument::BinaryAcquirRec(BYTE *pBinaryAt)
{
	//expect this binary record

#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;
		UCHAR type;
		unsigned int julian;
		UCHAR status;
		float rates[5];
		float g_unc[2];
		unsigned int elapsed_time;
	} *psBinaryRecord;
#pragma pack()

	
	//get the address of a binary record
	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	//big endian to little endian the time
	Reverse((BYTE*)&(psBinaryRecord->julian),4);

	//big endian to little endian the rates
	for (int i = 0; i < 5; i++)
		Reverse((BYTE*)&(psBinaryRecord->rates[i]),4);
	for (i = 0; i < 2; i++)
		Reverse((BYTE*)&(psBinaryRecord->g_unc[i]),4);

	//big endian to little endian the elapsed time
	Reverse((BYTE*)&(psBinaryRecord->elapsed_time),4);

	//area to build the full text record
	char Record[1024];

	//areas to mess with the individual rates
	char szTemp0[3][32];
	char szTemp1[2][32];
	char szTemp2[2][32];

	//format the neutron rates
	for (i = 0; i < 2; i++)
	{
		if (fabs((double)(psBinaryRecord->rates[3+i])) < 1.0e-8)
			psBinaryRecord->rates[3+i] = 0.0;
		if (fabs((double)(psBinaryRecord->g_unc[i])) < 1.0e-8)
			psBinaryRecord->g_unc[i] = 0.0;

		//correct for - rate!!!!!
		sprintf(szTemp1[i],"%#10.3e",psBinaryRecord->rates[3+i]);
		if (strlen(szTemp1[i]) > 10)
			sprintf(szTemp1[i], "%#10.2e",psBinaryRecord->rates[3+i]);

		sprintf(szTemp2[i],"%#9.2e",psBinaryRecord->g_unc[i]);
		if (strlen(szTemp2[i]) > 9)
			sprintf(szTemp2[i],"%#9.1e",psBinaryRecord->g_unc[i]);
		
		//
		// pjm
		//
		// Format definition for e  double Signed value having the form 
		// [ - ]d.dddd e [sign]ddd where d is a single decimal digit, 
		// dddd is one or more decimal digits, ddd is exactly three decimal digits, 
		// and sign is + or -. 
		//
		// NOTE:  "exactly three" decimal digits for the exponent.

		TrunkExp(szTemp1[i],1);
		TrunkExp(szTemp2[i],1);
	}

	//SCR00236: Large numbers need e-format representation for 
	//binary BBM data conversions.
	//format the gamma rates
	for (i = 0; i < 3; i++)
	{
		if (psBinaryRecord->rates[i] >= 999999.95)
		{
			sprintf(szTemp0[i],"%10.4e",psBinaryRecord->rates[i]);
			TrunkExp(szTemp0[i],1);
		}
		else
			sprintf(szTemp0[i],"%8.1f",psBinaryRecord->rates[i]);
	}

	//pre-SCR00236
	//format the gamma rates
	//for (i = 0; i < 3; i++)
	//{
	//	if (psBinaryRecord->rates[i] >= 1000000)
	//		sprintf(szTemp0[i],"%8.0f",psBinaryRecord->rates[i]);
	//	else
	//		sprintf(szTemp0[i],"%8.1f",psBinaryRecord->rates[i]);
	//}

	//build the text record
	sprintf(Record,"15%10u%02x%s%s%s%s%s%s%s%5u",
		psBinaryRecord->julian,
		psBinaryRecord->status,
		szTemp0[0],
		szTemp0[1],
		szTemp0[2],
		szTemp1[0],
		szTemp2[0],
		szTemp1[1],
		szTemp2[1],
		psBinaryRecord->elapsed_time
		);

	//add checksum
	AppendCheckSum(Record);

	//add the two bytes for authentication
	strcat(Record,"--");
	
	//send to the first tab page
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	//tell the watch window
	PostToWatch(Record);
	//handle the record with the "text" version handler
	AcquirRec(Record);
	//return the number of bytes the binary record
	return sizeof(SBinaryRec);
}
*/
void CInstrument::StrFloatConvert(char*cString, int iFieldSize)
{
	int iLength = strlen(cString);
	if (iLength > iFieldSize)
	{
		if ((cString[iLength-4] == '+') || (cString[iLength-4] == '-'))
		{
			for (int i = 0; i < 3; i++)
				cString[iLength-3+i] = cString[iLength-2+i];
		}
		iLength--;
		if (iLength > iFieldSize)
		{
			char* chAt = strchr(cString,'e');
			if (chAt)
				strcpy(&cString[4],chAt);
		}
	}
//	iLength = strlen(cString);
}

void CInstrument::OnRADAcquireRecord(char *szRecord)
{

	int CheckSum;
	if (LengthOk(szRecord,sizeof(RADAcquireTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		char szTemp[32];
		char szTemp2[16];

		RADAcquireRecord AcquireRec1;

		//make a pointer to the correct type of record
		//get a pointer to a ESBAcquireTextRecord
		RADAcquireTextRecord* pszRecord = (RADAcquireTextRecord*)szRecord;

		//save the location in to set to write out to the MGD file
		AcquireRec1.bLength = sizeof(RADAcquireRecord);

		strncpy(szTemp,(char*)pszRecord->chOpcode,sizeof(pszRecord->chOpcode));
		szTemp[sizeof(pszRecord->chOpcode)] = NULL;
		sscanf(szTemp,"%x",&AcquireRec1.bOpcode);

 		//get julian time and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
		szTemp[sizeof(pszRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp,"%u",&AcquireRec1.uiJulianSeconds);
		AcquireRec1.ucJulianSec10ths = pszRecord->cdJulianSec10ths - '0';

		CJulianTime cTime;
		cTime.Init(AcquireRec1.uiJulianSeconds);

		//move some of the date displays down one
		SetString(DATAPAGE,IDC_DATA_DATADATE4,
			GetString(DATAPAGE,IDC_DATA_DATADATE3));
		SetString(DATAPAGE,IDC_DATA_DATADATE3,
			GetString(DATAPAGE,IDC_DATA_DATADATE2));
		SetString(DATAPAGE,IDC_DATA_DATADATE2,
			GetString(DATAPAGE,IDC_DATA_DATADATE1));
		SetString(DATAPAGE,IDC_DATA_DATADATE1,cTime.Date());

		//move all the date displays down one
		sprintf(szTemp,"%s.%c",cTime.Time(),pszRecord->cdJulianSec10ths);
		SetString(DATAPAGE,IDC_DATA_DATATIME4,
			GetString(DATAPAGE,IDC_DATA_DATATIME3));
		SetString(DATAPAGE,IDC_DATA_DATATIME3,
			GetString(DATAPAGE,IDC_DATA_DATATIME2));
		SetString(DATAPAGE,IDC_DATA_DATATIME2,
			GetString(DATAPAGE,IDC_DATA_DATATIME1));
		SetString(DATAPAGE,IDC_DATA_DATATIME1,szTemp);

		//get the status bytes and put them in the binary record
		strncpy(szTemp2,(char*)pszRecord->chStatusByte0,sizeof(pszRecord->chStatusByte0));
		szTemp2[sizeof(pszRecord->chStatusByte0)] = NULL;
		sscanf(szTemp2,"%2x",&AcquireRec1.bStatusByte0);
		strcpy(szTemp,szTemp2);
		strcat(szTemp,":");

		strncpy(szTemp2,(char*)pszRecord->chStatusByte1,sizeof(pszRecord->chStatusByte1));
		szTemp2[sizeof(pszRecord->chStatusByte1)] = NULL;
		sscanf(szTemp2,"%2x",&AcquireRec1.bStatusByte1);
		strcat(szTemp,szTemp2);
		strcat(szTemp,":");

		strncpy(szTemp2,(char*)pszRecord->chStatusByte2,sizeof(pszRecord->chStatusByte2));
		szTemp2[sizeof(pszRecord->chStatusByte2)] = NULL;
		sscanf(szTemp2,"%2x",&AcquireRec1.bStatusByte2);
		strcat(szTemp,szTemp2);
		strcat(szTemp,":");

		strncpy(szTemp2,(char*)pszRecord->chStatusByte3,sizeof(pszRecord->chStatusByte3));
		szTemp2[sizeof(pszRecord->chStatusByte3)] = NULL;
		sscanf(szTemp2,"%2x",&AcquireRec1.bStatusByte3);
		strcat(szTemp,szTemp2);
		
		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_STATUS01234,
			GetString(DATAPAGE,IDC_DATA_STATUS01233));
		SetString(DATAPAGE,IDC_DATA_STATUS01233,
			GetString(DATAPAGE,IDC_DATA_STATUS01232));
		SetString(DATAPAGE,IDC_DATA_STATUS01232,
			GetString(DATAPAGE,IDC_DATA_STATUS01231));
		SetString(DATAPAGE,IDC_DATA_STATUS01231,szTemp);

		//get the pp input state and put it in the binary record
		szTemp[0] = pszRecord->chPPInputState;
		szTemp[1] = NULL;
		sscanf(szTemp,"%x",&AcquireRec1.bPPInputState);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_PPINPUT4,
			GetString(DATAPAGE,IDC_DATA_PPINPUT3));
		SetString(DATAPAGE,IDC_DATA_PPINPUT3,
			GetString(DATAPAGE,IDC_DATA_PPINPUT2));
		SetString(DATAPAGE,IDC_DATA_PPINPUT2,
			GetString(DATAPAGE,IDC_DATA_PPINPUT1));
		SetString(DATAPAGE,IDC_DATA_PPINPUT1,szTemp);

		//CH 0
		
		//get the ch0 status and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->chCh0Status,sizeof(pszRecord->chCh0Status));
		szTemp[sizeof(pszRecord->chCh0Status)] = NULL;
		sscanf(szTemp,"%x",&AcquireRec1.bCh0Status);

		//move the saved versions all done one
		SetString(DATAPAGE,IDC_DATA_CH0STATUS4,
			GetString(DATAPAGE,IDC_DATA_CH0STATUS3));
		SetString(DATAPAGE,IDC_DATA_CH0STATUS3,
			GetString(DATAPAGE,IDC_DATA_CH0STATUS2));
		SetString(DATAPAGE,IDC_DATA_CH0STATUS2,
			GetString(DATAPAGE,IDC_DATA_CH0STATUS1));
		SetString(DATAPAGE,IDC_DATA_CH0STATUS1,szTemp);

		//get the ch0 count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh0CountRate,sizeof(pszRecord->cfCh0CountRate));
		szTemp[sizeof(pszRecord->cfCh0CountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh0CountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH0CR4,
			GetString(DATAPAGE,IDC_DATA_CH0CR3));
		SetString(DATAPAGE,IDC_DATA_CH0CR3,
			GetString(DATAPAGE,IDC_DATA_CH0CR2));
		SetString(DATAPAGE,IDC_DATA_CH0CR2,
			GetString(DATAPAGE,IDC_DATA_CH0CR1));
		SetString(DATAPAGE,IDC_DATA_CH0CR1,szTemp);

		//get the ch0 short count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh0ShortCountRate,sizeof(pszRecord->cfCh0ShortCountRate));
		szTemp[sizeof(pszRecord->cfCh0ShortCountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh0ShortCountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH0SCR4,
			GetString(DATAPAGE,IDC_DATA_CH0SCR3));
		SetString(DATAPAGE,IDC_DATA_CH0SCR3,
			GetString(DATAPAGE,IDC_DATA_CH0SCR2));
		SetString(DATAPAGE,IDC_DATA_CH0SCR2,
			GetString(DATAPAGE,IDC_DATA_CH0SCR1));
		SetString(DATAPAGE,IDC_DATA_CH0SCR1,szTemp);

		//get the ch0 long count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh0LongCountRate,sizeof(pszRecord->cfCh0LongCountRate));
		szTemp[sizeof(pszRecord->cfCh0LongCountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh0LongCountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH0LCR4,
			GetString(DATAPAGE,IDC_DATA_CH0LCR3));
		SetString(DATAPAGE,IDC_DATA_CH0LCR3,
			GetString(DATAPAGE,IDC_DATA_CH0LCR2));
		SetString(DATAPAGE,IDC_DATA_CH0LCR2,
			GetString(DATAPAGE,IDC_DATA_CH0LCR1));
		SetString(DATAPAGE,IDC_DATA_CH0LCR1,szTemp);

		//CH 1
		
		//get the ch1 status and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->chCh1Status,sizeof(pszRecord->chCh1Status));
		szTemp[sizeof(pszRecord->chCh1Status)] = NULL;
		sscanf(szTemp,"%2x",&AcquireRec1.bCh1Status);

		//move the saved versions all done one
		SetString(DATAPAGE,IDC_DATA_CH1STATUS4,
			GetString(DATAPAGE,IDC_DATA_CH1STATUS3));
		SetString(DATAPAGE,IDC_DATA_CH1STATUS3,
			GetString(DATAPAGE,IDC_DATA_CH1STATUS2));
		SetString(DATAPAGE,IDC_DATA_CH1STATUS2,
			GetString(DATAPAGE,IDC_DATA_CH1STATUS1));
		SetString(DATAPAGE,IDC_DATA_CH1STATUS1,szTemp);

		//get the ch1 count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh1CountRate,sizeof(pszRecord->cfCh1CountRate));
		szTemp[sizeof(pszRecord->cfCh1CountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh1CountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH1CR4,
			GetString(DATAPAGE,IDC_DATA_CH1CR3));
		SetString(DATAPAGE,IDC_DATA_CH1CR3,
			GetString(DATAPAGE,IDC_DATA_CH1CR2));
		SetString(DATAPAGE,IDC_DATA_CH1CR2,
			GetString(DATAPAGE,IDC_DATA_CH1CR1));
		SetString(DATAPAGE,IDC_DATA_CH1CR1,szTemp);

		//get the ch1 short count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh1ShortCountRate,sizeof(pszRecord->cfCh1ShortCountRate));
		szTemp[sizeof(pszRecord->cfCh1ShortCountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh1ShortCountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH1SCR4,
			GetString(DATAPAGE,IDC_DATA_CH1SCR3));
		SetString(DATAPAGE,IDC_DATA_CH1SCR3,
			GetString(DATAPAGE,IDC_DATA_CH1SCR2));
		SetString(DATAPAGE,IDC_DATA_CH1SCR2,
			GetString(DATAPAGE,IDC_DATA_CH1SCR1));
		SetString(DATAPAGE,IDC_DATA_CH1SCR1,szTemp);

		//get the ch1 long count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh1LongCountRate,sizeof(pszRecord->cfCh1LongCountRate));
		szTemp[sizeof(pszRecord->cfCh1LongCountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh1LongCountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH1LCR4,
			GetString(DATAPAGE,IDC_DATA_CH1LCR3));
		SetString(DATAPAGE,IDC_DATA_CH1LCR3,
			GetString(DATAPAGE,IDC_DATA_CH1LCR2));
		SetString(DATAPAGE,IDC_DATA_CH1LCR2,
			GetString(DATAPAGE,IDC_DATA_CH1LCR1));
		SetString(DATAPAGE,IDC_DATA_CH1LCR1,szTemp);

		//CH 2
		
		//get the ch2 status and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->chCh2Status,sizeof(pszRecord->chCh2Status));
		szTemp[sizeof(pszRecord->chCh2Status)] = NULL;
		sscanf(szTemp,"%2x",&AcquireRec1.bCh2Status);

		//move the saved versions all done one
		SetString(DATAPAGE,IDC_DATA_CH2STATUS4,
			GetString(DATAPAGE,IDC_DATA_CH2STATUS3));
		SetString(DATAPAGE,IDC_DATA_CH2STATUS3,
			GetString(DATAPAGE,IDC_DATA_CH2STATUS2));
		SetString(DATAPAGE,IDC_DATA_CH2STATUS2,
			GetString(DATAPAGE,IDC_DATA_CH2STATUS1));
		SetString(DATAPAGE,IDC_DATA_CH2STATUS1,szTemp);

		//get the ch2 count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh2CountRate,sizeof(pszRecord->cfCh2CountRate));
		szTemp[sizeof(pszRecord->cfCh2CountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh2CountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH2CR4,
			GetString(DATAPAGE,IDC_DATA_CH2CR3));
		SetString(DATAPAGE,IDC_DATA_CH2CR3,
			GetString(DATAPAGE,IDC_DATA_CH2CR2));
		SetString(DATAPAGE,IDC_DATA_CH2CR2,
			GetString(DATAPAGE,IDC_DATA_CH2CR1));
		SetString(DATAPAGE,IDC_DATA_CH2CR1,szTemp);

		//get the ch2 short count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh2ShortCountRate,sizeof(pszRecord->cfCh2ShortCountRate));
		szTemp[sizeof(pszRecord->cfCh2ShortCountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh2ShortCountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH2SCR4,
			GetString(DATAPAGE,IDC_DATA_CH2SCR3));
		SetString(DATAPAGE,IDC_DATA_CH2SCR3,
			GetString(DATAPAGE,IDC_DATA_CH2SCR2));
		SetString(DATAPAGE,IDC_DATA_CH2SCR2,
			GetString(DATAPAGE,IDC_DATA_CH2SCR1));
		SetString(DATAPAGE,IDC_DATA_CH2SCR1,szTemp);

		//get the ch2 long count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh2LongCountRate,sizeof(pszRecord->cfCh2LongCountRate));
		szTemp[sizeof(pszRecord->cfCh2LongCountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh2LongCountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH2LCR4,
			GetString(DATAPAGE,IDC_DATA_CH2LCR3));
		SetString(DATAPAGE,IDC_DATA_CH2LCR3,
			GetString(DATAPAGE,IDC_DATA_CH2LCR2));
		SetString(DATAPAGE,IDC_DATA_CH2LCR2,
			GetString(DATAPAGE,IDC_DATA_CH2LCR1));
		SetString(DATAPAGE,IDC_DATA_CH2LCR1,szTemp);

		//CH 3
		
		//get the ch3 status and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->chCh3Status,sizeof(pszRecord->chCh3Status));
		szTemp[sizeof(pszRecord->chCh3Status)] = NULL;
		sscanf(szTemp,"%2x",&AcquireRec1.bCh3Status);

		//move the saved versions all done one
		SetString(DATAPAGE,IDC_DATA_CH3STATUS4,
			GetString(DATAPAGE,IDC_DATA_CH3STATUS3));
		SetString(DATAPAGE,IDC_DATA_CH3STATUS3,
			GetString(DATAPAGE,IDC_DATA_CH3STATUS2));
		SetString(DATAPAGE,IDC_DATA_CH3STATUS2,
			GetString(DATAPAGE,IDC_DATA_CH3STATUS1));
		SetString(DATAPAGE,IDC_DATA_CH3STATUS1,szTemp);

		//get the ch3 count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh3CountRate,sizeof(pszRecord->cfCh3CountRate));
		szTemp[sizeof(pszRecord->cfCh3CountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh3CountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH3CR4,
			GetString(DATAPAGE,IDC_DATA_CH3CR3));
		SetString(DATAPAGE,IDC_DATA_CH3CR3,
			GetString(DATAPAGE,IDC_DATA_CH3CR2));
		SetString(DATAPAGE,IDC_DATA_CH3CR2,
			GetString(DATAPAGE,IDC_DATA_CH3CR1));
		SetString(DATAPAGE,IDC_DATA_CH3CR1,szTemp);

		//get the ch3 short count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh3ShortCountRate,sizeof(pszRecord->cfCh3ShortCountRate));
		szTemp[sizeof(pszRecord->cfCh3ShortCountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh3ShortCountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH3SCR4,
			GetString(DATAPAGE,IDC_DATA_CH3SCR3));
		SetString(DATAPAGE,IDC_DATA_CH3SCR3,
			GetString(DATAPAGE,IDC_DATA_CH3SCR2));
		SetString(DATAPAGE,IDC_DATA_CH3SCR2,
			GetString(DATAPAGE,IDC_DATA_CH3SCR1));
		SetString(DATAPAGE,IDC_DATA_CH3SCR1,szTemp);

		//get the ch3 long count rate and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cfCh3LongCountRate,sizeof(pszRecord->cfCh3LongCountRate));
		szTemp[sizeof(pszRecord->cfCh3LongCountRate)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCh3LongCountRate);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH3LCR4,
			GetString(DATAPAGE,IDC_DATA_CH3LCR3));
		SetString(DATAPAGE,IDC_DATA_CH3LCR3,
			GetString(DATAPAGE,IDC_DATA_CH3LCR2));
		SetString(DATAPAGE,IDC_DATA_CH3LCR2,
			GetString(DATAPAGE,IDC_DATA_CH3LCR1));
		SetString(DATAPAGE,IDC_DATA_CH3LCR1,szTemp);

		//get the duration of this 
		float fTemp;
		strncpy(szTemp,(char*)pszRecord->cdCountTime10ths,sizeof(pszRecord->cdCountTime10ths));
		szTemp[sizeof(pszRecord->cdCountTime10ths)] = NULL;
		sscanf(szTemp,"%f",&fTemp);
		AcquireRec1.uiCountTime10ths = int(fTemp*10.0F);

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CH3LCR4,
			GetString(DATAPAGE,IDC_DATA_CH3LCR3));
		SetString(DATAPAGE,IDC_DATA_CH3LCR3,
			GetString(DATAPAGE,IDC_DATA_CH3LCR2));
		SetString(DATAPAGE,IDC_DATA_CH3LCR2,
			GetString(DATAPAGE,IDC_DATA_CH3LCR1));
		SetString(DATAPAGE,IDC_DATA_CH3LCR1,szTemp);

		//get the cpu and board tempuratures
		strncpy(szTemp,(char*)pszRecord->cdCPUTemp,sizeof(pszRecord->cdCPUTemp));
		szTemp[sizeof(pszRecord->cdCPUTemp)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec1.fCPUTemp);

		strncpy(szTemp2,(char*)pszRecord->cdBOARDTemp,sizeof(pszRecord->cdBOARDTemp));
		szTemp2[sizeof(pszRecord->cdBOARDTemp)] = NULL;
		sscanf(szTemp2,"%f",&AcquireRec1.fBOARDTemp);

		char* pszAt = szTemp2;
		while (*pszAt == ' ') pszAt++;
		strcat(szTemp, " ");
		strcat(szTemp, pszAt);
		pszAt = szTemp;
		while (*pszAt == ' ') pszAt++;

		//move the saved versions all down one
		SetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP4,
			GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP3));
		SetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP3,
			GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP2));
		SetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP2,
			GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP1));
		SetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP1,pszAt);

		//send it off to be displayed if possible
		if (m_pPropertySheet)
			m_pPropertySheet->AcquirRec1(szRecord);

		//save a copy to update display later
		//push them all down one to make room for the newest
		for (int i = 0; i < (NUMBEROFACQUIRERECS1DISPLAYED-1); i++)
			m_cAcquireRec1[i] = m_cAcquireRec1[i+1];
		//because m_cAcquireRec1 is an array of CString a new copy is created
		m_cAcquireRec1[NUMBEROFACQUIRERECS1DISPLAYED-1] = szRecord;

		//save the binary version of this record
		memcpy(&AcquireRecX[m_dAcquireRecordCountX],&AcquireRec1,sizeof(RADAcquireRecord));

		//save the display values
		//if not in dumplast sequence then add this record to MGD file
		if (m_bInDumpLast)
		{
			m_dPreviousBIDTime = AcquireRec1.uiJulianSeconds;
		}
		else
		{
			m_dAcquireRecordCountX++;
//TRACE("Acquire Record Count: %d\r\n",m_dAcquireRecordCountX);
			if (m_dAcquireRecordCountX >= NUMBEROFACQUIRERECS) 
				m_dAcquireRecordCountX = NUMBEROFACQUIRERECS-1;
		}
		m_bFirst15Rec = false;

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, szRecord);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,ACQUIRE_RECORD,CheckSum);
}

void CInstrument::OnESBAcquireRecord(char *szRecord)
{
//TRACE("Record:%d ESB:%d\r\n",strlen(szRecord),sizeof(ESBAcquireTextRecord));
	int CheckSum;
	if (LengthOk(szRecord,sizeof(ESBAcquireTextRecord)))
		CheckSum = Checksum(szRecord);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		//working space for each of the record items
		char szTemp[32];
		char szTemp2[8];

		//create a binary record
		ESBAcquireRecord AcquireRec2;

		//get a pointer to a ESBAcquireTextRecord
		ESBAcquireTextRecord* pszRecord = (ESBAcquireTextRecord*)szRecord;

		//get length and opcode and place in binary record
		AcquireRec2.bLength = sizeof(ESBAcquireRecord);
		strncpy(szTemp,(char*)pszRecord->chOpcode,sizeof(pszRecord->chOpcode));
		szTemp[sizeof(pszRecord->chOpcode)] = NULL;
		sscanf(szTemp,"%x",&AcquireRec2.bOpcode);

 		//get julian time and put it in the binary record
		strncpy(szTemp,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
		szTemp[sizeof(pszRecord->cdJulianSeconds)]=NULL;
		sscanf(szTemp,"%u",&AcquireRec2.uiJulianSeconds);

		//get the fractional time
		AcquireRec2.bJulianSec10ths = (unsigned char)(pszRecord->cdJulianSec10ths - '0');

		CJulianTime cTime;
		cTime.Init(AcquireRec2.uiJulianSeconds);

		//move some of the date displays down one
		SetString(DATA2PAGE,IDC_DATA2_DATADATE4,
			GetString(DATA2PAGE,IDC_DATA2_DATADATE3));
		SetString(DATA2PAGE,IDC_DATA2_DATADATE3,
			GetString(DATA2PAGE,IDC_DATA2_DATADATE2));
		SetString(DATA2PAGE,IDC_DATA2_DATADATE2,
			GetString(DATA2PAGE,IDC_DATA2_DATADATE1));
		SetString(DATA2PAGE,IDC_DATA2_DATADATE1,cTime.Date());

		//move all the date displays down one
		sprintf(szTemp,"%s.%c",	cTime.Time(),pszRecord->cdJulianSec10ths);
		SetString(DATA2PAGE,IDC_DATA2_DATATIME4,
			GetString(DATA2PAGE,IDC_DATA2_DATATIME3));
		SetString(DATA2PAGE,IDC_DATA2_DATATIME3,
			GetString(DATA2PAGE,IDC_DATA2_DATATIME2));
		SetString(DATA2PAGE,IDC_DATA2_DATATIME2,
			GetString(DATA2PAGE,IDC_DATA2_DATATIME1));
		SetString(DATA2PAGE,IDC_DATA2_DATADATE1,szTemp);

		//get the status bytes and put them in the binary record
		strncpy(szTemp2,(char*)pszRecord->chStatusByte0,sizeof(pszRecord->chStatusByte0));
		szTemp2[sizeof(pszRecord->chStatusByte0)] = NULL;
		sscanf(szTemp2,"%2x",&AcquireRec2.bStatusByte0);
		strcpy(szTemp,szTemp2);
		strcat(szTemp,":");

		strncpy(szTemp2,(char*)pszRecord->chStatusByte1,sizeof(pszRecord->chStatusByte1));
		szTemp2[sizeof(pszRecord->chStatusByte1)] = NULL;
		sscanf(szTemp2,"%2x",&AcquireRec2.bStatusByte1);
		strcat(szTemp,szTemp2);
		strcat(szTemp,":");

		strncpy(szTemp2,(char*)pszRecord->chStatusByte2,sizeof(pszRecord->chStatusByte2));
		szTemp2[sizeof(pszRecord->chStatusByte2)] = NULL;
		sscanf(szTemp2,"%2x",&AcquireRec2.bStatusByte2);
		strcat(szTemp,szTemp2);
		strcat(szTemp,":");

		strncpy(szTemp2,(char*)pszRecord->chStatusByte3,sizeof(pszRecord->chStatusByte3));
		szTemp2[sizeof(pszRecord->chStatusByte3)] = NULL;
		sscanf(szTemp2,"%2x",&AcquireRec2.bStatusByte3);
		strcat(szTemp,szTemp2);
		
		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_STATUS01234,
			GetString(DATA2PAGE,IDC_DATA2_STATUS01233));
		SetString(DATA2PAGE,IDC_DATA2_STATUS01233,
			GetString(DATA2PAGE,IDC_DATA2_STATUS01232));
		SetString(DATA2PAGE,IDC_DATA2_STATUS01232,
			GetString(DATA2PAGE,IDC_DATA2_STATUS01231));
		SetString(DATA2PAGE,IDC_DATA2_STATUS01231,szTemp);

		//get the accelerometer value and put it in the binary record
		memcpy(szTemp,(char*)pszRecord->chAccelerometer,sizeof(pszRecord->chAccelerometer))                      ;
		szTemp[sizeof(pszRecord->chAccelerometer)] = NULL;
		sscanf(szTemp,"%3x",&AcquireRec2.usAccelerometer);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_ACCEL4,
			GetString(DATA2PAGE,IDC_DATA2_ACCEL3));
		SetString(DATA2PAGE,IDC_DATA2_ACCEL3,
			GetString(DATA2PAGE,IDC_DATA2_ACCEL2));
		SetString(DATA2PAGE,IDC_DATA2_ACCEL2,
			GetString(DATA2PAGE,IDC_DATA2_ACCEL1));
		SetString(DATA2PAGE,IDC_DATA2_ACCEL1,szTemp);

		//do the Humidity value and put it in th ebinary record
		memcpy(szTemp,pszRecord->cdHumidity,sizeof(pszRecord->cdHumidity))                      ;
		szTemp[sizeof(pszRecord->cdHumidity)] = NULL;
		int iTemp;
		sscanf(szTemp,"%d",&iTemp);
		//if (iTemp > 100) iTemp = 100;
		AcquireRec2.ubHumidity = unsigned char(iTemp);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_HUMID4,
			GetString(DATA2PAGE,IDC_DATA2_HUMID3));
		SetString(DATA2PAGE,IDC_DATA2_HUMID3,
			GetString(DATA2PAGE,IDC_DATA2_HUMID2));
		SetString(DATA2PAGE,IDC_DATA2_HUMID2,
			GetString(DATA2PAGE,IDC_DATA2_HUMID1));
		SetString(DATA2PAGE,IDC_DATA2_HUMID1,szTemp);

		//do the Light Intensity
		memcpy(szTemp,pszRecord->chLightInt,sizeof(pszRecord->chLightInt))                      ;
		szTemp[sizeof(pszRecord->chLightInt)] = NULL;
		sscanf(szTemp,"%x",&AcquireRec2.usLightInt);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_LIGHT4,
			GetString(DATA2PAGE,IDC_DATA2_LIGHT3));
		SetString(DATA2PAGE,IDC_DATA2_LIGHT3,
			GetString(DATA2PAGE,IDC_DATA2_LIGHT2));
		SetString(DATA2PAGE,IDC_DATA2_LIGHT2,
			GetString(DATA2PAGE,IDC_DATA2_LIGHT1));
		SetString(DATA2PAGE,IDC_DATA2_LIGHT1,szTemp);

		//do the MagnetometerX
		memcpy(szTemp,pszRecord->chMagnetometerX,sizeof(pszRecord->chMagnetometerX))                      ;
		szTemp[sizeof(pszRecord->chMagnetometerX)] = NULL;
		sscanf(szTemp,"%x",&AcquireRec2.usMagnetometerX);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_MAGX4,
			GetString(DATA2PAGE,IDC_DATA2_MAGX3));
		SetString(DATA2PAGE,IDC_DATA2_MAGX3,
			GetString(DATA2PAGE,IDC_DATA2_MAGX2));
		SetString(DATA2PAGE,IDC_DATA2_MAGX2,
			GetString(DATA2PAGE,IDC_DATA2_MAGX1));
		SetString(DATA2PAGE,IDC_DATA2_MAGX1,szTemp);

		//do the MagnetometerY
		memcpy(szTemp,pszRecord->chMagnetometerY,sizeof(pszRecord->chMagnetometerY))                      ;
		szTemp[sizeof(pszRecord->chMagnetometerY)] = NULL;
		sscanf(szTemp,"%x",&AcquireRec2.usMagnetometerY);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_MAGY4,
			GetString(DATA2PAGE,IDC_DATA2_MAGY3));
		SetString(DATA2PAGE,IDC_DATA2_MAGY3,
			GetString(DATA2PAGE,IDC_DATA2_MAGY2));
		SetString(DATA2PAGE,IDC_DATA2_MAGY2,
			GetString(DATA2PAGE,IDC_DATA2_MAGY1));
		SetString(DATA2PAGE,IDC_DATA2_MAGY1,szTemp);

		//do the MagnetometerZ
		memcpy(szTemp,pszRecord->chMagnetometerZ,sizeof(pszRecord->chMagnetometerZ))                      ;
		szTemp[sizeof(pszRecord->chMagnetometerZ)] = NULL;
		sscanf(szTemp,"%x",&AcquireRec2.usMagnetometerZ);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_MAGZ4,
			GetString(DATA2PAGE,IDC_DATA2_MAGZ3));
		SetString(DATA2PAGE,IDC_DATA2_MAGZ3,
			GetString(DATA2PAGE,IDC_DATA2_MAGZ2));
		SetString(DATA2PAGE,IDC_DATA2_MAGZ2,
			GetString(DATA2PAGE,IDC_DATA2_MAGZ1));
		SetString(DATA2PAGE,IDC_DATA2_MAGZ1,szTemp);

		//do the Surface Wetness
		memcpy(szTemp,pszRecord->chSurfWetness,sizeof(pszRecord->chSurfWetness))                      ;
		szTemp[sizeof(pszRecord->chSurfWetness)] = NULL;
		sscanf(szTemp,"%x",&AcquireRec2.usSurfWetness);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_SURFWET4,
			GetString(DATA2PAGE,IDC_DATA2_SURFWET3));
		SetString(DATA2PAGE,IDC_DATA2_SURFWET3,
			GetString(DATA2PAGE,IDC_DATA2_SURFWET2));
		SetString(DATA2PAGE,IDC_DATA2_SURFWET2,
			GetString(DATA2PAGE,IDC_DATA2_SURFWET1));
		SetString(DATA2PAGE,IDC_DATA2_SURFWET1,szTemp);

		//do the CPU Temperature
		memcpy(szTemp,pszRecord->cfCPUTemp,sizeof(pszRecord->cfCPUTemp))                      ;
		szTemp[sizeof(pszRecord->cfCPUTemp)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec2.fCPUTemp);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_CPUTEMP4,
			GetString(DATA2PAGE,IDC_DATA2_CPUTEMP3));
		SetString(DATA2PAGE,IDC_DATA2_CPUTEMP3,
			GetString(DATA2PAGE,IDC_DATA2_CPUTEMP2));
		SetString(DATA2PAGE,IDC_DATA2_CPUTEMP2,
			GetString(DATA2PAGE,IDC_DATA2_CPUTEMP1));
		SetString(DATA2PAGE,IDC_DATA2_CPUTEMP1,szTemp);

		//do the BOARD Temperature
		memcpy(szTemp,pszRecord->cfBOARDTemp,sizeof(pszRecord->cfBOARDTemp))                      ;
		szTemp[sizeof(pszRecord->cfBOARDTemp)] = NULL;
		sscanf(szTemp,"%f",&AcquireRec2.fBOARDTemp);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_BOARDTEMP4,
			GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP3));
		SetString(DATA2PAGE,IDC_DATA2_BOARDTEMP3,
			GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP2));
		SetString(DATA2PAGE,IDC_DATA2_BOARDTEMP2,
			GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP1));
		SetString(DATA2PAGE,IDC_DATA2_BOARDTEMP1,szTemp);

		//do the count time
		memcpy(szTemp,(char*)pszRecord->cfLastRecIntervTime10ths,sizeof(pszRecord->cfLastRecIntervTime10ths))                      ;
		szTemp[sizeof(pszRecord->cfLastRecIntervTime10ths)] = NULL;
		float fTemp;
		sscanf(szTemp,"%f",&fTemp);
		AcquireRec2.uiLastRecIntervTime10ths = int(fTemp*10.0);

		//move the saved versions all down one
		SetString(DATA2PAGE,IDC_DATA2_INTERVAL4,
			GetString(DATA2PAGE,IDC_DATA2_INTERVAL3));
		SetString(DATA2PAGE,IDC_DATA2_INTERVAL3,
			GetString(DATA2PAGE,IDC_DATA2_INTERVAL2));
		SetString(DATA2PAGE,IDC_DATA2_INTERVAL2,
			GetString(DATA2PAGE,IDC_DATA2_INTERVAL1));
		SetString(DATA2PAGE,IDC_DATA2_INTERVAL1,szTemp);

		//do the Serial Number 
		memcpy(szTemp,pszRecord->chESBSerialNumber,sizeof(pszRecord->chESBSerialNumber))                      ;
		szTemp[sizeof(pszRecord->chESBSerialNumber)] = NULL;
		sscanf(szTemp,"%2x%2x%2x%2x%2x%2x",   
			&AcquireRec2.bESBSerialNumber[0],	//may need to reverse these
			&AcquireRec2.bESBSerialNumber[1],	//if we do then just change
			&AcquireRec2.bESBSerialNumber[2],	//the subscript numbers
			&AcquireRec2.bESBSerialNumber[3],
			&AcquireRec2.bESBSerialNumber[4],
			&AcquireRec2.bESBSerialNumber[5]);

		//move the saved serial numbers all down one
		SetString(DATA2PAGE,IDC_DATA2_SERIALNUM4,
			GetString(DATA2PAGE,IDC_DATA2_SERIALNUM3));
		SetString(DATA2PAGE,IDC_DATA2_SERIALNUM3,
			GetString(DATA2PAGE,IDC_DATA2_SERIALNUM2));
		SetString(DATA2PAGE,IDC_DATA2_SERIALNUM2,
			GetString(DATA2PAGE,IDC_DATA2_SERIALNUM1));
		SetString(DATA2PAGE,IDC_DATA2_SERIALNUM1,szTemp);

		//send it off to be displayed if possible
		if (m_pPropertySheet)
			m_pPropertySheet->AcquirRec2(szRecord);

		//save a copy to update display later
		for (int i = 0; i < (NUMBEROFACQUIRERECS2DISPLAYED-1); i++)
			m_cAcquireRec2[i] = m_cAcquireRec2[i+1];
		m_cAcquireRec2[NUMBEROFACQUIRERECS2DISPLAYED-1] = szRecord;

		//save the binary version of this record
		memcpy(&AcquireRecX[m_dAcquireRecordCountX],&AcquireRec2,sizeof(ESBAcquireRecord));

		//save the display values
		//if not in dumplast sequence then add this record to MGD file
//		if (m_bInDumpLast)
//		{
//			m_dPreviousBIDTime2 = AcquireRec2.uiJulianSeconds;
//		}
//		else
//		{
//			m_dAcquireRecordCountX++;
//			if (m_dAcquireRecordCountX >= NUMBEROFACQUIRERECS) 
//				m_dAcquireRecordCountX = NUMBEROFACQUIRERECS-1;
//		}
		m_bFirst15Rec = false;

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, szRecord);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,ACQUIRE_RECORD,CheckSum);
}

//record 15
//old acquire record (TO BE REMOVED WHEN NO LONGER REQUIRED)
void CInstrument::AcquirRec(char *Record)
{

	//test text RAD acquire record 1

	char szRecord[256];

	strncpy(&szRecord[0],"58",2);
	strncpy(&szRecord[2],&Record[2],10);
	strncpy(&szRecord[12],"1",1);
	szRecord[13] = NULL;
	strcat(szRecord,"112233445"
	"00 0.1e+01 0.2e+02 0.3e+0301 1.1e+01 1.2e+02 1.3e+0302 2.1e+01 2.2e+02 2.3e+0303 3.1e+01 3.2e+02"
	" 3.3e+03"
	"+999.1"
	"-888.2"
	"000100.0");

	char szTemp[16];
	unsigned short sum = 0;
	for (unsigned int i=0; i<strlen(szRecord); i++)
		sum = (unsigned short)(sum + szRecord[i]);
	sum &= 0x00ff;	
	sprintf(szTemp,"%02x",sum);
	strcat(szRecord,szTemp);
	strcat(szRecord,"\r\n");
	OnRADAcquireRecord(szRecord);

/*
	//test binary RAD acquire record 1

	char szTemp[32];
	RADAcquireRecord Acq1;

	Acq1.bLength = sizeof(RADAcquireRecord);
	Acq1.bOpcode = 0x058;

	strncpy(szTemp,&Record[2],10);
	szTemp[10] = NULL;
	sscanf(szTemp,"%u",&Acq1.uiJulianSeconds);
	Reverse((BYTE*)&Acq1.uiJulianSeconds,4);

	Acq1.ucJulianSec10ths = 0x03;
		
	Acq1.bStatusByte0 = 0xff;
	Acq1.bStatusByte1 = 0x88;
	Acq1.bStatusByte2 = 0x44;
	Acq1.bStatusByte3 = 0x22;
	Acq1.bPPInputState = 0x01;
	Acq1.bCh0Status = 0x01;
	Acq1.fCh0CountRate = 100.0F;
	Reverse((unsigned char*)&Acq1.fCh0CountRate,4);
	Acq1.fCh0ShortCountRate = 102.2F;
	Reverse((unsigned char*)&Acq1.fCh0ShortCountRate,4);
	Acq1.fCh0LongCountRate = 101.1F;
	Reverse((unsigned char*)&Acq1.fCh0LongCountRate,4);
	Acq1.bCh1Status = 0x02;
	Acq1.fCh1CountRate = 200.0F;
//	Acq1.fCh1CountRate = 3.402823466e+38F;
	Reverse((unsigned char*)&Acq1.fCh1CountRate,4);
	Acq1.fCh1ShortCountRate = 202.2F;
	Reverse((unsigned char*)&Acq1.fCh1ShortCountRate,4);
	Acq1.fCh1LongCountRate = 201.55555F;
	Reverse((unsigned char*)&Acq1.fCh1LongCountRate,4);
	Acq1.bCh2Status = 0x03;
	Acq1.fCh2CountRate = 300.0F;
	Reverse((unsigned char*)&Acq1.fCh2CountRate,4);
	Acq1.fCh2ShortCountRate = 302.2F;
	Reverse((unsigned char*)&Acq1.fCh2ShortCountRate,4);
	Acq1.fCh2LongCountRate = 301.1F;
	Reverse((unsigned char*)&Acq1.fCh2LongCountRate,4);
	Acq1.bCh3Status = 0x04;
	Acq1.fCh3CountRate = 400.0F;
	Reverse((unsigned char*)&Acq1.fCh3CountRate,4);
	Acq1.fCh3ShortCountRate = 402.2F;
	Reverse((unsigned char*)&Acq1.fCh3ShortCountRate,4);
	Acq1.fCh3LongCountRate = 401.1F;
	Reverse((unsigned char*)&Acq1.fCh3LongCountRate,4);
	Acq1.uiCountTime10ths = 104;
	Reverse((unsigned char*)&Acq1.uiCountTime10ths,4);

	Acq1.fCPUTemp = 39.7F;
	Reverse((unsigned char*)&Acq1.fCPUTemp,4);

	Acq1.fBOARDTemp = -47.6F;
	Reverse((unsigned char*)&Acq1.fBOARDTemp,4);

	OnBinaryRADAcquireRec((BYTE*)&Acq1);
*/
/*
	//test ESB TEXT acquire record
	//12 + 8 + 3*7 + 6 + 8 + 2 = 57
	char szRecord[256];
	strcpy(szRecord,"59");
	strncpy(&szRecord[  2],&Record[2],10);
	szRecord[12] = '5';
	strcpy(&szRecord[ 13],"11223344"
		"111"
		"043"
		"FFF"
		"fff"
		"888"
		"777"
		"666"
		"+023.4"
		"-432.1"
		"000010.0"
		"123456789abc");

//	char szTemp[16];
	unsigned short sum = 0;
	for (unsigned int i=0; i<strlen(szRecord); i++)
		sum = (unsigned short)(sum + szRecord[i]);
	sum &= 0x00ff;	
	sprintf(szTemp,"%02x",sum);
	strcat(szRecord,szTemp);
	strcat(szRecord,"\r\n");

	OnESBAcquireRecord(szRecord);
*/
/*
	char szTemp[32];

	//text ESB binary acquire record 2
	ESBAcquireRecord Acq2;

	Acq2.bLength = sizeof(ESBAcquireRecord);
	Acq2.bOpcode = 0x59;
	
	strncpy(szTemp,&Record[2],10);
	sscanf(szTemp,"%u",&Acq2.uiJulianSeconds);
	Reverse((BYTE*)&Acq2.uiJulianSeconds,4);

	Acq2.bJulianSec10ths = 0x06;
		
	Acq2.bStatusByte0 = 0xff;
	Acq2.bStatusByte1 = 0x88;
	Acq2.bStatusByte2 = 0x44;
	Acq2.bStatusByte3 = 0x22;

	Acq2.usAccelerometer = 257;
	Reverse((BYTE*)&Acq2.usAccelerometer,2);

	Acq2.ubHumidity = 50;

	Acq2.usLightInt = 258;
	Reverse((BYTE*)&Acq2.usLightInt,2);

	Acq2.usMagnetometerX = 0xfff;
	Reverse((BYTE*)&Acq2.usMagnetometerX,2);

	Acq2.usMagnetometerY = 260;
	Reverse((BYTE*)&Acq2.usMagnetometerY,2);

	Acq2.usMagnetometerZ = 261;
	Reverse((BYTE*)&Acq2.usMagnetometerZ,2);

	Acq2.usSurfWetness = 262;
	Reverse((BYTE*)&Acq2.usSurfWetness,2);

	Acq2.fCPUTemp = -32.0F;
	Reverse((BYTE*)&Acq2.fCPUTemp,4);

	Acq2.fBOARDTemp = 32.0F;
	Reverse((BYTE*)&Acq2.fBOARDTemp,4);

	Acq2.uiLastRecIntervTime10ths = 105;
	Reverse((BYTE*)&Acq2.uiLastRecIntervTime10ths,4);

	Acq2.bESBSerialNumber[0] = 0x00;
	Acq2.bESBSerialNumber[1] = 0x11;
	Acq2.bESBSerialNumber[2] = 0x22;
	Acq2.bESBSerialNumber[3] = 0x33;
	Acq2.bESBSerialNumber[4] = 0x44;
	Acq2.bESBSerialNumber[5] = 0x55;

	OnBinaryESBAcquireRec((BYTE*)&Acq2);
*/
/*
	//test uGRAND TEXT Configuration Record
	//2 + 10 + 10 + 2 + 4 + 2 + 8 + 2 + 2 + 2 + 2 + 2 + 5 + 1 + 4 + 6 + 2 + 2 + 
	//		8 + 2 + 1 + 10 + 10 + 10 + 10 + 10 + 10 + 10 + 10 + 2 + 2
	char szRecord[256];
	strcpy(szRecord,"50");
	strncpy(&szRecord[2],&Record[2],10);
	strcpy(&szRecord[12],"       100"
		"12"
		"5.12"
		"ff"
		"01234567"
		"03"
		"70"
		"50"
		"30"
		"20"
		"00000"
		"3"
		"3030"
		"063030"
		" 5"
		" 2"
		"12345678"
		"55"
		"f"
		"      1.00"
		"     20.00"
		"    300.00"
		"   4000.00"
		"  50000.00"
		" 600000.00"
		"7000000.00"
		"9999999999");

	char szTemp[16];
	unsigned short sum = 0;
	for (unsigned int i=0; i<strlen(szRecord); i++)
		sum = (unsigned short)(sum + szRecord[i]);
	sum &= 0x00ff;	
	sprintf(szTemp,"%02x",sum);
	strcat(szRecord,szTemp);
	strcat(szRecord,"\r\n");
	OnuGConfigRecord(szRecord);
*/
/*
	//test uGRAND BINARY Configuration Record
	char szTemp[16];

	uGConfigRecord uGConfigRec;
	uGConfigRec.bLength = sizeof(uGConfigRecord);
	uGConfigRec.bOpcode = UGCONFIGRECORD_OPCODE;

	strncpy(szTemp,&Record[2],10);
	szTemp[10] = NULL;
	sscanf(szTemp,"%u",&uGConfigRec.uiJulianSeconds);
	Reverse((BYTE*)&uGConfigRec.uiJulianSeconds,4);

	uGConfigRec.uiAcqInterval10ths = 100;
	Reverse((BYTE*)&uGConfigRec.uiAcqInterval10ths,4);

	uGConfigRec.bBaudFrame = 0x12;

	uGConfigRec.fPHDVSetPoint = 5.2F;
	Reverse((BYTE*)&uGConfigRec.fPHDVSetPoint,4);

	uGConfigRec.bCPUSpeed = 0xff;

	uGConfigRec.uiModeFlags = 0x01234567;

	uGConfigRec.bFilterControl = 0x03;

	uGConfigRec.bImmBuffSize = 70;

	uGConfigRec.bImmBuffSaveSize = 50;

	uGConfigRec.bLocalBGSize = 30;

	uGConfigRec.bLocalBGEnd = 20;

	uGConfigRec.usStatusRecInterv = 0;
	Reverse((BYTE*)&uGConfigRec.usStatusRecInterv,2);

	uGConfigRec.bTimeSyncState = 0x03;

	uGConfigRec.bHourlyTimeSyncMM = 30;
	uGConfigRec.bHourlyTimeSyncSS = 30;

	uGConfigRec.bDailyTimeSyncHH = 6;
	uGConfigRec.bDailyTimeSyncMM = 30;
	uGConfigRec.bDailyTimeSyncSS = 30;

	uGConfigRec.bTimeSyncLowerTol = 5;
	uGConfigRec.bTimeSyncUpperTol = 2;

	uGConfigRec.uiInspectorID = 12345678;
	Reverse((BYTE*)&uGConfigRec.uiInspectorID,4);

	uGConfigRec.bUnitID = 0x55;

	uGConfigRec.bImpedanceSet = 0x0f;

	uGConfigRec.fCh0KalmanR = 1.0F;
	Reverse((BYTE*)&uGConfigRec.fCh0KalmanR,4);

	uGConfigRec.fCh1KalmanR = 20.0F;
	Reverse((BYTE*)&uGConfigRec.fCh1KalmanR,4);

	uGConfigRec.fCh2KalmanR = 300.0F;
	Reverse((BYTE*)&uGConfigRec.fCh2KalmanR,4);

	uGConfigRec.fCh3KalmanR = 4000.0F;
	Reverse((BYTE*)&uGConfigRec.fCh3KalmanR,4);

	uGConfigRec.fCh0KalmanQ = 50000.0F;
	Reverse((BYTE*)&uGConfigRec.fCh0KalmanQ,4);

	uGConfigRec.fCh1KalmanQ = 600000.0F;
	Reverse((BYTE*)&uGConfigRec.fCh1KalmanQ,4);

	uGConfigRec.fCh2KalmanQ = 7000000.0F;
	Reverse((BYTE*)&uGConfigRec.fCh2KalmanQ,4);

	uGConfigRec.fCh3KalmanQ = 3.402823466e+38F;
	Reverse((BYTE*)&uGConfigRec.fCh3KalmanQ,4);

	OnBinaryuGConfigRecord((BYTE*)&uGConfigRec);
*/

/*

	//test uG BINARY Status Record
	char szTemp[16];
	uGStatusRecord uGStatusRec;
	uGStatusRec.bLength = sizeof(uGStatusRecord);
	uGStatusRec.bOpcode = UGSTATUSRECORD_OPCODE;

	strncpy(szTemp,&Record[2],10);
	szTemp[10] = NULL;
	sscanf(szTemp,"%u",&uGStatusRec.uiJulianSeconds);
	Reverse((BYTE*)&uGStatusRec.uiJulianSeconds,sizeof(uGStatusRec.uiJulianSeconds));

	uGStatusRec.bJulianSec10ths = 5;

	uGStatusRec.f50VSupply = 5.02F;
	Reverse((BYTE*)&uGStatusRec.f50VSupply,sizeof(uGStatusRec.f50VSupply));

	uGStatusRec.f33VSupply = 3.35F;
	Reverse((BYTE*)&uGStatusRec.f33VSupply,sizeof(uGStatusRec.f33VSupply));

	uGStatusRec.f25VSupply = 2.55F;
	Reverse((BYTE*)&uGStatusRec.f25VSupply,sizeof(uGStatusRec.f25VSupply));
	
	uGStatusRec.fPHDVoltage = 1.22F;
	Reverse((BYTE*)&uGStatusRec.fPHDVoltage,sizeof(uGStatusRec.fPHDVoltage));

	uGStatusRec.bStatus1Byte = 0xc7;

	uGStatusRec.bStatus2Byte = 0x00;

	uGStatusRec.bStatus3Byte = 0x01;

	uGStatusRec.bStatus4Byte = 0x02;

	uGStatusRec.uiCPUUpTime = 0xffffffff;
	Reverse((BYTE*)&uGStatusRec.uiCPUUpTime,sizeof(uGStatusRec.uiCPUUpTime));

	uGStatusRec.fCPUTemp = -999.9F;
	Reverse((BYTE*)&uGStatusRec.fCPUTemp,sizeof(uGStatusRec.fCPUTemp));

	uGStatusRec.fBOARDTemp = +999.9F;
	Reverse((BYTE*)&uGStatusRec.fBOARDTemp,sizeof(uGStatusRec.fBOARDTemp));

	uGStatusRec.uiReserved = 0x12345678;
	Reverse((BYTE*)&uGStatusRec.uiReserved,sizeof(uGStatusRec.uiReserved));

	OnBinaryuGStatusRecord((BYTE*)&uGStatusRec);
*/
/*
	//test uG TEXT Status Record
	char szRecord[256];
	strcpy(szRecord,"55");
	strncpy(&szRecord[2],&Record[2],10);
	szRecord[12] = '5';
	strcpy(&szRecord[13],
		" 5.0"
		" 3.3"
		" 2.6"
		" 1.2"
		"c7"
		"00"
		"01"
		"02"
		"1234567890"
		"+999.9"
		"-999.9"
		"01020304");

	char szTemp[16];
	unsigned short sum = 0;
	for (unsigned int i=0; i<strlen(szRecord); i++)
		sum = (unsigned short)(sum + szRecord[i]);
	sum &= 0x00ff;	
	sprintf(szTemp,"%02x",sum);
	strcat(szRecord,szTemp);
	strcat(szRecord,"\r\n");
	OnuGStatusRecord(szRecord);
*/
/*
	//test uG TEXT Info Record
	char szRecord[256];
	strcpy(szRecord,"5a");
	strncpy(&szRecord[2],&Record[2],10);
	strcpy(&szRecord[12],
		"a1b2"
		"01.02.03"
		"ffff"
		"010203040506");
	char szTemp[16];
	unsigned short sum = 0;
	for (unsigned int i=0; i<strlen(szRecord); i++)
		sum = (unsigned short)(sum + szRecord[i]);
	sum &= 0x00ff;	
	sprintf(szTemp,"%02x",sum);
	strcat(szRecord,szTemp);
	strcat(szRecord,"\r\n");
	OnuGInfoRecord(szRecord);
*/
/*
	//test ESB TEXT Info Record
	char szRecord[256];
	strcpy(szRecord,"5b");
	strncpy(&szRecord[2],&Record[2],10);
	strcpy(&szRecord[12],
		"01.02.03"
		"ffff"
		"010203040506");
	char szTemp[16];
	unsigned short sum = 0;
	for (unsigned int i=0; i<strlen(szRecord); i++)
		sum = (unsigned short)(sum + szRecord[i]);
	sum &= 0x00ff;	
	sprintf(szTemp,"%02x",sum);
	strcat(szRecord,szTemp);
	strcat(szRecord,"\r\n");
	OnESBInfoRecord(szRecord);
*/
/*
	//test PSB TEXT Info Record
	char szRecord[256];
	strcpy(szRecord,"5c");
	strncpy(&szRecord[2],&Record[2],10);
	strcpy(&szRecord[12],
		"01.02.03"
		"ffff"
		"010203040506");
	char szTemp[16];
	unsigned short sum = 0;
	for (unsigned int i=0; i<strlen(szRecord); i++)
		sum = (unsigned short)(sum + szRecord[i]);
	sum &= 0x00ff;	
	sprintf(szTemp,"%02x",sum);
	strcat(szRecord,szTemp);
	strcat(szRecord,"\r\n");
	OnPSBInfoRecord(szRecord);

*/
/*
	//test BMB TEXT Info Record
//	char szRecord[256];
	static unsigned short isusTemp = 0;

	szRecord[0] = NULL;
	strcpy(szRecord,"5d");
	strncpy(&szRecord[2],&Record[2],10);
	strcpy(&szRecord[12],
		"01.02.03"
		"ffff"
		"0102030405");
	//	char szTemp[16];
	szTemp[0] = NULL;
	sprintf(szTemp,"%02x",isusTemp++);
	isusTemp &= 0x000f;
	strcat(szRecord,szTemp);
	szTemp[0] = NULL;

//	unsigned short sum = 0;
	sum = 0;
//	for (unsigned int i=0; i<strlen(szRecord); i++)
//		sum = (unsigned short)(sum + szRecord[i]);
	for (i=0; i<strlen(szRecord); i++)
		sum = (unsigned short)(sum + szRecord[i]);
	sum &= 0x00ff;	
	sprintf(szTemp,"%02x",sum);
	strcat(szRecord,szTemp);
	strcat(szRecord,"\r\n");
	OnBMBInfoRecord(szRecord);
*/
/*

	//old acquire record code
	if (m_pPropertySheet)
		m_pPropertySheet->AcquirRec(Record);

	int CheckSum;
	if (LengthOk(Record,ACQUIRE_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		m_cAcquireRec1[0] = m_cAcquireRec1[1];
		m_cAcquireRec1[1] = m_cAcquireRec1[2];
		m_cAcquireRec1[2] = m_cAcquireRec1[3];
		m_cAcquireRec1[3] = Record;

		char temp[32];

		int AcquireRecordTarget = m_dAcquireRecordCountX;
 		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		AcquireTime[AcquireRecordTarget].Init(temp);
		
		//save time in binary to acquire record
		AcquireRec[AcquireRecordTarget].JulianSeconds = 
			AcquireTime[AcquireRecordTarget].TotalSeconds();

		//get status byte 
		short Status;
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status);

		//save status in binary to acquire record
		AcquireRec[AcquireRecordTarget].StatusByte = Status;
		//move all the date displays down one
		SetString(DATA2PAGE,IDC_DATA2_DATADATE4,
			GetString(DATA2PAGE,IDC_DATA2_DATADATE3));
		SetString(DATA2PAGE,IDC_DATA2_DATADATE3,
			GetString(DATA2PAGE,IDC_DATA2_DATADATE2));
		SetString(DATA2PAGE,IDC_DATA2_DATADATE2,
			GetString(DATA2PAGE,IDC_DATA2_DATADATE1));
		SetString(DATA2PAGE,IDC_DATA2_DATADATE1,AcquireTime[AcquireRecordTarget].Date());
	
		//move all the time displays down one
		SetString(DATA2PAGE,IDC_DATA2_DATATIME4,
			GetString(DATA2PAGE,IDC_DATA2_DATATIME3));
		SetString(DATA2PAGE,IDC_DATA2_DATATIME3,
			GetString(DATA2PAGE,IDC_DATA2_DATATIME2));
		SetString(DATA2PAGE,IDC_DATA2_DATATIME2,
			GetString(DATA2PAGE,IDC_DATA2_DATATIME1));
		SetString(DATA2PAGE,IDC_DATA2_DATATIME1,AcquireTime[AcquireRecordTarget].Time());
	
		//move all the Neutron A count rate displays down one
//		SetString(DATAPAGE,IDC_DATA_PULSEACNT4,
//			GetString(DATAPAGE,IDC_DATA_PULSEACNT3));
//		SetString(DATAPAGE,IDC_DATA_PULSEACNT3,
//			GetString(DATAPAGE,IDC_DATA_PULSEACNT2));
//		SetString(DATAPAGE,IDC_DATA_PULSEACNT2,
//			GetString(DATAPAGE,IDC_DATA_PULSEACNT1));
		strncpy(temp,&Record[14],8);
		temp[8]=NULL;
//		SetString(DATAPAGE,IDC_DATA_PULSEACNT1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].NeutronChA = (float)atof(temp);
	
		//move all the Neutron B count rate displays down one
//		SetString(DATAPAGE,IDC_DATA_PULSEBCNT4,
//			GetString(DATAPAGE,IDC_DATA_PULSEBCNT3));
//		SetString(DATAPAGE,IDC_DATA_PULSEBCNT3,
//			GetString(DATAPAGE,IDC_DATA_PULSEBCNT2));
//		SetString(DATAPAGE,IDC_DATA_PULSEBCNT2,
//			GetString(DATAPAGE,IDC_DATA_PULSEBCNT1));
		strncpy(temp,&Record[22],8);
		temp[8]=NULL;
//		SetString(DATAPAGE,IDC_DATA_PULSEBCNT1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].NeutronChB = (float)atof(temp);
	
		//move all the Neutron C count rate displays down one
//		SetString(DATAPAGE,IDC_DATA_PULSECCNT4,
//			GetString(DATAPAGE,IDC_DATA_PULSECCNT3));
//		SetString(DATAPAGE,IDC_DATA_PULSECCNT3,
//			GetString(DATAPAGE,IDC_DATA_PULSECCNT2));
//		SetString(DATAPAGE,IDC_DATA_PULSECCNT2,
//			GetString(DATAPAGE,IDC_DATA_PULSECCNT1));
		strncpy(temp,&Record[30],8);
		temp[8]=NULL;
//		SetString(DATAPAGE,IDC_DATA_PULSECCNT1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].NeutronChC = (float)atof(temp);
	
		//move all the gross gammas 1 displays down one
//		SetString(DATAPAGE,IDC_DATA_GAMMA1GR4,
//			GetString(DATAPAGE,IDC_DATA_GAMMA1GR3));
//		SetString(DATAPAGE,IDC_DATA_GAMMA1GR3,
//			GetString(DATAPAGE,IDC_DATA_GAMMA1GR2));
//		SetString(DATAPAGE,IDC_DATA_GAMMA1GR2,
//			GetString(DATAPAGE,IDC_DATA_GAMMA1GR1));
		strncpy(temp,&Record[38],8);
		temp[8]=NULL;
//		SetString(DATAPAGE,IDC_DATA_GAMMA1GR1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].GammaCh1 = (float)atof(temp);
	
		//move all the gamma 1 sigma displays down one
//		SetString(DATAPAGE,IDC_DATA_GAMMA1S4,
//			GetString(DATAPAGE,IDC_DATA_GAMMA1S3));
//		SetString(DATAPAGE,IDC_DATA_GAMMA1S3,
//			GetString(DATAPAGE,IDC_DATA_GAMMA1S2));
//		SetString(DATAPAGE,IDC_DATA_GAMMA1S2,
//			GetString(DATAPAGE,IDC_DATA_GAMMA1S1));
		strncpy(temp,&Record[46],7);
		temp[7]=NULL;
//		SetString(DATAPAGE,IDC_DATA_GAMMA1S1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].GammaCh1Sigma = (float)atof(temp);
	
		//move all the gross gamma 2 displays down one
//		SetString(DATAPAGE,IDC_DATA_GAMMA2GR4,
//			GetString(DATAPAGE,IDC_DATA_GAMMA2GR3));
//		SetString(DATAPAGE,IDC_DATA_GAMMA2GR3,
//			GetString(DATAPAGE,IDC_DATA_GAMMA2GR2));
//		SetString(DATAPAGE,IDC_DATA_GAMMA2GR2,
//			GetString(DATAPAGE,IDC_DATA_GAMMA2GR1));
		strncpy(temp,&Record[53],8);
		temp[8]=NULL;
//		SetString(DATAPAGE,IDC_DATA_GAMMA2GR1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].GammaCh2 = (float)atof(temp);
	
		//move all the sigma 2 displays down one
//		SetString(DATAPAGE,IDC_DATA_GAMMA2S4,
//			GetString(DATAPAGE,IDC_DATA_GAMMA2S3));
//		SetString(DATAPAGE,IDC_DATA_GAMMA2S3,
//			GetString(DATAPAGE,IDC_DATA_GAMMA2S2));
//		SetString(DATAPAGE,IDC_DATA_GAMMA2S2,
//			GetString(DATAPAGE,IDC_DATA_GAMMA2S1));
		strncpy(temp,&Record[61],7);
		temp[7]=NULL;
//		SetString(DATAPAGE,IDC_DATA_GAMMA2S1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].GammaCh2Sigma = (float)atof(temp);
	
		//move all the time data acquired displays down one
//		SetString(DATAPAGE,IDC_DATA_LENGTHT4,
//			GetString(DATAPAGE,IDC_DATA_LENGTHT3));
//		SetString(DATAPAGE,IDC_DATA_LENGTHT3,
//			GetString(DATAPAGE,IDC_DATA_LENGTHT2));
//		SetString(DATAPAGE,IDC_DATA_LENGTHT2,
//			GetString(DATAPAGE,IDC_DATA_LENGTHT1));
		strncpy(temp,&Record[68],5);
		temp[5]=NULL;
//		SetString(DATAPAGE,IDC_DATA_LENGTHT1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].CountTime = (short)atoi(temp);

		//get the checksum byte
		strncpy(temp,&Record[73],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&AcquireCS[AcquireRecordTarget]);

		//get the authentication byte
//		strncpy(temp,&Record[75],2);
//		temp[2]=NULL;
//		sscanf(temp,"%2hx",&AcquireAUTH[AcquireRecordTarget]);
//
//		AcquireAUTHIgnore[AcquireRecordTarget] = bIgnoreAuth;

		//if not in dumplast sequence then add this record to BID file
		if (m_bInDumpLast)
		{
			m_dPreviousBIDTime = AcquireRec[AcquireRecordTarget].JulianSeconds;
		}
		else
		{
			m_dAcquireRecordCountX++;
			if (m_dAcquireRecordCountX >= NUMBEROFACQUIRERECS) m_dAcquireRecordCountX = NUMBEROFACQUIRERECS-1;
		}
		m_bFirst15Rec = false;

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,ACQUIRE_RECORD,CheckSum);
*/
}
/*
//record 16
void CInstrument::DumpEnd(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,END_DUMP_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	m_dNumberOfAcquires = -1;

	if (m_pPropertySheet)
		m_pPropertySheet->DumpEnd(Record);

	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,END_DUMP_RECORD,CheckSum);
}
*/
//record 61
void CInstrument::DumpEnd2(char *Record)
{
	if (m_pPropertySheet)
		m_pPropertySheet->DumpEnd2(Record);

	int CheckSum = CHECKSUM_BAD;
	if (strlen(Record) == END_DUMP2_RECORD_LENGTH+1)
	{
		if (LengthOk(Record,END_DUMP2_RECORD_LENGTH+1))
			CheckSum = CHECKSUM_GOOD;
		char temp[3];
		if (CheckSum == CHECKSUM_GOOD)
		{
			temp[0] = Record[2];
			temp[1] = Record[3];
			temp[2] = NULL;
			sscanf(temp,"%d",&m_dNumberOfAcquires);
		}
	}
	else
	{
		if (LengthOk(Record,END_DUMP2_RECORD_LENGTH))
			CheckSum = Checksum(Record);
		else
			CheckSum = CHECKSUM_BAD;
		char temp[2];
		if (CheckSum == CHECKSUM_GOOD)
		{
			temp[0] = Record[2];
			temp[1] = NULL;
			sscanf(temp,"%x",&m_dNumberOfAcquires);
		}
		else
			m_dNumberOfAcquires = -1;
	}
//if we are using the 15 records to simulate something
//other than 58 records then set NumberOfAcquires to 
//m_dNumberOfAcquires = 0;
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,END_DUMP2_RECORD,CheckSum);
}


//record 17
void CInstrument::DumpOk(char *Record)
{
	//don't know if pre or post 4.1 so don't:
	//AdjustTabsTo(PRE4);

	if (m_pPropertySheet)
		m_pPropertySheet->DumpOk(Record);

	int CheckSum;
	if (LengthOk(Record,DUMP_OK_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		char temp[8];

		//get bbm bytes
		int bbm;
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		char * pCh = temp;
		while (*pCh == ' ')pCh++;

		bbm = atoi(temp);
		m_pButton->put_Numeric((ULONG)bbm);
		SetString(DATAIPAGE,IDC_DATAI_BYTESMEM,pCh);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

		//check checksum
			//send message to instrument if bad
		//send message to parent saying we got this record
		//use second parameter as flag for good or bad checksum
		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,bbm);
	}
	PostMessage(IDC_RECEIVE_RESULT,DUMP_OK_RECORD,CheckSum);
}

bool CInstrument::CurrentSetParam(UINT uiIniName, CString cIniName,CString cValue,bool bDoMatch)
{
	return CurrentSetParam(uiIniName, (LPSTR)LPCSTR(cIniName),(LPSTR)LPCSTR(cValue),bDoMatch);
}

bool CInstrument::CurrentSetParam(UINT uiIniName, char*szIniName,char*szValue,bool bDoMatch)
{
	bool bReturn = true;
	if (uiIniName >= SET_BASE && uiIniName <= SET_TOP)
	{
		//SetupStrings is a CString so assignment creates a new storage
		SetupStrings[uiIniName-SET_BASE] = szValue;
	}

	char szIniValue[128];
	GetPrivateProfileString(m_pName,szIniName,"",szIniValue,sizeof(szIniValue),m_szIniFile);
	if (uiIniName >= SET_BASE && uiIniName <= SET_TOP)
	{
		SetupINIStrings[uiIniName-SET_BASE] = szIniValue;
	}

	//send true saying we have a match
//	if (m_pPropertySheet)
//		m_pPropertySheet->CurrentSetParam(szIniName,szValue,bDoMatch,true);

	m_cSetParamMatchText = szIniValue;
	if (bDoMatch)
	{
		if (strcmp(szIniValue,szValue)!=0)
		{
			int iInstSetupTab;
			bReturn = false;
			if ((uiIniName >= SET_BASE) && (uiIniName <= SET_TOP))
			{
				m_pButton->ShowIcon_Square(VARIANT_TRUE);
				iInstSetupTab = m_iInst4Setup_Tab;
				m_iInst4Setup_Tab |= 0x0001;
				Setup4StaticColors[uiIniName-SET_BASE] = COLOR_RED;
				if (m_pPropertySheet && iInstSetupTab != m_iInst4Setup_Tab)
				{
					m_pPropertySheet->SetTabColor(IDC_INST4SETUP_TAB, COLOR_RED);
					m_pPropertySheet->Invalidate(false);
				}
			}
		}
	}
	return bReturn;
}
/*
//record 1a
int CInstrument::BinaryDualIonRecOld(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		float bias;				//4
		UCHAR gain_mode[2];		//2
		BYTE max_gain[2];		//2	
		BYTE gain[2];			//2
		UCHAR dis_mode;			//1
	} *psBinaryRecord;			//17 total 
#pragma pack()

	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->bias),4);

	char Record[1024];
	
	sprintf(Record,"1a%10u00%6.1f%c%c%7d%7d%7d%7d%c",
		psBinaryRecord->julian,
		psBinaryRecord->bias,
		(psBinaryRecord->gain_mode[0]==0)?'A':'M',
		(psBinaryRecord->gain_mode[1]==0)?'A':'M',
		1<<(psBinaryRecord->max_gain[0]*2),
		1<<(psBinaryRecord->max_gain[1]*2),
		1<<(psBinaryRecord->gain[0]*2),
		1<<(psBinaryRecord->gain[1]*2),
		(psBinaryRecord->dis_mode==0)?'I':'R'
		);

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");	
	PostToWatch(Record);
	DualIonRec(Record, true);

	return sizeof(SBinaryRec);
}
*/
/*
//record 1a
int CInstrument::BinaryDualIonRec(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;				//1
		UCHAR type;					//1
		unsigned int uiJulian;		//4
		UCHAR ucBoardID;			//1
		float fBias;				//4
		UCHAR ucGainMode[2];		//2
		unsigned int uiMaxGain[2];	//8
		unsigned int uiGain[2];		//8
		UCHAR ucDisconnectMode;		//1
	} *psBinaryRecord;				//30 total
#pragma pack()

	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->uiJulian),4);
	Reverse((BYTE*)&(psBinaryRecord->fBias),4);
	Reverse((BYTE*)&(psBinaryRecord->uiMaxGain[0]),4);
	Reverse((BYTE*)&(psBinaryRecord->uiMaxGain[1]),4);
	Reverse((BYTE*)&(psBinaryRecord->uiGain[0]),4);
	Reverse((BYTE*)&(psBinaryRecord->uiGain[1]),4);

	char Record[1024];
	
//	sprintf(Record,"1a%10u00%6.1f%c%c%7d%7d%7d%7d%c",
//		psBinaryRecord->julian,
//		psBinaryRecord->bias,
//		(psBinaryRecord->gain_mode[0]==0)?'A':'M',
//		(psBinaryRecord->gain_mode[1]==0)?'A':'M',
//		1<<(psBinaryRecord->max_gain[0]*2),
//		1<<(psBinaryRecord->max_gain[1]*2),
//		1<<(psBinaryRecord->gain[0]*2),
//		1<<(psBinaryRecord->gain[1]*2),
//		(psBinaryRecord->dis_mode==0)?'I':'R'
//		);
	
	sprintf(Record,"1a%10u%02x%6.1f%c%c%7d%7d%7d%7d%c",
		psBinaryRecord->uiJulian,		//%10u
		psBinaryRecord->ucBoardID,		//%02x
		psBinaryRecord->fBias,			//%6.1f
		psBinaryRecord->ucGainMode[0],	//%c
		psBinaryRecord->ucGainMode[1],	//%c
		psBinaryRecord->uiMaxGain[0],	//%7d
		psBinaryRecord->uiMaxGain[1],	//%7d
		psBinaryRecord->uiGain[0],		//%7d
		psBinaryRecord->uiGain[1],		//%7d
		psBinaryRecord->ucDisconnectMode//%c
		);

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");	
	PostToWatch(Record);
	DualIonRec(Record, false);
	
	return sizeof(SBinaryRec);
}
*/
/*
//record 1a
void CInstrument::DualIonRec(char *Record, bool bBinarySource)
{

	int CheckSum;
	if (LengthOk(Record,DUAL_ION_STAT_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(PRE4);
		m_cDualIonRec = Record;

		char temp[32];
		char temp2[32];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//set both dates and times
		strcpy(temp,Julian.Date());
		SetString(GENERALPAGE,IDC_GENERAL_G1DATE,temp);
		SetString(GENERALPAGE,IDC_GENERAL_G2DATE,temp);

		strcpy(temp,Julian.Time());
		SetString(GENERALPAGE,IDC_GENERAL_G1TIME,temp);
		SetString(GENERALPAGE,IDC_GENERAL_G2TIME,temp);

		//board id 00 
		//"00" if from binary message
		if (!bBinarySource)
		{
			//handle board ID
		}

		//Ion Chamber Bias
		float Current;
		strncpy(temp,&Record[14],6);
		temp[6] = NULL;
		Current = (float)atof(temp);
		if (m_dDIRFirst)
		{
			m_fDIRHigh = m_fDIRLow = Current;
			m_dDIRFirst = false;
		}
		if (Current < m_fDIRLow) m_fDIRLow = Current;
		if (Current > m_fDIRHigh) m_fDIRHigh = Current;
		sprintf(temp,"%6.1f",m_fDIRLow);
		SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMIN,temp);

		sprintf(temp,"%6.1f",Current);
		SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVCUR,temp);

		sprintf(temp,"%6.1f",m_fDIRHigh);
		SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMAX,temp);

		bool bMatch;
		//gain mode ch 1
		SetString(GENERALPAGE,IDC_GENERAL_GAMMA1M,(Record[20]=='A')?"Automatic":"Manual");
		temp2[0] = NULL;
		temp[0] = Record[20];
		temp[1] = NULL;
		GetPrivateProfileString(m_pName,"SET_GAMMA1_MODE","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (Record[20] != temp2[0])
		{
			SetColor(GENERALPAGE,IDC_GENERAL_GAMMA1M,COLOR_YELLOW);
			WriteToFileEx(51,TO_CEV|TO_PFM,TYPE_INST, &Julian, "00000 CONFIGURATION MISMATCH - GAMMA 1 MODE (1a)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_GAMMA1_MODE,"SET_GAMMA1_MODE",temp,bMatch);

		//gain mode ch 2
		SetString(GENERALPAGE,IDC_GENERAL_GAMMA2M,(Record[21]=='A')?"Automatic":"Manual");
		temp2[0] = NULL;
		temp[0] = Record[21];
		temp[1] = NULL;
		GetPrivateProfileString(m_pName,"SET_GAMMA2_MODE","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (Record[21] != temp2[0])
		{
			SetColor(GENERALPAGE,IDC_GENERAL_GAMMA2M,COLOR_YELLOW);
			WriteToFileEx(52,TO_CEV|TO_PFM,TYPE_INST, &Julian, "00000 CONFIGURATION MISMATCH - GAMMA 2 MODE (1a)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_GAMMA2_MODE,"SET_GAMMA2_MODE",temp,bMatch);
		
		//max gain0
		strncpy(temp,&Record[22],7);
		temp[7] = NULL;
		SetString(GENERALPAGE,IDC_GENERAL_MAXG1,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pName,"SET_MAX_GAIN0","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_MAXG1,COLOR_YELLOW);
			WriteToFileEx(53,TO_CEV|TO_PFM,TYPE_INST, &Julian, "00000 CONFIGURATION MISMATCH - GAMMA MAX GAIN0 (1a)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_MAX_GAIN0,"SET_MAX_GAIN0",temp,bMatch);
		//max gain1
		strncpy(temp,&Record[29],7);
		temp[7] = NULL;
		SetString(GENERALPAGE,IDC_GENERAL_MAXG2,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pName,"SET_MAX_GAIN1","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_MAXG2,COLOR_YELLOW);
			WriteToFileEx(54,TO_CEV|TO_PFM,TYPE_INST, &Julian, "00000 CONFIGURATION MISMATCH - GAMMA MAX GAIN1 (1a)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_MAX_GAIN1,"SET_MAX_GAIN1",temp,bMatch);
	
		//gain
		strncpy(temp,&Record[36],7);
		temp[7] = NULL;
		SetString(GENERALPAGE,IDC_GENERAL_GAMMA1G,temp);
		strncpy(temp,&Record[43],7);
		temp[7] = NULL;
		SetString(GENERALPAGE,IDC_GENERAL_GAMMA2G,temp);
	
		//disconnect mode
		strncpy(temp,&Record[50],1);
		temp[1] = NULL;
		SetString(GENERALPAGE,IDC_GENERAL_DISCM,(Record[50]=='I')?"Internal":"Remote");
		temp2[0] = NULL;
		GetPrivateProfileString(m_pName,"SET_DISCONNECT_MODE","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_DISCM,COLOR_YELLOW);
			WriteToFileEx(55,TO_CEV|TO_PFM,TYPE_INST, &Julian, "00000 CONFIGURATION MISMATCH - DISCONNECT MODE (1a)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_DISCONNECT_MODE,"SET_DISCONNECT_MODE",temp,bMatch);

		if (m_pPropertySheet)
			m_pPropertySheet->DualIonRec(Record, bBinarySource);

		char cBuff[1024];
		strcpy(cBuff,"00000 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//dual ion record 1a

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,DUAL_ION_STAT_RECORD,CheckSum);
}
*/
/*
//record 1b
int CInstrument::BinaryGenStatRecOld(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR status;			//1
		float battery;			//4
		float plus5;			//4
		float plus12;			//4
		float minus12;			//4
	} *psBinaryRecord;			//23 total
#pragma pack()
	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->battery),4);
	Reverse((BYTE*)&(psBinaryRecord->plus5),4);
	Reverse((BYTE*)&(psBinaryRecord->plus12),4);
	Reverse((BYTE*)&(psBinaryRecord->minus12),4);
	char szRecord[512];

	sprintf(szRecord,"1b%10u00%02x00.0000000000000000000000%4.1f%4.2f%4.1f%5.1f0000",
		psBinaryRecord->julian,
		psBinaryRecord->status,
		psBinaryRecord->battery,
		psBinaryRecord->plus5,
		psBinaryRecord->plus12,
		psBinaryRecord->minus12
		);
	

//	sprintf(szRecord,"1b%10u%02x%02x%c%c%c%c%c%07d%02x%02x%c%05d%c%c%c%4.1f%4.1f%4.1f%5.1f%04X",
//		psBinaryRecord->uiJulian,
//		psBinaryRecord->ucUnitID,
//		psBinaryRecord->ucStatus,
//		psBinaryRecord->cVersion[0],
//		psBinaryRecord->cVersion[1],
//		psBinaryRecord->cVersion[2],
//		psBinaryRecord->cVersion[3],
//		psBinaryRecord->cVersion[4],
//		psBinaryRecord->ulInspectorID,
//		psBinaryRecord->cBoard1Type,
//		psBinaryRecord->cBoard2Type,
//		psBinaryRecord->cAcqMode,
//		(psBinaryRecord->ulBaudRate > 99999)?psBinaryRecord->ulBaudRate/10:psBinaryRecord->ulBaudRate,
//		psBinaryRecord->cFrame[0],
//		psBinaryRecord->cFrame[1],
//		psBinaryRecord->cXOn,
//		psBinaryRecord->fBattery,
//		psBinaryRecord->fPlus5,
//		psBinaryRecord->fPlus12,
//		psBinaryRecord->fMinus12,
//		psBinaryRecord->uiCodeCksum
//		);

	AppendCheckSum(szRecord);
	UpdateSummary(szRecord);
	strcat(szRecord,"\r\n");	
	PostToWatch(szRecord);
	GenStatRec(szRecord, true);

	return sizeof(SBinaryRec);
}
*/
/*
//record 1b
int CInstrument::BinaryGenStatRec(BYTE *pBinaryAt)
{
#pragma pack(1)

//	struct SBinaryRec {
//		UCHAR length;			//1
//		UCHAR type;				//1
//		unsigned int julian;	//4
//		UCHAR status;			//1
//		float battery;			//4
//		float plus5;			//4
//		float plus12;			//4
//		float minus12;			//4
//	} *psBinaryRecord;			//23 total

  struct SBinaryRec {
		UCHAR length;				//1
		UCHAR type;					//1
		unsigned int uiJulian;		//4
		UCHAR ucUnitID;				//1
		UCHAR ucStatus;				//1
		char  cVersion[5];			//5
		unsigned int ulInspectorID;	//4
		char  cBoard1Type;			//1
		char  cBoard2Type;			//1
		char  cAcqMode;				//1
		unsigned int ulBaudRate;	//4
		char  cFrame[2];			//2
		char  cXOn;					//1
		float fBattery;				//4
		float fPlus5;				//4
		float fPlus12;				//4
		float fMinus12;				//4
		unsigned short uiCodeCksum;	//2
	} *psBinaryRecord;				//45 total

	AdjustTabsTo(PRE4);

#pragma pack()
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->uiJulian),4);
	Reverse((BYTE*)&(psBinaryRecord->ulInspectorID),4);
	Reverse((BYTE*)&(psBinaryRecord->ulBaudRate),4);
	Reverse((BYTE*)&(psBinaryRecord->fBattery),4);
	Reverse((BYTE*)&(psBinaryRecord->fPlus5),4);
	Reverse((BYTE*)&(psBinaryRecord->fPlus12),4);
	Reverse((BYTE*)&(psBinaryRecord->fMinus12),4);
	Reverse((BYTE*)&(psBinaryRecord->uiCodeCksum),2);
	char szRecord[512];

	sprintf(szRecord,"1b%10u%02x%02x%c%c%c%c%c%07d%02x%02x%c%05d%c%c%c%4.1f%4.1f%4.1f%5.1f%04X",
		psBinaryRecord->uiJulian,
		psBinaryRecord->ucUnitID,
		psBinaryRecord->ucStatus,
		psBinaryRecord->cVersion[0],
		psBinaryRecord->cVersion[1],
		psBinaryRecord->cVersion[2],
		psBinaryRecord->cVersion[3],
		psBinaryRecord->cVersion[4],
		psBinaryRecord->ulInspectorID,
		psBinaryRecord->cBoard1Type,
		psBinaryRecord->cBoard2Type,
		psBinaryRecord->cAcqMode,
		(psBinaryRecord->ulBaudRate > 99999)?psBinaryRecord->ulBaudRate/10:psBinaryRecord->ulBaudRate,
		psBinaryRecord->cFrame[0],
		psBinaryRecord->cFrame[1],
		psBinaryRecord->cXOn,
		psBinaryRecord->fBattery,
		psBinaryRecord->fPlus5,
		psBinaryRecord->fPlus12,
		psBinaryRecord->fMinus12,
		psBinaryRecord->uiCodeCksum
		);

	AppendCheckSum(szRecord);
	UpdateSummary(szRecord);
	strcat(szRecord,"\r\n");	
	PostToWatch(szRecord);
	GenStatRec(szRecord, false);

	return sizeof(SBinaryRec);
}
*/
/*
//record 1b
void CInstrument::GenStatRec(char *Record, bool bBinarySource)
{

	int CheckSum;
	if (LengthOk(Record,GEN_STATUS_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(PRE4);

		m_cGenStatRec = Record;

		bool CurrentRed = false;
		bool CurrentLow = false;
		bool CurrentHigh= false;
		
		char temp[64];
		char temp2[64];
		//get julian time and date
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		SetString(GENERALPAGE,IDC_GENERAL_TIME,temp);

		bool bMatch;
	
		//main unit id
		//in HEX on a UGRANDIII in dec on a MiniUGRAND
		if (!bBinarySource)
		{
			strncpy(temp,&Record[12],2);
			temp[2] = NULL;
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				int iTemp;
				sscanf(temp,"%x",&iTemp);
				sprintf(temp,"%02d",iTemp);
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(DATAPAGE,IDC_DATA_UNITID,COLOR_YELLOW);
					WriteToFileEx(56,TO_CEV|TO_PFM,TYPE_INST, &Julian, "00000 CONFIGURATION MISMATCH - UNIT ID(1b)\r\n");
					m_iData_Tab |= 0x0002;
					bMatch = false;
				}
			}
			SetString(DATAPAGE,IDC_DATA_UNITID,temp);
			CurrentSetParam(SET_UNIT_ID,"SET_UNIT_ID",temp,bMatch);
		}
	
		//status
		strncpy(temp,&Record[14],2);
		temp[2] = NULL;
		short Status;
		sscanf(temp,"%2hx",&Status);

		if (Status & ACPOWER)
		{
			strcpy(temp,FAIL);
			SetColor(DATAPAGE,IDC_DATA_ACPOWER,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
			m_iData_Tab |= 0x0001;
		}
		else 
			strcpy(temp,OK);

		//ac power changed since last status record
		if (Status & ACPOWERCH)
		{
			strcat(temp," with loss since last status record");
	
			if (!m_bACPowerLoss)
			{
				WriteToFileEx(57,TO_CEV | TO_PFM, TYPE_INST, &Julian,"00000 STATUS CHANGE - EXTERNAL POWER LOSS Since last status record (1b)\r\n");
				m_bACPowerLoss = true;
			}
		}
		else
			m_bACPowerLoss = false;

		SetString(DATAPAGE,IDC_DATA_ACPOWER,temp);
		SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,temp);

		//bit 6 battery 
		if (Status & BATTERY) 
		{			
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,LOW);
			SetColor(DATAPAGE,IDC_DATA_BATTERYOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,LOW);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFileEx(58,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - BATTERY LOW (1b)\r\n");
				FlagOthr(1,true);
			}
			m_bBatteryOk = false;
			m_iData_Tab |= 0x0002;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
			if (!m_bBatteryOk)
				WriteToFileEx(58,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - BATTERY OK (1b)\r\n");
			m_bBatteryOk = true;
		}

		//Version: 00.00
		if (!bBinarySource)
		{
			strncpy(temp,&Record[16],5);
			temp[5] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_UGRANDP,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_VERSION","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_UGRANDP,COLOR_YELLOW);
				WriteToFileEx(59,TO_CEV|TO_PFM,TYPE_INST, &Julian, "00000 CONFIGURATION MISMATCH - VERSION (1b)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_VERSION,"SET_VERSION",temp,bMatch);
		}

		//Insp ID: 0000000
		//exclude if from binary record
		if (!bBinarySource)
		{
			strncpy(temp,&Record[21],7);
			temp[7] = NULL;
			SetString(DATAPAGE,IDC_DATA_USERID,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_USER_ID","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(DATAPAGE,IDC_DATA_USERID,COLOR_YELLOW);
				WriteToFileEx(60,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - USERID (1b)\r\n");
				m_iData_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_USER_ID,"SET_USER_ID",temp,bMatch);
		}

		//Board 1 type: FF
		//exclude if from binary record
		if (!bBinarySource)
		{
			strncpy(temp,&Record[28],2);
			temp[2] = NULL;
			SetString(DATAPAGE,IDC_DATA_UNITID2,temp);
			GetPrivateProfileString(m_pName,"SET_BOARD1_ID","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(DATAPAGE,IDC_DATA_UNITID2,COLOR_YELLOW);
				WriteToFileEx(61,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - BOARD1 (1b)\r\n");
				m_iData_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_BOARD1_ID,"SET_BOARD1_ID",temp,bMatch);
		}

		//Board 2 type: FF
		//exclude if from binary record
		if (!bBinarySource)
		{
			strncpy(temp,&Record[30],2);
			temp[2] = NULL;
			SetString(DATAPAGE,IDC_DATA_UNITID3,temp);		
			GetPrivateProfileString(m_pName,"SET_BOARD2_ID","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(DATAPAGE,IDC_DATA_UNITID3,COLOR_YELLOW);
				WriteToFileEx(62,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - BOARD2 (1b)\r\n");
				m_iData_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_BOARD2_ID,"SET_BOARD2_ID",temp,bMatch);
		}
	
		//acq mode
		if (!bBinarySource)
		{
			if (Record[32]=='0') strcpy(temp,"Gamma Only");
			else if (Record[32]== '1') strcpy(temp,"Neutron Only");
			else strcpy(temp,"Gamma & Neutron");
			temp[0] = Record[32];
			temp[1] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_MODE2,temp);
			GetPrivateProfileString(m_pName,"SET_ACQ_MODE1B","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			//if (strcmp(temp,temp2) != 0)
			if (Record[32] != temp2[0])
			{
				SetColor(GENERALPAGE,IDC_GENERAL_MODE2,COLOR_YELLOW);
				WriteToFileEx(63,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - ACQ MODE (1b)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_ACQ_MODE1B,"SET_ACQ_MODE1B",temp,bMatch);
		}

		//baud rate
		//exclude if from binary source

		//port setting
		//exclude if from binary source

		//XON
		//exclude if from binary source

		//battery voltage
		strncpy(temp,&Record[41],4);
		temp[4] = NULL;
		m_fCurBV=(float)atof(temp);
		if (m_bFirstBV)
		{
			m_bFirstBV = false;
			m_fLowBV = m_fHighBV = m_fCurBV;
		}
		if (m_fCurBV < m_fLowBV) 
		{
			m_fLowBV = m_fCurBV;
			CurrentLow = true;
		}
		if (m_fCurBV > m_fHighBV)
		{
			m_fHighBV= m_fCurBV;
			CurrentHigh= true;
		}
		if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
		{
			CurrentRed = true;
			SetColor(GENERALPAGE,IDC_GENERAL_BATTERYCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,COLOR_RED);
			if (m_bBatteryTolOk)
			{
				WriteToFileEx(64,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - BATTERY OUT OF TOLERANCE (1b)\r\n");
				FlagOthr(1,true);
			}
			m_bBatteryTolOk = false;
			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bBatteryTolOk)
				WriteToFileEx(64,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - BATTERY IN TOLERANCE (1b)\r\n");
			m_bBatteryTolOk = true;
		}

		if (CurrentRed && CurrentLow && (m_fLowBV <= m_fBatVoltL))// || (m_fLowBV >= m_fBatVoltH))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,COLOR_RED);
		}

		if (CurrentRed && CurrentHigh && (m_fHighBV >= m_fBatVoltH))// || (m_fHighBV >= m_fBatVoltH))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_BATTERYMIN,temp);
		sprintf(temp,"%5.1f",m_fCurBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_BATTERYCUR,temp);
		sprintf(temp,"%5.1f",m_fHighBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_BATTERYMAX,temp);

		//+5 volts
		strncpy(temp,&Record[45],4);
		temp[4] = NULL;
		m_fCurP5=(float)atof(temp);
		if (m_bFirstP5)
		{
			m_bFirstP5 = false;
			m_fLowP5 = m_fHighP5 = m_fCurP5;
		}
		if (m_fCurP5 < m_fLowP5)
		{
			CurrentLow = true;
			m_fLowP5 = m_fCurP5;
		}
		else
			CurrentLow = false;
		if (m_fCurP5 > m_fHighP5)
		{
			CurrentHigh = true;
			m_fHighP5= m_fCurP5;
		}
		else
			CurrentHigh = false;
		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		{
			CurrentRed = true;			
			SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,COLOR_RED);
			if (m_bP5VoltOk)
			{
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - PLUS 5 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
				FlagOthr(1,true);
			}
			m_bP5VoltOk = false;

			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bP5VoltOk)
				WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - PLUS 5 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
			m_bP5VoltOk = true;
		}

		if (CurrentRed && CurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,COLOR_RED);
		}

		if (CurrentRed && CurrentHigh && (m_fHighP5 >= m_fSup_P05H))// || (m_fHighP5 >= m_fSup_P05H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,temp);

		//+15 volts
		strncpy(temp,&Record[49],4);
		temp[4] = NULL;
		m_fCurP25=(float)atof(temp);
		if (m_bFirstP25)
		{
			m_bFirstP25 = false;
			m_fLowP25 = m_fHighP25 = m_fCurP25;
		}
		if (m_fCurP25 < m_fLowP25)
		{
			CurrentLow = true;
			m_fLowP25 = m_fCurP25;
		}
		else
			CurrentLow = false;
		if (m_fCurP25 > m_fHighP25)
		{
			CurrentHigh = true;
			m_fHighP25= m_fCurP25;
		}
		else
			CurrentHigh = false;

		if ((m_fCurP25 <= m_fSup_P25L) || (m_fCurP25 >= m_fSup_P25H))
		{
			CurrentRed = true;			
			SetColor(GENERALPAGE,IDC_GENERAL_P25VOLTCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P25VOLT2,COLOR_RED);
			if (m_bP25VoltOk)
			{
				WriteToFileEx(66,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - PLUS 15 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
				FlagOthr(1,true);
			}
			m_bP25VoltOk = false;
			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bP25VoltOk)
				WriteToFileEx(66,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - PLUS 15 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
			m_bP25VoltOk = true;
		}

		if (CurrentRed && CurrentLow && (m_fLowP25 <= m_fSup_P25L))// || (m_fLowP25 >= m_fSup_P25H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P25VOLTMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P25VOLT1,COLOR_RED);
		}

		if (CurrentRed && CurrentHigh && (m_fHighP25 >= m_fSup_P25H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P25VOLTMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P25VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowP25);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P25VOLT1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P25VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurP25);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P25VOLT2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P25VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighP25);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P25VOLT3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P25VOLTMAX,temp);

		//-15 volt
		strncpy(temp,&Record[53],5);
		temp[5] = NULL;
		m_fCurP33=(float)atof(temp);
		if (m_bFirstP33)
		{
			m_bFirstP33 = false;
			m_fLowP33 = m_fHighP33 = m_fCurP33;
		}
		if (m_fCurP33 < m_fLowP33)
		{
			CurrentLow = true;
			m_fLowP33 = m_fCurP33;
		}
		else
			CurrentLow = false;
		if (m_fCurP33 > m_fHighP33)
		{
			CurrentHigh = true;
			m_fHighP33= m_fCurP33;
		}
		else
			CurrentHigh = false;
		if ((m_fCurP33 <= m_fSup_P33L) || (m_fCurP33 >= m_fSup_P33H))
		{
			CurrentRed = true;
			SetColor(GENERALPAGE,IDC_GENERAL_P33VOLTCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P33VOLT2,COLOR_RED);
			if (m_bP33VoltOk)
			{
				WriteToFileEx(67,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - MINUS 15 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
				FlagOthr(1,true);
			}
			m_bP33VoltOk = false;
			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bP33VoltOk)
				WriteToFileEx(67,TO_CEV | TO_PFM, TYPE_INST, &Julian, "00000 STATUS CHANGE - MINUS 15 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
			m_bP33VoltOk = true;
		}

		if (CurrentRed && CurrentLow && (m_fLowP33 <= m_fSup_P33L))// || (m_fLowP33 >= m_fSup_P33H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P33VOLTMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P33VOLT1,COLOR_RED);
		}

		if (CurrentRed && CurrentHigh && (m_fHighP33 >= m_fSup_P33H))// || (m_fHighP33 >= m_fSup_P33H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P33VOLTMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P33VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowP33);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P33VOLT1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P33VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurP33);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P33VOLT2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P33VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighP33);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P33VOLT3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P33VOLTMAX,temp);

		//rom checksum
		if (!bBinarySource)
		{
			strncpy(temp,&Record[58],4);
			temp[4] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_ROM,temp);

			GetPrivateProfileString(m_pName,"SET_1B_ROM_CHECKSUM","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_ROM,COLOR_YELLOW);
				WriteToFileEx(68,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - 1B ROM CHECKSUM (1b)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_1B_ROM_CHECKSUM,"SET_1B_ROM_CHECKSUM",temp,bMatch);
		}
	
		if (m_pPropertySheet)
			m_pPropertySheet->GenStatRec(Record, bBinarySource);


		char cBuff[1024];
		strcpy(cBuff,"00000 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//general stat record 1b

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}
	PostMessage(IDC_RECEIVE_RESULT,GEN_STATUS_RECORD,CheckSum);
}
*/
/*
//record 1c
int CInstrument::BinaryMon_A_RecOld(BYTE *pBinaryAt)
{
#pragma pack(1)

	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR acq_mode;			//1
	} *psBinaryRecord;			//7 total
#pragma pack()

	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	char Record[1024];
						
	sprintf(Record,"1c%10u00.00 000000000000000.0000000.0000000.0000000.000000000.00000.0000.00000000.0000000.0%02x0000",
		psBinaryRecord->julian,
		psBinaryRecord->acq_mode
		);

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");	
	PostToWatch(Record);
	Mon_A_Rec(Record, true);
	
	return sizeof(SBinaryRec);
}
*/
/*
//record 1c
int CInstrument::BinaryMon_A_Rec(BYTE *pBinaryAt)
{
#pragma pack(1)

//	struct SBinaryRec {
//		UCHAR length;			//1
//		UCHAR type;				//1
//		unsigned int julian;	//4
//		UCHAR acq_mode;			//1
//	} *psBinaryRecord;			//7 total

	struct SBinaryRec {
		UCHAR length;					//1
		UCHAR type;						//1
		unsigned int usJulian;			//4
		char  cVersion[5];				//5
		unsigned short int usAcqTime;	//2
		UCHAR ucImmSize;				//1
		UCHAR ucImmSaveSize;			//1
		float fThresholds[5];			//20
		UCHAR ucChangingEntryCount;		//1
		float fChangingSigma;			//4
		UCHAR ucThresholdEntryCount;	//1
		float fThresholdMult;			//4
		UCHAR ucThresholdExitCount;		//1
		UCHAR ucBkgdSize;				//1
		UCHAR ucBkgdStart;				//1
		float fTamperSigma;				//4
		float fTamperCount;				//4
		char  cAcqMode;					//1
		unsigned short int usCodeCksum;	//2
	} *psBinaryRecord;					//57 total

#pragma pack()

	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->usJulian),4);
	Reverse((BYTE*)&(psBinaryRecord->usAcqTime),2);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[0]),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[1]),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[2]),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[3]),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[4]),4);
	Reverse((BYTE*)&(psBinaryRecord->fChangingSigma),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholdMult),4);
	Reverse((BYTE*)&(psBinaryRecord->fTamperSigma),4);
	Reverse((BYTE*)&(psBinaryRecord->fTamperCount),4);
	Reverse((BYTE*)&(psBinaryRecord->usCodeCksum),2);
	char Record[1024];
						
//	sprintf(Record,"1c%10u00.00 000000000000000.0000000.0000000.0000000.000000000.00000.0000.00000000.0000000.0%02x0000",
//		psBinaryRecord->julian,
//		psBinaryRecord->acq_mode
//		);

	sprintf(Record,"1c%10u%c%c%c%c%c%5d%02d%02d%8.1f%8.1f%8.1f%10.1f%10.1f%2d%3.1f%2d%3.1f%2d%02d%02d%3.1f%8.1f%02x%04X",
		psBinaryRecord->usJulian,				//%10u
		psBinaryRecord->cVersion[0],			//%c
		psBinaryRecord->cVersion[1],			//%c
		psBinaryRecord->cVersion[2],			//%c
		psBinaryRecord->cVersion[3],			//%c
		psBinaryRecord->cVersion[4],			//%c
		psBinaryRecord->usAcqTime,				//%05d
		psBinaryRecord->ucImmSize,				//%02d
		psBinaryRecord->ucImmSaveSize,			//%02d
		psBinaryRecord->fThresholds[0],			//%8.1f
		psBinaryRecord->fThresholds[1],			//%8.1f
		psBinaryRecord->fThresholds[2],			//%8.1f
		psBinaryRecord->fThresholds[3],			//%10.3f
		psBinaryRecord->fThresholds[4],			//%10.3f
		psBinaryRecord->ucChangingEntryCount,	//%2d
		psBinaryRecord->fChangingSigma,			//%3.1f
		psBinaryRecord->ucThresholdEntryCount,	//%2d
		psBinaryRecord->fThresholdMult,			//%3.1f
		psBinaryRecord->ucThresholdExitCount,	//%2d
		psBinaryRecord->ucBkgdSize,				//%02d
		psBinaryRecord->ucBkgdStart,			//%02d	
		psBinaryRecord->fTamperSigma,			//%3.1f
		psBinaryRecord->fTamperCount,			//%8.1f
		psBinaryRecord->cAcqMode,				//%02x
		psBinaryRecord->usCodeCksum				//%04x
		);

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");	
	PostToWatch(Record);
	Mon_A_Rec(Record, false);

	return sizeof(SBinaryRec);
}
*/
/*
//record 1c
void CInstrument::Mon_A_Rec(char *Record, bool bBinarySource)
{

	int CheckSum;
	if (LengthOk(Record,MONITOR_A_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(PRE4);
		m_cMon_A_Rec = Record;

		char temp[64];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		SetString(USERPAGE,IDC_USERPROG_TIME,temp);

		bool bMatch;
		
		char temp2[32];
		char temp3[32];

		//user program version number
		if (!bBinarySource)
		{
			strncpy(temp,&Record[12],5);
			temp[5] = NULL;
			SetString(USERPAGE,IDC_USERPROG_UGRANDUV,temp);

			GetPrivateProfileString(m_pName,"SET_UGRAND_USERVER","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_UGRANDUV,COLOR_YELLOW);

				WriteToFileEx(69,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - uGRAND USER VERSION (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_UGRAND_USERVER,"SET_UGRAND_USERVER",temp,bMatch);
		}
	
		//user select data acquire time
		if (!bBinarySource)
		{
			strncpy(temp,&Record[17],5);
			temp[5] = NULL;
			SetString(USERPAGE,IDC_USERPROG_USER,temp);

			GetPrivateProfileString(m_pName,"SET_ACQUIRE_TIME","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_USER,COLOR_YELLOW);
				WriteToFileEx(70,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - ACQUIRE TIME (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_ACQUIRE_TIME,"SET_ACQUIRE_TIME",temp,bMatch);

		}
	
		//Immediate Buffer Size / save size
		if (!bBinarySource)
		{
			strcpy(temp,"Total: ");
			strncat(temp,&Record[22],2);
			temp3[0] = Record[22];
			temp3[1] = Record[23];
			temp3[2] = NULL;
			GetPrivateProfileString(m_pName,"SET_BUFFER_TOTAL","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[22],temp2,2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_IMMEDIATE,COLOR_YELLOW);
				WriteToFileEx(71,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - BUFFER TOTAL (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_BUFFER_TOTAL,"SET_BUFFER_TOTAL",temp3,bMatch);

			strcat(temp,", Save: ");
			strncat(temp,&Record[24],2);
			temp3[0] = Record[24];
			temp3[1] = Record[25];
			temp3[2] = NULL;
			SetString(USERPAGE,IDC_USERPROG_IMMEDIATE,temp);

			GetPrivateProfileString(m_pName,"SET_BUFFER_SAVE","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[24],temp2,2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_IMMEDIATE,COLOR_YELLOW);
				WriteToFileEx(72,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - BUFFER SAVE (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_BUFFER_SAVE,"SET_BUFFER_SAVE",temp3,bMatch);
		}
	
		if (!bBinarySource)
		{
			//neutron A threshold
			strncpy(temp,&Record[26],8);
			temp[8] = NULL;
			SetString(USERPAGE,IDC_USERPROG_PULSEAT,temp);

			GetPrivateProfileString(m_pName,"SET_NEUTRON_PULSEATHRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_PULSEAT,COLOR_YELLOW);
				WriteToFileEx(73,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - NEUTRON PULSE A THRESHOLD (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_NEUTRON_PULSEATHRESH,"SET_NEUTRON_PULSEATHRESH",temp,bMatch);
	
			//neutron B threshold
			strncpy(temp,&Record[34],8);
			temp[8] = NULL;
			SetString(USERPAGE,IDC_USERPROG_PULSEBT,temp);
			GetPrivateProfileString(m_pName,"SET_NEUTRON_PULSEBTHRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_PULSEBT,COLOR_YELLOW);
				WriteToFileEx(74,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - NEUTRON PULSE B THRESHOLD (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_NEUTRON_PULSEBTHRESH,"SET_NEUTRON_PULSEBTHRESH",temp,bMatch);
		
			//neutron C threshold
			strncpy(temp,&Record[42],8);
			temp[8] = NULL;
			SetString(USERPAGE,IDC_USERPROG_PULSECT,temp);
			GetPrivateProfileString(m_pName,"SET_NEUTRON_PULSECTHRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_PULSECT,COLOR_YELLOW);
				WriteToFileEx(75,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - NEUTRON PULSE C THRESHOLD (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_NEUTRON_PULSECTHRESH,"SET_NEUTRON_PULSECTHRESH",temp,bMatch);
		
			//gamma 1 threshold
			strncpy(temp,&Record[50],10);
			temp[10] = NULL;
			SetString(USERPAGE,IDC_USERPROG_GAMMA1T,temp);
			GetPrivateProfileString(m_pName,"SET_GAMMA_1THRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_GAMMA1T,COLOR_YELLOW);
				WriteToFileEx(76,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - GAMMA 1 THRESHOLD (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_GAMMA_1THRESH,"SET_GAMMA_1THRESH",temp,bMatch);
		
			//gamma 2 threshold
			strncpy(temp,&Record[60],10);
			temp[10] = NULL;
			SetString(USERPAGE,IDC_USERPROG_GAMMA2T,temp);
			GetPrivateProfileString(m_pName,"SET_GAMMA_2THRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_GAMMA2T,COLOR_YELLOW);
				WriteToFileEx(77,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - GAMMA 2 THRESHOLD (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_GAMMA_2THRESH,"SET_GAMMA_2THRESH",temp,bMatch);
		
			//no filter and enter mii
			strncpy(temp,&Record[70],2);
			temp[2] = NULL;
			char temp3[4];
			temp3[0] = Record[70];
			temp3[1] = Record[71];
			temp3[2] = NULL;
			strcat(temp," runs > ");
			strncat(temp,&Record[72],3);
			strcat(temp," sigma from background");
			SetString(USERPAGE,IDC_USERPROG_NOFILTER,temp);
			GetPrivateProfileString(m_pName,"SET_NOFILTERENTERMII_RUNS","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[70],temp2,2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_NOFILTER,COLOR_YELLOW);
				WriteToFileEx(78,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - NO FILTER ENTER MII RUNS (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_NOFILTERENTERMII_RUNS,"SET_NOFILTERENTERMII_RUNS",temp3,bMatch);

			temp3[0] = Record[72];
			temp3[1] = Record[73];
			temp3[2] = Record[74];
			temp3[4] = NULL;
			GetPrivateProfileString(m_pName,"SET_NOFILTERENTERMII_SIGMA","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[72],temp2,3) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_NOFILTER,COLOR_YELLOW);
				WriteToFileEx(79,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - NO FILTER ENTER MII SIGMA (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_NOFILTERENTERMII_SIGMA,"SET_NOFILTERENTERMII_SIGMA",temp3,bMatch);
		
			//mii enter - consecutive runs
			strncpy(temp,&Record[75],2);
			temp[2] = NULL;
			temp3[0] = Record[75];
			temp3[1] = Record[76];
			temp3[2] = NULL;
			strcat(temp," runs > ");
			strncat(temp,&Record[77],3);
			strcat(temp," sigma from background");
			SetString(USERPAGE,IDC_USERPROG_MIIENTER,temp);
			GetPrivateProfileString(m_pName,"SET_ENTERMII_RUNS","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[75],temp2,2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_MIIENTER,COLOR_YELLOW);
				WriteToFileEx(80,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - ENTER MII RUNS (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_ENTERMII_RUNS,"SET_ENTERMII_RUNS",temp3,bMatch);

			temp3[0] = Record[77];
			temp3[1] = Record[78];
			temp3[2] = Record[79];
			temp3[3] = NULL;
			GetPrivateProfileString(m_pName,"SET_ENTERMII_SIGMA","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[77],temp2,3) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_MIIENTER,COLOR_YELLOW);
				WriteToFileEx(81,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - ENTER MII SIGMA (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_ENTERMII_SIGMA,"SET_ENTERMII_SIGMA",temp3,bMatch);
		
			//mii exit
			strncpy(temp,&Record[80],2);
			temp[2] = NULL;
			temp3[0] = Record[80];
			temp3[1] = Record[81];
			temp3[2] = NULL;
			strcat(temp," runs < MII threshold");
			SetString(USERPAGE,IDC_USERPROG_MIIEXIT,temp);
			GetPrivateProfileString(m_pName,"SET_MII_EXIT","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[80],temp2,2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_MIIEXIT,COLOR_YELLOW);
				WriteToFileEx(82,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - MII EXIT (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_MII_EXIT,"SET_MII_EXIT",temp3,bMatch);
		
			//local background
			strcpy(temp,"First ");
			strncat(temp,&Record[82],2);
			temp3[0] = Record[82];
			temp3[1] = Record[83];
			temp3[2] = NULL;
			GetPrivateProfileString(m_pName,"SET_LOCAL_BACKGROUND_FIRST","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[82],temp2,2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_LOCALBACK,COLOR_YELLOW);
				WriteToFileEx(83,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - LOCAL BACKGROUND FIRST(1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_LOCAL_BACKGROUND_FIRST,"SET_LOCAL_BACKGROUND_FIRST",temp3,bMatch);

		
			strcat(temp," of last ");
			strncat(temp,&Record[84],2);
			temp3[0] = Record[84];
			temp3[1] = Record[85];
			temp3[2] = NULL;
			strcat(temp," runs");
			GetPrivateProfileString(m_pName,"SET_LOCAL_BACKGROUND_LAST","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[84],temp2,2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_LOCALBACK,COLOR_YELLOW);
				WriteToFileEx(84,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - LOCAL BACKGROUND LAST(1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_LOCAL_BACKGROUND_LAST,"SET_LOCAL_BACKGROUND_LAST",temp3,bMatch);

		}
	
		//acquisition mode
		strncpy(temp,&Record[97],2);
		temp[2] = NULL;
		short Status;
		short Status2;
		sscanf(temp,"%2hx",&Status);
		temp2[0] = NULL;
		Status2 = 0;
		GetPrivateProfileString(m_pName,"SET_ACQ_MODE1C","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strlen(temp2))
			sscanf(temp2,"%2hx",&Status2);
		if ((Status & 0x0001) != (Status2 & 0x0001))
		{
			SetColor(USERPAGE,IDC_USERPROG_PULSEAF,COLOR_YELLOW);
			WriteToFileEx(85,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - PULSEA FLAG (1c)\r\n");
			m_iUserProg_Tab |= 0x0002;
			bMatch = false;
		}
		SetString(USERPAGE,IDC_USERPROG_PULSEAF,(Status & 0x0001)?YES:NO);

		if ((Status & 0x0002) != (Status2 & 0x0002))
		{
			SetColor(USERPAGE,IDC_USERPROG_PULSEBF,COLOR_YELLOW);
			WriteToFileEx(86,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - PULSEB FLAG (1c)\r\n");
			m_iUserProg_Tab |= 0x0002;
			bMatch = false;
		}
		SetString(USERPAGE,IDC_USERPROG_PULSEBF,(Status & 0x0002)?YES:NO);

		if ((Status & 0x0004) != (Status2 & 0x0004))
		{
			SetColor(USERPAGE,IDC_USERPROG_PULSECF,COLOR_YELLOW);
			WriteToFileEx(87,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - PULSEC FLAG (1c)\r\n");
			m_iUserProg_Tab |= 0x0002;
			bMatch = false;
		}
		SetString(USERPAGE,IDC_USERPROG_PULSECF,(Status & 0x0004)?YES:NO);

		if ((Status & 0x0008) != (Status2 & 0x0008))
		{
			SetColor(USERPAGE,IDC_USERPROG_GAMMA1F,COLOR_YELLOW);
			WriteToFileEx(88,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - GAMMA1 FLAG (1c)\r\n");
			m_iUserProg_Tab |= 0x0002;
			bMatch = false;
		}
		SetString(USERPAGE,IDC_USERPROG_GAMMA1F,(Status & 0x0008)?YES:NO);

		if ((Status & 0x0010) != (Status2 & 0x0010))
		{
			SetColor(USERPAGE,IDC_USERPROG_GAMMA2F,COLOR_YELLOW);
			WriteToFileEx(89,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - GAMMA2 FLAG (1c)\r\n");
			m_iUserProg_Tab |= 0x0002;
			bMatch = false;
		}
		SetString(USERPAGE,IDC_USERPROG_GAMMA2F,(Status & 0x0010)?YES:NO);
		CurrentSetParam(SET_ACQ_MODE1C,"SET_ACQ_MODE1C",temp,bMatch);
	
		//rom checksum
		if (!bBinarySource)
		{
			strncpy(temp,&Record[99],4);
			temp[4] = NULL;
			SetString(USERPAGE,IDC_USERPROG_ROMCHECK,temp);

			GetPrivateProfileString(m_pName,"SET_1C_ROM_CHECKSUM","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_ROMCHECK,COLOR_YELLOW);
				WriteToFileEx(90,TO_CEV|TO_PFM,TYPE_INST,&Julian,"00000 CONFIGURATION MISMATCH - ROM CHECKSUM (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_1C_ROM_CHECKSUM,"SET_1C_ROM_CHECKSUM",temp,bMatch);
		}
		
		if (m_pPropertySheet)
			m_pPropertySheet->Mon_A_Rec(Record, bBinarySource);

		char cBuff[1024];
		strcpy(cBuff,"00000 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//mon_a_rec 1c

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,MONITOR_A_RECORD,CheckSum);	
}
*/
/*
//record 1d
void CInstrument::TripN2Rec(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,TRIP_NEU2_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{

		m_cTripN2Rec = Record;

		char temp[32];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);
		char cBuff[1024];
		strcpy(cBuff,"00000 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//tripn2rec 1d

		if (m_pPropertySheet)
			m_pPropertySheet->TripN2Rec(Record);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}
	PostMessage(IDC_RECEIVE_RESULT,TRIP_NEU2_RECORD,CheckSum);
}
*/
/*
//record 1e
//binary version should NEVER be received
int CInstrument::BinaryID2_Record(BYTE *pBinaryAt)
{

#pragma pack(1)
	struct SBinaryRec {	
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR main_id;			//1
		UCHAR status1;			//1
		UCHAR status2;			//1
		ULONG bbm_used;			//4
	} *psBinaryRecord;			//13 total
#pragma pack()

	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->bbm_used),4);
	char Record[1024];
	sprintf(Record,"1e%10u%02x%02x%02x%6u",
		psBinaryRecord->julian,
		psBinaryRecord->main_id,
		psBinaryRecord->status1,
		psBinaryRecord->status2,
		psBinaryRecord->bbm_used
		);
	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	PostToWatch(Record);
	ID2_Record(Record);
	
	return sizeof(SBinaryRec);
}
*/

//record 1e
void CInstrument::ID2_Record(char *Record)
{
	//don't know if pre 4.1 or post 4.1 so don't:
	//AdjustTabsTo(PRE4);

//SET_IRR_TIME
//SET_IRR_BOARDSN


	int CheckSum;
	if (LengthOk(Record,MONITOR_ID2_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		m_cID2_Record = Record;

		bool AnyChange = false;

		char temp[64];
		char temp2[128];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		strcpy(temp2,Julian.Date());
		strcat(temp2," ");
		strcat(temp2,Julian.Time());
		CurrentSetParam(SET_IRR_TIME,"SET_IRR_TIME",temp2,false);

		if (Julian.Year() < 1990)
		{
			SetColor(DATAIPAGE,IDC_DATAI_INQUIRETIME,COLOR_RED);
			PostMessage(IDC_RECEIVE_RESULT,IDC_BADYEAR_VALUE,Julian.Year());
			WriteToFileEx(91,TO_CEV|TO_PFM,TYPE_COMP,&Julian,"00000 Year less then 1990");
		}

		//post invalid time to CEV and PFM if necessary
		if (m_szPreviousGTime[0] != NULL) 
		{
			if (strcmp(m_szPreviousGTime,temp)>0)
			{
//
//dgp remove eventually -- this is looking into where 
//time is shifting in the middle of the night
//char temptemp[256];
//strcpy(temptemp,m_szPreviousGTime);
//strcat(temptemp," ");
//strcat(temptemp,temp);
//WriteToFile(TO_PFM,TYPE_INVTIME,NULL,temptemp);
//
//
//
				m_dInvalidTimeCount++;
				CJulianTime JulianT(m_szPreviousGTime);
				WriteToFileEx(92,TO_CEV|TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
				FlagTime(1,false);			
				SetColor(DATAIPAGE,IDC_DATAI_INQUIRETIME,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_RED);
			}
			else
			{
				m_dInvalidTimeCount = 0;
			}
		}

		strcpy(m_szPreviousGTime,temp);

		if ((m_bStartOfFile)||(Julian.Day()!=m_dCurrentDay))
		{
			char cBuff[1024];
			strcpy(cBuff,"00000 ");
			strcat(cBuff,Record);
			WriteToFile(TO_CEV | TO_PFM, TYPE_GID2, &Julian, cBuff);//id2 record if start of file
			m_bStartOfFile = false;
			m_dCurrentDay = Julian.Day();
		}

		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
			      Julian.Hour(), Julian.Minute(), Julian.Second() );
		COleDateTime t2 = COleDateTime::GetCurrentTime();
		COleDateTimeSpan ts = t1 - t2;  // Subtract 2 COleDateTimes
		ts += HALF_SECOND;

		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());

		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
			(ts.GetTotalSeconds() < -m_dTime_Err ))
		{
			FlagTime(1,false);			
			SetColor(DATAIPAGE,IDC_DATAI_INQUIRETIME,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_RED);
			m_iDataI_Tab |= 0x0001;
			if (!m_bTimeError)
			{
				if (WriteToFileEx(93,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - Time Out of Tolerance (1e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);//id2 record
			}
			m_bTimeError = true;
		} 

		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
			(ts.GetTotalSeconds() > -m_dTime_Err ))
		{
			if (m_bTimeError)
			{
				if (WriteToFileEx(94,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - Time In Tolerance (1e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);
			}
			m_bTimeError = false;
		}
		long days = ts.GetDays();
		if (days > 0)
			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
		else if (days == 0)
			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
		else 
		{
			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
		}
		SetString(DATAIPAGE,IDC_DATAI_INQUIRETIME,temp);
		SetString(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,temp);

		bool bMatch;

		//get Main Unit ID
		strncpy(temp,&Record[12],2);
		temp[2] = NULL;
		temp2[0] = NULL;
		//SetString(DATAIPAGE,SET_IIR_MGSN,temp);//11
		SetString(DATAIPAGE,IDC_DATAI_UNITID,temp);//11
		GetPrivateProfileString(m_pName,"SET_IRR_BOARDSN","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(DATAIPAGE,IDC_DATAI_UNITID,COLOR_YELLOW);
			WriteToFileEx(95,TO_CEV|TO_PFM,TYPE_INST, &Julian, "00000 CONFIGURATION MISMATCH - UNIT ID (1e)\r\n");
			m_iDataI_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_IRR_BOARDSN,"SET_IRR_BOARDSN",temp,true);
		
		//get status byte 1 and 2
		short Status1, Status2;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status1);
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status2);

		//bit 7 acpower
		if (Status1 & ACPOWER)
		{			
			SetString(DATAIPAGE,IDC_DATAI_ACPOWER,FAIL);
			SetColor(DATAIPAGE,IDC_DATAI_ACPOWER,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,FAIL);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
			if (m_bACPowerOk) 
			{
				WriteToFileEx(96,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - External Power Off (1e)\r\n");
				AnyChange = true;
				FlagOthr(1,true);
			}
			m_bACPowerOk = false;
			m_iDataI_Tab |= 0x0001;
		}
		else 
		{
			SetString(DATAIPAGE,IDC_DATAI_ACPOWER,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,OK);
			if (!m_bACPowerOk)
			{
				WriteToFileEx(96,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - External Power On (1e)\r\n");
				AnyChange = true;
			}
			m_bACPowerOk = true;
		}

		//bit 6 battery
		if (Status1 & BATTERY) 
		{
			SetString(DATAIPAGE,IDC_DATAI_BATTERYOK,LOW);
			SetColor(DATAIPAGE,IDC_DATAI_BATTERYOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,LOW);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFileEx(97,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - BATTERY LOW (1e)\r\n");
				AnyChange = true;
				FlagOthr(1,true);
			}

			m_bBatteryOk = false;
			m_iDataI_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAIPAGE,IDC_DATAI_BATTERYOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
			if (!m_bBatteryOk)
			{
				WriteToFileEx(97,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - BATTERY OK (1e)\r\n");
				AnyChange = true;
			}
			m_bBatteryOk = true;
		}

		//bit 5 tamper alarm
		if (Status1 & TAMPER)
		{
			if (m_bCountRateTamperOk)
			{
				WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - Changing Signal (1e)\r\n");
				AnyChange = true;
			}

			m_bCountRateTamperOk = false;
		}
		else
		{
			if (!m_bCountRateTamperOk)
			{
				WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - No Changing Signal (1e)\r\n");
				AnyChange = true;
			}
			m_bCountRateTamperOk = true;
		}

		//bit 4 RMS Sigma Tamper
		if (Status1 & RMSTAMPER)
		{
			if (m_bRMSSigmaTamperOk)
			{
				WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - Threshold Event (1e)\r\n");
				AnyChange = true;
			}
			m_bRMSSigmaTamperOk = false;
		}
		else
		{
			if (!m_bRMSSigmaTamperOk)
			{
				WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - No Threshold Event (1e)\r\n");
				AnyChange = true;
			}
			m_bRMSSigmaTamperOk = true;
		}

		//bit 3 bbm
		if (Status1 & BBMERROR)
		{
			char cBuff[1024];
			if (Status2 & BBMWRITEERROR_FLAG)
			{
				strcpy(temp,BBMWRITEERROR);
				strcpy(cBuff,"00000");
			}
			else if (Status2 & BBMNUMOFBYTES_FLAG)
			{
				strcpy(temp,BBMNUMOFBYTES);
				strcpy(cBuff,"00000");
			}
			else if (Status2 & BBMISFULLEDER_FLAG)
			{
				strcpy(temp,BBMISFULLEDER);
				strcpy(cBuff,"00000");
			}
			else
			{
				strcpy(temp,BBMINVALIDOPC);
				strcpy(cBuff,"00000");
			}
			strcat(cBuff," STATUS CHANGE (cont) - ");

			
			SetString(DATAIPAGE,IDC_DATAI_BBMOK,temp);
			SetColor(DATAIPAGE,IDC_DATAI_BBMOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,temp);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BBMOK,COLOR_RED);
			if (m_bBBMOk)
			{
				char temp2[256];
				strcpy(temp2,cBuff);
				strcat(temp2,temp);
				strcat(temp2,"\r\n");
				if (WriteToFileEx(100,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - BATTERY BACKED UP MEMORY ERROR (1e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, temp2);//specific failure
				AnyChange = true;
				FlagOthr(1,true);
			}

			m_bBBMOk = false;
			m_iDataI_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAIPAGE,IDC_DATAI_BBMOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,OK);
			if (!m_bBBMOk)
			{
				WriteToFileEx(100,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - NO BATTERY BACKED UP MEMORY ERROR (1e)\r\n");
				AnyChange = true;
			}
			m_bBBMOk = true;
		}
	
		//bit 2 startup
		if (Status1 & CWSTART) 
		{
			SetString(DATAIPAGE,IDC_DATAI_OPERATION,START_COLD);
			if (m_dCWStart != 0)
			{
				WriteToFileEx(101,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - COLD START (1e)\r\n");
				AnyChange = true;
			}
			m_dCWStart = 0;
		}
		else
		{
			SetString(DATAIPAGE,IDC_DATAI_OPERATION,START_NORMAL);
			if (m_dCWStart != 1)
			{
				WriteToFileEx(101,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - NORMAL START (1e)\r\n");
				AnyChange = true;
			}
			m_dCWStart = 1;
		}
	
		//bit 0 mii
		if (Status1 & INMII)
		{
			m_pButton->ShowIcon_Bell(VARIANT_TRUE);
			SetString(DATAIPAGE,IDC_DATAI_OOMII,INMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,INMII_TXT);
			if (!m_bInMII && m_bLogMII)
			{
				WriteToFileEx(102,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - In MEASUREMENT INTERVAL OF INTEREST [BELL icon on] (1e)\r\n");
				AnyChange = true;
			}
			m_bInMII = true;
		}
		else
		{
			m_pButton->ShowIcon_Bell(VARIANT_FALSE);
			SetString(DATAIPAGE,IDC_DATAI_OOMII,NOTINMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,NOTINMII_TXT);
			if (m_bInMII && m_bLogMII)
			{
				WriteToFileEx(102,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - Out of MEASUREMENT INTERVAL OF INTEREST [BELL icon off] (1e)\r\n");
				AnyChange = true;
			}
			m_bInMII = false;
		}
	
		//bit 7 of sw 2
		if (Status2 & DATAFILTER_FLAG)
		{
			SetString(DATAIPAGE,IDC_DATAI_DATAFILTER,DATAFILTER_NO);
			if (!m_bDataFiltered && m_bLogFilter)
			{
				WriteToFileEx(103,TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - DATA NOT FILTERED (1e)\r\n");
				AnyChange = true;
			}

			m_bDataFiltered = true;
		}
		else
		{
			SetString(DATAIPAGE,IDC_DATAI_DATAFILTER,DATAFILTER_YES);
			if (m_bDataFiltered && m_bLogFilter)
			{
				WriteToFileEx(103,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - DATA FILTERED (1e)\r\n");
				AnyChange = true;
			}

			m_bDataFiltered = false;
		}

		//write to pfm file if there was a change in the status bits
		if (AnyChange)
		{
			char cBuff[1024];
			strcpy(cBuff,"00000 ");
			strcat(cBuff,Record);
			WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//any change to status
		}
	
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

		//get bbm
		strncpy(temp,&Record[18],6);
		temp[6]=NULL;
		char *pCh = temp;
		while (*pCh == ' ')pCh++;
		SetString(DATAIPAGE,IDC_DATAI_BYTESMEM,pCh);
		int bbm=atoi(temp);
		m_pButton->put_Numeric((ULONG)bbm);

		if (m_pPropertySheet)
			m_pPropertySheet->ID2_Record(Record);	
		
		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,bbm);
	}
	PostMessage(IDC_RECEIVE_RESULT,MONITOR_ID2_RECORD,CheckSum);	
}

//record 1f
void CInstrument::AnalyzeOk(char *Record)
{
	//don't know if we will get 4.10+ records so don't do:
	//AdjustTabsTo(PRE4);

	int CheckSum;
	if (LengthOk(Record,ANALYZE_OK_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		m_cAnalyzeOkRec = Record;

		char temp[8];
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		int bbm=atoi(temp);
		m_pButton->put_Numeric((ULONG)bbm);
		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,bbm);

		if (m_pPropertySheet)
			m_pPropertySheet->AnalyzeOk(Record);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}
	PostMessage(IDC_RECEIVE_RESULT,ANALYZE_OK_RECORD,CheckSum);
}

//record 30
int CInstrument::OnBinaryInfoRecord(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;
		UCHAR type;
		unsigned int julian;
		UCHAR flag_byte1;
		UCHAR flag_byte2;
		UCHAR flag_byte3;
		UCHAR flag_byte4;
		UCHAR status;
		UCHAR days;
		UCHAR hr;
		UCHAR min;
		UCHAR sec;
	} *psBinaryRecord;
#pragma pack()
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	char Record[1024];
	
	if (psBinaryRecord->flag_byte1 & 0x80)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x%02x%02u:%02u:%02u",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4,
			psBinaryRecord->status,
			(psBinaryRecord->hr  > 99)?99:psBinaryRecord->hr,
			(psBinaryRecord->min > 99)?99:psBinaryRecord->min,
			(psBinaryRecord->sec > 99)?99:psBinaryRecord->sec
			);
	} else if (psBinaryRecord->flag_byte1 & 0x40)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x%3d       ",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4,
			psBinaryRecord->days
			);
	} else if (psBinaryRecord->flag_byte1 & 0x20)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x%02x        ",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4,
			psBinaryRecord->status
			);
	} else if (psBinaryRecord->flag_byte1 & 0x10)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x          ",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4
			);
	} else if (psBinaryRecord->flag_byte1 & 0x08)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x          ",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4
			);
	} else 
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x%02X%02X%02X%02X%02X",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4,
			psBinaryRecord->status,
			psBinaryRecord->days,
			psBinaryRecord->hr, 
			psBinaryRecord->min,
			psBinaryRecord->sec
			);
	}

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	PostToWatch(Record);
	OnInfoRecord(Record);

	return sizeof(SBinaryRec);
}

//record 30
void CInstrument::OnInfoRecord(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,INFORMATION_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		bool bStale = true;										

		char temp[32];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);
		if (m_uiInformationRecordTime < Julian.TotalSeconds())	
		{														
			m_uiInformationRecordTime = Julian.TotalSeconds();	

			m_cInformRec = Record;								

			if (m_pPropertySheet)								
				m_pPropertySheet->InformRec(Record);			

			bStale = false;										
		}														

		char cBuff[1024];
		strcpy(cBuff,"00000 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//InformRec	

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}

	PostMessage(IDC_RECEIVE_RESULT,INFORMATION_RECORD,CheckSum);
}

//record 4d
void CInstrument::AnalyzeOk3Record(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,ANALYZE_OK3_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
//		AdjustTabsTo(POST4);
		m_cAnalyzeOk3Rec = Record;

		char temp[16];
		strncpy(temp,&Record[2],10);
		temp[10] = NULL;
		// SCR00235 Processing of 4F DUMPOKRESPONSE
		//int bbm=atoi(temp);
		unsigned long ulbbm;
		sscanf(temp,"%d",&ulbbm);
		m_pButton->put_Numeric(ulbbm);

		if (m_pPropertySheet)
			m_pPropertySheet->AnalyzeOk3Record(Record);

		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,ulbbm);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}
	PostMessage(IDC_RECEIVE_RESULT,ANALYZE_OK_RECORD,CheckSum);
}

//record 4f
void CInstrument::DumpOk3_Record(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,DUMP_OK3_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
//		AdjustTabsTo(POST4);
		char temp[16];

		//get bbm bytes
		// SCR00235 Processing of 4F DUMPOKRESPONSE
		unsigned long ulbbm;
		strncpy(temp,&Record[2],10);
		temp[10] = NULL;
		sscanf(temp,"%d",&ulbbm);
		if (m_pButton)
			m_pButton->put_Numeric(ulbbm);
		char *pCh = temp;
		while (*pCh == ' ')pCh++;
		SetString(DATAIPAGE,IDC_DATAI_BYTESMEM,pCh);

		if (m_pPropertySheet)
			m_pPropertySheet->DumpOk3_Record(Record);

		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,ulbbm);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}
	PostMessage(IDC_RECEIVE_RESULT,DUMP_OK_RECORD,CheckSum);
}
 
//record 4e
void CInstrument::ID3_Record(char *Record)
{

	int CheckSum;
	LengthOk(Record,MONITOR_ID3_RECORD_LENGTH);
	if (true)
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
	
		m_cID3_Record = Record;

		bool AnyChange = false;
	
		char temp[64];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (Julian.Year() < 1990)
		{
			SetColor(DATAIPAGE,IDC_DATAI_INQUIRETIME,COLOR_RED);
			PostMessage(IDC_RECEIVE_RESULT,IDC_BADYEAR_VALUE,Julian.Year());
			WriteToFileEx(119,TO_CEV|TO_PFM,TYPE_COMP,&Julian,"00000 Year less then 1990");
		}

		//post invalid time to CEV if necessary
		if (m_szPreviousGTime[0] != NULL) 
		{
			if (strcmp(m_szPreviousGTime,temp)>0)
			{
				m_dInvalidTimeCount++;
				CJulianTime JulianT(m_szPreviousGTime);
				WriteToFileEx(120,TO_CEV|TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
				FlagTime(1,false);			
				SetColor(DATAIPAGE,IDC_DATAI_INQUIRETIME,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_RED);
			}
			else
			{
				m_dInvalidTimeCount = 0;
			}
		}

		strcpy(m_szPreviousGTime,temp);

		if ((m_bStartOfFile)||(Julian.Day()!=m_dCurrentDay))
		{
			char cBuff[1024];
			strcpy(cBuff,"00000 ");
			strcat(cBuff,Record);
			WriteToFile(TO_CEV | TO_PFM, TYPE_GID2, &Julian, cBuff);//id2 record if start of file
			m_bStartOfFile = false;
			m_dCurrentDay = Julian.Day();
		}

		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
			      Julian.Hour(), Julian.Minute(), Julian.Second() );
		COleDateTime t2 = COleDateTime::GetCurrentTime();
		COleDateTimeSpan ts = t1 - t2;  // Subtract 2 COleDateTimes
		ts += HALF_SECOND;

		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());

		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
			(ts.GetTotalSeconds() < -m_dTime_Err ))
		{
			FlagTime(1,false);			
			SetColor(DATAIPAGE,IDC_DATAI_INQUIRETIME,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_RED);
			m_iDataI_Tab |= 0x0001;
			if (!m_bTimeError)
			{
				if (WriteToFileEx(121,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - Time Out of Tolerance (4e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);//id2 record
			}
			m_bTimeError = true;
		} 

		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
			(ts.GetTotalSeconds() > -m_dTime_Err ))
		{
			if (m_bTimeError)
			{
				if (WriteToFileEx(121,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - Time In Tolerance (4e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);
			}
			m_bTimeError = false;
		}
		long days = ts.GetDays();
		if (days > 0)
			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
		else if (days == 0)
			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
		else 
		{
			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
		}
		SetString(DATAIPAGE,IDC_DATAI_INQUIRETIME,temp);
		SetString(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,temp);

		bool bMatch;

		//get Main Unit ID
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		char temp2[8];
		temp2[0] = NULL;
		SetString(DATAIPAGE,IDC_DATAI_UNITID,temp);
		GetPrivateProfileString(m_pName,"SET_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(DATAIPAGE,IDC_DATAI_UNITID,COLOR_YELLOW);
			WriteToFileEx(122,TO_CEV|TO_PFM,TYPE_INST, &Julian, "00000 CONFIGURATION MISMATCH - UNIT ID (1b)\r\n");
			m_iDataI_Tab |= 0x0002;
			bMatch = false;
		}
		
		//get status byte 1 and 2
		short Status1, Status2;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status1);
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status2);

		//bit 7 acpower
		if (Status1 & ACPOWER)
		{
			SetString(DATAIPAGE,IDC_DATAI_ACPOWER,FAIL);
			SetColor(DATAIPAGE,IDC_DATAI_ACPOWER,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,FAIL);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
			if (m_bACPowerOk) 
			{
				WriteToFileEx(123,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - External Power Off (4e)\r\n");
				AnyChange = true;
				FlagOthr(1,true);
			}
			m_bACPowerOk = false;
			m_iDataI_Tab |= 0x0001;
		}
		else 
		{
			SetString(DATAIPAGE,IDC_DATAI_ACPOWER,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,OK);
			if (!m_bACPowerOk)
			{
				WriteToFileEx(123,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - External Power On (4e)\r\n");
				AnyChange = true;
			}
			m_bACPowerOk = true;
		}
	
		//bit 6 battery
		if (Status1 & BATTERY) 
		{
			SetString(DATAIPAGE,IDC_DATAI_BATTERYOK,LOW);
			SetColor(DATAIPAGE,IDC_DATAI_BATTERYOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,LOW);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFileEx(124,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - BATTERY LOW (4e)\r\n");
				AnyChange = true;
				FlagOthr(1,true);
			}

			m_bBatteryOk = false;
			m_iDataI_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAIPAGE,IDC_DATAI_BATTERYOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
			if (!m_bBatteryOk)
			{
				WriteToFileEx(124,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - BATTERY OK (4e)\r\n");
				AnyChange = true;
			}
			m_bBatteryOk = true;

		}
	
		//bit 5 tamper alarm
		if (Status1 & TAMPER)
		{
			if (m_bCountRateTamperOk)
			{
				WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - Changing Signal (1e)\r\n");
				AnyChange = true;
			}

			m_bCountRateTamperOk = false;
		}
		else
		{
			if (!m_bCountRateTamperOk)
			{
				WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - No Changing Signal (1e)\r\n");
				AnyChange = true;
			}
			m_bCountRateTamperOk = true;
		}

		//bit 4 RMS Sigma Tamper
		if (Status1 & RMSTAMPER)
		{
			if (m_bRMSSigmaTamperOk)
			{
				WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - Threshold Event (1e)\r\n");
				AnyChange = true;
			}

			m_bRMSSigmaTamperOk = false;
		}
		else
		{
			if (!m_bRMSSigmaTamperOk)
			{
				WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - No Threshold Event (1e)\r\n");
				AnyChange = true;
			}

			m_bRMSSigmaTamperOk = true;
		}

		//bit 3 bbm
		if (Status1 & BBMERROR)
		{
			char cBuff[1024];
			if (Status2 & BBMWRITEERROR_FLAG)
			{
				strcpy(temp,BBMWRITEERROR);
				strcpy(cBuff,"00000");
			}
			else if (Status2 & BBMNUMOFBYTES_FLAG)
			{
				strcpy(temp,BBMNUMOFBYTES);
				strcpy(cBuff,"00000");
			}
			else if (Status2 & BBMISFULLEDER_FLAG)
			{
				strcpy(temp,BBMISFULLEDER);
				strcpy(cBuff,"00000");
			}
			else
			{
				strcpy(temp,BBMINVALIDOPC);
				strcpy(cBuff,"00000");
			}
			strcat(cBuff," STATUS CHANGE (cont) - ");
		
			SetString(DATAIPAGE,IDC_DATAI_BBMOK,temp);
			SetColor(DATAIPAGE,IDC_DATAI_BBMOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,temp);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BBMOK,COLOR_RED);
			if (m_bBBMOk)
			{
				char temp2[256];
				strcpy(temp2,cBuff);
				strcat(temp2,temp);
				strcat(temp2,"\r\n");
				if (WriteToFileEx(127,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - BATTERY BACKED UP MEMORY ERROR (4e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, temp2);//specific failure
				AnyChange = true;
				FlagOthr(1,true);
			}

			m_bBBMOk = false;
			m_iDataI_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAIPAGE,IDC_DATAI_BBMOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,OK);
			if (!m_bBBMOk)
			{
				WriteToFileEx(127,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - NO BATTERY BACKED UP MEMORY ERROR (4e)\r\n");
				AnyChange = true;
			}

			m_bBBMOk = true;
		}
	
		//bit 2 startup
		if (Status1 & CWSTART) 
		{
			SetString(DATAIPAGE,IDC_DATAI_OPERATION,START_COLD);
			if (m_dCWStart != 0)
			{
				WriteToFileEx(128,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - COLD START (4e)\r\n");
				AnyChange = true;
			}

			m_dCWStart = 0;
		}
		else
		{
			SetString(DATAIPAGE,IDC_DATAI_OPERATION,START_NORMAL);
			if (m_dCWStart != 1)
			{
				WriteToFileEx(128,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - NORMAL START (4e)\r\n");
				AnyChange = true;
			}

			m_dCWStart = 1;

		}
	
		//bit 1 reserved

		//bit 0 mii
		if (Status1 & INMII)
		{
			m_pButton->ShowIcon_Bell(VARIANT_TRUE);
			SetString(DATAIPAGE,IDC_DATAI_OOMII,INMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,INMII_TXT);
			if (!m_bInMII && m_bLogMII)
			{
				WriteToFileEx(129,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - In MEASUREMENT INTERVAL OF INTEREST [BELL icon on] (4e)\r\n");
				AnyChange = true;
			}

			m_bInMII = true;
		}
		else
		{
			m_pButton->ShowIcon_Bell(VARIANT_FALSE);
			SetString(DATAIPAGE,IDC_DATAI_OOMII,NOTINMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,NOTINMII_TXT);
			if (m_bInMII && m_bLogMII)
			{
				WriteToFileEx(129,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - Out of MEASUREMENT INTERVAL OF INTEREST [BELL icon off] (4e)\r\n");
				AnyChange = true;
			}

			m_bInMII = false;
		}
	
		//bit 7 of sw 2
		if (Status2 & DATAFILTER_FLAG)
		{
			SetString(DATAIPAGE,IDC_DATAI_DATAFILTER,DATAFILTER_NO);
			if (!m_bDataFiltered && m_bLogFilter)
			{
				WriteToFileEx(130,TO_PFM, TYPE_INST, &Julian,
					"00000 STATUS CHANGE - DATA NOT FILTERED (4e)\r\n");
				AnyChange = true;
			}

			m_bDataFiltered = true;
		}
		else
		{
			SetString(DATAIPAGE,IDC_DATAI_DATAFILTER,DATAFILTER_YES);
			if (m_bDataFiltered && m_bLogFilter)
			{
				WriteToFileEx(130,TO_PFM, TYPE_INST, &Julian, 
					"00000 STATUS CHANGE - DATA FILTERED (4e)\r\n");
				AnyChange = true;
			}

			m_bDataFiltered = false;
		}

		if (m_pPropertySheet)
			m_pPropertySheet->ID3_Record(Record);

		//write to pfm file if there was a change in the status bits
		if (AnyChange)
		{
			char cBuff[1024];
			strcpy(cBuff,"00000 ");
			strcat(cBuff,Record);
			WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//any change to status
		}

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	
		//get bbm
		strncpy(temp,&Record[18],10);
		temp[10]=NULL;
		char *pCh = temp;
		while (*pCh == ' ')pCh++;
		SetString(DATAIPAGE,IDC_DATAI_BYTESMEM,pCh);
		unsigned long ulbbm;
		sscanf(temp,"%d",&ulbbm);
		m_pButton->put_Numeric(ulbbm);
	
		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,ulbbm);
	}
	PostMessage(IDC_RECEIVE_RESULT,MONITOR_ID3_RECORD,CheckSum);	
}

//record 45 
int  
CInstrument::OnBinaryTriggerConfigRecord(BYTE *pBinaryAt)
{
	//set a record pointer to the record data
	TriggerConfigureRecord* psBinaryRecord = (TriggerConfigureRecord*)pBinaryAt;
	//Reverse the appropriate fields
	Reverse((BYTE*)&(psBinaryRecord->uiJulianSeconds),sizeof(psBinaryRecord->uiJulianSeconds));
	//build the text version
	char Record[sizeof(TriggerConfigureTextRecord)+1];
	sprintf(Record,"%02X%10u"
		"%1X%02X%02X%02X%02X%02X%02X%02X%02X"
		"%1X%02X%02X%02X%02X%02X%02X%02X%02X"
		"%1X%02X%02X%02X%02X%02X%02X%02X%02X"
		"%1X%02X%02X%02X%02X%02X%02X%02X%02X",
		psBinaryRecord->bOpcode,					
		psBinaryRecord->uiJulianSeconds,				
		psBinaryRecord->bTriggerPin4Logic,			
		psBinaryRecord->bTriggerPin4ChanType[0],	
		psBinaryRecord->bTriggerPin4ChanType[1],	
		psBinaryRecord->bTriggerPin4ChanType[2],	
		psBinaryRecord->bTriggerPin4ChanType[3],	
		psBinaryRecord->bTriggerPin4ChanType[4],	
		psBinaryRecord->bTriggerPin4ChanType[5],	
		psBinaryRecord->bTriggerPin4ChanType[6],	
		psBinaryRecord->bTriggerPin4ChanType[7],	
		psBinaryRecord->bTriggerPin5Logic,			
		psBinaryRecord->bTriggerPin5ChanType[0],	
		psBinaryRecord->bTriggerPin5ChanType[1],	
		psBinaryRecord->bTriggerPin5ChanType[2],	
		psBinaryRecord->bTriggerPin5ChanType[3],	
		psBinaryRecord->bTriggerPin5ChanType[4],	
		psBinaryRecord->bTriggerPin5ChanType[5],	
		psBinaryRecord->bTriggerPin5ChanType[6],	
		psBinaryRecord->bTriggerPin5ChanType[7],	
		psBinaryRecord->bTriggerPin6Logic,			
		psBinaryRecord->bTriggerPin6ChanType[0],	
		psBinaryRecord->bTriggerPin6ChanType[1],	
		psBinaryRecord->bTriggerPin6ChanType[2],	
		psBinaryRecord->bTriggerPin6ChanType[3],	
		psBinaryRecord->bTriggerPin6ChanType[4],	
		psBinaryRecord->bTriggerPin6ChanType[5],	
		psBinaryRecord->bTriggerPin6ChanType[6],	
		psBinaryRecord->bTriggerPin6ChanType[7],	
		psBinaryRecord->bTriggerPin7Logic,			
		psBinaryRecord->bTriggerPin7ChanType[0],	
		psBinaryRecord->bTriggerPin7ChanType[1],	
		psBinaryRecord->bTriggerPin7ChanType[2],	
		psBinaryRecord->bTriggerPin7ChanType[3],	
		psBinaryRecord->bTriggerPin7ChanType[4],	
		psBinaryRecord->bTriggerPin7ChanType[5],	
		psBinaryRecord->bTriggerPin7ChanType[6],	
		psBinaryRecord->bTriggerPin7ChanType[7]);	

	//add a good checksum
	AppendCheckSum(Record);
	//send it to the summary page
	UpdateSummary(Record);
	//add the end of line stuff
	strcat(Record,"\r\n");

	//validate the formatting
	char cBuff[1024];
	if (strlen(Record) == TRIGGER_CONFIG_RECORD_LENGTH)
	{
		//send it to the watch window
		PostToWatch(Record);

		//send it to the text version handler
		OnTriggerConfigRecord(Record);
	}
	else
	{
		sprintf(cBuff,"00000 BINARY TRIGGER CONFIG RECORD FAILED FORMAT (Formatted): %s",Record);
		if (WriteToFileEx(135,TO_PFM,TYPE_COMP,NULL,cBuff))
		{
			sprintf(cBuff,"00000 BINARY TRIGGER CONFIG RECORD FAILED FORMAT (Source   ): ");
			char cTemp[1024];
			for (int iIndex = 0; iIndex < sizeof(TriggerConfigureRecord); iIndex++)
			{
				sprintf(cTemp,"%02x ",*(pBinaryAt+iIndex));
				strcat(cBuff,cTemp);		
			}
			strcat(cBuff,"\r\n");
			WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff);
		}
	}

	//tell caller how long it was
	return sizeof(TriggerConfigureRecord);
}

//record 45 (post monitor version 4 only)
void
CInstrument::OnTriggerConfigRecord(char *Record)
{

	int CheckSum;

	if (LengthOk(Record,TRIGGER_CONFIG_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{

		m_cTriggerConfigRecord = Record;

		bool bChange = false;
		bool bDoYellow = false;
	
		CString cFull(Record);
		CString cPart;
		CString cPart2;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);
		cPart = cjTime.Date();
		cPart += " ";
		cPart += cjTime.Time();
		CurrentSetParam(SET_TCR_TIME,"SET_TCR_TIME",cPart,true);
		SetString(USER4PAGE,IDC_USER4PROG_TIME,cPart);

		cPart = cFull.Mid(12,1);
		if (!CurrentSetParam(SET_TCR_TRIGGER4LOGIC,"SET_TCR_TRIGGER4LOGIC",cPart,false))
		{
			m_iUser4Prog_Tab |= 0x0002;
			bChange = true;
			bDoYellow = true;
			SetColor(USER4PAGE,IDC_USER4PROG_LOGIC1,COLOR_YELLOW);
			WriteToFileEx(174,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "00000 CONFIGURATION MISMATCH - TRIGGER_4_LOGIC (45)\r\n");
		}
		SetString(USER4PAGE,IDC_USER4PROG_LOGIC1,cPart);
		
		cPart = cFull.Mid(13,16);
		if (!CurrentSetParam(SET_TCR_TRIGGER4CHTYPE,"SET_TCR_TRIGGER4CHTYPE",cPart,false))
		{
			m_iUser4Prog_Tab |= 0x0002;
			bChange = true;
			bDoYellow = true;

			while (m_cSetParamMatchText.GetLength() < 16) m_cSetParamMatchText += ' ';
			WriteToFileEx(175,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "00000 CONFIGURATION MISMATCH - TRIGGER_4_CHAN_TYPE (45)\r\n");
		}
		else
			bDoYellow = false;

		cPart2 = cPart[0];
		cPart2 += "/";
		cPart2 += cPart[1];
		if (bDoYellow)
			if ((m_cSetParamMatchText[0] != cPart[0]) || (m_cSetParamMatchText[1] != cPart[1]))
				SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE1,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE1,cPart2);

		cPart2 = cPart[2];
		cPart2 += "/";
		cPart2 += cPart[3];
		if (bDoYellow)
			if ((m_cSetParamMatchText[2] != cPart[2]) || (m_cSetParamMatchText[3] != cPart[3]))
				SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE2,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE2,cPart2);

		cPart2 = cPart[4];
		cPart2 += "/";
		cPart2 += cPart[5];
		if (bDoYellow)
			if ((m_cSetParamMatchText[4] != cPart[4]) || (m_cSetParamMatchText[5] != cPart[5]))
				SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE3,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE3,cPart2);

		cPart2 = cPart[6];
		cPart2 += "/";
		cPart2 += cPart[7];
		if (bDoYellow)
			if ((m_cSetParamMatchText[6] != cPart[6]) || (m_cSetParamMatchText[7] != cPart[7]))
				SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE4,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE4,cPart2);

		cPart2 = cPart[8];
		cPart2 += "/";
		cPart2 += cPart[9];
		if (bDoYellow)
			if ((m_cSetParamMatchText[8] != cPart[8]) || (m_cSetParamMatchText[9] != cPart[9]))
				SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE5,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE5,cPart2);

		cPart2 = cPart[10];
		cPart2 += "/";
		cPart2 += cPart[11];
		if (bDoYellow)
			if ((m_cSetParamMatchText[10] != cPart[10]) || (m_cSetParamMatchText[11] != cPart[11]))
				SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE6,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE6,cPart2);

		cPart2 = cPart[12];
		cPart2 += "/";
		cPart2 += cPart[13];
		if (bDoYellow)
			if ((m_cSetParamMatchText[12] != cPart[12]) || (m_cSetParamMatchText[13] != cPart[13]))
				SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE7,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE7,cPart2);

		cPart2 = cPart[14];
		cPart2 += "/";
		cPart2 += cPart[15];
		if (bDoYellow)
			if ((m_cSetParamMatchText[14] != cPart[14]) || (m_cSetParamMatchText[15] != cPart[15]))
				SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE8,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE8,cPart2);
		
		cPart = cFull.Mid(29,1);
		if (!CurrentSetParam(SET_TCR_TRIGGER5LOGIC,"SET_TCR_TRIGGER5LOGIC",cPart,false))
		{
			m_iUser4Prog_Tab |= 0x0002;
			bChange = true;
			bDoYellow = true;
			SetColor(USER4PAGE,IDC_USER4PROG_LOGIC2,COLOR_YELLOW);
			WriteToFileEx(176,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "00000 CONFIGURATION MISMATCH - TRIGGER_5_LOGIC (45)\r\n");
		}
		else
			bDoYellow = false;
		SetString(USER4PAGE,IDC_USER4PROG_LOGIC2,cPart);
		
		cPart = cFull.Mid(30,16);
		if (!CurrentSetParam(SET_TCR_TRIGGER5CHTYPE,"SET_TCR_TRIGGER5CHTYPE",cPart,false))
		{
			m_iUser4Prog_Tab |= 0x0002;
			bChange = true;
			bDoYellow = true;

			while (m_cSetParamMatchText.GetLength() < 16) m_cSetParamMatchText += ' ';
			WriteToFileEx(177,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "00000 CONFIGURATION MISMATCH - TRIGGER_5_CHAN_TYPE (45)\r\n");
		}
		else
			bDoYellow = false;
		cPart2 = cPart[0];
		cPart2 += "/";
		cPart2 += cPart[1];
		if (bDoYellow)
			if ((m_cSetParamMatchText[0] != cPart[0]) || (m_cSetParamMatchText[1] != cPart[1]))
				SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE1,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE1,cPart2);

		cPart2 = cPart[2];
		cPart2 += "/";
		cPart2 += cPart[3];
		if (bDoYellow)
			if ((m_cSetParamMatchText[2] != cPart[2]) || (m_cSetParamMatchText[3] != cPart[3]))
				SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE2,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE2,cPart2);

		cPart2 = cPart[4];
		cPart2 += "/";
		cPart2 += cPart[5];
		if (bDoYellow)
			if ((m_cSetParamMatchText[4] != cPart[4]) || (m_cSetParamMatchText[5] != cPart[5]))
				SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE3,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE3,cPart2);

		cPart2 = cPart[6];
		cPart2 += "/";
		cPart2 += cPart[7];
		if (bDoYellow)
			if ((m_cSetParamMatchText[6] != cPart[6]) || (m_cSetParamMatchText[7] != cPart[7]))
				SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE4,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE4,cPart2);

		cPart2 = cPart[8];
		cPart2 += "/";
		cPart2 += cPart[9];
		if (bDoYellow)
			if ((m_cSetParamMatchText[8] != cPart[8]) || (m_cSetParamMatchText[9] != cPart[9]))
				SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE5,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE5,cPart2);

		cPart2 = cPart[10];
		cPart2 += "/";
		cPart2 += cPart[11];
		if (bDoYellow)
			if ((m_cSetParamMatchText[10] != cPart[10]) || (m_cSetParamMatchText[11] != cPart[11]))
				SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE6,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE6,cPart2);

		cPart2 = cPart[12];
		cPart2 += "/";
		cPart2 += cPart[13];
		if (bDoYellow)
			if ((m_cSetParamMatchText[12] != cPart[12]) || (m_cSetParamMatchText[13] != cPart[13]))
				SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE7,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE7,cPart2);

		cPart2 = cPart[14];
		cPart2 += "/";
		cPart2 += cPart[15];
		if (bDoYellow)
			if ((m_cSetParamMatchText[14] != cPart[14]) || (m_cSetParamMatchText[15] != cPart[15]))
				SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE8,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE8,cPart2);
		
		cPart = cFull.Mid(46,1);
		if (!CurrentSetParam(SET_TCR_TRIGGER6LOGIC,"SET_TCR_TRIGGER6LOGIC",cPart,false))
		{
			m_iUser4Prog_Tab |= 0x0002;
			bChange = true;
			bDoYellow = true;
			SetColor(USER4PAGE,IDC_USER4PROG_LOGIC3,COLOR_YELLOW);
			WriteToFileEx(178,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "00000 CONFIGURATION MISMATCH - TRIGGER_6_LOGIC (45)\r\n");
		}
		else
			bDoYellow = false;
		SetString(USER4PAGE,IDC_USER4PROG_LOGIC3,cPart);
		
		cPart = cFull.Mid(47,16);
		if (!CurrentSetParam(SET_TCR_TRIGGER6CHTYPE,"SET_TCR_TRIGGER6CHTYPE",cPart,false))
		{
			m_iUser4Prog_Tab |= 0x0002;
			bChange = true;
			bDoYellow = true;
			while (m_cSetParamMatchText.GetLength() < 16) m_cSetParamMatchText += ' ';
			WriteToFileEx(179,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "00000 CONFIGURATION MISMATCH - TRIGGER_6_CHAN_TYPE (45)\r\n");
		}
		else
			bDoYellow = false;
		cPart2 = cPart[0];
		cPart2 += "/";
		cPart2 += cPart[1];
		if (bDoYellow)
			if ((m_cSetParamMatchText[0] != cPart[0]) || (m_cSetParamMatchText[1] != cPart[1]))
				SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE1,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE1,cPart2);

		cPart2 = cPart[2];
		cPart2 += "/";
		cPart2 += cPart[3];
		if (bDoYellow)
			if ((m_cSetParamMatchText[2] != cPart[2]) || (m_cSetParamMatchText[3] != cPart[3]))
				SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE2,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE2,cPart2);

		cPart2 = cPart[4];
		cPart2 += "/";
		cPart2 += cPart[5];
		if (bDoYellow)
			if ((m_cSetParamMatchText[4] != cPart[4]) || (m_cSetParamMatchText[5] != cPart[5]))
				SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE3,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE3,cPart2);

		cPart2 = cPart[6];
		cPart2 += "/";
		cPart2 += cPart[7];
		if (bDoYellow)
			if ((m_cSetParamMatchText[6] != cPart[6]) || (m_cSetParamMatchText[7] != cPart[7]))
				SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE4,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE4,cPart2);

		cPart2 = cPart[8];
		cPart2 += "/";
		cPart2 += cPart[9];
		if (bDoYellow)
			if ((m_cSetParamMatchText[8] != cPart[8]) || (m_cSetParamMatchText[9] != cPart[9]))
				SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE5,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE5,cPart2);

		cPart2 = cPart[10];
		cPart2 += "/";
		cPart2 += cPart[11];
		if (bDoYellow)
			if ((m_cSetParamMatchText[10] != cPart[10]) || (m_cSetParamMatchText[11] != cPart[11]))
				SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE6,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE6,cPart2);
		
		cPart2 = cPart[12];
		cPart2 += "/";
		cPart2 += cPart[13];
		if (bDoYellow)
			if ((m_cSetParamMatchText[12] != cPart[12]) || (m_cSetParamMatchText[13] != cPart[13]))
				SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE7,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE7,cPart2);

		cPart2 = cPart[14];
		cPart2 += "/";
		cPart2 += cPart[15];
		if (bDoYellow)
			if ((m_cSetParamMatchText[14] != cPart[14]) || (m_cSetParamMatchText[15] != cPart[15]))
				SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE8,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE8,cPart2);
		
		cPart = cFull.Mid(63,1);
		if (!CurrentSetParam(SET_TCR_TRIGGER7LOGIC,"SET_TCR_TRIGGER7LOGIC",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			m_iUser4Prog_Tab |= 0x0002;
			SetColor(USER4PAGE,IDC_USER4PROG_LOGIC4,COLOR_YELLOW);
			WriteToFileEx(180,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "00000 CONFIGURATION MISMATCH - TRIGGER_7_LOGIC (45)\r\n");
		}
		else
			bDoYellow = false;
		SetString(USER4PAGE,IDC_USER4PROG_LOGIC4,cPart);
		
		cPart = cFull.Mid(64,16);
		if (!CurrentSetParam(SET_TCR_TRIGGER7CHTYPE,"SET_TCR_TRIGGER7CHTYPE",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			m_iUser4Prog_Tab |= 0x0002;
			while (m_cSetParamMatchText.GetLength() < 16) m_cSetParamMatchText += ' ';
			WriteToFileEx(181,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "00000 CONFIGURATION MISMATCH - TRIGGER_7_CHAN_TYPE (45)\r\n");
		}
		else
			bDoYellow = false;
		cPart2 = cPart[0];
		cPart2 += "/";
		cPart2 += cPart[1];
		if (bDoYellow)
			if ((m_cSetParamMatchText[0] != cPart[0]) || (m_cSetParamMatchText[1] != cPart[1]))
				SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE1,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE1,cPart2);

		cPart2 = cPart[2];
		cPart2 += "/";
		cPart2 += cPart[3];
		if (bDoYellow)
			if ((m_cSetParamMatchText[2] != cPart[2]) || (m_cSetParamMatchText[3] != cPart[3]))
				SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE2,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE2,cPart2);

		cPart2 = cPart[4];
		cPart2 += "/";
		cPart2 += cPart[5];
		if (bDoYellow)
			if ((m_cSetParamMatchText[4] != cPart[4]) || (m_cSetParamMatchText[5] != cPart[5]))
				SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE3,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE3,cPart2);

		cPart2 = cPart[6];
		cPart2 += "/";
		cPart2 += cPart[7];
		if (bDoYellow)
			if ((m_cSetParamMatchText[6] != cPart[6]) || (m_cSetParamMatchText[7] != cPart[7]))
				SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE4,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE4,cPart2);

		cPart2 = cPart[8];
		cPart2 += "/";
		cPart2 += cPart[9];
		if (bDoYellow)
			if ((m_cSetParamMatchText[8] != cPart[8]) || (m_cSetParamMatchText[9] != cPart[9]))
				SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE5,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE5,cPart2);

		cPart2 = cPart[10];
		cPart2 += "/";
		cPart2 += cPart[11];
		if (bDoYellow)
			if ((m_cSetParamMatchText[10] != cPart[10]) || (m_cSetParamMatchText[11] != cPart[11]))
				SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE6,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE6,cPart2);

		cPart2 = cPart[12];
		cPart2 += "/";
		cPart2 += cPart[13];
		if (bDoYellow)
			if ((m_cSetParamMatchText[12] != cPart[12]) || (m_cSetParamMatchText[13] != cPart[13]))
				SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE7,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE7,cPart2);

		cPart2 = cPart[14];
		cPart2 += "/";
		cPart2 += cPart[15];
		if (bDoYellow)
			if ((m_cSetParamMatchText[14] != cPart[14]) || (m_cSetParamMatchText[15] != cPart[15]))
				SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE8,COLOR_YELLOW);
		SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE8,cPart2);

		if (m_pPropertySheet)
			m_pPropertySheet->TriggerConfigRecord(Record);

		char cBuff[1024];
		strcpy(cBuff,"00000 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);
	}
	PostMessage(IDC_RECEIVE_RESULT,TRIGGER_CONFIG_RECORD,CheckSum);	
}

int 
CInstrument::Checksum(char* Record)
{
    unsigned short nbytes;	/* # bytes to include in checksum calculation */
    unsigned short sum;		/* sum of all bytes in checksum */
	unsigned short rec_sum;
    /* checksum is a one byte sum of all bytes up to the checksum byte */
	sum = 0;
    nbytes = (unsigned short)(strlen(Record)-4);//don't count lf/cr nor checksum
	//adjust for authentication in acquire record
	if (strncmp(ACQUIRE_RECORD_CHAR,Record,2)==0) 
	{
		//and cause a checksum error if wrong length received
		nbytes -= 2;//adjust for acquire record's auth byte
	}

	//don't bother with checking checksum if length is wrong
	for (int i=0; i<nbytes; i++)
		sum = (unsigned short)(sum + Record[i]);
	sum &= 0x00ff;	/* keep only relevant part of checksum */
	sscanf(&Record[nbytes],"%2hx",&rec_sum);
	if (rec_sum == sum)
		/* calculated checksum matches checksum in record */
		return CHECKSUM_GOOD;

////	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
////	CDataStat* datapage    = m_pParent->m_pDataStatus;
////	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
////	CModParam* modparpage  = m_pParent->m_pModParams;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("CheckSum: %d/%H:%M:%S ");
	DateTime += Record;
//	if (GetPageIndex(generalpage) >= 0)
//	{
//		SetString(GENERALPAGE,IDC_GENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
//		SetColor(GENERALPAGE,IDC_GENERAL_CHECKSUM,COLOR_RED);
//		m_iGeneral_Tab |= 0x0001;
//		SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
//	}
//	if (GetPageIndex(general4page) >= 0)
//	{
///		SetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,(char*)LPCTSTR(DateTime));
///		SetColor(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,COLOR_RED);
///		m_iGeneral4_Tab |= 0x0001;

		SetString(DATAIPAGE,IDC_DATAI_CHECKSUM,(char*)LPCTSTR(DateTime));
		SetColor(DATAIPAGE,IDC_DATAI_CHECKSUM,COLOR_RED);
		m_iDataI_Tab |= 0x0001;
//		if (m_iGeneral4_Tab == 0x0003)
//			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
//		else if (m_iGeneral4_Tab == 0x0002)
//			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
//		else if (m_iGeneral4_Tab == 0x0001)
//			SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
//		else if (m_iGeneral4_Tab == 0x0000)
//			SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
//	}

	WriteToFile(TO_PFM, TYPE_COMP, NULL, "00000 CHECKSUM ERROR\r\n");
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

	if (m_bDoDump)
	{
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "00000 CHECKSUM ERROR\r\n");
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
	}

    return CHECKSUM_BAD;	       /* checksums do not match */
}

bool 
CInstrument::LengthOk(char* Record, int length)
{
	//Check the length of the record	
	if (strlen(Record) == (unsigned)length)
		return true;

	//must have failed the length check

////	CSummary*  summarypage = m_pParent->m_pSummaryPage;
////	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
////	CDataStat* datapage    = m_pParent->m_pDataStatus;
////	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
////	CModParam* modparpage  = m_pParent->m_pModParams;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("Length: %d/%H:%M:%S\n");
	DateTime += Record;
	DateTime = DateTime.Left(DateTime.GetLength()-2);

//	SetString(GENERALPAGE,IDC_GENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
//	SetColor(GENERALPAGE,IDC_GENERAL_CHECKSUM,COLOR_RED);
//	m_iGeneral_Tab |= 0x0001;

///	SetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,(char*)LPCTSTR(DateTime));
///	SetColor(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,COLOR_RED);
	SetString(DATAIPAGE,IDC_DATAI_CHECKSUM,(char*)LPCTSTR(DateTime));
	SetColor(DATAIPAGE,IDC_DATAI_CHECKSUM,COLOR_RED);
///	m_iGeneral4_Tab |= 0x0001;
	m_iDataI_Tab |= 0x0001;

//	generalpage->GetDlgItem(IDC_GENERAL_CHECKSUM)->Invalidate();
////	m_pParent->FlagComm(1,false);			
//	SetTabColor(IDC_GENERAL_TAB,COLOR_RED);

	WriteToFile(TO_PFM, TYPE_COMP, NULL, "00000 LENGTH ERROR\r\n");
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

	if (m_bDoDump)
	{
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "00000 LENGTH ERROR in message\r\n");
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
	}
//debug to test ILON problem -- force pause on length error
//	AfxGetMainWnd()->PostMessage(IDC_FORCE_PAUSE, NULL, NULL);
//end debug to test ILON problem
    return false;
}

//this function should go into the parent class
void 
CInstrument::BuildFileName(CString &cResult,
	CString cFormat,CString cPath,CString cType,
	CString cID,
	int iYear,	int iMonth,	int iDay,
	int iHour,	int iMinute,int iSecond,
	CString cSuffix
	)
{
	cResult.Empty();
	if (cPath.GetLength())
	{
		cResult = cPath;
		if (cResult.Right(1) != '\\')
			cResult += '\\';
	}
	CString cTemp;
	bool bHaveEscape = false;
	for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
	{
		if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
			bHaveEscape = true;
		else if (bHaveEscape)
		{
			bHaveEscape = false;
			switch (cFormat[iSourceIndex]) {
			case 'T':
				cResult += cType;
				break;
			case 'I':
				{
					int iLength = cID.GetLength();
					while (iLength < 3)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'i':
				{
					int iLength = cID.GetLength();
					while (iLength < 2)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'y':
				cTemp.Format("%04d",iYear);
				cResult += cTemp;
				break;
			case 'm':
				cTemp.Format("%02d",iMonth);
				cResult += cTemp;
				break;
			case 'd':
				cTemp.Format("%02d",iDay);
				cResult += cTemp;
				break;
			case 'H':
				cTemp.Format("%02d",iHour);
				cResult += cTemp;
				break;
			case 'M':
				cTemp.Format("%02d",iMinute);
				cResult += cTemp;
				break;
			case 'S':
				cTemp.Format("%02d",iSecond);
				cResult += cTemp;
				break;
			default:
				cResult += cFormat[iSourceIndex];
			}
		}
		else
			cResult += cFormat[iSourceIndex];
	}
	if (cSuffix.GetLength())
	{
		if ((cResult.Right(1) != '.') && (cSuffix.Left(1) != '.'))
			cResult += '.';
		cResult += cSuffix;
	}
}

bool CInstrument::WriteToMGDFileX(AcquireRecordX* AcquireRec, short AcquireCS)
{
//TRACE("CInstrument::WriteToMGDFileX\r\n");
	FILE *File;
	fpos_t position;
	struct AcquireHeader AcquireHead;
//	CGenlStat* generalpage;
//	CGenl4Stat* general4page;
	if (m_bBlockWrite) return false;

	//could qualify to only main board and env sensor board here
	//but if we don't then we can add other binary messages to the
	//MGD file
	//AcquireRec->bLength;
	//AcquireRec->bOpcode;
	CJulianTime Time(AcquireRec->uiJulianSeconds);

	//build filename
	if (m_bUseShortFilename)
	{
		sprintf(m_szCurrentBIDFileName,"%s\\%s%c%c%c%c%02d.%3s",
				m_szSaveLoc,	
				m_pID,
				((Time.Year()-1990)<10)?
					((Time.Year()-1990<0)?'#':'0'+(Time.Year()-1990)):
					'A'+(Time.Year()-2000),
	////			'0' + (Time->Year() % 10),
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,MGD_SUFFIX);
	}
	else
	{
		CString cResult;
		BuildFileName(cResult,m_csLongNameFormatString,m_szSaveLoc,"uG",
			m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,MGD_SUFFIX);
		strcpy(m_szCurrentBIDFileName,cResult);
	}

	//is the file there
	// '0' checks for existance only
	if (access(m_szCurrentBIDFileName,0) != 0)
	{
////		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "00000 ACCESS 1 FAILED\r\n");
		//make sure path exists
		MakeMyPath(m_szCurrentBIDFileName);

		//build archive file name
		CString ArchiveName;
		if (m_bUseShortFilename)
		{
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				m_szSaveLoc,	
				m_pID,
				((Time.Year()-1990)<10)?
					((Time.Year()-1990<0)?'#':'0'+(Time.Year()-1990)):
					'A'+(Time.Year()-2000),
	////			'0' + (Time->Year()%10),
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,MGD_SUFFIX);
		}
		else
		{
			CString cTemp;
			cTemp = m_szSaveLoc;
			cTemp += "\\archive\\";
			BuildFileName(ArchiveName,m_csLongNameFormatString,cTemp,"uG",
				m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,MGD_SUFFIX);
		}
		//if the file is in the archive directory
		if (access(ArchiveName,0)==0)
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentBIDFileName,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	}

	int Attempts = 5;
	File = NULL;
	while ((Attempts > 0) && (File == NULL))
	{
		File = fopen(m_szCurrentBIDFileName,"ab+");
		if (File == NULL)
		{
			Sleep(200);
		}
		Attempts--;
	}

	if (File != NULL)
	{
		//get file position
		fseek(File,0,SEEK_END);

		if (fgetpos( File, &position )==0)
		{
			//if new file (e.g. no header there yet)
			if (position == 0)
			{
				//initialize header
				char temp[32];
				memset( &AcquireHead, ' ', sizeof(AcquireHeader));

				//fill header
				//size of header beyond first four bytes
				sprintf(temp,"%4d",sizeof(AcquireHeader)-4);
				memcpy(AcquireHead.SizeOfHeader,temp,4);

				//Fix for new version number formatting inside of binary file
				//not converting to a float for the review software.
				//Incompatible format for binary file came out "2.0.0".
				//Compatible format is " 2000".
				//Prior to 2.0.0.0, the version number was formatted "1.907".
				//This code reformats the newer version format to look like the old.
				//SK uses a float to read the value from the binary file.
				//Fix provided by DP and incorporated by PJM 8/5/2005
				//
				//sprintf(temp,"%5s",m_pParent->m_csVersion);
				short sA,sB,sC,sD;
				sscanf(m_csVersion,"%d.%d.%d.%d",&sA,&sB,&sC,&sD);
				sprintf(temp,"%2d%1d%1d%1d",sA,sB,sC,sD);  //no decimal point
				//End fix of 8/5/2005

				memcpy(AcquireHead.Version,temp,5);
				sprintf(temp,"%3s",m_szCurrentID);
				memcpy(AcquireHead.StationId,temp,3);
				sprintf(temp,"%3d",Time.Year()%100);
				memcpy(AcquireHead.Year,temp,3);
				sprintf(temp,"%3d",Time.Month());
				memcpy(AcquireHead.Month,temp,3);
				sprintf(temp,"%3d",Time.Day());
				memcpy(AcquireHead.Day,temp,3);
				sprintf(temp,"%4d",Time.Year());
				memcpy(AcquireHead.unused2,temp,4);
				//write header
				fwrite(&AcquireHead,sizeof(AcquireHeader),1,File);
			}
		}
		
		//send record to the BID file
		int iWriteResult;
		if ((iWriteResult = fwrite(AcquireRec,AcquireRec->bLength,1,File)) != 1)
		{
			//if failed it could be because MicXfer is trying to
			//clear the archive bit, so wait a short time and try
			//again.
			Sleep(100);
			if ((iWriteResult = fwrite(AcquireRec,AcquireRec->bLength,1,File)) != 1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(200);
				if ((iWriteResult = fwrite(AcquireRec,AcquireRec->bLength,1,File)) != 1)
				{
					//if failed on write then show a file write problem
					//on the instrument's button
					m_pButton->SetDiskIcon_Floppy();
					m_pButton->ShowIcon_Disk(VARIANT_TRUE);
					m_pButton->put_Flash(VARIANT_TRUE);
					//write the error to the pfm and cev files
					WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "00000 Could not write to MGD file [DISK icon on] \r\n");
					//close the file anyway
					fclose(File);
					//save the fact that we couildn't so that we can tell PFM/CEV when good
					m_bCouldNotOpenBinaryFile = true;
					//return write failure 
					return false;
				}
			}
		}

		//if successful write
		if (iWriteResult == 1)
		{
			//calculate checksum
			unsigned short usChecksum = 0;
			for (int i = 0; i < AcquireRec->bLength-2; i++)
				usChecksum += AcquireRec->bBytes[i];
			//write checksum to file
			fwrite(&usChecksum,sizeof(short),1,File);
			//write BEEF to file
			usChecksum = 0xbeef;
			fwrite(&usChecksum,sizeof(short),1,File);

			//if at least one open failure
			if (m_bCouldNotOpenBinaryFile)
			{
				m_bCouldNotOpenBinaryFile = false;
				WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "00000 Successful write to MGD file after multiple attempts\r\n");
			}
		}

		//close the bid file
		fclose(File);
		//return good write
		return true;
	}
	else
	{
		//save the fact that we couildn't so that we can tell PFM/CEV when good
		m_bCouldNotOpenBinaryFile = true;
		//couldn't open the file so...
		//show a file write problem
		//on the instrument's button
		m_pButton->ShowIcon_Disk(VARIANT_TRUE);
		m_pButton->put_Flash(VARIANT_TRUE);
		WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "00000 Could not open MGD file [DISK icon on] \r\n");
	}
	return false;
}

bool CInstrument::WriteToSOHFileX(AcquireRecordX* AcquireRec)
{
//TRACE("CInstrument::WriteToSOHFileX\r\n");
	FILE *File;
	fpos_t position;
	struct AcquireHeader AcquireHead;
//	CGenlStat* generalpage;
//	CGenl4Stat* general4page;
	if (m_bBlockWrite) return false;

	//could qualify to only main board and env sensor board here
	//but if we don't then we can add other binary messages to the
	CJulianTime Time(AcquireRec->uiJulianSeconds);

	//build filename
	if (m_bUseShortFilename)
	{
		sprintf(m_szCurrentSOHFileName,"%s\\%s%c%c%c%c%02d.%3s",
				m_szSaveLoc,	
				m_pID,
				((Time.Year()-1990)<10)?
					((Time.Year()-1990<0)?'#':'0'+(Time.Year()-1990)):
					'A'+(Time.Year()-2000),
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,"SOH");
	}
	else
	{
		CString cResult;
		BuildFileName(cResult,m_csLongNameFormatString,m_szSaveLoc,"uG",
			m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,"SOH");
		strcpy(m_szCurrentSOHFileName,cResult);
	}

	//is the file there
	// '0' checks for existance only
	if (access(m_szCurrentSOHFileName,0) != 0)
	{
		//make sure path exists
		MakeMyPath(m_szCurrentSOHFileName);

		//build archive file name
		CString ArchiveName;
		if (m_bUseShortFilename)
		{
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				m_szSaveLoc,	
				m_pID,
				((Time.Year()-1990)<10)?
					((Time.Year()-1990<0)?'#':'0'+(Time.Year()-1990)):
					'A'+(Time.Year()-2000),
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,"SOH");
		}
		else
		{
			CString cTemp;
			cTemp = m_szSaveLoc;
			cTemp += "\\archive\\";
			BuildFileName(ArchiveName,m_csLongNameFormatString,cTemp,"uG",
				m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,"SOH");
		}
		//if the file is in the archive directory
		if (access(ArchiveName,0)==0)
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentSOHFileName,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	}

	int Attempts = 5;
	File = NULL;
	while ((Attempts > 0) && (File == NULL))
	{
		File = fopen(m_szCurrentSOHFileName,"ab+");
		if (File == NULL)
		{
			Sleep(200);
		}
		Attempts--;
	}

	if (File != NULL)
	{
		//get file position
		fseek(File,0,SEEK_END);

		if (fgetpos( File, &position )==0)
		{
			//if new file (e.g. no header there yet)
			if (position == 0)
			{
				//initialize header
				char temp[32];
				memset( &AcquireHead, ' ', sizeof(AcquireHeader));

				//fill header
				//size of header beyond first four bytes
				sprintf(temp,"%4d",sizeof(AcquireHeader)-4);
				memcpy(AcquireHead.SizeOfHeader,temp,4);

				//Fix for new version number formatting inside of binary file
				//not converting to a float for the review software.
				//Incompatible format for binary file came out "2.0.0".
				//Compatible format is " 2000".
				//Prior to 2.0.0.0, the version number was formatted "1.907".
				//This code reformats the newer version format to look like the old.
				//SK uses a float to read the value from the binary file.
				//Fix provided by DP and incorporated by PJM 8/5/2005
				//
				//sprintf(temp,"%5s",m_pParent->m_csVersion);
				short sA,sB,sC,sD;
				sscanf(m_csVersion,"%d.%d.%d.%d",&sA,&sB,&sC,&sD);
				sprintf(temp,"%2d%1d%1d%1d",sA,sB,sC,sD);  //no decimal point
				//End fix of 8/5/2005

				memcpy(AcquireHead.Version,temp,5);
				sprintf(temp,"%3s",m_szCurrentID);
				memcpy(AcquireHead.StationId,temp,3);
				sprintf(temp,"%3d",Time.Year()%100);
				memcpy(AcquireHead.Year,temp,3);
				sprintf(temp,"%3d",Time.Month());
				memcpy(AcquireHead.Month,temp,3);
				sprintf(temp,"%3d",Time.Day());
				memcpy(AcquireHead.Day,temp,3);
				sprintf(temp,"%4d",Time.Year());
				memcpy(AcquireHead.unused2,temp,4);
				AcquireHead.unused2[46] = SOH_MGD;
				//write header
				fwrite(&AcquireHead,sizeof(AcquireHeader),1,File);

			}
		}
		
		//send record to the SOH file
		int iWriteResult;
		if ((iWriteResult = fwrite(AcquireRec,AcquireRec->bLength,1,File)) != 1)
		{
			//if failed it could be because MicXfer is trying to
			//clear the archive bit, so wait a short time and try
			//again.
			Sleep(100);
			if ((iWriteResult = fwrite(AcquireRec,AcquireRec->bLength,1,File)) != 1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(200);
				if ((iWriteResult = fwrite(AcquireRec,AcquireRec->bLength,1,File)) != 1)
				{
					//if failed on write then show a file write problem
					//on the instrument's button
					m_pButton->SetDiskIcon_Floppy();
					m_pButton->ShowIcon_Disk(VARIANT_TRUE);
					m_pButton->put_Flash(VARIANT_TRUE);
					//write the error to the pfm and cev files
					WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "00000 Could not write to SOH file [DISK icon on] \r\n");
					//close the file anyway
					fclose(File);
					//save the fact that we couildn't so that we can tell PFM/CEV when good
					m_bCouldNotOpenBinaryFile = true;
					//return write failure 
					return false;
				}
			}
		}
	
		//if successful write
		if (iWriteResult == 1)
		{
			//calculate checksum
			unsigned short usChecksum = 0;
			for (int i = 0; i < AcquireRec->bLength-2; i++)
				usChecksum += AcquireRec->bBytes[i];
			//write checksum to file
			fwrite(&usChecksum,sizeof(short),1,File);
			//write BEEF to file
			usChecksum = 0xbeef;
			fwrite(&usChecksum,sizeof(short),1,File);
			//if at least one open failure
			if (m_bCouldNotOpenBinaryFile)
			{
				m_bCouldNotOpenBinaryFile = false;
				WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "00000 Successful write to SOH file after multiple attempts\r\n");
			}
		}
		//close the bid file
		fclose(File);
		//return good write
		return true;
	}
	else
	{
		//save the fact that we couildn't so that we can tell PFM/CEV when good
		m_bCouldNotOpenBinaryFile = true;
		//couldn't open the file so...
		//show a file write problem
		//on the instrument's button
		m_pButton->ShowIcon_Disk(VARIANT_TRUE);
		m_pButton->put_Flash(VARIANT_TRUE);
		WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "00000 Could not open SOH file [DISK icon on] \r\n");
	}
	return false;
}

void CInstrument::ReConfig()
{
	//reset any item that may have been changed by the "config dialog"

	//reset all ofthe max message repetitions
	for (int i = 0; i < sizeof(m_iMessageLimiter)/sizeof(int);i++)
		m_iMessageLimiter[i] = m_iMaxRepetition;
}

bool CInstrument::WriteToFileEx(
	unsigned uiIndex, int ToWhom, int Type, CJulianTime* Time, char* Msg, bool bStale)
{
	bool bReturn = false;
	if (uiIndex < sizeof(m_iMessageLimiter)/sizeof(int))
	{
		if (m_iMessageLimiter[uiIndex] > 1)
		{
			WriteToFile(ToWhom, Type, Time, Msg);
			bReturn = true;
		}
		else if (m_iMessageLimiter[uiIndex] == 1)
		{
			//strip off right \r\n if we can
			CString cNewMsg(Msg);
			cNewMsg.TrimRight("\r\n");
			cNewMsg += " (further occurrences not included)\r\n";
			WriteToFile(ToWhom, Type, Time, (LPSTR)LPCSTR(cNewMsg), bStale);
			bReturn = true;
		}

		m_iMessageLimiter[uiIndex]--;
		if (m_iMessageLimiter[uiIndex] < 0)
			m_iMessageLimiter[uiIndex] = 0;
	}
	return bReturn;
}

void CInstrument::WriteToFile(int ToWhom, int Type, CJulianTime* Time, char* Msg, bool bStale)
{
	FILE *File;
	char Message[2048];
	fpos_t position;
	Message[0] = NULL;
	COleDateTime TC = COleDateTime::GetCurrentTime(); 		//computer time
	COleDateTime TG;
	COleDateTimeSpan TS;
	CString ArchiveFileName;

	if (m_bBlockWrite) 
		return;

	//EstablishVersionString();

	if (Msg)
	{
		unsigned uLength = strlen(Msg);
		if (uLength >= 2)
		{
			if ((Msg[uLength-1] == 0x0d) && (Msg[uLength-2] == 0x0a))
			{
				Msg[uLength-1] = 0x0a;
				Msg[uLength-2] = 0x0d;
			}
		}
	}

	//default to the provided Ugrand time
	if (Time)
	{
		int Year,Month,Day,Hour,Minute,Second;
		Year = Time->Year();
		Month = Time->Month();
		Day = Time->Day();
		Hour = Time->Hour();
		Minute = Time->Minute();
		Second = Time->Second();
		if (Year	< 100)  Year	= 1900;//COleDateTime limits year 100-9999
		if (Year    > 9999) Year    = 9999;//COleDateTime limits year 100-9999
		if (Month	< 1)	Month	= 1;
		if (Day		< 1)	Day		= 1;
		if (Hour	< 0)	Hour	= 0;
		if (Minute	< 0)	Minute	= 0;
		if (Second	< 0)	Second	= 0;
		TG = COleDateTime(Year, Month, Day, Hour, Minute, Second);
	}
	//if that wasn't available then get the computer time
	//this is the case in all TYPE_COMP messages
	else
	{
		TG = COleDateTime::GetCurrentTime();
	}

	if (ToWhom == TO_DMP)
	{
		if (m_bUseShortFilename)
		{
			//	build new file name and save it
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_szSaveLoc,	
				m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
	////			'0' + (TC.GetYear()%10),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,m_csLongNameFormatString,m_szSaveLoc,"uG",
				m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,".");
			strcpy(m_szCurrentFileName,cResult);
		}
	}
	else
	{
		if (m_bUseShortFilename)
		{
			//	build new file name and save it
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_szSaveLoc,	
				m_pID,
				((TG.GetYear()-1990)<10)?
					((TG.GetYear()-1990<0)?'#':'0'+(TG.GetYear()-1990)):
					'A'+(TG.GetYear()-2000),
////				'0' + (TG.Year() % 10),
				((TG.GetMonth()<10)?
					'0'+(TG.GetMonth()):
					'A'+(TG.GetMonth()-10)),
				((TG.GetDay()  <10)?
					'0'+(TG.GetDay()):
					'A'+(TG.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,m_csLongNameFormatString,m_szSaveLoc,"uG",
				m_pID,TG.GetYear(),TG.GetMonth(),TG.GetDay(),0,0,0,".");
			strcpy(m_szCurrentFileName,cResult);
		}
	}	

	//format string to send
	switch (Type){

	case TYPE_DUMP:
		sprintf(Message,"%4d.%02d.%02d %02d:%02d:%02d %s",
			TC.GetYear(),TC.GetMonth(),TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),
			Msg);
//		strcpy(Message,Msg);
		break;

	case TYPE_INST:
		//	message = Time::YY.MM.DD HH:MM:SS G (MESSAGE==NULL)?\r:MESSAGE

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d G %s",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"\r\n":Msg);
		break;

	case TYPE_COMP:
		//	message = MICTIME::YY.MM.DD HH:MM:SS C (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C %s",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),Msg[0]==NULL?"\r\n":Msg);
		break;

	case TYPE_GID2:
	case TYPE_TIME:
		//	message = Time::YY.MM.DD HH:MM:SS UGrand Time   "+
		//					"YY.MM.DD HH:MM:SS Computer Time   C - G = xxxx\r"
		//computer time
		//Ugrand time
		TS = TC - TG;  // Subtract 2 COleDateTimes
		TS += HALF_SECOND;
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d G 00000 µGRAND Time %04d.%02d.%02d %02d:%02d:%02d C 727 Computer Time   C - G = %.0f seconds\r\n",
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			TS.GetTotalSeconds());
		break;

	case TYPE_INVTIME:
		//	message = "INVALID TIME  "+
		//		"Previous Record Time Saved::YY.MM.DD HH:MM:SS "+
		//		"Current Record Time Time::YY.MM.DD HH:MM:SS\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 00000 INVALID TIME %04d.%02d.%02d %02d:%02d:%02d\r\n",
			((CJulianTime*)Msg)->Year(),	((CJulianTime*)Msg)->Month(),	((CJulianTime*)Msg)->Day(),
			((CJulianTime*)Msg)->Hour(),		((CJulianTime*)Msg)->Minute(),	((CJulianTime*)Msg)->Second(),
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond());
		break;

	case TYPE_START:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C uGRAND COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 00000 µGRAND COLLECT Version %s started\r\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_csVersion);
		break;

	case TYPE_ABNORMAL:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C uGRAND COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 00000 µGRAND COLLECT Version %s started from abnormal shutdown\r\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_csVersion);
		break;

	case TYPE_DELETE:
	//	message = MICNOW::YY.MM.DD HH:MM:SS C file %s<MESSAGE> deleted\r"

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 00000 File %s deleted\r\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
		break;

	//just in case
	default:
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 00000 Unknown TYPE %s\r\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
	}

	//if to dmp do the write to todays file and get out
	if (ToWhom == TO_DMP)
	{
		//	open filename+dmp
		CString fn(m_szCurrentFileName);
		fn += DMP_SUFFIX;
////		CString ArchiveFileNameEx = ArchiveFileName + DMP_SUFFIX;
		if (access(fn,0) != 0)
		{
////			if (access(ArchiveFileNameEx,0)!=-1)
////				MoveFileEx(ArchiveFileNameEx,fn,
////					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			//make sure path exists
			MakeMyPath(fn);
		}

		if ((File = fopen(fn,"ab+")) != NULL)
		{
			//	send string
			fprintf(File,Message);
			//	close cev
			fclose(File);
		}
		return;
	}

	//if to cev
	if (ToWhom & TO_CEV)
	{
		//	open filename+cev
		CString fn(m_szCurrentFileName);
		fn += CEV_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);
			
			if (m_bUseShortFilename)
			{
				//build archive path\name
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_szSaveLoc,
				m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
////				'0' + (TC.GetYear() % 10),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,CEV_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,m_csLongNameFormatString,cTemp,"uG",
					m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,CEV_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"ab+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos(File,&position);
			if (position==0)
			{
				for (int i = 0; i < (sizeof(m_iMessageLimiter)/sizeof(int)); i++)
					m_iMessageLimiter[i] = m_iMaxRepetition;//0-302 used, see WriteToFileEx

				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
				m_bBatteryTolOk	= true;
				m_bP25VoltOk	= true;
				m_bP33VoltOk	= true;
				m_bACPowerOk	= true;
				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
				m_bCountRateTamperOk = true;
				m_bRMSSigmaTamperOk  = true;
				m_bInMII		= false;
				m_bDataFiltered	= false;
				m_bFCHVError	= false;
				m_bICHVError	= false;
				m_dBreakSentCount = 0;
				m_dTimeOutCount = 0;
				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}

			//	send stale string															//DR00406
			if (bStale)																		//DR00406
			{	
				unsigned iLen = strlen(Message);											//DR00406
				char ch1 = Message[iLen-1];													//DR00406
				char ch2 = Message[iLen-2];													//DR00406
				if ((ch1 == '\r') || (ch1 == '\n')) Message[iLen-1] = NULL;					//DR00406
				if ((ch2 == '\r') || (ch2 == '\n')) Message[iLen-2] = NULL;					//DR00406
				fprintf(File,"%s %s",Message,"Note: A newer message has been received.\r\n");	//DR00406
			}																				//DR00406
			else																			//DR00406
				fprintf(File,"%s",Message);
			//	close cev
			fclose(File);
		}
	}

	//if to pfm
	if (ToWhom & TO_PFM)
	{
		//	open filename+pfm
		CString fn(m_szCurrentFileName);
		fn += PFM_SUFFIX;

		CString ArchiveFileNameExt = ArchiveFileName + PFM_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);

			//check if it is in the archive directory
			if (m_bUseShortFilename)
			{
				//build file name
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_szSaveLoc,
				m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
	////			'0' + (TC.GetYear() % 10),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,PFM_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,m_csLongNameFormatString,cTemp,"uG",
					m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,PFM_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"ab+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos( File, &position );
			if (position==0)
			{
				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
				m_bBatteryTolOk	= true;
				m_bP25VoltOk	= true;
				m_bP33VoltOk	= true;
				m_bACPowerOk	= true;
				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
				m_bCountRateTamperOk = true;
				m_bRMSSigmaTamperOk  = true;
				m_bInMII		= false;
				m_bDataFiltered	= false;
				m_bFCHVError	= false;
				m_bICHVError	= false;
				m_dBreakSentCount = 0;
				m_dTimeOutCount = 0;
				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}
			//	send stale string															//DR00406
			if (bStale)																		//DR00406
			{																				//DR00406
				unsigned iLen = strlen(Message);											//DR00406
				char ch1 = Message[iLen-1];													//DR00406
				char ch2 = Message[iLen-2];													//DR00406
				if ((ch1 == '\r') || (ch1 == '\n')) Message[iLen-1] = NULL;					//DR00406
				if ((ch2 == '\r') || (ch2 == '\n')) Message[iLen-2] = NULL;					//DR00406
				fprintf(File,"%s %s",Message,"Note: A newer message has been received.\r\n");	//DR00406
			}																				//DR00406
			else																			//DR00406
				fprintf(File,"%s",Message);
			//	close pfm
			fclose(File);
		}
	}
}

void CInstrument::GetStrings(PAGETYPE ePageType, UINT uiDlgItem, CString& cString, char* cFlag)
{
	COLORREF rgbColor = COLOR_BLACK;
	*cFlag = ' ';
	switch (ePageType)
	{
	case SUMMARYPAGE:
		if ((uiDlgItem >= IDC_SUMMARY_FIRST) && (uiDlgItem <= IDC_SUMMARY_LAST)) 
		{
		  rgbColor = SummaryStaticColors[uiDlgItem-IDC_SUMMARY_FIRST];
		  cString = SummaryStrings[uiDlgItem-IDC_SUMMARY_FIRST];
		}
		break;
//	case GENERALPAGE:
//		if ((uiDlgItem >= IDC_GENERAL_FIRST) && (uiDlgItem <= IDC_GENERAL_LAST)) 
//		{
//		  rgbColor = GeneralStaticColors[uiDlgItem-IDC_GENERAL_FIRST];
//		  cString = GeneralStrings[uiDlgItem-IDC_GENERAL_FIRST];
//		}
//		break;
///	case GENERAL4PAGE:
///		if ((uiDlgItem >= IDC_GENERAL4_FIRST) && (uiDlgItem <= IDC_GENERAL4_LAST)) 
///		{
///		  rgbColor = General4StaticColors[uiDlgItem-IDC_GENERAL4_FIRST];
///		  cString = General4Strings[uiDlgItem-IDC_GENERAL4_FIRST];
///		}
///		break;
	case USER4PAGE:
		if ((uiDlgItem >= IDC_USER4PROG_FIRST) && (uiDlgItem <= IDC_USER4PROG_LAST)) 
		{
		  rgbColor = User4StaticColors[uiDlgItem-IDC_USER4PROG_FIRST];
		  cString = User4Strings[uiDlgItem-IDC_USER4PROG_FIRST];
		}
		break;
//	case USERPAGE:
//		if ((uiDlgItem >= IDC_USERPROG_FIRST) && (uiDlgItem <= IDC_USERPROG_LAST)) 
//		{
//		  rgbColor = UserStaticColors[uiDlgItem-IDC_USERPROG_FIRST];
//		  cString = UserStrings[uiDlgItem-IDC_USERPROG_FIRST];
//		}
//		break;
	case DATAPAGE:
		if ((uiDlgItem >= IDC_DATA_FIRST) && (uiDlgItem <= IDC_DATA_LAST)) 
		{
		  rgbColor = DataStaticColors[uiDlgItem-IDC_DATA_FIRST];
		  cString = DataStrings[uiDlgItem-IDC_DATA_FIRST];
		}
		break;
	case DATA2PAGE:
		if ((uiDlgItem >= IDC_DATA2_FIRST) && (uiDlgItem <= IDC_DATA2_LAST)) 
		{
		  rgbColor = DataStaticColors[uiDlgItem-IDC_DATA2_FIRST];
		  cString = DataStrings[uiDlgItem-IDC_DATA2_FIRST];
		}
		break;
	case DATAIPAGE:
		if ((uiDlgItem >= IDC_DATAI_FIRST) && (uiDlgItem <= IDC_DATAI_LAST)) 
		{
		  rgbColor = DataStaticColors[uiDlgItem-IDC_DATAI_FIRST];
		  cString = DataStrings[uiDlgItem-IDC_DATAI_FIRST];
		}
		break;
	case UGINFOPAGE:
		if ((uiDlgItem >= IDC_UGINFO_FIRST) && (uiDlgItem <= IDC_UGINFO_LAST))
		{
			rgbColor = DataStaticColors[uiDlgItem-IDC_UGINFO_FIRST];
			cString = DataStrings[uiDlgItem-IDC_UGINFO_FIRST];
		}

	}

	if (rgbColor == COLOR_YELLOW) *cFlag = '#';
	else if (rgbColor != COLOR_DEFAULT_TEXT) *cFlag = '*';
}

void CInstrument::SummaryPageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3;//, cTemp4;

	fprintf(outfile,"SUMMARY\n");
	fprintf(outfile," µGRAND Setup\n");

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,temp1,&cTemp1);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,temp2,&cTemp2);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,temp3,&cTemp3);

	fprintf(outfile,"  Battery Voltage (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P25VOLT1,temp1,&cTemp1);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P25VOLT2,temp2,&cTemp2);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P25VOLT3,temp3,&cTemp3);
	fprintf(outfile,"  +12 Volt Supply (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P33VOLT1,temp1,&cTemp1);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P33VOLT2,temp2,&cTemp2);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P33VOLT3,temp3,&cTemp3);
	fprintf(outfile,"  -12 Volt Supply (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,temp1,&cTemp1);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,temp2,&cTemp2);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,temp3,&cTemp3);
	fprintf(outfile,"   +5 Volt Supply (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	fprintf(outfile," Data Status\n");
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_OOMII,temp,&cTemp);
	fprintf(outfile,"   Measurement Interval of Interest: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,temp,&cTemp);
	fprintf(outfile,"                     External Power: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,temp,&cTemp);
	fprintf(outfile,"                            Battery: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BBMOK,temp,&cTemp);
	fprintf(outfile,"           Battery Backed Up Memory: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,temp,&cTemp);/*\n            */
	fprintf(outfile,"  Last INQUIRE Status (Y.M.D H:M:S): %s%c\n",LPCTSTR(temp),cTemp);

	fprintf(outfile,"  Recent Commands and Responses:\n");
//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND6,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[5]);

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND5,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[4]);

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND4,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[3]);

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND3,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[2]);

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND2,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[1]);

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND1,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[0]);
}
/*
void CInstrument::GeneralPageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3;//, cTemp4;

	fprintf(outfile,"uGRAND SETUP\n");
	GetStrings(GENERALPAGE,IDC_GENERAL_TIME,temp,&cTemp);
	fprintf(outfile,"  Last GENERAL STATUS Record (Y.M.D H:M:S): %s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_MODE,temp,&cTemp);
	fprintf(outfile,"                          Operational Mode: %s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_MODE2,temp,&cTemp);
	fprintf(outfile,"                          Operational Mode: %s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_OFFSET,temp,&cTemp);
	fprintf(outfile,"                        Last Offset Record: %s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_FCHIGHVMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_FCHIGHVCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_FCHIGHVMAX,temp3,&cTemp3);
	fprintf(outfile," Pulse Counter HV Bias (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_ICHIGHVMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_ICHIGHVCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_ICHIGHVMAX,temp3,&cTemp3);
	fprintf(outfile,"   Ion Chamber HV Bias (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_BATTERYMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_BATTERYCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_BATTERYMAX,temp3,&cTemp3);
	fprintf(outfile,"       Battery Voltage (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_P25VOLTMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_P25VOLTCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_P25VOLTMAX,temp3,&cTemp3);
	fprintf(outfile,"       +12 Volt Supply (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_P33VOLTMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_P33VOLTCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_P33VOLTMAX,temp3,&cTemp3);
	fprintf(outfile,"       -12 Volt Supply (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,temp3,&cTemp3);
	fprintf(outfile,"        +5 Volt Supply (Min/Cur/Max): %7s%c %7s%c %7s%c\n\n",
		temp1,cTemp1,temp2,cTemp2,temp3,cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_PULSE,temp,&cTemp);
	fprintf(outfile," Pulse Input Configuration: %s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_LLDA,temp,&cTemp);
	fprintf(outfile,"   LLDA: %10s%c    ",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_WINDOWA,temp,&cTemp);
	fprintf(outfile," Window A: %10s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_LLDB,temp,&cTemp);
	fprintf(outfile,"   LLDB: %10s%c    ",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_WINDOWB,temp,&cTemp);
	fprintf(outfile," Window B: %10s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_ANALOG,temp,&cTemp);
	fprintf(outfile," Analog Pulse Channel Gain: %s%c\n\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_DISCM,temp,&cTemp);
	fprintf(outfile," Ion Chamber Disconnect Mode: %s%c\n",temp,cTemp);

	fprintf(outfile,"                          Gamma1           Gamma2\n");
	GetStrings(GENERALPAGE,IDC_GENERAL_GAMMA1G,temp,&cTemp);
	fprintf(outfile,"                Gain: %11s%c     ",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_GAMMA2G,temp,&cTemp);
	fprintf(outfile,"%10s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_GAMMA1M,temp,&cTemp);
	fprintf(outfile,"                Mode: %11s%c     ",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_GAMMA2M,temp,&cTemp);
	fprintf(outfile,"%10s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_MAXG1,temp,&cTemp);
	fprintf(outfile,"            Max Gain: %11s%c     ",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_MAXG2,temp,&cTemp);
	fprintf(outfile,"%10s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_G1DATE,temp,&cTemp);
	fprintf(outfile,"  Date Last Dual Ion: %11s%c     ",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_G2DATE,temp,&cTemp);
	fprintf(outfile,"%10s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_G1TIME,temp,&cTemp);
	fprintf(outfile,"  Time Last Dual Ion: %11s%c     ",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_G2TIME,temp,&cTemp);
	fprintf(outfile,"%10s%c\n\n",temp);

	GetStrings(GENERALPAGE,IDC_GENERAL_UGRANDP,temp,&cTemp);
	fprintf(outfile," uGRAND Program Version Number: %s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_ROM,temp,&cTemp);
	fprintf(outfile," ROM Checksum: %s%c\n",temp,cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_CHECKSUM,temp,&cTemp);
	fprintf(outfile," Checksum, Length, or Authentication Error Received in Message:%c\n %s\n",cTemp,temp);
}
*/
/* ///
void CInstrument::General4PageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3, cTemp4;

	fprintf(outfile,"uG SETUP\n");
	fprintf(outfile,"Most Recent General Status Record, Instrument Status Record, and Battery Record\n");

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_BATTINT,temp,&cTemp);
	fprintf(outfile,"                      Battery Interval (min.): %s %c\n",temp,cTemp);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_STATINT,temp,&cTemp);
	fprintf(outfile,"                       Status Interval (min.): %s %c\n",temp,cTemp);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_FCHVSETPOINT,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVMAX,temp4,&cTemp4);
	fprintf(outfile,"PC HV Set Point %6s%c Pulse Counter HV Bias: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ICHVSETPOINT,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVMAX,temp4,&cTemp4);
	fprintf(outfile,"IC HV Set Point %6s%c   Ion Chamber HV Bias: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_COUNTTIME,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_BATTERYMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_BATTERYCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_BATTERYMAX,temp4,&cTemp4);
	fprintf(outfile,"    Count Time: %6s%c       Battery Voltage: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_MODE,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P25VOLTMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P25VOLTCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P25VOLTMAX,temp4,&cTemp4);
	fprintf(outfile,"   Mode Flag 1: %6s%c       +12 Volt Supply: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_MODE2,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P33VOLTMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P33VOLTCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P33VOLTMAX,temp4,&cTemp4);
	fprintf(outfile,"   Mode Flag 2: %6s%c       -12 Volt Supply: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P5VOLTMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P5VOLTCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P5VOLTMAX,temp4,&cTemp4);
	fprintf(outfile,"                              +5 Volt Supply: %6s%c %6s%c %6s%c\n",
		temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ISRTIME,temp1,&cTemp1);
	fprintf(outfile,"          ISR Date & Time: %s\n",temp1);               

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TIME,temp1,&cTemp1);
	fprintf(outfile,"          GSR Date & Time: %s\n",temp1);               

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_OFFSET,temp1,&cTemp1);
	fprintf(outfile,"      Battery Date & Time: %s\n",temp1);               

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSSTATE,temp1,&cTemp1);
	fprintf(outfile,"          Time Sync State: %s%c\n",temp1,cTemp1);               

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSHOURLY,temp1,&cTemp1);
	fprintf(outfile,"              Hourly Time: %s%c\n",temp1,cTemp1);               

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSDAILY,temp1,&cTemp1);
	fprintf(outfile,"               Daily Time: %s%c\n",temp1,cTemp1);               

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSUPPER,temp1,&cTemp1);
	fprintf(outfile,"          Upper Tolerance: %s%c\n",temp1,cTemp1); 
	
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSLOWER,temp1,&cTemp1);
	fprintf(outfile,"          Lower Tolerance: %s%c\n",temp1,cTemp1); 
	
	fprintf(outfile,"Most Recent Instrument Status and Dual Current Mode Records\n");
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_G1DATE,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_G2DATE,temp2,&cTemp2);  //DP 10/14/04
	fprintf(outfile,"     Dual Ion Date & Time: %s %s\n",temp1,temp2);               

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMAUNMULT,temp1,&cTemp1);
	fprintf(outfile," Gamma Uncert. Multiplier: %s%c\n",temp1,cTemp1);
	
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMA1G,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMA2G,temp2,&cTemp2);
	fprintf(outfile,"                           Gamma1   Gamma2\n");
	fprintf(outfile,"                Gain Mode: %6s%c  %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);
	
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMA1M,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMA2M,temp2,&cTemp2);
	fprintf(outfile,"       Max or Fixed Index: %6s%c  %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_MAXG1,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_MAXG2,temp2,&cTemp2);
	fprintf(outfile,"       Present Gain Index: %6s%c  %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_DISCM,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_HVCONFIG,temp2,&cTemp2);
	fprintf(outfile,"              Offset Mode: %6s%c         HV Config: %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_G1TIME,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_G2TIME,temp2,&cTemp2);
	fprintf(outfile,"         Nominal Interval: %6s%c  Maximum Interval: %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_UGRANDP,temp1,&cTemp1);
	fprintf(outfile,"  µGRAND Program Version Num: %6s%c\n",
		temp1, cTemp1);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ROM,temp1,&cTemp1);
	fprintf(outfile,"               ROM Checksum: %6s%c\n",
		temp1, cTemp1);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,temp,&cTemp1);
	fprintf(outfile," Checksum or Length Error Received in Message: %c\n  %s\n",cTemp1,temp);
}
*/ ///
void CInstrument::DataIPageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cChar;
	fprintf(outfile,"Most Recent INQUIRE Record\n");

	GetStrings(DATAIPAGE,IDC_DATAI_INQUIRETIME,temp,&cChar);
	fprintf(outfile," Last INQUIRE Status (Y.M.D H:M:S):\n %s%c\n",temp,cChar);

	GetStrings(DATAIPAGE,IDC_DATAI_UNITID,temp,&cChar);
	fprintf(outfile,"                      Main Unit ID: %s%c\n",temp,cChar);

	GetStrings(DATAIPAGE,IDC_DATAI_BYTESMEM,temp,&cChar);
	fprintf(outfile,"     Bytes of Data in µGRAND Memory: %s%c\n",temp,cChar);

	GetStrings(DATAIPAGE,IDC_DATAI_ACPOWER,temp,&cChar);
	fprintf(outfile,"            External Power Present: %s%c\n",temp,cChar);

	GetStrings(DATAIPAGE,IDC_DATAI_OOMII,temp,&cChar);
	fprintf(outfile,"   Measurement Interval of Interest: %s%c\n",temp,cChar);

	GetStrings(DATAIPAGE,IDC_DATAI_BATTERYOK,temp,&cChar);
	fprintf(outfile,"                           Battery: %s%c\n",temp,cChar);

	GetStrings(DATAIPAGE,IDC_DATAI_BBMOK,temp,&cChar);
	fprintf(outfile,"          Battery Backed Up Memory: %s%c\n",temp,cChar);

	GetStrings(DATAIPAGE,IDC_DATAI_DATAFILTER,temp,&cChar);
	fprintf(outfile,"                Data Filter Status: %s%c\n",temp,cChar);

	GetStrings(DATAIPAGE,IDC_DATAI_OPERATION,temp,&cChar);
	fprintf(outfile,"                         Operation: %s%c\n",temp,cChar);

	GetStrings(DATAIPAGE,IDC_DATAI_CHECKSUM,temp,&cChar);
	fprintf(outfile," Checksum or Length Error Received in Message: %c\n %s\n",cChar,temp);

}

void CInstrument::DataPageIssuePrint(FILE *outfile)
{
	fprintf(outfile,"Most Recent Main Board ACQUIRE Records\n");

	fprintf(outfile,"   Date of Record (Y/M/D): %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_DATADATE1),
		GetString(DATAPAGE,IDC_DATA_DATADATE2),
		GetString(DATAPAGE,IDC_DATA_DATADATE3),
		GetString(DATAPAGE,IDC_DATA_DATADATE4));

	fprintf(outfile," Time of Record (H:M:S.s): %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_DATATIME1),
		GetString(DATAPAGE,IDC_DATA_DATATIME2),
		GetString(DATAPAGE,IDC_DATA_DATATIME3),
		GetString(DATAPAGE,IDC_DATA_DATATIME4));
	fprintf(outfile,"        Status 0: 1: 2: 3: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_STATUS01231),
		GetString(DATAPAGE,IDC_DATA_STATUS01232),
		GetString(DATAPAGE,IDC_DATA_STATUS01233),
		GetString(DATAPAGE,IDC_DATA_STATUS01234));

	fprintf(outfile,"           PP Input State: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_PPINPUT1),
		GetString(DATAPAGE,IDC_DATA_PPINPUT2),
		GetString(DATAPAGE,IDC_DATA_PPINPUT3),
		GetString(DATAPAGE,IDC_DATA_PPINPUT4));

	fprintf(outfile,"              Ch 0 Status: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH0STATUS1),
		GetString(DATAPAGE,IDC_DATA_CH0STATUS2),
		GetString(DATAPAGE,IDC_DATA_CH0STATUS3),
		GetString(DATAPAGE,IDC_DATA_CH0STATUS4));

	fprintf(outfile,"          Ch 0 Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH0CR1),
		GetString(DATAPAGE,IDC_DATA_CH0CR2),
		GetString(DATAPAGE,IDC_DATA_CH0CR3),
		GetString(DATAPAGE,IDC_DATA_CH0CR4));

	fprintf(outfile,"    Ch 0 Short Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH0SCR1),
		GetString(DATAPAGE,IDC_DATA_CH0SCR2),
		GetString(DATAPAGE,IDC_DATA_CH0SCR3),
		GetString(DATAPAGE,IDC_DATA_CH0SCR4));

	fprintf(outfile,"     Ch 0 Long Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH0LCR1),
		GetString(DATAPAGE,IDC_DATA_CH0LCR2),
		GetString(DATAPAGE,IDC_DATA_CH0LCR3),
		GetString(DATAPAGE,IDC_DATA_CH0LCR4));

	fprintf(outfile,"              Ch 1 Status: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH1STATUS1),
		GetString(DATAPAGE,IDC_DATA_CH1STATUS2),
		GetString(DATAPAGE,IDC_DATA_CH1STATUS3),
		GetString(DATAPAGE,IDC_DATA_CH1STATUS4));

	fprintf(outfile,"          Ch 1 Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH1CR1),
		GetString(DATAPAGE,IDC_DATA_CH1CR2),
		GetString(DATAPAGE,IDC_DATA_CH1CR3),
		GetString(DATAPAGE,IDC_DATA_CH1CR4));

	fprintf(outfile,"    Ch 1 Short Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH1SCR1),
		GetString(DATAPAGE,IDC_DATA_CH1SCR2),
		GetString(DATAPAGE,IDC_DATA_CH1SCR3),
		GetString(DATAPAGE,IDC_DATA_CH1SCR4));

	fprintf(outfile,"     Ch 1 Long Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH1LCR1),
		GetString(DATAPAGE,IDC_DATA_CH1LCR2),
		GetString(DATAPAGE,IDC_DATA_CH1LCR3),
		GetString(DATAPAGE,IDC_DATA_CH1LCR4));

	fprintf(outfile,"              Ch 2 Status: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH2STATUS1),
		GetString(DATAPAGE,IDC_DATA_CH2STATUS2),
		GetString(DATAPAGE,IDC_DATA_CH2STATUS3),
		GetString(DATAPAGE,IDC_DATA_CH2STATUS4));

	fprintf(outfile,"          Ch 2 Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH2CR1),
		GetString(DATAPAGE,IDC_DATA_CH2CR2),
		GetString(DATAPAGE,IDC_DATA_CH2CR3),
		GetString(DATAPAGE,IDC_DATA_CH2CR4));

	fprintf(outfile,"    Ch 2 Short Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH2SCR1),
		GetString(DATAPAGE,IDC_DATA_CH2SCR2),
		GetString(DATAPAGE,IDC_DATA_CH2SCR3),
		GetString(DATAPAGE,IDC_DATA_CH2SCR4));

	fprintf(outfile,"     Ch 2 Long Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH2LCR1),
		GetString(DATAPAGE,IDC_DATA_CH2LCR2),
		GetString(DATAPAGE,IDC_DATA_CH2LCR3),
		GetString(DATAPAGE,IDC_DATA_CH2LCR4));

	fprintf(outfile,"              Ch 3 Status: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH3STATUS1),
		GetString(DATAPAGE,IDC_DATA_CH3STATUS2),
		GetString(DATAPAGE,IDC_DATA_CH3STATUS3),
		GetString(DATAPAGE,IDC_DATA_CH3STATUS4));

	fprintf(outfile,"          Ch 3 Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH3CR1),
		GetString(DATAPAGE,IDC_DATA_CH3CR2),
		GetString(DATAPAGE,IDC_DATA_CH3CR3),
		GetString(DATAPAGE,IDC_DATA_CH3CR4));

	fprintf(outfile,"    Ch 3 Short Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH3SCR1),
		GetString(DATAPAGE,IDC_DATA_CH3SCR2),
		GetString(DATAPAGE,IDC_DATA_CH3SCR3),
		GetString(DATAPAGE,IDC_DATA_CH3SCR4));

	fprintf(outfile,"     Ch 3 Long Count Rate: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CH3LCR1),
		GetString(DATAPAGE,IDC_DATA_CH3LCR2),
		GetString(DATAPAGE,IDC_DATA_CH3LCR3),
		GetString(DATAPAGE,IDC_DATA_CH3LCR4));

	fprintf(outfile,"        CPU & BOARD Temp.: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP1),
		GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP2),
		GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP3),
		GetString(DATAPAGE,IDC_DATA_CPUBOARDTEMP4));

	fprintf(outfile,"   Duration Data Acquired: %15s %15s %15s %15s\n",
		GetString(DATAPAGE,IDC_DATA_LENGTHT1),
		GetString(DATAPAGE,IDC_DATA_LENGTHT2),
		GetString(DATAPAGE,IDC_DATA_LENGTHT3),
		GetString(DATAPAGE,IDC_DATA_LENGTHT4));
}

void CInstrument::Data2PageIssuePrint(FILE *outfile)
{
	fprintf(outfile,"Most Recent Sensor Board ACQUIRE Records\n");

	fprintf(outfile," Date of Record (Y/M/D): %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_DATADATE1),
		GetString(DATA2PAGE,IDC_DATA2_DATADATE2),
		GetString(DATA2PAGE,IDC_DATA2_DATADATE3),
		GetString(DATA2PAGE,IDC_DATA2_DATADATE4));
	//                                      :
	fprintf(outfile," Time of Record (H:M:S.s): %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_DATATIME1),
		GetString(DATA2PAGE,IDC_DATA2_DATATIME2),
		GetString(DATA2PAGE,IDC_DATA2_DATATIME3),
		GetString(DATA2PAGE,IDC_DATA2_DATATIME4));
	//
	fprintf(outfile,"         Status 0:1:2:3: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_STATUS01231),
		GetString(DATA2PAGE,IDC_DATA2_STATUS01232),
		GetString(DATA2PAGE,IDC_DATA2_STATUS01233),
		GetString(DATA2PAGE,IDC_DATA2_STATUS01234));
	//                                      :
	fprintf(outfile,"          Accelerometer: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_ACCEL1),
		GetString(DATA2PAGE,IDC_DATA2_ACCEL2),
		GetString(DATA2PAGE,IDC_DATA2_ACCEL3),
		GetString(DATA2PAGE,IDC_DATA2_ACCEL4));
	//                                      :
	fprintf(outfile,"               Humidity: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_HUMID1),
		GetString(DATA2PAGE,IDC_DATA2_HUMID2),
		GetString(DATA2PAGE,IDC_DATA2_HUMID3),
		GetString(DATA2PAGE,IDC_DATA2_HUMID4));
	//                                      :
	fprintf(outfile,"        Light Intensity: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_LIGHT1),
		GetString(DATA2PAGE,IDC_DATA2_LIGHT2),
		GetString(DATA2PAGE,IDC_DATA2_LIGHT3),
		GetString(DATA2PAGE,IDC_DATA2_LIGHT4));
	//                                      :
	fprintf(outfile,"         Magnetometer X: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_MAGX1),
		GetString(DATA2PAGE,IDC_DATA2_MAGX2),
		GetString(DATA2PAGE,IDC_DATA2_MAGX3),
		GetString(DATA2PAGE,IDC_DATA2_MAGX4));
	//                                      :
	fprintf(outfile,"         Magnetometer Y: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_MAGY1),
		GetString(DATA2PAGE,IDC_DATA2_MAGY2),
		GetString(DATA2PAGE,IDC_DATA2_MAGY3),
		GetString(DATA2PAGE,IDC_DATA2_MAGY4));
	//                                      :
	fprintf(outfile,"         Magnetometer Z: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_MAGZ1),
		GetString(DATA2PAGE,IDC_DATA2_MAGZ2),
		GetString(DATA2PAGE,IDC_DATA2_MAGZ3),
		GetString(DATA2PAGE,IDC_DATA2_MAGZ4));
	//                                      :
	fprintf(outfile,"           Surf Wetness: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_SURFWET1),
		GetString(DATA2PAGE,IDC_DATA2_SURFWET2),
		GetString(DATA2PAGE,IDC_DATA2_SURFWET3),
		GetString(DATA2PAGE,IDC_DATA2_SURFWET4));
	//                                      :
	fprintf(outfile,"              CPU Temp.: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_CPUTEMP1),
		GetString(DATA2PAGE,IDC_DATA2_CPUTEMP2),
		GetString(DATA2PAGE,IDC_DATA2_CPUTEMP3),
		GetString(DATA2PAGE,IDC_DATA2_CPUTEMP4));
	//
	fprintf(outfile,"            BOARD Temp.: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP1),
		GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP2),
		GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP3),
		GetString(DATA2PAGE,IDC_DATA2_BOARDTEMP4));
	//                                      :
	fprintf(outfile,"          Interval Time: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_INTERVAL1),
		GetString(DATA2PAGE,IDC_DATA2_INTERVAL2),
		GetString(DATA2PAGE,IDC_DATA2_INTERVAL3),
		GetString(DATA2PAGE,IDC_DATA2_INTERVAL4));
	//
	fprintf(outfile,"      ESB Serial Number: %15s %15s %15s %15s\n",
		GetString(DATA2PAGE,IDC_DATA2_SERIALNUM1),
		GetString(DATA2PAGE,IDC_DATA2_SERIALNUM2),
		GetString(DATA2PAGE,IDC_DATA2_SERIALNUM3),
		GetString(DATA2PAGE,IDC_DATA2_SERIALNUM4));

}

/*
void CInstrument::UserPageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3, cTemp4;
	fprintf(outfile,"MONITOR PARAMETERS\n");
//	GetDlgItemText(IDC_USERPROG_TIME,temp);
	fprintf(outfile," Last USER PROGRAM STATUS Record (Y.M.D H:M:S): %s\n",
		GetString(USERPAGE,IDC_USERPROG_TIME));

	GetStrings(USERPAGE,IDC_USERPROG_USER,temp,&cTemp);
	fprintf(outfile," User Selected Data Acquire Time (secs): %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_IMMEDIATE,temp,&cTemp);
	fprintf(outfile," Immediate Buffer Size / Save Size: %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_LOCALBACK,temp,&cTemp);
	fprintf(outfile,"       Local Background Definition: %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_NOFILTER,temp,&cTemp);
	fprintf(outfile," No Filter and Enter MII Condition: %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_FILTERMETH,temp,&cTemp);
	fprintf(outfile,"                     Filter Method: %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_MIIENTER,temp,&cTemp);
	fprintf(outfile,"               MII Enter Condition: %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_MIIEXIT,temp,&cTemp);
	fprintf(outfile,"                MII Exit Condition: %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_FILTERENAB,temp,&cTemp);
	fprintf(outfile,"                 Filtering Enabled: %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_TRIGGERMODE,temp,&cTemp);
	fprintf(outfile,"                      Trigger Mode: %s%c\n",temp,cTemp);

	fprintf(outfile,"                       Pulse A    Pulse B    Pulse C    Gamma 1    Gamma 2\n");

	GetStrings(USERPAGE,IDC_USERPROG_PULSEAF,temp,&cTemp);
	GetStrings(USERPAGE,IDC_USERPROG_PULSEBF,temp1,&cTemp1);
	GetStrings(USERPAGE,IDC_USERPROG_PULSECF,temp2,&cTemp2);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA1F,temp3,&cTemp3);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA2F,temp4,&cTemp4);
	temp.TrimLeft();		temp1.TrimLeft();		temp2.TrimLeft();
	temp3.TrimLeft();		temp4.TrimLeft();
	fprintf(outfile," Filter & MII Active:%9s%c %9s%c %9s%c %9s%c %9s%c\n",
		temp,cTemp,temp1,cTemp1,temp2,cTemp2,temp3,cTemp3,temp4,cTemp4);

	GetStrings(USERPAGE,IDC_USERPROG_PULSEAT,temp ,&cTemp);
	GetStrings(USERPAGE,IDC_USERPROG_PULSEBT,temp1,&cTemp1);
	GetStrings(USERPAGE,IDC_USERPROG_PULSECT,temp2,&cTemp2);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA1T,temp3,&cTemp3);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA2T,temp4,&cTemp4);
	temp.TrimLeft();		temp1.TrimLeft();		temp2.TrimLeft();
	temp3.TrimLeft();		temp4.TrimLeft();
	fprintf(outfile,"           Threshold:%9s%c %9s%c %9s%c %9s%c %9s%c\n",
		temp,cTemp,temp1,cTemp1,temp2,cTemp2,temp3,cTemp3,temp4,cTemp4);

	GetStrings(USERPAGE,IDC_USERPROG_PULSEAL,temp ,&cTemp);
	GetStrings(USERPAGE,IDC_USERPROG_PULSEBL,temp1,&cTemp1);
	GetStrings(USERPAGE,IDC_USERPROG_PULSECL,temp2,&cTemp2);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA1L,temp3,&cTemp3);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA2L,temp4,&cTemp4);
	temp.TrimLeft();		temp1.TrimLeft();		temp2.TrimLeft();
	temp3.TrimLeft();		temp4.TrimLeft();
	fprintf(outfile," Filtering Threshold:%9s%c %9s%c %9s%c %9s%c %9s%c\n\n",
		temp,cTemp,temp1,cTemp1,temp2,cTemp2,temp3,cTemp3,temp4,cTemp4);

	GetStrings(USERPAGE,IDC_USERPROG_SYNCE,temp,&cTemp);
	fprintf(outfile,"               Syncronization Enabled: %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_SYNCT,temp,&cTemp);
	fprintf(outfile,"       Syncronization time (hh:mm:ss): %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_SYNCL,temp,&cTemp);
	fprintf(outfile," Syncronization Lower Limit (seconds): %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_SYNCU,temp,&cTemp);
	fprintf(outfile," Syncronization Upper Limit (minutes): %s%c\n\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_TAKEOFF,temp,&cTemp);
	fprintf(outfile,"        Take Offsets Every (hours): %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_TAKEOFFMAX,temp,&cTemp);
	fprintf(outfile,"   Maximum Between Offsets (hours): %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_LOGBATT,temp,&cTemp);
	fprintf(outfile,"  Log Battery Info Every (minutes): %s%c\n\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_UGRANDUV,temp,&cTemp);
	fprintf(outfile," uGRAND User Program Version Number: %s%c\n",temp,cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_ROMCHECK,temp,&cTemp);
	fprintf(outfile,"                      ROM Checksum: %s%c\n",temp,cTemp);

}
*/
void CInstrument::User4PageIssuePrint(FILE *outfile)
{
	CString temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9;
	char cTemp0, cTemp1, cTemp2, cTemp3, cTemp4, cTemp5, cTemp6, cTemp7, cTemp8, cTemp9;

	fprintf(outfile,"TRIGGER PARAMETERS\n");
	fprintf(outfile,"  Most Recent Trigger Configuration Record\n");
	fprintf(outfile,"  Date/Time of Record (Y.M.D H:M:S): %s\n",
		GetString(USER4PAGE,IDC_USER4PROG_TIME));

	fprintf(outfile,"             Logic     Channel / Type\n");
	GetStrings(USER4PAGE,IDC_USER4PROG_LOGIC1,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE1,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE2,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE3,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE4,temp5,&cTemp5);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE5,temp6,&cTemp6);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE6,temp7,&cTemp7);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE7,temp8,&cTemp8);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE8,temp9,&cTemp9);
	fprintf(outfile,"      Pin 4: %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c\n",
		temp1, cTemp1,	temp2, cTemp2,	temp3, cTemp3,	temp4, cTemp4,
		temp5, cTemp5,	temp6, cTemp6,	temp7, cTemp7,	temp8, cTemp8,
		temp9, cTemp9);

	GetStrings(USER4PAGE,IDC_USER4PROG_LOGIC2,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE1,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE2,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE3,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE4,temp5,&cTemp5);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE5,temp6,&cTemp6);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE6,temp7,&cTemp7);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE7,temp8,&cTemp8);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE8,temp9,&cTemp9);
	fprintf(outfile,"      Pin 5: %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c\n",
		temp1, cTemp1,	temp2, cTemp2,	temp3, cTemp3,	temp4, cTemp4,
		temp5, cTemp5,	temp6, cTemp6,	temp7, cTemp7,	temp8, cTemp8,
		temp9, cTemp9);

	GetStrings(USER4PAGE,IDC_USER4PROG_LOGIC3,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE1,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE2,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE3,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE4,temp5,&cTemp5);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE5,temp6,&cTemp6);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE6,temp7,&cTemp7);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE7,temp8,&cTemp8);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE8,temp9,&cTemp9);
	fprintf(outfile,"      Pin 6: %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c\n",
		temp1, cTemp1,	temp2, cTemp2,	temp3, cTemp3,	temp4, cTemp4,
		temp5, cTemp5,	temp6, cTemp6,	temp7, cTemp7,	temp8, cTemp8,
		temp9, cTemp9);

	GetStrings(USER4PAGE,IDC_USER4PROG_LOGIC4,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE1,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE2,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE3,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE4,temp5,&cTemp5);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE5,temp6,&cTemp6);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE6,temp7,&cTemp7);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE7,temp8,&cTemp8);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE8,temp9,&cTemp9);
	fprintf(outfile,"      Pin 7: %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c\n",
		temp1, cTemp1,	temp2, cTemp2,	temp3, cTemp3,	temp4, cTemp4,
		temp5, cTemp5,	temp6, cTemp6,	temp7, cTemp7,	temp8, cTemp8,
		temp9, cTemp9);

	fprintf(outfile,"  Most Recent Channel Config. Records\n");

	GetStrings(USER4PAGE,IDC_USER4PROG_CH0USED,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH1USED,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH2USED,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH3USED,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH4USED,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH5USED,temp5,&cTemp5);
	if (temp0.GetLength()==0)temp0 = "Ch 0";
	if (temp1.GetLength()==0)temp1 = "Ch 1";
	if (temp2.GetLength()==0)temp2 = "Ch 2";
	if (temp3.GetLength()==0)temp3 = "Ch 3";
	if (temp4.GetLength()==0)temp4 = "Ch 4";
	if (temp5.GetLength()==0)temp5 = "Ch 5";
	fprintf(outfile,"                         %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5); 

	GetStrings(USER4PAGE,IDC_USER4PROG_C00,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C10,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C20,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C30,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C40,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C50,temp5,&cTemp5);
	fprintf(outfile,"     Thresh 1 Type::Dir: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5); 

	GetStrings(USER4PAGE,IDC_USER4PROG_C01,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C11,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C21,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C31,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C41,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C51,temp5,&cTemp5);
	fprintf(outfile,"         Thresh 1 Value: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C02,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C12,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C22,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C32,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C42,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C52,temp5,&cTemp5);
	fprintf(outfile,"   Thresh 1 Entry::Exit: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C03,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C13,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C23,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C33,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C43,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C53,temp5,&cTemp5);
	fprintf(outfile,"     Thresh 2 Type::Dir: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C04,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C14,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C24,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C34,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C44,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C54,temp5,&cTemp5);
	fprintf(outfile,"         Thresh 2 Value: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C05,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C15,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C25,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C35,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C45,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C55,temp5,&cTemp5);
	fprintf(outfile,"   Thresh 2 Entry::Exit: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C06,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C16,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C26,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C36,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C46,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C56,temp5,&cTemp5);
	fprintf(outfile,"  Chng. Sig. Multiplier: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C07,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C17,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C27,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C37,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C47,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C57,temp5,&cTemp5);
	fprintf(outfile,"   Chng. Sig. Entry Pt.: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C08,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C18,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C28,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C38,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C48,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C58,temp5,&cTemp5);
	fprintf(outfile,"           Filter Limit: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C09,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C19,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C29,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C39,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C49,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C59,temp5,&cTemp5);
	fprintf(outfile,"             Hysteresis: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0, cTemp0, temp1, cTemp1, 
		temp2, cTemp2, temp3, cTemp3, 
		temp4, cTemp4, temp5, cTemp5);
}
/*
void CInstrument::SetupPageIssuePrint(FILE* outfile)
{
	CUGrandSetupData cSetupData;
	for (int iIndex = 0; iIndex <= (SET_TOP - SET_BASE); iIndex++)
	{
		cSetupData.SetItteratorLineTo(iIndex);
		fprintf(outfile,"%35s: %s\n",cSetupData.GetNext(),SetupStrings[iIndex]);
	}
}
*/
void CInstrument::SetupPageIssuePrint(FILE* outfile)
{
	CUGrandSetupData cSetupData;
	bool bAtChConfRecords = false;

	fprintf(outfile, "Instrument Settings\n");

	fprintf(outfile,"  Item: Reported(instrument) :: Desired(INI file)\n");
	for (int iIndex = 0; iIndex <= (SET_TOP - SET_BASE); iIndex++)
	{
		cSetupData.SetItteratorLineTo(iIndex);
		fprintf(outfile,"%35s: %s :: %s\n",cSetupData.GetNext(),SetupStrings[iIndex], SetupINIStrings[iIndex]);
	}
}

void CInstrument::IssuePrint2(FILE* outfile)
{
	CString temp, temp1, temp2, temp3, temp4;

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");

	fprintf(outfile,"* = Displayed in RED (reading out of tolerance)\n");
	fprintf(outfile,"# = Displayed in YELLOW (configuration doesn't match)\n");
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
	SummaryPageIssuePrint(outfile);
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
//	if (m_eMonitorVers == PRE4)
//	{
//		GeneralPageIssuePrint(outfile);
//		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
//	}
//	if (m_eMonitorVers == POST4)
//	{
///		General4PageIssuePrint(outfile);
///		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
//	}

	DataPageIssuePrint(outfile);
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
	DataIPageIssuePrint(outfile);
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
	Data2PageIssuePrint(outfile);
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);

//	if (m_eMonitorVers == PRE4)
//	{
//		UserPageIssuePrint(outfile);
//		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
//	}
//	if (m_eMonitorVers == POST4)
//	{
		User4PageIssuePrint(outfile);
		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
//	}

//	if (m_eMonitorVers == PRE4)
//	{
//		SetupPageIssuePrint(outfile);
//		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
//	}
//	if (m_eMonitorVers == POST4)
//	{
		SetupPageIssuePrint(outfile);
		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
//	}
//
//	modparpage->IssuePrint(outfile);
//	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
//
	fprintf(outfile,"\nCURRENT CONFIGURATION (from INI file)\n");
	char cIniData[32768];
	if (GetPrivateProfileSection(m_pName,cIniData,sizeof(cIniData),m_szIniFile)>0)
	{
		char *at = cIniData;		//char pointer to step through buffpass
		while (strlen(at)>0) 
		{
			fprintf(outfile,"  %s\n",at);
			at = at + strlen(at)+1;
		}
	}
}

void CInstrument::FlagTime(int val, bool redraw)
{
	CISO::FlagTime(val,redraw);

	if (val == 1)
	{
		WriteToFileEx(303,TO_PFM,TYPE_COMP,NULL,
			"00000 INQUIRE2 Response Record caused [TIME icon on] \r\n");
	}
}

void CInstrument::FlagComm(int val, bool redraw)
{
	CISO::FlagComm(val,redraw);

	if (val == 1)
	{
		WriteToFileEx(304,TO_PFM,TYPE_COMP,NULL,
			"00000 A communications problem caused [FLAG, SKULL, or BOMB icon on]\r\n");
	}
}

void CInstrument::FlagOthr(int val, bool redraw)
{
	CISO::FlagOthr(val,redraw);

	if (val == 1)
	{
		WriteToFileEx(305,TO_PFM,TYPE_COMP,NULL,
			"00000 A power or BBM problem caused [FROWN FACE icon on] \r\n");
	}
}


void CInstrument::UpdateDisplay()
{
		if (!m_cID3_Record.IsEmpty())
			m_pPropertySheet->ID3_Record((LPSTR)LPCSTR(m_cID3_Record));

		if (!m_cuGConfigRecord.IsEmpty())
			m_pPropertySheet->OnuGConfigRecord((LPSTR)LPCSTR(m_cuGConfigRecord));

		if (!m_cUGStatusRecord.IsEmpty())
			m_pPropertySheet->OnuGStatusRecord((LPSTR)LPCSTR(m_cUGStatusRecord));

		if (!m_cUGInfoRecord.IsEmpty())
			m_pPropertySheet->OnuGInfoRecord((LPSTR)LPCSTR(m_cUGInfoRecord));

		if (!m_cESBInfoRecord1.IsEmpty())
			m_pPropertySheet->OnESBInfoRecord((LPSTR)LPCSTR(m_cESBInfoRecord1));
		if (!m_cESBInfoRecord2.IsEmpty())
			m_pPropertySheet->OnESBInfoRecord((LPSTR)LPCSTR(m_cESBInfoRecord2));
		if (!m_cESBInfoRecord3.IsEmpty())
			m_pPropertySheet->OnESBInfoRecord((LPSTR)LPCSTR(m_cESBInfoRecord3));
		if (!m_cESBInfoRecord4.IsEmpty())
			m_pPropertySheet->OnESBInfoRecord((LPSTR)LPCSTR(m_cESBInfoRecord4));

		if (!m_cPSBInfoRecord1.IsEmpty())
			m_pPropertySheet->OnPSBInfoRecord((LPSTR)LPCSTR(m_cPSBInfoRecord1));
		if (!m_cPSBInfoRecord2.IsEmpty())
			m_pPropertySheet->OnPSBInfoRecord((LPSTR)LPCSTR(m_cPSBInfoRecord2));
		if (!m_cPSBInfoRecord3.IsEmpty())
			m_pPropertySheet->OnPSBInfoRecord((LPSTR)LPCSTR(m_cPSBInfoRecord3));
		if (!m_cPSBInfoRecord4.IsEmpty())
			m_pPropertySheet->OnPSBInfoRecord((LPSTR)LPCSTR(m_cPSBInfoRecord4));

		if (!m_cBMBInfoRecord1.IsEmpty())
			m_pPropertySheet->OnBMBInfoRecord((LPSTR)LPCSTR(m_cBMBInfoRecord1));
		if (!m_cBMBInfoRecord2.IsEmpty())
			m_pPropertySheet->OnBMBInfoRecord((LPSTR)LPCSTR(m_cBMBInfoRecord2));
		if (!m_cBMBInfoRecord3.IsEmpty())
			m_pPropertySheet->OnBMBInfoRecord((LPSTR)LPCSTR(m_cBMBInfoRecord3));
		if (!m_cBMBInfoRecord4.IsEmpty())
			m_pPropertySheet->OnBMBInfoRecord((LPSTR)LPCSTR(m_cBMBInfoRecord4));

		if (!m_cESBConfigRecord1.IsEmpty())
			m_pPropertySheet->OnESBConfigRecord((LPSTR)LPCSTR(m_cESBConfigRecord1));
		if (!m_cESBConfigRecord2.IsEmpty())
			m_pPropertySheet->OnESBConfigRecord((LPSTR)LPCSTR(m_cESBConfigRecord2));
		if (!m_cESBConfigRecord3.IsEmpty())
			m_pPropertySheet->OnESBConfigRecord((LPSTR)LPCSTR(m_cESBConfigRecord3));
		if (!m_cESBConfigRecord4.IsEmpty())
			m_pPropertySheet->OnESBConfigRecord((LPSTR)LPCSTR(m_cESBConfigRecord4));

		if (!m_cPSBConfigRecord1.IsEmpty())
			m_pPropertySheet->OnPSBConfigRecord((LPSTR)LPCSTR(m_cPSBConfigRecord1));
		if (!m_cPSBConfigRecord2.IsEmpty())
			m_pPropertySheet->OnPSBConfigRecord((LPSTR)LPCSTR(m_cPSBConfigRecord2));
		if (!m_cPSBConfigRecord3.IsEmpty())
			m_pPropertySheet->OnPSBConfigRecord((LPSTR)LPCSTR(m_cPSBConfigRecord3));
		if (!m_cPSBConfigRecord4.IsEmpty())
			m_pPropertySheet->OnPSBConfigRecord((LPSTR)LPCSTR(m_cPSBConfigRecord4));

		if (!m_cBMBConfigRecord1.IsEmpty())
			m_pPropertySheet->OnBMBConfigRecord((LPSTR)LPCSTR(m_cBMBConfigRecord1));
		if (!m_cBMBConfigRecord2.IsEmpty())
			m_pPropertySheet->OnBMBConfigRecord((LPSTR)LPCSTR(m_cBMBConfigRecord2));
		if (!m_cBMBConfigRecord3.IsEmpty())
			m_pPropertySheet->OnBMBConfigRecord((LPSTR)LPCSTR(m_cBMBConfigRecord3));
		if (!m_cBMBConfigRecord4.IsEmpty())
			m_pPropertySheet->OnBMBConfigRecord((LPSTR)LPCSTR(m_cBMBConfigRecord4));
		
		if (!m_cPSBStatusRecord1.IsEmpty())
			m_pPropertySheet->OnPSBStatusRecord((LPSTR)LPCSTR(m_cPSBStatusRecord1));
		if (!m_cPSBStatusRecord2.IsEmpty())
			m_pPropertySheet->OnPSBStatusRecord((LPSTR)LPCSTR(m_cPSBStatusRecord2));
		if (!m_cPSBStatusRecord3.IsEmpty())
			m_pPropertySheet->OnPSBStatusRecord((LPSTR)LPCSTR(m_cPSBStatusRecord3));
		if (!m_cPSBStatusRecord4.IsEmpty())
			m_pPropertySheet->OnPSBStatusRecord((LPSTR)LPCSTR(m_cPSBStatusRecord4));

		if (!m_cBMBStatusRecord1.IsEmpty())
			m_pPropertySheet->OnBMBStatusRecord((LPSTR)LPCSTR(m_cBMBStatusRecord1));
		if (!m_cBMBStatusRecord2.IsEmpty())
			m_pPropertySheet->OnBMBStatusRecord((LPSTR)LPCSTR(m_cBMBStatusRecord2));
		if (!m_cBMBStatusRecord3.IsEmpty())
			m_pPropertySheet->OnBMBStatusRecord((LPSTR)LPCSTR(m_cBMBStatusRecord3));
		if (!m_cBMBStatusRecord4.IsEmpty())
			m_pPropertySheet->OnBMBStatusRecord((LPSTR)LPCSTR(m_cBMBStatusRecord4));




//		if (!m_cDualCurrentModeConfigRecord.IsEmpty())
//			m_pPropertySheet->DualCurrentModeConfigRecord((LPSTR)LPCSTR(m_cDualCurrentModeConfigRecord));

		if (!m_cTriggerConfigRecord.IsEmpty())
			m_pPropertySheet->TriggerConfigRecord((LPSTR)LPCSTR(m_cTriggerConfigRecord));

//		if (m_cInstrumentInfoRecord.GetLength())
//			m_pPropertySheet->InstrumentInfoRecord((LPSTR)LPCSTR(m_cInstrumentInfoRecord));

//		if (m_cChannel0ConfigRecord.GetLength())
//			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel0ConfigRecord),CHANNEL0_CONFIG_RECORD);

//		if (m_cChannel1ConfigRecord.GetLength())
//			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel1ConfigRecord),CHANNEL1_CONFIG_RECORD);

//		if (m_cChannel2ConfigRecord.GetLength())
//			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel2ConfigRecord),CHANNEL2_CONFIG_RECORD);

//		if (m_cChannel3ConfigRecord.GetLength())
//			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel3ConfigRecord),CHANNEL3_CONFIG_RECORD);

//		if (m_cChannel4ConfigRecord.GetLength())
//			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel4ConfigRecord),CHANNEL4_CONFIG_RECORD);

//		if (m_cChannel5ConfigRecord.GetLength())
//			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel5ConfigRecord),CHANNEL5_CONFIG_RECORD);

//		if (m_cChannel6ConfigRecord.GetLength())
//			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel6ConfigRecord),CHANNEL6_CONFIG_RECORD);
//	}

//always do these
	if (m_cInformRec.GetLength())
		m_pPropertySheet->InformRec((LPSTR)LPCSTR(m_cInformRec));

//	if (m_cAnalyzeOkRec.GetLength())
//		m_pPropertySheet->AnalyzeOkRec((LPSTR)LPCSTR(m_cAnalyzeOkRec));

	if (m_cID2_Record.GetLength())
		m_pPropertySheet->ID2_Record((LPSTR)LPCSTR(m_cID2_Record));

//	if (m_cTripN2Rec.GetLength())
//		m_pPropertySheet->TripN2Rec((LPSTR)LPCSTR(m_cTripN2Rec));

//	if (m_cBatteryRec.GetLength())
//		m_pPropertySheet->BatteryRec((LPSTR)LPCSTR(m_cBatteryRec));

	for (int i = 0; i < NUMBEROFACQUIRERECS1DISPLAYED; i++)
	{
		if (m_cAcquireRec1[i].GetLength())
			m_pPropertySheet->AcquirRec1((LPSTR)LPCSTR(m_cAcquireRec1[i]));//,true);
	}
	for ( i = 0; i < NUMBEROFACQUIRERECS2DISPLAYED; i++)
	{
		if (m_cAcquireRec2[i].GetLength())
			m_pPropertySheet->AcquirRec2((LPSTR)LPCSTR(m_cAcquireRec2[i]));//,true);
	}

//	if (m_cIDRec.GetLength())
//		m_pPropertySheet->ID_Record((LPSTR)LPCSTR(m_cIDRec));

//	if (m_cOffsetRec.GetLength())
//		m_pPropertySheet->OffsetRec((LPSTR)LPCSTR(m_cOffsetRec));

//	m_pPropertySheet->UpdateTabs();

}
	
void CInstrument::ShowInstrument(bool show)
{

//	TRACE("RADAcquireRecord = %d\r\n",sizeof(RADAcquireRecord));
//	TRACE("ESBAcquireRecord = %d\r\n",sizeof(ESBAcquireRecord));
//	TRACE("uGConfigRecord = %d\r\n",sizeof(uGConfigRecord));
//	TRACE("uGChConfigRecord = %d\r\n",sizeof(uGChConfigRecord));
//	TRACE("ESBConfigRecord = %d\r\n",sizeof(ESBConfigRecord));
//	TRACE("PSBConfigRecord = %d\r\n",sizeof(PSBConfigRecord));
//	TRACE("BMBConfigRecord = %d\r\n",sizeof(BMBConfigRecord));
//	TRACE("uGStatusRecord = %d\r\n",sizeof(uGStatusRecord));
//	TRACE("PSBStatusRecord = %d\r\n",sizeof(PSBStatusRecord));
//	TRACE("BMBStatusRecord = %d\r\n",sizeof(BMBStatusRecord));
//	TRACE("uGInfoRecord = %d\r\n",sizeof(uGInfoRecord));
//	TRACE("ESBInfoRecord = %d\r\n",sizeof(ESBInfoRecord));
//	TRACE("PSBInfoRecord = %d\r\n",sizeof(PSBInfoRecord));
//	TRACE("BMBInfoRecord = %d\r\n",sizeof(BMBInfoRecord));
//	TRACE("InformationRecord = %d\r\n",sizeof(InformationRecord));
//	TRACE("TriggerConfigureRecord = %d\r\n",sizeof(TriggerConfigureRecord));

	//if we already have 10 windows open don't allow any more
	if (CSafePropertySheet::ms_iUGRANDPropertySheetCount < 10)
	{
		//correct for possible misscount
		if (CSafePropertySheet::ms_iUGRANDPropertySheetCount < 0) 
			CSafePropertySheet::ms_iUGRANDPropertySheetCount = 0;
		//create and open a new window
		if (!m_pPropertySheet)
		{
			m_pPropertySheet   = new CSafePropertySheet(LPCTSTR(m_pFullNameSpaces),this, m_szIniFile);

			//create each of the pages we'll need in the property sheet
			m_pSummaryPage     = new CSummary(IDD_SUMMARY, this);
///			m_pGeneral4StatPage= new CGenl4Stat(IDD_GENERAL4STAT_PAGE);
			m_pDataStatus      = new CDataStat(IDD_DATASTATUS_PAGE);
			m_pData2Status      = new CData2Stat(IDD_DATA2STATUS_PAGE);
			m_pDataIStatus      = new CDataIStat(IDD_DATAISTATUS_PAGE);
			m_pUser4ProgStatus = new CUser4Prog(IDD_USER4PROGSTAT_PAGE);
			m_pSetupData	   = new CSetupData(m_pPropertySheet, m_szIniFile);
			m_pModParams       = new CModParam(IDD_MODPARAMS_PAGE,this,m_szIniFile,false);
			m_pUGInfo		   = new CUGINFO(/*IDD_DIALOG_UG*/);
			m_pBMBInfo		   = new CBMBInfo(/*IDD_DIALOG_BMB*/);
			m_pPSBInfo		   = new CPSBInfo(/*IDD_DIALOG_PSB*/);
			m_pESBInfo		   = new CESBInfo(/*IDD_DIALOG_ESB*/);
			m_pLegend		   = new CLegend(/*IDD_PROPPAGE_LEGEND*/);

			m_pSetupData->SetVersion("SET_");

			//we keep the color info up here so tell each page where to find it
			m_pSummaryPage->StaticColors = SummaryStaticColors;
///			m_pGeneral4StatPage->StaticColors = General4StaticColors;
			m_pDataStatus->StaticColors = DataStaticColors;
			m_pDataIStatus->StaticColors = DataIStaticColors;
			m_pData2Status->StaticColors = Data2StaticColors;
			m_pUGInfo->StaticColors = UGInfoStaticColors;
			m_pPSBInfo->StaticColors = PSBInfoStaticColors;
			m_pBMBInfo->StaticColors = BMBInfoStaticColors;
			m_pESBInfo->StaticColors = ESBInfoStaticColors;
			m_pUser4ProgStatus->StaticColors = User4StaticColors;

			char szTemp[MAX_PATH];
			GetPrivateProfileString(m_pName,"DATADISPLAYVECTOR","10010010010010",szTemp,sizeof(szTemp),m_szIniFile);
			for (int i = 0; i < 14; i++)
				m_pDataStatus->m_bCheckVector[i] = (szTemp[i]=='1');

			m_pPropertySheet->AddPage(m_pSummaryPage);
///			m_pPropertySheet->AddPage(m_pGeneral4StatPage);
			m_pPropertySheet->AddPage(m_pDataIStatus);
			m_pPropertySheet->AddPage(m_pDataStatus);
			m_pPropertySheet->AddPage(m_pData2Status);
			m_pPropertySheet->AddPage(m_pUGInfo);
			m_pPropertySheet->AddPage(m_pESBInfo);
			m_pPropertySheet->AddPage(m_pBMBInfo);
			m_pPropertySheet->AddPage(m_pPSBInfo);
			m_pPropertySheet->AddPage(m_pUser4ProgStatus);
			m_pPropertySheet->AddPage(m_pSetupData);
			m_pPropertySheet->AddPage(m_pModParams);
			m_pPropertySheet->AddPage(m_pLegend);

			m_pPropertySheet->Create( this, //NULL, //DR00142
				WS_POPUP | WS_CAPTION | DS_MODALFRAME
				| WS_MINIMIZEBOX|WS_OVERLAPPED|WS_SYSMENU,
				0);

			m_pSummaryPage->SetPropertySheet(m_pPropertySheet);
///			m_pGeneral4StatPage->SetPropertySheet(m_pPropertySheet);
			m_pDataStatus->SetPropertySheet(m_pPropertySheet);
			m_pDataIStatus->SetPropertySheet(m_pPropertySheet);
			m_pData2Status->SetPropertySheet(m_pPropertySheet);
			m_pUser4ProgStatus->SetPropertySheet(m_pPropertySheet);
			m_pSetupData->SetPropertySheet(m_pPropertySheet,m_pName);
			m_pModParams->SetPropertySheet(m_pPropertySheet,m_pName);
			m_pUGInfo->SetPropertySheet(m_pPropertySheet);
			m_pESBInfo->SetPropertySheet(m_pPropertySheet);
			m_pBMBInfo->SetPropertySheet(m_pPropertySheet);
			m_pPSBInfo->SetPropertySheet(m_pPropertySheet);

			//necessary because CPropertySheet doesn't creat a page until 
			//  it needs to display it.  So force it to create them all
			for (i = m_pPropertySheet->GetPageCount();i>0;i--)
				m_pPropertySheet->SetActivePage(i-1);

			//set the summary page "scrolling" information
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND6,m_cSummaryRecent[5]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND5,m_cSummaryRecent[4]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND4,m_cSummaryRecent[3]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND3,m_cSummaryRecent[2]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND2,m_cSummaryRecent[1]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND1,m_cSummaryRecent[0]);
/*
			((CButton*)GetDlgItem(IDC_CHECKC0CR))->SetCheck(m_pDataStatus->m_bCheckVector[0]);
			((CButton*)GetDlgItem(IDC_CHECKC0SCR))->SetCheck(m_pDataStatus->m_bCheckVector[1]);
			((CButton*)GetDlgItem(IDC_CHECKC0LCR))->SetCheck(m_pDataStatus->m_bCheckVector[2]);
			((CButton*)GetDlgItem(IDC_CHECKC1CR))->SetCheck(m_pDataStatus->m_bCheckVector[3]);
			((CButton*)GetDlgItem(IDC_CHECKC1SCR))->SetCheck(m_pDataStatus->m_bCheckVector[4]);
			((CButton*)GetDlgItem(IDC_CHECKC1LCR))->SetCheck(m_pDataStatus->m_bCheckVector[5]);
			((CButton*)GetDlgItem(IDC_CHECKC2CR))->SetCheck(m_pDataStatus->m_bCheckVector[6]);
			((CButton*)GetDlgItem(IDC_CHECKC2SCR))->SetCheck(m_pDataStatus->m_bCheckVector[7]);
			((CButton*)GetDlgItem(IDC_CHECKC2LCR))->SetCheck(m_pDataStatus->m_bCheckVector[8]);
			((CButton*)GetDlgItem(IDC_CHECKC3CR))->SetCheck(m_pDataStatus->m_bCheckVector[9]);
			((CButton*)GetDlgItem(IDC_CHECKC3SCR))->SetCheck(m_pDataStatus->m_bCheckVector[10]);
			((CButton*)GetDlgItem(IDC_CHECKC3LCR))->SetCheck(m_pDataStatus->m_bCheckVector[11]);
			((CButton*)GetDlgItem(IDC_CHECKCPUTEMP))->SetCheck(m_pDataStatus->m_bCheckVector[12]);
			((CButton*)GetDlgItem(IDC_CHECKCPUTEMP2))->SetCheck(m_pDataStatus->m_bCheckVector[13]);
*/
			//set the legend page text
			GetModuleFileName(AfxGetInstanceHandle(),szTemp,sizeof(szTemp));
			char *c = strrchr(szTemp,'\\');//find the last "\"
			*c = '\0';						//make it null
			strcat(szTemp,"\\");
			
			CString cTemp(szTemp);
			cTemp += m_pName;
			cTemp += ".rtf";
			if (_access(cTemp,4) == 0)
				m_pLegend->SetText((LPSTR)LPCSTR(cTemp));
			else
			{
				cTemp = szTemp;
				cTemp += "uGRAND.rtf";
				if (_access(cTemp,4) == 0)
					m_pLegend->SetText((LPSTR)LPCSTR(cTemp));
				else
				{
					cTemp = szTemp;
					cTemp += "µGRAND.rtf";
					if (_access(cTemp,4) == 0)
						m_pLegend->SetText((LPSTR)LPCSTR(cTemp));
				}
			}

			//set the button
			ULONG RGBValue;
			m_pButton->get_FaceColor(&RGBValue);
			ULONG numeric;
			m_pButton->get_Numeric(&numeric);

			m_pPropertySheet->m_pCloseButton->SetFaceColor(RGBValue,false);
			m_pPropertySheet->m_pCloseButton->SetNumeric(numeric,false);
			if (RGBValue == COLOR_PAUSE)
				m_pPropertySheet->m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
			else
				m_pPropertySheet->m_pPauseButton->SetWindowText(COLLECTINGPAUSE);

			//go get the data for the "green bar" page
			szTemp[0] = NULL;
			CUGrandSetupData cUGrandSetupData;
			char *pszDescription;
			char *pszIniName;
			char *pszDefault;
			char *pszCommand;
//			while ((pszDescription = cUGrandSetupData.GetNext())!=NULL)
//			{
//				pszIniName = cUGrandSetupData.GetNext();
//				pszDefault = cUGrandSetupData.GetNext();
//				pszCommand = cUGrandSetupData.GetNext();
//				GetPrivateProfileString(m_pName,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
//				m_pModParams->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
//				m_pSetupData->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
//			}
			while ((pszDescription = cUGrandSetupData.GetNext())!=NULL)
			{
				pszIniName = cUGrandSetupData.GetNext();
				pszDefault = cUGrandSetupData.GetNext();
				pszCommand = cUGrandSetupData.GetNext();
				GetPrivateProfileString(m_pName,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
				m_pModParams->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
				m_pSetupData->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
			}

			m_pPropertySheet->SetLimits(
				m_fbatvolth, m_fbatvoltl,
				m_fsup_p25h, m_fsup_p25l,
				m_fsup_p33h, m_fsup_p33l,
				m_fsup_p05h, m_fsup_p05l,
				m_dtime_err, m_pID);

			m_pPropertySheet->SetHLVoltageDisplay(
				m_fHighBV, m_fLowBV, m_bFirstBV,
				m_fHighP25, m_fLowP25, m_bFirstP25,
				m_fHighP33, m_fLowP33, m_bFirstP33,
				m_fHighP5, m_fLowP5, m_bFirstP5);

///			m_pGeneral4StatPage->SetDlgItemText(IDC_GENERAL4_CHECKSUM, GetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM));
			m_pDataIStatus->SetDlgItemText(IDC_DATAI_CHECKSUM, GetString(DATAIPAGE,IDC_DATAI_CHECKSUM));

			m_pModParams->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(m_bPause?TRUE:FALSE);
	
			UpdateStrings();

			//need to do this one before UpdateDisplay
			//because UpdateDisplay may want to change colors
			m_pSetupData->UpdateColors(Setup4StaticColors);

			m_pPropertySheet->UpdateTabs();

			UpdateDisplay();

			m_pModParams->OnModparamsReset();

			m_pPropertySheet->Invalidate(false);
		}

		m_pPropertySheet->SetWindowPos(&CWnd::wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	}
	else
	{
		AfxMessageBox("Close a µGRAND Instrument window and try again");
	}
}

void
CInstrument::AckButton()
{
	FlagComm(0,false);
	FlagOthr(0,false);
	FlagTime(0,false);
	m_pButton->ShowIcon_Bell(VARIANT_FALSE);
	m_pButton->ShowIcon_Square(VARIANT_FALSE);
	m_pButton->ShowIcon_Disk(VARIANT_FALSE);
	m_pButton->put_Flash(VARIANT_FALSE);

//	m_iGeneral_Tab		= 0;
///	m_iGeneral4_Tab		= 0;
	m_iData_Tab			= 0;
	m_iDataI_Tab		= 0;
	m_iData2_Tab		= 0;
//	m_iUserProg_Tab		= 0;
	m_iUser4Prog_Tab	= 0;
//	m_iInstSetup_Tab	= 0;
	m_iInst4Setup_Tab	= 0;
	m_iUGInfo_Tab		= 0;
	m_iBMBInfo_Tab		= 0;
	m_iPSBInfo_Tab		= 0;
	m_iESBInfo_Tab		= 0;

	int i;
	for (i = 0; i < sizeof(SummaryStaticColors)/sizeof(COLORREF); i++)
		SummaryStaticColors[i] = COLOR_DEFAULT_TEXT;
//	for (i = 0; i < sizeof(GeneralStaticColors)/sizeof(COLORREF); i++)
//		GeneralStaticColors[i] = COLOR_DEFAULT_TEXT;
///	for (i = 0; i < sizeof(General4StaticColors)/sizeof(COLORREF); i++)
///		General4StaticColors[i] = COLOR_DEFAULT_TEXT;
//	for (i = 0; i < sizeof(UserStaticColors)/sizeof(COLORREF); i++)
//		UserStaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(User4StaticColors)/sizeof(COLORREF); i++)
		User4StaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(DataStaticColors)/sizeof(COLORREF); i++)
		DataStaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(Data2StaticColors)/sizeof(COLORREF); i++)
		Data2StaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(DataIStaticColors)/sizeof(COLORREF); i++)
		DataIStaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(Setup4StaticColors)/sizeof(COLORREF); i++)
		Setup4StaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(BMBInfoStaticColors)/sizeof(COLORREF); i++)
		BMBInfoStaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(PSBInfoStaticColors)/sizeof(COLORREF); i++)
		PSBInfoStaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(ESBInfoStaticColors)/sizeof(COLORREF); i++)
		ESBInfoStaticColors[i] = COLOR_DEFAULT_TEXT;
//	for (i = 0; i < sizeof(SetupStaticColors)/sizeof(COLORREF); i++)
//		SetupStaticColors[i] = COLOR_DEFAULT_TEXT;

	WriteToFile(TO_PFM,TYPE_COMP,NULL,"00000 Flags acknowledged by user. [FLAGS all off]\r\n");
}
