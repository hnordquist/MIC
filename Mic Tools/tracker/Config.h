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
#if !defined(AFX_CONFIG_H__0B7423B1_5591_11D3_8F77_006008A70A6A__INCLUDED_)
#define AFX_CONFIG_H__0B7423B1_5591_11D3_8F77_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Config.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfig dialog
class CTrackerDlg;
class CConfig : public CDialog
{
// Construction
public:
	CConfig(char *cIniFile, CWnd* pParent = NULL);   // standard constructor
	CTrackerDlg* m_pParent;
	int		m_bDoUDP;
	char m_cIniFile[MAX_PATH];
// Dialog Data
	//{{AFX_DATA(CConfig)
	enum { IDD = IDD_DIALOG_CONFIG };
	CButton	m_cCheckSendStatus;
	CComboBox	m_cPushXMLCombo;
	CComboBox	m_cPushCombo;
	BOOL	m_bDoHTML;
	CString	m_sName;
	CString	m_szMailSlotName;
	int		m_dMSTime;
	int		m_dUDPPort;
	int		m_dUDP1;
	int		m_dUDP2;
	int		m_dUDP3;
	int		m_dUDP4;
	int		m_dHTMLTime;
	CString	m_szHTMLName;
	BOOL	m_bAllowForward;
	CString	m_cLogName;
	BOOL	m_bDoAlarm;
	//}}AFX_DATA
	bool	m_bToMIC;
	bool	m_bAdvanced;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfig)
	virtual void OnOK();
	afx_msg void OnCheckDoHTML();
	afx_msg void OnRadioDoMailSlot();
	afx_msg void OnRadioDoUDP();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAutoforward();
	afx_msg void OnRadioToMIC();
	afx_msg void OnRadioToTRACKER();
	afx_msg void OnButtonPushadd();
	afx_msg void OnButtonPushdelete();
	afx_msg void OnButtonPushxmladd();
	afx_msg void OnButtonPushxmldelete();
	afx_msg void OnButtonAdvanced();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIG_H__0B7423B1_5591_11D3_8F77_006008A70A6A__INCLUDED_)
