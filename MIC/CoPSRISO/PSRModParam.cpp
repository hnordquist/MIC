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
// PSRModParam.cpp

#include "stdafx.h"
#include "invalid_chars.h"
#include "INI_definitions.h"
#include "ISO_definitions.h"
#include "resource.h"
#include "colors.h"
#include "PSRModParam.h"
#include "DirDialog.h"
#include "PSRConfig2.h"
#include "PSRInst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SLOW_RESET_SPIN_MIN	0
#define SLOW_RESET_SPIN_MAX	9999
#define SLOW_RESET_SPIN_DEF	900

#define MAX_TIMEOUT_SPIN_MIN	2
#define MAX_TIMEOUT_SPIN_MAX	99
#define MAX_TIMEOUT_SPIN_DEF	20

#define REALSTHRESH_SPIN_MIN 1
#define REALSTHRESH_SPIN_MAX 9999
#define REALSTHRESH_SPIN_DEF 50

#define TOTALSTHRESH_SPIN_MIN 1
#define TOTALSTHRESH_SPIN_MAX 9999
#define TOTALSTHRESH_SPIN_DEF 50

#define TOTALS1THRESH_SPIN_MIN 1
#define TOTALS1THRESH_SPIN_MAX 9999
#define TOTALS1THRESH_SPIN_DEF 50

#define TOTALSLTHRESH_SPIN_MIN 0
#define TOTALSLTHRESH_SPIN_MAX 9999
#define TOTALSLTHRESH_SPIN_DEF 10

#define SIGMATHRESH_SPIN_MIN 1
#define SIGMATHRESH_SPIN_MAX 32000
#define SIGMATHRESH_SPIN_DEF 50

#define GATEWIDTH_SPIN_MIN 1
#define GATEWIDTH_SPIN_MAX 999
#define GATEWIDTH_SPIN_DEF 50

#define HIGHV_SPIN_MIN 500
#define HIGHV_SPIN_MAX 4999
#define HIGHV_SPIN_DEF 1800

#define MAXCOMPRESS_SPIN_MIN 1
#define MAXCOMPRESS_SPIN_MAX 999
#define MAXCOMPRESS_SPIN_DEF 25

#define CT_UNITS_SPIN_MIN 10
#define CT_UNITS_SPIN_MAX 99
#define CT_UNITS_SPIN_DEF "16"

#define CT_EXP_SPIN_MIN 0
#define CT_EXP_SPIN_MAX 9
#define CT_EXP_SPIN_DEF "1"

#define PREDELAY_DEF 1

#define HIGHVOLTH_SPIN_DEF  "4999"
#define HIGHVOLTH_SPIN_MAX	4999
#define HIGHVOLTH_SPIN_MIN	0
#define HIGHVOLTL_SPIN_MAX  4999
#define HIGHVOLTL_SPIN_MIN  0
#define HIGHVOLTL_SPIN_DEF  "0"

#define NODE_SPIN_MAX		32767
#define NODE_SPIN_MIN		-1
#define NODE_SPIN_DEF		-1

#define MAXCYCLE_SPIN_MIN	100
#define MAXCYCLE_SPIN_MAX	2000
#define MAXCYCLE_SPIN_DEF	300

#define MAXPAUSE_SPIN_MIN	0
#define MAXPAUSE_SPIN_MAX	99999
#define MAXPAUSE_SPIN_DEF	"60"

#define DODUMP_DEF			1
#define LOGMII_DEF			1
#define LOGFILTER_DEF		1

#define FILEID_SPIN_MAX	99
#define FILEID_SPIN_MIN	0
#define FILEID_SPIN_DEF	"01"

#define SAVELOC_DEF			"C:\\DATA\\PSR01"
#define PORT_DEF			"ERROR"

#define UPDATE_MESSAGE	(WM_USER+3333)
//registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;
///////////////////////////////////////////////////////////////////////////
// CPSRModParam property page

IMPLEMENT_DYNCREATE(CPSRModParam, CPropertyPage)

CPSRModParam::CPSRModParam() : CPropertyPage(CPSRModParam::IDD)
{
}


CPSRModParam::CPSRModParam(UINT id, CISO* pParent, bool DoingConfig) 
	: CPropertyPage(id)
{
	m_pParent = pParent;
	m_bChange = false;
	m_bDataSetup = false;
	m_bCollectingParametersForNewISO = DoingConfig;
	m_pPS = NULL;
	m_pApplyButton = NULL;
	//m_pApplyButton = NULL;
	//m_pApplyPSRButton = NULL;  //<<QA>> later

	uIDC_RECEIVE = m_pParent->m_uIDC_RECEIVE;

	for (int i= IDC_PSRMODPARAMS_FIRST; i <= IDC_PSRMODPARAMS_LAST; i++)
	{
		StaticColors[i-IDC_PSRMODPARAMS_FIRST] = COLOR_DEFAULT_TEXT;
	}

	if (m_bCollectingParametersForNewISO)
	{
		//See if default section of INI file is already there...
		char testbuf[16];
		int numchars = GetPrivateProfileSection(
			PSR_DEFAULT,
			testbuf,
			sizeof(testbuf),
			m_pParent->m_szIniFile);

		//If not, then go write it
		if (numchars == 0)
			WriteIniFileDefaultSection();
	}
}

CPSRModParam::~CPSRModParam()
{
	delete m_pApplyButton;
	delete m_pApplyPSRButton;
}

void CPSRModParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSRModParam)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPSRModParam, CPropertyPage)
	//{{AFX_MSG_MAP(CPSRModParam)
	ON_BN_CLICKED(IDC_PSRMODPARAMS_APPLY, OnModparamsApply)
	ON_BN_CLICKED(IDC_PSRMODPARAMS_APPLYPSR, OnModparamsApplyPSR)
	ON_BN_CLICKED(IDC_PSRMODPARAMS_BROWSE, OnModparamsBrowse)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_HIGHVOLTL_SPIN, OnDeltaposModparamsHighvoltlSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_HIGHVOLTH_SPIN, OnDeltaposModparamsHighvolthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_FILEID_SPIN,   OnDeltaposModparamsFileidSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_MAXPAUSE_SPIN, OnDeltaposModparamsMaxpauseSpin)
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_NODE_SPIN, OnDeltaposModparamsMostSpin)
	ON_CBN_SELCHANGE(IDC_PSRMODPARAMS_PORT, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_CT_UNITS, OnChangeModparamsCT)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_SAVELOC, OnChangeModparamsSaveLoc)
	ON_EN_KILLFOCUS(IDC_PSRMODPARAMS_FILEID, OnKillfocusModparamsFileid)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_HIGHVOLTL, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_REALSTHRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_TOTALSTHRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_TOTALS1THRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_TOTALSLTHRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_SIGMATHRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_MAXCOMPRESS_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_MAXCYCLE_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_PSRMODPARAMS_LOGFILTER, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_FILEID, OnChangeModparamsMost)
	ON_BN_CLICKED(IDC_PSRMODPARAMS_DODUMP, OnChangeModparamsMost)
	ON_BN_CLICKED(IDC_PSRMODPARAMS_DEFAULT, OnModparamsDefaultBut)
	ON_BN_CLICKED(IDC_PSRMODPARAMS_RESET, OnModparamsReset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_SLOW_RESET_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PSRMODPARAMS_MAX_TIMEOUT_SPIN,OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_SLOW_RESET,OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_NODE,		OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_MAXPAUSE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_MAXCYCLE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_REALSTHRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_TOTALSTHRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_TOTALSLTHRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_TOTALS1THRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_TOTALSLTHRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_SIGMATHRESH,OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_MAXCOMPRESS, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_CT_EXP, OnChangeModparamsCT)
	ON_CBN_SELCHANGE(IDC_PSRMODPARAMS_PREDELAY, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_GATEWIDTH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_HIGHV, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_HIGHVOLTH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_SAVELOC,OnChangeModparamsFileID)
	ON_EN_CHANGE(IDC_PSRMODPARAMS_PORT,OnChangeModparamsPort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPSRModParam::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_PSRMODPARAMS_FIRST) &&
		(index <= IDC_PSRMODPARAMS_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

void CPSRModParam::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_PSRMODPARAMS_FIRST) &&
		(index <= IDC_PSRMODPARAMS_LAST)) {
	  StaticColors[index-IDC_PSRMODPARAMS_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CPSRModParam::SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox)
{
	char temp[MAX_PATH+1];
	char temp2[2*MAX_PATH];
	if (FromCheckBox)
		strcpy(temp,(((CButton*)GetDlgItem(dDlgItem))->GetState()&0001)?"1":"0");
	else
		GetDlgItem(dDlgItem)->GetWindowText(temp,sizeof(temp));
	while (temp[strlen(temp)-1]=='\\')
		temp[strlen(temp)-1]=NULL;
	WritePrivateProfileString(szName, szItem, temp, m_pParent->m_szIniFile);
	strcpy(temp2,"53370 PSR COLLECT CONFIG CHANGE - ");
	strcat(temp2,szItem);
	strcat(temp2," = ");
	strcat(temp2,temp);
	strcat(temp2,"\n");

	if (m_pPS && !m_bCollectingParametersForNewISO)
		((CPSRPropertySheet*)m_pPS)->WriteToFile(TO_PFM,TYPE_COMP,NULL,temp2);
}

void CPSRModParam::DoApply()
{
	//get all the data from the controls and save them
//	TRACE("CPSRModParam::DoApply\n");

	char Name[_MAX_PATH];
	char Temp[_MAX_PATH];

	strncpy(Name, m_pParent->GetName(),sizeof(Name)-1);

	if (GetDlgItemInt(IDC_PSRMODPARAMS_MAX_TIMEOUT) < MAX_TIMEOUT_SPIN_MIN)
		SetDlgItemInt(IDC_PSRMODPARAMS_MAX_TIMEOUT,MAX_TIMEOUT_SPIN_MIN);
	if (GetDlgItemInt(IDC_PSRMODPARAMS_MAX_TIMEOUT) > MAX_TIMEOUT_SPIN_MAX)
		SetDlgItemInt(IDC_PSRMODPARAMS_MAX_TIMEOUT,MAX_TIMEOUT_SPIN_MAX);
	
	if (GetDlgItemInt(IDC_PSRMODPARAMS_SLOW_RESET) < SLOW_RESET_SPIN_MIN)
		SetDlgItemInt(IDC_PSRMODPARAMS_SLOW_RESET,SLOW_RESET_SPIN_MIN);
	if (GetDlgItemInt(IDC_PSRMODPARAMS_SLOW_RESET) > SLOW_RESET_SPIN_MAX)
		SetDlgItemInt(IDC_PSRMODPARAMS_SLOW_RESET,SLOW_RESET_SPIN_MAX);

	SaveItem(Name,"HIGHVOLTH",	IDC_PSRMODPARAMS_HIGHVOLTH);
	SaveItem(Name,"HIGHVOLTL",	IDC_PSRMODPARAMS_HIGHVOLTL);
	SaveItem(Name,"SLOW_RESET",	IDC_PSRMODPARAMS_SLOW_RESET);
	SaveItem(Name, NODE ,		IDC_PSRMODPARAMS_NODE);
	SaveItem(Name,"MAXCYCLE",	IDC_PSRMODPARAMS_MAXCYCLE);
	SaveItem(Name, MAXPAUSE ,	IDC_PSRMODPARAMS_MAXPAUSE);
	SaveItem(Name,"COMMFAIL",	IDC_PSRMODPARAMS_MAX_TIMEOUT);
	SaveItem(Name,"REALSTHRESH", IDC_PSRMODPARAMS_REALSTHRESH);
	SaveItem(Name,"TOTALSTHRESH", IDC_PSRMODPARAMS_TOTALSTHRESH);
	SaveItem(Name,"TOTALS1THRESH",IDC_PSRMODPARAMS_TOTALS1THRESH);
	SaveItem(Name,"TOTALSLOWTHRESH",IDC_PSRMODPARAMS_TOTALSLTHRESH);
	SaveItem(Name,"SIGMATHRESH", IDC_PSRMODPARAMS_SIGMATHRESH);
	SaveItem(Name,"MAXCOMPRESS",  IDC_PSRMODPARAMS_MAXCOMPRESS);
	SaveItem(Name,"COUNTTIMEUNIT",IDC_PSRMODPARAMS_CT_UNITS);
	SaveItem(Name,"COUNTTIMEEXP", IDC_PSRMODPARAMS_CT_EXP);
	SaveItem(Name,"DO_DUMP",	IDC_PSRMODPARAMS_DODUMP,	true);
	SaveItem(Name,"LOG_FILTER",	IDC_PSRMODPARAMS_LOGFILTER, true);
//	SaveItem(Name,"LOG_MII",	IDC_PSRMODPARAMS_LOGMII,	true);
	SaveItem(Name, FILE_ID,	IDC_PSRMODPARAMS_FILEID);
	SaveItem(Name,"PORT",		IDC_PSRMODPARAMS_PORT);

	GetDlgItemText(IDC_PSRMODPARAMS_CT_UNITS,Temp,sizeof(Temp));
	WritePrivateProfileString(Name, "COUNTTIMEUNIT", Temp, m_pParent->m_szIniFile);
	GetDlgItemText(IDC_PSRMODPARAMS_CT_EXP,Temp,sizeof(Temp));
	WritePrivateProfileString(Name, "COUNTTIMEEXP",Temp,m_pParent->m_szIniFile);
	int select = ((CComboBox*)GetDlgItem(IDC_PSRMODPARAMS_PREDELAY))->GetCurSel();
	sprintf(Temp,"%0X",select);
	WritePrivateProfileString(Name, "PREDELAY",Temp,m_pParent->m_szIniFile);
	GetDlgItemText(IDC_PSRMODPARAMS_GATEWIDTH,Temp,sizeof(Temp));
	WritePrivateProfileString(Name,"GATEWIDTH",Temp,m_pParent->m_szIniFile);
	GetDlgItemText(IDC_PSRMODPARAMS_HIGHV,Temp,sizeof(Temp));
	WritePrivateProfileString(Name,"HIGHV",Temp,m_pParent->m_szIniFile);
	
	GetDlgItemText(IDC_PSRMODPARAMS_SAVELOC,Temp,sizeof(Temp));
	while ((Temp[strlen(Temp)-1]=='\\')||
		   (Temp[strlen(Temp)-1]=='/' )  ) 
	{
		Temp[strlen(Temp)-1] = NULL;
		SetDlgItemText(IDC_PSRMODPARAMS_SAVELOC,Temp);
	}
	SaveItem(Name,SAVE_LOC,	IDC_PSRMODPARAMS_SAVELOC);
	if (access(Temp,0)==-1)
	{
		_mkdir(Temp);
		strcat(Temp,"\\archive");
		_mkdir(Temp);
	}

	//propagate all the new values to the program
	if (!m_bCollectingParametersForNewISO)
	{
		m_pParent->UpdateParams(true);
		m_pApplyButton->EnableWindow(FALSE);
	}
}

void CPSRModParam::OnModparamsApplyPSR()
{
	//Button caption:  "Apply New PSR Configuration To Instrument && MIC"
	if (m_pParent->Validate())
	{
		char *pName = m_pParent->m_pName;
		char *file = m_pParent->m_szIniFile;
		char temp[32];

		//if it doesn't have these in it then give it a set
		GetPrivateProfileString(pName,"CAMERA_PORT","",temp,sizeof(temp),file);
		if (strlen(temp)==0)
		{
			WritePrivateProfileString(pName,"CAMERA_PORT","LPT2",file);
			WritePrivateProfileString(pName,"CAMERA_PIN","2",file);
			WritePrivateProfileString(pName,"CAMERA_TRIGGER_TYPE","RT1",file);
//			WritePrivateProfileString(pName,"CAMERA_TRIGGER_THRESHOLD_RATE","50.00",m_pParent->m_szIniFile);
			WritePrivateProfileString(pName,"CAMERA_TRIGGER_HOLDOFF","0:0:0:10",file);
			WritePrivateProfileString(pName,"CAMERA_TRIGGER_SUPPRESS","0:0:5:0",file);
		}

		//get each of the PSR specific dialog item and save them in the ini file
		GetDlgItemText(IDC_PSRMODPARAMS_CT_UNITS,temp,sizeof(temp));
		if (strlen(temp) < 1)
		{
			strcpy(temp,CT_UNITS_SPIN_DEF);
			SetDlgItemText(IDC_PSRMODPARAMS_CT_UNITS,temp);
		}
		WritePrivateProfileString(pName, "COUNTTIMEUNIT", temp, file);

		//get exponential of count time
		GetDlgItemText(IDC_PSRMODPARAMS_CT_EXP,temp,sizeof(temp));
		if (strlen(temp) < 1)
		{
			strcpy(temp, CT_EXP_SPIN_DEF);
			SetDlgItemText(IDC_PSRMODPARAMS_CT_EXP,temp);
		}
		WritePrivateProfileString(pName, "COUNTTIMEEXP",temp,file);

		//get predelay
		int select = ((CComboBox*)GetDlgItem(IDC_PSRMODPARAMS_PREDELAY))->GetCurSel();
		sprintf(temp,"%0X",select);
		WritePrivateProfileString(pName,"PREDELAY",temp,file);

		//get gate width
		GetDlgItemText(IDC_PSRMODPARAMS_GATEWIDTH,temp,sizeof(temp));
		WritePrivateProfileString(pName,"GATEWIDTH",temp,file);

		//get high voltage
		GetDlgItemText(IDC_PSRMODPARAMS_HIGHV,temp,sizeof(temp));
		WritePrivateProfileString(pName,"HIGHV",temp,file);

		//tell parent instrument to do the jsr configurations
		m_pParent->PostMessage(uIDC_RECEIVE,RESETUP,NULL);
		m_pParent->UpdateParams(true);

		//tell the graphic window to update params
		char szTemp[_MAX_PATH];
		strcpy(szTemp,pName);
		strcat(szTemp," Watcher");
		CWnd * pcWnd = FindWindow(NULL,szTemp);
		if (pcWnd)
			pcWnd->PostMessage(UPDATE_MESSAGE,1,0);
	
	}
}

void CPSRModParam::OnModparamsApply() 
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (m_pParent->Validate()) 
	{
		DoApply();
		//tell the graphic window to update params
		char szTemp[_MAX_PATH];
		strcpy(szTemp,m_pParent->m_pName);
		strcat(szTemp," Watcher");
		CWnd * pcWnd = FindWindow(NULL,szTemp);
		if (pcWnd)
			pcWnd->PostMessage(UPDATE_MESSAGE,1,0);

		m_bChange = false;
		m_pApplyButton->EnableWindow(FALSE);
	}
}

