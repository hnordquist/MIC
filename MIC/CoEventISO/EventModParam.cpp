// EventModParam.cpp : implementation file

#include "stdafx.h"
#include "invalid_chars.h"
#include "INI_definitions.h"
#include "ISO_definitions.h"
#include "resource.h"
#include "colors.h"
#include "EventModParam.h"
#include "DirDialog.h"
#include "EventInstrument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EVENT_DEFAULT		"Event Default"
#define TIME_ERR_SPIN_MIN	0
#define TIME_ERR_SPIN_MAX	10000
#define TIME_ERR_SPIN_DEF	60

#define SLOW_RESET_SPIN_MIN	0
#define SLOW_RESET_SPIN_MAX	10000
#define SLOW_RESET_SPIN_DEF	900

#define BATVOLTH_SPIN_DEF	"50.0"
#define BATVOLTH_SPIN_MAX	500
#define BATVOLTH_SPIN_MIN	0
#define BATVOLTL_SPIN_MAX	500
#define BATVOLTL_SPIN_MIN	0
#define BATVOLTL_SPIN_DEF	"0.0"

#define SUP_P15H_SPIN_DEF	"24.0"
#define SUP_P15H_SPIN_MAX	240
#define SUP_P15H_SPIN_MIN	0
#define SUP_P15L_SPIN_MAX	240
#define SUP_P15L_SPIN_MIN	0
#define SUP_P15L_SPIN_DEF	"0.0"

#define SUP_M15H_SPIN_DEF	"0.0"
#define SUP_M15H_SPIN_MAX	0
#define SUP_M15H_SPIN_MIN	-240
#define SUP_M15L_SPIN_MAX	0
#define SUP_M15L_SPIN_MIN	-240
#define SUP_M15L_SPIN_DEF	"-24.0"

#define SUP_P05H_SPIN_DEF   "9.9"
#define SUP_P05H_SPIN_MAX	99
#define SUP_P05H_SPIN_MIN	0
#define SUP_P05L_SPIN_MAX	99
#define SUP_P05L_SPIN_MIN	0
#define SUP_P05L_SPIN_DEF   "0.0"

#define NODE_SPIN_MAX		32767
#define NODE_SPIN_MIN		-1
#define NODE_SPIN_DEF		-1

#define MAXCYCLE_SPIN_MIN	500
#define MAXCYCLE_SPIN_MAX	32767
#define MAXCYCLE_SPIN_DEF	1000

#define MAXPAUSE_SPIN_MIN	0
#define MAXPAUSE_SPIN_MAX	99999
#define MAXPAUSE_SPIN_DEF	"60"

#define MAXINQUIRE_SPIN_MIN 2
#define MAXINQUIRE_SPIN_MAX 32767
#define MAXINQUIRE_SPIN_DEF 10

#define MAXSTATUS_SPIN_MIN	0
#define MAXSTATUS_SPIN_MAX	32767
#define MAXSTATUS_SPIN_DEF	30

#define COMMFAIL_SPIN_MIN	1
#define COMMFAIL_SPIN_MAX	99
#define COMMFAIL_SPIN_DEF	5

#define MAXBBM_SPIN_MIN		0
#define MAXBBM_SPIN_MAX		500000
#define MAXBBM_SPIN_DEF		"1000"

#define TIMEOUT_SPIN_MIN	2	
#define	TIMEOUT_SPIN_MAX	99
#define TIMEOUT_SPIN_DEF	5

#define DODUMP_DEF			1
#define LOGMII_DEF			1
#define LOGFILTER_DEF		1
#define AUTOTIME_DEF		1
#define BINARY_BBM_DEF		0

#define FILEID_SPIN_MAX	1035
#define FILEID_SPIN_MIN	0
#define FILEID_SPIN_DEF	"01"

#define SAVELOC_DEF			"C:\\DATA\\EVENT01"
#define PORT_DEF			"ERROR"
/////////////////////////////////////////////////////////////////////////////
// CModParam property page

