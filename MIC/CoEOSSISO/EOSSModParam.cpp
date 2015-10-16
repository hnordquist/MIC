// EOSSModParam.cpp : implementation file

#include "stdafx.h"
#include "invalid_chars.h"
#include "INI_definitions.h"
#include "ISO_definitions.h"
#include "resource.h"
#include "colors.h"
#include "EOSSModParam.h"
#include "DirDialog.h"
#include "depth.h"
#include "EOSSInstrument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EOSS_DEFAULT		"EOSS Default"


#define MAXPAUSE_SPIN_MIN	0
#define MAXPAUSE_SPIN_MAX	99999
#define MAXPAUSE_SPIN_DEF	"60"

#define MAXINQUIRE_SPIN_MIN 2
#define MAXINQUIRE_SPIN_MAX 32767
#define MAXINQUIRE_SPIN_DEF 10

#define FILEID_SPIN_MAX	1035
#define FILEID_SPIN_MIN	0
#define FILEID_SPIN_DEF	"01"

#define SAVELOC_DEF			"C:\\DATA\\EOSS01"
#define PORT_DEF			"ERROR"
/////////////////////////////////////////////////////////////////////////////
// CModParam property page

IMPLEMENT_DYNCREATE(CEOSSModParam, CPropertyPage)

CEOSSModParam::CEOSSModParam() : CPropertyPage(CEOSSModParam::IDD)
{
}

CEOSSModParam::CEOSSModParam(
	UINT id, 
	CISO* pParent,  
	bool bDoingConfig) 
	: CPropertyPage(id)
{
	m_bAllowFinish = false;
	m_pParent = pParent;
	m_bChange = m_bIntervalChange = false;
	m_datasetup = false;
	m_bCollectingParametersForNewISO = bDoingConfig;
	m_pPS = NULL;
	m_pApplyButton = NULL;

	for (int i= IDC_EOSSMODPARAMS_FIRST; i <= IDC_EOSSMODPARAMS_LAST; i++)
	{
		//StaticColors[i-IDC_EOSSMODPARAMS_FIRST] = COLOR_DEFAULT_TEXT;
	}

	if (m_bCollectingParametersForNewISO)
	{
		//See if default section of INI file is already there...
		char testbuf[16];
		int numchars = GetPrivateProfileSection(
			EOSS_DEFAULT,
			testbuf,
			sizeof(testbuf),
			m_pParent->m_szIniFile);

		//If not, then go write it
		if (numchars == 0)
			WriteIniFileDefaultSection();
	}
}

void CEOSSModParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEOSSModParam)
	DDX_Control(pDX, IDC_EOSSMODPARAMS_OFFSETHOURS, m_cCombo_Offset);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_MINUTES, m_cCombo_Minutes);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_HOURS, m_cCombo_Hours);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_DOW, m_cCombo_DOW);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_DOM, m_cCombo_DOM);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_COMBOWHEN, m_cComboWhen);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_FILEID_SPIN, m_EOSSModParamsFileIDSpin);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_FILEID, m_EOSSModParamsFileID);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_READER_LOG, m_bReaderLogging);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_PRESENCE_CHECK, m_bFullDataInterval);
	DDX_Control(pDX, IDC_EOSSMODPARAMS_INTERVAL, m_EditInterval);
	//}}AFX_DATA_MAP
}

CEOSSModParam::~CEOSSModParam()
{
	delete m_pApplyButton;
}

BEGIN_MESSAGE_MAP(CEOSSModParam, CPropertyPage)
	//{{AFX_MSG_MAP(CEOSSModParam)
	ON_BN_CLICKED(IDC_EOSSMODPARAMS_APPLY, OnModparamsApply)
	ON_BN_CLICKED(IDC_EOSSMODPARAMS_BROWSE, OnModparamsBrowse)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EOSSMODPARAMS_FILEID_SPIN,   OnDeltaposModparamsFileidSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EOSSMODPARAMS_MAXPAUSE_SPIN, OnDeltaposModparamsMaxpauseSpin)
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_MAXPAUSE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_FILEID, OnChangeModparamsFileID)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_SAVELOC, OnChangeModparamsSaveloc)
	ON_EN_KILLFOCUS(IDC_EOSSMODPARAMS_FILEID, OnKillfocusModparamsFileid)
	ON_CBN_EDITCHANGE(IDC_EOSSMODPARAMS_COMBOWHEN, OnEditchangeComboWhen)
	ON_CBN_SELCHANGE(IDC_EOSSMODPARAMS_COMBOWHEN, OnSelchangeComboWhen)
	ON_CBN_EDITCHANGE(IDC_EOSSMODPARAMS_OFFSETHOURS, OnEditchangeEossmodparamsOffsethours)
	ON_CBN_SELCHANGE(IDC_EOSSMODPARAMS_OFFSETHOURS, OnEditchangeEossmodparamsOffsethours)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_ACCOUNT, OnChangeEossmodparamsAccount)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_PW, OnChangeEossmodparamsPw)
	ON_CBN_SELCHANGE(IDC_EOSSMODPARAMS_DOM, OnSelchangeEossmodparamsDom)
	ON_CBN_SELCHANGE(IDC_EOSSMODPARAMS_HOURS, OnSelchangeEossmodparamsHours)
	ON_CBN_SELCHANGE(IDC_EOSSMODPARAMS_MINUTES, OnSelchangeEossmodparamsMinutes)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_MAINBATLOW, OnChangeEossmodparamsMainbatlow)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_MAINBATCRIT, OnChangeEossmodparamsMainbatcrit)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_EMERBATLOW, OnChangeEossmodparamsEmerbatlow)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_EMERBATCRIT, OnChangeEossmodparamsEmerbatcrit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EOSSMODPARAMS_MAINBATLOW_SPIN, OnDeltaposEossmodparamsMainbatlowSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EOSSMODPARAMS_MAINBATCRIT_SPIN, OnDeltaposEossmodparamsMainbatcritSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EOSSMODPARAMS_EMERBATLOW_SPIN, OnDeltaposEossmodparamsEmerbatlowSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EOSSMODPARAMS_EMERBATCRIT_SPIN, OnDeltaposEossmodparamsEmerbatcritSpin)
	ON_BN_CLICKED(IDC_EOSSMODPARAMS_DEFAULT, OnModparamsDefaultBut)
	ON_BN_CLICKED(IDC_EOSSMODPARAMS_RESET, OnModparamsReset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EOSSMODPARAMS_TIMEERROR_SPIN, OnDeltaposEossmodparamsTimeerrorSpin)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_TIMEERROR, OnChangeEossmodparamsTimeerror)
	ON_BN_CLICKED(IDC_EOSSMODPARAMS_READER_LOG, OnBnClickedEossmodparamsReaderLog)
	ON_BN_CLICKED(IDC_EOSSMODPARAMS_PRESENCE_CHECK, OnBnClickedEossmodparamsFullIntervalCheck)
	ON_EN_CHANGE(IDC_EOSSMODPARAMS_INTERVAL, OnChangeEossmodparamsInterval)
	ON_EN_KILLFOCUS(IDC_EOSSMODPARAMS_INTERVAL, OnKillfocusEossmodparamsInterval)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEOSSModParam::SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox)
{
	char temp[MAX_PATH+1];
	if (FromCheckBox)
		strcpy(temp,(((CButton*)GetDlgItem(dDlgItem))->GetState()&0001)?"1":"0");
	else
		GetDlgItem(dDlgItem)->GetWindowText(temp,sizeof(temp));
	while (temp[strlen(temp)-1]=='\\')
		temp[strlen(temp)-1]=NULL;
	WritePrivateProfileString(szName, szItem, temp, m_pParent->m_szIniFile);
}