//reset float item for a spinner with a non-automatic buddy
void CPSRModParam::ResetItem(char *szName,char *szItem,char*szDefault,int dItem,int dMin, int dMax, int dTextItem)
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
void CPSRModParam::ResetItemI(char *szName,char *szItem,char*szDefault,
						   int dItem,int dMin, int dMax, int dTextItem)
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
//	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(0,32000);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(0);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetAccel(5, (struct _UDACCEL *)pAccel );
//	GetDlgItem(dTextItem)->SetWindowText(temp);
//	itoa(dMax,temp,10);
//	((CEdit*)GetDlgItem(dTextItem))->SetLimitText(strlen(temp));

	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(0,32000);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(0);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(atoi(szDefault));
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetAccel(5, (struct _UDACCEL *)pAccel );
	GetDlgItem(dTextItem)->SetWindowText(temp);
	itoa(dMax,temp,10);
	((CEdit*)GetDlgItem(dTextItem))->SetLimitText(strlen(temp));

}

//reset item for a spinner with an automatic buddy
void CPSRModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem,int dMin, int dMax)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(dtemp);
		char temp[32];
		itoa(dMax,temp,10);
		((CEdit*)((CSpinButtonCtrl*)GetDlgItem(dItem))->GetBuddy())->SetLimitText(strlen(temp));
}

