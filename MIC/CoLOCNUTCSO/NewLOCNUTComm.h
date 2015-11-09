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
//$History: NewLOCNUTComm.h $
// * 
// * *****************  Version 2  *****************
// * User: Pelowitz     Date: 4/29/08    Time: 2:14p
// * Updated in $/MIC 2004/CoLOCNUTCSO
// * final check in prior to retirement
// * 
// * *****************  Version 1  *****************
// * User: Pelowitz     Date: 3/12/08    Time: 2:52p
// * Created in $/MIC 2004/CoLOCNUTCSO
// * At or near retirement (EOSS still being tested)
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
// User: Moore        Date: 5/03/05    Time: 2:11p
// Created in $/MIC 2.0/CoLOCNUTCSO
// Added CoLOCNUTCSO to workspace.
// 

#ifndef NEWLOCNUTCOMM_H
#define NEWLOCNUTCOMM_H

// NewLOCNUTComm.h : header file

#include "resource.h"
#include "ccbutton.h"

/////////////////////////////////////////////////////////////////////////////
// CNewLOCNUTComm dialog

class CNewLOCNUTComm : public CDialog
{
	DECLARE_DYNCREATE(CNewLOCNUTComm)

// Construction
public:
	CNewLOCNUTComm(char *cIniFile, int iDlgClose, char *name);
	CNewLOCNUTComm();
	virtual ~CNewLOCNUTComm();

// Dialog Data
	//{{AFX_DATA(CNewLOCNUTComm)
	enum { IDD = IDD_LOCNUT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewLOCNUTComm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	CYellowButton *m_pCreateButton;

	int  m_dDlgCloseMilliseconds;

	char m_pName[128];
	char m_szIniFile[_MAX_PATH];

	CString m_csTitle;

	// Generated message map functions
	//{{AFX_MSG(CNewLOCNUTComm)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // NEWLOCNUTCOMM_H
