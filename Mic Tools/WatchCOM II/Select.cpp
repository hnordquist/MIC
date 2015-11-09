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
// Select.cpp : implementation file
//

#include "stdafx.h"
#include <afxdisp.h>
#include "WatchCom.h"
#include "Select.h"
#include "dirdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelect dialog


CSelect::CSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelect)
	m_uiTCPPort = 0;
	//}}AFX_DATA_INIT
	m_bDeactivate = false;
}


void CSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelect)
	DDX_Control(pDX, IDC_IPADDRESS1, m_cIPAddress);
	DDX_Control(pDX, IDC_LIST1, m_pListBox);
	DDX_Text(pDX, IDC_EDIT_TCPPORTNUMBER, m_uiTCPPort);
	DDV_MinMaxUInt(pDX, m_uiTCPPort, 1, 65535);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelect, CDialog)
	//{{AFX_MSG_MAP(CSelect)
	ON_EN_CHANGE(IDC_EDIT_LONGNAME, OnChangeEditLongname)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_DEACTIVATE, OnButtonDeactivate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelect message handlers

BOOL CSelect::OnInitDialog() 
{ 
	CDialog::OnInitDialog();

	m_cIPAddress.SetWindowText(m_szIPAddress);
	
	// TODO: Add extra initialization here
	((CButton*)GetDlgItem(IDC_RADIO_BAUD300		))->SetCheck(m_dBaudRate==300?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD600		))->SetCheck(m_dBaudRate==600?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD1200	))->SetCheck(m_dBaudRate==1200?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD1800	))->SetCheck(m_dBaudRate==1800?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD2400	))->SetCheck(m_dBaudRate==2400?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD4800	))->SetCheck(m_dBaudRate==4800?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD7200	))->SetCheck(m_dBaudRate==7200?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD9600	))->SetCheck(m_dBaudRate==9600?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD14400	))->SetCheck(m_dBaudRate==14400?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD19200	))->SetCheck(m_dBaudRate==19200?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD38400	))->SetCheck(m_dBaudRate==38400?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD56000	))->SetCheck(m_dBaudRate==56000?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD57600	))->SetCheck(m_dBaudRate==57600?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD115200	))->SetCheck(m_dBaudRate==115200?1:0);
	((CButton*)GetDlgItem(IDC_RADIO_BAUD128000	))->SetCheck(m_dBaudRate==128000?1:0);

	((CButton*)GetDlgItem(IDC_RADIO15	))->SetCheck(m_dDataBits==5?1:0);
	((CButton*)GetDlgItem(IDC_RADIO16	))->SetCheck(m_dDataBits==6?1:0);
	((CButton*)GetDlgItem(IDC_RADIO17	))->SetCheck(m_dDataBits==7?1:0);
	((CButton*)GetDlgItem(IDC_RADIO18	))->SetCheck(m_dDataBits==8?1:0);
	((CButton*)GetDlgItem(IDC_RADIO19	))->SetCheck(m_dDataBits==16?1:0);

	((CButton*)GetDlgItem(IDC_RADIO20	))->SetCheck(m_dStopBits==1?1:0);
	((CButton*)GetDlgItem(IDC_RADIO21	))->SetCheck(m_dStopBits==15?1:0);
	((CButton*)GetDlgItem(IDC_RADIO22	))->SetCheck(m_dStopBits==2?1:0);

	((CButton*)GetDlgItem(IDC_RADIO23	))->SetCheck(m_dParity==0?1:0);
	((CButton*)GetDlgItem(IDC_RADIO24	))->SetCheck(m_dParity==1?1:0);
	((CButton*)GetDlgItem(IDC_RADIO25	))->SetCheck(m_dParity==2?1:0);
	((CButton*)GetDlgItem(IDC_RADIO26	))->SetCheck(m_dParity==3?1:0);

	//can have 256 comm items (very unlikely) in the form COMxxx
	HKEY HKey;
	DWORD KeyIndex = 0;
	DWORD Type;

	char ValueName[2048];
	char Value[2048];
	DWORD ValueNameSize = 2047;
	DWORD ValueSize = 2047;

	((CListBox*)GetDlgItem(IDC_LIST1))->AddString("TCP/IP");

	//find out what com NT thinks it has 
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\SERIALCOMM",
		0,KEY_READ,&HKey)==ERROR_SUCCESS)
	{
		while (RegEnumValue(HKey, KeyIndex,	
			ValueName, &ValueNameSize, NULL,
			&Type, (unsigned char*)&Value, &ValueSize)
			==ERROR_SUCCESS)
		{
			//and add these com objects iff there isn't a comm
			//object currently using it
			ValueSize = 2047;
			ValueNameSize = 2047;
			KeyIndex++;
			//if not currently used 
			bool used = false;
			//for each comm object check to see if it uses this port
			((CListBox*)GetDlgItem(IDC_LIST1))->AddString(Value);
		}
		RegCloseKey(HKey);
	}
	//if none found the give us some anyway
	if (KeyIndex == 0)
	{
		char temp[8];
		for (int i=0; i<10; i++)
		{
			sprintf(temp,"COM%d",i+1);
			((CListBox*)GetDlgItem(IDC_LIST1))->AddString(temp);
		}
	}
	int iSel = ((CListBox*)GetDlgItem(IDC_LIST1))->FindString(-1,m_szPort);
	if (iSel < 0) iSel = 0;
	((CListBox*)GetDlgItem(IDC_LIST1))->SetCurSel(iSel);

	((CEdit*)GetDlgItem(IDC_DESTINATIONNAME_EDIT))->SetWindowText(m_szDestination);
	((CEdit*)GetDlgItem(IDC_EDIT_LONGNAME))->SetWindowText(m_szFileName);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelect::OnOK() 
{
	// TODO: Add extra validation here
		// get the currently selected baud rate
	if		(((CButton*)GetDlgItem(IDC_RADIO_BAUD300))->GetCheck()==1)
		m_dBaudRate = 300;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD600))->GetCheck()==1)
		m_dBaudRate = 600;
	else if (((CButton*)GetDlgItem(IDC_RADIO_BAUD1200))->GetCheck()==1)
		m_dBaudRate = 1200;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD1800))->GetCheck()==1)
		m_dBaudRate = 1800;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD2400))->GetCheck()==1)
		m_dBaudRate = 2400;
	else if (((CButton*)GetDlgItem(IDC_RADIO_BAUD4800))->GetCheck()==1)
		m_dBaudRate = 4800;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD7200))->GetCheck()==1)
		m_dBaudRate = 7200;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD9600))->GetCheck()==1)
		m_dBaudRate = 9600;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD14400))->GetCheck()==1)
		m_dBaudRate = 14400;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD19200))->GetCheck()==1)
		m_dBaudRate = 19200;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD38400))->GetCheck()==1)
		m_dBaudRate = 38400;
	else if (((CButton*)GetDlgItem(IDC_RADIO_BAUD56000))->GetCheck()==1)
		m_dBaudRate = 56000;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD57600))->GetCheck()==1)
		m_dBaudRate = 57600;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD115200))->GetCheck()==1)
		m_dBaudRate = 115200;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_BAUD128000))->GetCheck()==1)
		m_dBaudRate = 128000;
	else 
		m_dBaudRate = 9600;

	if		(((CButton*)GetDlgItem(IDC_RADIO15))->GetCheck()==1)
		m_dDataBits = 5;
	else if (((CButton*)GetDlgItem(IDC_RADIO16))->GetCheck()==1)
		m_dDataBits = 6;
	else if	(((CButton*)GetDlgItem(IDC_RADIO17))->GetCheck()==1)
		m_dDataBits = 7;
	else if	(((CButton*)GetDlgItem(IDC_RADIO18))->GetCheck()==1)
		m_dDataBits = 8;
	else if	(((CButton*)GetDlgItem(IDC_RADIO19))->GetCheck()==1)
		m_dDataBits = 16;
	else
		m_dDataBits = 8;

	if		(((CButton*)GetDlgItem(IDC_RADIO23))->GetCheck()==1)
		m_dParity = 0;
	else if (((CButton*)GetDlgItem(IDC_RADIO24))->GetCheck()==1)
		m_dParity = 1;
	else if	(((CButton*)GetDlgItem(IDC_RADIO25))->GetCheck()==1)
		m_dParity = 2;
	else if (((CButton*)GetDlgItem(IDC_RADIO26))->GetCheck()==1)
		m_dParity = 3;
	else
		m_dParity = 3;

	if		(((CButton*)GetDlgItem(IDC_RADIO20))->GetCheck()==1)
		m_dStopBits = 1;
	else if (((CButton*)GetDlgItem(IDC_RADIO21))->GetCheck()==1)
		m_dStopBits = 15;
	else if	(((CButton*)GetDlgItem(IDC_RADIO22))->GetCheck()==1)
		m_dStopBits = 2;
	else
		m_dStopBits = 1;

	m_pListBox.GetText(m_pListBox.GetCurSel(),m_szPort);

	GetDlgItemText(IDC_DESTINATIONNAME_EDIT,m_szDestination,sizeof(m_szDestination));
	GetDlgItemText(IDC_EDIT_LONGNAME,m_szFileName,sizeof(m_szDestination));

	CDialog::OnOK();
}