//reset item for a check box
void CPSRModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
	((CButton*)GetDlgItem(dItem))->SetCheck(dtemp);
}

//reset item for a list box
void CPSRModParam::ResetItemL(char *szName,char *szItem, int /*dDefault*/, int dItem)
{
	char temp[4];
	int dtemp;
	//should change the "1" to itoa(dDefault)
	GetPrivateProfileString(szName,szItem,"1",temp,sizeof(temp),m_pParent->m_szIniFile);
	sscanf(temp,"%x",&dtemp);
	((CComboBox*)GetDlgItem(dItem))->SetCurSel(dtemp);
}

//reset item for a simple edit field
void CPSRModParam::ResetItem(char *szName,char *szItem, char *szDefault, int dItem)
{
	char temp[MAX_PATH+1];
//	int nSelect = CB_ERR;
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	switch(dItem)
	{
	case IDC_PSRMODPARAMS_PORT:
		((CComboBox*)GetDlgItem(dItem))->SelectString(-1, temp);
		break;
	default:; 
	}
	GetDlgItem(dItem)->SetWindowText(temp);
}

void CPSRModParam::SetPropertySheet(CPropertySheet *pPS)//, char *pName)
{
	m_pPS = (CTPropertySheet*)pPS;
}


void CPSRModParam::ResetItem(char *szName, char *szItem, char *szDefault,
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

void CPSRModParam::OnModparamsReset() 
{
//	TRACE("CPSRModParam::OnModparamsReset\n");

	if (m_pPS)m_pPS->SetToClose(0);

	m_bDataSetup = true;

	if (!m_bCollectingParametersForNewISO)
	{
		GetDlgItem(IDC_PSRMODPARAMS_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PSRMODPARAMS_NODE)->EnableWindow(FALSE);
	}

	ResetItem(m_pParent->m_pName,FILE_ID, FILEID_SPIN_DEF,
		IDC_PSRMODPARAMS_FILEID_SPIN,IDC_PSRMODPARAMS_FILEID);

	ResetItem(m_pParent->m_pName,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_PSRMODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"COMMFAIL",MAX_TIMEOUT_SPIN_DEF,
		IDC_PSRMODPARAMS_MAX_TIMEOUT_SPIN,MAX_TIMEOUT_SPIN_MIN,MAX_TIMEOUT_SPIN_MAX);

	ResetItem(m_pParent->m_pName, NODE ,NODE_SPIN_DEF,
		IDC_PSRMODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);
	
	ResetItem(m_pParent->m_pName,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_PSRMODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"DO_DUMP", DODUMP_DEF, IDC_PSRMODPARAMS_DODUMP);

	ResetItem(m_pParent->m_pName,"LOG_FILTER", LOGFILTER_DEF, IDC_PSRMODPARAMS_LOGFILTER);

//	ResetItem(m_pParent->m_pName,"LOG_MII", LOGMII_DEF, IDC_PSRMODPARAMS_LOGMII);

	ResetItem(m_pParent->m_pName,SAVE_LOC, SAVELOC_DEF, IDC_PSRMODPARAMS_SAVELOC);

	char buff[16384];
	char buff2[64];
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,
		sizeof(buff), m_pParent->m_szIniFile)>0)
	{
		char *at = buff;
		((CComboBox*)GetDlgItem(IDC_PSRMODPARAMS_PORT))->ResetContent();

		while (strlen(at) > 0)
		{
			strcpy(buff2,at);
			strtok(buff2,"=");
			((CComboBox*)GetDlgItem(IDC_PSRMODPARAMS_PORT))->AddString(buff2);

			at = at + strlen(at)+1;
		}

		((CComboBox*)GetDlgItem(IDC_PSRMODPARAMS_PORT))->SetCurSel(0);
	}

	ResetItem(m_pParent->m_pName,PORT, PORT_DEF, IDC_PSRMODPARAMS_PORT);

	ResetItemI(m_pParent->m_pName,MAXPAUSE,
		MAXPAUSE_SPIN_DEF,
		IDC_PSRMODPARAMS_MAXPAUSE_SPIN,
		MAXPAUSE_SPIN_MIN,
		MAXPAUSE_SPIN_MAX,
		IDC_PSRMODPARAMS_MAXPAUSE);

	ResetItemI(m_pParent->m_pName,"HIGHVOLTH",HIGHVOLTH_SPIN_DEF,
		IDC_PSRMODPARAMS_HIGHVOLTH_SPIN,HIGHVOLTH_SPIN_MIN,HIGHVOLTH_SPIN_MAX,
		IDC_PSRMODPARAMS_HIGHVOLTH);

	ResetItemI(m_pParent->m_pName,"HIGHVOLTL",HIGHVOLTL_SPIN_DEF,
		IDC_PSRMODPARAMS_HIGHVOLTL_SPIN,HIGHVOLTL_SPIN_MIN,HIGHVOLTL_SPIN_MAX,
		IDC_PSRMODPARAMS_HIGHVOLTL);

	ResetItem(m_pParent->m_pName,"REALSTHRESH",
		REALSTHRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_REALSTHRESH_SPIN,
		REALSTHRESH_SPIN_MIN,
		REALSTHRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"TOTALSTHRESH",
		TOTALSTHRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_TOTALSTHRESH_SPIN,
		TOTALSTHRESH_SPIN_MIN,
		TOTALSTHRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"TOTALS1THRESH",
		TOTALS1THRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_TOTALS1THRESH_SPIN,
		TOTALS1THRESH_SPIN_MIN,
		TOTALS1THRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"TOTALSLOWTHRESH",
		TOTALSLTHRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_TOTALSLTHRESH_SPIN,
		TOTALSLTHRESH_SPIN_MIN,
		TOTALSLTHRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"SIGMATHRESH",
		SIGMATHRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_SIGMATHRESH_SPIN,
		SIGMATHRESH_SPIN_MIN,
		SIGMATHRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"GATEWIDTH",
		GATEWIDTH_SPIN_DEF,
		IDC_PSRMODPARAMS_GATEWIDTH_SPIN,
		GATEWIDTH_SPIN_MIN,
		GATEWIDTH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"HIGHV",
		HIGHV_SPIN_DEF,
		IDC_PSRMODPARAMS_HIGHV_SPIN,
		HIGHV_SPIN_MIN,
		HIGHV_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"MAXCOMPRESS",
		MAXCOMPRESS_SPIN_DEF,
		IDC_PSRMODPARAMS_MAXCOMPRESS_SPIN,
		MAXCOMPRESS_SPIN_MIN,
		MAXCOMPRESS_SPIN_MAX);

	ResetItemI(m_pParent->m_pName,"COUNTTIMEUNIT",
		CT_UNITS_SPIN_DEF,
		IDC_PSRMODPARAMS_CT_UNITS_SPIN,
		CT_UNITS_SPIN_MIN,
		CT_UNITS_SPIN_MAX,
		IDC_PSRMODPARAMS_CT_UNITS);

	ResetItemI(m_pParent->m_pName,"COUNTTIMEEXP",
		CT_EXP_SPIN_DEF,
		IDC_PSRMODPARAMS_CT_EXP_SPIN,
		CT_EXP_SPIN_MIN,
		CT_EXP_SPIN_MAX,
		IDC_PSRMODPARAMS_CT_EXP);

	ResetItemL(m_pParent->m_pName,"PREDELAY",
		PREDELAY_DEF,
		IDC_PSRMODPARAMS_PREDELAY);

	m_bChange = false;

	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}

