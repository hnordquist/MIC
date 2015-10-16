#if !defined(AFX_ADDEOSS_H__C7D4C2A9_7F52_4FA8_A4BD_1180278A80A3__INCLUDED_)
#define AFX_ADDEOSS_H__C7D4C2A9_7F52_4FA8_A4BD_1180278A80A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddEOSS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddEOSS dialog
#include "resource.h"
class CAddEOSS : public CDialog
{
// Construction
public:
	CAddEOSS(CWnd* pParent = NULL);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CAddEOSS)
	enum { IDD = IDD_DIALOG_ADD };
	CEdit	m_cEOSSEdit;
	CString	m_csEOSSSN;
	CListCtrl* m_parentsListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddEOSS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddEOSS)
	afx_msg void OnChangeAddId();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnKillfocusAddId();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool IsDuplicateId();
	bool IsUniqueId();


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDEOSS_H__C7D4C2A9_7F52_4FA8_A4BD_1180278A80A3__INCLUDED_)
