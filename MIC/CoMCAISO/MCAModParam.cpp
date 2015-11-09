/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
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
// MCAModParam.cpp

#include "stdafx.h"
#include "invalid_chars.h"
#include "INI_definitions.h"
#include "ISO_definitions.h"
#include "resource.h"
#include "colors.h"
#include "MCAModParam.h"
#include "DirDialog.h"
#include "MCAConfig2.h"
#include "MCAInst.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIME_ERR_SPIN_MIN	0
#define TIME_ERR_SPIN_MAX	10000
#define TIME_ERR_SPIN_DEF	60

#define SLOW_RESET_SPIN_MIN	0
#define SLOW_RESET_SPIN_MAX	10000
#define SLOW_RESET_SPIN_DEF	60

#define BATVOLTH_SPIN_DEF	"50.0"
#define BATVOLTH_SPIN_MAX	500
#define BATVOLTH_SPIN_MIN	0
#define BATVOLTL_SPIN_MAX	500
#define BATVOLTL_SPIN_MIN	 0
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
#define NODE_SPIN_DEF		1

#define MAXCYCLE_SPIN_MIN	100
#define MAXCYCLE_SPIN_MAX	32767
#define MAXCYCLE_SPIN_DEF	1000

#define MAXPAUSE_SPIN_MIN	0
#define MAXPAUSE_SPIN_MAX	99999
#define MAXPAUSE_SPIN_DEF	"60"

#define MAXINQUIRE_SPIN_MIN 1
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

#define SPECTRATYPE_DEF		0

#define FILEID_SPIN_MAX	99
#define FILEID_SPIN_MIN	0
#define FILEID_SPIN_DEF	"01"

#define SAVELOC_DEF			"C:\\DATA\\MCA01"
#define PORT_DEF			"ERROR"
/////////////////////////////////////////////////////////////////////////////
// CMCAModParam property page

IMPLEMENT_DYNCREATE(CMCAModParam, CPropertyPage)

CMCAModParam::CMCAModParam() : CPropertyPage(CMCAModParam::IDD)
{
}


CMCAModParam::CMCAModParam(
	UINT id, 
	CISO* pParent, 
	bool DoingConfig) 
	: CPropertyPage(id)
{
	m_bAllowFinish = false;
	m_pParent = pParent;
	m_bChange = false;
	m_datasetup = false;
	m_bCollectingParametersForNewISO = DoingConfig;
	m_pPS = NULL;
	m_pApplyButton = NULL;

	for (int i= IDC_MODPARAMS_FIRST; i <= IDC_MODPARAMS_LAST; i++)
	{
		StaticColors[i-IDC_MODPARAMS_FIRST] = COLOR_DEFAULT_TEXT;
	}

	if (m_bCollectingParametersForNewISO)
	{
		//See if default section of INI file is already there...
		char testbuf[16];
		int numchars = GetPrivateProfileSection(
			MCA_DEFAULT,
			testbuf,
			sizeof(testbuf),
			m_pParent->m_szIniFile);

		//If not, then go write it
		if (numchars == 0)
			WriteIniFileDefaultSection();
	}


	//{{AFX_DATA_INIT(CMCAModParam)
	//}}AFX_DATA_INIT
}

CMCAModParam::~CMCAModParam()
{
	delete m_pApplyButton;
}