void CEOSSModParam::DoApply()
{
#define BUFSIZE 1024

	char *Name = m_pParent->m_pName;
	char Temp[_MAX_PATH];

	char szComPort[8];
	char szPath[MAX_PATH];
	char szFormat[MAX_PATH];
	char szTimeOffset[8];
	char szName[BUFSIZE];
	unsigned long lRet;
	HKEY hKeyLANL;
	HKEY hKeyMIC_GM;
//	DWORD dwBufLen;
	HKEY hResult;


	//get all the stuff and save it
	bool bCarryOn;

	if (m_bCollectingParametersForNewISO)
	{
		bCarryOn = true;
	}
	else
	{
		bCarryOn = m_pParent->Validate();
	}

	if (bCarryOn)
	{
		char szID[4];

		SaveItem(Name,"MAXPAUSE",	IDC_EOSSMODPARAMS_MAXPAUSE);
		SaveItem(Name, FILE_ID,		IDC_EOSSMODPARAMS_FILEID);
		SaveItem(Name,"ACCOUNT_USER",IDC_EOSSMODPARAMS_ACCOUNT);
		SaveItem(Name,"ACCOUNT_PW",IDC_EOSSMODPARAMS_PW);
		SaveItem(Name,"AUTORUN",IDC_EOSSMODPARAMS_COMBOWHEN);
		SaveItem(Name,"DAYOFMONTH",IDC_EOSSMODPARAMS_DOM);
		SaveItem(Name,"DAYOFWEEK",IDC_EOSSMODPARAMS_DOW);
		SaveItem(Name,"HOURS",IDC_EOSSMODPARAMS_HOURS);
		SaveItem(Name,"MINUTES",IDC_EOSSMODPARAMS_MINUTES);
		SaveItem(Name,"PORT",IDC_EOSSMODPARAMS_PORT);
		SaveItem(Name,"MAIN_BATT_LOW",IDC_EOSSMODPARAMS_MAINBATLOW);
		SaveItem(Name,"MAIN_BATT_CRIT",IDC_EOSSMODPARAMS_MAINBATCRIT);
		SaveItem(Name,"EMER_BATT_LOW",IDC_EOSSMODPARAMS_EMERBATLOW);
		SaveItem(Name,"EMER_BATT_CRIT",IDC_EOSSMODPARAMS_EMERBATCRIT);
		SaveItem(Name,"TIME_OFFSET",IDC_EOSSMODPARAMS_OFFSETHOURS);
		SaveItem(Name,"TIME_ERROR",IDC_EOSSMODPARAMS_TIMEERROR);
		SaveItem(Name,READ_LOG,IDC_EOSSMODPARAMS_READER_LOG, true);
		SaveItem(Name,USE_FULL_DATA_INTERVAL,IDC_EOSSMODPARAMS_PRESENCE_CHECK, true);
		SaveItem(Name,FULL_DATA_INTERVAL,IDC_EOSSMODPARAMS_INTERVAL);

		int intervalcheck = m_bFullDataInterval.GetCheck();
		BOOL transgood;
		int interval = GetDlgItemInt(IDC_EOSSMODPARAMS_INTERVAL, &transgood, FALSE);
		if (!transgood)
			interval = 0;

		//get the instrument ID or "station number"
		GetDlgItemText(IDC_EOSSMODPARAMS_FILEID,szID,sizeof(szID));

		//get the time offset
		GetDlgItemText(IDC_EOSSMODPARAMS_OFFSETHOURS,szTimeOffset,sizeof(szTimeOffset));

		//clean up the save location and save it
		GetDlgItemText(IDC_EOSSMODPARAMS_SAVELOC,Temp,sizeof(Temp));
		while ((Temp[strlen(Temp)-1]=='\\')||
			   (Temp[strlen(Temp)-1]=='/' )  ) 
		{
			Temp[strlen(Temp)-1] = NULL;
			SetDlgItemText(IDC_EOSSMODPARAMS_SAVELOC,Temp);
		}
		SaveItem(Name,SAVE_LOC,	IDC_EOSSMODPARAMS_SAVELOC);

		//get the save location 
		GetDlgItemText(IDC_EOSSMODPARAMS_SAVELOC,szPath,sizeof(szPath));

		//make sure the path exists and create it if it doesn't
		strcat(Temp,"\\archive");
		if (_access(Temp,0)==-1)
			m_pParent->MakeMyPath(Temp);

		//get the number part of the com port the EOSSes are attached to
		GetDlgItemText(IDC_EOSSMODPARAMS_PORT,Temp,sizeof(Temp));
		GetPrivateProfileString(Temp,PORT,"",szComPort,sizeof(szComPort),m_pParent->m_szIniFile);
		strcpy(szComPort,&szComPort[3]);

		//get the file format
		GetPrivateProfileString(CONFIGURATION,LONGNAMEFORM,"%T%I_%y%m%d_%H%M%S",szFormat,sizeof(szFormat),m_pParent->m_szIniFile);
		GetPrivateProfileString(CONFIGURATION,DOSHORTNAME,"No",Temp,sizeof(Temp),m_pParent->m_szIniFile);
		if (toupper(Temp[0]) == 'Y')
			strcpy(szFormat,"SHORTFORM");
		else
			strcat(szFormat,".ess");

		//clear the flags
		if (m_bCollectingParametersForNewISO)
		{
			WritePrivateProfileString(Name,"FLAGCOMM","0",m_pParent->m_szIniFile);
			WritePrivateProfileString(Name,"FLAGOTHR","0",m_pParent->m_szIniFile);
			WritePrivateProfileString(Name,"FLAGTIME","0",m_pParent->m_szIniFile);
		}

		strcpy(szName,"SOFTWARE\\Los Alamos National Laboratory");

		//open the registry where the values are stored and save the new values
		DWORD dwDisposition;
		lRet = RegCreateKeyExA(HKEY_LOCAL_MACHINE,szName,0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,0,&hKeyLANL,&dwDisposition);
		if (lRet == ERROR_SUCCESS)
		{
			strcat(szName,"\\MIC_GM");
			lRet = RegCreateKeyExA(HKEY_LOCAL_MACHINE,szName,0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,0,&hKeyMIC_GM,&dwDisposition);
			//lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_ALL_ACCESS, &hKeyMIC_GM);
			if (lRet == ERROR_SUCCESS)
			{
				lRet = RegCreateKeyExA(hKeyMIC_GM,Name,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&hResult,&dwDisposition);
				if (lRet == ERROR_SUCCESS)
				{
					lRet = RegSetValueExA(hResult,"COMPORT",NULL,REG_SZ,(BYTE*)szComPort,strlen(szComPort)+1);
					lRet = RegSetValueExA(hResult,"PATH",NULL,REG_SZ,(BYTE*)szPath,strlen(szPath)+1);
					lRet = RegSetValueExA(hResult,"FORMAT",NULL,REG_SZ,(BYTE*)szFormat,strlen(szFormat)+1);
					lRet = RegSetValueExA(hResult,"INSTRUMENT ID",NULL,REG_SZ,(BYTE*)szID,strlen(szID)+1);
					lRet = RegSetValueExA(hResult,"TIME OFFSET",NULL,REG_SZ,(BYTE*)szTimeOffset,strlen(szTimeOffset)+1);
					lRet = RegSetValueExA(hResult,USE_FULL_DATA_INTERVAL,NULL,REG_BINARY,(BYTE*)&intervalcheck,sizeof(intervalcheck));
					lRet = RegSetValueExA(hResult,FULL_DATA_INTERVAL,NULL,REG_BINARY,(BYTE*)&interval,sizeof(interval));
					RegCloseKey( hResult );
				}
				RegCloseKey( hKeyMIC_GM );
			}
			RegCloseKey (hKeyLANL);
		}

		SetEOSSScheduleText();

		//propagate all the new values to the program
		if (!m_bCollectingParametersForNewISO)
		{
			m_pParent->UpdateParams(true);
			if (m_bIntervalChange)
			{
				m_bIntervalChange = false;
				((CEOSSPropertySheet*)m_pPS)->ResetReaderCount(); // set the interval full vs status counter used by the reader process back to 0
			}
		}

	}//end bCarryOn
}

