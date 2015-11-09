/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-040. This software was exported from the United States in accordance with the Export
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
// Config.cpp : implementation file
//

#include "stdafx.h"
#include "Tracker.h"
#include "Config.h"
#include "TrackerDlg.h"
#include "GetPortNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfig dialog


CConfig::CConfig(char * cIniFile, CWnd* pParent /*=NULL*/)
	: CDialog(CConfig::IDD, pParent)
{
	m_pParent = (CTrackerDlg*)pParent;
	strcpy_s(m_cIniFile,cIniFile);
	m_bAdvanced = false;
	//{{AFX_DATA_INIT(CConfig)
	m_bDoHTML = FALSE;
	m_sName = _T("");
	m_szMailSlotName = _T("");
	m_dMSTime = 0;
	m_dUDPPort = 0;
	m_dUDP1 = 0;
	m_dUDP2 = 0;
	m_dUDP3 = 0;
	m_dUDP4 = 0;
	m_dHTMLTime = 0;
	m_szHTMLName = _T("");
	m_bAllowForward = FALSE;
	m_cLogName = _T("");
	m_bDoAlarm = FALSE;
	//}}AFX_DATA_INIT
}


void CConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfig)
	DDX_Control(pDX, IDC_CHECK_SENDSTATUS, m_cCheckSendStatus);
	DDX_Control(pDX, IDC_COMBO_PUSHXML, m_cPushXMLCombo);
	DDX_Control(pDX, IDC_COMBO_PUSH, m_cPushCombo);
	DDX_Check(pDX, IDC_CHECK1, m_bDoHTML);
	DDX_Text(pDX, IDC_EDIT1, m_sName);
	DDX_Text(pDX, IDC_EDIT2, m_szMailSlotName);
	DDX_Text(pDX, IDC_EDIT5, m_dMSTime);
	DDX_Text(pDX, IDC_EDIT10, m_dUDPPort);
	DDX_Text(pDX, IDC_EDIT6, m_dUDP1);
	DDX_Text(pDX, IDC_EDIT7, m_dUDP2);
	DDX_Text(pDX, IDC_EDIT8, m_dUDP3);
	DDX_Text(pDX, IDC_EDIT9, m_dUDP4);
	DDX_Text(pDX, IDC_EDIT3, m_dHTMLTime);
	DDX_Text(pDX, IDC_EDIT4, m_szHTMLName);
	DDX_Check(pDX, IDC_CHECK_AUTOFORWARD, m_bAllowForward);
	DDX_Text(pDX, IDC_EDIT11, m_cLogName);
	DDX_Check(pDX, IDC_CHECK_AUDIO, m_bDoAlarm);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfig, CDialog)
	//{{AFX_MSG_MAP(CConfig)
	ON_BN_CLICKED(IDC_CHECK1, OnCheckDoHTML)
	ON_BN_CLICKED(IDC_RADIO1, OnRadioDoMailSlot)
	ON_BN_CLICKED(IDC_RADIO2, OnRadioDoUDP)
	ON_BN_CLICKED(IDC_CHECK_AUTOFORWARD, OnCheckAutoforward)
	ON_BN_CLICKED(IDC_RADIO3, OnRadioToMIC)
	ON_BN_CLICKED(IDC_RADIO4, OnRadioToTRACKER)
	ON_BN_CLICKED(IDC_BUTTON_PUSHADD, OnButtonPushadd)
	ON_BN_CLICKED(IDC_BUTTON_PUSHDELETE, OnButtonPushdelete)
	ON_BN_CLICKED(IDC_BUTTON_PUSHXMLADD, OnButtonPushxmladd)
	ON_BN_CLICKED(IDC_BUTTON_PUSHXMLDELETE, OnButtonPushxmldelete)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED, OnButtonAdvanced)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfig message handlers

