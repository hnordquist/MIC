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
// WinDumpDlg.h : header file
//

#if !defined(AFX_WINDUMPDLG_H__64BAAE51_B8C8_4305_90CB_13909A65E55B__INCLUDED_)
#define AFX_WINDUMPDLG_H__64BAAE51_B8C8_4305_90CB_13909A65E55B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWinDumpDlgAutoProxy;

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlg dialog

class CWinDumpDlg : public CDialog
{
	DECLARE_DYNAMIC(CWinDumpDlg);
	friend class CWinDumpDlgAutoProxy;

// Construction
public:
	CWinDumpDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CWinDumpDlg();
	char m_szCmdLine[MAX_PATH*2];
	CString m_cMessage;

// Dialog Data
	//{{AFX_DATA(CWinDumpDlg)
	enum { IDD = IDD_WINDUMP_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinDumpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWinDumpDlgAutoProxy* m_pAutoProxy;
	enum Types {CHN,GRAND,MCA,ISR,JSR,PSR,VACOSS,BINARY,GPS,UNKNOWN} eThisType;
	HICON m_hIcon;
	BOOL CanExit();
	void DoIt(char* FileName, Types eThisType);
	void DoAFile(char* FileName);
	int  ReadALine(FILE* infile, char *szLineBuff, int iBufSize);


	// Generated message map functions
	//{{AFX_MSG(CWinDumpDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDUMPDLG_H__64BAAE51_B8C8_4305_90CB_13909A65E55B__INCLUDED_)
