

#if !defined(AFX_ACCESS_H__651C8C81_884C_11D1_B39B_006008A70A6A__INCLUDED_)
#define AFX_ACCESS_H__651C8C81_884C_11D1_B39B_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Access.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CAccess dialog
// maintain accesscontrol list dialog box
class CAccess : public CDialog
{
// Construction
public:
	CAccess(CWnd* pParent);   // standard constructor
	virtual ~CAccess();
	int CountUsers();

// Dialog Data
	//{{AFX_DATA(CAccess)
	enum { IDD = IDD_ACCESS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccess)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnNew(void);
	// Generated message map functions
	//{{AFX_MSG(CAccess)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeUserList();
	afx_msg void OnNewButton();
	afx_msg void OnEditButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnDblclkUserList();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCESS_H__651C8C81_884C_11D1_B39B_006008A70A6A__INCLUDED_)
