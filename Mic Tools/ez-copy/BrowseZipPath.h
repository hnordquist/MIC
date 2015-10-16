#if !defined(AFX_BROWSEZIPPATH_H__AD9E37E7_57C3_457A_B18A_F74D8BAA2295__INCLUDED_)
#define AFX_BROWSEZIPPATH_H__AD9E37E7_57C3_457A_B18A_F74D8BAA2295__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BrowseZipPath.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBrowseZipPath dialog

class CBrowseZipPath : public CDialog
{
// Construction
public:
	CBrowseZipPath(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBrowseZipPath)
	enum { IDD = IDD_DIALOG3 };
	CString	m_csZIPPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseZipPath)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrowseZipPath)
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSEZIPPATH_H__AD9E37E7_57C3_457A_B18A_F74D8BAA2295__INCLUDED_)