BEGIN_MESSAGE_MAP(CMCAModParam, CPropertyPage)
	//{{AFX_MSG_MAP(CMCAModParam)
	ON_BN_CLICKED(IDC_MODPARAMS_APPLY, OnModparamsApply)
	ON_BN_CLICKED(IDC_MODPARAMS_BROWSE, OnModparamsBrowse)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_BATVOLTL_SPIN, OnDeltaposModparamsBatvoltlSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_BATVOLTH_SPIN, OnDeltaposModparamsBatvolthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_M15H_SPIN, OnDeltaposModparamsSupM15hSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_M15L_SPIN, OnDeltaposModparamsSupM15lSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_P05H_SPIN, OnDeltaposModparamsSupP05hSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_P05L_SPIN, OnDeltaposModparamsSupP05lSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_P15H_SPIN, OnDeltaposModparamsSupP15hSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_P15L_SPIN, OnDeltaposModparamsSupP15lSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXBBM_SPIN, OnDeltaposModparamsMaxbbmSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXPAUSE_SPIN, OnDeltaposModparamsMaxpauseSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_FILEID_SPIN,   OnDeltaposModparamsFileidSpin)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_NODE_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_MODPARAMS_FILEID, OnChangeModparamsMost)
	ON_CBN_SELCHANGE(IDC_MODPARAMS_PORT, OnChangeModparamsPort)
	ON_EN_CHANGE(IDC_MODPARAMS_SAVELOC, OnChangeModparamsSaveloc)
	ON_EN_KILLFOCUS(IDC_MODPARAMS_FILEID, OnKillfocusModparamsFileid)
	ON_EN_CHANGE(IDC_MODPARAMS_SAVELOC, OnChangeModparamsFileID)
	ON_BN_CLICKED(IDC_MODPARAMS_DATETIME, OnModparamsDatetime)
	ON_BN_CLICKED(IDC_CHECK_ALLOWAUTODATE, OnCheckAllowautodate)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXSTATUS_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXINQUIRE_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXCYCLE_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_MODPARAMS_LOGMII, OnChangeModparamsMost)
	ON_BN_CLICKED(IDC_MODPARAMS_LOGFILTER, OnChangeModparamsMost)
	ON_BN_CLICKED(IDC_MODPARAMS_DODUMP, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_COMMFAIL_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_MODPARAMS_DEFAULT, OnModparamsDefaultBut)
	ON_BN_CLICKED(IDC_MODPARAMS_RESET, OnModparamsReset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_TIMEOUT_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_MODPARAMS_TIMEOUT, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_TIME_ERR_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SLOW_RESET_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_MODPARAMS_TIME_ERR, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SLOW_RESET, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_P15L, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_P15H, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_P05L, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_P05H, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_M15L, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_M15H, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_NODE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXSTATUS, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXPAUSE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXINQUIRE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXCYCLE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXBBM, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_COMMFAIL, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_BATVOLTL, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_BATVOLTH, OnChangeModparamsMost)
	ON_CBN_SELCHANGE(IDC_MODPARAMS_SPECTRAFILE, OnSelchangeComboSpectrafile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CMCAModParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCAModParam)
	//}}AFX_DATA_MAP
}

void CMCAModParam::SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox)
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

void CMCAModParam::DoApply()
{
	char *Name = m_pParent->m_pName;
	char Temp[_MAX_PATH];

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
		SaveItem(Name,"BATVOLTH",	IDC_MODPARAMS_BATVOLTH);
		SaveItem(Name,"BATVOLTL",	IDC_MODPARAMS_BATVOLTL);
		SaveItem(Name,"SUP_P15H",	IDC_MODPARAMS_SUP_P15H);
		SaveItem(Name,"SUP_P15L",	IDC_MODPARAMS_SUP_P15L);
		SaveItem(Name,"SUP_M15H",	IDC_MODPARAMS_SUP_M15H);
		SaveItem(Name,"SUP_M15L",	IDC_MODPARAMS_SUP_M15L);	
		SaveItem(Name,"SUP_P05H",	IDC_MODPARAMS_SUP_P05H);
		SaveItem(Name,"SUP_P05L",	IDC_MODPARAMS_SUP_P05L);
		SaveItem(Name,"TIME_ERR",	IDC_MODPARAMS_TIME_ERR);
		SaveItem(Name,"SLOW_RESET",	IDC_MODPARAMS_SLOW_RESET);
		SaveItem(Name,"NODE",		IDC_MODPARAMS_NODE);
		SaveItem(Name,"MAXCYCLE",	IDC_MODPARAMS_MAXCYCLE);
		SaveItem(Name,"MAXPAUSE",	IDC_MODPARAMS_MAXPAUSE);
		SaveItem(Name,"DO_DUMP",	IDC_MODPARAMS_DODUMP,	true);
		SaveItem(Name,"AUTOTIMESET",IDC_CHECK_ALLOWAUTODATE,	true);
		SaveItem(Name,"LOG_FILTER",	IDC_MODPARAMS_LOGFILTER, true);
		SaveItem(Name,"LOG_MII",	IDC_MODPARAMS_LOGMII,	true);
		SaveItem(Name,"PORT",		IDC_MODPARAMS_PORT);
		SaveItem(Name,"MAXINQUIRE",	IDC_MODPARAMS_MAXINQUIRE);
		SaveItem(Name,"MAXSTATUS",	IDC_MODPARAMS_MAXSTATUS);
		SaveItem(Name,"COMMFAIL",	IDC_MODPARAMS_COMMFAIL);
		SaveItem(Name,"TIMEOUT",	IDC_MODPARAMS_TIMEOUT);
		SaveItem(Name,"MAXBBM",		IDC_MODPARAMS_MAXBBM);
		SaveItem(Name,FILE_ID,	IDC_MODPARAMS_FILEID);

		m_iSpectrumFileType = ((CComboBox*)GetDlgItem(IDC_MODPARAMS_SPECTRAFILE))->GetCurSel();
		WritePrivateProfileString(
			Name,"SPECTRAFILETYPE",itoa(m_iSpectrumFileType,Temp,10),
			m_pParent->m_szIniFile);

		GetDlgItemText(IDC_MODPARAMS_SAVELOC,Temp,sizeof(Temp));
			
		while ((Temp[strlen(Temp)-1]=='\\')||
			   (Temp[strlen(Temp)-1]=='/' )  ) 
		{
			Temp[strlen(Temp)-1] = NULL;
			SetDlgItemText(IDC_MODPARAMS_SAVELOC,Temp);
		}
		SaveItem(Name,SAVE_LOC,	IDC_MODPARAMS_SAVELOC);

		if (access(Temp,0)==-1)
		{
			_mkdir(Temp);
			strcat(Temp,"\\archive");
			_mkdir(Temp);
		}

		//propagate all the new values to the program
		if (!m_bCollectingParametersForNewISO)
			m_pParent->UpdateParams(true);

	}//end bCarryOn
}