IMPLEMENT_DYNCREATE(CEventModParam, CPropertyPage)

CEventModParam::CEventModParam() : CPropertyPage(CEventModParam::IDD)
{
}

CEventModParam::CEventModParam(
	UINT id, 
	CISO* pParent,  
	bool bDoingConfig) 
	: CPropertyPage(id)
{
	m_bAllowFinish = false;
	m_pParent = pParent;
	m_bChange = false;
	m_datasetup = false;
	m_bCollectingParametersForNewISO = bDoingConfig;
	m_pPS = NULL;
	m_pApplyButton = NULL;

	for (int i= IDC_EVENTMODPARAMS_FIRST; i <= IDC_EVENTMODPARAMS_LAST; i++)
	{
		StaticColors[i-IDC_EVENTMODPARAMS_FIRST] = COLOR_DEFAULT_TEXT;
	}

	if (m_bCollectingParametersForNewISO)
	{
		//See if default section of INI file is already there...
		char testbuf[16];
		int numchars = GetPrivateProfileSection(
			EVENT_DEFAULT,
			testbuf,
			sizeof(testbuf),
			m_pParent->m_szIniFile);

		//If not, then go write it
		if (numchars == 0)
			WriteIniFileDefaultSection();
	}
}

void CEventModParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventModParam)
	DDX_Control(pDX, IDC_EVENTMODPARAMS_FILEID_SPIN, m_EventModParamsFileIDSpin);
	DDX_Control(pDX, IDC_EVENTMODPARAMS_FILEID, m_EventModParamsFileID);
	//}}AFX_DATA_MAP
}

CEventModParam::~CEventModParam()
{
	delete m_pApplyButton;
}

BEGIN_MESSAGE_MAP(CEventModParam, CPropertyPage)
	//{{AFX_MSG_MAP(CEventModParam)
	ON_BN_CLICKED(IDC_EVENTMODPARAMS_APPLY, OnModparamsApply)
	ON_BN_CLICKED(IDC_EVENTMODPARAMS_BROWSE, OnModparamsBrowse)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_FILEID_SPIN,   OnDeltaposModparamsFileidSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_MAXBBM_SPIN,   OnDeltaposModparamsMaxbbmSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_MAXPAUSE_SPIN, OnDeltaposModparamsMaxpauseSpin)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_NODE_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_MAXINQUIRE, OnChangeModparamsMaxInq)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_MAXCYCLE, OnChangeModparamsMaxCycle)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_FILEID, OnChangeModparamsFileID)
	ON_CBN_SELCHANGE(IDC_EVENTMODPARAMS_PORT, OnChangeModparamsPort)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_SAVELOC, OnChangeModparamsSaveloc)
	ON_EN_KILLFOCUS(IDC_EVENTMODPARAMS_FILEID, OnKillfocusModparamsFileid)
	ON_BN_CLICKED(IDC_EVENTMODPARAMS_DATETIME, OnModparamsDatetime)
	ON_BN_CLICKED(IDC_CHECK_ALLOWAUTODATE, OnCheckAllowautodate)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_MAXSTATUS_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_MAXINQUIRE_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_MAXCYCLE_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_EVENTMODPARAMS_DODUMP, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_COMMFAIL_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_EVENTMODPARAMS_DEFAULT, OnModparamsDefaultBut)
	ON_BN_CLICKED(IDC_EVENTMODPARAMS_RESET, OnModparamsReset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_TIMEOUT_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_TIMEOUT, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_TIME_ERR_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTMODPARAMS_SLOW_RESET_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_TIME_ERR, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_SLOW_RESET, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_NODE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_MAXSTATUS, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_MAXPAUSE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_MAXBBM, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_COMMFAIL, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_BATVOLTL, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_EVENTMODPARAMS_BATVOLTH, OnChangeModparamsMost)
	ON_BN_CLICKED(IDC_CHECK_BINARY, OnCheckBinary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEventModParam::SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox)
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

