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
// treeDlg.h : header file
//

#if !defined(AFX_TREEDLG_H__A49AA586_D720_11D3_BBD3_00A024DA5B93__INCLUDED_)
#define AFX_TREEDLG_H__A49AA586_D720_11D3_BBD3_00A024DA5B93__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "FileSpec.h"
#include "Schedule.h"
#include "GetNumeric.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeDlg dialog

class CTreeDlg : public CDialog
{
// Construction
public:
	CTreeDlg(CWnd* pParent = NULL);	// standard constructor
	~CTreeDlg();
	char m_cCommandLine[_MAX_PATH];
	void UpdateDisplay();
	HTREEITEM InsertItem(char *szGroup,char *szItem, char *szPreText, HTREEITEM hItem, DWORD dwType);
	HTREEITEM InsertSet(char *szName);
	void InsertItemFromTos(char *szSetName, HTREEITEM hItem);
	void WriteSection(char*,HTREEITEM);

// Dialog Data
	//{{AFX_DATA(CTreeDlg)
	enum { IDD = IDD_TREE_DIALOG };
	CButton	m_ButtonAdd;
	CButton	m_ButtonDelete;
	CStatic	m_StaticImage;
	CButton	m_ButtonEdit;
	CTreeCtrl	m_Tree;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hIconCheck;
	HICON m_hIconQuest;
	HICON m_hIconWait;
	char m_szIniFile[_MAX_PATH];
	CScrollBar m_Grip;
	CFileSpec m_FileSpec;
	CSchedule m_Schedule;
	CGetNumeric m_GetNumber;
	bool m_bInitialized;
	CRect m_OldRect;
	CRect m_OldEdit;
	HTREEITEM m_CONFIG;
	HTREEITEM m_CALENDAR;
	HTREEITEM m_CEV;
	HTREEITEM m_PFM;
	HTREEITEM m_ACQUIRE;
	HTREEITEM m_JPG;
	HTREEITEM m_OTHER;


	// Generated message map functions
	//{{AFX_MSG(CTreeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonReload();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonAdd();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEDLG_H__A49AA586_D720_11D3_BBD3_00A024DA5B93__INCLUDED_)
