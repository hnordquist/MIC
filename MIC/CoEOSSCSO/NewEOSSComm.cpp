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
// NewEOSSComm.cpp : implementation file
//

#include "stdafx.h"
#include "INI_definitions.h"
#include "NewEOSSComm.h"
#include "CSO_definitions.h"
#include "TimedMessageDialog.h"
#include "ComEOSS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                 Constructors and Destructors
//
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CNewEOSSComm, CDialog)

CNewEOSSComm::CNewEOSSComm():CDialog(CNewEOSSComm::IDD)
{
//	TRACE("CNewEOSSComm::CTOR #1()\n");
}
CNewEOSSComm::CNewEOSSComm(char* cIniFile,int iDlgClose,char *name)
	: CDialog(CNewEOSSComm::IDD)
{
//	TRACE(_T("CNewEOSSComm::CTOR #2()\n"));

	strncpy(m_szIniFile, cIniFile, sizeof(m_szIniFile));
	strncpy(m_pName, name, sizeof(m_pName));
	m_dDlgCloseMilliseconds = iDlgClose;

	m_pCreateButton = NULL;

	m_dCurSel_Serial = -1;

	m_csTitle = CString("EOSS CSO") + CString(" - ");
	m_csTitle += CString(": Create Comm");

	//{{AFX_DATA_INIT(CNewEOSSComm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CNewEOSSComm::~CNewEOSSComm()
{
//	TRACE("CNewEOSSComm::DTOR()\n");
	delete m_pCreateButton;
}

BEGIN_MESSAGE_MAP(CNewEOSSComm, CDialog)
	//{{AFX_MSG_MAP(CNewEOSSComm)
	ON_LBN_SELCHANGE(IDC_PORT_LIST, OnSelchangePortList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CNewEOSSComm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewEOSSComm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


void CNewEOSSComm::OnOK() 
{
//	TRACE("CNewEOSSComm::OnOK()\n");

	char buff2[32];
	CString csText;

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
	if ((port=((CListBox*)GetDlgItem(IDC_PORT_LIST))->GetCurSel())== LB_ERR)
	{
		csText = CString("A Communications PORT must be selected!  ");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}


	ShowWindow(SW_SHOW);  //<<QA>> temporary until fixed below.

	//save all to ini file
	//entry in the communications for this com object
	WritePrivateProfileString(COMMUNICATIONS,m_pName,EOSS_DISCOVERY_TYPE,m_szIniFile);
	((CListBox*)GetDlgItem(IDC_PORT_LIST))->GetText(port,buff2);
	WritePrivateProfileString(m_pName,PORT,buff2,m_szIniFile);
		
	CDialog::OnOK();
}

BOOL CNewEOSSComm::OnInitDialog() 
{
//	TRACE("CNewEOSSComm::OnInitDialog()\n");

	CDialog::OnInitDialog();

	PopulateSerialPortList();

	CEdit *pedit = ((CEdit*)GetDlgItem(IDC_NAME_EDIT));;
	pedit->SetWindowText(m_pName);
	pedit->EnableWindow(false);

	//create the create button
	m_pCreateButton = new CYellowButton("Create",IDOK,m_szIniFile,this);

	return TRUE;
}

void CNewEOSSComm::PopulateSerialPortList()
{
//	TRACE("CNewEOSSComm::PopulateSerialPortList()\n");

	CListBox *plistbox = (CListBox*)GetDlgItem(IDC_PORT_LIST);

	HKEY HKey;
	DWORD KeyIndex = 0;
	DWORD Type;

	//can have 256 comm items (very unlikely) in the form COMxxx
	char ValueName[2048];
	char Value[2048];
	DWORD ValueNameSize = 2047;
	DWORD ValueSize = 2047;
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
			char buff[256];
			if (GetPrivateProfileSection(COMMUNICATIONS,buff,sizeof(buff), m_szIniFile)>0)
			{
				char *at = buff;				//char pointer to step through buff

		 		//step through list of communications using a pointer into buff
				while (strlen(at) > 0) 
				{
					//getting instrument names from INI file so strip off extra stuff
					CString At(at);
					At.TrimRight();
					char temp[32];
					char buff2[32];
					strcpy(temp,At.SpanExcluding((LPCTSTR)"="));
					if (strlen(temp)>0) 
					{
						GetPrivateProfileString(temp,"PORT","",buff2,sizeof(buff2),m_szIniFile);
						if (strcmp(buff2,Value)==0)
							used = true;
					}
   				at = at + strlen(at)+1;
				}
			}
			if (!used)
			{
				CString cTemp;
				cTemp = "\\\\.\\";
				cTemp += Value;
				FILE *ftest = fopen(cTemp,"wt");
				if (ftest)
				{
					plistbox->AddString(Value);
					fclose(ftest);
				}
//this solution takes nearly twice as long as the fopen/fclose 
//				CString cTemp;
//				cTemp = "\\\\.\\";
//				cTemp += Value;
//				HANDLE h = CreateFile(cTemp,0 | GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//				if (h != INVALID_HANDLE_VALUE)
//				{
//					plistbox->AddString(Value);
//					CloseHandle(h);
//				}
			}
		}
		RegCloseKey(HKey);
	}
}

void CNewEOSSComm::OnSelchangePortList() 
{
//	TRACE("CNewEOSSComm::OnSelchangePortList()\n");
	// user clicked on one of the port selections in the list box
	// so find out which is the current selection

	CListBox *plistbox = (CListBox*)GetDlgItem(IDC_PORT_LIST);
	int cursel = plistbox->GetCurSel();

	if (cursel == LB_ERR)
		m_pCreateButton->EnableWindow(FALSE);
	else
		m_pCreateButton->EnableWindow(TRUE);

	m_dCurSel_Serial = cursel;
}