void CPSRModParam::DoSpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
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
	*pResult = 0;
}

void CPSRModParam::DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
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
	sscanf(temp2,"%d",&buddyvalue);
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

void CPSRModParam::DoSpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{
	if (m_pPS)m_pPS->SetToClose(0);

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

void CPSRModParam::DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
							int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{
	if (m_pPS)m_pPS->SetToClose(0);

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
	sscanf(temp2,"%f",&fbuddyvalue);
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


void CPSRModParam::DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{
	if (m_pPS)m_pPS->SetToClose(0);

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
	itoa(iNew,temp,10);
	while (strlen(temp) < 2)
	{
		temp[2] = NULL;
		temp[1] = temp[0];
		temp[0] = '0';
	}

	SetDlgItemText(DLGITEM,temp);

	CString Text;
	GetDlgItemText(IDC_PSRMODPARAMS_SAVELOC,Text);

//	if "PSR" at end then...
	Text.MakeReverse();
	switch (Text.Find("RSJ"))
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
				Text += "PSR";
			else
				Text += "\\PSR";
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
				Text += "PSR";
			else
				Text += "\\PSR";
		}
		break;
	default:
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "PSR";
			else
				Text += "\\PSR";
		}
	}

	*pResult = 0;
	Text += temp;
	SetDlgItemText(IDC_PSRMODPARAMS_SAVELOC,Text);
}