void CEventModParam::DoApply()
{
	char *Name = m_pParent->m_pName;
	char Temp[_MAX_PATH];

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
		SaveItem(Name,"TIME_ERR",	IDC_EVENTMODPARAMS_TIME_ERR);
		SaveItem(Name,"SLOW_RESET",	IDC_EVENTMODPARAMS_SLOW_RESET);
		SaveItem(Name,"NODE",		IDC_EVENTMODPARAMS_NODE);
		SaveItem(Name,"MAXCYCLE",	IDC_EVENTMODPARAMS_MAXCYCLE);
		SaveItem(Name,"MAXPAUSE",	IDC_EVENTMODPARAMS_MAXPAUSE);
		SaveItem(Name,"DO_DUMP",	IDC_EVENTMODPARAMS_DODUMP,	true);
		SaveItem(Name,"AUTOTIMESET",IDC_CHECK_ALLOWAUTODATE,true);
//		SaveItem(Name,"LOG_FILTER",	IDC_EVENTMODPARAMS_LOGFILTER,true);
//		SaveItem(Name,"LOG_MII",	IDC_EVENTMODPARAMS_LOGMII,	true);
		SaveItem(Name,"PORT",		IDC_EVENTMODPARAMS_PORT);
		SaveItem(Name,"MAXINQUIRE",	IDC_EVENTMODPARAMS_MAXINQUIRE);
		SaveItem(Name,"MAXSTATUS",	IDC_EVENTMODPARAMS_MAXSTATUS);
		SaveItem(Name,"COMMFAIL",	IDC_EVENTMODPARAMS_COMMFAIL);
		SaveItem(Name,"TIMEOUT",	IDC_EVENTMODPARAMS_TIMEOUT);
		SaveItem(Name,"MAXBBM",		IDC_EVENTMODPARAMS_MAXBBM);
		SaveItem(Name, FILE_ID,		IDC_EVENTMODPARAMS_FILEID);
//		SaveItem(Name,"BINARY_BBM",	IDC_CHECK_BINARY,true);

		GetDlgItemText(IDC_EVENTMODPARAMS_SAVELOC,Temp,sizeof(Temp));
		while ((Temp[strlen(Temp)-1]=='\\')||
			   (Temp[strlen(Temp)-1]=='/' )  ) 
		{
			Temp[strlen(Temp)-1] = NULL;
			SetDlgItemText(IDC_EVENTMODPARAMS_SAVELOC,Temp);
		}
		SaveItem(Name,SAVE_LOC,	IDC_EVENTMODPARAMS_SAVELOC);
		if (_access(Temp,0)==-1)
		{
			_mkdir(Temp);
			strcat(Temp,"\\archive");
			_mkdir(Temp);
		}

		if (m_bCollectingParametersForNewISO)
		{
			WritePrivateProfileString(Name,"FLAGCOMM","0",m_pParent->m_szIniFile);
			WritePrivateProfileString(Name,"FLAGOTHR","0",m_pParent->m_szIniFile);
			WritePrivateProfileString(Name,"FLAGTIME","0",m_pParent->m_szIniFile);
		}

		//propagate all the new values to the program
		if (!m_bCollectingParametersForNewISO)
			m_pParent->UpdateParams(true);

	}//end bCarryOn
}

void CEventModParam::OnModparamsApply() 
{
	//for all the comm device must exist
	if (m_pPS) 
		m_pPS->SetToClose(0);
	//disable first so user can't click on it multiple times
	m_pApplyButton->EnableWindow(FALSE);
	DoApply();
	m_bChange = false;
}

