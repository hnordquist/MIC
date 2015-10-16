#if !defined(AFX_FILESPEC_H__9DE89E23_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_)
#define AFX_FILESPEC_H__9DE89E23_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FileSpec.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileSpec dialog

class CFileSpec : public CDialog
{
// Construction
public:
	CFileSpec(CWnd* pParent = NULL);   // standard constructor
	CString m_Name;
	CString m_Message;
	CString m_FileSpec;
	CString m_AddOn;
	bool m_bAllowBrowse;
	bool m_bAddGemAuth;
	bool m_bGetForm;
// Dialog Data
	//{{AFX_DATA(CFileSpec)
	enum { IDD = IDD_DIALOG1 };
	CEdit	m_pEditPath;
	CButton	m_pBrowseButton;
	CString	m_cPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileSpec)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileSpec)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditPath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILESPEC_H__9DE89E23_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_)
