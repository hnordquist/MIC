// User4Prog.h

#if !defined(AFX_USER4PROG_H__00F38782_981A_11D1_B39E_006008A70A6A__INCLUDED_)
#define AFX_USER4PROG_H__00F38782_981A_11D1_B39E_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SafePS.h"

/////////////////////////////////////////////////////////////////////////////
// CUser4Prog dialog
class CUser4Prog : public CPropertyPage
{
	DECLARE_DYNCREATE(CUser4Prog)

// Construction
public:
	CUser4Prog(UINT id);
	CUser4Prog();
	~CUser4Prog();
	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;
//	void IssuePrint(FILE *outfile);

// Dialog Data
	//{{AFX_DATA(CUser4Prog)
	enum { IDD = IDD_USER4PROGSTAT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUser4Prog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
	void GetStrings(int nId, CString &szName, char*szFlag);

	// Generated message map functions
	//{{AFX_MSG(CUser4Prog)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USER4PROG_H__00F38782_981A_11D1_B39E_006008A70A6A__INCLUDED_)
