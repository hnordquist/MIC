#if !defined(AFX_ADDITEM_H__B95DBA81_D4E1_11D3_8FB5_006008A70A6A__INCLUDED_)
#define AFX_ADDITEM_H__B95DBA81_D4E1_11D3_8FB5_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AddItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AddItem dialog

class AddItem : public CDialog
{
// Construction
public:
	AddItem(CWnd* pParent = NULL);   // standard constructor
	void SetUp(CString cSourceDir, BOOL bDoSubDirs, BOOL bDoBulk,
		CString cDestinationDir, CString cFileTypes, char* szIniFile, BOOL bOnlyIfChange = FALSE);
	void SetTitle(CString cTitle){m_cTitle = cTitle;};
	void CheckAll();
	bool m_bGoodSource;
	bool m_bGoodDestination;
	bool m_bGoodFileTypes;
	BOOL m_bOnlyIfChange;
	CString m_cTitle;
	char* m_szIniFile;
// Dialog Data
	//{{AFX_DATA(AddItem)
	enum { IDD = IDD_DIALOG2 };
	BOOL	m_bDoSubDirs;
	CString	m_cSourceDir;
	CString	m_cDestinationDir;
	CString	m_cFileTypes;
	BOOL	m_bDoBulk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AddItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AddItem)
	afx_msg void OnButtonBrowseSource();
	afx_msg void OnButtonBrowseDestination();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSource();
	afx_msg void OnChangeDestination();
	afx_msg void OnChangeFileTypes();
	afx_msg void OnButtonSaveDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDITEM_H__B95DBA81_D4E1_11D3_8FB5_006008A70A6A__INCLUDED_)
