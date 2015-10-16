#if !defined(AFX_USERINFO_H__651C8C82_884C_11D1_B39B_006008A70A6A__INCLUDED_)
#define AFX_USERINFO_H__651C8C82_884C_11D1_B39B_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// UserInfo.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CUserInfo dialog

class CUserInfo : public CDialog
{
// Construction
public:
	CUserInfo(CWnd* pParent, int mode, char* name, char* password );   // standard constructor

// Dialog Data
	int   m_dMode;
	char *m_pcName;
	char *m_pcPassword;
	//{{AFX_DATA(CUserInfo)
	enum { IDD = IDD_USERINFO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserInfo)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserInfo)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERINFO_H__651C8C82_884C_11D1_B39B_006008A70A6A__INCLUDED_)