void CEventModParam::OnModparamsReset() 
{
	if (m_pPS) m_pPS->SetToClose(0);

	m_datasetup = true;
	char *name = m_pParent->m_pName;

	if (m_bCollectingParametersForNewISO)
	{
		GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_EVENTMODPARAMS_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EVENTMODPARAMS_NODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->ShowWindow(SW_SHOW);
	}

	ResetItem(name,FILE_ID, FILEID_SPIN_DEF,
		IDC_EVENTMODPARAMS_FILEID_SPIN,
		IDC_EVENTMODPARAMS_FILEID);

	ResetItem(name,"TIME_ERR",TIME_ERR_SPIN_DEF,
		IDC_EVENTMODPARAMS_TIME_ERR_SPIN,TIME_ERR_SPIN_MIN,TIME_ERR_SPIN_MAX);

	ResetItem(name,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_EVENTMODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);

	ResetItem(name,"NODE",NODE_SPIN_DEF,
		IDC_EVENTMODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);
	
	ResetItem(name,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(name,"DO_DUMP", DODUMP_DEF, IDC_EVENTMODPARAMS_DODUMP);

//	ResetItem(name,"LOG_FILTER", LOGFILTER_DEF, IDC_EVENTMODPARAMS_LOGFILTER);

//	ResetItem(name,"LOG_MII", LOGMII_DEF, IDC_EVENTMODPARAMS_LOGMII);
	ResetItem(name,"AUTOTIMESET", AUTOTIME_DEF, IDC_CHECK_ALLOWAUTODATE);
 
	ResetItem(name,SAVE_LOC, SAVELOC_DEF, IDC_EVENTMODPARAMS_SAVELOC);

//	ResetItem(name,"BINARY_BBM", BINARY_BBM_DEF,IDC_CHECK_BINARY);

	char buff[16384];
	char buff2[64];
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,
		sizeof(buff), m_pParent->m_szIniFile)>0)
	{
		char *at = buff;
		int sel = ((CComboBox*)GetDlgItem(IDC_EVENTMODPARAMS_PORT))->GetCurSel();
		((CComboBox*)GetDlgItem(IDC_EVENTMODPARAMS_PORT))->ResetContent();

		m_bAllowFinish = false;

		while (strlen(at) > 0)
		{
			strcpy(buff2,at);
			strtok(buff2,"=");
			((CComboBox*)GetDlgItem(IDC_EVENTMODPARAMS_PORT))->AddString(buff2);
			m_bAllowFinish = true;
			at = at + strlen(at)+1;
		}

		((CComboBox*)GetDlgItem(IDC_EVENTMODPARAMS_PORT))->SetCurSel(sel);
	}

	ResetItem(name,"PORT", PORT_DEF, IDC_EVENTMODPARAMS_PORT);

	ResetItem(name,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItem(name,"MAXSTATUS",MAXSTATUS_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXSTATUS_SPIN,MAXSTATUS_SPIN_MIN,MAXSTATUS_SPIN_MAX);

	ResetItem(name,"COMMFAIL",COMMFAIL_SPIN_DEF,
		IDC_EVENTMODPARAMS_COMMFAIL_SPIN,COMMFAIL_SPIN_MIN,COMMFAIL_SPIN_MAX);

	ResetItem(name,"TIMEOUT",TIMEOUT_SPIN_DEF,
		IDC_EVENTMODPARAMS_TIMEOUT_SPIN,TIMEOUT_SPIN_MIN,TIMEOUT_SPIN_MAX);

	ResetItemI(name,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,
		IDC_EVENTMODPARAMS_MAXPAUSE);

	ResetItemI(name,"MAXBBM",MAXBBM_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXBBM_SPIN,MAXBBM_SPIN_MIN,MAXBBM_SPIN_MAX,
		IDC_EVENTMODPARAMS_MAXBBM);
		
	m_bChange = false;
	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}

