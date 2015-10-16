// SetEdit.h 

#if !defined(AFX_SETEDIT_H__DA6295E6_6513_41C3_9AF5_CE0B3EF69E4D__INCLUDED_)
#define AFX_SETEDIT_H__DA6295E6_6513_41C3_9AF5_CE0B3EF69E4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CSetEdit dialog

class CSetEdit : public CDialog
{
// Construction
public:
	CSetEdit(CWnd* pParent = NULL);   // standard constructor
	unsigned int m_uLength;
// Dialog Data
	//{{AFX_DATA(CSetEdit)
	enum { IDD = IDD_DIALOG_EDIT };
	CEdit	m_cEdit;
	CString	m_cDesired;
	CString	m_cDiscription;
	CString	m_cReported;
	CString	m_cIniName;
	CString	m_cCommand;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetEdit)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETEDIT_H__DA6295E6_6513_41C3_9AF5_CE0B3EF69E4D__INCLUDED_)
