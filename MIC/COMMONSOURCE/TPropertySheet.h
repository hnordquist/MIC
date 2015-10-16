// TPropertySheet.h
 

#if !defined(AFX_TPROPERTYSHEET_H__4A58AC03_BE6E_11D1_B3B2_006008A70A6A__INCLUDED_)
#define AFX_TPROPERTYSHEET_H__4A58AC03_BE6E_11D1_B3B2_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define IDB_TOCLOSE 3999

#include <afxdlgs.h>

/////////////////////////////////////////////////////////////////////////////
// CTPropertySheet

class CTPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CTPropertySheet)

// Construction
public:
	CTPropertySheet(UINT nIDCaption, int dlg_close, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTPropertySheet(LPCTSTR pszCaption, int dlg_close, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	virtual void SetToClose(int set);
	virtual void ForceDateTime(void);
	virtual void SameNextFinish();
	int m_dToClose;
	int m_dMyDlgClose;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	virtual int DoModal();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTPropertySheet();
protected:
	bool	m_bSameNextFinish;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTPropertySheet)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToCloseButton();
	afx_msg void OnClose();
	//}}AFX_MSG
//	afxx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TPROPERTYSHEET_H__4A58AC03_BE6E_11D1_B3B2_006008A70A6A__INCLUDED_)
