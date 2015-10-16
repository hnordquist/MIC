 

#if !defined(AFX_DELETECOM_H__5D235C02_C25B_11D1_B3B3_006008A70A6A__INCLUDED_)
#define AFX_DELETECOM_H__5D235C02_C25B_11D1_B3B3_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DeleteCom.h : header file
//
#include "resource.h"
#include "ccbutton.h"
/////////////////////////////////////////////////////////////////////////////
// CDeleteCom dialog

class CDeleteCom : public CDialog
{
// Construction
public:
	CDeleteCom(CWnd* pParent);
	~CDeleteCom();

// Dialog Data
	//{{AFX_DATA(CDeleteCom)
	enum { IDD = IDD_DELETECOMM_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteCom)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_csTitle;
	CYellowButton *m_pDeleteButton;

	// Generated message map functions
	//{{AFX_MSG(CDeleteCom)
	afx_msg void OnSelchangeDeleteList();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETECOM_H__5D235C02_C25B_11D1_B3B3_006008A70A6A__INCLUDED_)
