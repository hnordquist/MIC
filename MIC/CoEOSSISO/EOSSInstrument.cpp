// EOSSInstrument.cpp

#include "stdafx.h"
#include "resource.h"
#include "ISO_definitions.h"
#include "CSO_Definitions.h"
#include "ccbutton.h"
#include "EOSSInstrument.h"
#include "EOSSSafePS.h"
#include "EOSSDataStat.h"
#include "EOSSModParam.h"
#include "ISO_definitions.h"
#include "INI_definitions.h"
#include "colors.h"
#include "TPropertySheet.h"
#include "EditEOSSList.h"
#include  <math.h>
#include <sys/stat.h>
//#include <atlfile.h>

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

#define COLOR_GREENRED	1
#define COLOR_BLUERED   2
#define COLOR_REDYELLOW 3

#define REGISTRYLOCATION "SOFTWARE\\Los Alamos National Laboratory\\MIC_GM\\"

#define POWER_BIT(stat) (stat.Flags & FLAG_EXT_POWER)

// jfl todo make these two into ini file entries
// approx. 1 minute wait between registry checks
#define CHECKPOINTTICKS	59


const char* levelnames[eQueryLevelMax + 1] = 
{"Existence", "SealChallenge", "SealStatus", "SealStatusAndLog", "SealStatusAndLogAndLastReadDelta", "ReadAllData", "QueryLevelMax"};

//Update Params gets called if initializing an instrument and after modifying it
bool CEOSSInstrument::UpdateParams(bool resetting)
{
	//get the file id to be used
	GetPrivateProfileString(m_pName,FILE_ID,"",m_pID,sizeof(m_pID),m_szIniFile);

	//get the file location to save data at
	GetPrivateProfileString(m_pName,SAVE_LOC,"C:\\DATA",m_szSaveLoc,sizeof(m_szSaveLoc),m_szIniFile);

	//get the DO_DUMP from the ini file
	//	m_bDoDump = GetPrivateProfileInt(m_pName,"DO_DUMP",0,m_szIniFile)==1;

	//get the LOG_MII from the ini file
	//	m_bLogMII = GetPrivateProfileInt(m_pName,"LOG_MII",1,m_szIniFile)==1;

	//	//get do binary dump bbm
	//	m_bDoBinaryBBM = GetPrivateProfileInt(m_pName,"BINARY_BBM",0,m_szIniFile)==1;
	//
	//	//get the LOG_FILTER from the ini file
	//	m_bLogFilter = GetPrivateProfileInt(m_pName,"LOG_FILTER",1,m_szIniFile)==1;

	//get the allow auto time set flag from the ini file
	//	m_bAllowAutoTime = GetPrivateProfileInt(m_pName,"AUTOTIMESET",1,m_szIniFile)==1;

	//get the SLOWRESET from the ini file
	//	m_dSlowReset = GetPrivateProfileInt(m_pName,"SLOW_RESET",900,m_szIniFile);
	//	m_dSlowReset *= 1000;

	//get the port string from the ini file
	//if we are resetting and it has changed then we must unregister
	//and reregister
	char tempport[32];
	if (resetting) strcpy(tempport,m_pPort);
	bool portchange;
	GetPrivateProfileString(m_pName,"PORT","",m_pPort,sizeof(m_pPort)-1,m_szIniFile);
	if (strcmp(tempport,m_pPort)==0)
		portchange = false;
	else
		portchange = true;


	//get the maximum pause of this instrument
	int maxpause = GetPrivateProfileInt(m_pName,"MAXPAUSE",30000,m_szIniFile);
	if (maxpause < 0) maxpause = 0;
	m_iPauseMax = maxpause * 1000;	//dgp 18 Oct 2006

	//get the data cycle time
	//	m_dCycleInt = GetPrivateProfileInt(m_pName,"MAXCYCLE",1000,m_szIniFile)*1000;
	//	if (m_dCycleInt < 500) m_dCycleInt = 500;
	m_dCycleInt = 1000;

	//get the maxinquire interval
	m_dInquireInt = GetPrivateProfileInt(m_pName,"MAXINQUIRE",10,m_szIniFile);
	if (m_dInquireInt < 1) m_dInquireInt = 1;

	m_ulBatteryWarn = GetPrivateProfileInt(m_pName,"MAIN_BATT_LOW",MBATVOLTL_SPIN_DEF,m_szIniFile);
	m_ulBatteryCrit = GetPrivateProfileInt(m_pName,"MAIN_BATT_CRIT",MBATVOLTC_SPIN_DEF,m_szIniFile);
	m_ulEBatteryWarn = GetPrivateProfileInt(m_pName,"EMER_BATT_LOW",EBATVOLTL_SPIN_DEF,m_szIniFile);
	m_ulEBatteryCrit = GetPrivateProfileInt(m_pName,"EMER_BATT_CRIT",EBATVOLTC_SPIN_DEF,m_szIniFile);
	m_ulEBatteryCrit = GetPrivateProfileInt(m_pName,"EMER_BATT_CRIT",EBATVOLTC_SPIN_DEF,m_szIniFile);

	// minute-based automatic check count for a registry change before flagging seal connectivity as failed red
	m_uHardFailCount = GetPrivateProfileInt(m_pName,"HARDWARE_COMM_FAILURE_MINUTES",120,m_szIniFile);
	// number of actual read attempts with the reader, before flagging seal connectivity as failed red
	m_uHardFailCheckCycles = GetPrivateProfileInt(m_pName,"HARDWARE_COMM_FAILURE_READ_TRIES",2,m_szIniFile);

	m_uFullDataInterval = GetPrivateProfileInt(m_pName,FULL_DATA_INTERVAL,1,m_szIniFile);

	if (resetting)
	{
		m_bReSchedule = true;

		UpdateStrings();

		//		m_pPropertySheet->SetLimits(m_dtime_err, m_pID);

		m_pModParams->OnModparamsReset();  

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

void CEOSSInstrument::UpdateStrings()
{
	//create the string for the title bar
	CreateFullNameString();  //in ISO base class
	m_pPropertySheet->SetWindowText(m_pFullName);
}


void CEOSSInstrument::DriveToZero(bool bDriveToZero, bool bInPwrFail)
{
	if (bInPwrFail)
	{
		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, 
			"47205 EOSS COLLECT Power Fail");
	}
	PauseInstrument(false);
	m_dTickToInquire = m_dInquireInt;
}

void CEOSSInstrument::IssuePrint(FILE* outfile)
{
	if (m_pPropertySheet)
		m_pPropertySheet->IssuePrint(outfile);

	fprintf(outfile,"MIC EOSS Instrument Status:\n");
	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
	fprintf(outfile,"GENERAL\n");
	fprintf(outfile," - ALL:\n%s",sGeneral.cAll);
	fprintf(outfile," - %s\n", sGeneral.cCOMPORT);
	fprintf(outfile," - %s\n", sGeneral.cPath);
	fprintf(outfile," - %s\n", sGeneral.cFORMAT);
	fprintf(outfile," - %s\n", sGeneral.cINSTRUMENT_ID);
	fprintf(outfile," - %s\n", sGeneral.cLATEST_READ);
	fprintf(outfile," - %s\n", sGeneral.cSPECIFIED_EOSSes);
	fprintf(outfile," - %s\n", sGeneral.cLATEST_RESULTS);
	fprintf(outfile," - %s\n", sGeneral.cREADER_VERSION);
	fprintf(outfile," - %s\n", sGeneral.cTimeOffset);

	int iIndex = 0;
	while ((EOSSes[iIndex] != 0) && (iIndex < 32))
	{
		fprintf(outfile,"\n - EOSS S/N: %u\n",EOSSes[iIndex]);
		fprintf(outfile,"   - RED ITEM COUNT: %d\n",sStatus[iIndex].ucRedCount);
		fprintf(outfile,"   - YELLOW ITEM COUNT: %d\n",sStatus[iIndex].ucYellowCount);

		//(sStatus[iIndex].ucBlueCount > 0 ? 
		//	fprintf(outfile,"   - BLUE ITEM COUNT: %d\n",sStatus[iIndex].ucBlueCount) : 0);

		fprintf(outfile,"   - ALL: %s\n",sStatus[iIndex].cAll);
		fprintf(outfile,"   - %s",CleanString(sStatus[iIndex].cLatestRead));
		fprintf(outfile,"   - %s",CleanString(sStatus[iIndex].cLatestResults));
		fprintf(outfile,"   - %s",CleanString(sStatus[iIndex].cKeyInfo));
		fprintf(outfile,"   - %s",CleanString(sStatus[iIndex].cVersion));
		fprintf(outfile,"   - %s",CleanString(sStatus[iIndex].cStatus));
		fprintf(outfile,"   - %s",CleanString(sStatus[iIndex].cLogInfo));
		fprintf(outfile,"   - %s",CleanString(sStatus[iIndex].cSealLog));
		fprintf(outfile,"   - %s",CleanString(sStatus[iIndex].cSOHLog));
		fprintf(outfile,"   - %s",CleanString(sStatus[iIndex].cSystemLog));
		iIndex++;
	}
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
	fprintf(outfile,"========================================================\n");
}

CString CEOSSInstrument::CleanString(CString cString)
{
	CString cResult(cString);
	cResult.TrimLeft();
	cResult.Replace("\r\n","\n");
	cResult.Replace("\n\n","\n");
	return cResult;
}

void CEOSSInstrument::Reset()
{
	if (IsPaused())
		m_dCurrent_State = eStartSchtasksCheckWrite;
}

bool CEOSSInstrument::EditEOSSList()
{
	if (m_bEditActive)
		return false;
	bool bReturn = false;
	CEditEOSSList cEditEOSSList;
	char szEOSSes[512];
	GetPrivateProfileString(m_pName,"EOSSes","",szEOSSes,sizeof(szEOSSes),m_szIniFile);
	cEditEOSSList.m_czEOSSes = szEOSSes;
	m_bEditActive = true;
	try
	{
		if (cEditEOSSList.DoModal() == IDOK)
		{
			//do what needs to be done to copy list into either the REG and/or the INI
			int iIndex;
			char* szAt;
			strcpy(szEOSSes,LPCSTR(cEditEOSSList.m_czEOSSes));
			szAt = strtok(szEOSSes,",");
			iIndex = 0;
			while (szAt)
			{
				sscanf(szAt,"%d",&EOSSes[iIndex++]);
				szAt = strtok(NULL,",");
			}
			sGeneral.ucSPECIFIED_EOSSes = iIndex;		
			for (;iIndex < CEditEOSSList::maxEOSSCount; iIndex++)
				EOSSes[iIndex] = 0;

			for (iIndex = 0; iIndex < CEditEOSSList::maxEOSSCount; iIndex++)
			{
				sStatus[iIndex].ucRedCount = 0;
				sStatus[iIndex].ucYellowCount = 0;
			}

			WritePrivateProfileString(m_pName,"EOSSes",cEditEOSSList.m_czEOSSes,m_szIniFile);
			sGeneral.cSPECIFIED_EOSSes = cEditEOSSList.m_czEOSSes;

			InitArrays();
			if (m_pSummaryPage)
			{
				m_pSummaryPage->PopulateChecker();
				m_pSummaryPage->Invalidate(TRUE);
			}
			if (m_pModParams)
			{
				m_pModParams->SetEOSSEstimateListTime();
				m_pModParams->Invalidate(TRUE);
			}
			bReturn = true;
		}
	}
	catch (...)
	{
	}
	m_bEditActive = false;
	return bReturn;
}

CEOSSInstrument::CEOSSInstrument()
{
	//	TRACE("CEOSSInstrument::CTOR #1\n");
	m_pParent = NULL;
	FinishConstruction();
}

CEOSSInstrument::CEOSSInstrument(CWnd* Parent, char *inifile) : CISO(inifile)
{
	//	TRACE("CEOSSInstrument::CTOR #2\n");
	m_pParent = Parent;
	FinishConstruction();
}

void CEOSSInstrument::FinishConstruction()
{
	m_eCategory = STANDARD_ISO;
	m_bReSchedule = false;
	m_bRunNow = false;
	m_bRemoveOld = false;
	m_bUpdateDataDisplay = false;
	m_bEditActive = false;
	m_iColorFlagS = 0;
	m_iColorFlagG = 0;
	strncpy(m_pType, EOSS_DISCOVERY_TYPE, sizeof(m_pType));
	strcpy(m_szLatestRead,"");

	CString csComma(",");
	CString csExtensions = 
		CString(ESS_SUFFIX) + csComma + 
		PFM_SUFFIX + csComma +	
		SOH_SUFFIX + csComma +	
		CEV_SUFFIX + csComma + CString("RLOG");				

	strncpy(m_szFileExtensionsList,(LPCTSTR)csExtensions,sizeof(m_szFileExtensionsList));

	m_pSummaryPage		= NULL;
	m_pDataStatus		= NULL;
	m_pModParams		= NULL;
	m_pLegend			= NULL;
	m_pPropertySheet	= NULL;

	m_bGoodRegister		= false;
	m_bIsPaused = true;
	m_bPrimed = false;

	m_bHardFailResults = m_bHardFail = m_bHWNotified = false;
	m_uReaderCycles = m_uReaderCheckCount = m_uReaderCompletionCount = 0;
	m_uHardFailCount = 120;
	m_uHardFailCheckCycles = 2;
	m_bTokLibTrigger = true;
	m_uFullDataInterval = 1;  // default is gather full data each time

	// running in compatibility mode this is always true LOL
	OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
	m_XPOrMilTime = osvi.dwMajorVersion <= 5;
}


CEOSSInstrument::~CEOSSInstrument()
{
	//	TRACE("CEOSSInstrument::DTOR\n");

	//tell the world we are shutting down
	char str[512];		//dr00115
	char str2[128];

	if (m_pPropertySheet)
	{
		strcpy(str,"47403 EOSS COLLECT stopped:");
		unsigned long size = sizeof(str2);
		if (::GetUserName(str2,&size))
		{
			strcat(str," Logon ID:");
			strcat(str,str2);
		}
		strcat(str," MIC User ID:");		
		GetPrivateProfileString(CONFIGURATION,LAST_VALIDATION,"UNKNOWN",str2,sizeof(str2),m_szIniFile);
		strcat(str,str2);														
		//		strcat(str,"\r\n");														

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


BEGIN_MESSAGE_MAP(CEOSSInstrument, CWnd)
	//{{AFX_MSG_MAP(CEOSSInstrument)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE,OnReceive)
END_MESSAGE_MAP()


void CEOSSInstrument::OnTimer(UINT nIDEOSS) 
{
	//if we have a timer tick then notify the state machine
	//if we have a pause timer rundown then UN-pause this instrument
	switch (nIDEOSS)
	{
		//max pause time ran out so we need to un-pause this instrument
	case TIMER_PAUSE:	
		PauseInstrument(false);
		break;

		//	case TIMER_RUNNOW:
		//		KillTimer(TIMER_RUNNOW);
		//		m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW)->EnableWindow(TRUE);
		//		break;

		//normal timer increment -- need to send this to the state machine
	case TIMER_TICK:
		PostMessage(uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
		break;
	}
	//let the system handle all other timer messages 
	CWnd::OnTimer(nIDEOSS);
}

void CEOSSInstrument::OnAck()
{
	m_iFlagTime = 0;
	m_iFlagOthr = 0;
	m_iFlagComm = 0;

	WritePrivateProfileString(m_pName,FLAGTIME,"0",m_szIniFile);
	WritePrivateProfileString(m_pName,FLAGOTHR,"0",m_szIniFile);
	WritePrivateProfileString(m_pName,FLAGCOMM,"0",m_szIniFile);

	m_pButton->ShowIcon_Clock(VARIANT_FALSE);
	m_pButton->ShowIcon_Frown(VARIANT_FALSE);
	m_pButton->ShowIcon_Error(VARIANT_FALSE);

	m_pButton->ShowIcon_Square(VARIANT_FALSE);
	m_pButton->ShowIcon_Disk(VARIANT_FALSE);
	m_pButton->ShowIcon_Bell(VARIANT_FALSE);
}

bool CEOSSInstrument::CreateInstrument()
{
	//	TRACE("CEOSSInstrument::CreateInstrument()\n");
	//	ASSERT(m_pName != NULL);
	//	ASSERT(m_pButton != NULL);
	//	int iColorFlag;

	m_bPause = true;

	if (!UpdateParams(false))	//This no longer check for the port
		return false;			// And may not be needed.

	//create CEOSSInstrument object -- we won't actually have a window
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
		m_pButton->put_DisplayNumericString(VARIANT_FALSE,10);
		m_pButton->put_FaceColor(COLOR_PAUSE);
		m_pButton->put_Flash(VARIANT_FALSE);
		//		m_pButton->put_Numeric(0);

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

		InitArrays();
		//		ReadRegistry();

		//create each of the pages we'll need in the property sheet
		m_pSummaryPage     = new CEOSSSummary(IDD_EOSSSUMMARY_PAGE,this);
		m_pDataStatus      = new CEOSSDataStat(IDD_EOSSDATASTATUS_PAGE,this);
		m_pModParams       = new CEOSSModParam(IDD_EOSSMODPARAMS_PAGE,this,false);
		m_pLegend		   = new CLegend(/*IDD_PROPPAGE_LEGEND*/);


		//create the user interface dialog box for this instrument
		m_pPropertySheet   = new CEOSSPropertySheet(LPCTSTR(m_pName),this);

		m_pSummaryPage->SetPropertySheet(m_pPropertySheet);
		m_pDataStatus->SetPropertySheet(m_pPropertySheet);
		m_pModParams->SetPropertySheet(m_pPropertySheet);

		m_pPropertySheet->AddPage(m_pSummaryPage);
		m_pPropertySheet->AddPage(m_pDataStatus);
		m_pPropertySheet->AddPage(m_pModParams);
		m_pPropertySheet->AddPage(m_pLegend);

		DWORD dwStyle = 
			WS_POPUP | WS_CAPTION | DS_MODALFRAME |
			WS_MINIMIZEBOX|WS_OVERLAPPED|WS_SYSMENU;

		m_pPropertySheet->Create(this,dwStyle,0);

		m_pSummaryPage->SetPropertySheet(m_pPropertySheet);
		m_pDataStatus->SetPropertySheet(m_pPropertySheet);
		m_pModParams->SetPropertySheet(m_pPropertySheet);

		//necessary because PropertySheet doesn't creat a page until 
		//  it needs to display it.  So force it to create them all
		m_pPropertySheet->SetActivePage(m_pLegend);
		m_pPropertySheet->SetActivePage(m_pModParams);
		m_pPropertySheet->SetActivePage(m_pSummaryPage);
		m_pPropertySheet->SetActivePage(m_pDataStatus);


		//
		//GET THIS DATA FROM THE MODIFY PARAMETERS TAB
		//
		m_pPropertySheet->SetTabColor(IDC_EOSSDATA_TAB,
			((m_iColorFlagS | m_iColorFlagG) == 0)?COLOR_GREEN:
			((m_iColorFlagS | m_iColorFlagG) == 1)?COLOR_YELLOW:
			((m_iColorFlagS | m_iColorFlagG) == 2)?COLOR_RED:
			((m_iColorFlagS | m_iColorFlagG) == 3)?COLOR_REDYELLOW:COLOR_WHITE); // jfl, note map of status 3 to image 10, m_iColorFlagS set *only* by ValidateSeal

		if ((m_iColorFlagS | m_iColorFlagG) & 0x02) 
			m_pButton->ShowIcon_Square(VARIANT_TRUE);
		else if ((m_iColorFlagS | m_iColorFlagG) & 0x01)
		{
			m_pButton->ShowIcon_Error(VARIANT_TRUE);
		}

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
			cTemp += "EOSS.rtf";
			if (_access(cTemp,4))
			{
				cTemp = 
					"Instrument Button Colors...\n"
					"GREEN_ICON\nNormal state.\n"
					"---------------------------\n"
					"YELLOW_ICON\nMIC_EOSSReader is in the process of reading the seals.\n"
					"---------------------------\n"
					"RED_ICON\nCommunications Failure! 1) The seals are not responding to commands, despite several attempts to make contact, or\n"
					"                                    2) MIC cannot read the seal data from the Windows registry.\n"
					"---------------------------\n"
					"WHITE_ICON\nFLASHING to previous color. The disk is full or the instrument has a failure which is causing data loss\n"
					"---------------------------\n"
					"GRAY_ICON\nMIC has been instructed not to continue looking for changes in the registry. In all cases this state eventually reverts to a normal operational state.\n"
					"===========================\n"
					"\n"
					"Instrument Button ICONs...\n"
					"SQUARE_ICON Square: CRITICAL: One or more of the items in the Data Status page need IMMEDIATE attention.\n"
					"---------------------------\n"
					"CLOCK_ICON Clock: The difference between MIC's time and the instrument's time is greater than the user set tolerance.\n"
					"---------------------------\n"
					"FLAG_ICON Flag:  WARNING: One or more of the items in the Data Status page need attention\n"
					"---------------------------\n"
					"FACE_ICON Face: The instrument has reported a power problem, or the instrument is no longer communicating.\n"	
					"---------------------------\n"
					"FLOPPY_ICON Floppy Disk & Flashing: MIC has stopped writing to disk and talking to the instrument. This can be caused by the hard drive being too full, the maximum use of the hard drive set too low, or disk write failure.\n"
					"---------------------------\n"
					"SKULL_ICON Skull and Cross Bones: N/A.\n"
					"---------------------------\n"
					"BOMB_ICON Bomb: Configuration Error. MIC is not configured correctly. To correct this problem, edit the MIC.INI file or delete the instrument object and recreate it.\n";
			}	
		}
		m_pLegend->SetText((LPSTR)LPCSTR(cTemp));

		UpdateStrings();

		//set the time to start data collecting 
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);	//dgp 18 Oct 2006

		m_pModParams->OnModparamsReset();

		m_dCurrent_State = eStartSchtasksCheckWrite;
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
			VARIANT_FALSE,  
			&vbResult);	

		m_bGoodRegister = (vbResult == VARIANT_TRUE);

		m_pNow = COleDateTime::GetCurrentTime();

		//tell the files that mic is starting for this instrument
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, 
			(m_bNormalShutdown)?TYPE_START:TYPE_ABNORMAL);


	}  //if result

	return m_bGoodRegister;
}