void CEventModParam::OnModparamsDefaultBut()
{
	OnModparamsDefault(false);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEventModParam::OnModparamsDefault(bool SetPort)
{
	if (m_pPS) m_pPS->SetToClose(0);
	m_datasetup = true;

	ResetItem(EVENT_DEFAULT, FILE_ID, FILEID_SPIN_DEF,  
		IDC_EVENTMODPARAMS_FILEID_SPIN, IDC_EVENTMODPARAMS_FILEID);

	ResetItem(EVENT_DEFAULT,"TIME_ERR",TIME_ERR_SPIN_DEF,
		IDC_EVENTMODPARAMS_TIME_ERR_SPIN,TIME_ERR_SPIN_MIN,TIME_ERR_SPIN_MAX);

	ResetItem(EVENT_DEFAULT,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_EVENTMODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);
	
	ResetItem(EVENT_DEFAULT,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(EVENT_DEFAULT,"DO_DUMP",DODUMP_DEF,IDC_EVENTMODPARAMS_DODUMP);
 
	ResetItem(EVENT_DEFAULT,SAVE_LOC,SAVELOC_DEF,IDC_EVENTMODPARAMS_SAVELOC);

	if (SetPort || m_bCollectingParametersForNewISO)
	{
		ResetItem(EVENT_DEFAULT,"NODE",NODE_SPIN_DEF,
			IDC_EVENTMODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);

		char buff[4096];
		char buff2[32];
		if (GetPrivateProfileSection(COMMUNICATIONS,buff,
			sizeof(buff), m_pParent->m_szIniFile)>0)
		{
			char *at = buff;
			((CComboBox*)GetDlgItem(IDC_EVENTMODPARAMS_PORT))->ResetContent();
			m_bAllowFinish = false;
			while (strlen(at) > 0)
			{
				strcpy(buff2,at);
				strtok(buff2,"=");
				((CComboBox*)GetDlgItem(IDC_EVENTMODPARAMS_PORT))->AddString(buff2);
				m_bAllowFinish = true;
				at = at + strlen(at)+1;
			}
		}
		((CComboBox*)GetDlgItem(IDC_EVENTMODPARAMS_PORT))->SetCurSel(0);

		ResetItem(EVENT_DEFAULT,"PORT",PORT_DEF,IDC_EVENTMODPARAMS_PORT);
	}

	ResetItem(EVENT_DEFAULT,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItem(EVENT_DEFAULT,"MAXSTATUS",MAXSTATUS_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXSTATUS_SPIN,MAXSTATUS_SPIN_MIN,MAXSTATUS_SPIN_MAX);

	ResetItem(EVENT_DEFAULT,"COMMFAIL",COMMFAIL_SPIN_DEF,
		IDC_EVENTMODPARAMS_COMMFAIL_SPIN,COMMFAIL_SPIN_MIN,COMMFAIL_SPIN_MAX);

	ResetItem(EVENT_DEFAULT,"TIMEOUT",TIMEOUT_SPIN_DEF,
		IDC_EVENTMODPARAMS_TIMEOUT_SPIN,TIMEOUT_SPIN_MIN,TIMEOUT_SPIN_MAX);

	ResetItemI(EVENT_DEFAULT,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,
		IDC_EVENTMODPARAMS_MAXPAUSE);

	ResetItemI(EVENT_DEFAULT,"MAXBBM",MAXBBM_SPIN_DEF,
		IDC_EVENTMODPARAMS_MAXBBM_SPIN,MAXBBM_SPIN_MIN,MAXBBM_SPIN_MAX,
		IDC_EVENTMODPARAMS_MAXBBM);
}

BOOL CEventModParam::OnSetActive() 
{
	if (m_pPS) 
		m_pPS->SetToClose(0);

	if (m_bCollectingParametersForNewISO) 
	{
		SetDlgItemText(IDC_EVENTMODPARAMS_STATICTEXT,"");
		m_pApplyButton->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EVENTMODPARAMS_RESET)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->ShowWindow(SW_HIDE);

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
		SetDlgItemText(IDC_EVENTMODPARAMS_STATICTEXT,"Changes will NOT take effect until you click on \"Apply\".");
		GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->ShowWindow(SW_SHOW);

		if (m_pApplyButton)
			m_pApplyButton->EnableWindow(m_bChange?TRUE:FALSE);
	}
	return CPropertyPage::OnSetActive();
}

void CEventModParam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPS) m_pPS->SetToClose(0);
//	CWnd* pWnd;
//	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
//	{
//		UINT index = pWnd->GetDlgCtrlID();
//		if ((index >= IDC_MODPARAMS_FIRST) && (index <= IDC_MODPARAMS_LAST))
//		{
//			MessageBox("Got One",MB_OK);
//		}
//	}
	CPropertyPage::OnLButtonDown(nFlags, point);
}