void CMCAModParam::OnModparamsApply() 
{
	if(m_pPS)
		m_pPS->SetToClose(0);

	DoApply();
	m_bChange = false;
	m_pApplyButton->EnableWindow(FALSE);
}

void CMCAModParam::SetPropertySheet(CPropertySheet *pPS, char *pName)
{
	m_pPS = (CTPropertySheet*)pPS;
}

void CMCAModParam::OnModparamsReset() 
{
	if(m_pPS)m_pPS->SetToClose(0);

	m_datasetup = true;
	char *name = m_pParent->m_pName;

	if (m_bCollectingParametersForNewISO)
	{
		GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_MODPARAMS_DATETIME)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_MODPARAMS_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_MODPARAMS_NODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_MODPARAMS_DATETIME)->ShowWindow(SW_SHOW);
	}

	ResetItem(name,FILE_ID, FILEID_SPIN_DEF,
		IDC_MODPARAMS_FILEID_SPIN,IDC_MODPARAMS_FILEID);

	ResetItem(name, "BATVOLTH", BATVOLTH_SPIN_DEF,  
		IDC_MODPARAMS_BATVOLTH_SPIN, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX,
		IDC_MODPARAMS_BATVOLTH);

	ResetItem(name, "BATVOLTL", BATVOLTL_SPIN_DEF,
		IDC_MODPARAMS_BATVOLTL_SPIN, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX,
		IDC_MODPARAMS_BATVOLTL);

	ResetItem(name,"SUP_P15H",SUP_P15H_SPIN_DEF,
		IDC_MODPARAMS_SUP_P15H_SPIN,SUP_P15H_SPIN_MIN,SUP_P15H_SPIN_MAX,
		IDC_MODPARAMS_SUP_P15H);

	ResetItem(name,"SUP_P15L",SUP_P15L_SPIN_DEF,
		IDC_MODPARAMS_SUP_P15L_SPIN,SUP_P15L_SPIN_MIN,SUP_P15L_SPIN_MAX,
		IDC_MODPARAMS_SUP_P15L);

	ResetItem(name,"SUP_M15H",SUP_M15H_SPIN_DEF,
		IDC_MODPARAMS_SUP_M15H_SPIN,SUP_M15H_SPIN_MIN,SUP_M15H_SPIN_MAX,
		IDC_MODPARAMS_SUP_M15H);

	ResetItem(name,"SUP_M15L",SUP_M15L_SPIN_DEF,
		IDC_MODPARAMS_SUP_M15L_SPIN,SUP_M15L_SPIN_MIN,SUP_M15L_SPIN_MAX,
		IDC_MODPARAMS_SUP_M15L);	

	ResetItem(name,"SUP_P05H",SUP_P05H_SPIN_DEF,
		IDC_MODPARAMS_SUP_P05H_SPIN,SUP_P05H_SPIN_MIN,SUP_P05H_SPIN_MAX,
		IDC_MODPARAMS_SUP_P05H);

	ResetItem(name,"SUP_P05L",SUP_P05L_SPIN_DEF,
		IDC_MODPARAMS_SUP_P05L_SPIN,SUP_P05L_SPIN_MIN,SUP_P05L_SPIN_MAX,
		IDC_MODPARAMS_SUP_P05L);

	ResetItem(name,"TIME_ERR",TIME_ERR_SPIN_DEF,
		IDC_MODPARAMS_TIME_ERR_SPIN,TIME_ERR_SPIN_MIN,TIME_ERR_SPIN_MAX);

	ResetItem(name,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_MODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);

	ResetItem(name,"NODE",NODE_SPIN_DEF,
		IDC_MODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);
	
	ResetItem(name,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_MODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(name,"DO_DUMP", DODUMP_DEF, IDC_MODPARAMS_DODUMP);

	ResetItem(name,"LOG_FILTER", LOGFILTER_DEF, IDC_MODPARAMS_LOGFILTER);

	ResetItem(name,"LOG_MII", LOGMII_DEF, IDC_MODPARAMS_LOGMII);
	ResetItem(name,"AUTOTIMESET", AUTOTIME_DEF, IDC_CHECK_ALLOWAUTODATE);

	ResetItem(name,SAVE_LOC, SAVELOC_DEF, IDC_MODPARAMS_SAVELOC);

	char buff[16384];
	char buff2[64];
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,
		sizeof(buff), m_pParent->m_szIniFile)>0)
	{
		char *at = buff;
		int sel = ((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->GetCurSel();
		((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->ResetContent();

		m_bAllowFinish = false;

		while (strlen(at) > 0)
		{
			strcpy(buff2,at);
			strtok(buff2,"=");
			((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->AddString(buff2);
			m_bAllowFinish = true;
			at = at + strlen(at)+1;
		}
		if (m_pPS)
			((CMCAConfig2*)m_pPS->GetPage(1))->m_bAllowFinish = m_bAllowFinish;

		((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->SetCurSel(sel);
	}

	ResetItem(name,"PORT", PORT_DEF, IDC_MODPARAMS_PORT);

	ResetItem(name,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
		IDC_MODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItem(name,"MAXSTATUS",MAXSTATUS_SPIN_DEF,
		IDC_MODPARAMS_MAXSTATUS_SPIN,MAXSTATUS_SPIN_MIN,MAXSTATUS_SPIN_MAX);

	ResetItem(name,"COMMFAIL",COMMFAIL_SPIN_DEF,
		IDC_MODPARAMS_COMMFAIL_SPIN,COMMFAIL_SPIN_MIN,COMMFAIL_SPIN_MAX);

	ResetItem(name,"TIMEOUT",TIMEOUT_SPIN_DEF,
		IDC_MODPARAMS_TIMEOUT_SPIN,TIMEOUT_SPIN_MIN,TIMEOUT_SPIN_MAX);

	ResetItemI(name,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_MODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,IDC_MODPARAMS_MAXPAUSE);

	ResetItemI(name,"MAXBBM",MAXBBM_SPIN_DEF,
		IDC_MODPARAMS_MAXBBM_SPIN,MAXBBM_SPIN_MIN,MAXBBM_SPIN_MAX,IDC_MODPARAMS_MAXBBM);

	m_iSpectrumFileType = 
		GetPrivateProfileInt(name,"SPECTRAFILETYPE",SPECTRATYPE_DEF,m_pParent->m_szIniFile);

	((CComboBox*)GetDlgItem(IDC_MODPARAMS_SPECTRAFILE))->SetCurSel(m_iSpectrumFileType);
	
	m_bChange = false;

	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}

void CMCAModParam::OnModparamsDefaultBut()
{
	OnModparamsDefault(false);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CMCAModParam::OnModparamsDefault(bool SetPort) 
{
	if(m_pPS)m_pPS->SetToClose(0);
	m_datasetup = true;

	ResetItem(MCA_DEFAULT, FILE_ID, FILEID_SPIN_DEF,  
		IDC_MODPARAMS_FILEID_SPIN, IDC_MODPARAMS_FILEID);

	ResetItem(MCA_DEFAULT, "BATVOLTH", BATVOLTH_SPIN_DEF,  
		IDC_MODPARAMS_BATVOLTH_SPIN, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX,
		IDC_MODPARAMS_BATVOLTH);

	ResetItem(MCA_DEFAULT, "BATVOLTL", BATVOLTL_SPIN_DEF,
		IDC_MODPARAMS_BATVOLTL_SPIN, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX,
		IDC_MODPARAMS_BATVOLTL);

	ResetItem(MCA_DEFAULT,"SUP_P15H",SUP_P15H_SPIN_DEF,
		IDC_MODPARAMS_SUP_P15H_SPIN,SUP_P15H_SPIN_MIN,SUP_P15H_SPIN_MAX,
		IDC_MODPARAMS_SUP_P15H);

	ResetItem(MCA_DEFAULT,"SUP_P15L",SUP_P15L_SPIN_DEF,
		IDC_MODPARAMS_SUP_P15L_SPIN,SUP_P15L_SPIN_MIN,SUP_P15L_SPIN_MAX,
		IDC_MODPARAMS_SUP_P15L);

	ResetItem(MCA_DEFAULT,"SUP_M15H",SUP_M15H_SPIN_DEF,
		IDC_MODPARAMS_SUP_M15H_SPIN,SUP_M15H_SPIN_MIN,SUP_M15H_SPIN_MAX,
		IDC_MODPARAMS_SUP_M15H);

	ResetItem(MCA_DEFAULT,"SUP_M15L",SUP_M15L_SPIN_DEF,
		IDC_MODPARAMS_SUP_M15L_SPIN,SUP_M15L_SPIN_MIN,SUP_M15L_SPIN_MAX,
		IDC_MODPARAMS_SUP_M15L);	

	ResetItem(MCA_DEFAULT,"SUP_P05H",SUP_P05H_SPIN_DEF,
		IDC_MODPARAMS_SUP_P05H_SPIN,SUP_P05H_SPIN_MIN,SUP_P05H_SPIN_MAX,
		IDC_MODPARAMS_SUP_P05H);

	ResetItem(MCA_DEFAULT,"SUP_P05L",SUP_P05L_SPIN_DEF,
		IDC_MODPARAMS_SUP_P05L_SPIN,SUP_P05L_SPIN_MIN,SUP_P05L_SPIN_MAX,
		IDC_MODPARAMS_SUP_P05L);

	ResetItem(MCA_DEFAULT,"TIME_ERR",TIME_ERR_SPIN_DEF,
		IDC_MODPARAMS_TIME_ERR_SPIN,TIME_ERR_SPIN_MIN,TIME_ERR_SPIN_MAX);

	ResetItem(MCA_DEFAULT,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_MODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);
	
	ResetItem(MCA_DEFAULT,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_MODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(MCA_DEFAULT,"DO_DUMP",DODUMP_DEF,IDC_MODPARAMS_DODUMP);

	ResetItem(MCA_DEFAULT,"LOG_FILTER",LOGFILTER_DEF,IDC_MODPARAMS_LOGFILTER);

	ResetItem(MCA_DEFAULT,"LOG_MII",LOGMII_DEF,IDC_MODPARAMS_LOGMII);
	ResetItem(MCA_DEFAULT,"AUTOTIMESET",AUTOTIME_DEF,IDC_CHECK_ALLOWAUTODATE);

	ResetItem(MCA_DEFAULT,SAVE_LOC,SAVELOC_DEF,IDC_MODPARAMS_SAVELOC);

	if (SetPort || m_bCollectingParametersForNewISO)
	{
		ResetItem(MCA_DEFAULT,"NODE",NODE_SPIN_DEF,
			IDC_MODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);

		char buff[16384];
		char buff2[32];
		if (GetPrivateProfileSection(COMMUNICATIONS,buff,
			sizeof(buff), m_pParent->m_szIniFile)>0)
		{
			char *at = buff;
			((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->ResetContent();
			m_bAllowFinish = false;
			while (strlen(at) > 0)
			{
				strcpy(buff2,at);
				strtok(buff2,"=");
				((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->AddString(buff2);
				m_bAllowFinish = true;
				at = at + strlen(at)+1;
			}
			if (m_pPS)
				((CMCAConfig2*)m_pPS->GetPage(1))->m_bAllowFinish = m_bAllowFinish;
		}
		((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->SetCurSel(0);
	
		ResetItem(MCA_DEFAULT,"PORT",PORT_DEF,IDC_MODPARAMS_PORT);
	}

	ResetItem(MCA_DEFAULT,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
		IDC_MODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItem(MCA_DEFAULT,"MAXSTATUS",MAXSTATUS_SPIN_DEF,
		IDC_MODPARAMS_MAXSTATUS_SPIN,MAXSTATUS_SPIN_MIN,MAXSTATUS_SPIN_MAX);

	ResetItem(MCA_DEFAULT,"COMMFAIL",COMMFAIL_SPIN_DEF,
		IDC_MODPARAMS_COMMFAIL_SPIN,COMMFAIL_SPIN_MIN,COMMFAIL_SPIN_MAX);

	ResetItem(MCA_DEFAULT,"TIMEOUT",TIMEOUT_SPIN_DEF,
		IDC_MODPARAMS_TIMEOUT_SPIN,TIMEOUT_SPIN_MIN,TIMEOUT_SPIN_MAX);

	ResetItemI(MCA_DEFAULT,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_MODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,IDC_MODPARAMS_MAXPAUSE);

	ResetItemI(MCA_DEFAULT,"MAXBBM",MAXBBM_SPIN_DEF,
		IDC_MODPARAMS_MAXBBM_SPIN,MAXBBM_SPIN_MIN,MAXBBM_SPIN_MAX,IDC_MODPARAMS_MAXBBM);

	m_iSpectrumFileType = 
		GetPrivateProfileInt(
		MCA_DEFAULT,"SPECTRAFILETYPE",SPECTRATYPE_DEF,m_pParent->m_szIniFile);

	((CComboBox*)GetDlgItem(IDC_MODPARAMS_SPECTRAFILE))->SetCurSel(m_iSpectrumFileType);
}

BOOL CMCAModParam::OnSetActive() 
{
	if (m_pPS)
		m_pPS->SetToClose(0);

	if (m_bCollectingParametersForNewISO)
	{
		m_iSpectrumFileType = 
			GetPrivateProfileInt(
			m_pParent->m_pName,"SPECTRAFILETYPE",SPECTRATYPE_DEF,
			m_pParent->m_szIniFile);
		((CComboBox*)GetDlgItem(IDC_MODPARAMS_SPECTRAFILE))->SetCurSel(m_iSpectrumFileType);

		SetDlgItemText(IDC_MODPARAMS_STATICTEXT," ");

		if (m_pApplyButton)
			m_pApplyButton->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_MODPARAMS_RESET)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_MODPARAMS_DATETIME)->ShowWindow(SW_HIDE);
		m_pPS->GetDlgItem(ID_WIZNEXT)->ShowWindow(SW_SHOW);
		m_pPS->GetDlgItem(ID_WIZNEXT)->EnableWindow(TRUE);
		m_pPS->GetDlgItem(ID_WIZBACK)->ShowWindow(SW_SHOW);
		m_pPS->GetDlgItem(ID_WIZBACK)->EnableWindow(FALSE);
		m_pPS->GetDlgItem(ID_WIZFINISH)->ShowWindow(SW_SHOW);
		m_pPS->GetDlgItem(ID_WIZFINISH)->EnableWindow(FALSE);

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

	}
	else
	{
		SetDlgItemText(IDC_MODPARAMS_STATICTEXT,"Changes will NOT take effect until you click on \"Apply\".");
		GetDlgItem(IDC_MODPARAMS_DATETIME)->ShowWindow(SW_SHOW);

		if (m_pApplyButton)
			m_pApplyButton->EnableWindow(m_bChange?TRUE:FALSE);
	}
	return CPropertyPage::OnSetActive();
}

void CMCAModParam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_pPS)m_pPS->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

BOOL CMCAModParam::OnWizardFinish() 
{
//	ASSERT(m_bCollectingParametersForNewISO);

	//Put the name of the instrument in the ini file
	WritePrivateProfileString(
		INSTRUMENTS,m_pParent->m_pName,MCA_DISCOVERY_TYPE,m_pParent->m_szIniFile);

	DoApply();
	((CMCAConfig2*)m_pPS->GetPage(1))->DoApply();

	return CPropertyPage::OnWizardFinish();
}

BOOL CMCAModParam::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_pApplyButton = new CYellowButton(
		"Apply",
		IDC_MODPARAMS_APPLY, 
		m_pParent->m_szIniFile,
		this);
	
	return TRUE;
}


void CMCAModParam::WriteIniFileDefaultSection()
{
	char *inifile = m_pParent->m_szIniFile;

	WritePrivateProfileSection(MCA_DEFAULT,"",inifile);

	WritePrivateProfileString(MCA_DEFAULT,"BATVOLTH","30.0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"BATVOLTL","10.0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"SUP_P15H","20.0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"SUP_P15L","10.0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"SUP_M15H","-10.0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"SUP_M15L","-20.0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"SUP_P05H","6.0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"SUP_P05L","4.0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"TIME_ERR","60",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"SLOW_RESET","900",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"NODE","-1",inifile);
	WritePrivateProfileString(MCA_DEFAULT,PORT,"",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"FLAGCOMM","0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"FLAGOTHR","0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"FLAGTIME","0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"MAXCYCLE","1000",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"MAXPAUSE","600",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"DO_DUMP","0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"LOG_FILTER","0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"CHAN1","U235",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"CHAN2","Cs137",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"CHAN3","U238",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"CHAN4","Gross Counts",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"CHAN5","Scaler",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"CHAN6","U235 / U238 Ratio",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"CHAN7","Cs137 / U238 Ratio",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"ALARM1","U235",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"ALARM2","Cs137",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"ALARM3","U238",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"ALARM4","Grs Cnts",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"ALARM5","Scaler",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"ALARM6","Ratio 1",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"ALARM7","Ratio 2",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"LOG_MII","0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,SAVE_LOC,"C:\\DATA\\MCA01",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"MAXINQUIRE","10",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"MAXSTATUS","0",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"COMMFAIL","5",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"TIMEOUT","5",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"MAXBBM","1000",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"AUTOTIMESET","1",inifile);
	WritePrivateProfileString(MCA_DEFAULT,FILE_ID,"01",inifile);
	WritePrivateProfileString(MCA_DEFAULT,"SPECTRAFILETYPE","0",inifile);
}

//reset float item for a spinner with a non-automatic buddy
void CMCAModParam::ResetItem(char *szName,char *szItem,char*szDefault,int dItem,int dMin, int dMax, int dTextItem)
{
	char temp[32];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	float ftemp = (float)atof(temp);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos((int)(ftemp*10.0));
	sprintf(temp,"%1.1f",ftemp);
	SetDlgItemText(dTextItem,temp);
}

//reset int item for a spinner with a non-automatic buddy
void CMCAModParam::ResetItemI(char *szName,char *szItem,char*szDefault,
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
	itoa(dMax,temp,10);
	((CEdit*)GetDlgItem(dTextItem))->SetLimitText(strlen(temp));
}

//reset item for a spinner with an automatic buddy
void CMCAModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem,int dMin, int dMax)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(dtemp);
	char temp[32];
	itoa(dMax,temp,10);
	((CEdit*)((CSpinButtonCtrl*)GetDlgItem(dItem))->GetBuddy())->SetLimitText(strlen(temp));
}

//reset item for a check box
void CMCAModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
	((CButton*)GetDlgItem(dItem))->SetCheck(dtemp);
}

//reset item for a simple edit field
void CMCAModParam::ResetItem(char *szName,char *szItem, char *szDefault, int dItem)
{
	char temp[MAX_PATH+1];
//	int nSelect = CB_ERR;
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	switch(dItem)
	{
	case IDC_MODPARAMS_PORT:
		((CComboBox*)GetDlgItem(dItem))->SelectString(-1, temp);
		break;
	default:;
	}
	GetDlgItem(dItem)->SetWindowText(temp);
}


void CMCAModParam::ResetItem(char *szName, char *szItem, char *szDefault,
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


void CMCAModParam::DoSpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{
	if(m_pPS)m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
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

void CMCAModParam::DoSpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{

	if(m_pPS)m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
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

void CMCAModParam::DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
							int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{

	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
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
//	AfxMessageBox("Error at 111",MB_OK);
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

void CMCAModParam::DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{
	if(m_pPS)m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
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

	itoa(iNew,temp,10);
	while (strlen(temp) < 2)
	{
		temp[2] = NULL;
		temp[1] = temp[0];
		temp[0] = '0';
	}

	SetDlgItemText(DLGITEM,temp);

	CString Text;
	GetDlgItemText(IDC_MODPARAMS_SAVELOC,Text);

//	if "MCA" at end then...
	Text.MakeReverse();
	switch (Text.Find("ACM"))
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
				Text += "MCA";
			else
				Text += "\\MCA";
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
				Text += "MCA";
			else
				Text += "\\MCA";
		}
		break;
	default:
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "MCA";
			else
				Text += "\\MCA";
		}
	}

	*pResult = 0;
	Text += temp;
	SetDlgItemText(IDC_MODPARAMS_SAVELOC,Text);
}

void CMCAModParam::OnDeltaposModparamsBatvolthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX, 
		IDC_MODPARAMS_BATVOLTH, IDC_MODPARAMS_BATVOLTL,IDC_MODPARAMS_BATVOLTL_SPIN, false);
}

void CMCAModParam::OnDeltaposModparamsBatvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX, 
		IDC_MODPARAMS_BATVOLTL, IDC_MODPARAMS_BATVOLTH,IDC_MODPARAMS_BATVOLTH_SPIN, true);
}

void CMCAModParam::OnDeltaposModparamsSupM15hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_M15H_SPIN_MIN, SUP_M15H_SPIN_MAX, 
		IDC_MODPARAMS_SUP_M15H, IDC_MODPARAMS_SUP_M15L, IDC_MODPARAMS_SUP_M15L_SPIN, false);
}

void CMCAModParam::OnDeltaposModparamsSupM15lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_M15L_SPIN_MIN, SUP_M15L_SPIN_MAX, 
		IDC_MODPARAMS_SUP_M15L, IDC_MODPARAMS_SUP_M15H, IDC_MODPARAMS_SUP_M15H_SPIN,true);
}

void CMCAModParam::OnDeltaposModparamsSupP05hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P05H_SPIN_MIN, SUP_P05H_SPIN_MAX, 
		IDC_MODPARAMS_SUP_P05H, IDC_MODPARAMS_SUP_P05L, IDC_MODPARAMS_SUP_P05L_SPIN, false);
}

void CMCAModParam::OnDeltaposModparamsSupP05lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P05L_SPIN_MIN, SUP_P05L_SPIN_MAX, 
		IDC_MODPARAMS_SUP_P05L, IDC_MODPARAMS_SUP_P05H, IDC_MODPARAMS_SUP_P05H_SPIN, true );
}