void CEOSSInstrument::InitArrays()
{
	//no known EOSSes yet
	memset(EOSSes,NULL,sizeof(EOSSes));

	//set all to the red icon
	sGeneral.ucAll = // jfl added by dp
		sGeneral.ucCOMPORT = sGeneral.ucFORMAT = sGeneral.ucINSTRUMENT_ID = sGeneral.ucSPECIFIED_EOSSes =
		sGeneral.ucLATEST_READ = sGeneral.ucLATEST_RESULTS = sGeneral.ucPATH = sGeneral.ucREADER_VERSION = 6;

	sGeneral.cCOMPORT =
		sGeneral.cPath =
		sGeneral.cFORMAT =
		sGeneral.cINSTRUMENT_ID =
		sGeneral.cLATEST_READ =
		sGeneral.cSPECIFIED_EOSSes =
		sGeneral.cLATEST_RESULTS =
		sGeneral.cREADER_VERSION = "Not yet read from registry";

	for (int i = 0; i < MAXEOSSES; i++)
	{
		sStatus[i].cAll.Empty();

		sStatus[i].ucInstID    = sStatus[i].ucKeyInfo = sStatus[i].ucLatestRead = sStatus[i].ucLatestResults =
			sStatus[i].ucLogInfo   = sStatus[i].ucSealLog = sStatus[i].ucSOHLog     = sStatus[i].ucStatus = 
			sStatus[i].ucSystemLog = sStatus[i].ucVersion = 6;

		sStatus[i].cInstID    = sStatus[i].cKeyInfo = sStatus[i].cLatestRead = sStatus[i].cLatestResults =
			sStatus[i].cLogInfo   = sStatus[i].cSealLog = sStatus[i].cSOHLog     = sStatus[i].cStatus = 
			sStatus[i].cSystemLog = sStatus[i].cVersion = sStatus[i].cAll = "Not yet read from registry";

		sStatus[i].ucRedCount = 0;
		sStatus[i].ucYellowCount = 0;
		sStatus[i].ucBlueCount = 0;  // jfl used for seal open condition, not an error, but not routine
	}


	//// jfl added by dp
	//find out what EOSSes we currently have
	char szEOSSes[512];
	GetPrivateProfileString(m_pName,"EOSSes","",szEOSSes,sizeof(szEOSSes),m_szIniFile);
	sGeneral.cSPECIFIED_EOSSes = szEOSSes;

	//do what needs to be done to copy list into either the REG, INI or both
	int iIndex;
	char* szAt;
	szAt = strtok(szEOSSes,",");
	iIndex = 0;
	while (szAt)
	{
		sscanf(szAt,"%d",&EOSSes[iIndex]);
		sStatus[iIndex].uiEOSSSN = EOSSes[iIndex];
		iIndex++;
		szAt = strtok(NULL,",");
	}

	m_iColorFlagS = 4;
	m_iColorFlagG = 4;
}

bool CEOSSInstrument::RegChanged()
{
	bool bResults;

	LONG lRet;
	HKEY hKey;
	DWORD dwType;

	char szName[MAX_PATH];

	bResults = false;

	if (m_bUpdateDataDisplay) return true;

	//open the registry where the values are stored
	strcpy(szName,REGISTRYLOCATION);
	strcat(szName,m_pName);

	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szName,
		0,KEY_READ, &hKey);

	if (lRet == ERROR_SUCCESS)
	{
		char szLatestRead[32];
		unsigned long ulSize = sizeof(szLatestRead);

		lRet = RegQueryValueExA(hKey,"LATEST READ",NULL,
			&dwType,(BYTE*)szLatestRead,&ulSize);

		if (lRet == ERROR_SUCCESS)
		{
			if (strcmp(m_szLatestRead,szLatestRead)!=0)
			{
				bResults = true;
				strcpy(m_szLatestRead,szLatestRead);
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47408 EOSS Registry Entries Changed");
				if (m_uReaderCycles < UINT_MAX) m_uReaderCycles++; else m_uReaderCycles = 0; // auto-rollover
			}
		}
		RegCloseKey(hKey);
	}
	return bResults;
}

bool CEOSSInstrument::ReadRegistry()
{
	bool bResult;
	if ((bResult = RegChanged())==true)
	{
		InitArrays();
		PopulateArrays();
		if (m_pPropertySheet)
		{
			m_pPropertySheet->SetTabColor(IDC_EOSSDATA_TAB,
				((m_iColorFlagS | m_iColorFlagG) == 0)?COLOR_GREEN:
				((m_iColorFlagS | m_iColorFlagG) == 1)?COLOR_YELLOW:
				((m_iColorFlagS | m_iColorFlagG) == 2)?COLOR_RED:
				((m_iColorFlagS | m_iColorFlagG) == 3)?COLOR_REDYELLOW:COLOR_WHITE); // jfl, note map of status 3 to image 10, m_iColorFlagS set *only* by ValidateSeal
			if (m_pDataStatus)
				m_pDataStatus->PopulateTree();
		}

		if ((m_iColorFlagS | m_iColorFlagG) & 0x02) 
			m_pButton->ShowIcon_Square(VARIANT_TRUE);
		else if ((m_iColorFlagS | m_iColorFlagG) & 0x01)
		{
			m_pButton->ShowIcon_Error(VARIANT_TRUE);
		}
		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47407 EOSS Read Registry Entries");
	}
	return bResult;
}