BOOL CEventModParam::OnWizardFinish() 
{
//	ASSERT(m_bCollectingParametersForNewISO);

	//Put the name of the instrument in the ini file
	WritePrivateProfileString(
		INSTRUMENTS,m_pParent->m_pName,EVENT_DISCOVERY_TYPE,m_pParent->m_szIniFile);
	//Put the parameters for the instrument into the INI file
	DoApply();  //This does the validate

	return CPropertyPage::OnWizardFinish();
}

void CEventModParam::WriteIniFileDefaultSection()
{
//	TRACE("CEventModParam::WriteIniFileDefaultSection()\n");

	char *inifile = m_pParent->m_szIniFile;

	WritePrivateProfileSection(EVENT_DEFAULT,"",inifile);

	WritePrivateProfileString(EVENT_DEFAULT,"SLOW_RESET","900",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"TIME_ERR","60",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"NODE","-1",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"PORT","",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"FLAGCOMM","0",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"FLAGOTHR","0",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"FLAGTIME","0",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"MAXCYCLE","1000",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"MAXPAUSE","600",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"DO_DUMP","0",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,SAVE_LOC,"C:\\DATA\\EVENT01",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"MAXINQUIRE","10",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"MAXSTATUS","0",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"COMMFAIL","5",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"TIMEOUT","5",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"MAXBBM","1000",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,FILE_ID,"01",inifile);
	WritePrivateProfileString(EVENT_DEFAULT,"AUTOTIMESET","0",inifile);
}

/*
void
CEventModParam::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_EVENTMODPARAMS_FIRST) &&
		(index <= IDC_EVENTMODPARAMS_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

void
CEventModParam::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_EVENTMODPARAMS_FIRST) &&
		(index <= IDC_EVENTMODPARAMS_LAST)) {
	  StaticColors[index-IDC_EVENTMODPARAMS_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}
*/

//reset float item for a spinner with a non-automatic buddy
void CEventModParam::ResetItem(char *szName,char *szItem,char*szDefault,int dItem,int dMin, int dMax, int dTextItem)
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
void CEventModParam::ResetItemI(char *szName,char *szItem,char*szDefault,
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
void CEventModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem,int dMin, int dMax)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(dtemp);
		char temp[32];
		_itoa(dMax,temp,10);
		((CEdit*)((CSpinButtonCtrl*)GetDlgItem(dItem))->GetBuddy())->SetLimitText(strlen(temp));
}

//reset item for a check box
void CEventModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
	((CButton*)GetDlgItem(dItem))->SetCheck(dtemp);
}

//reset item for a simple edit field
void CEventModParam::ResetItem(char *szName, char *szItem, char *szDefault, int dItem)
{
	char temp[MAX_PATH+1];
//	int nSelect = CB_ERR;
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	switch(dItem)
	{
	case IDC_EVENTMODPARAMS_PORT:
		((CComboBox*)GetDlgItem(dItem))->SelectString(-1, temp);
		break;
	default:;
	}
	GetDlgItem(dItem)->SetWindowText(temp);
}


void CEventModParam::SetPropertySheet(CPropertySheet *pPS, char *pName)
{
	m_pPS = (CTPropertySheet*)pPS;
}

BOOL CEventModParam::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_pApplyButton = new CYellowButton(
		"Apply",
		IDC_EVENTMODPARAMS_APPLY, 
		m_pParent->m_szIniFile,
		this);
	
	return TRUE;
}