void CPSRModParam::OnDeltaposModparamsHighvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	DoBuddySpinI(pNMHDR, pResult, HIGHVOLTL_SPIN_MIN, HIGHVOLTL_SPIN_MAX, 
		IDC_PSRMODPARAMS_HIGHVOLTL, IDC_PSRMODPARAMS_HIGHVOLTH,IDC_PSRMODPARAMS_HIGHVOLTH_SPIN, true);
}

void CPSRModParam::OnDeltaposModparamsHighvolthSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);;
	}
	DoBuddySpinI(pNMHDR, pResult, HIGHVOLTH_SPIN_MIN, HIGHVOLTH_SPIN_MAX, 
		IDC_PSRMODPARAMS_HIGHVOLTH, IDC_PSRMODPARAMS_HIGHVOLTL, IDC_PSRMODPARAMS_HIGHVOLTL_SPIN, false);
}

void CPSRModParam::OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	DoSpinI(pNMHDR, pResult, MAXPAUSE_SPIN_MIN, MAXPAUSE_SPIN_MAX, IDC_PSRMODPARAMS_MAXPAUSE);
}

void CPSRModParam::OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS)m_pPS->SetToClose(0);

	DoSpinID(pNMHDR, pResult, FILEID_SPIN_MIN, FILEID_SPIN_MAX, IDC_PSRMODPARAMS_FILEID);
}

void CPSRModParam::OnModparamsBrowse() 
{
	if (m_pPS)m_pPS->SetToClose(0);

	CDirDialog DirDialog(this);
	char temp[_MAX_DIR+1];
	GetDlgItem(IDC_PSRMODPARAMS_SAVELOC)->GetWindowText(temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Directory to save files in.";
	if (DirDialog.DoBrowse())
	{
		CString Id;
		GetDlgItemText(IDC_PSRMODPARAMS_FILEID,Id);
		Id.TrimLeft();
		CString Text(DirDialog.m_strPath);
		CString Name("PSR"+Id);
		if (Text.Find(Name,0)==-1)
		{
			if (Text.Right(1) == '\\')
				Text += "PSR";
			else
				Text += "\\PSR";
			Text += Id;
		}
		SetDlgItemText(IDC_PSRMODPARAMS_SAVELOC, Text);

		if (m_pApplyButton)
			m_pApplyButton->EnableWindow(TRUE);
	}
}

void CPSRModParam::OnModparamsDefaultBut()
{
	OnModparamsDefault(false);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CPSRModParam::OnModparamsDefault(bool SetPort) 
{
	if (m_pPS)m_pPS->SetToClose(0);

	m_bDataSetup = true;

	ResetItem(PSR_DEFAULT, FILE_ID, FILEID_SPIN_DEF,  
		IDC_PSRMODPARAMS_FILEID_SPIN, IDC_PSRMODPARAMS_FILEID);

	ResetItem(PSR_DEFAULT,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_PSRMODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"COMMFAIL",MAX_TIMEOUT_SPIN_DEF,
		IDC_PSRMODPARAMS_MAX_TIMEOUT_SPIN,MAX_TIMEOUT_SPIN_MIN,MAX_TIMEOUT_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_PSRMODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"DO_DUMP",DODUMP_DEF,IDC_PSRMODPARAMS_DODUMP);

	ResetItem(PSR_DEFAULT,"LOG_FILTER",LOGFILTER_DEF,IDC_PSRMODPARAMS_LOGFILTER);

	ResetItem(PSR_DEFAULT,SAVE_LOC,SAVELOC_DEF,IDC_PSRMODPARAMS_SAVELOC);

	if (SetPort || m_bCollectingParametersForNewISO)
	{
		ResetItem(PSR_DEFAULT,"NODE",NODE_SPIN_DEF,
			IDC_PSRMODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);
		
		char buff[16384];
		char buff2[32];
		if (GetPrivateProfileSection(COMMUNICATIONS,buff,
			sizeof(buff), m_pParent->m_szIniFile)>0)
		{
			char *at = buff;
			((CComboBox*)GetDlgItem(IDC_PSRMODPARAMS_PORT))->ResetContent();

			while (strlen(at) > 0)
			{
				strcpy(buff2,at);
				strtok(buff2,"=");
				((CComboBox*)GetDlgItem(IDC_PSRMODPARAMS_PORT))->AddString(buff2);

				at = at + strlen(at)+1;
			}
		}
		((CComboBox*)GetDlgItem(IDC_PSRMODPARAMS_PORT))->SetCurSel(0);
	
		ResetItem(PSR_DEFAULT,PORT,PORT_DEF,IDC_PSRMODPARAMS_PORT);
	}

	ResetItemI(PSR_DEFAULT,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_PSRMODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,
		IDC_PSRMODPARAMS_MAXPAUSE);

	ResetItemI(PSR_DEFAULT,"HIGHVOLTH",HIGHVOLTH_SPIN_DEF,
		IDC_PSRMODPARAMS_HIGHVOLTH_SPIN,HIGHVOLTH_SPIN_MIN,HIGHVOLTH_SPIN_MAX,
		IDC_PSRMODPARAMS_HIGHVOLTH);

	ResetItemI(PSR_DEFAULT,"HIGHVOLTL",HIGHVOLTL_SPIN_DEF,
		IDC_PSRMODPARAMS_HIGHVOLTL_SPIN,HIGHVOLTL_SPIN_MIN,HIGHVOLTL_SPIN_MAX,
		IDC_PSRMODPARAMS_HIGHVOLTL);

	ResetItem(PSR_DEFAULT,"REALSTHRESH",
		REALSTHRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_REALSTHRESH_SPIN,
		REALSTHRESH_SPIN_MIN,
		REALSTHRESH_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"TOTALSTHRESH",
		TOTALSTHRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_TOTALSTHRESH_SPIN,
		TOTALSTHRESH_SPIN_MIN,
		TOTALSTHRESH_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"TOTALS1THRESH",
		TOTALS1THRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_TOTALS1THRESH_SPIN,
		TOTALS1THRESH_SPIN_MIN,
		TOTALS1THRESH_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"TOTALSLOWTHRESH",
		TOTALSLTHRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_TOTALSLTHRESH_SPIN,
		TOTALSLTHRESH_SPIN_MIN,
		TOTALSLTHRESH_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"SIGMATHRESH",
		SIGMATHRESH_SPIN_DEF,
		IDC_PSRMODPARAMS_SIGMATHRESH_SPIN,
		SIGMATHRESH_SPIN_MIN,
		SIGMATHRESH_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"GATEWIDTH",
		GATEWIDTH_SPIN_DEF,
		IDC_PSRMODPARAMS_GATEWIDTH_SPIN,
		GATEWIDTH_SPIN_MIN,
		GATEWIDTH_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"HIGHV",
		HIGHV_SPIN_DEF,
		IDC_PSRMODPARAMS_HIGHV_SPIN,
		HIGHV_SPIN_MIN,
		HIGHV_SPIN_MAX);

	ResetItem(PSR_DEFAULT,"MAXCOMPRESS",
		MAXCOMPRESS_SPIN_DEF,
		IDC_PSRMODPARAMS_MAXCOMPRESS_SPIN,
		MAXCOMPRESS_SPIN_MIN,
		MAXCOMPRESS_SPIN_MAX);

	ResetItemI(PSR_DEFAULT,"COUNTTIMEUNIT",
		CT_UNITS_SPIN_DEF,
		IDC_PSRMODPARAMS_CT_UNITS_SPIN,
		CT_UNITS_SPIN_MIN,
		CT_UNITS_SPIN_MAX,
		IDC_PSRMODPARAMS_CT_UNITS);

	ResetItemI(PSR_DEFAULT,"COUNTTIMEEXP",
		CT_EXP_SPIN_DEF,
		IDC_PSRMODPARAMS_CT_EXP_SPIN,
		HIGHVOLTL_SPIN_MIN,
		HIGHVOLTL_SPIN_MAX,
		IDC_PSRMODPARAMS_CT_EXP);

	ResetItemL(PSR_DEFAULT,"PREDELAY",
		PREDELAY_DEF,
		IDC_PSRMODPARAMS_PREDELAY);

}


BOOL CPSRModParam::OnSetActive() 
{
//	TRACE("CPSRModParam::OnSetActive\n");

	if (m_bCollectingParametersForNewISO)
	{
		if (m_pApplyButton)
			m_pApplyButton->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_PSRMODPARAMS_RESET)->ShowWindow(SW_HIDE);

		if (!m_bDataSetup) 
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

		//GetDlgItem(IDC_PSRMODPARAMS_APPLY)->EnableWindow(false);
		SetDlgItemText(IDC_STATIC_MESSAGE,"These items will not be applied to the PSR-12 instrument until "
			"initial setup is completed.");
		GetDlgItem(IDC_PSRMODPARAMS_APPLYPSR)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_PSRMODPARAMS_APPLYPSR)->EnableWindow(false);
		GetDlgItem(IDC_PSRMODPARAMS_STATICTEXT)->SetWindowText("");
	}
	else  //!m_bCollectingParametersForNewISO
	{
		m_pApplyButton->EnableWindow(m_bChange);
		//GetDlgItem(IDC_PSRMODPARAMS_APPLYPSR)->EnableWindow(false);
		GetDlgItem(IDC_PSRMODPARAMS_APPLYPSR)->EnableWindow(m_pParent->m_bPause);
	}
	return CPropertyPage::OnSetActive();
}