int CEOSSInstrument::ValidateSeal(char* szItem,char*szFinal,unsigned long ulEOSSIndex)
{
	int iResult = 0;

	SYSTEMLOGPAGE sSystemLogPage;
	LOGINFO sLogInfoBuffer;
	STATUS sStatusBuffer;
	SOHLOGPAGE sSoHLogPage;
	SEALLOGPAGE sSealLogPage;
	PAGEHEADER sPageHeader;

	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szTemp[1024];
	char szPageName[16];
	unsigned long ulBuffSize;
	unsigned int uiSealEvents;
	unsigned int uiSystemEvents;
	unsigned int uiSoHRecords;

	sStatus[ulEOSSIndex].uiEOSSSN = atol(szFinal);
	sStatus[ulEOSSIndex].cAll.Empty();

	strcpy(szTemp,szItem);
	strcat(szTemp,"\\");
	strcat(szTemp,szFinal);

	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szTemp,
		0,KEY_QUERY_VALUE, &hKey);

	if (lRet == ERROR_SUCCESS)
	{
		//LATEST READ
		ulBuffSize = sizeof(szTemp);
		lRet = RegQueryValueExA(hKey,"LATEST READ",NULL,
			&dwType,(BYTE*)szTemp,&ulBuffSize);
		sStatus[ulEOSSIndex].cLatestRead = "LATEST READ...\r\n\r\n";
		if (lRet == ERROR_SUCCESS)
		{
			sStatus[ulEOSSIndex].ucLatestRead = 1;

			unsigned int uiYear,uiMon,uiDay,uiHour,uiMin,uiSec;
			COleDateTime cNow = COleDateTime::GetCurrentTime();
			char szSchINI[16];
			COleDateTimeSpan cSpan; 
			GetPrivateProfileString(m_pName,"AUTORUN","DAILY",szSchINI,sizeof(szSchINI),m_szIniFile);
			//cspan contains the cycle time plus 10 minutes to see if it has finished yet
			switch (toupper(szSchINI[0])) {
			case 'H'://hourly
				cSpan = COleDateTimeSpan(0,1,10,0);
				break;
			case 'D'://daily
				cSpan = COleDateTimeSpan(0,24,10,0);
				break;
			case 'W'://weekly
				cSpan = COleDateTimeSpan(7,0,10,0);
				break;
			case 'M'://monthly
				cSpan = COleDateTimeSpan(31,0,10,0);
				break;
			case 'N'://never???
				cSpan = COleDateTimeSpan(120,0,10,0);
				break;
			}
			COleDateTime cThen;
			if (sscanf(szTemp,"%u\\%u\\%u %u:%u:%u",
				&uiYear,&uiMon,&uiDay,&uiHour,&uiMin,&uiSec) == 6) //get 6 vars
			{
				sStatus[ulEOSSIndex].cLatestRead += szTemp;
				sStatus[ulEOSSIndex].cLatestRead += "\r\n\r\n";
				cThen = COleDateTime(uiYear,uiMon,uiDay,uiHour,uiMin,uiSec);
				//is now past last plus cycle time
				if (cThen+cSpan <= cNow)
				{
					sStatus[ulEOSSIndex].ucLatestRead = 11; // jfl the 11 here was originally a yellow triangle duplicate of bitmap/state 3
					sStatus[ulEOSSIndex].cAll += "WARNING: Read from seal is overdue!\r\n";
					sStatus[ulEOSSIndex].cLatestRead += "WARNING: Read from seal is overdue!";
					sStatus[ulEOSSIndex].ucRedCount++;
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47208 EOSS Read from seal is overdue!");
				}

			}
			else
			{
				sStatus[ulEOSSIndex].ucLatestRead = 0;
				sStatus[ulEOSSIndex].cAll += "WARNING: Date of LATEST READ corrupted!\r\n";
				sStatus[ulEOSSIndex].cLatestRead += "WARNING: Date of LATEST READ corrupted!";
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47300 EOSS Date of LATEST READ in registry corrupted");
			}
		}
		else
		{
			sStatus[ulEOSSIndex].ucLatestRead = 0;
			sStatus[ulEOSSIndex].cAll += "WARNING: Cannot read LATEST READ from registry!\r\n";
			sStatus[ulEOSSIndex].cLatestRead += "WARNING: Cannot read LATEST READ from registry!";
			sStatus[ulEOSSIndex].ucRedCount++;
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47202 EOSS Cannot read LATEST READ from registry");
		}

		//LATEST RESULTS
		ulBuffSize = sizeof(szTemp);
		lRet = RegQueryValueExA(hKey,"LATEST RESULTS",NULL,
			&dwType,(BYTE*)szTemp,&ulBuffSize);
		sStatus[ulEOSSIndex].cLatestResults = "LATEST RESULTS...\r\n\r\n";
		if (lRet == ERROR_SUCCESS)
		{
			sStatus[ulEOSSIndex].cLatestResults += szTemp;
			sStatus[ulEOSSIndex].cLatestResults += "\r\n";

			if (strncmp(szTemp,"No errors", 9) != 0)
			{
				sStatus[ulEOSSIndex].ucLatestResults = 0;
				sStatus[ulEOSSIndex].cAll += "WARNING: Errors occurred during LATEST READ from seal!\r\n";
				sStatus[ulEOSSIndex].cLatestResults += "WARNING: Errors occurred during LATEST READ from seal!";
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47302 EOSS Errors occurred during LATEST READ from seal");
			}
			else
			{
				sStatus[ulEOSSIndex].ucLatestResults = 1;
			}
		}
		else
		{
			sStatus[ulEOSSIndex].ucLatestResults = 0;
			sStatus[ulEOSSIndex].cAll += "WARNING: Cannot read LATEST RESULTS from registry!\r\n";
			sStatus[ulEOSSIndex].cLatestResults += "WARNING: Cannot read LATEST RESULTS from registry!";
			sStatus[ulEOSSIndex].ucRedCount++;
			m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47203 EOSS Cannot read LATEST RESULTS from registry!");
		}

		//KEY INFO
		sStatus[ulEOSSIndex].cKeyInfo = "KEY INFO...\r\n\r\n";
		unsigned int uiKeyInfo;

		ulBuffSize = sizeof(uiKeyInfo);
		lRet = RegQueryValueExA(hKey,"KEY NUMBER",NULL,
			&dwType,(BYTE*)&uiKeyInfo,&ulBuffSize);
		if (lRet == ERROR_SUCCESS)
		{
			sprintf(szTemp," - Number\t\t%u-%u\r\n",uiKeyInfo>>8,uiKeyInfo & 0x000000ff);
			sStatus[ulEOSSIndex].cKeyInfo += szTemp;
			ulBuffSize = sizeof(uiKeyInfo);
			lRet = RegQueryValueExA(hKey,"KEY TIME",NULL,
				&dwType,(BYTE*)&uiKeyInfo,&ulBuffSize);
			if (lRet == ERROR_SUCCESS)
			{
				sStatus[ulEOSSIndex].ucKeyInfo = 1;
				COleDateTimeSpan cSpan(0,0,0,uiKeyInfo);
				COleDateTime cTime(2000,1,1,0,0,0);
				cTime += cSpan;
				sStatus[ulEOSSIndex].cKeyInfo += cTime.Format(" - Date\t\t%Y\\%m\\%d %H:%M:%S\r\n");;
			}
			else
			{
				sStatus[ulEOSSIndex].ucKeyInfo = 0;
				sStatus[ulEOSSIndex].cAll += "WARNING: Cannot read KEY TIME from registry\r\n";
				sStatus[ulEOSSIndex].cKeyInfo += "WARNING: Cannot read KEY TIME from registry";
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47201 EOSS Cannot read KEY TIME from registry!");
			}

		}
		else
		{
			sStatus[ulEOSSIndex].ucKeyInfo = 0;
			sStatus[ulEOSSIndex].cAll += "WARNING: Cannot read KEY NUMBER from registry!\r\n";
			sStatus[ulEOSSIndex].cKeyInfo += "WARNING: Cannot read KEY NUMBER from registry!";
			sStatus[ulEOSSIndex].ucRedCount++;
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47200 EOSS Cannot read KEY NUMBER from registry!");
		}

		//VERSION
		sStatus[ulEOSSIndex].cVersion = "FIRMWARE VERSION...\r\n\r\n";
		unsigned short usVersion;
		ulBuffSize = sizeof(usVersion);

		lRet = RegQueryValueExA(hKey,"VERSION",NULL,
			&dwType,(BYTE*)&usVersion,&ulBuffSize);
		if (lRet == ERROR_SUCCESS)
		{
			sStatus[ulEOSSIndex].ucVersion = 1;
			sprintf(szTemp," - Major\t\t%u\r\n - Minor:\t\t%u\r\n",usVersion >> 8, usVersion & 0x00ff);
			sStatus[ulEOSSIndex].cVersion += szTemp;
		}
		else
		{
			sStatus[ulEOSSIndex].ucVersion = 0;
			sStatus[ulEOSSIndex].cAll += "WARNING: Cannot read VERSION from registry!\r\n";
			sStatus[ulEOSSIndex].cVersion += "WARNING: Cannot read VERSION from registry!";
			sStatus[ulEOSSIndex].ucRedCount++;
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47204 EOSS Cannot read VERSION from registry!");
		}

		//STATUS
		sStatus[ulEOSSIndex].cStatus = "STATUS...\r\n\r\n";
		ulBuffSize = sizeof(STATUS);
		lRet = RegQueryValueExA(hKey,"STATUS",NULL,
			&dwType,(BYTE*)&sStatusBuffer,&ulBuffSize);
		int iColorFlags = 0;
		if (lRet == ERROR_SUCCESS)
		{
			if (POWER_BIT(sStatusBuffer)) // jfl: bit set means no external power, and per IAEA, we show absence of external power as a RED failure state
			{
				iColorFlags |= 0x02;
				sStatus[ulEOSSIndex].cAll += "WARNING: No external power!\r\n";
				sStatus[ulEOSSIndex].cStatus += "WARNING: No external power!\r\n";
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47304 EOSS WARNING: No external power!");
			}
			else
			{
				sStatus[ulEOSSIndex].cStatus += "INFO: External power present.\r\n";
			}

			if (sStatusBuffer.Flags & FLAG_FOOPEN) // jfl: an open seal is not a yellow status, per Cesare, but should still be flagged visibly, use circle for OK and blue to distinguish
			{
				//iColorFlags |= 0x04;
				sStatus[ulEOSSIndex].cStatus += "INFO: Seal wire OPEN during latest read.\r\n";
				sStatus[ulEOSSIndex].ucBlueCount++;
			}
			else
			{
				sStatus[ulEOSSIndex].cStatus += "INFO: Seal wire CLOSED during latest read.\r\n";
			}

			if (sStatusBuffer.Flags & FLAG_FOFAIL)
			{
				iColorFlags |= 0x02;
				sStatus[ulEOSSIndex].cAll += "WARNING: Seal wire FAILURE!\r\n";
				sStatus[ulEOSSIndex].cStatus += "WARNING: Seal wire FAILURE!\r\n";
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47322 EOSS WARNING: Seal wire FAILURE!");
			}
			if (sStatusBuffer.Flags & FLAG_HOPEN)
			{
				iColorFlags |= 0x02;
				sStatus[ulEOSSIndex].cAll += "WARNING: Drill protection OPEN!\r\n";
				sStatus[ulEOSSIndex].cStatus += "WARNING: Drill protection OPEN!\r\n";
				sStatus[ulEOSSIndex].ucRedCount++;
			}
			if (sStatusBuffer.Flags & FLAG_SHORT)
			{
				iColorFlags |= 0x02;
				sStatus[ulEOSSIndex].cAll += "WARNING: Drill protection SHORTED!\r\n";
				sStatus[ulEOSSIndex].cStatus += "WARNING: Drill protection SHORTED!\r\n";
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47307 EOSS WARNING: Drill protection SHORTED!");
			}
			if (!(sStatusBuffer.Flags & FLAG_HOPEN) && !(sStatusBuffer.Flags & FLAG_SHORT))
			{
				sStatus[ulEOSSIndex].cStatus += "INFO: Drill protection SAFE.\r\n";
			}

			if (sStatusBuffer.BatteryConsumed/1000 > m_ulBatteryCrit)
			{
				iColorFlags |= 0x02;
				sStatus[ulEOSSIndex].cAll += "WARNING: Main battery CRITICALLY low!\r\n";
				sStatus[ulEOSSIndex].cStatus += "WARNING: Main battery CRITICALLY low!\r\n";
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pButton->ShowIcon_Frown(VARIANT_TRUE);
			}
			else if (sStatusBuffer.BatteryConsumed/1000 > m_ulBatteryWarn)
			{
				iColorFlags |= 0x01;
				sStatus[ulEOSSIndex].cStatus += "INFO: Main battery low!\r\n";
				sStatus[ulEOSSIndex].ucYellowCount++;
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47324 EOSS INFO: Main battery low!");
				m_pButton->ShowIcon_Frown(VARIANT_TRUE);
			}
			else
			{
				sStatus[ulEOSSIndex].cStatus += "INFO: Main battery OK.\r\n";
			}

			if (sStatusBuffer.EBatteryConsumed/1000 > m_ulEBatteryCrit)
			{
				iColorFlags |= 0x02;
				sStatus[ulEOSSIndex].cAll += "WARNING: Emergency battery CRITICALLY low!\r\n";
				sStatus[ulEOSSIndex].cStatus += "WARNING: Emergency battery CRITICALLY low!\r\n";
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47308 EOSS WARNING: Emergency battery CRITICALLY low!");
				m_pButton->ShowIcon_Frown(VARIANT_TRUE);
			}
			else if (sStatusBuffer.EBatteryConsumed/1000 > m_ulEBatteryWarn)
			{
				iColorFlags |= 0x01;
				sStatus[ulEOSSIndex].cStatus += "INFO: Emergency battery low!\r\n";
				sStatus[ulEOSSIndex].ucYellowCount++;
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47309 EOSS INFO: Emergency battery low!");
				m_pButton->ShowIcon_Frown(VARIANT_TRUE);
			}
			else
			{
				sStatus[ulEOSSIndex].cStatus += "INFO: Emergency battery OK\r\n";
			}

			if (sStatusBuffer.FlashWarnings)
			{
				iColorFlags |= 0x01;
				sprintf(szTemp,"INFO: %d Flash memory warnings!\r\n",sStatusBuffer.FlashWarnings);
				sStatus[ulEOSSIndex].cStatus += szTemp;
				sStatus[ulEOSSIndex].ucYellowCount++;
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47325 EOSS INFO: Flash memory warnings!");
			}
			if (sStatusBuffer.FlashErrors)
			{
				iColorFlags |= 0x02;
				sprintf(szTemp,"WARNING: %d Flash memory errors!\r\n",sStatusBuffer.FlashErrors);
				sStatus[ulEOSSIndex].cAll += szTemp;
				sStatus[ulEOSSIndex].cStatus += szTemp;
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47320 EOSS WARNING: Flash memory errors!");
			}
			if (sStatusBuffer.MajoWarnings)
			{
				iColorFlags |= 0x01;
				sprintf(szTemp,"INFO: %d Majority vote memory warnings!\r\n",sStatusBuffer.MajoWarnings);
				sStatus[ulEOSSIndex].cStatus += szTemp;
				sStatus[ulEOSSIndex].ucYellowCount++;
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47323 EOSS INFO: Majority vote memory warnings!");
			}
			if (sStatusBuffer.MajoErrors)
			{
				iColorFlags |= 0x02;
				sprintf(szTemp,"WARNING: %d Majority vote memory errors!\r\n",sStatusBuffer.MajoErrors);
				sStatus[ulEOSSIndex].cAll += szTemp;
				sStatus[ulEOSSIndex].cStatus += szTemp;
				sStatus[ulEOSSIndex].ucRedCount++;
				m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47321 EOSS WARNING: Majority vote memory errors!");
			}

			sprintf(szTemp,"\r\nS/N:\t\t%05u\r\n",sStatusBuffer.Seal);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			SYSTEMTIME sSystemTime;
			EossToSystemTime(&sSystemTime,&sStatusBuffer.Time);
			sprintf(szTemp,"EOSS Time:\t%04d.%02d.%02d %02d:%02d:%02d\r\nLOCAL Time:\t",
				sSystemTime.wYear,sSystemTime.wMonth,sSystemTime.wDay,
				sSystemTime.wHour,sSystemTime.wMinute,sSystemTime.wSecond);
			sStatus[ulEOSSIndex].cStatus += szTemp;

#pragma pack(1)
			struct tagTIMEREC
			{
				double dt;
				COleDateTime::DateTimeStatus status;
				BYTE   bOffset;
			} sTimeRec;
#pragma pack()

			ulBuffSize = sizeof(sTimeRec);
			lRet = RegQueryValueExA(hKey,"LOCAL TIME",NULL,
				&dwType,(BYTE*)&sTimeRec,&ulBuffSize);
			if (lRet == ERROR_SUCCESS)
			{
				COleDateTime cLocalTime;
				cLocalTime.m_dt = sTimeRec.dt;
				cLocalTime.m_status = sTimeRec.status;
				if (cLocalTime.GetStatus() == COleDateTime::valid)
				{
					sStatus[ulEOSSIndex].cStatus += cLocalTime.Format("%Y.%m.%d %H:%M:%S");
					COleDateTime cEOSSTime(sSystemTime);
					int iOffsetHours = GetPrivateProfileInt(m_pName,"TIME_OFFSET",0,m_szIniFile);
					COleDateTimeSpan cSpan(0,iOffsetHours,0,0);
					cEOSSTime = cEOSSTime + cSpan;
					int iErrorSeconds = GetPrivateProfileInt(m_pName,"TIME_ERROR",60,m_szIniFile);
					cSpan = COleDateTimeSpan(0,0,0,iErrorSeconds);
					COleDateTimeSpan cDiff;
					cDiff = cEOSSTime - cLocalTime;
					bool bOutOfTol = false;
					if (fabs(cDiff) > fabs(cSpan))
					{
						//process out of tolerance time error
						bOutOfTol = true;
						char szTemp[128];
						sStatus[ulEOSSIndex].ucYellowCount++;
						iColorFlags |= 0x01;

						//	put up clock
						m_pButton->ShowIcon_Clock(VARIANT_TRUE);
						//	put entry in pfm
						sprintf(szTemp,"47301 EOSS WARNING on %d, Time out of tolerance (%.0f sec)!",EOSSes[ulEOSSIndex],fabs(cDiff.GetTotalSeconds()));
						m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,szTemp);
					}

					sprintf(szTemp," (%d sec error) %s",(int)cDiff.GetTotalSeconds(),bOutOfTol? "Out of Tol.":"");
					sStatus[ulEOSSIndex].cStatus += szTemp;

				}
				else
					sStatus[ulEOSSIndex].cStatus += "Invalid";
			}
			else
				sStatus[ulEOSSIndex].cStatus += "Not Available";
			sStatus[ulEOSSIndex].cStatus += "\r\n";

			UnMakeID(sStatusBuffer.Operator1, szTemp);
			sStatus[ulEOSSIndex].cStatus += "Oper. 1\t\t";
			sStatus[ulEOSSIndex].cStatus += szTemp;
			//UnMakeID(sStatusBuffer.Operator2, szTemp);
			//sStatus[ulEOSSIndex].cStatus += "\r\nOper. 2\t\t";
			//sStatus[ulEOSSIndex].cStatus += szTemp;
			sStatus[ulEOSSIndex].cStatus += "\r\n";

			sprintf(szTemp,"External power\t%spresent\r\n",(POWER_BIT(sStatusBuffer))?"not ":"");
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp,"Seal wire\t\t%s\r\n",(sStatusBuffer.Flags & FLAG_FOOPEN)?"open":"closed");
			sStatus[ulEOSSIndex].cStatus += szTemp;

			if (sStatusBuffer.Flags & FLAG_FOFAIL)
			{
				sprintf(szTemp,"Seal wire\t\t%sfailed\r\n",(sStatusBuffer.Flags & FLAG_FOFAIL)?"":"not ");
				sStatus[ulEOSSIndex].cStatus += szTemp;
			}

			sprintf(szTemp,"Drill protection\t%s\r\n",(sStatusBuffer.Flags & FLAG_SHORT)?"short":(sStatusBuffer.Flags & FLAG_BROKEN)?"broken":"safe");
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sStatus[ulEOSSIndex].cStatus += "\r\nMain battery\r\n";

			sprintf(szTemp," - Compartment\t%s\r\n",(sStatusBuffer.Flags & FLAG_HOPEN)?"open":"closed");
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp," - Voltage\t\t%3.1f V\r\n",((float)sStatusBuffer.MainBattery)/10.0);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp," - Power consumed\t%d of 2000 mAh\r\n",
				sStatusBuffer.BatteryConsumed/1000);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			strncpy(szTemp,sStatusBuffer.BatteryType,8);
			szTemp[8] = NULL;
			sStatus[ulEOSSIndex].cStatus += " - Type\t\t";
			sStatus[ulEOSSIndex].cStatus += szTemp;
			sStatus[ulEOSSIndex].cStatus += "\r\n";

			sprintf(szTemp,"%02x%02x %02x%02x %02x%02x %02x%02x\r\n",
				sStatusBuffer.BatID[0],	sStatusBuffer.BatID[1],
				sStatusBuffer.BatID[2],	sStatusBuffer.BatID[3],
				sStatusBuffer.BatID[4],	sStatusBuffer.BatID[5],
				sStatusBuffer.BatID[6],	sStatusBuffer.BatID[7]);
			sStatus[ulEOSSIndex].cStatus += " - ID\t\t";
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp,"%04d",sStatusBuffer.BatteryMfg);
			sStatus[ulEOSSIndex].cStatus += " - Manufactured\t20";
			sStatus[ulEOSSIndex].cStatus += szTemp[2];
			sStatus[ulEOSSIndex].cStatus += szTemp[3];
			sStatus[ulEOSSIndex].cStatus += "-";
			sStatus[ulEOSSIndex].cStatus += szTemp[0];
			sStatus[ulEOSSIndex].cStatus += szTemp[1];
			sStatus[ulEOSSIndex].cStatus += "\r\n";

			EOSSTIME sEOSStime;
			sEOSStime.HalfDays = sStatusBuffer.BatteryInstalled;
			sEOSStime.Seconds = 0;
			SYSTEMTIME ssSystemTime;
			EossToSystemTime(&ssSystemTime, &sEOSStime);

			sprintf(szTemp," - Installed\t%d-%02d-%02d\r\n",
				ssSystemTime.wYear,ssSystemTime.wMonth,ssSystemTime.wDay);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sStatus[ulEOSSIndex].cStatus += "\r\nEmergency battery\r\n";

			sprintf(szTemp," - Voltage\t\t%3.1f V\r\n",((float)sStatusBuffer.EmergencyBattery)/10.0);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp," - Power consumed\t%d of 170 mAh\r\n",
				sStatusBuffer.EBatteryConsumed/1000);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp,"%04d",sStatusBuffer.EBatteryMfg);
			sStatus[ulEOSSIndex].cStatus += " - Manufactured\t20";
			sStatus[ulEOSSIndex].cStatus += szTemp[2];
			sStatus[ulEOSSIndex].cStatus += szTemp[3];
			sStatus[ulEOSSIndex].cStatus += "-";
			sStatus[ulEOSSIndex].cStatus += szTemp[0];
			sStatus[ulEOSSIndex].cStatus += szTemp[1];
			sStatus[ulEOSSIndex].cStatus += "\r\n";

			sEOSStime.HalfDays = sStatusBuffer.EBatteryInstalled;
			sEOSStime.Seconds = 0;
			EossToSystemTime(&ssSystemTime, &sEOSStime);

			sprintf(szTemp," - Installed\t%d-%02d-%02d\r\n",
				ssSystemTime.wYear,ssSystemTime.wMonth,ssSystemTime.wDay);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp,"\r\nTemperature\t%d °C\r\n",sStatusBuffer.Temperature);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sStatus[ulEOSSIndex].cStatus += "\r\nVACOSS emulation\r\n";
			sprintf(szTemp," - Enabled\t%s\r\n",sStatusBuffer.VacossEnabled?"yes":"no");
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp," - Attachments\t%lu\r\n",sStatusBuffer.VacossAttachments);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sStatus[ulEOSSIndex].cStatus += "\r\nFlash memory\r\n";

			sprintf(szTemp," - Warnings\t%d\r\n",sStatusBuffer.FlashWarnings);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp," - Errors\t\t%d\r\n",sStatusBuffer.FlashErrors);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sStatus[ulEOSSIndex].cStatus += "\r\nMajority-vote memory\r\n";

			sprintf(szTemp," - Warnings\t%d\r\n",sStatusBuffer.MajoWarnings);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp," - Errors\t\t%d\r\n",sStatusBuffer.MajoErrors);
			sStatus[ulEOSSIndex].cStatus += szTemp;

			sprintf(szTemp,"\r\nEPROM CRC\t0x%04X\r\n",sStatusBuffer.EPROMCrc16);
			sStatus[ulEOSSIndex].cStatus += szTemp;

		}
		else
		{
			iColorFlags |= 0x02;
			sStatus[ulEOSSIndex].cAll += "WARNING: Cannot read STATUS from registry!\r\n";
			sStatus[ulEOSSIndex].cStatus += "WARNING: Cannot read STATUS from registry!\r\n";
			sStatus[ulEOSSIndex].ucRedCount++;
			m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47306 EOSS WARNING: Cannot read STATUS from registry!");
		}

		if (sStatus[ulEOSSIndex].cAll.IsEmpty())
			sStatus[ulEOSSIndex].cAll = "Ok";

		switch (iColorFlags){
		case 0: //green
			sStatus[ulEOSSIndex].ucStatus = 1;
			break;
		case 1://yellow
			sStatus[ulEOSSIndex].ucStatus = 3; // yellow triangle, was originally 11, a duplicate of yellow
			break;
		case 2://red
			sStatus[ulEOSSIndex].ucStatus = 0;
			break;
		case 3://red and yellow
			sStatus[ulEOSSIndex].ucStatus = 10;
			break;
		default:
			sStatus[ulEOSSIndex].ucStatus = 1;
		}

		if (iColorFlags == 0 && sStatus[ulEOSSIndex].ucBlueCount) // jfl, show blue circle status *only* if green and seal open, do not show as red or yellow
			sStatus[ulEOSSIndex].ucStatus = 2;  // blue circle

		//log info
		ulBuffSize = sizeof(LOGINFO);
		lRet = RegQueryValueExA(hKey,"LOGINFO",NULL,
			&dwType,(BYTE*)&sLogInfoBuffer,&ulBuffSize);
		sStatus[ulEOSSIndex].cLogInfo = "LOG INFO...\r\n\r\n";
		if (lRet == ERROR_SUCCESS)
		{
			bool bSealEventsOverFlow = false;
			bool bSystemEventsOverFlow = false;
			bool bSoHRecordsOverFlow = false;

			uiSealEvents = sLogInfoBuffer.SealEvents;
			if (uiSealEvents > (sLogInfoBuffer.SealLogLastPage - sLogInfoBuffer.SealLogFirstPage + 1)*SEALEVENTS_PER_PAGE)
			{
				uiSealEvents = ((sLogInfoBuffer.SealLogLastPage - sLogInfoBuffer.SealLogFirstPage)*SEALEVENTS_PER_PAGE) + (sLogInfoBuffer.SealEvents % SEALEVENTS_PER_PAGE);
				if ((sLogInfoBuffer.SealEvents % SEALEVENTS_PER_PAGE) == 0)
					uiSealEvents += SEALEVENTS_PER_PAGE;
				bSealEventsOverFlow = true;
			}

			uiSystemEvents = sLogInfoBuffer.SystemEvents;
			if (uiSystemEvents > (sLogInfoBuffer.SystemLogLastPage - sLogInfoBuffer.SystemLogFirstPage + 1)*SYSTEMEVENTS_PER_PAGE)
			{
				uiSystemEvents = ((sLogInfoBuffer.SystemLogLastPage - sLogInfoBuffer.SystemLogFirstPage)*SYSTEMEVENTS_PER_PAGE) + (sLogInfoBuffer.SystemEvents % SYSTEMEVENTS_PER_PAGE);
				if ((sLogInfoBuffer.SystemEvents % SYSTEMEVENTS_PER_PAGE) == 0)
					uiSystemEvents += SYSTEMEVENTS_PER_PAGE;
				bSystemEventsOverFlow = true;
			}

			uiSoHRecords = sLogInfoBuffer.SoHRecords+1;
			if (uiSoHRecords > (sLogInfoBuffer.SoHLogLastPage - sLogInfoBuffer.SoHLogFirstPage + 1)*SOHRECORDS_PER_PAGE)
			{
				uiSoHRecords = ((sLogInfoBuffer.SoHLogLastPage - sLogInfoBuffer.SoHLogFirstPage)*SOHRECORDS_PER_PAGE) + (sLogInfoBuffer.SoHRecords % SOHRECORDS_PER_PAGE);
				if ((sLogInfoBuffer.SoHRecords % SOHRECORDS_PER_PAGE) == 0)
					uiSoHRecords += SOHRECORDS_PER_PAGE;
				bSoHRecordsOverFlow = true;
			}

			//build loginfo string
			sprintf(szTemp,"Total PAD Pages\t%d\r\n",sLogInfoBuffer.TotalPadPages);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;
			sprintf(szTemp,"Ext. Status Flags\t0x%02x\r\n",sLogInfoBuffer.ExStatusFlags);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			unsigned int uiKeyNumber = sLogInfoBuffer.KeysetNumber;
			uiKeyNumber &= 0x00ffffff;
			sprintf(szTemp,"Key Number\t%u-%u\r\n",uiKeyNumber>>8,sLogInfoBuffer.KeysetNumber & 0xff000000>>24);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			SYSTEMTIME sysTime;
			EossToSystemTime(&sysTime, &sLogInfoBuffer.KeysetTime);
			sprintf(szTemp,"Key Time\t\t%04u\\%02u\\%02u %02u:%02u:%02u\r\n",
				sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;			


			sStatus[ulEOSSIndex].cLogInfo += "\r\nSeal Events Log\r\n";

			sprintf(szTemp," - Events\t\t%d",sLogInfoBuffer.SealEvents);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			if (bSealEventsOverFlow)
			{
				sprintf(szTemp," INCOMPLETE, %u available",uiSealEvents);
				sStatus[ulEOSSIndex].cLogInfo += " INCOMPLETE";
			}
			sStatus[ulEOSSIndex].cLogInfo += "\r\n";

			sprintf(szTemp," - First Page\t%d\r\n",sLogInfoBuffer.SealLogFirstPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sprintf(szTemp," - Last Page\t%d\r\n",sLogInfoBuffer.SealLogLastPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sprintf(szTemp," - Start Page\t%d\r\n",sLogInfoBuffer.SealLogStartPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sprintf(szTemp," - Current Page\t%d\r\n",sLogInfoBuffer.SealLogCurrentPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sStatus[ulEOSSIndex].cLogInfo += "\r\nSystem Events Log\r\n";

			sprintf(szTemp," - Events\t\t%d",sLogInfoBuffer.SystemEvents);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;
			if (bSystemEventsOverFlow)
			{
				sprintf(szTemp," INCOMPLETE, %u available",uiSystemEvents);
				sStatus[ulEOSSIndex].cLogInfo += szTemp;
			}
			sStatus[ulEOSSIndex].cLogInfo += "\r\n";

			sprintf(szTemp," - First Page\t%d\r\n",sLogInfoBuffer.SystemLogFirstPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sprintf(szTemp," - Last Page\t%d\r\n",sLogInfoBuffer.SystemLogLastPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sprintf(szTemp," - Start Page\t%d\r\n",sLogInfoBuffer.SystemLogStartPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sprintf(szTemp," - Current Page\t%d\r\n",sLogInfoBuffer.SystemLogCurrentPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sStatus[ulEOSSIndex].cLogInfo += "\r\nSOH Log\r\n";

			sprintf(szTemp," - SoH Records\t%d",sLogInfoBuffer.SoHRecords);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;
			if (bSoHRecordsOverFlow)
			{
				sprintf(szTemp," INCOMPLETE, %u available",uiSoHRecords);
				sStatus[ulEOSSIndex].cLogInfo += szTemp;
			}
			sStatus[ulEOSSIndex].cLogInfo += "\r\n";

			sprintf(szTemp," - First Page\t%d\r\n",sLogInfoBuffer.SoHLogFirstPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sprintf(szTemp," - Last Page\t%d\r\n",sLogInfoBuffer.SoHLogLastPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sprintf(szTemp," - Start Page\t%d\r\n",sLogInfoBuffer.SoHLogStartPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sprintf(szTemp," - Current Page\t%d\r\n",sLogInfoBuffer.SoHLogCurrentPage);
			sStatus[ulEOSSIndex].cLogInfo += szTemp;

			sStatus[ulEOSSIndex].ucLogInfo = 1;

		}
		else
		{
			uiSealEvents = 0;
			uiSystemEvents = 0;
			uiSoHRecords = 0;
			sStatus[ulEOSSIndex].ucLogInfo = 0;
			sStatus[ulEOSSIndex].cAll += "WARNING: Cannot read LOGINFO from registry!\r\n";
			sStatus[ulEOSSIndex].cLogInfo += "WARNING: Cannot read LOGINFO from registry!\r\n";
			sStatus[ulEOSSIndex].ucRedCount++;
			m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47305 EOSS WARNING: Cannot read LOGINFO from registry!");
		}

		//jfl BMEND-13 todo . . .
		//	condition error status of missing SEAL, SYSTEM AND SOH Log pages on Query Status
		//	if status and log only then these status results (47311, 47314 and 47317) are not in error when the registry content is unavailable.

		QueryLevel ql = GetLatestDepth();

		//SEAL LOG
		unsigned int uiPageIndex = 1;

		bool bDone = false;
		CString cTemp;

		CString cStringHeader;

		sStatus[ulEOSSIndex].cSealLog.Empty();
		sStatus[ulEOSSIndex].ucSealLog = 1;

		bool bOk;
		while (!bDone)
		{
			sprintf(szPageName,"SEALLOG_%03u",uiPageIndex);
			ulBuffSize = sizeof(SEALLOGPAGE);
			lRet = RegQueryValueExA(hKey,szPageName,NULL,
				&dwType,(BYTE*)&sSealLogPage,&ulBuffSize);

			if (lRet == ERROR_SUCCESS)
			{
				bOk = true;
				if (uiPageIndex != 1)
				{
					if (memcmp(&sPageHeader,"FAILED READ",11)==0)
					{
						sStatus[ulEOSSIndex].ucSealLog = 0;
						cTemp.Format("ERROR: FAILED READ of seal log page %03d!\r\n",uiPageIndex);
						sStatus[ulEOSSIndex].cAll += cTemp;
						sStatus[ulEOSSIndex].cSealLog += cTemp;
						sStatus[ulEOSSIndex].ucRedCount++;
						m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47312 EOSS WARNING: FAILED seal log page read!");

						bOk = false;
						//bDone = true;
					}
					else if ((sPageHeader.Time.HalfDays != sSealLogPage.Entries[0].HalfDays) ||
						(sPageHeader.Time.Seconds != sSealLogPage.Entries[0].Seconds))
					{
						sStatus[ulEOSSIndex].ucSealLog = 0;
						cTemp.Format("ERROR: Seal log page %03d linking failed!\r\n",uiPageIndex);
						sStatus[ulEOSSIndex].cAll += cTemp;
						sStatus[ulEOSSIndex].cSealLog += cTemp;
						sStatus[ulEOSSIndex].ucRedCount++;
						m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47310 EOSS WARNING: FAILED seal log page linking!");

						//bOk = false;
						//bDone = true;
					}
				}

				if (bOk)
				{
					memcpy(&sPageHeader,&sSealLogPage.Header,sizeof(sSealLogPage.Header));

					sprintf(szTemp,"Page Header\t%s\r\n",szPageName);
					cStringHeader += szTemp;

					sprintf(szTemp," - Seal\t\t%u\r\n",sSealLogPage.Header.Seal);
					cStringHeader += szTemp;

					sprintf(szTemp," - Log\t\t%u\r\n",sSealLogPage.Header.Log);
					cStringHeader += szTemp;

					sprintf(szTemp," - Page\t\t%u\r\n",sSealLogPage.Header.Page);
					cStringHeader += szTemp;

					SYSTEMTIME sTime;
					EossToSystemTime(&sTime, &sSealLogPage.Header.Time);
					sprintf(szTemp," - Time\t\t%u\\%02u\\%02u %02u:%02u:%02u\r\n",
						sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
					cStringHeader += szTemp;

					sprintf(szTemp," - Authenticator\t0x%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
						sSealLogPage.Header.Authenticator[0],
						sSealLogPage.Header.Authenticator[1],
						sSealLogPage.Header.Authenticator[2],
						sSealLogPage.Header.Authenticator[3],
						sSealLogPage.Header.Authenticator[4],
						sSealLogPage.Header.Authenticator[5],
						sSealLogPage.Header.Authenticator[6],
						sSealLogPage.Header.Authenticator[7]);
					cStringHeader += szTemp;

					for (int i = 0; i < SEALEVENTS_PER_PAGE; i++)
					{
						if (uiSealEvents > 0)
						{
							//							if (iSealEvents != (signed)sLogInfoBuffer.SealEvents)
							//							{
							SYSTEMTIME sTime;
							EossToSystemTime(&sTime, &sSealLogPage.Entries[i]);
							sprintf(szTemp," - %03d %s\t%04u\\%02u\\%02u %02u:%02u:%02u\r\n",i+1,(i&1)?"Closed":"Opened",
								sTime.wYear,sTime.wMonth,sTime.wDay,
								sTime.wHour,sTime.wMinute,sTime.wSecond);
							sStatus[ulEOSSIndex].cSealLog = szTemp + sStatus[ulEOSSIndex].cSealLog;
							//							}
							uiSealEvents--;
						}
					}
				}

				uiPageIndex++;
				if (sSealLogPage.Header.Page == sLogInfoBuffer.SealLogCurrentPage)
					bDone = true;
			}
			else
			{
				if (ql <= eStatusAndLog)
				{
					cTemp.Format("INFO: Seal log query skipped, query depth is %s\r\n",levelnames[ql]);
					sStatus[ulEOSSIndex].cAll += cTemp;
					sStatus[ulEOSSIndex].cSealLog += cTemp;
				}
				else
				{
					sStatus[ulEOSSIndex].ucSealLog = 0;
					cTemp.Format("ERROR: Seal log page %03d not available in registry!\r\n",uiPageIndex);
					sStatus[ulEOSSIndex].cAll += cTemp;
					sStatus[ulEOSSIndex].cSealLog += cTemp;
					sStatus[ulEOSSIndex].ucRedCount++;
					m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47311 EOSS WARNING: FAILED seal log page not available in registry!");
				}

				bDone = true;
			}
		}

		sStatus[ulEOSSIndex].cSealLog = "SEAL LOG...\r\n\r\nEvents\r\n" + sStatus[ulEOSSIndex].cSealLog;
		sStatus[ulEOSSIndex].cSealLog += "\r\n";


		//SYSTEM LOG
		uiPageIndex = 1;
		bDone = false;
		//		int iSystemEvents = sLogInfoBuffer.SystemEvents;
		int iEventCount = 0;

		cStringHeader.Empty();
		sStatus[ulEOSSIndex].cSystemLog.Empty();
		sStatus[ulEOSSIndex].ucSystemLog = 1;
		bOk = true;

		while (!bDone)
		{
			sprintf(szPageName,"SYSTEMLOG_%03u",uiPageIndex);
			ulBuffSize = sizeof(SYSTEMLOGPAGE);
			lRet = RegQueryValueExA(hKey,szPageName,NULL,
				&dwType,(BYTE*)&sSystemLogPage,&ulBuffSize);
			if (lRet == ERROR_SUCCESS)
			{
				if (uiPageIndex != 1)
				{
					if (memcmp(&sPageHeader,"FAILED READ",11)==0)
					{
						sStatus[ulEOSSIndex].ucSystemLog = 0;
						cTemp.Format("ERROR: FAILED READ of system log page %03d!\r\n",uiPageIndex);
						sStatus[ulEOSSIndex].cAll += cTemp;
						sStatus[ulEOSSIndex].cSystemLog += cTemp;
						sStatus[ulEOSSIndex].ucRedCount++;
						m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47318 EOSS WARNING: FAILED system log page read!");
						bOk = false;
						//bDone = true;
					}
					else if ((sPageHeader.Time.HalfDays != sSystemLogPage.Entries[0].Time.HalfDays) ||
						(sPageHeader.Time.Seconds != sSystemLogPage.Entries[0].Time.Seconds))
					{
						sStatus[ulEOSSIndex].ucSystemLog = 0;
						cTemp.Format("ERROR: System log page %03d linking failed!\r\n",uiPageIndex);
						sStatus[ulEOSSIndex].cAll += cTemp;
						sStatus[ulEOSSIndex].cSystemLog += cTemp;
						sStatus[ulEOSSIndex].ucRedCount++;
						m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47316 EOSS WARNING: FAILED system log page linking!");
						//bDone = true;
					}
				}
				if (bOk)
				{
					memcpy(&sPageHeader,&sSystemLogPage.Header,sizeof(sSystemLogPage.Header));

					sprintf(szTemp,"Registry Entry\t%s\r\n",szPageName);
					cStringHeader += szTemp;

					sprintf(szTemp," - Seal\t\t%u\r\n",sSystemLogPage.Header.Seal);
					cStringHeader += szTemp;

					sprintf(szTemp," - Log\t\t%u\r\n",sSystemLogPage.Header.Log);
					cStringHeader += szTemp;

					sprintf(szTemp," - Page\t\t%u\r\n",sSystemLogPage.Header.Page);
					cStringHeader += szTemp;

					SYSTEMTIME sTime;
					EossToSystemTime(&sTime, &sSystemLogPage.Header.Time);
					sprintf(szTemp," - Time\t\t%u\\%02u\\%02u %02u:%02u:%02u\r\n",
						sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
					cStringHeader += szTemp;

					sprintf(szTemp," - Authenticator\t0x%02X%02X%02X%02X%02X%02X%02X%02X\r\n\r\n",
						sSystemLogPage.Header.Authenticator[0],
						sSystemLogPage.Header.Authenticator[1],
						sSystemLogPage.Header.Authenticator[2],
						sSystemLogPage.Header.Authenticator[3],
						sSystemLogPage.Header.Authenticator[4],
						sSystemLogPage.Header.Authenticator[5],
						sSystemLogPage.Header.Authenticator[6],
						sSystemLogPage.Header.Authenticator[7]);
					cStringHeader += szTemp;

					for (int i = 0; i < SYSTEMEVENTS_PER_PAGE; i++)
					{
						if (uiSystemEvents > 0)
						{
							SYSTEMTIME sTime;
							EossToSystemTime(&sTime, &sSystemLogPage.Entries[i].Time);
							sprintf(szTemp," - %03d  %04u\\%02u\\%02u %02u:%02u:%02u  %s\r\n",
								++iEventCount,
								sTime.wYear,sTime.wMonth,sTime.wDay,
								sTime.wHour,sTime.wMinute,sTime.wSecond,
								GetEossMessage(sSystemLogPage.Entries[i].Code));
							sStatus[ulEOSSIndex].cSystemLog = szTemp + sStatus[ulEOSSIndex].cSystemLog;
							uiSystemEvents--;
						}
					}
				}
				uiPageIndex++;
				if (sSystemLogPage.Header.Page == sLogInfoBuffer.SystemLogCurrentPage)
					bDone = true;
			}
			else
			{
				if (ql <= eStatusAndLog)
				{
					cTemp.Format("INFO: System log query skipped, query depth is %s\r\n",levelnames[ql]);
					sStatus[ulEOSSIndex].cAll += cTemp;
					sStatus[ulEOSSIndex].cSystemLog += cTemp;
				}
				else
				{
					sStatus[ulEOSSIndex].ucSystemLog = 0;
					cTemp.Format("ERROR: System log page %03d not available in registry!\r\n",uiPageIndex);
					sStatus[ulEOSSIndex].cSystemLog += cTemp;
					sStatus[ulEOSSIndex].ucRedCount++;
					m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47317 EOSS WARNING: FAILED system log page not available in registry!");
				}
				bDone = true;
			}
		}
		sStatus[ulEOSSIndex].cSystemLog = "SYSTEM LOG...\r\n\r\nEvents\r\n" + sStatus[ulEOSSIndex].cSystemLog;
		sStatus[ulEOSSIndex].cSystemLog += "\r\n";

		//sStatus[ulEOSSIndex].cSystemLog += cStringHeader;
		if (sStatus[ulEOSSIndex].ucSystemLog == 6) 
			sStatus[ulEOSSIndex].ucSystemLog = 1;

		//SOH LOG
		uiPageIndex = 1;
		bDone = false;
		unsigned int uiCount = 0;
		//uiCount = sLogInfoBuffer.SoHRecords;
		uiCount = uiSoHRecords;

		cStringHeader.Empty();
		sStatus[ulEOSSIndex].cSOHLog.Empty();
		int iSoHCount = 0;
		sStatus[ulEOSSIndex].ucSOHLog = 1;
		bOk = true;

		while (!bDone)
		{
			sprintf(szPageName,"SOHLOG_%03u",uiPageIndex);
			ulBuffSize = sizeof(SOHLOGPAGE);
			lRet = RegQueryValueExA(hKey,szPageName,NULL,
				&dwType,(BYTE*)&sSoHLogPage,&ulBuffSize);
			if (lRet == ERROR_SUCCESS)
			{
				if (uiPageIndex != 1)
				{
					if (memcmp(&sPageHeader,"FAILED READ",11)==0)
					{
						sStatus[ulEOSSIndex].ucSOHLog = 0;
						cTemp.Format("ERROR: FAILED READ of SoH log page %03d!\r\n",uiPageIndex);
						sStatus[ulEOSSIndex].cAll += cTemp;
						sStatus[ulEOSSIndex].cSOHLog += cTemp;
						sStatus[ulEOSSIndex].ucRedCount++;
						bOk = false;
						m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47315 EOSS WARNING: FAILED SoH log page read!");
					}
					/*
					else //correct for another firmware problem in EOSS
					{

					if ((sSoHLogPage.Header.Time.HalfDays != sSoHLogPage.Entries[min(SOHRECORDS_PER_PAGE-1,uiCount)].Date) &&
					(sSoHLogPage.Header.Time.HalfDays != sSoHLogPage.Entries[min(SOHRECORDS_PER_PAGE-1,uiCount)].Date+1) &&
					(sSoHLogPage.Header.Time.HalfDays != sSoHLogPage.Entries[min(SOHRECORDS_PER_PAGE-1,uiCount-1)].Date) &&
					(sSoHLogPage.Header.Time.HalfDays != sSoHLogPage.Entries[min(SOHRECORDS_PER_PAGE-1,uiCount-1)].Date+1))
					{
					sStatus[ulEOSSIndex].ucSOHLog = 0;
					cTemp.Format("ERROR: SoH log page %03d linking failed!\r\n",uiPageIndex);
					sStatus[ulEOSSIndex].cAll += cTemp;
					sStatus[ulEOSSIndex].cSOHLog += cTemp;
					sStatus[ulEOSSIndex].ucRedCount++;
					m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47313 EOSS WARNING: FAILED SoH log page linking!");
					}
					}
					*/
				}
				if (bOk)
				{
					memcpy(&sPageHeader,&sSoHLogPage.Header,sizeof(sSoHLogPage.Header));

					sprintf(szTemp,"\r\nPage Header\t%s\r\n",szPageName);
					cStringHeader += szTemp;
					sStatus[ulEOSSIndex].cSOHLog += szTemp;

					sprintf(szTemp," - Seal\t\t%u\r\n",sSoHLogPage.Header.Seal);
					cStringHeader += szTemp;
					sStatus[ulEOSSIndex].cSOHLog += szTemp;

					sprintf(szTemp," - Log\t\t%u\r\n",sSoHLogPage.Header.Log);
					cStringHeader += szTemp;
					sStatus[ulEOSSIndex].cSOHLog += szTemp;

					sprintf(szTemp," - Page\t\t%u\r\n",sSoHLogPage.Header.Page);
					cStringHeader += szTemp;
					sStatus[ulEOSSIndex].cSOHLog += szTemp;

					SYSTEMTIME sTime;
					EossToSystemTime(&sTime, &sSoHLogPage.Header.Time);
					sprintf(szTemp," - Time\t\t%u\\%02u\\%02u %02u:%02u:%02u\r\n",
						sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
					cStringHeader += szTemp;
					sStatus[ulEOSSIndex].cSOHLog += szTemp;

					sprintf(szTemp," - Authenticator\t0x%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
						sSoHLogPage.Header.Authenticator[0],
						sSoHLogPage.Header.Authenticator[1],
						sSoHLogPage.Header.Authenticator[2],
						sSoHLogPage.Header.Authenticator[3],
						sSoHLogPage.Header.Authenticator[4],
						sSoHLogPage.Header.Authenticator[5],
						sSoHLogPage.Header.Authenticator[6],
						sSoHLogPage.Header.Authenticator[7]);
					cStringHeader += szTemp;
					sStatus[ulEOSSIndex].cSOHLog += szTemp;


					for (int i = 0; i < SOHRECORDS_PER_PAGE; i++)
					{
						if (uiSoHRecords > 0)
						{
							SYSTEMTIME sTime;
							EOSSTIME eTime;
							eTime.HalfDays = sSoHLogPage.Entries[i].Date;
							eTime.Seconds = 0;
							EossToSystemTime(&sTime, &eTime);
							sprintf(szTemp," - %02d\t%04u\\%02u\\%02u   %3d° %3d°   %3.1fv %3.1fv   %4u %3u\r\n",++iSoHCount,
								sTime.wYear,sTime.wMonth,sTime.wDay,
								sSoHLogPage.Entries[i].MaxTemperature,
								sSoHLogPage.Entries[i].MinTemperature,
								(float)sSoHLogPage.Entries[i].EmergencyBattery/10.0,
								(float)sSoHLogPage.Entries[i].MainBattery/10.0,
								sSoHLogPage.Entries[i].OnBatteries*12,
								sSoHLogPage.Entries[i].MVEvents);
							sStatus[ulEOSSIndex].cSOHLog += szTemp;
							uiSoHRecords--;
						}
					}
				}
				uiPageIndex++;
				if (sSoHLogPage.Header.Page == sLogInfoBuffer.SoHLogCurrentPage)
					bDone = true;
			}
			else
			{
				if (ql <= eStatusAndLog)
				{
					cTemp.Format("INFO: SoH log query skipped, query depth is %s\r\n",levelnames[ql]);
					sStatus[ulEOSSIndex].cAll += cTemp;
					sStatus[ulEOSSIndex].cSOHLog += cTemp;
				}
				else
				{
					sStatus[ulEOSSIndex].ucSOHLog = 0;
					cTemp.Format("ERROR: SoH log page %03d not available in registry!\r\n",uiPageIndex);
					sStatus[ulEOSSIndex].cAll += cTemp;
					sStatus[ulEOSSIndex].cSOHLog += cTemp;
					sStatus[ulEOSSIndex].ucRedCount++;
					m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"47314 EOSS WARNING: FAILED SoH log page not available in registry!");
				}
				bDone = true;
			}
			uiCount -= SOHRECORDS_PER_PAGE;
		}

		sStatus[ulEOSSIndex].cSOHLog = "SOH LOG...\r\n\r\nRecords\tDate, Max °C, Min °C, Emerg. "
			"Bat. V.,\r\n\tMain Bat. V., On Bat. Min., MV Events\r\n" + sStatus[ulEOSSIndex].cSOHLog;

		sStatus[ulEOSSIndex].cSOHLog += "\r\n";

		////////////////
		//// check for open while not logged in
		////////////////

		RegCloseKey(hKey);

		if ((m_iColorFlagS | m_iColorFlagG) & 0x02) 
			m_pButton->ShowIcon_Square(VARIANT_TRUE);
		else if ((m_iColorFlagS | m_iColorFlagG) & 0x01)
		{
			m_pButton->ShowIcon_Error(VARIANT_TRUE);
		}
	}
	else
	{
		sStatus[ulEOSSIndex].cAll = "ERROR: Cannot read EOSS section in registry!";
		sStatus[ulEOSSIndex].ucRedCount++;

		m_pButton->ShowIcon_Error(VARIANT_TRUE);
	}


	if (sStatus[ulEOSSIndex].ucRedCount > 0) iResult = 2;
	else if (sStatus[ulEOSSIndex].ucYellowCount > 0) iResult = 1;
	else iResult = 0;

	return iResult;
}

void CEOSSInstrument::BuildStatusStrings()
{
	char szFinal[MAX_PATH];
	strcpy(szFinal,REGISTRYLOCATION);
	strcat(szFinal,m_pName);
	LONG lRet;
	HKEY hKey;
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, szFinal, 0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		m_iColorFlagG |= GetString("LATEST READ",&sGeneral.ucLATEST_READ,&sGeneral.cLATEST_READ,hKey);
		m_iColorFlagG |= GetString("LATEST RESULTS",&sGeneral.ucLATEST_RESULTS,&sGeneral.cLATEST_RESULTS,hKey);
		m_iColorFlagG |= GetString("INSTRUMENT ID",&sGeneral.ucINSTRUMENT_ID,&sGeneral.cINSTRUMENT_ID,hKey);
		m_iColorFlagG |= GetString("TIME OFFSET",&sGeneral.bTimeOffset,&sGeneral.cTimeOffset,hKey);
		m_iColorFlagG |= GetString("PATH",&sGeneral.ucPATH,&sGeneral.cPath,hKey);
		m_iColorFlagG |= GetString("FORMAT",&sGeneral.ucFORMAT,&sGeneral.cFORMAT,hKey);
		m_iColorFlagG |= GetString("COMPORT",&sGeneral.ucCOMPORT,&sGeneral.cCOMPORT,hKey);
		m_iColorFlagG |= GetString("READER VERSION",&sGeneral.ucREADER_VERSION,&sGeneral.cREADER_VERSION,hKey);
		m_iColorFlagG |= GetString("SPECIFIED EOSSes",&sGeneral.ucSPECIFIED_EOSSes,&sGeneral.cSPECIFIED_EOSSes,hKey);
		//m_iColorFlagG |= GetString(QUERY_DEPTH,&sGeneral.ucQueryDepth,&sGeneral.cQUERY_DEPTH,hKey);
		RegCloseKey(hKey);
	}
	else
	{
		char szBuff[2048];
		sprintf(szBuff,"ERROR: Cannot access %s status in registry!\r\n",m_pName);
		sGeneral.cAll += szBuff; 
		sGeneral.ucLATEST_READ = sGeneral.ucLATEST_RESULTS = sGeneral.ucINSTRUMENT_ID = sGeneral.bTimeOffset = sGeneral.ucPATH = sGeneral.ucFORMAT =
			sGeneral.ucCOMPORT = sGeneral.ucREADER_VERSION = sGeneral.ucSPECIFIED_EOSSes = 2;//sGeneral.ucQueryDepth = 2;//red
		sGeneral.cLATEST_READ += szBuff;
		sGeneral.cLATEST_RESULTS += szBuff;
		sGeneral.cINSTRUMENT_ID += szBuff;
		sGeneral.cTimeOffset += szBuff;
		sGeneral.cPath += szBuff;
		sGeneral.cFORMAT += szBuff;
		sGeneral.cCOMPORT += szBuff;
		sGeneral.cREADER_VERSION += szBuff;
		sGeneral.cSPECIFIED_EOSSes += szBuff;
		sprintf(szBuff,"47206 EOSS ERROR: Cannot access %s status in registry!",m_pName);
		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,szBuff);
	}

}

int CEOSSInstrument::GetString(char *szItem, unsigned char *pucColorFlag, CString *pcString, HKEY hKey)
{

	*pucColorFlag = 0;
	*pcString = CString(szItem);
	*pcString += "...\r\n\r\n";

	if (strcmp(szItem,"SPECIFIED EOSSes")==0) // not in registry, the list exists in ini file
	{
		char szEOSSes[512];
		GetPrivateProfileString(m_pName,"EOSSes","",szEOSSes,sizeof(szEOSSes),m_szIniFile);
		char* szAt = strtok(szEOSSes,",");
		int iIndex = 0;
		while (szAt && iIndex < CEditEOSSList::maxEOSSCount)
		{
			*pcString += szAt;
			*pcString += "\r\n";
			szAt = strtok(NULL,",");
			iIndex++;
		}
		if (iIndex == 0) *pucColorFlag = 2;//red
	}
	else
	{
		char szBuff[2048];
		unsigned long ulBuffSize;
		LONG lRet;
		DWORD dwType;
		ulBuffSize = sizeof(szBuff);
		lRet = RegQueryValueExA(hKey,szItem,NULL,
			&dwType,(unsigned char *)szBuff,&ulBuffSize);
		if (lRet == ERROR_SUCCESS)
		{
			if (strcmp(szItem,"LATEST RESULTS")==0)
			{
				*pcString += szBuff;
				if (strstr(szBuff,"Error:"))
					*pucColorFlag = 2;//red
				else
					*pucColorFlag = 0;//green
			}
			else
			{
				*pcString += szBuff;
				*pucColorFlag = 0;//green
			}
		}
		else
		{
			sprintf(szBuff,"ERROR: Cannot read %s from registry!\r\n",szItem);
			sGeneral.cAll += szBuff;
			*pcString += szBuff;
			*pucColorFlag = 2;//red
			sprintf(szBuff,"47207 EOSS ERROR: Cannot read %s from registry!",szItem);
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,szBuff);

		}
	}

	return *pucColorFlag;
}

bool CEOSSInstrument::QueryLatestResultsForHardFail()
{
	bool bError = false;
	char szFinal[MAX_PATH];
	strcpy(szFinal,REGISTRYLOCATION);
	strcat(szFinal,m_pName);
	LONG lRet;
	HKEY hKey;
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, szFinal, 0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		char szBuff[512];
		unsigned long ulBuffSize;
		LONG lRet;
		DWORD dwType;
		ulBuffSize = sizeof(szBuff);
		lRet = RegQueryValueExA(hKey,"LATEST RESULTS",NULL,
			&dwType,(unsigned char *)szBuff,&ulBuffSize);
		if (lRet == ERROR_SUCCESS)
		{
			if (_strnicmp(szBuff,"Error:", 6)==0)  // The label "Error:" is prepended in reader when a comm error or a fundamental registry access error occurs
				bError = true;
			else
				bError = false;
		}
		RegCloseKey(hKey);
	}
	return bError;
}


void CEOSSInstrument::CleanRegistry()
{
	LONG lRet;
	HKEY hKey;

	char szName[MAX_PATH];
	char szEOSSName[32];
	DWORD dwEOSSNameSize;
	FILETIME ftLastWriteTime;

	//open the registry where the values are stored
	strcpy(szName,REGISTRYLOCATION);
	strcat(szName,m_pName);

	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szName,
		0,KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &hKey);

	if (lRet == ERROR_SUCCESS)
	{
		DWORD dwIndex = 0;
		bool bDone = false;
		while (!bDone)
		{
			dwEOSSNameSize = sizeof(szEOSSName);
			lRet = RegEnumKeyExA(hKey,dwIndex++,szEOSSName,&dwEOSSNameSize,
				NULL,NULL,NULL,&ftLastWriteTime);
			if (lRet == ERROR_SUCCESS)
			{
				bool bFound = false;
				for (int iIndex = 0; iIndex < MAXEOSSES; iIndex++)
				{
					if (EOSSes[iIndex] == (unsigned)atoi(szEOSSName))
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					char szBuff[1024];
					RegDeleteKey(hKey,szEOSSName);
					sprintf(szBuff,"47409 EOSS Removed %s from registry!",szEOSSName);
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,szBuff);
				}
			}
			else
				bDone = true;
		}
		RegCloseKey(hKey);
	}
}

DWORD CEOSSInstrument::GetEOSSIndex(char* id) const
{
	DWORD i = 0;
	UINT tid = atol(id);
	bool found = false;
	while (i < MAXEOSSES && EOSSes[i] > 0)
	{
		if (tid == EOSSes[i])
		{
			found = true;
			break;
		}
		i++;
	}
	if (found)
		return i;
	else
		return -1;
}

void CEOSSInstrument::PopulateArrays()
{
	m_iColorFlagS = 0; 
	m_iColorFlagG = 0; 

	LONG lRet;
	HKEY hKey;

	char szName[MAX_PATH];
	char szEOSSName[32];
	DWORD dwEOSSNameSize;
	FILETIME ftLastWriteTime;

	BuildStatusStrings();

	if (m_iColorFlagG == 3)
		sGeneral.ucAll = 10;//red and yellow
	else if (m_iColorFlagG == 2)
		sGeneral.ucAll = 0;//red
	else if (m_iColorFlagG == 1)
		sGeneral.ucAll = 3; // yellow, was 11 duplicate yellow
	else
		sGeneral.ucAll = 1;//green

	//memset(EOSSes,NULL,sizeof(EOSSes)); // jfl commented out by dp

	//open the registry where the values are stored
	strcpy(szName,REGISTRYLOCATION);
	strcat(szName,m_pName);

	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szName,
		0,KEY_QUERY_VALUE |KEY_ENUMERATE_SUB_KEYS, &hKey);

	if (lRet == ERROR_SUCCESS)
	{
		DWORD dwIndex = 0;
		ULONG index = 0;
		short count = 0;
		bool bDone = false;
		m_iColorFlagS = 0;
		while (!bDone && count < MAXEOSSES)
		{
			dwEOSSNameSize = sizeof(szEOSSName);
			lRet = RegEnumKeyExA(hKey,dwIndex,szEOSSName,&dwEOSSNameSize,
				NULL,NULL,NULL,&ftLastWriteTime);
			if (lRet == 0 || lRet == ERROR_MORE_DATA)
			{	
				//EOSSes[dwIndex] = atoi(szEOSSName);// jfl commented out by dp
				//CString cResults;
				index = GetEOSSIndex(szEOSSName);
				if (index >= 0 && index < MAXEOSSES)
				{
					m_iColorFlagS |= ValidateSeal(szName,szEOSSName,index);
				}

				dwIndex++;  // registry enum index only
				//if (dwIndex >= MAXEOSSES) bDone = true;
			}
			else
			{
				bDone = true;
			}
			count++;
		}
		RegCloseKey(hKey);
	}
}

bool CEOSSInstrument::CollectParametersForNewISO(CString& csNameOut)
{
	//	TRACE("CEOSSInstrument::CollectParametersForNewISO()\n");

	//This ISO is temporary, instantiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new ISO from that data.

	bool bReturn;

	//Collect the data for the new instrument.
	//
	CTPropertySheet tps("",m_iDlgCloseMilliseconds,this);
	//CEOSSModParam *pModParam = new CEOSSModParam(IDD_EOSSMODPARAMS_PAGE,this,true);
	CEOSSModParam modparam(IDD_EOSSMODPARAMS_PAGE,this,true);
	modparam.SetPropertySheet((CEOSSPropertySheet*)&tps,m_pName);
	tps.AddPage(&modparam);
	tps.SetWizardMode();

	if (tps.DoModal()==ID_WIZFINISH)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(INSTRUMENTS,m_pName,EOSS_DISCOVERY_TYPE,m_szIniFile);
		bReturn = true;
	}
	else
	{
		csNameOut.Empty();
		bReturn = false;
	}

	return bReturn;
}

void CEOSSInstrument::SetLocalRegisteredMessageValues()
{
	//	TRACE("CEOSSInstrument::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}


//disconnect this instrument from the communications object
//it is registered with
bool CEOSSInstrument::UnRegInstrument()
{
	//This is different from the base class version
	//of UnRegInstrument();

	//don't attempt to disconnect if it wasn't
	//registered in the first place

	//if we are unreg-ing and the ini section doesn't exist then we
	//are actually deleting this instrument support object, in which
	//case we need to make sure we also delete the scheduler in schtasks
	//and delete the registry entries.
	char szSch[32];
	strcpy(szSch,"NEVER");
	GetPrivateProfileString(m_pName,"AUTORUN","NEVER",szSch,sizeof(szSch),m_szIniFile);
	if (strcmp(szSch,"NEVER")==0)
	{
		//check to see if we are currently scheduled
		//by asking schtasks and piping the results
		//into a file to read later on
		char szFileName[MAX_PATH];
		sprintf(szFileName,"c:\\%s.bat",m_pName);
		m_file = fopen(szFileName,"wt");
		if (m_file != NULL)
		{
			fprintf(m_file,"schtasks /DELETE /F /TN \"%s\"\n",m_pName);
			fclose(m_file);
			ShellExecute(m_hWnd,"open",szFileName,"","c:\\",SW_HIDE);
		}
		else
		{	if (m_pPropertySheet)
		{
			errno_t err;
			_get_errno( &err );
			char szBuff[2048];
			sprintf(szBuff,"47103 EOSS Cannot create EOSS task deletion script at %.256s; (%.64s)",szFileName, strerror(err));
			m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,szBuff);
			m_pPropertySheet->UpdateSummary(szBuff);
		}
		}


		LONG lRet;
		HKEY hKey;

		//open the registry where the values are stored
		strcpy(szFileName,REGISTRYLOCATION);

		lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
			szFileName,
			0,KEY_ALL_ACCESS, &hKey);

		if (lRet == ERROR_SUCCESS)
		{
			RegDeleteKey(hKey,m_pName);
			RegCloseKey(hKey);
		}
	}

	if (m_pComm && m_bGoodRegister)
		m_pComm->UnRegisterISO(m_iNode & 0x0000ffff,m_eCategory);
	return true;
}

void CEOSSInstrument::PostToWatch(char *pMessage)
{
	if ((NULL != pMessage) && (NULL != m_pComm))
	{
		BSTR bstr_out;
		Convert_Character_String_to_BSTR(pMessage, bstr_out);
		m_pComm->SendToWatch(bstr_out, ULONG(m_iNode));
	}
}

void CEOSSInstrument::ShowInstrument(bool show)
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

void CEOSSInstrument::PauseInstrument(bool pause)
{
	//kill the timer so that it won't unpause us when we don't want
	KillTimer(TIMER_PAUSE);
	//	KillTimer(TIMER_TICK);

	//if we are changing the pause state then change the button face 
	//and the member variable
	m_pButton->put_FaceColor((pause)?COLOR_PAUSE:COLOR_OK);
	m_pPropertySheet->SetButtonColor((pause)?COLOR_PAUSE:COLOR_OK);
	if (m_bPause != pause)
	{
		char temp[256];
		if (pause)
			sprintf(temp, "47404 EOSS COLLECT pausing registry data examination for at most %d seconds", m_iPauseMax/1000);
		else
			sprintf(temp, "47405 EOSS COLLECT registry data examination starting");
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, temp);
	}
	m_bPause = pause;

	//if we are now paused then set the timer to un-pause us when
	//it expires at end of PauseMax
	if (pause) 
	{
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);	//dgp 18 Oct 2006
	}
	else
	{
		if (m_dCurrent_State == ePauseState) 
		{
			m_dTick_Count = 0;
			m_dCurrent_State = eStartSchtasksCheckWrite;
			m_dTickToInquire = m_dInquireInt;
		}
		SetTimer(TIMER_TICK,m_dCycleInt,NULL);
		m_bIsPaused = false;
	}
	// Have the MICDlg review the status of the buttons
	if (m_hButtonKeeperHandle)
		::PostMessage(m_hButtonKeeperHandle,m_uWM_CHECKDATABUTTON,0,0);
}
//-----------------------------------------------------------------------------
//
// get up to 3 captital letters from 16 bits
// (there's no error checking here...)
// 
void CEOSSInstrument::UnMakeID(unsigned short Id, char * szOperator)
{
	szOperator[0] = NULL;
	unsigned char Digit;
	CString szId;

	while (Id) {
		Digit = Id % 27;
		if (Digit)
			szId = (char)(Digit + 64) + szId;
		else
			szId = " " + szId;
		Id = Id / 27;
	}
	strcpy(szOperator, szId);
}

bool CEOSSInstrument::Unattended(unsigned short usCode)
{
	bool bLoggedOff = true;
	if ((usCode > 0) && (usCode < 19683))
		bLoggedOff = false;
	else if ((usCode > 20480) && (usCode < 40163))
		bLoggedOff = false;
	else if (usCode == 40960)
		bLoggedOff = true;
	return (bLoggedOff && (usCode != 40960) && (usCode != 41024));
}


void CEOSSInstrument::EossToSystemTime(SYSTEMTIME * pSystemTime, EOSSTIME * pEossTime)
{
	SYSTEMTIME SystemTime;

	SystemTime.wYear = 2000;
	SystemTime.wMonth = 1;
	SystemTime.wDay = 1;
	SystemTime.wHour = 0;
	SystemTime.wMinute = 0;
	SystemTime.wSecond = 0;
	SystemTime.wMilliseconds = 0;

	ULONGLONG FileTime;

	SystemTimeToFileTime(&SystemTime, (LPFILETIME)&FileTime);

	ULONGLONG EossTime = pEossTime->HalfDays * 43200 + (43200 - pEossTime->Seconds);

	FileTime += (EossTime * 10000000);

	FileTimeToSystemTime((LPFILETIME)&FileTime, pSystemTime);
}

CString CEOSSInstrument::GetEossMessage(unsigned short usCode)
{
	CString cString;
	char szTemp[64];
	if (usCode > 0 && usCode < 19683)
	{
		//		char szID[4];
		UnMakeID(usCode,szTemp);
		cString = szTemp;
		cString += " logged on as operator 1";
	}
	else if (usCode > 20480 && usCode < 40163)
	{
		//		char szID[4];
		UnMakeID(usCode - 20480,szTemp);
		cString = szTemp;
		cString += " logged on as operator 2";
	}
	else if (usCode == 40960) cString = "Operator(s) logged off.";
	else if (usCode == 40976) cString = "Date and time set.";
	else if (usCode == 40977) cString = "Date and time set.";
	else if (usCode == 40992) cString = "Battery compartment opened";
	else if (usCode == 40993) cString = "Battery compartment closed";
	else if (usCode == 41008) cString = "Seal tampered with. Key-set erased";
	else if (usCode == 41024) cString = "Access denied. Authorization failed";
	else if (usCode == 41040) cString = "New key-set generated";
	else if (usCode == 41056) cString = "Reset";
	else if (usCode == 41072) cString = "Key-set erased on operator request";
	else if (usCode == 41073) cString = "Temperature out of range. Key-set erased";
	else if (usCode == 41088) cString = "Entering deep sleep mode";
	else cString = "UNKNOWN code";
	if (Unattended(usCode)) cString += " WHILE NOT LOGGED IN";
	return cString;
}

//
//KillEOSS won't see the version that is running (if it is running) to support
//MIC_EOSSReader but will see any logged on users' version.  Even if it does 
//and kills MIC's version it will get restarted by MIC_EOSSReader as needed
//  JFL May 2009: this turns out to be flawed. Re-starting the EOSS Token COM server by
//  creating an instance and invoking a method (MIC__EOSSREader) requires appropriate
//  permissions in three areas and per login account, and we cannot guarantee the permissions
//  and accounts used satisfy the constraints, even if the accounts have admin priv.
//
void CEOSSInstrument::KillEOSS() 
{
	//TRACE("KillEOSS\r\n");
	//	return;
	char string[256];
	HWND handle;
	handle = ::FindWindowEx (NULL, NULL, NULL, NULL);
	while (handle)
	{
		::GetWindowTextA(handle, string,256);
		if (strlen(string))  
		{
			if (strstr(string,"EOSS Crypto Token"))
			{
				char szTemp[MAX_PATH];
				GetClassNameA(handle,szTemp,sizeof(szTemp));
				if ((strcmp(szTemp,"#32770")==0) && (handle != m_hWnd))
				{
					::PostMessage(handle,WM_QUIT,0,0);
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47400 EOSS Attempted to close \"EOSS Crypto Token\"");
					m_pPropertySheet->UpdateSummary("EOSS attempted to close the \"EOSS Crypto Token\" server.");
				}
			}
		}
		handle = ::FindWindowEx (NULL, handle, NULL, NULL);
	}

	CloseHandle(handle);
}