void CSelect::OnChangeEditLongname() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString cResult;
	CString cTemp;
	CString cFormat;
	COleDateTime cNow = COleDateTime::GetCurrentTime();
	GetDlgItemText(IDC_EDIT_LONGNAME,cFormat);
	bool bHaveEscape;
	bHaveEscape = false;
	for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
	{
		if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
			bHaveEscape = true;
		else if (bHaveEscape)
		{
			bHaveEscape = false;
			switch (cFormat[iSourceIndex]) {
			case 'P':
				cResult += "002";
				break;
			case 'y':
				cTemp.Format("%04d",cNow.GetYear());
				cResult += cTemp;
				break;
			case 'm':
				cTemp.Format("%02d",cNow.GetMonth());
				cResult += cTemp;
				break;
			case 'd':
				cTemp.Format("%02d",cNow.GetDay());
				cResult += cTemp;
				break;
			case 'H':
				cTemp.Format("%02d",cNow.GetHour());
				cResult += cTemp;
				break;
			case 'M':
				cTemp.Format("%02d",cNow.GetMinute());
				cResult += cTemp;
				break;
			case 'S':
				cTemp.Format("%02d",cNow.GetSecond());
				cResult += cTemp;
				break;
			default:
				cResult += cFormat[iSourceIndex];
			}
		}
		else
			cResult += cFormat[iSourceIndex];
	}
	if (cResult.FindOneOf("\\/?*:;><\"")>=0)
		SetDlgItemText(IDC_STATIC_EXAMPLE,"DON'T USE character used!");
	else
		SetDlgItemText(IDC_STATIC_EXAMPLE,cResult);	
}

void CSelect::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	CDirDialog DirDialog(this);
	char temp[_MAX_DIR+1];
	GetDlgItem(IDC_DESTINATIONNAME_EDIT)->GetWindowText(temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Default Destination for Copy:";
	if (DirDialog.DoBrowse())
	{
		CString Path(DirDialog.m_strPath);
		GetDlgItem(IDC_DESTINATIONNAME_EDIT)->SetWindowText(Path);
	}
}

BOOL CSelect::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_cIPAddress.GetWindowText(m_szIPAddress,sizeof(m_szIPAddress));

	return CDialog::DestroyWindow();
}

void CSelect::OnButtonDeactivate() 
{
	// TODO: Add your control notification handler code here
	m_bDeactivate = true;
	OnOK();
}