void CMCAModParam::OnDeltaposModparamsSupP15hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P15H_SPIN_MIN, SUP_P15H_SPIN_MAX, 
		IDC_MODPARAMS_SUP_P15H, IDC_MODPARAMS_SUP_P15L, IDC_MODPARAMS_SUP_P15L_SPIN, false);
}

void CMCAModParam::OnDeltaposModparamsSupP15lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P15L_SPIN_MIN, SUP_P15L_SPIN_MAX, 
		IDC_MODPARAMS_SUP_P15L, IDC_MODPARAMS_SUP_P15H, IDC_MODPARAMS_SUP_P15H_SPIN, true);
}

void CMCAModParam::OnDeltaposModparamsMaxbbmSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_pPS)m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MAXBBM_SPIN_MIN, MAXBBM_SPIN_MAX, IDC_MODPARAMS_MAXBBM);
}

void CMCAModParam::OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_pPS)m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MAXPAUSE_SPIN_MIN, MAXPAUSE_SPIN_MAX, IDC_MODPARAMS_MAXPAUSE);
}

void CMCAModParam::OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_pPS)m_pPS->SetToClose(0);
	DoSpinID(pNMHDR, pResult, FILEID_SPIN_MIN, FILEID_SPIN_MAX, IDC_MODPARAMS_FILEID);
}