void CEOSSModParam::OnModparamsApply() 
{
	//for all the comm device must exist
	if (m_pPS) 
		m_pPS->SetToClose(0);
	//disable first so user can't click on it multiple times
	m_pApplyButton->EnableWindow(FALSE);
	DoApply();
	m_bChange = false;
}

void CEOSSModParam::OnModparamsReset() 
{
	if (m_pPS) m_pPS->SetToClose(0);

	m_datasetup = true;
	char *name = m_pParent->m_pName;

	GetDlgItem(IDC_EOSSMODPARAMS_PORT)->EnableWindow(m_bCollectingParametersForNewISO?TRUE:FALSE);

	ResetItem(name,SAVE_LOC, SAVELOC_DEF, IDC_EOSSMODPARAMS_SAVELOC);

	char buff[16384];
	char buff2[64];
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,
		sizeof(buff), m_pParent->m_szIniFile)>0)
	{
		char *at = buff;
		int sel = ((CComboBox*)GetDlgItem(IDC_EOSSMODPARAMS_PORT))->GetCurSel();
		((CComboBox*)GetDlgItem(IDC_EOSSMODPARAMS_PORT))->ResetContent();

		m_bAllowFinish = false;

		while (strlen(at) > 0)
		{
			strcpy(buff2,at);
			strtok(buff2,"=");
			((CComboBox*)GetDlgItem(IDC_EOSSMODPARAMS_PORT))->AddString(buff2);
			m_bAllowFinish = true;
			at = at + strlen(at)+1;
		}

		((CComboBox*)GetDlgItem(IDC_EOSSMODPARAMS_PORT))->SetCurSel(sel);
	}

	ResetItem(name,"PORT", PORT_DEF, IDC_EOSSMODPARAMS_PORT);

	ResetItemI(name,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_EOSSMODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,
		IDC_EOSSMODPARAMS_MAXPAUSE);

//	ResetItem(name,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
//		IDC_EOSSMODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItem(name,FILE_ID, FILEID_SPIN_DEF,
		IDC_EOSSMODPARAMS_FILEID_SPIN,
		IDC_EOSSMODPARAMS_FILEID);

	ResetItem(name,"MAIN_BATT_LOW",MBATVOLTL_SPIN_DEF,
		IDC_EOSSMODPARAMS_MAINBATLOW_SPIN,MBATVOLTL_SPIN_MIN,MBATVOLTL_SPIN_MAX);

	ResetItem(name,"MAIN_BATT_CRIT",MBATVOLTC_SPIN_DEF,
		IDC_EOSSMODPARAMS_MAINBATCRIT_SPIN,MBATVOLTC_SPIN_MIN,MBATVOLTC_SPIN_MAX);

	ResetItem(name,"EMER_BATT_LOW",EBATVOLTL_SPIN_DEF,
		IDC_EOSSMODPARAMS_EMERBATLOW_SPIN,EBATVOLTL_SPIN_MIN,EBATVOLTL_SPIN_MAX);

	ResetItem(name,"EMER_BATT_CRIT",EBATVOLTC_SPIN_DEF,
		IDC_EOSSMODPARAMS_EMERBATCRIT_SPIN,EBATVOLTC_SPIN_MIN,EBATVOLTC_SPIN_MAX);
	
	ResetItem(name,READ_LOG,0,IDC_EOSSMODPARAMS_READER_LOG);
	ResetItem(name,USE_FULL_DATA_INTERVAL,0,IDC_EOSSMODPARAMS_PRESENCE_CHECK);
	ResetItem(name,FULL_DATA_INTERVAL,"1",IDC_EOSSMODPARAMS_INTERVAL);
	
	ResetItem(name,"TIME_ERROR",TIMEERROR_SPIN_DEF,
		IDC_EOSSMODPARAMS_TIMEERROR_SPIN,TIMEERROR_SPIN_MIN,TIMEERROR_SPIN_MAX);

	ResetItem(name,"ACCOUNT_USER","UNKNOWN",IDC_EOSSMODPARAMS_ACCOUNT);
	ResetItem(name,"ACCOUNT_PW","UNKNOWN",IDC_EOSSMODPARAMS_PW);

	char szTemp[32];
	//m_cComboWhen.SetCurSel(2);
	GetPrivateProfileString(name,"AUTORUN","Daily",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);

	switch (toupper(szTemp[0])) {
	case 'M':
		m_cComboWhen.SetCurSel(0);	
		break;
	case 'W':
		m_cComboWhen.SetCurSel(1);	
		break;
	case 'D':
		m_cComboWhen.SetCurSel(2);	
		break;
	case 'H':
		m_cComboWhen.SetCurSel(3);	
		break;
	default:
		m_cComboWhen.SetCurSel(2);	
	}
	//m_cCombo_DOM.SetCurSel(0);
	GetPrivateProfileString(name,"DAYOFMONTH","1",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);
	m_cCombo_DOM.SetCurSel(atoi(szTemp)-1);

	//set time offset
	GetPrivateProfileString(name,"TIME_OFFSET","000",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);
	if (m_cCombo_Offset.SelectString(-1,szTemp) == LB_ERR)
		m_cCombo_Offset.SetCurSel(12);

	//m_cCombo_DOW.SetCurSel(0);
	GetPrivateProfileString(name,"DAYOFWEEK","Sunday",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);
	switch (toupper(szTemp[0])) {
	case 'S'://Sunday, Saturday
		m_cCombo_DOW.SetCurSel((toupper(szTemp[1]) == 'U')?0:6);
		break;
	case 'M'://Monday
		m_cCombo_DOW.SetCurSel(1);
		break;
	case 'T'://Tuesday, Thursday
		m_cCombo_DOW.SetCurSel((toupper(szTemp[1]) == 'U')?2:4);
		break;
	case 'W'://Wednesday
		m_cCombo_DOW.SetCurSel(5);
		break;
	default:
		m_cCombo_DOW.SetCurSel(0);
	}

	GetPrivateProfileString(name,"HOURS","01",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);
	m_cCombo_Hours.SetCurSel(atoi(szTemp));

	GetPrivateProfileString(name,"MINUTES","30",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);
	m_cCombo_Minutes.SetCurSel(atoi(szTemp));

	OnSelchangeComboWhen();

	m_bChange = false;
	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}

