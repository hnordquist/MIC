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
//NewInst.cpp
// 

// User: Moore        Date: 12/23/05   Time: 4:43p
// DR00199: Create list picks wrong component.
// Updated in $/mic 2.001/MicMgr
// Fixed the problem of the CSO and ISO create lists getting out of order
// when some of the components cannot be found, even though the registry
// says they are there.
// 

// User: Moore        Date: 12/23/05   Time: 2:04p
// DR00200: Error messages are needed when installation is not as expected.
// Updated in $/mic 2.001/MicMgr
// Added a more detailed description of the error when the registry has
// MIC_ISO or MIC_CSO DLL's registered, but the DLL files are not 
// found where the registry expects them.  Made the usable space on the
// IDD_MESSAGE_DIALOG larger to accomodate the new messages.
// 
 

// NewInst.cpp

#include "stdafx.h"
#include "NewInst.h"
#include "TimedMessageDialog.h"
#include "INI_definitions.h"
#include "MIC_definitions.h"
#include "validate.h"
#include "../Interfaces/CATIDs.h"
#include "MicMgrDlg.h"
#include "globals.h"
#include "colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CNewInst, CDialog)

CNewInst::CNewInst()
{
//	TRACE("CNewInst::CTOR #1()\n");
}

CNewInst::CNewInst(CWnd* pParent)
	: CDialog(CNewInst::IDD, pParent)
{
//	TRACE("CNewInst::CTOR #2()\n");

	m_bHaveName = false;
	m_bHaveType = false;
	m_pName[0] = NULL;
	m_pTemporaryIISO = NULL;
	m_pParent = pParent;
	m_pCreateButton = NULL;

	m_pICatInformation = NULL ;
	m_pICatRegister = NULL ;

	m_csTitle = CString(MIC_NAMESHORT)+CString(": Add ISO");


	for (int i = 0; i < MAX_CLSIDS; i++)
	{
		m_pClassFactory[i] = NULL;
		memset(&m_CLSID[i], 0, sizeof(m_CLSID[i]));
	}

	//{{AFX_DATA_INIT(CNewInst)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CNewInst::~CNewInst()
{
//	TRACE("CNewInst::DTOR\n");

	for (int i = 0; i < MAX_CLSIDS; i++)
	{
		if (NULL != m_pClassFactory[i])
			m_pClassFactory[i]->Release();
	}

	CleanUpTemporaryIISO();

	delete m_pCreateButton;
}

void CNewInst::CleanUpTemporaryIISO()
{
//	TRACE("CNewInst::CleanUpTemporaryIISO()\n");

	if (m_pTemporaryIISO)
	{
		m_pTemporaryIISO->Release();
		m_pTemporaryIISO = NULL;
	}
}

void CNewInst::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewInst)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewInst, CDialog)
	//{{AFX_MSG_MAP(CNewInst)
	ON_EN_CHANGE(IDC_INSTNAME_EDIT, OnChangeInstnameEdit)
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_INSTTYPE_LIST, OnSelchangeInsttypeList)
	ON_BN_CLICKED(IDC_CREATE_NEWINST_BUTTON, OnCreateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewInst message handlers

void CNewInst::OnChangeInstnameEdit() 
{
	//get the change in instrument name and
	//activate the add button if there is something
	//we can use as a name AND an instrument type
	((CEdit*)GetDlgItem(IDC_INSTNAME_EDIT))->GetWindowText(m_pName,MAX_INST_NAME_LENGTH);

	if (strlen(m_pName)>0)
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_INSTNAME_EDIT))->GetSel(posStart,posEnd);
		if (strpbrk(m_pName,INVALIDCHARS))
		{
			((CEdit*)GetDlgItem(IDC_INSTNAME_EDIT))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_INSTNAME_EDIT))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_INSTNAME_EDIT))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_INSTNAME_EDIT))->SetSel(posStart,posEnd);
		((CEdit*)GetDlgItem(IDC_INSTNAME_EDIT))->GetWindowText(m_pName,MAX_INST_NAME_LENGTH);
		if (strlen(m_pName)>0)
			m_bHaveName = true;
		else
			m_bHaveName = false;
	}
	else
		m_bHaveName = false;

	if (m_bHaveName && m_bHaveType)
		m_pCreateButton->EnableWindow(TRUE);
	else
		m_pCreateButton->EnableWindow(FALSE);
}

