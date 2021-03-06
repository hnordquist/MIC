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
//ReOrganize.h 

#if !defined(AFX_REORGANIZE_H__A52FDA72_EDA5_476B_80B9_5B5F9D34F59F__INCLUDED_)
#define AFX_REORGANIZE_H__A52FDA72_EDA5_476B_80B9_5B5F9D34F59F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReOrganize.h : header file
//

#include "mtreectl.h"
#include "globals.h"
#include "resource.h"
#include "COM_Utilities.h"
/////////////////////////////////////////////////////////////////////////////
// CReOrganize dialog

class CReOrganize : public CDialog
{
// Construction
public:
	CReOrganize(CWnd* pParent);

// Dialog Data
	//{{AFX_DATA(CReOrganize)
	enum { IDD = IDD_REORG_DLG };
	CMyTreeCtrl m_cTreeView;
	CButton	  m_cHideWatch;
	CListBox  m_cListBox;
	CButton	  m_cMoveUp;
	CButton	  m_cMoveDown;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReOrganize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DoReorg();

	BOOL MoveRow(int from, int to);
	BOOL CopyRow(int from, int to);
	CImageList* pImageList;

	CString m_cOriginalNames[MAX_INSTRUMENTS];
	int	m_iOriginalNodes[MAX_INSTRUMENTS];
	int m_iNewNodes[MAX_INSTRUMENTS];
	HTREEITEM m_hComItemDrop;
	// Generated message map functions
	//{{AFX_MSG(CReOrganize)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMoveDown();
	afx_msg void OnSelchangeList1();
	afx_msg void OnMoveUp();
	virtual BOOL OnInitDialog();
	afx_msg void OnOk2();
	afx_msg void OnCheckHidewatch();
	afx_msg void OnRenameSelected();
	afx_msg void OnRenameRevert();
	afx_msg void OnBegindragTreeView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList1();
	afx_msg void OnButtonRegister();
	//}}AFX_MSG
	afx_msg LRESULT OnPreDrop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPostDrop(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REORGANIZE_H__A52FDA72_EDA5_476B_80B9_5B5F9D34F59F__INCLUDED_)