void CEOSSModParam::OnModparamsDefaultBut()
{
	OnModparamsDefault(false);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnModparamsDefault(bool SetPort)
{
	if (m_pPS) m_pPS->SetToClose(0);
	m_datasetup = true;

	ResetItem(EOSS_DEFAULT, FILE_ID, FILEID_SPIN_DEF,  
		IDC_EOSSMODPARAMS_FILEID_SPIN, IDC_EOSSMODPARAMS_FILEID);

 
	ResetItem(EOSS_DEFAULT,SAVE_LOC,SAVELOC_DEF,IDC_EOSSMODPARAMS_SAVELOC);

	if (SetPort || m_bCollectingParametersForNewISO)
	{

		char buff[4096];
		char buff2[32];
		if (GetPrivateProfileSection(COMMUNICATIONS,buff,
			sizeof(buff), m_pParent->m_szIniFile)>0)
		{
			char *at = buff;
			((CComboBox*)GetDlgItem(IDC_EOSSMODPARAMS_PORT))->ResetContent();
			m_bAllowFinish = false;
			while (strlen(at) > 0)
			{
				strcpy(buff2,at);
				strtok(buff2,"=");
				((CComboBox*)GetDlgItem(IDC_EOSSMODPARAMS_PORT))->AddString(buff2);
				m_bAllowFinish = true;
				at = at + strlen(at)+1;
			}
		}
		((CComboBox*)GetDlgItem(IDC_EOSSMODPARAMS_PORT))->SetCurSel(0);

		ResetItem(EOSS_DEFAULT,"PORT",PORT_DEF,IDC_EOSSMODPARAMS_PORT);
	}

//	ResetItem(EOSS_DEFAULT,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
//		IDC_EOSSMODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItemI(EOSS_DEFAULT,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_EOSSMODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,
		IDC_EOSSMODPARAMS_MAXPAUSE);

	ResetItem(EOSS_DEFAULT,"MAIN_BATT_LOW",MBATVOLTL_SPIN_DEF,
		IDC_EOSSMODPARAMS_MAINBATLOW_SPIN,MBATVOLTL_SPIN_MIN,MBATVOLTL_SPIN_MAX);

	ResetItem(EOSS_DEFAULT,"MAIN_BATT_CRIT",MBATVOLTC_SPIN_DEF,
		IDC_EOSSMODPARAMS_MAINBATCRIT_SPIN,MBATVOLTC_SPIN_MIN,MBATVOLTC_SPIN_MAX);

	ResetItem(EOSS_DEFAULT,"EMER_BATT_LOW",EBATVOLTL_SPIN_DEF,
		IDC_EOSSMODPARAMS_EMERBATLOW_SPIN,EBATVOLTL_SPIN_MIN,EBATVOLTL_SPIN_MAX);

	ResetItem(EOSS_DEFAULT,"EMER_BATT_CRIT",EBATVOLTC_SPIN_DEF,
		IDC_EOSSMODPARAMS_EMERBATCRIT_SPIN,EBATVOLTC_SPIN_MIN,EBATVOLTC_SPIN_MAX);

	ResetItem(EOSS_DEFAULT,READ_LOG,0,IDC_EOSSMODPARAMS_READER_LOG);
	ResetItem(EOSS_DEFAULT,USE_FULL_DATA_INTERVAL,0,IDC_EOSSMODPARAMS_PRESENCE_CHECK);
	ResetItem(EOSS_DEFAULT,FULL_DATA_INTERVAL,"1",IDC_EOSSMODPARAMS_INTERVAL);

	ResetItem(EOSS_DEFAULT,"TIME_ERROR",TIMEERROR_SPIN_DEF,
		IDC_EOSSMODPARAMS_TIMEERROR_SPIN,TIMEERROR_SPIN_MIN,TIMEERROR_SPIN_MAX);

	ResetItem(EOSS_DEFAULT,"ACCOUNT_USER","UNKNOWN",IDC_EOSSMODPARAMS_ACCOUNT);
	ResetItem(EOSS_DEFAULT,"ACCOUNT_PW","UNKNOWN",IDC_EOSSMODPARAMS_PW);
	
	char szTemp[32];
	//m_cComboWhen.SetCurSel(2);
	GetPrivateProfileString(EOSS_DEFAULT,"AUTORUN","Daily",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);

	switch (toupper(szTemp[0])) {
	case 'M':
		m_cComboWhen.SetCurSel(0);	
		break;
	case 'W':
		m_cComboWhen.SetCurSel(1);	
		break;
	case 'D':
		m_cComboWhen.SetCurSel(2);	
		break;
	case 'H':
		m_cComboWhen.SetCurSel(3);	
		break;
	default:
		m_cComboWhen.SetCurSel(2);	
	}
	//m_cCombo_DOM.SetCurSel(0);
	GetPrivateProfileString(EOSS_DEFAULT,"DAYOFMONTH","1",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);
	m_cCombo_DOM.SetCurSel(atoi(szTemp)-1);

	//m_cCombo_DOW.SetCurSel(0);
	GetPrivateProfileString(EOSS_DEFAULT,"DAYOFWEEK","Sunday",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);
	switch (toupper(szTemp[0])) {
	case 'S'://Sunday, Saturday
		m_cCombo_DOW.SetCurSel((toupper(szTemp[1]) == 'U')?0:6);
		break;
	case 'M'://Monday
		m_cCombo_DOW.SetCurSel(1);
		break;
	case 'T'://Tuesday, Thursday
		m_cCombo_DOW.SetCurSel((toupper(szTemp[1]) == 'U')?2:4);
		break;
	case 'W'://Wednesday
		m_cCombo_DOW.SetCurSel(5);
		break;
	default:
		m_cCombo_DOW.SetCurSel(0);
	}

	ResetItem(EOSS_DEFAULT,"HOURS","01",IDC_EOSSMODPARAMS_HOURS);
	ResetItem(EOSS_DEFAULT,"MINUTES","00",IDC_EOSSMODPARAMS_MINUTES);
	ResetItem(EOSS_DEFAULT,"TIME_OFFSET","000",IDC_EOSSMODPARAMS_OFFSETHOURS);

	OnSelchangeComboWhen();

}

