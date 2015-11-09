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
#if !defined(AFX_ADDITEM_H__B95DBA81_D4E1_11D3_8FB5_006008A70A6A__INCLUDED_)
#define AFX_ADDITEM_H__B95DBA81_D4E1_11D3_8FB5_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AddItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AddItem dialog

class AddItem : public CDialog
{
// Construction
public:
	AddItem(CWnd* pParent = NULL);   // standard constructor
	void SetUp(CString cSourceDir, BOOL bDoSubDirs, BOOL bDoBulk,
		CString cDestinationDir, CString cFileTypes, char* szIniFile, BOOL bOnlyIfChange = FALSE);
	void SetTitle(CString cTitle){m_cTitle = cTitle;};
	void CheckAll();
	bool m_bGoodSource;
	bool m_bGoodDestination;
	bool m_bGoodFileTypes;
	BOOL m_bOnlyIfChange;
	CString m_cTitle;
	char* m_szIniFile;
// Dialog Data
	//{{AFX_DATA(AddItem)
	enum { IDD = IDD_DIALOG2 };
	BOOL	m_bDoSubDirs;
	CString	m_cSourceDir;
	CString	m_cDestinationDir;
	CString	m_cFileTypes;
	BOOL	m_bDoBulk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AddItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AddItem)
	afx_msg void OnButtonBrowseSource();
	afx_msg void OnButtonBrowseDestination();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSource();
	afx_msg void OnChangeDestination();
	afx_msg void OnChangeFileTypes();
	afx_msg void OnButtonSaveDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDITEM_H__B95DBA81_D4E1_11D3_8FB5_006008A70A6A__INCLUDED_)