void CNewInst::OnTimer(UINT nIDEvent) 
{
//	TRACE("CNewInst::OnTimer\n");

	//if time expires just go away
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}

void CNewInst::OnSelchangeInsttypeList() 
{
	// get the instrument type if one is selected

	m_iSelectionIndex = ((CListBox*)GetDlgItem(IDC_INSTTYPE_LIST))->GetCurSel();

	m_bHaveType = !(LB_ERR == m_iSelectionIndex);

	//activate the add button only if we know the type
	//and a name is provided
	if (m_bHaveName && m_bHaveType)
		m_pCreateButton->EnableWindow(TRUE);
}

void CNewInst::GetFriendlyName(const CLSID& clsid, char namestring[], int numchars)	//dgp 24 Oct 2006
{
	//																				//dgp 24 Oct 2006
	// Get friendly name for a CLSID from the Registry.								//dgp 24 Oct 2006
	//																				//dgp 24 Oct 2006
	// Convert clsid to a string.													//dgp 24 Oct 2006
	wchar_t wszCLSID[80];															//dgp 24 Oct 2006
	int r = ::StringFromGUID2(clsid, wszCLSID, 80);									//dgp 24 Oct 2006
																					//dgp 24 Oct 2006
	if (r == 0)																		//dgp 24 Oct 2006
	{																				//dgp 24 Oct 2006
		strcpy(namestring,"UNKNOWN");												//dgp 24 Oct 2006
		return;																		//dgp 24 Oct 2006
	}																				//dgp 24 Oct 2006
																					//dgp 24 Oct 2006
	// Convert to single byte char.													//dgp 24 Oct 2006
	char szCLSID[40] ;																//dgp 24 Oct 2006
	wcstombs(szCLSID, wszCLSID, 40) ;												//dgp 24 Oct 2006
																					//dgp 24 Oct 2006
	// Open the Registry key.														//dgp 24 Oct 2006
	HKEY key = NULL;																//dgp 24 Oct 2006
	long l = ::RegOpenKeyEx(HKEY_CLASSES_ROOT,"CLSID",0,KEY_ALL_ACCESS,&key);		//dgp 24 Oct 2006
	if (l != ERROR_SUCCESS) 														//dgp 24 Oct 2006
	{																				//dgp 24 Oct 2006
		strcpy(namestring,"UNKNOWN");												//dgp 24 Oct 2006
		return;																		//dgp 24 Oct 2006
	}																				//dgp 24 Oct 2006
																					//dgp 24 Oct 2006
	// Get the friendly name.														//dgp 24 Oct 2006
	char szFriendly[256] ;															//dgp 24 Oct 2006
	long size = sizeof(szFriendly);													//dgp 24 Oct 2006
	l = ::RegQueryValue(key,szCLSID,szFriendly,&size);								//dgp 24 Oct 2006
	if (l != ERROR_SUCCESS)															//dgp 24 Oct 2006
	{																				//dgp 24 Oct 2006
		strcpy(namestring,"UNKNOWN");												//dgp 24 Oct 2006
		return;																		//dgp 24 Oct 2006
	}																				//dgp 24 Oct 2006
																					//dgp 24 Oct 2006
	// Output the friendly name.													//dgp 24 Oct 2006
	memcpy(namestring,szFriendly,numchars);											//dgp 24 Oct 2006
																					//dgp 24 Oct 2006
	// Clean up.																	//dgp 24 Oct 2006
	::RegCloseKey(key) ;															//dgp 24 Oct 2006
}																					//dgp 24 Oct 2006