BOOL CEOSSModParam::OnSetActive() 
{
	if (m_pPS) 
		m_pPS->SetToClose(0);

	if (m_bCollectingParametersForNewISO) 
	{
		SetDlgItemText(IDC_EOSSMODPARAMS_STATICTEXT,"");
		m_pApplyButton->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EOSSMODPARAMS_RESET)->ShowWindow(SW_HIDE);

        if (!m_datasetup)
		{
			OnModparamsDefault();

			if (m_pPS)
			{
				CString sCaption(m_pParent->m_pName);
				TC_ITEM tcItem;
				tcItem.mask = TCIF_TEXT;
				tcItem.pszText = (LPTSTR)((LPCTSTR)sCaption);
				m_pPS->GetTabControl()->SetItem(0, &tcItem );
			}
		}
		if (m_bAllowFinish)
			m_pPS->SetWizardButtons(PSWIZB_FINISH );
		else
			m_pPS->SetWizardButtons(PSWIZB_DISABLEDFINISH );

	}
	else
	{
		SetDlgItemText(IDC_EOSSMODPARAMS_STATICTEXT,"Changes will NOT take effect until you click on \"Apply\".");
//		GetDlgItem(IDC_EOSSMODPARAMS_DATETIME)->ShowWindow(SW_SHOW);

		if (m_pApplyButton)
			m_pApplyButton->EnableWindow(m_bChange?TRUE:FALSE);
	}
	return CPropertyPage::OnSetActive();
}

void CEOSSModParam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

BOOL CEOSSModParam::OnWizardFinish() 
{
//	ASSERT(m_bCollectingParametersForNewISO);

	//Put the name of the instrument in the ini file
	WritePrivateProfileString(
		INSTRUMENTS,m_pParent->m_pName,EOSS_DISCOVERY_TYPE,m_pParent->m_szIniFile);
	//Put the parameters for the instrument into the INI file
	DoApply();  //This does the validate

	return CPropertyPage::OnWizardFinish();
}

void CEOSSModParam::WriteIniFileDefaultSection()
{
//	TRACE("CEOSSModParam::WriteIniFileDefaultSection()\n");
	char szTemp[16];
	char *inifile = m_pParent->m_szIniFile;

	WritePrivateProfileSection(EOSS_DEFAULT,"",inifile);

	WritePrivateProfileString(EOSS_DEFAULT,"PORT","",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"FLAGCOMM","0",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"FLAGOTHR","0",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"FLAGTIME","0",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"MAXPAUSE","600",inifile);
	WritePrivateProfileString(EOSS_DEFAULT, SAVE_LOC,"C:\\DATA\\EOSS01",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"MAXINQUIRE","10",inifile);
	WritePrivateProfileString(EOSS_DEFAULT, FILE_ID,"01",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"ACCOUNT_USER","UNKNOWN",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"ACCOUNT_PW","UNKNOWN",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"AUTORUN","Daily",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"DAYOFMONTH","1",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"DAYOFWEEK","Sunday",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"HOURS","01",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"MINUTES","00",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,READ_LOG,"0",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"TIME_OFFSET","000",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"TIME_ERROR","60",inifile);

	WritePrivateProfileString(EOSS_DEFAULT,"MAIN_BATT_LOW",_itoa(MBATVOLTL_SPIN_DEF,szTemp,10),inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"MAIN_BATT_CRIT",_itoa(MBATVOLTC_SPIN_DEF,szTemp,10),inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"EMER_BATT_LOW",_itoa(EBATVOLTL_SPIN_DEF,szTemp,10),inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"EMER_BATT_CRIT",_itoa(EBATVOLTC_SPIN_DEF,szTemp,10),inifile);

	WritePrivateProfileString(EOSS_DEFAULT,"HARDWARE_COMM_FAILURE_MINUTES","120",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,"HARDWARE_COMM_FAILURE_READ_TRIES","2",inifile);
	
	WritePrivateProfileString(EOSS_DEFAULT,USE_FULL_DATA_INTERVAL,"0",inifile);
	WritePrivateProfileString(EOSS_DEFAULT,FULL_DATA_INTERVAL,"1",inifile);

}

//reset float item for a spinner with a non-automatic buddy
void CEOSSModParam::ResetItem(char *szName,char *szItem,char*szDefault,int dItem,int dMin, int dMax, int dTextItem)
{
	char temp[32];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	float ftemp = (float)atof(temp);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos((int)(ftemp*10.0));
	sprintf(temp,"%1.1f",ftemp);
	GetDlgItem(dTextItem)->SetWindowText(temp);
}

//reset int item for a spinner with a non-automatic buddy
void CEOSSModParam::ResetItemI(char *szName,char *szItem,char*szDefault,
						   int dItem,int /*dMin*/, int dMax, int dTextItem)
{
	int pAccel [] =
	{
		0,1,
		2,10,
		4,100,
		6,1000,
		8,10000,
	};

	char temp[32];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(0,32000);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(0);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetAccel(5, (struct _UDACCEL *)pAccel );
	GetDlgItem(dTextItem)->SetWindowText(temp);
	_itoa(dMax,temp,10);
	((CEdit*)GetDlgItem(dTextItem))->SetLimitText(strlen(temp));
}

//reset item for a spinner with an automatic buddy
void CEOSSModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem,int dMin, int dMax)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(dtemp);
		char temp[32];
		_itoa(dMax,temp,10);
		((CEdit*)((CSpinButtonCtrl*)GetDlgItem(dItem))->GetBuddy())->SetLimitText(strlen(temp));
}

//reset item for a check box
void CEOSSModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
	((CButton*)GetDlgItem(dItem))->SetCheck(dtemp);
}