void CConfig::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	int iIndex;
	WritePrivateProfileString("PUSH",NULL,NULL,m_cIniFile);
	for (iIndex = 0; iIndex < m_cPushCombo.GetCount(); iIndex++)
	{
		CString cString;
		char szI[8];
		sprintf_s(szI,"%d",iIndex);
		m_cPushCombo.GetLBText(iIndex,cString);
		WritePrivateProfileString("PUSH",szI,cString,m_cIniFile);
	}
	
	char szName1[32];
	char szName2[32];
	GetPrivateProfileString(  PUSH_XML,"NAME1","LANL",szName1,sizeof(szName1),m_cIniFile);
	GetPrivateProfileString(  PUSH_XML,"NAME2","N-4",szName2,sizeof(szName2),m_cIniFile);
	WritePrivateProfileString(PUSH_XML,NULL,NULL,m_cIniFile);
	WritePrivateProfileString(PUSH_XML,"SENDSTATUS",(m_cCheckSendStatus.GetCheck()==1)?"1":"0",m_cIniFile);
	WritePrivateProfileString(PUSH_XML,"NAME1",szName1,m_cIniFile);
	WritePrivateProfileString(PUSH_XML,"NAME2",szName2,m_cIniFile);
	for (iIndex = 0; iIndex < m_cPushXMLCombo.GetCount(); iIndex++)
	{
		CString cString;
		char szI[32];
		char szPort[16];
		sprintf_s(szI,"A%d",iIndex);
		m_cPushXMLCombo.GetLBText(iIndex,cString);
		int iPort = m_cPushXMLCombo.GetItemData(iIndex);
		WritePrivateProfileString(PUSH_XML,szI,cString,m_cIniFile);
		sprintf_s(szI,"P%d",iIndex);
		sprintf_s(szPort,"%d",iPort);
		WritePrivateProfileString(PUSH_XML,szI,szPort,m_cIniFile);
	}

	CDialog::OnOK();
}

void CConfig::OnCheckDoHTML() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	GetDlgItem(IDC_SPIN1)->EnableWindow(m_bDoHTML);
	GetDlgItem(IDC_EDIT3)->EnableWindow(m_bDoHTML);
	GetDlgItem(IDC_EDIT4)->EnableWindow(m_bDoHTML);
}

void CConfig::OnRadioDoMailSlot() 
{
	// TODO: Add your control notification handler code here
	m_bDoUDP = false;
	GetDlgItem(IDC_STATIC_UDP1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_UDP2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_UDP3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT6)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT7)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT8)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT9)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT10)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SPIN3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO4)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_MS1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_MS2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT5)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SPIN2)->ShowWindow(SW_SHOW);
}

void CConfig::OnRadioDoUDP() 
{
	// TODO: Add your control notification handler code here
	m_bDoUDP = true;
	GetDlgItem(IDC_STATIC_MS1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_MS2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SPIN2)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_UDP1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_UDP2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_UDP3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RADIO3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RADIO4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT6)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT7)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT8)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT9)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT10)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SPIN3)->ShowWindow(SW_SHOW);
}

