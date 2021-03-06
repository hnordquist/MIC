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
// MCAConfig2.cpp : implementation file

#include "stdafx.h"
#include "resource.h"
#include "MCAConfig2.h"
#include "MCAModParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMCAConfig2 property page

IMPLEMENT_DYNCREATE(CMCAConfig2, CPropertyPage)

CMCAConfig2::CMCAConfig2()
{
}

CMCAConfig2::CMCAConfig2(UINT /*id*/, CISO* pParent, bool /*bDoingConfig*/) 
	: CPropertyPage(CMCAConfig2::IDD)
{
	m_pParent = pParent;
	m_pPS = NULL;
	m_bAllowFinish = false;

	//{{AFX_DATA_INIT(CMCAConfig2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CMCAConfig2::SetPropertySheet(CTPropertySheet *pPS)
{
	m_pPS = pPS;
}

CMCAConfig2::~CMCAConfig2()
{
}

void CMCAConfig2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCAConfig2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCAConfig2, CPropertyPage)
	//{{AFX_MSG_MAP(CMCAConfig2)
	ON_BN_CLICKED(IDC_MCACONFIG_APPLY, OnMcaConfig2Apply)
	ON_BN_CLICKED(IDC_MCACONFIG_RESET, OnMcaConfig2Reset)
	ON_BN_CLICKED(IDC_MCACONFIG_DEFAULT, OnMcaConfig2Default)
	ON_BN_CLICKED(IDC_MCACONFIG_CLEARALL, OnMcaconfigClearall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMCAConfig2 message handlers
void CMCAConfig2::DoApply()
{
	//get all the stuff and save it

	char *Name = m_pParent->m_pName;
	
	SaveItem(Name,"CHAN1",	IDC_MCACONFIG_EDIT1);
	SaveItem(Name,"CHAN2",	IDC_MCACONFIG_EDIT2);
	SaveItem(Name,"CHAN3",	IDC_MCACONFIG_EDIT3);
	SaveItem(Name,"CHAN4",	IDC_MCACONFIG_EDIT4);
	SaveItem(Name,"CHAN5",	IDC_MCACONFIG_EDIT5);
	SaveItem(Name,"CHAN6",	IDC_MCACONFIG_EDIT6);	
	SaveItem(Name,"CHAN7",	IDC_MCACONFIG_EDIT7);
	SaveItem(Name,"ALARM1", IDC_EDIT_ALARM1);
	SaveItem(Name,"ALARM2", IDC_EDIT_ALARM2);
	SaveItem(Name,"ALARM3", IDC_EDIT_ALARM3);
	SaveItem(Name,"ALARM4", IDC_EDIT_ALARM4);
	SaveItem(Name,"ALARM5", IDC_EDIT_ALARM5);
	SaveItem(Name,"ALARM6", IDC_EDIT_ALARM6);
	SaveItem(Name,"ALARM7", IDC_EDIT_ALARM7);
}

void CMCAConfig2::OnMcaConfig2Apply() 
{
	if (m_pParent->Validate()) 
	{
		((CMCAModParam*)m_pPS->GetPage(0))->DoApply();
		DoApply();
	}
}

void CMCAConfig2::SaveItem(char *szName, char *szItem, int dDlgItem)
{
	char temp[MAX_PATH+1];
	GetDlgItem(dDlgItem)->GetWindowText(temp,sizeof(temp));
	WritePrivateProfileString(szName, szItem, temp, m_pParent->m_szIniFile);
}


void CMCAConfig2::OnMcaConfig2Reset() 
{
	// TODO: Add your control notification handler code here
}

void CMCAConfig2::OnMcaConfig2Default() 
{
	char temp[_MAX_PATH];

	char *inifile = m_pParent->m_szIniFile;

	GetPrivateProfileString(MCA_DEFAULT,"CHAN1","",temp,sizeof(temp),inifile);	
	SetDlgItemText(IDC_MCACONFIG_EDIT1,temp);
	GetPrivateProfileString(MCA_DEFAULT,"CHAN2","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_MCACONFIG_EDIT2,temp);
	GetPrivateProfileString(MCA_DEFAULT,"CHAN3","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_MCACONFIG_EDIT3,temp);
	GetPrivateProfileString(MCA_DEFAULT,"CHAN4","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_MCACONFIG_EDIT4,temp);
	GetPrivateProfileString(MCA_DEFAULT,"CHAN5","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_MCACONFIG_EDIT5,temp);
	GetPrivateProfileString(MCA_DEFAULT,"CHAN6","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_MCACONFIG_EDIT6,temp);	
	GetPrivateProfileString(MCA_DEFAULT,"CHAN7","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_MCACONFIG_EDIT7,temp);
	GetPrivateProfileString(MCA_DEFAULT,"ALARM1","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_EDIT_ALARM1,temp);
	GetPrivateProfileString(MCA_DEFAULT,"ALARM2","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_EDIT_ALARM2,temp);
	GetPrivateProfileString(MCA_DEFAULT,"ALARM3","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_EDIT_ALARM3,temp);
	GetPrivateProfileString(MCA_DEFAULT,"ALARM4","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_EDIT_ALARM4,temp);
	GetPrivateProfileString(MCA_DEFAULT,"ALARM5","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_EDIT_ALARM5,temp);
	GetPrivateProfileString(MCA_DEFAULT,"ALARM6","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_EDIT_ALARM6,temp);
	GetPrivateProfileString(MCA_DEFAULT,"ALARM7","",temp,sizeof(temp),inifile);
	SetDlgItemText(IDC_EDIT_ALARM7,temp);
	
}

BOOL CMCAConfig2::OnSetActive() 
{
//	m_pPS->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT | PSWIZB_FINISH);
	m_pPS->GetDlgItem(ID_WIZNEXT)->ShowWindow(SW_SHOW);
	m_pPS->GetDlgItem(ID_WIZNEXT)->EnableWindow(FALSE);
	m_pPS->GetDlgItem(ID_WIZBACK)->ShowWindow(SW_SHOW);
	m_pPS->GetDlgItem(ID_WIZBACK)->EnableWindow(TRUE);
	m_pPS->GetDlgItem(ID_WIZFINISH)->ShowWindow(SW_SHOW);

	if (m_bAllowFinish)
		m_pPS->GetDlgItem(ID_WIZFINISH)->EnableWindow(TRUE);
	else
		m_pPS->GetDlgItem(ID_WIZFINISH)->EnableWindow(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL CMCAConfig2::OnWizardFinish() 
{
	((CMCAModParam*)m_pPS->GetPage(0))->DoApply();
	DoApply();
	return CPropertyPage::OnWizardFinish();
}

BOOL CMCAConfig2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	((CEdit*)GetDlgItem(IDC_MCACONFIG_EDIT1))->SetLimitText(32);
	((CEdit*)GetDlgItem(IDC_MCACONFIG_EDIT2))->SetLimitText(32);
	((CEdit*)GetDlgItem(IDC_MCACONFIG_EDIT3))->SetLimitText(32);
	((CEdit*)GetDlgItem(IDC_MCACONFIG_EDIT4))->SetLimitText(32);
	((CEdit*)GetDlgItem(IDC_MCACONFIG_EDIT5))->SetLimitText(32);
	((CEdit*)GetDlgItem(IDC_MCACONFIG_EDIT6))->SetLimitText(32);
	((CEdit*)GetDlgItem(IDC_MCACONFIG_EDIT7))->SetLimitText(32);
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM1))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM2))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM3))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM4))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM5))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM6))->SetLimitText(8);
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM7))->SetLimitText(8);

	OnMcaConfig2Default();

	return TRUE;
}

void CMCAConfig2::OnMcaconfigClearall() 
{
	SetDlgItemText(IDC_MCACONFIG_EDIT1,"");
	SetDlgItemText(IDC_MCACONFIG_EDIT2,"");
	SetDlgItemText(IDC_MCACONFIG_EDIT3,"");
	SetDlgItemText(IDC_MCACONFIG_EDIT4,"");
	SetDlgItemText(IDC_MCACONFIG_EDIT5,"");
	SetDlgItemText(IDC_MCACONFIG_EDIT6,"");	
	SetDlgItemText(IDC_MCACONFIG_EDIT7,"");
	SetDlgItemText(IDC_EDIT_ALARM1,"");
	SetDlgItemText(IDC_EDIT_ALARM2,"");
	SetDlgItemText(IDC_EDIT_ALARM3,"");
	SetDlgItemText(IDC_EDIT_ALARM4,"");
	SetDlgItemText(IDC_EDIT_ALARM5,"");
	SetDlgItemText(IDC_EDIT_ALARM6,"");
	SetDlgItemText(IDC_EDIT_ALARM7,"");
}
