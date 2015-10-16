#if !defined(AFX_GETNUMERIC_H__9DE89E25_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_)
#define AFX_GETNUMERIC_H__9DE89E25_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GetNumeric.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetNumeric dialog

class CGetNumeric : public CDialog
{
// Construction
public:
	CGetNumeric(CWnd* pParent = NULL);   // standard constructor
	CString m_cMessage;
	CString m_cTitle;

// Dialog Data
	//{{AFX_DATA(CGetNumeric)
	enum { IDD = IDD_DIALOG2 };
	int		m_dValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetNumeric)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGetNumeric)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETNUMERIC_H__9DE89E25_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_)
