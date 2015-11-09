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
// SyncDlg.h : header file
//

#if !defined(AFX_SYNCDLG_H__BB7D61A6_7350_11D2_94AB_EE5FE0791316__INCLUDED_)
#define AFX_SYNCDLG_H__BB7D61A6_7350_11D2_94AB_EE5FE0791316__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SystemTray.h"

#define NAME		"DelFi"
#define CONFIG		"CONFIGURATION"
#define AGE			"AGE"
#define INDIRECTORY	"INDIRECTORY"
#define DESTINATION	"DESTINATION"
#define CYCLETIME	"CYCLETIME"
#define MAXPAUSE	"MAXPAUSE"
#define SHOW		"SHOW"
#define CYCLETIMER	10000
#define INI_TIMER	20000
#define TIMER_MESSAGE (WM_USER+6)
#define HIDEDISP	  (WM_USER+7)

/////////////////////////////////////////////////////////////////////////////
// CSyncDlg dialog

class CSyncDlg : public CDialog
{
// Construction
public:
	CSyncDlg(CWnd* pParent = NULL);	// standard constructor

	char m_cCommandLine[_MAX_PATH];

// Dialog Data
	//{{AFX_DATA(CSyncDlg)
	enum { IDD = IDD_SYNC_DIALOG };
	CButton	m_pConfigButton;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	char	m_szIniFile[_MAX_DIR];
	bool	m_bPause;
	int		m_dPauseCnt;
	int		m_dMaxPause;
	bool	m_bShow;
	bool	m_bNewIni;
	COleDateTimeSpan m_Age;
	COleDateTimeSpan dCycleTime;
	CSystemTray m_SystemTray;
	// Generated message map functions
	//{{AFX_MSG(CSyncDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPause();
	afx_msg void OnButton2();
	afx_msg void OnButtonConfigure();
	afx_msg void OnButtonHidenow();
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void MsgFromTray(WPARAM WParam, LPARAM LParam);
	afx_msg void DoWork(WPARAM WParam, LPARAM LParam);
	void DoShow(bool);
	void CheckFiles(char *InDirectory, COleDateTimeSpan Age, COleDateTime TimeNow,
						  bool bConsiderArchiveBit, bool bDoSubDirectories);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCDLG_H__BB7D61A6_7350_11D2_94AB_EE5FE0791316__INCLUDED_)