//reset item for a simple edit field
void CEOSSModParam::ResetItem(char *szName, char *szItem, char *szDefault, int dItem)
{
	char temp[MAX_PATH+1];
//	int nSelect = CB_ERR;
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	switch(dItem)
	{
	case IDC_EOSSMODPARAMS_PORT:
		((CComboBox*)GetDlgItem(dItem))->SelectString(-1, temp);
		break;
	case IDC_EOSSMODPARAMS_OFFSETHOURS:
		((CComboBox*)GetDlgItem(dItem))->SelectString(-1, temp);
		break;
	default:;
	}
	GetDlgItem(dItem)->SetWindowText(temp);
}


void CEOSSModParam::SetPropertySheet(CPropertySheet *pPS, char *pName)
{
	m_pPS = (CTPropertySheet*)pPS;
}

BOOL CEOSSModParam::OnInitDialog() 
{

	CPropertyPage::OnInitDialog();

	m_pApplyButton = new CYellowButton(
		"Apply",
		IDC_EOSSMODPARAMS_APPLY, 
		m_pParent->m_szIniFile,
		this);

	CString *csversion = ((CEOSSPropertySheet*)m_pPS)->GetVersionString();
	CString cs = "Version " + m_pParent->m_csVersion + " Build Date:" + __DATE__;
	SetDlgItemText(IDC_STATIC_ADVERT, cs);

	SetEOSSEstimateListTime();

	SetEOSSScheduleText();

	return TRUE;
}

void CEOSSModParam::SetEOSSScheduleText()
{
	int interval = GetPrivateProfileInt(m_pParent->m_pName,FULL_DATA_INTERVAL,1,m_pParent->m_szIniFile);
	int bUseInterval = GetPrivateProfileInt(m_pParent->m_pName,USE_FULL_DATA_INTERVAL,0,m_pParent->m_szIniFile);

	CString a;
	if (bUseInterval && (interval > 1))
	{
		char ending[5];
		strcpy(ending, "th");
		int endrem = (interval % 10);
		switch  (endrem)
		{
		case 1:
			if (interval != 11)
				strcpy(ending, "st");
			break;
		case 2:
			if (interval != 12)
				strcpy(ending, "nd");
			break;
		case 3:
			if (interval != 13)
				strcpy(ending, "rd");
			break;
		default:
			break;
		}
		a.Format("MIC scheduled to automatically collect status %d of every %d runs and all data every %d%s run.", interval - 1, interval, interval, ending); 
	}
	else
		a.Format("MIC scheduled to automatically collect all data on every run."); 
	
	SetDlgItemText(IDC_EOSSMODPARAMS_GATHER_SCHED, a);
}


void CEOSSModParam::SetEOSSEstimateListTime()
{
	char szEOSSes[512];
	int iItems = 0;
	DWORD chars = GetPrivateProfileString(m_pParent->m_pName,"EOSSes","",szEOSSes,sizeof(szEOSSes),m_pParent->m_szIniFile);
	if (chars > 1)
	{
		char* szAt;
		szAt = strtok(szEOSSes,",");
		while (szAt)
		{
			iItems++;
			szAt = strtok(NULL,",");
		}
	}

	int interval = GetPrivateProfileInt(m_pParent->m_pName,FULL_DATA_INTERVAL,1,m_pParent->m_szIniFile);
	int bUseInterval = GetPrivateProfileInt(m_pParent->m_pName,USE_FULL_DATA_INTERVAL,0,m_pParent->m_szIniFile);

	float full = iItems*60.f + 10.f; // 1 minute per seal average
	float status = iItems*4.f + 10.f; //4 seconds per seal average; 10 second startup time

	CString a;
	//a.Format("%.2f minutes (%.0f sec) estimated time for all data of %d seals",full/60,full,iItems);
	//SetDlgItemText(IDC_EOSSMODPARAMS_FULL_ESTIMATE, a);

	a.Format(
		"Collection time estimates for %d seals,\nStatus: %.1f minutes (%.0f sec); All Data - %.1f minutes (%.0f sec).",
		iItems, status/60,status,full/60,full);
	SetDlgItemText(IDC_EOSSMODPARAMS_PRESENCE_ESTIMATE, a);
}

void CEOSSModParam::ResetItem(char *szName, char *szItem, char *szDefault,
						  int dItemSpin, int dItem)
{
	char temp[3];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	if (strlen(temp)==1)
	{
		temp[1] = temp[0];
		temp[0] = '0';
		temp[2] = NULL;
	}
	SetDlgItemText(dItem,temp);
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(dItemSpin))->SetRange(FILEID_SPIN_MIN,FILEID_SPIN_MAX);
	((CSpinButtonCtrl*)GetDlgItem(dItemSpin))->SetPos(value);
	((CEdit*)GetDlgItem(dItem))->SetLimitText(2);
}

void CEOSSModParam::DoSpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{
	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	char temp[16];								
	GetDlgItem(DLGITEM)->GetWindowText(temp,sizeof(temp)-1);

	int iNew = atoi(temp) + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
	}
	
	sprintf(temp,"%d",iNew);
	GetDlgItem(DLGITEM)->SetWindowText(temp);
	*pResult = 0;
}

void CEOSSModParam::DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
								int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{
	if (m_pPS)m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	char temp[16];								
	GetDlgItemText(DLGITEM,temp,sizeof(temp)-1);

	int iNew = atoi(temp) + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
	}
	
	sprintf(temp,"%d",iNew);
	SetDlgItemText(DLGITEM,temp);

	char temp2[16];
	int buddyvalue;
	GetDlgItemText(DLGITEMBUDDY,temp2,sizeof(temp2));
//try{
	sscanf(temp2,"%d",&buddyvalue);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 10",MB_OK);
//	AfxMessageBox(temp2,MB_OK);
//}
	if (Above)
	{
		if (iNew > buddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY, temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}
	else
	{
		if (iNew < buddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY,temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}

	*pResult = 0;
}
void CEOSSModParam::DoSpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{

	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iNew = pNMUpDown->iPos + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
		pNMUpDown->iDelta = MAX-pNMUpDown->iPos;
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
		pNMUpDown->iDelta = MIN-pNMUpDown->iPos;
	}
	
	char temp[8];								
	sprintf(temp,"%1.1f",(float)iNew/10.0);
	GetDlgItem(DLGITEM)->SetWindowText(temp);
	*pResult = 0;
}

void CEOSSModParam::DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
							int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{

	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iNew = pNMUpDown->iPos + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
		pNMUpDown->iDelta = MAX-pNMUpDown->iPos;
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
		pNMUpDown->iDelta = MIN-pNMUpDown->iPos;
	}
	
	char temp[8];
	char temp2[8];
	float fvalue = (float)iNew/(float)10.0;
	sprintf(temp,"%1.1f",(float)iNew/10.0);
	GetDlgItem(DLGITEM)->SetWindowText(temp);

	float fbuddyvalue;
	GetDlgItemText(DLGITEMBUDDY,temp2,sizeof(temp2));