void CEventModParam::ResetItem(char *szName, char *szItem, char *szDefault,
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



void CEventModParam::DoSpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
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

void CEventModParam::DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
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
void CEventModParam::DoSpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
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

void CEventModParam::DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
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

void CEventModParam::DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
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
	GetDlgItemText(IDC_EVENTMODPARAMS_SAVELOC,Text);

//	if "EVENT" at end of name
	Text.MakeReverse();
	switch (Text.Find("TNEVE"))
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
				Text += "EVENT";
			else
				Text += "\\EVENT";
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
				Text += "EVENT";
			else
				Text += "\\EVENT";
		}
		break;
	default:
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "EVENT";
			else
				Text += "\\EVENT";
		}
	}

	*pResult = 0;
	Text += temp;
	SetDlgItemText(IDC_EVENTMODPARAMS_SAVELOC,Text);
}

void CEventModParam::OnDeltaposModparamsBatvolthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX, 
		IDC_EVENTMODPARAMS_BATVOLTH, IDC_EVENTMODPARAMS_BATVOLTL,IDC_EVENTMODPARAMS_BATVOLTL_SPIN, false);
}

void CEventModParam::OnDeltaposModparamsBatvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX, 
		IDC_EVENTMODPARAMS_BATVOLTL, IDC_EVENTMODPARAMS_BATVOLTH,IDC_EVENTMODPARAMS_BATVOLTH_SPIN, true);
}

void CEventModParam::OnDeltaposModparamsSupM15hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_M15H_SPIN_MIN, SUP_M15H_SPIN_MAX, 
		IDC_EVENTMODPARAMS_SUP_M15H, IDC_EVENTMODPARAMS_SUP_M15L, IDC_EVENTMODPARAMS_SUP_M15L_SPIN, false);
}

void CEventModParam::OnDeltaposModparamsSupM15lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_M15L_SPIN_MIN, SUP_M15L_SPIN_MAX, 
		IDC_EVENTMODPARAMS_SUP_M15L, IDC_EVENTMODPARAMS_SUP_M15H, IDC_EVENTMODPARAMS_SUP_M15H_SPIN,true);
}

void CEventModParam::OnDeltaposModparamsSupP05hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P05H_SPIN_MIN, SUP_P05H_SPIN_MAX, 
		IDC_EVENTMODPARAMS_SUP_P05H, IDC_EVENTMODPARAMS_SUP_P05L, IDC_EVENTMODPARAMS_SUP_P05L_SPIN, false);
}

void CEventModParam::OnDeltaposModparamsSupP05lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P05L_SPIN_MIN, SUP_P05L_SPIN_MAX, 
		IDC_EVENTMODPARAMS_SUP_P05L, IDC_EVENTMODPARAMS_SUP_P05H, IDC_EVENTMODPARAMS_SUP_P05H_SPIN, true );
}

void CEventModParam::OnDeltaposModparamsSupP15hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P15H_SPIN_MIN, SUP_P15H_SPIN_MAX, 
		IDC_EVENTMODPARAMS_SUP_P15H, IDC_EVENTMODPARAMS_SUP_P15L, IDC_EVENTMODPARAMS_SUP_P15L_SPIN, false);
}

void CEventModParam::OnDeltaposModparamsSupP15lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P15L_SPIN_MIN, SUP_P15L_SPIN_MAX, 
		IDC_EVENTMODPARAMS_SUP_P15L, IDC_EVENTMODPARAMS_SUP_P15H, IDC_EVENTMODPARAMS_SUP_P15H_SPIN, true);
}

void CEventModParam::OnDeltaposModparamsMaxbbmSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MAXBBM_SPIN_MIN, MAXBBM_SPIN_MAX, IDC_EVENTMODPARAMS_MAXBBM);
}

void CEventModParam::OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MAXPAUSE_SPIN_MIN, MAXPAUSE_SPIN_MAX, IDC_EVENTMODPARAMS_MAXPAUSE);
}

void CEventModParam::OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinID(pNMHDR, pResult, FILEID_SPIN_MIN, FILEID_SPIN_MAX, IDC_EVENTMODPARAMS_FILEID);
}