BOOL CNewInst::OnInitDialog() 
{
//	TRACE("CNewInst::OnInitDialog\n");

	CDialog::OnInitDialog();

	/////////////////////////////////////////////////////
	//
	// This is where the "dynamic discovery" happens
	//
	/////////////////////////////////////////////////////

	bool bOk = DoDynamicDiscovery();	//DR00161
	//limit the number of characters the instrument name may contain
	((CEdit*)GetDlgItem(IDC_INSTNAME_EDIT))->SetLimitText(MAX_INST_NAME_LENGTH);
	
	//set the automatic dialog close timer
	SetTimer(1,g_iDlgCloseMillisec,NULL);

	//create the create button
	m_pCreateButton = new CYellowButton(
		"Create...",
		IDC_CREATE_NEWINST_BUTTON,
		g_szIniFile,
		this);

	return TRUE;
}

bool CNewInst::DoDynamicDiscovery(bool bDoGUI, CString* pcsOut)	//DR00161
{
	bool bSoFarSoGood = true;
	HRESULT hr;
	IEnumCLSID* pIEnumCLSID = NULL;
	GUID CATID_ThisCategory[1];
	int iNumberDiscovered = 0;

	// Create the standard COM Category manager.
	if (bSoFarSoGood)
	{
		hr = ::CoCreateInstance(CLSID_StdComponentCategoriesMgr,
										NULL, 
										CLSCTX_ALL,
										IID_ICatInformation, 
										(void**)&m_pICatInformation) ;
		if (FAILED(hr))
		{
			if (bDoGUI)
				AfxMessageBox("Could not create component category manager.") ;
			bSoFarSoGood = false;
		}
	}
	// Get the Category Registration interface.
	if (bSoFarSoGood)
	{
		hr = m_pICatInformation->QueryInterface(IID_ICatRegister,
												(void**)&m_pICatRegister);
		if (FAILED(hr))
		{
			if (bDoGUI)
				AfxMessageBox("Attempt to query for ICatRegister failed.");
			bSoFarSoGood = false;
		}
	}

	//Ensure the MIC instrument category has been established properly.
	if (bSoFarSoGood)
	{
		bool bGoodCategoryRegistration = TestRegistrationOfComponentCategory();
		if (!bGoodCategoryRegistration)
		{
			if (bDoGUI)
				AfxMessageBox("Problem with MIC_ISO component category registration.");
			bSoFarSoGood = false;
		}
	}

	// Get the component CLSID enumerator for the MIC_ISO category
	if (bSoFarSoGood)
	{
		CATID_ThisCategory[0] = MIC_ISO_CATID;
		hr = m_pICatInformation->EnumClassesOfCategories(
			1,CATID_ThisCategory,0,NULL,&pIEnumCLSID);

		if (FAILED(hr))
		{
			if (bDoGUI)
				AfxMessageBox("ListCategoryMembers: EnumClassesOfCategories failed.") ;
			bSoFarSoGood = false;
		}
	}
	// Get the array of CLSID's in this MIC_ISO category
	if (bSoFarSoGood)
	{
		iNumberDiscovered = 0;

		for (int i = 0; i < MAX_CLSIDS; i++)	
		{
			// Get the CLSID of the next component that is a member of this category.
			hr = pIEnumCLSID->Next(	1, &m_CLSID[i], NULL);

			if (hr == S_OK)
			{
				//Get the class factory for this component

				CoGetClassObject(
							m_CLSID[i],//CLSID associated with the class object
							CLSCTX_INPROC_SERVER, //In-process server
							NULL,                 //Instantiate class object on this machine.
							IID_IClassFactory,    //Interface we are looking for
							(LPVOID *)&m_pClassFactory[i]); //pointer to the interface

				//if this class factory fetching failed, process the failure
				if (m_pClassFactory[i] == NULL)
				{
					char namestring[256];													//dgp 24 Oct 2006
					GetFriendlyName(m_CLSID[i], namestring, sizeof(namestring));			//dgp 24 Oct 2006

					memset(&m_CLSID[i], 0, sizeof(CLSID));  //get rid of the bad CLSID
					if (bDoGUI)
					{
						CString cs;
						cs.Format("CoGetClassObject fails for ISO component\nClass index: %d\n"	//dgp 24 Oct 2006
							"Class name: %s\n\n",i,namestring);									//dgp 24 Oct 2006

						cs += m_csRegistrationAnomalyMessage;
	
						new CTimedMessageDialog(
							CString("MIC Setup Anomaly"),
							cs,
							g_iDlgCloseMillisec/1000);  //seconds
					}
				}
				else
				{
					iNumberDiscovered++;
				}
			}	//end if (hr == S_OK)
		} //end for MAX_CLSIDS


		if (0 == iNumberDiscovered)
		{
			if (bDoGUI)
				AfxMessageBox("Found NO component classes registered under category MIC_ISO.");
			bSoFarSoGood = false;
		}
	} //end sofarsogood

	//Instantiate a temporary object from each of the ISO component classes.
	//Get the type string from the object through its IISO interface.
	//Display the type string in the pick list.
	//Destroy the temporary object.
	if (bSoFarSoGood)
	{
		if (bDoGUI)
		{
			BSTR bstrTypeString;
			char szTypeString[32];

			for (int i = 0; i < MAX_CLSIDS; i++)
			{
				ASSERT(m_pTemporaryIISO == NULL);

				if (NULL != m_pClassFactory[i])
				{
					hr = m_pClassFactory[i]->CreateInstance(
						NULL, IID_IISO, (void **)&m_pTemporaryIISO);

					if (NULL != m_pTemporaryIISO)
					{
						hr = m_pTemporaryIISO->get_ISOTypeString(&bstrTypeString);
						Convert_BSTR_to_Character_String(bstrTypeString, szTypeString,32);
						((CListBox*)GetDlgItem(IDC_INSTTYPE_LIST))->AddString(szTypeString);

						CleanUpTemporaryIISO();
					}
				}
			}
		}
		else
		{
			BSTR bstrTypeString,bstrVersionString;
			char szTypeString[32],szVersionString[100];
			CString csrn("\r\n");

			if (pcsOut != NULL)
				pcsOut->Empty();

			for (int i = 0; i < MAX_CLSIDS; i++)
			{
				ASSERT(m_pTemporaryIISO == NULL);

				if (m_pClassFactory[i] != NULL)
				{
					hr = m_pClassFactory[i]->CreateInstance(
						NULL, IID_IISO, (void **)&m_pTemporaryIISO);

					if (m_pTemporaryIISO != NULL)
					{
						hr = m_pTemporaryIISO->get_ISOTypeString(&bstrTypeString);
						Convert_BSTR_to_Character_String(bstrTypeString, szTypeString,32);

						if (pcsOut)
						{
							//CString csTypeString = (CString)szTypeString + CString("\t");
							hr = m_pTemporaryIISO->get_VersionString(&bstrVersionString);
							Convert_BSTR_to_Character_String(bstrVersionString, szVersionString,100);
							//*pcsOut += csTypeString + (CString)szVersionString + csrn;
							int l = 0;
							for (int k = 0; k < (signed)strlen(szVersionString); k++)
							{
								if (szVersionString[k] == ' ')
								{
									szVersionString[k] = '\t';
									l++;
									if (l > 1) break;
								}
							}
							*pcsOut += (CString)szVersionString + csrn;
						}

						CleanUpTemporaryIISO();
					}
				}
			}
		}
	}
	return bSoFarSoGood;
}

