#ifndef ISRUSERPROG_H
#define ISRUSERPROG_H

// ISRUserProg.h : header file
//
#include "ISRSafePS.h"

/////////////////////////////////////////////////////////////////////////////
// CISRUserProg dialog

class CISRUserProg : public CPropertyPage
{
	DECLARE_DYNCREATE(CISRUserProg)

// Construction
public:
	CISRUserProg(UINT id);
	CISRUserProg();
	~CISRUserProg();

	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void SetPropertySheet(CISRPropertySheet*);
	void IssuePrint(FILE* outfile);
	COLORREF StaticColors[IDC_ISRUSERPROG_LAST-IDC_ISRUSERPROG_FIRST+1];

// Dialog Data
	//{{AFX_DATA(CISRUserProg)
	enum { IDD = IDD_ISRUSERPROGSTAT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CISRUserProg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CISRPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CISRUserProg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // ISRUSERPROG_H