BOOL CConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	//allow forward of messages
	if (m_pParent->m_bAllowForward)
		((CButton*)GetDlgItem(IDC_CHECK_AUTOFORWARD))->SetCheck(1);
	else
		((CButton*)GetDlgItem(IDC_CHECK_AUTOFORWARD))->SetCheck(0);

	if (m_pParent->m_bToMIC)
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(0);
		m_bToMIC = true;
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(1);
		m_bToMIC = false;
	}


	//are receiving via mail slot
	if (m_pParent->m_bDoingMailSlot)
	{
		OnRadioDoMailSlot();
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	}
	else
	{
		OnRadioDoUDP();
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);
	}

	SetDlgItemText(IDC_EDIT1,m_pParent->m_cName);

	m_bDoHTML = m_pParent->m_bDoHTML;
	if (m_bDoHTML)
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(1);
	OnCheckDoHTML();

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetRange(10,9999);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetPos(m_pParent->m_dHTMLTime);
	SetDlgItemText(IDC_EDIT4,m_pParent->m_szHTMLName);

	SetDlgItemInt(IDC_EDIT6,m_pParent->m_b1);
	SetDlgItemInt(IDC_EDIT7,m_pParent->m_b2);
	SetDlgItemInt(IDC_EDIT8,m_pParent->m_b3);
	SetDlgItemInt(IDC_EDIT9,m_pParent->m_b4);

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN3))->SetRange(1,9999);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN3))->SetPos(m_pParent->m_dDefPort);

	SetDlgItemText(IDC_EDIT2,m_pParent->m_szSlotName);

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->SetRange(1,9999);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->SetPos(m_pParent->m_dMSTime);

	SetDlgItemText(IDC_EDIT11,m_pParent->m_cLogFileName);

	int i = 0;
	char szI[8];
	char szPort[8];
	char szForwardAddress[32];
	_itoa_s(i,szI,10);
	szForwardAddress[0] = NULL;
	GetPrivateProfileString("PUSH",szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
	while (strlen(szForwardAddress))
	{
		m_cPushCombo.AddString(szForwardAddress);
	
		_itoa_s(++i,szI,10);
		szForwardAddress[0] = NULL;
		GetPrivateProfileString("PUSH",szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
	}

	bool bSendStatus = GetPrivateProfileInt(PUSH_XML,"SENDSTATUS",0,m_cIniFile)==1;
	m_cCheckSendStatus.SetCheck((bSendStatus)?1:0);

	//GetPrivateProfileString(PUSH_XML,"SENDSTATUS",
	i = 0;
	sprintf_s(szI,"A%d",i);
	szForwardAddress[0] = NULL;
	GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
	while (strlen(szForwardAddress))
	{

		sprintf_s(szPort,"P%d",i);
		int iPort = GetPrivateProfileInt(PUSH_XML,szPort,0,m_cIniFile);
		int iIndex = m_cPushXMLCombo.AddString(szForwardAddress);
		m_cPushXMLCombo.SetItemData(iIndex,iPort);
	
		sprintf_s(szI,"A%d",++i);
		szForwardAddress[0] = NULL;
		GetPrivateProfileString(PUSH_XML,szI,"",szForwardAddress,sizeof(szForwardAddress),m_cIniFile);
	}

	if (m_pParent->m_bDoAlarm)
		((CButton*)GetDlgItem(IDC_CHECK_AUDIO))->SetCheck(1);

	OnButtonAdvanced();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfig::OnCheckAutoforward() 
{
	// TODO: Add your control notification handler code here
	m_bAllowForward = ((CButton*)GetDlgItem(IDC_CHECK_AUTOFORWARD))->GetCheck()==1;
}

void CConfig::OnRadioToMIC() 
{
	// TODO: Add your control notification handler code here
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(0);
		m_bToMIC = true;
}

void CConfig::OnRadioToTRACKER() 
{
	// TODO: Add your control notification handler code here
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(1);
		m_bToMIC = false;
}

void CConfig::OnButtonPushadd() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CString cNewString;
	m_cPushCombo.GetWindowText(cNewString);
	if (cNewString.GetLength())
	{
		if (m_cPushCombo.FindStringExact(-1,cNewString) == LB_ERR)
			m_cPushCombo.AddString(cNewString);
	}
}

void CConfig::OnButtonPushdelete() 
{
	// TODO: Add your control notification handler code here
	CString cNewString;
	m_cPushCombo.GetWindowText(cNewString);
	if (cNewString.GetLength())
	{
		int iIndex;
		if ((iIndex = m_cPushCombo.FindStringExact(-1,cNewString)) != LB_ERR)
		{
			m_cPushCombo.DeleteString(iIndex);
			m_cPushCombo.SetWindowText("");
		}
		else
		{
			int iSelected = m_cPushCombo.GetCurSel();
			if (iSelected != LB_ERR)
			{
				m_cPushCombo.DeleteString(iIndex);
				m_cPushCombo.InsertString(iIndex,cNewString);
			}

		}
	}

}

void CConfig::OnButtonPushxmladd() 
{
	// TODO: Add your control notification handler code here
	CString cNewString;
	m_cPushXMLCombo.GetWindowText(cNewString);
	if (cNewString.GetLength())
	{
		int iIndex;
		int iPort = 0;
		//get the new port number
		if ((iIndex = m_cPushXMLCombo.FindStringExact(-1,cNewString)) != LB_ERR)
			iPort = m_cPushXMLCombo.GetItemData(iIndex);
		CGetPortNumber cGetPortNumber;
		cGetPortNumber.m_cStaticAddress = cNewString;
		cGetPortNumber.m_uPort = iPort;
		if (cGetPortNumber.DoModal()==IDOK)
		{
			//add the two to the list box
			if ((iIndex = m_cPushXMLCombo.FindStringExact(-1,cNewString)) == LB_ERR)
				iIndex = m_cPushXMLCombo.AddString(cNewString);
			m_cPushXMLCombo.SetItemData(iIndex,cGetPortNumber.m_uPort);
		}
	}
}

void CConfig::OnButtonPushxmldelete() 
{
	// TODO: Add your control notification handler code here
	CString cNewString;
	m_cPushXMLCombo.GetWindowText(cNewString);
	if (cNewString.GetLength())
	{
		int iIndex;
		if ((iIndex = m_cPushXMLCombo.FindStringExact(-1,cNewString)) != LB_ERR)
		{
			m_cPushXMLCombo.DeleteString(iIndex);
			m_cPushXMLCombo.SetWindowText("");
		}
	}
	
}

void CConfig::OnButtonAdvanced() 
{
	// TODO: Add your control notification handler code here
	CRect cRect;
	CRect cRightEdge;
	CRect cRightEdge2;
	GetWindowRect(&cRect);
	GetDlgItem(IDC_STATIC_RIGHT)->GetWindowRect(&cRightEdge);
	GetDlgItem(IDC_STATIC_RIGHT2)->GetWindowRect(&cRightEdge2);
	if (m_bAdvanced)
	{
		SetDlgItemText(IDC_BUTTON_ADVANCED,"<<");
		cRect.right=cRightEdge2.left;
		MoveWindow(cRect);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_ADVANCED,">>");
		cRect.right=cRightEdge.left;
		MoveWindow(cRect);
	}
	m_bAdvanced = !m_bAdvanced;
}
