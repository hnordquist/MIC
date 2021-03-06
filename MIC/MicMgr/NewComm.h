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

#ifndef NEWCOMM_H
#define NEWCOMM_H

// NewComm.h

#include "resource.h"
#include "COM_Utilities.h"
#include "../Interfaces/ICSO.h"
#include <comcat.h>
#include "ccbutton.h"

#define MAX_COMM_NAME_LENGTH  31
#define MAX_COMM_TYPE_LENGTH  31

class CNewComm : public CDialog
{
	DECLARE_DYNCREATE(CNewComm)

// Construction
	CNewComm(); // protected constructor used by dynamic creation
	virtual ~CNewComm();
public:
	CNewComm(CWnd* pParent);

// Dialog Data

	char m_pName[MAX_COMM_NAME_LENGTH + 1];
	char m_pType[MAX_COMM_TYPE_LENGTH + 1];
	bool m_bHaveName;
	bool m_bHaveType;
	int  m_iSelectionIndex;
	ICSO* m_pTemporaryICSO;
	CWnd* m_pParent;
	CString m_csTitle;
	CYellowButton *m_pCreateButton;

	//Dynamic Discovery variables and functions
#define MAX_CLSIDS 64

	ICatInformation* m_pICatInformation;
	ICatRegister*    m_pICatRegister;
	CLSID          m_CLSID[MAX_CLSIDS];
	IClassFactory *m_pClassFactory[MAX_CLSIDS];

	bool TestRegistrationOfComponentCategory();
	void CleanUpTemporaryICSO();
	void GetFriendlyName(const CLSID& clsid, char namestring[], int numchars);
	bool DoDynamicDiscovery(bool bDoGUI = true, CString *pcsOut = NULL);

	//{{AFX_DATA(CNewComm)
	enum { IDD = IDD_NEWCOMM_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewComm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewComm)
	afx_msg void OnChangeCommnameEdit();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeCommtypeList();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCreateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // NEWCOMM_H