void CEventModParam::OnModparamsBrowse() 
{
	if (m_pPS) m_pPS->SetToClose(0);

	CDirDialog DirDialog(this);
	char temp[_MAX_DIR+1];
	GetDlgItemText(IDC_EVENTMODPARAMS_SAVELOC,temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Directory to save files in.";
	if (DirDialog.DoBrowse())
	{
		CString Id;
		GetDlgItemText(IDC_EVENTMODPARAMS_FILEID,Id);
		Id.TrimLeft();
		CString Text(DirDialog.m_strPath);
		CString Name("EVENT"+Id);
		if (Text.Find(Name,0)==-1)
		{
			if (Text.Right(1) == '\\')
				Text += "EVENT";
			else
				Text += "\\EVENT";
		}
		Text += Id;
		SetDlgItemText(IDC_EVENTMODPARAMS_SAVELOC, Text);
	}
}

void CEventModParam::OnDeltaposModparamsMostSpin(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
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

void CEventModParam::OnChangeModparamsMaxInq()
{
	int value =	GetDlgItemInt(IDC_EVENTMODPARAMS_MAXINQUIRE);
	if (value < MAXINQUIRE_SPIN_MIN)
		SetDlgItemInt(IDC_EVENTMODPARAMS_MAXINQUIRE, MAXINQUIRE_SPIN_MIN);
	if (value > MAXINQUIRE_SPIN_MAX)
		SetDlgItemInt(IDC_EVENTMODPARAMS_MAXINQUIRE, MAXINQUIRE_SPIN_MAX);

	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEventModParam::OnChangeModparamsMaxCycle()
{
	int value =	GetDlgItemInt(IDC_EVENTMODPARAMS_MAXCYCLE);
	if (value < MAXCYCLE_SPIN_MIN)
		SetDlgItemInt(IDC_EVENTMODPARAMS_MAXCYCLE, MAXCYCLE_SPIN_MIN);
	if (value > MAXCYCLE_SPIN_MAX)
		SetDlgItemInt(IDC_EVENTMODPARAMS_MAXCYCLE, MAXCYCLE_SPIN_MAX);

	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}


void CEventModParam::OnChangeModparamsMost()
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEventModParam::OnChangeModparamsPort()
{
	if(m_pPS)m_pPS->SetToClose(0);
}

void CEventModParam::OnChangeModparamsFileID()
{
	if (m_pPS) m_pPS->SetToClose(0);
	CString cString;
	m_EventModParamsFileID.GetWindowText(cString);
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
		m_EventModParamsFileID.SetWindowText(cString);
	}

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEventModParam::OnChangeModparamsSaveloc() 
{
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	CString Text;
	GetDlgItemText(IDC_EVENTMODPARAMS_SAVELOC,Text);
	if (!Text.IsEmpty())
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_EVENTMODPARAMS_SAVELOC))->GetSel(posStart,posEnd);
		if (Text.FindOneOf(INVALIDCHARSSUB) != -1)
		{
			((CEdit*)GetDlgItem(IDC_EVENTMODPARAMS_SAVELOC))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_EVENTMODPARAMS_SAVELOC))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_EVENTMODPARAMS_SAVELOC))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_EVENTMODPARAMS_SAVELOC))->SetSel(posStart,posEnd);
	}
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEventModParam::OnKillfocusModparamsFileid() 
{
	char temp[3];
	GetDlgItemText(IDC_EVENTMODPARAMS_FILEID,temp,sizeof(temp));
	while (strlen(temp) < 2)
	{
		temp[2] = temp [1];
		temp[1] = temp [0];
		temp[0] = '0';
	}
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(IDC_EVENTMODPARAMS_FILEID_SPIN))->SetPos(value);
	((CEdit*)GetDlgItem(IDC_EVENTMODPARAMS_FILEID))->SetWindowText(temp);
}



void CEventModParam::OnModparamsDatetime() 
{
	if (m_pPS)
	{
		m_pPS->SetToClose(0);
		m_pPS->ForceDateTime();
	}
		
}

void CEventModParam::OnCheckAllowautodate() 
{
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CEventModParam::OnCheckBinary()
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}