void CMCAModParam::OnModparamsBrowse() 
{
	if(m_pPS)m_pPS->SetToClose(0);

	CDirDialog DirDialog(this);
	char temp[_MAX_DIR+1];
	GetDlgItem(IDC_MODPARAMS_SAVELOC)->GetWindowText(temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Directory to save files in.";
	if (DirDialog.DoBrowse())
	{
		CString Id;
		GetDlgItemText(IDC_MODPARAMS_FILEID,Id);
		Id.TrimLeft();
		CString Text(DirDialog.m_strPath);
		CString Name("MCA"+Id);
		if (Text.Find(Name,0)==-1)
		{
			if (Text.Right(1) == '\\')
				Text += "MCA";
			else
				Text += "\\MCA";
			Text += Id;
		}
		SetDlgItemText(IDC_MODPARAMS_SAVELOC, Text);
	}
}


void CMCAModParam::OnDeltaposModparamsMostSpin(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
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

void CMCAModParam::OnChangeModparamsMost()
{
	if(m_pPS)m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CMCAModParam::OnChangeModparamsPort()
{
//	if (m_bDoingConfig)
//	{
//		SetDlgItemText(IDC_MODPARAMS_STATICTEXT,"");
//		m_pPS->SetWizardButtons(PSWIZB_NEXT);
//		m_bNextOk = true;
//	}

	if(m_pPS)m_pPS->SetToClose(0);
}

void CMCAModParam::OnChangeModparamsFileID()
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CMCAModParam::OnChangeModparamsSaveloc() 
{
	CString Text;
	GetDlgItemText(IDC_MODPARAMS_SAVELOC,Text);
	if (!Text.IsEmpty())
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->GetSel(posStart,posEnd);
		if (Text.FindOneOf(INVALIDCHARSSUB) != -1)
		{
			((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->SetSel(posStart,posEnd);
	}

	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CMCAModParam::OnKillfocusModparamsFileid() 
{
	// TODO: Add your control notification handler code here
	char temp[3];
	GetDlgItemText(IDC_MODPARAMS_FILEID,temp,sizeof(temp));
	while (strlen(temp) < 2)
	{
		temp[2] = temp [1];
		temp[1] = temp [0];
		temp[0] = '0';
	}
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(IDC_MODPARAMS_FILEID_SPIN))->SetPos(value);
	((CEdit*)GetDlgItem(IDC_MODPARAMS_FILEID))->SetWindowText(temp);
}



void CMCAModParam::OnModparamsDatetime() 
{
	if (m_pPS)
	{
		m_pPS->SetToClose(0);
		m_pPS->ForceDateTime();
	}
		
}

void CMCAModParam::OnCheckAllowautodate() 
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}	
}



void CMCAModParam::OnSelchangeComboSpectrafile() 
{
	// TODO: Add your control notification handler code here
	if (!m_bCollectingParametersForNewISO)
	{
		int iSpectrumFileType = ((CComboBox*)GetDlgItem(IDC_MODPARAMS_SPECTRAFILE))->GetCurSel();
		if (iSpectrumFileType != m_iSpectrumFileType)
		{
			m_bChange = true;

			if (m_pApplyButton)
				m_pApplyButton->EnableWindow(TRUE);
		}
	}
}