//try{
		sscanf(temp2,"%f",&fbuddyvalue);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 11",MB_OK);
//	AfxMessageBox(temp2,MB_OK);
//}
	if (Above)
	{
		if (fvalue > fbuddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY, temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}
	else
	{
		if (fvalue < fbuddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY,temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}

	*pResult = 0;
}

void CEOSSModParam::DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{

	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iNew = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (iNew > MAX)
	{
		iNew = MAX;
		pNMUpDown->iDelta = 0;
		pNMUpDown->iPos = MAX;
	}
	if (iNew < MIN)
	{
		iNew = MIN;
		pNMUpDown->iDelta = 0;
		pNMUpDown->iPos = MIN;
	}
	char temp[3];

	if (iNew > 99)
	{
		int iTemp = iNew - 100;
		int i1 = iTemp/36;
		int i2 = iTemp % 36;
		temp[0] = (char)('A' + i1);
		if (i2 < 10)
			temp[1] = (char)('0' + i2);
		else
			temp[1] = (char)('A' + i2-10);
	}
	else
		sprintf(temp,"%02d",iNew);

	temp[2] = NULL;

	SetDlgItemText(DLGITEM,temp);

	CString Text;
	GetDlgItemText(IDC_EOSSMODPARAMS_SAVELOC,Text);

//	if "EOSS" at end of name
	Text.MakeReverse();
	switch (Text.Find("SSOE"))
	{
	case 0:
		Text.MakeReverse();
		break;
	case 1:	
		if (
			((Text[0] >= '0') && (Text[0] <= '9')) ||
			((Text[0] >= 'A') && (Text[0] <= 'Z')) ||
			((Text[0] >= 'a') && (Text[0] <= 'z')) 
		   )
		{
			Text.MakeReverse();
			Text = Text.Left(Text.GetLength()-1);
		}
		else
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "EOSS";
			else
				Text += "\\EOSS";
		}
		break;
	case 2:
		if (
			(
			((Text[0] >= '0') && (Text[0] <= '9')) ||
			((Text[0] >= 'A') && (Text[0] <= 'Z')) ||
			((Text[0] >= 'a') && (Text[0] <= 'z')) 
			) && (
			((Text[1] >= '0') && (Text[1] <= '9')) ||
			((Text[1] >= 'A') && (Text[1] <= 'Z')) ||
			((Text[1] >= 'a') && (Text[1] <= 'z')) 
			)
		   )
		{
			Text.MakeReverse();
			Text = Text.Left(Text.GetLength()-2);
		}
		else
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "EOSS";
			else
				Text += "\\EOSS";
		}
		break;
	default:
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "EOSS";
			else
				Text += "\\EOSS";
		}
	}

	*pResult = 0;
	Text += temp;
	SetDlgItemText(IDC_EOSSMODPARAMS_SAVELOC,Text);
}

void CEOSSModParam::OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MAXPAUSE_SPIN_MIN, MAXPAUSE_SPIN_MAX, IDC_EOSSMODPARAMS_MAXPAUSE);
}

void CEOSSModParam::OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinID(pNMHDR, pResult, FILEID_SPIN_MIN, FILEID_SPIN_MAX, IDC_EOSSMODPARAMS_FILEID);
}

void CEOSSModParam::OnModparamsBrowse() 
{
	if (m_pPS) m_pPS->SetToClose(0);

	CDirDialog DirDialog(this);
	char temp[_MAX_DIR+1];
	GetDlgItemText(IDC_EOSSMODPARAMS_SAVELOC,temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Directory to save files in.";
	if (DirDialog.DoBrowse())
	{
		CString Id;
		GetDlgItemText(IDC_EOSSMODPARAMS_FILEID,Id);
		Id.TrimLeft();
		CString Text(DirDialog.m_strPath);
		CString Name("EOSS"+Id);
		if (Text.Find(Name,0)==-1)
		{
			if (Text.Right(1) == '\\')
				Text += "EOSS";
			else
				Text += "\\EOSS";
		}
		Text += Id;
		SetDlgItemText(IDC_EOSSMODPARAMS_SAVELOC, Text);
	}
}

void CEOSSModParam::OnDeltaposModparamsMostSpin(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	*pResult = 0;
}
/*
void CEOSSModParam::OnChangeModparamsMaxInq()
{
//	int value =	GetDlgItemInt(IDC_EOSSMODPARAMS_MAXINQUIRE);
	if (value < MAXINQUIRE_SPIN_MIN)
		SetDlgItemInt(IDC_EOSSMODPARAMS_MAXINQUIRE, MAXINQUIRE_SPIN_MIN);
	if (value > MAXINQUIRE_SPIN_MAX)
		SetDlgItemInt(IDC_EOSSMODPARAMS_MAXINQUIRE, MAXINQUIRE_SPIN_MAX);

	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}
*/
/*
void CEOSSModParam::OnChangeModparamsMaxCycle()
{
	int value =	GetDlgItemInt(IDC_EOSSMODPARAMS_MAXCYCLE);
//	if (value < MAXCYCLE_SPIN_MIN)
//		SetDlgItemInt(IDC_EOSSMODPARAMS_MAXCYCLE, MAXCYCLE_SPIN_MIN);
//	if (value > MAXCYCLE_SPIN_MAX)
//		SetDlgItemInt(IDC_EOSSMODPARAMS_MAXCYCLE, MAXCYCLE_SPIN_MAX);

	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}
*/

void CEOSSModParam::OnChangeModparamsMost()
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnChangeModparamsPort()
{
	if(m_pPS)m_pPS->SetToClose(0);
}

void CEOSSModParam::OnChangeModparamsFileID()
{
	if (m_pPS) m_pPS->SetToClose(0);
	CString cString;
	m_EOSSModParamsFileID.GetWindowText(cString);
	bool bDoSave = false;
	//force upper
	for (int i = 0; i < cString.GetLength(); i++)
	{
		if (islower(cString[i]))
			bDoSave = true;	
		if (!isalnum(cString[i]))
		{
			cString.SetAt(i,'0');
			bDoSave = true;
		}
	}

	if (bDoSave)
	{
		cString.MakeUpper();
		m_EOSSModParamsFileID.SetWindowText(cString);
	}

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnChangeModparamsSaveloc() 
{
	
	CString Text;
	GetDlgItemText(IDC_EOSSMODPARAMS_SAVELOC,Text);
	Text.TrimRight();
	Text.TrimRight("\\");
	Text.TrimLeft();
	if (!Text.IsEmpty())
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_EOSSMODPARAMS_SAVELOC))->GetSel(posStart,posEnd);
		if (Text.FindOneOf(INVALIDCHARSSUB) != -1)
		{
			((CEdit*)GetDlgItem(IDC_EOSSMODPARAMS_SAVELOC))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_EOSSMODPARAMS_SAVELOC))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_EOSSMODPARAMS_SAVELOC))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_EOSSMODPARAMS_SAVELOC))->SetSel(posStart,posEnd);
	}
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnKillfocusModparamsFileid() 
{
	char temp[3];
	GetDlgItemText(IDC_EOSSMODPARAMS_FILEID,temp,sizeof(temp));
	while (strlen(temp) < 2)
	{
		temp[2] = temp [1];
		temp[1] = temp [0];
		temp[0] = '0';
	}
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(IDC_EOSSMODPARAMS_FILEID_SPIN))->SetPos(value);
	((CEdit*)GetDlgItem(IDC_EOSSMODPARAMS_FILEID))->SetWindowText(temp);
}


