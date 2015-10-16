//validate.h

#if !defined(AFX_VALIDATE_H__EE826121_86AC_11D1_B39B_006008A70A6A__INCLUDED_)
#define AFX_VALIDATE_H__EE826121_86AC_11D1_B39B_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// validate.h : header file

#include "globals.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// validate dialog

class CValidate : public CDialog
{
// Construction
public:

	CValidate(CWnd* pParent, bool bSuperUser=false);
	~CValidate();

	bool Test(char *, char *);
	void Encrypt(char *, char *, char = NULL);
	char* GetUserName();
	bool m_bSuperUser;

// Dialog Data
	//{{AFX_DATA(CValidate)
	enum { IDD = IDD_VALIDATE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CValidate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//char IniFileName[_MAX_PATH];
	char m_szUserName[256];
	// Generated message map functions
	//{{AFX_MSG(CValidate)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VALIDATE_H__EE826121_86AC_11D1_B39B_006008A70A6A__INCLUDED_)
