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
// CullFileDlg.h : header file
//

#if !defined(AFX_CULLFILEDLG_H__3B9FD266_2BD3_11D3_BBD3_00A024DA5B93__INCLUDED_)
#define AFX_CULLFILEDLG_H__3B9FD266_2BD3_11D3_BBD3_00A024DA5B93__INCLUDED_

#include "DateTimeCtrl.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
/////////////////////////////////////////////////////////////////////////////
// CCullFileDlg dialog
class CCullFileDlg : public CDialog
{
// Construction
public:
	CCullFileDlg(CWnd* pParent = NULL);	// standard constructor
	char	m_cCommandLine[_MAX_PATH];

// Dialog Data
	//{{AFX_DATA(CCullFileDlg)
	enum { IDD = IDD_CULLFILE_DIALOG };
	CButton	m_pEditDir;
	CComboBox	m_pSelectionSet;
	CListBox	m_pList;
	CButton	m_pMonthPlusButton;
	CButton	m_pMonthMinusButton;
	CButton	m_pAddDirButton;
	CButton	m_pDoCopyButton;
	CButton	m_pAbortButton;
	CButton m_pPauseButton;
	CButton	m_pDateFromName;
	CButton	m_pDoLog;
	CListCtrl	m_pListDir;
	CButton	m_pRemDir;
	BOOL	m_bDoLog;
	BOOL	m_bDateFromName;
	CString	m_szList;
	CString	m_szSelectionSet;
	UINT	m_uiSpan;
	//}}AFX_DATA
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCullFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hAbortIcon;

	HANDLE  m_hThread;
	CRect	m_sInitialRect;
	int		m_dSelectedItem;
	int		m_iTimeSpanMonths;
	bool	m_bHaveError;

	char	m_szLog;

	CDateTimeCtrl m_DateTimeFrom;
	CDateTimeCtrl m_DateTimeTo;

	// Generated message map functions
	//{{AFX_MSG(CCullFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonAdddir();
	afx_msg void OnButtonRemdir();
	virtual void OnOK();
	afx_msg void OnButtonDocopy();
	afx_msg void OnItemchangedListDirectories(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonMonthplus();
	afx_msg void OnButtonMonthminus();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnCheckDatefromname();
	afx_msg void OnButtonAbort();
	afx_msg void OnButtonPause();
	afx_msg void OnProgressMsg(WPARAM WParam, LPARAM LParam);
	afx_msg void OnCheckDolog();
	afx_msg void OnSelectionChange();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonEditdir();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnButtonZippath();
	afx_msg void OnChangeEditSpan();
	//}}AFX_MSG
	afx_msg void OnSelectCommand(WPARAM WParam, LPARAM LParam);	//SCR00504 dgp

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CULLFILEDLG_H__3B9FD266_2BD3_11D3_BBD3_00A024DA5B93__INCLUDED_)