/*
void CEOSSModParam::OnModparamsDatetime() 
{
	if (m_pPS)
	{
		m_pPS->SetToClose(0);
		m_pPS->ForceDateTime();
	}
		
}

void CEOSSModParam::OnCheckAllowautodate() 
{
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnCheckBinary()
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}
*/

void CEOSSModParam::OnEditchangeComboWhen() 
{
	// TODO: Add your control notification handler code here
}

void CEOSSModParam::OnSelchangeComboWhen() 
{
	int nIndex = m_cComboWhen.GetCurSel();
	if (nIndex != LB_ERR)
	{
		switch (nIndex) {
		case 0://Monthly
			GetDlgItem(IDC_EOSSMODPARAMS_DOM)->EnableWindow(TRUE);
			GetDlgItem(IDC_EOSSMODPARAMS_DOW)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_HOURS)->EnableWindow(TRUE);
			GetDlgItem(IDC_EOSSMODPARAMS_MINUTES)->EnableWindow(TRUE);
			break;
		case 1://Weekly
			GetDlgItem(IDC_EOSSMODPARAMS_DOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_DOW)->EnableWindow(TRUE);
			GetDlgItem(IDC_EOSSMODPARAMS_HOURS)->EnableWindow(TRUE);
			GetDlgItem(IDC_EOSSMODPARAMS_MINUTES)->EnableWindow(TRUE);
			break;
		case 2://Daily
			GetDlgItem(IDC_EOSSMODPARAMS_DOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_DOW)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_HOURS)->EnableWindow(TRUE);
			GetDlgItem(IDC_EOSSMODPARAMS_MINUTES)->EnableWindow(TRUE);
			break;
		case 3://Hourly
			GetDlgItem(IDC_EOSSMODPARAMS_DOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_DOW)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_HOURS)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_MINUTES)->EnableWindow(TRUE);
			break;
		default://Never
			GetDlgItem(IDC_EOSSMODPARAMS_DOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_DOW)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_HOURS)->EnableWindow(FALSE);
			GetDlgItem(IDC_EOSSMODPARAMS_MINUTES)->EnableWindow(FALSE);
			break;
		}
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnChangeEossmodparamsAccount() 
{
	m_bChange = true;
	m_pApplyButton->EnableWindow(TRUE);
}

void CEOSSModParam::OnChangeEossmodparamsPw() 
{
	m_bChange = true;
	m_pApplyButton->EnableWindow(TRUE);
	
}

void CEOSSModParam::OnSelchangeEossmodparamsDom() 
{
	m_bChange = true;
	m_pApplyButton->EnableWindow(TRUE);
	
}

void CEOSSModParam::OnSelchangeEossmodparamsHours() 
{
	m_bChange = true;
	m_pApplyButton->EnableWindow(TRUE);
	
}

void CEOSSModParam::OnSelchangeEossmodparamsMinutes() 
{
	m_bChange = true;
	m_pApplyButton->EnableWindow(TRUE);
	
}

void CEOSSModParam::OnChangeEossmodparamsMainbatlow() 
{
	if (m_pApplyButton)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	
}

void CEOSSModParam::OnChangeEossmodparamsMainbatcrit() 
{
	if (m_pApplyButton)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	
}

void CEOSSModParam::OnChangeEossmodparamsEmerbatlow() 
{

	if (m_pApplyButton)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnChangeEossmodparamsEmerbatcrit() 
{
	if (m_pApplyButton)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}


void CEOSSModParam::OnDeltaposEossmodparamsMainbatlowSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MBATVOLTL_SPIN_MIN, MBATVOLTL_SPIN_MAX, IDC_EOSSMODPARAMS_MAINBATLOW);
	
	*pResult = 0;
}

void CEOSSModParam::OnDeltaposEossmodparamsMainbatcritSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MBATVOLTC_SPIN_MIN, MBATVOLTC_SPIN_MAX, IDC_EOSSMODPARAMS_MAINBATCRIT);
	
	*pResult = 0;
}

void CEOSSModParam::OnDeltaposEossmodparamsEmerbatlowSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, EBATVOLTL_SPIN_MIN, EBATVOLTL_SPIN_MAX, IDC_EOSSMODPARAMS_EMERBATLOW);
	
	*pResult = 0;
}

void CEOSSModParam::OnDeltaposEossmodparamsEmerbatcritSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, EBATVOLTC_SPIN_MIN, EBATVOLTC_SPIN_MAX, IDC_EOSSMODPARAMS_EMERBATCRIT);
	
	*pResult = 0;
}

void CEOSSModParam::OnDeltaposEossmodparamsTimeerrorSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, TIMEERROR_SPIN_MIN, TIMEERROR_SPIN_MAX, IDC_EOSSMODPARAMS_TIMEERROR);
	
	*pResult = 0;
}

void CEOSSModParam::OnChangeEossmodparamsTimeerror() 
{
	
	if (m_pPS) m_pPS->SetToClose(0);
	if (m_pApplyButton)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnEditchangeEossmodparamsOffsethours() 
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (m_pApplyButton)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnBnClickedEossmodparamsReaderLog()
{
	if (m_pApplyButton)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}
void CEOSSModParam::OnBnClickedEossmodparamsFullIntervalCheck()
{
	if (m_pApplyButton)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}
void CEOSSModParam::OnChangeEossmodparamsInterval() 
{
	if (m_pApplyButton)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEOSSModParam::OnKillfocusEossmodparamsInterval() 
{
	BOOL transgood;
	int interval = GetDlgItemInt(IDC_EOSSMODPARAMS_INTERVAL, &transgood, FALSE);
	int curinterval = GetPrivateProfileInt(m_pParent->m_pName,FULL_DATA_INTERVAL,1,m_pParent->m_szIniFile);
	if (interval != curinterval)
	{
		if (m_pApplyButton)
		{
			m_bIntervalChange = true;
			m_bChange = true;
			m_pApplyButton->EnableWindow(TRUE);
		}
	}
	else
		m_bIntervalChange = false;
}