void CEOSSInstrument::ResetReaderCount() // set the interval full vs status counter used by the reader process back to 0
{
	char szName[MAX_PATH];
	strcpy(szName,REGISTRYLOCATION);
	strcat(szName, m_pName);

	HKEY hKey;
	UINT count = 0;  // counter used in reader process for % op on interval choice
	LSTATUS lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_WRITE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		lRet = RegSetValueExA(hKey,READ_COUNT, NULL,REG_BINARY,(BYTE*)&count,sizeof(count));
		RegCloseKey( hKey );
	}
}

void CEOSSInstrument::SetDepthForManualOveride()
{
	char szName[MAX_PATH];
	//update the registry
	strcpy(szName,REGISTRYLOCATION);
	strcat(szName, m_pName);

	HKEY hKey;
	UINT uval = 1;  // flag on here, unset in reader process
	//open the registry where the values are stored
	LSTATUS lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_WRITE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		lRet = RegSetValueExA(hKey,USE_OVERRIDE_QUERY_DEPTH, NULL,REG_BINARY,(BYTE*)&uval,sizeof(uval));
		uval = m_QueryDepth;
		lRet = RegSetValueExA(hKey,OVERRIDE_QUERY_DEPTH, NULL,REG_BINARY,(BYTE*)&uval,sizeof(uval));
		RegCloseKey( hKey );
	}
}