void CPSRModParam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPS)m_pPS->SetToClose(0);

/*
	CWnd* pWnd;

	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_PSRMODPARAMS_FIRST) && (index <= IDC_PSRMODPARAMS_LAST))
		{
			MessageBox("Got One",MB_OK);
		}
	}
*/
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CPSRModParam::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	if (m_bCollectingParametersForNewISO && bShow)
	{
//		m_pPS->SetWizardButtons(PSWIZB_FINISH);
		m_pPS->SetWizardButtons(PSWIZB_NEXT);
		m_pPS->GetDlgItem(ID_WIZFINISH)->ShowWindow(SW_SHOW);
//		m_pPS->GetDlgItem(ID_WIZNEXT)->ShowWindow(SW_SHOW);
		m_pPS->GetDlgItem(ID_WIZFINISH)->EnableWindow(FALSE);

		m_pApplyButton->ShowWindow(SW_HIDE);
		m_pApplyPSRButton->ShowWindow(SW_HIDE);
	}
	else  //SCR00114
	{
		if (m_pApplyPSRButton)
			m_pApplyPSRButton->EnableWindow(m_pParent->IsPaused());
	}
	if (m_pPS)m_pPS->SetToClose(0);
}

void CPSRModParam::OnDeltaposModparamsMostSpin(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	//NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	*pResult = 0;
}

void CPSRModParam::OnChangeModparamsMost()
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO && m_bDataSetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CPSRModParam::OnChangeModparamsPort()
{
	if (m_pPS)m_pPS->SetToClose(0);
}