void CNewInst::OnOK() 
{
//	TRACE("CNewInst::OnOK()\n");
	KillTimer(1);
	CDialog::OnOK();
}

void CNewInst::OnCreateButton() 
{
//	TRACE("CNewInst::OnCreateButton()\n");

	ASSERT(m_pTemporaryIISO == NULL);

	// NAME must NOT exist as instrument or as a communications object
	//going to do a child dialog box so don't want 
	//to time out until it returns
	KillTimer(1);

	char *at;
	while ((at = strpbrk(m_pName,INVALIDCHARS)) != NULL)
		at[0] = '-';
	SetDlgItemText(IDC_INSTNAME_EDIT,m_pName);

	char temp[32];
	//check if an instrument by this name already exists
	GetPrivateProfileString(
		INSTRUMENTS,m_pName,"NO_MATCH",temp,sizeof(temp)-1,g_szIniFile);

	if (strcmp(temp,"NO_MATCH")!=0)
	{
		CString csText("An Instrument object with this\nname already exists.\nSelect a different name.");
		new CTimedMessageDialog(m_csTitle, csText,g_iDlgCloseMillisec/1000, this);
		SetTimer(1,g_iDlgCloseMillisec,NULL);
		m_pCreateButton->EnableWindow(FALSE);
		return;
	}

	//check if a communications object by this name already exists
	GetPrivateProfileString(
		COMMUNICATIONS,m_pName,"NO_MATCH",temp,sizeof(temp)-1,g_szIniFile);

	if (strcmp(temp,"NO_MATCH")!=0)
	{
		CString csText("A Communications object with this\nname already exists.\nSelect a different name.");
		new CTimedMessageDialog(m_csTitle, csText,g_iDlgCloseMillisec/1000,this);
		SetTimer(1,g_iDlgCloseMillisec,NULL);
		m_pCreateButton->EnableWindow(FALSE);
		return;
	}

	GetDlgItemText(IDC_INSTNAME_EDIT,temp,31);
	if (temp[0] == ';')
	{
		CString csText("A \"Name\" for an instrument object cannot start with \";\".");
		new CTimedMessageDialog(m_csTitle,csText,g_iDlgCloseMillisec/1000,this);
		SetTimer(1,g_iDlgCloseMillisec,NULL);
		m_pCreateButton->EnableWindow(FALSE);
		return;
	}

	// Pick the class factory from the index out of the picklist.
	// Recall that some of the class factory pointers might be NULL.
	// Also establish the CLSID for this component type to put in the INI file later.
	IClassFactory *pFactory = NULL;
	CLSID this_CLSID;
	memset(&this_CLSID,0,sizeof(CLSID));

	int j = -1;

	for (int i = 0; i < MAX_CLSIDS; i++)
	{
		if (m_pClassFactory[i] != NULL)
			j++;
		if (j == m_iSelectionIndex)
		{
			pFactory = m_pClassFactory[i];
			memcpy(&this_CLSID, &m_CLSID[i], sizeof(CLSID));
			break;
		}
	}
	//Instantiate a short-lived ISO component
	//and have it gather its own data from its own version of
	//CModParam.

	if (pFactory == NULL)
		return;

	HRESULT hr = pFactory->CreateInstance(
		NULL, 
		IID_IISO,
		(void **)&m_pTemporaryIISO);

	if (FAILED(hr) || (m_pTemporaryIISO == NULL))
		return;

	//Tell it to collect its startup parameters
	//And put them in the INI file
	BSTR bstr;
	Convert_Character_String_to_BSTR(g_szIniFile,bstr);
	m_pTemporaryIISO->put_INIPathname(bstr);
	Convert_Character_String_to_BSTR(m_pName, bstr);
	m_pTemporaryIISO->put_Name(bstr);
	m_pTemporaryIISO->put_DialogAutoCloseTime((ULONG)(g_iDlgCloseMillisec/1000));

	//With the exception of its CLSID, the 
	//temporary ISO writes its own data to the INI file.  The content of this
	//data would vary according to the requirements of the ISO.

	//The ISO is allowed to change the name.  The output csISONameOut is used to capture
	//that changed name.  We have allowed this because the CSO (for IPX) does change its name
	//and retaining the parallelism between NewComm and NewInst is a good thing.  To date, none
	//of the existing ISO types change their names.

	CString csNameOut("NONAME");
	BSTR bstrNameOut;
	VARIANT_BOOL vbHaveNewISO;
	WCHAR *pwide = NULL;
	char narrow[80];

	m_pTemporaryIISO->CollectParametersForNewISO(&bstrNameOut,&vbHaveNewISO);

	if (vbHaveNewISO == VARIANT_TRUE)  //a new ISO was requested
	{
		if (0 != SysStringLen(bstrNameOut))
		{
			Convert_BSTR_to_CString(bstrNameOut, csNameOut);

			//char narrow[80];
			::StringFromCLSID(this_CLSID,&pwide);
			::wcstombs(narrow,pwide,sizeof(narrow));
			CoTaskMemFree(pwide);
			pwide = NULL;
		}

		CValidate Valid(this);

		if (IDOK == Valid.DoModal())
		{
			//put the new instrument's class id into the ini file
			WritePrivateProfileString(csNameOut,"CLSID",narrow,g_szIniFile);

			//send message to MICDlg with name of new ISO so MicMgrDlg can look in the INI file
			UINT ubytes = 1 + csNameOut.GetLength();
			unsigned char *cobuffer = (unsigned char*)CoTaskMemAlloc((ULONG)ubytes);

			if (cobuffer != 0)
			{
				memcpy(cobuffer,(void *)((LPCTSTR)csNameOut),(unsigned int)ubytes);
				::PostMessage(AfxGetMainWnd()->m_hWnd,IDC_CREATE_INST,WPARAM(ubytes),LPARAM(cobuffer));
			}

			//Disable the create button since we have used the name that is in the comm name edit box.
			m_pCreateButton->EnableWindow(FALSE);
		}
		else //validate failed
		{
			//remove the non-validated name from the INI file
			WritePrivateProfileString(INSTRUMENTS,csNameOut,NULL,g_szIniFile);
		}
	}

	if (pwide != NULL)
		CoTaskMemFree(pwide);

	// destroy the temporary component
	CleanUpTemporaryIISO();

	//restart the timeout timer
	SetTimer(1,g_iDlgCloseMillisec,NULL);

	//get rid of the ugly focus indicator on the Create button.
	GetDlgItem(IDOK)->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
//
//                  TestRegistrationOfComponentCategory()
//
/////////////////////////////////////////////////////////////////////////////
bool CNewInst::TestRegistrationOfComponentCategory()
{
//	TRACE("CNewInst::TestRegistrationOfComponentCategory()\n");

	bool bGoodRegistration = false;
	IEnumCATEGORYINFO* pIEnumCATEGORYINFO = NULL ; 

	// Get an enumerator for the categories.
	HRESULT hr = m_pICatInformation->EnumCategories(::GetUserDefaultLCID(),
	                                                &pIEnumCATEGORYINFO) ;
	if (FAILED(hr))
	{
		AfxMessageBox("ListCategories: ICatInformation::EnumCategories failed.");
	}
	else
	{
		char szDescription[128] ;
		CATEGORYINFO CategoryInfo ;

		// Enumerate the categories.
		while (S_OK == (hr = pIEnumCATEGORYINFO->Next(1, &CategoryInfo, NULL)))
		{
			::wcstombs(szDescription, CategoryInfo.szDescription,sizeof(szDescription));

			if (0 == strcmp(szDescription,MIC_ISO_CATID_FRIENDLY_NAME))
			{
				bGoodRegistration = true;
				break;
			}
		} 

		if (FAILED(hr))	// Did Next fail?
		{
			AfxMessageBox("ListCategories: IEnumCATEGORYINFO::Next failed.", hr) ;
			bGoodRegistration = false;
		}
	}
	if (pIEnumCATEGORYINFO != NULL)
		pIEnumCATEGORYINFO->Release() ;

	return bGoodRegistration;
}
