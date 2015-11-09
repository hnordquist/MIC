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
//$History: NewLOCNUTComm.cpp $
// * 
// * *****************  Version 1  *****************
// * User: Pelowitz     Date: 3/12/08    Time: 2:52p
// * Created in $/MIC 2004/CoLOCNUTCSO
// * At or near retirement (EOSS still being tested)
// 
// *****************  Version 2  *****************
// User: 149047       Date: 10/23/06   Time: 1:29p
// Updated in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 9/13/06    Time: 12:39p
// Created in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 1/11/06    Time: 7:47a
// Created in $/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz        Date: 11/16/2005
// Created in $/MIC 2.0/CoLOCNUTCSO
// Added CoLOCNUTCSO to workspace.
// 
// NewLOCNUTComm.cpp : implementation file
//

#include "stdafx.h"
#include "INI_definitions.h"
#include "NewLOCNUTComm.h"
#include "CSO_definitions.h"
#include "TimedMessageDialog.h"
#include "ComLOCNUT.h"
//////////////////////////////////////////////////////////////////////////////
//
//                 Constructors and Destructors
//
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CNewLOCNUTComm, CDialog)

CNewLOCNUTComm::CNewLOCNUTComm():CDialog(CNewLOCNUTComm::IDD)
{
//	TRACE("CNewLOCNUTComm::CTOR #1()\n");
}

CNewLOCNUTComm::CNewLOCNUTComm(char* cIniFile,int iDlgClose,char *name)
	: CDialog(CNewLOCNUTComm::IDD)
{
//	TRACE(_T("CNewLOCNUTComm::CTOR #2()\n"));

	strncpy(m_szIniFile, cIniFile, sizeof(m_szIniFile));
	strncpy(m_pName, name, sizeof(m_pName));
	m_dDlgCloseMilliseconds = iDlgClose;

	m_pCreateButton = NULL;

	m_csTitle = CString("LOCNUT CSO") + CString(" - ");
	m_csTitle += CString(": Create Comm");

	//{{AFX_DATA_INIT(CNewLOCNUTComm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CNewLOCNUTComm::~CNewLOCNUTComm()
{
//	TRACE("CNewLOCNUTComm::DTOR()\n");
	delete m_pCreateButton;
}

BEGIN_MESSAGE_MAP(CNewLOCNUTComm, CDialog)
	//{{AFX_MSG_MAP(CNewLOCNUTComm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CNewLOCNUTComm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewLOCNUTComm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CNewLOCNUTComm::OnOK() 
{
//	TRACE("CNewLOCNUTComm::OnOK()\n");

	char buff2[32];
	CString csText;

	GetDlgItemText(IDC_NAME_EDIT,m_pName,sizeof(m_pName)-1);

	if (m_pName[0] == ';')
	{
		csText = CString("A \"Name\" for a communications object cannot start with \";\".");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//comm objects cannot have the same name
	GetPrivateProfileString(
		COMMUNICATIONS,
		m_pName,"ERROR",buff2,sizeof(buff2)-1,m_szIniFile);

	if (strcmp(buff2,"ERROR")!=0)
	{
		csText = CString("A Communications object by this\nname already exists!\n"\
				"Select a different name.");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//because we are using INI sections to save the information
	//we cannot have the same name as an instrument section either
	GetPrivateProfileString(
		INSTRUMENTS,
		m_pName,"ERROR",buff2,sizeof(buff2)-1,m_szIniFile);

	if (strcmp(buff2,"ERROR")!=0)
	{
		csText = CString("An Instrument object by this\nname already exists!\n"\
				"Select a different name.");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//get the port from the port list
	int port;
	if ((port=GetDlgItemInt(IDC_EDIT_PORT)) == 0)
	{
		csText = CString("A Port Number must be set!");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//get the PC time sync check state
	bool bTimeSync = ((CButton*)GetDlgItem(IDC_CHECK_TIMESYNC))->GetCheck() == 1;

	//entry in the communications for this com object
	WritePrivateProfileString(COMMUNICATIONS,m_pName,LOCNUT_DISCOVERY_TYPE,m_szIniFile);

	CString cTemp;
	GetDlgItemText(IDC_IPADDRESS1,cTemp);
	int i1,i2,i3;
	sscanf(cTemp,"%d.%d.%d",&i1,&i2,&i3);
	cTemp.Format("%d.%d.%d",i1,i2,i3);
	
	WritePrivateProfileString(m_pName,"BASEIP",cTemp,m_szIniFile);

	cTemp.Format("%d",port);
	WritePrivateProfileString(m_pName,"PORT",cTemp,m_szIniFile);
	WritePrivateProfileString(m_pName,"TIMESYNCPC",bTimeSync?"Yes":"No",m_szIniFile);
	WritePrivateProfileString(m_pName,"PASSPHRASE","system passphrase",m_szIniFile);

	CDialog::OnOK();
}

BOOL CNewLOCNUTComm::OnInitDialog() 
{
//	TRACE("CNewLOCNUTComm::OnInitDialog()\n");

	CDialog::OnInitDialog();

	CEdit *pedit = ((CEdit*)GetDlgItem(IDC_NAME_EDIT));;
	pedit->SetWindowText(m_pName);
	pedit->EnableWindow(false);

	char szHostName[128];
	if (gethostname(szHostName, 128) == 0)
	{
		CString cIPAddress;
		struct hostent* pHost;
		pHost = gethostbyname(szHostName);//should change to getaddrinfo
		if ((pHost!= NULL) && (pHost->h_addr_list != NULL))
		{
			cIPAddress.Format("%d.%d.%d.%d",
				(unsigned char)(pHost->h_addr_list[0][0]),
				(unsigned char)(pHost->h_addr_list[0][1]),
				(unsigned char)(pHost->h_addr_list[0][2]),
				(unsigned char)(pHost->h_addr_list[0][3]));
			SetDlgItemText(IDC_IPADDRESS1,cIPAddress);
		}
	}

	SetDlgItemText(IDC_EDIT_PORT,"6000");

	//create the create button
	m_pCreateButton = new CYellowButton("Create",IDOK,m_szIniFile,this);
	m_pCreateButton->EnableWindow(true);

	return TRUE;
}