void CPSRModParam::OnChangeModparamsFileID()
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CPSRModParam::OnChangeModparamsCT()
{
	if (m_pPS)m_pPS->SetToClose(0);

	char units[4];
	char exp[4];
	char result[16];
	GetDlgItemText(IDC_PSRMODPARAMS_CT_UNITS,units,sizeof(units));
	while (strlen(units) < 2)
	{
		units[2] = NULL;
		units[1] = units[0];
		units[0] = '0';
	}
	GetDlgItemText(IDC_PSRMODPARAMS_CT_EXP,exp,sizeof(exp));
	result[0] = units[0];
	result[1] = '.';
	result[2] = units[1];
	result[3] = 'E';
	result[4] = exp[0];
	result[5] = NULL;
	SetDlgItemText(IDC_STATIC_CT_RESULT,result);

	if (!m_bCollectingParametersForNewISO && m_bDataSetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CPSRModParam::OnChangeModparamsSaveLoc() 
{
	CString Text;
	GetDlgItemText(IDC_PSRMODPARAMS_SAVELOC,Text);
	if (!Text.IsEmpty())
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_PSRMODPARAMS_SAVELOC))->GetSel(posStart,posEnd);
		if (Text.FindOneOf(INVALIDCHARSSUB) != -1)
		{
			((CEdit*)GetDlgItem(IDC_PSRMODPARAMS_SAVELOC))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_PSRMODPARAMS_SAVELOC))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_PSRMODPARAMS_SAVELOC))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_PSRMODPARAMS_SAVELOC))->SetSel(posStart,posEnd);
	}

	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CPSRModParam::OnKillfocusModparamsFileid() 
{
	char temp[3];
	GetDlgItemText(IDC_PSRMODPARAMS_FILEID,temp,sizeof(temp));
	while (strlen(temp) < 2)
	{
		temp[2] = temp [1];
		temp[1] = temp [0];
		temp[0] = '0';
	}
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(IDC_PSRMODPARAMS_FILEID_SPIN))->SetPos(value);
	((CEdit*)GetDlgItem(IDC_PSRMODPARAMS_FILEID))->SetWindowText(temp);
}

BOOL CPSRModParam::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_pApplyButton = new CSimpleYellowButton( //SCR00114
		"Apply",
		IDC_PSRMODPARAMS_APPLY,
		this);

	//SCR00114
	m_pApplyPSRButton = new CSimpleYellowButton(
		"Apply New PSR\nConfiguration To Instrument && MIC",
		IDC_PSRMODPARAMS_APPLYPSR,
		this);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
//
//                   WriteIniFileDefaultSection
//
//////////////////////////////////////////////////////////////////////////////
void CPSRModParam::WriteIniFileDefaultSection()
{
//	TRACE("CPSRModParam::WriteIniFileDefaultSection()\n");

	char *file = m_pParent->m_szIniFile;

	WritePrivateProfileString(PSR_DEFAULT,"MAXPAUSE","60",file);
	WritePrivateProfileString(PSR_DEFAULT,"COMMFAIL","20",file);
	WritePrivateProfileString(PSR_DEFAULT,"DO_DUMP","0",file);
	WritePrivateProfileString(PSR_DEFAULT,"SLOW_RESET","900",file);
	WritePrivateProfileString(PSR_DEFAULT,SAVE_LOC,"C:\\DATA\\PSR01",file);
	WritePrivateProfileString(PSR_DEFAULT,FILE_ID,"01",file);
	WritePrivateProfileString(PSR_DEFAULT,"HIGHVOLTH","1803",file);
	WritePrivateProfileString(PSR_DEFAULT,"HIGHVOLTL","1797",file);
	WritePrivateProfileString(PSR_DEFAULT, NODE ,"-1",file);
	WritePrivateProfileString(PSR_DEFAULT, PORT,"",file);
	WritePrivateProfileString(PSR_DEFAULT,"MAXCYCLE","200",file);
	WritePrivateProfileString(PSR_DEFAULT,"LOG_FILTER","1",file);
	WritePrivateProfileString(PSR_DEFAULT, FLAGCOMM ,"0",file);
	WritePrivateProfileString(PSR_DEFAULT, FLAGOTHR ,"0",file);
	WritePrivateProfileString(PSR_DEFAULT, FLAGTIME ,"0",file);
	WritePrivateProfileString(PSR_DEFAULT,"COUNTTIMEUNIT","30",file);
	WritePrivateProfileString(PSR_DEFAULT,"COUNTTIMEEXP","1",file);
	WritePrivateProfileString(PSR_DEFAULT,"PREDELAY","6",file);
	WritePrivateProfileString(PSR_DEFAULT,"GATEWIDTH","64",file);
	WritePrivateProfileString(PSR_DEFAULT,"HIGHV","1800",file);
	WritePrivateProfileString(PSR_DEFAULT,"REALSTHRESH","50",file);
	WritePrivateProfileString(PSR_DEFAULT,"TOTALSTHRESH","50",file);
	WritePrivateProfileString(PSR_DEFAULT,"TOTALS1THRESH","50",file);
	WritePrivateProfileString(PSR_DEFAULT,"SIGMATHRESH","50",file);
	WritePrivateProfileString(PSR_DEFAULT,"MAXCOMPRESS","25",file);

	WritePrivateProfileString(PSR_DEFAULT,"CAMERA_PIN","2",file);

	WritePrivateProfileString(PSR_DEFAULT,"Remark: Allowed CAMERA_PORT strings","PORT1, PORT2, PORT3, or NONE",file);
	WritePrivateProfileString(PSR_DEFAULT,"CAMERA_PORT","NONE",file);

	WritePrivateProfileString(PSR_DEFAULT,"Remark: Allowed CAMERA_TRIGGER_TYPE","R and/or T and/or 1",file);
	WritePrivateProfileString(PSR_DEFAULT,"Remark: where R","reals, T = Totals, and 1 = Totals 1",file);

	WritePrivateProfileString(PSR_DEFAULT,"CAMERA_TRIGGER_TYPE","RT1",file);

	WritePrivateProfileString(PSR_DEFAULT,"Remark: Format for CAMERA_TRIGGER_HOLDOFF","D:H:M:S",file);
	WritePrivateProfileString(PSR_DEFAULT,"CAMERA_TRIGGER_HOLDOFF","0:0:0:10",file);

	WritePrivateProfileString(PSR_DEFAULT,"Remark: Format for CAMERA_TRIGGER_SUPPRESS","D:H:M:S",file);
	WritePrivateProfileString(PSR_DEFAULT,"CAMERA_TRIGGER_SUPPRESS","0:0:5:0",file);

//	WritePrivateProfileString(PSR_DEFAULT,"CAMERA_TRIGGER_THRESHOLD_RATE","50.00",file);

}
