#if !defined(AFX_FILETYPES_H__85F21EC3_2C66_11D3_BBD3_00A024DA5B93__INCLUDED_)
#define AFX_FILETYPES_H__85F21EC3_2C66_11D3_BBD3_00A024DA5B93__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FileTypes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileTypes dialog

class CFileTypes : public CDialog
{
// Construction
public:
	CFileTypes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileTypes)
	enum { IDD = IDD_DIALOG_FILETYPES };
	CString	m_cFileTypes;
	BOOL	m_bDoSubDirs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileTypes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileTypes)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETYPES_H__85F21EC3_2C66_11D3_BBD3_00A024DA5B93__INCLUDED_)