QueryLevel CEOSSInstrument::GetLatestDepth()
{
	QueryLevel q = eReadAllData;

	char szFinal[MAX_PATH];
	strcpy(szFinal,REGISTRYLOCATION);
	strcat(szFinal,m_pName);
	LONG lRet;
	HKEY hKey;
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, szFinal, 0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
		try
	{
		char szBuff[512];
		unsigned long ulBuffSize;
		DWORD dwType;
		ulBuffSize = sizeof(szBuff);
		lRet = RegQueryValueExA(hKey,QUERY_DEPTH,NULL, &dwType,(unsigned char *)szBuff,&ulBuffSize);
		if (lRet == ERROR_SUCCESS)
		{
			for (int i = 0; i < eQueryLevelMax; i++)
			{
				if (_stricmp(levelnames[i], szBuff) == 0)
				{
					q = QueryLevel(i);
					break;
				}
			}
		}
		RegCloseKey( hKey );
	}
	catch (...)
	{
	}
	return q;
}

#define XTRASECS (m_XPOrMilTime ? ":00" : "")
#define TEMPFILEWAIT 8

LRESULT CEOSSInstrument::OnReceive(WPARAM WParam, LPARAM LParam)
{

	//static bool once = true;

	//TRACE("m_dCurrent_State = %d\r\n",m_dCurrent_State);

	//make sure we have a CEV and a PFM for each day
	m_pNow = COleDateTime::GetCurrentTime();
	int dCurrentDay = m_pNow.GetDay();
	if (dCurrentDay != m_dPreviousDay)
	{
		m_dPreviousDay = dCurrentDay;
		//		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,"47402 EOSS COLLECT Start new day");
	}

	//when we need to pass some time the previous
	//step will zero this timer then the critical
	//step will watch the timer overreach an appropriate value
	if (WParam == TIMER_TICK)
		m_dTick_Count++;

	try
	{
		//the real state machine -- initial state should be eStartSchtasksCheckWrite
		switch (m_dCurrent_State) {
	case eStartSchtasksCheckWrite://write the scheduled tasks to a file

		// JFL: trouble brewing where access to root drive for file write and create requires proper permissions
		if (m_bPause) 
		{
			m_dCurrent_State = ePauseState;
			break;
		}

		m_pPropertySheet->UpdateSummary("Checking scheduling of MIC_EOSSReader.");
		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "47401 EOSS Checking scheduling of MIC_EOSSReader");

		//check to see if we are currently scheduled
		//by asking schtasks and piping the results
		//into a file to read later on
		sprintf(m_szFileName1,"c:\\%s.bat",m_pName);
		m_file = fopen(m_szFileName1,"wt");
		if (m_file == NULL)
		{
			if (m_pPropertySheet)
			{
				errno_t err;
				_get_errno( &err );
				char szBuff[2048];
				sprintf(szBuff,"47104 EOSS Cannot create EOSS task reader script at %.256s; (%.64s)", m_szFileName1, strerror(err));
				m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,szBuff);
				m_pPropertySheet->UpdateSummary(szBuff);
			}
		}
		else
		{
			sprintf(m_szFileName2,"\"c:\\%s.txt\"",m_pName);
			fprintf(m_file,"schtasks /Query /FO LIST /V >%s\n",m_szFileName2);
			fclose(m_file);
			ShellExecute(m_hWnd,"open",m_szFileName1,"","c:\\",SW_HIDE);
		}
		strcpy(m_szFileName1,&m_szFileName2[1]);
		m_szFileName1[strlen(m_szFileName1)-1] = NULL;

		m_dTick_Count = 0;
		m_dCurrent_State = eSchtasksCheckWaitThenRead;
		break;

	case eSchtasksCheckWaitThenRead://wait to be sure schtasks has finished
		//check for timeout, but fall through to allow good access to override
		if (m_dTick_Count >= TEMPFILEWAIT) 
		{
			m_dCurrent_State = eStartSchtasksCheckWrite;
			struct _stat buf;
			int result;
			result = _stat( m_szFileName1, &buf );
			// Check if statistics are valid: 
			if( result != 0 )  // file not there or other related failure, wait and try agionpon teh next tick up to TEMPFILEWAIT
			{
				//	switch (errno)
				//	{
				//		case ENOENT:
				//			sprintf(temp, "schtasks temp file %s not found, pend %d (%d)", m_szFileName1, m_dTick_Count, m_dCurrent_State);
				//			break;
				//		case EINVAL:
				//			sprintf(temp, "Invalid parameter to _stat");
				//			break;
				//		default:
				//			sprintf(temp, "Unexpected error in _stat.");
				//	}
				//	m_pPropertySheet->UpdateSummary(temp);
				//	sprintf(temp, "47552 EOSS %s", temp);
				//	m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, temp);
			}
			else
			{
				if (buf.st_size > 0)
				{
					//sprintf(temp, "%s has %u bytes, waited %d (%d)", m_szFileName1, buf.st_size, m_dTick_Count, m_dCurrent_State);
					//m_pPropertySheet->UpdateSummary(temp);
					m_dCurrent_State = eSchtasksCompare;
				}
				else  // empty file, waiting for OS shell flush ?
				{
					ASSERT(m_dTick_Count >= TEMPFILEWAIT);
					if (m_dTick_Count < (3*TEMPFILEWAIT)) // wait for two more intervals for the file to fill.
						m_dCurrent_State = eSchtasksCheckWaitThenRead;
					//else // infinite loop possibility if file is never filled, this conditional leaves the state in the default eStartSchtasksCheckWrite forcing a rewrite
					//sprintf(temp, "%s empty, waited %d (%d)", m_szFileName1, m_dTick_Count, m_dCurrent_State);
					//m_pPropertySheet->UpdateSummary(temp);
				}
			}

		}
		else		{		} // waiting for TEMPFILEWAIT ticks before checking for file existence and content from state eSchtasksCheckWaitThenRead
		break;

	case eSchtasksCompare://analyze what is scheduled and when 
		//look in schtasks output for my name
		// this code depends upon the label ordering of the schtasks output, and the differences from XP to W7: totally brittle and, in fact, lame
		{
			bool bFailedForSomeReason = false;  // set true on exception or failure to open file
			if ((m_file = fopen(m_szFileName1,"rt")) != 0)
			{
				try 
				{	//if open ok
					char szContents[4192*32];
					memset(szContents,NULL,sizeof(szContents));
					size_t nb = fread(szContents,1,sizeof(szContents)-1,m_file);
					fclose(m_file);
					char *szAt;
					if ((szAt = strstr(szContents,m_pName)) != NULL)
					{
						char szType[16];
						char szHours[4];
						char szMinutes[4];
						char szDays[16];
						char szNextTime[32];
						char szLog[32];
						szType[0] = szHours[0] = szMinutes[0] = szDays[0] = szNextTime[0] = szLog[0] = NULL;

						//get the next run time
						int iInPoint = 0;
						szNextTime[0] = NULL;
						szAt = strstr(szAt,"Next Run Time:");
						szAt += strlen("Next Run Time:");
						while (*szAt == ' ') szAt++;
						while ((*szAt != '\r') && (*szAt != '\n'))szNextTime[iInPoint++] = *szAt++;
						szNextTime[iInPoint] = NULL;

						int iHour,iMin,iSec,iMon,iDay,iYear;
						iHour=iMin=iSec=iMon=iDay=iYear = 0;
						int args = sscanf(szNextTime,"%d:%d:%d, %d/%d/%d",&iHour,&iMin,&iSec,&iMon,&iDay,&iYear);
//						m_cNextScheduled = COleDateTime(iYear,iMon,iDay,iHour,iMin,iSec);
						m_cNextScheduled.SetDateTime(iYear,iMon,iDay,iHour,iMin,iSec);
						char mon[8]; mon[0] = NULL;
						if (m_cNextScheduled.GetStatus() != COleDateTime::valid) // must be W7 or someting wronhg here
						{
							m_XPOrMilTime = false;
							mon[0] = 'A'; mon[1] = 'M'; mon[2] = NULL;
							iHour = 0;
							sscanf(szNextTime,"%d/%d/%d %d:%d:%d %2s",&iMon,&iDay,&iYear,&iHour,&iMin,&iSec,mon); //iHour not updated in this block sometimes, but minutes are why why why?
							if (strnicmp(mon, "PM", 2) == 0)
							{
								iHour += 12;
							}
							m_cNextScheduled = COleDateTime(iYear,iMon,iDay,iHour,iMin,iSec);

						}

						m_pDataStatus->SetDlgItemText(IDC_EOSSDATA_MSG,m_cNextScheduled.Format("%Y.%m.%d %H:%M:%S"));

						// get the log flag from the reader command line
						char szReadLogINI[32];
						szLog[0] = szLog[1] = NULL;
						szAt = strstr(szAt,"Task To Run:");
						szAt += strlen("Task To Run:");
						char *tcp = strstr(szAt, " -log");
						if (tcp)
							szLog[0] = '1';
						else
							szLog[0]= '0';

						GetPrivateProfileString(m_pName,READ_LOG,"0",szReadLogINI,sizeof(szReadLogINI),m_szIniFile);

						//get the type
						iInPoint = 0;
						szType[0] = NULL;
						char* restartAt = szAt;
						szAt = strstr(szAt,"Scheduled Type:");
						if (szAt == NULL) // must be W7 or someting wronhg here
						{
							m_XPOrMilTime = false;
							szAt = restartAt;
							szAt = strstr(szAt,"Schedule Type:");
							if (szAt != NULL)
								szAt += strlen("Schedule Type:");
						}
						else
							szAt += strlen("Scheduled Type:");
						if (szAt != NULL)
						{
							while (*szAt == ' ') szAt++;
							while ((*szAt != ' ') && (*szAt != '\r') && (*szAt != '\n'))szType[iInPoint++] = *szAt++;
							szType[iInPoint] = NULL;

							// special case
							if ('O' == szType[0] && strstr(szType,"One")) // for new "One Time Only, Hourly" syntax
							{
								strcpy(szType, "Hourly");
							}
						}

						//get the start time
						szAt = strstr(szAt,"Start Time:");
						if (szAt != NULL)
						{
							szAt += strlen("Start Time:");// this block had assumed 2 digit military for the hours from schtasks output, W7 begs to differ
							while (*szAt == ' ') szAt++;
							szHours[0] = *szAt++;
							if (*szAt == ':') // W7 single digit (have not checked Vista. Server 2003 on upwards, 'prolly the same)
							{ 
								m_XPOrMilTime = false;
								if (mon[0] == 'A')
								{
									szHours[1] = szHours[0]; szHours[0]  = '0'; 
								} 
								else if (mon[0] == 'P') 
								{ 
									szHours[1] = NULL;
									int h = atoi(szHours);
									h += 12;
									itoa(h,szHours, 10);
								} 
							} 
							else
							{
								szHours[1] = *szAt++;   
								if (mon[0] == 'P')
								{ 
									// W7 single digit translation again here
									m_XPOrMilTime = false;
									int h = atoi(szHours);
									h += 12;
									itoa(h,szHours, 10);
								}
							}
							szHours[2] = NULL;
							szAt++;
							szMinutes[0] = *szAt++;
							szMinutes[1] = *szAt++;
							szMinutes[2] = NULL;
						}

						//get the days
						iInPoint = 0;
						szAt = strstr(szAt,"Days:");
						if (szAt != NULL)
						{
							szAt += strlen("Days:");
							while (*szAt == ' ') szAt++;
							while ((*szAt != ' ') && (*szAt != '\r') && (*szAt != '\n'))szDays[iInPoint++] = *szAt++;
						}
						szDays[iInPoint] = NULL;

						char szSchINI[16];
						char szDOMINI[16];
						char szDOWINI[16];
						char szHoursINI[4];
						char szMinutesINI[4];
						GetPrivateProfileString(m_pName,"AUTORUN","DAILY",szSchINI,sizeof(szSchINI),m_szIniFile);
						GetPrivateProfileString(m_pName,"DAYOFMONTH","1",szDOMINI,sizeof(szDOMINI),m_szIniFile);
						GetPrivateProfileString(m_pName,"DAYOFWEEK","SUN",szDOWINI,sizeof(szDOWINI),m_szIniFile);
						GetPrivateProfileString(m_pName,"HOURS","00",szHoursINI,sizeof(szHoursINI),m_szIniFile);
						GetPrivateProfileString(m_pName,"MINUTES","00",szMinutesINI,sizeof(szMinutesINI),m_szIniFile);

						bool bFail = false, bFailOpt = false;
						if (toupper(szLog[0]) != toupper(szReadLogINI[0]))
						{
							bFailOpt = true;
						}
						if (toupper(szType[0]) != toupper(szSchINI[0]))
						{
							bFail = true;
						}
						else
						{
							switch(toupper(szSchINI[0])) {
							case 'H':
								if (strcmp(szMinutesINI,szMinutes) != 0) bFail = true;
								break;
							case 'D':
								if (strcmp(szHoursINI,szHours) != 0) bFail = true;
								if (strcmp(szMinutesINI,szMinutes) !=0) bFail = true;
								break;
							case 'W':
								if (strcmp(szHoursINI,szHours) != 0) bFail = true;
								if (strcmp(szMinutesINI,szMinutes) !=0) bFail = true;
								if (strnicmp(szDOWINI,szDays,3) != 0) bFail = true;
								break;
							case 'M':
								if (strcmp(szHoursINI,szHours)!= 0) bFail = true;
								if (strcmp(szMinutesINI,szMinutes)!=0) bFail = true;
								if (stricmp(szDOMINI,szDays) != 0) bFail = true;
								break;
							default:
								break;
							}
						}

						if (bFail || bFailOpt)
						{
							m_dCurrent_State = eSchtasksDelete;
							char tempus[128], tempwtf[128];
							if (bFail && !bFailOpt)
							{
								strcpy(tempus,"Scheduling ");
								strcat(tempus, "of MIC_EOSSReader does NOT match MIC");
							}
							else 
							{
								if (bFail && bFailOpt)
									strcpy(tempus,"Scheduling and options ");
								else if (!bFail && bFailOpt)
									strcpy(tempus,"Options ");
								strcat(tempus, "of MIC_EOSSReader do NOT match MIC");
							}

							strcpy(tempwtf, "47303 EOSS "); strcat(tempwtf, tempus);

							strcat(tempus, ".");

							m_pPropertySheet->UpdateSummary(tempus);
							m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,tempwtf);
						}
						else
						{
							// check for full v presence interval use
							m_dCurrent_State = eGatherRegistryResults;
							m_bReSchedule = false;
						}
					}
					else
					{
						m_dCurrent_State = eSchtasksRescheduling; 
						//char temp[256];
						//sprintf(temp, "475 70EOSS shifting to 3000 because '%s' name not found in schtask list", m_pName);
						//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,temp);
					}

					DeleteFile(m_szFileName1);
				}
				catch (...)
				{
					bFailedForSomeReason = true;
				}
			}
			else
			{
				bFailedForSomeReason = true;
			}

			if (bFailedForSomeReason)
			{
				m_dCurrent_State = eStartSchtasksCheckWrite;  // start over
				if (m_pPropertySheet)
				{
					errno_t err;
					_get_errno( &err );
					char szBuff[2048];
					sprintf(szBuff,"47105 EOSS Cannot create EOSS compare task script at %.256s; (%.64s)",m_szFileName1, strerror(err));
					m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,szBuff);
					m_pPropertySheet->UpdateSummary(szBuff);
				}
			}
		}
		break;

	case eSchtasksDelete:
		m_pPropertySheet->UpdateSummary("Deleting old MIC_EOSSReader schedule.");
		sprintf(m_szFileName1,"c:\\%s.bat",m_pName);
		m_file = fopen(m_szFileName1,"wt");
		if (m_file != NULL)
		{
			fprintf(m_file,"schtasks /Delete /TN \"%s\" /F\n", m_pName);
			fclose(m_file);
			ShellExecute(m_hWnd,"open",m_szFileName1,"","c:\\",SW_HIDE);
		}
		else
		{
			m_dCurrent_State = eStartSchtasksCheckWrite;  // start over
			if (m_pPropertySheet)
			{
				errno_t err;
				_get_errno( &err );
				char szBuff[2048];
				sprintf(szBuff,"47106 EOSS Cannot create EOSS task schedule deletion script at %.256s; (%.64s)",m_szFileName1, strerror(err));
				m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,szBuff);
				m_pPropertySheet->UpdateSummary(szBuff);
			}
		}
		m_dTick_Count = 0;
		m_dCurrent_State = eSchtasksDeleteWaitOnCompletion;
		// once = true;
		break;

	case eSchtasksDeleteWaitOnCompletion:
		if (m_dTick_Count >= 4)
		{
			m_dCurrent_State = eSchtasksRescheduling;
			//char temp[256];
			//sprintf(temp, "47571 EOSS shifting to 3000 because %d > 4 ticks have passed", m_dTick_Count);
			//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,temp);
		}
		break;

	case eSchtasksRescheduling://add this ISO to the schedule
		{
			sprintf(m_szFileName1,"c:\\%s.bat",m_pName);
			m_file = fopen(m_szFileName1,"wt");
			if (m_file != NULL)
			{
				char szSch[16];
				char szDOM[16];
				char szDOW[16];
				char szHours[4];
				char szMinutes[4];
				char szUser[MAX_PATH];
				char szPW[MAX_PATH];
				GetPrivateProfileString(m_pName,"AUTORUN","DAILY",szSch,sizeof(szSch),m_szIniFile);
				GetPrivateProfileString(m_pName,"DAYOFMONTH","1",szDOM,sizeof(szDOM),m_szIniFile);
				if (atoi(szDOM) < 1) strcpy(szDOM,"1");
				GetPrivateProfileString(m_pName,"DAYOFWEEK","SUN",szDOW,sizeof(szDOW),m_szIniFile);
				GetPrivateProfileString(m_pName,"HOURS","00",szHours,sizeof(szHours),m_szIniFile);
				if (atoi(szHours) <= 0) strcpy(szHours,"00");
				GetPrivateProfileString(m_pName,"MINUTES","00",szMinutes,sizeof(szMinutes),m_szIniFile);
				if (atoi(szMinutes) <= 0) strcpy(szMinutes,"00");
				GetPrivateProfileString(m_pName,"ACCOUNT_USER","system",szUser,sizeof(szUser),m_szIniFile);
				GetPrivateProfileString(m_pName,"ACCOUNT_PW","system",szPW,sizeof(szPW),m_szIniFile);

				char szDrive[_MAX_DRIVE];
				char szPath[_MAX_DIR];
				char szName[_MAX_FNAME];
				char szExt[_MAX_EXT];
				char szReaderPath[_MAX_PATH];
				char szShortReaderPath[_MAX_PATH];
				_splitpath(m_szIniFile,szDrive,szPath,szName,szExt);
				_makepath(szReaderPath,szDrive,szPath,"MIC_EOSSReader","exe");
				if (GetShortPathName(szReaderPath,szShortReaderPath,sizeof(szShortReaderPath)) == 0)
					strcpy(szShortReaderPath,szReaderPath);

				//fixup schedule
				_strupr(szSch);

				//fixup DOW
				szDOW[3] = NULL;
				_strupr(szDOW);

				char temp1[256], temp2[256];	
				strcpy(temp2, "Rescheduling MIC_EOSSReader: ");		strcat(temp2, szSch);

				//build schtasks command line depending on ISO's configuration
				fprintf(m_file,"schtasks /Create /RU \"%s\" /RP \"%s\" /SC %s ", szUser, szPW, szSch);
				switch (szSch[0]) {
					case 'M':
						fprintf(m_file,"/D %s /ST %s:%s%s ",szDOM, szHours, szMinutes, XTRASECS);
						sprintf(temp1, " %s %s:%s%s",szDOM, szHours, szMinutes, XTRASECS);
						break;
					case 'W':
						fprintf(m_file,"/D %s /ST %s:%s%s ",szDOW, szHours, szMinutes, XTRASECS);
						sprintf(temp1, " %s %s:%s%s",szDOW, szHours, szMinutes, XTRASECS);
						break;
					case 'D':
						fprintf(m_file,"/ST %s:%s%s ", szHours, szMinutes, XTRASECS);
						sprintf(temp1, " %s:%s%s", szHours, szMinutes, XTRASECS);
						break;
					case 'H':
						fprintf(m_file,"/ST 00:%s%s ",szMinutes, XTRASECS);
						sprintf(temp1, " 00:%s%s",szMinutes, XTRASECS);
						break;
				}
				strcat(temp2, temp1);			strcat(temp2, ".");
				m_pPropertySheet->UpdateSummary(temp2);

				bool brlog = (GetPrivateProfileInt(m_pName,READ_LOG,1,m_szIniFile)==1);
				if (brlog)
				{
					fprintf(m_file,"/TN \"%s\" /TR \"%s %s -v -log -f %s\"\n",m_pName,szShortReaderPath,m_pName, m_szIniFile);
				}
				else
				{
					fprintf(m_file,"/TN \"%s\" /TR \"%s %s -f %s\"\n",m_pName,szShortReaderPath,m_pName, m_szIniFile);
				}

				fclose(m_file);
				ShellExecute(m_hWnd,"open",m_szFileName1,"","c:\\",SW_HIDE);
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47410 EOSS Rescheduling MIC_EOSSReader");
				ResetReaderCount(); // set the interval full vs status counter used by the reader process back to 0
				//wait about 4 seconds to let it run
				m_dTick_Count = 0;
				m_bReSchedule = false;
				m_dCurrent_State = eSchtasksReschedulingWaitOnCompletion;
			}
			else
			{
				m_dCurrent_State = eStartSchtasksCheckWrite;  // start over
				if (m_pPropertySheet)
				{
					errno_t err;
					_get_errno( &err );
					char szBuff[2048];
					sprintf(szBuff,"47107 EOSS Cannot create EOSS reschedule task script at %.256s; (%.64s)",m_szFileName1, strerror(err));
					m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,szBuff);
					m_pPropertySheet->UpdateSummary(szBuff);
				}
			}
		}
		break;

	case eSchtasksReschedulingWaitOnCompletion:
		if (m_dTick_Count > 4) 
			m_dCurrent_State = eStartSchtasksCheckWrite;
		break;

	case eGatherRegistryResults://update this ISO from the registry
		//check if the file format has changed
		{
			unsigned long lRet;
			HKEY hKey;

			//get the current file format from the INI
			char szFormat[MAX_PATH];
			char szTemp[16];
			GetPrivateProfileString(CONFIGURATION,LONGNAMEFORM,"%T%I_%y%m%d_%H%M%S",szFormat,sizeof(szFormat),m_szIniFile);
			GetPrivateProfileString(CONFIGURATION,DOSHORTNAME,"No",szTemp,sizeof(szTemp),m_szIniFile);
			if (toupper(szTemp[0]) == 'Y')
				strcpy(szFormat,"SHORTFORM");
			else
				strcat(szFormat,".ess");
			//if it doesn't match what we have saved then 
			//write it out to the registry
			if (strcmp(szFormat,m_szFormat)!= 0)
			{
				char szName[MAX_PATH];
				//update the registry
				strcpy(szName,REGISTRYLOCATION);
				strcat(szName,m_pName);

				//open the registry where the values are stored
				lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_ALL_ACCESS, &hKey);
				if (lRet == ERROR_SUCCESS)
				{
					lRet = RegSetValueExA(hKey,"FORMAT",NULL,REG_SZ,(BYTE*)szFormat,strlen(szFormat)+1);
					//save it so we don't have to 
					strcpy(m_szFormat,szFormat);
					RegCloseKey( hKey );
					sGeneral.cFORMAT = "FORMAT...\r\n\r\n";
					sGeneral.cFORMAT += szFormat;
				}
			}
		}
		//read registry and update everything (inc. CEV and PFM)
		//if no update (date of last has not changed)
		if (ReadRegistry())
		{
			m_pDataStatus->GetDlgItem(IDC_BUTTON_REMOVEDATA)->EnableWindow(TRUE);
			m_pPropertySheet->UpdateSummary("Reading MIC_EOSSReader's output.");
			m_pPropertySheet->SetTabColor(IDC_EOSSDATA_TAB,
				((m_iColorFlagS | m_iColorFlagG) == 0)?COLOR_GREEN:
				((m_iColorFlagS | m_iColorFlagG) == 1)?COLOR_YELLOW:
				((m_iColorFlagS | m_iColorFlagG) == 2)?COLOR_RED:
				((m_iColorFlagS | m_iColorFlagG) == 3)?COLOR_REDYELLOW:COLOR_WHITE); // jfl, note map of status 3 to image 10, m_iColorFlagS set *only* by ValidateSeal
			m_pSummaryPage->PopulateChecker();
			m_pSummaryPage->m_ctrlChecker.Refresh();

			m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW)->EnableWindow(TRUE);
			m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW2)->EnableWindow(TRUE);
			m_pDataStatus->GetDlgItem(IDC_BUTTON_REMOVEDATA)->EnableWindow(TRUE);

			m_pButton->ShowIcon_Frown(VARIANT_FALSE);
			m_pButton->put_FaceColor(COLOR_OK);
			if (m_pPropertySheet) {
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_OK,true);
				//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47504 EOSS COLOR_OK Reading MIC_EOSSReader's output from registry");
			}

			//Dun dun DUN.Claire was here. 
			m_dCurrent_State = eStartSchtasksCheckWrite;
			m_bHardFailResults = QueryLatestResultsForHardFail();
			m_uReaderCheckCount = 0;
			m_bHWNotified = false;  // emit notice after any actual read event
			if (m_uReaderCompletionCount < UINT_MAX) m_uReaderCompletionCount++; else m_uReaderCompletionCount = 0; // auto-rollover
		}
		else
		{
			m_pPropertySheet->UpdateSummary("MIC_EOSSReader's output not changed.");//jfl to do: change this text, 
			if (m_uReaderCheckCount < UINT_MAX) m_uReaderCheckCount++; else m_uReaderCheckCount = 0; // auto-rollover
			m_dCurrent_State = eButtonAndReaderProcessStatusCheck;
		}

		if (m_bHardFailResults)
		{
			if ((m_uReaderCheckCount >= m_uHardFailCount) || // if nnn minutes has gone by in the hard fail state
				(m_uReaderCycles >= m_uHardFailCheckCycles)) // or if reader ran more than once 
			{
				if ((m_uHardFailCount > 0) && ((m_uReaderCheckCount % m_uHardFailCount) == 0))  // emit notice modulo minutes of the check
					m_bHWNotified = false;
				m_pButton->ShowIcon_Frown(VARIANT_TRUE);
				m_pButton->put_FaceColor(COLOR_ERROR);
				if (m_pPropertySheet) 
				{
					m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
					if (!m_bHWNotified) // notify only when directly read or after modulo count minute chexks
					{
						m_pPropertySheet->UpdateSummary("MIC_EOSSReader cannot detect any EOSS seals.");
						//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47502 COLOR_ERROR after hard failure limits reached");
						m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47330 EOSS MIC_EOSSReader cannot detect any EOSS seals, check hardware connections and MIC configuration");
						m_bHWNotified = true;
					}
				}
			}
		}

		m_dTick_Count = 0;
		m_bUpdateDataDisplay = false;

		break;

	case eRunDirectReaderProcess:
		m_pPropertySheet->UpdateSummary("Running MIC_EOSSReader now.");
		sprintf(m_szFileName1,"c:\\%s.bat",m_pName);
		m_file = fopen(m_szFileName1,"wt");
		if (m_file == NULL)
		{
			if (m_pPropertySheet)
			{
				errno_t err;
				_get_errno( &err );
				char szBuff[2048];
				sprintf(szBuff,"47108 EOSS Cannot create EOSS reader task script at %.256s; (%.64s)",m_szFileName1, strerror(err));
				m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,szBuff);
				m_pPropertySheet->UpdateSummary(szBuff);
			}
		} else {
			fprintf(m_file,"schtasks /Run /TN \"%s\"\n",m_pName);
			fclose(m_file);
			SetDepthForManualOveride();
			int sres = (int)ShellExecute(m_hWnd,"open",m_szFileName1,"","c:\\",SW_HIDE);
			///if (m_pPropertySheet) { char buff[512]; sprintf(buff, "47550 EOSS ShellExecute returned %d in manual run attempt %s", sres, m_szFileName1); m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, buff); }
		}
		m_dTick_Count = 0;
		m_dCurrent_State = eRunWaitOnDirectReaderProcess;
		m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW)->EnableWindow(FALSE);
		m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW2)->EnableWindow(FALSE);
		m_pDataStatus->GetDlgItem(IDC_BUTTON_REMOVEDATA)->EnableWindow(FALSE);
		m_pButton->put_FaceColor(COLOR_WAIT);
		if (m_pPropertySheet) {
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47551 EOSS COLOR_WAIT manually running MIC_EOSSReader now.");
		}

		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47406 EOSS Manually running MIC_EOSSReader");
		break;

	case eRunWaitOnDirectReaderProcess:
		if (m_dTick_Count >= 4)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = eButtonAndReaderProcessStatusCheck;
		}
		break;

	case eButtonAndReaderProcessStatusCheck://wait for next inquire time or pause

		if (m_cNextScheduled.GetStatus() == COleDateTime::valid)
		{
			COleDateTimeSpan cSpan;
			cSpan = m_pNow - m_cNextScheduled;
			int span = abs((int)cSpan.GetTotalSeconds());
			if (m_bTokLibTrigger && span < 15)
			{
				// seems to hang up restart by schtask use of mic_reader 
				// KillEOSS();
				m_bTokLibTrigger = false;
				m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW)->EnableWindow(FALSE);
				m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW2)->EnableWindow(FALSE);
				m_pDataStatus->GetDlgItem(IDC_BUTTON_REMOVEDATA)->EnableWindow(FALSE);
				m_pButton->put_FaceColor(COLOR_WAIT);
				if (m_pPropertySheet)
				{
					m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
					//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47500 EOSS Stopped existing EOSS Token process, time < 15 seconds"); // jfl
				}
			} else
			{
				m_bTokLibTrigger = true; 
			}
			//	if (m_pPropertySheet)
			//	{
			//		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47501 EOSS Time > 15 seconds to next"); // jfl
			//	}
		}

		if (m_bPause) 
			m_dCurrent_State = ePauseState;
		else if (m_bRunNow)
		{
			//if (m_pPropertySheet) m_pPropertySheet->UpdateSummary("Stopping existing EOSS Token process, preparing to run MIC seal reader.");
			KillEOSS();
			m_bRunNow = false;
			m_dCurrent_State = eWaitOnKillEOSS;
		}
		else if (m_bReSchedule)
		{
			m_dCurrent_State = eStartSchtasksCheckWrite;
		}
		else if (m_bRemoveOld)
		{
			m_dCurrent_State = eClearRegistry;
		}
		else if (m_bUpdateDataDisplay)
		{
			m_dCurrent_State = eGatherRegistryResults;
		}
		else
		{
			if (m_dTick_Count >= CHECKPOINTTICKS)
			{
				m_dCurrent_State = eRunTimerIntervalReaderProcess;
			}
		}
		break;
	case eRunTimerIntervalReaderProcess:
		{
			HANDLE hMutex;
			//check if MIC-EOSSReader is running
			if ((hMutex = CreateMutexA(NULL,TRUE,"MIC_EOSSReader")) != NULL)
			{
				DWORD dwResult = ::GetLastError();
				CloseHandle(hMutex);
				ReleaseMutex(hMutex);
				if (dwResult == ERROR_ALREADY_EXISTS)
				{
					//is running
					m_pButton->put_FaceColor(COLOR_OK);
					if (m_pPropertySheet) {
						m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_OK,true);
						//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47503 EOSS COLOR_OK reader running but ERROR_ALREADY_EXISTS, Tick_Count >= 59");
					}
					m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW)->EnableWindow(FALSE);
					m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW2)->EnableWindow(FALSE);
					m_pDataStatus->GetDlgItem(IDC_BUTTON_REMOVEDATA)->EnableWindow(FALSE);
					m_bPrimed = true;
				}
				else
				{
					//not running
					if (m_bPrimed)
					{
						m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW)->EnableWindow(TRUE);
						m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW2)->EnableWindow(TRUE);
						m_pDataStatus->GetDlgItem(IDC_BUTTON_REMOVEDATA)->EnableWindow(TRUE);

						m_pButton->put_FaceColor(COLOR_WAIT);
						if (m_pPropertySheet)
						{
							m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
							//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47505 EOSS COLOR_WAIT, primed, not running, reader running check? Tick_Count >= 59");
						}

						m_bPrimed = false;
					}
					else
					{
						//if (m_pPropertySheet) {
						//	m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47506 EOSS NOT primed, not running, reader running check? Tick_Count >= 59");
						//}
					}
				}
			}
			else
			{
				//is running
				DWORD dwResult = ::GetLastError();
				if (dwResult == ERROR_ALREADY_EXISTS)
				{
					m_pButton->put_FaceColor(COLOR_OK);
					if (m_pPropertySheet) {
						m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_OK,true);
						//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47507 EOSS COLOR_OK reader is already running");
					}
					m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW)->EnableWindow(FALSE);
					m_pDataStatus->GetDlgItem(IDC_BUTTON_RUNNOW2)->EnableWindow(FALSE);
					m_pDataStatus->GetDlgItem(IDC_BUTTON_REMOVEDATA)->EnableWindow(FALSE);
					m_bPrimed = true;
				} 
				else
					if (m_pPropertySheet) {
						//m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47508 EOSS OK reader is running, with no recent error available");
					}
			}
			m_dCurrent_State = eGatherRegistryResults;
		}
		break;

	case eWaitOnKillEOSS:	//wait about 4 seconds after the KillEOSS then continue
		if (m_dTick_Count >= 4)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = eRunDirectReaderProcess;
		}
		break;

	case eClearRegistry:
		CleanRegistry();
		m_bRemoveOld = false;
		m_dCurrent_State = eGatherRegistryResults;
		if (m_uReaderCompletionCount > 0) m_uReaderCompletionCount--;  // decrement count because the clear will increment it
		break;

	case ePauseState://pause
		//decrement pause
		//at end of pause go to 1000

		break;

	default:
		m_dCurrent_State = eStartSchtasksCheckWrite;
		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"47100 EOSS State machine fault");
		}
	}
	catch (...)
	{
		m_dCurrent_State = eStartSchtasksCheckWrite;
		m_pPropertySheet->WriteToFile(TO_PFM,TYPE_COMP, NULL,"47109 EOSS State machine general fault");
	}

	return 0;
}
