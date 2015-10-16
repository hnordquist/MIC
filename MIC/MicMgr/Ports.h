#include "afxcmn.h"
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Ports.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfig dialog
class CComPortList : public CDialog
{
// Construction
public:
	CComPortList(CWnd* pParent,CString **p);
	~CComPortList();

	CString ** theListOfPorts;

// Dialog Data
	//{{AFX_DATA(CComPortList)
	enum { IDD = IDD_COMPORTLIST };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComPortList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComPortList)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl meinpops;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

