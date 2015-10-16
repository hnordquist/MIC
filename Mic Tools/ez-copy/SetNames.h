#if !defined(AFX_SETNAMES_H__2FFA2341_D332_11D3_8FB5_006008A70A6A__INCLUDED_)
#define AFX_SETNAMES_H__2FFA2341_D332_11D3_8FB5_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SetNames.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetNames dialog

class CSetNames : public CDialog
{
// Construction
public:
	CSetNames(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetNames)
	enum { IDD = IDD_DIALOG1 };
	CString	m_cSet1;
	CString	m_cSet2;
	CString	m_cSet3;
	CString	m_cSet4;
	CString	m_cSet5;
	CString	m_cSet6;
	CString	m_cSet7;
	CString	m_cSet8;
	CString	m_cSet9;
	CString	m_cSet10;
	CString	m_cSet11;
	CString	m_cSet12;
	CString	m_cSet13;
	CString	m_cSet14;
	CString	m_cSet15;
	CString	m_cSet16;
	CString	m_cSet17;
	CString	m_cSet18;
	CString	m_cSet19;
	CString	m_cSet20;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetNames)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetNames)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETNAMES_H__2FFA2341_D332_11D3_8FB5_006008A70A6A__INCLUDED_)
