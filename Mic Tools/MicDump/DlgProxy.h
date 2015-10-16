// DlgProxy.h : header file
//

#if !defined(AFX_DLGPROXY_H__D97727D8_9A4C_420E_9AAF_1F7F84FD7449__INCLUDED_)
#define AFX_DLGPROXY_H__D97727D8_9A4C_420E_9AAF_1F7F84FD7449__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWinDumpDlg;

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlgAutoProxy command target

class CWinDumpDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CWinDumpDlgAutoProxy)

	CWinDumpDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CWinDumpDlg* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinDumpDlgAutoProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWinDumpDlgAutoProxy();

	// Generated message map functions
	//{{AFX_MSG(CWinDumpDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CWinDumpDlgAutoProxy)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CWinDumpDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROXY_H__D97727D8_9A4C_420E_9AAF_1F7F84FD7449__INCLUDED_)
