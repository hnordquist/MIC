// UserProg.h

#ifndef USERPROG_H
#define USERPROG_H

#include "SafePS.h"

/////////////////////////////////////////////////////////////////////////////
// CUserProg dialog

class CUserProg : public CPropertyPage
{
	DECLARE_DYNCREATE(CUserProg)

// Construction
public:
	CUserProg(UINT id);
	CUserProg();
	~CUserProg();

	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;
//	void IssuePrint(FILE *outfile);

// Dialog Data
	//{{AFX_DATA(CUserProg)
	enum { IDD = IDD_USERPROGSTAT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUserProg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
//	void GetStrings(int nId, CString &szName, char*szFlag);
	// Generated message map functions
	//{{AFX_MSG(CUserProg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // USERPROG_H