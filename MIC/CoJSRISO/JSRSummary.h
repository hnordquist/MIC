#ifndef JSRSUMMARY_H
#define JSRSUMMARY_H

// JSRSummary.h : header file
//
#include "JSRSafePS.h"
#include "resource.h"
#include "ISO.h"
/////////////////////////////////////////////////////////////////////////////
// CSummary dialog
class CJSRSummary : public CPropertyPage
{
	DECLARE_DYNCREATE(CJSRSummary)

// Construction
public:
	CJSRSummary(UINT id, CISO *parent);
	CJSRSummary(void);
	~CJSRSummary();

	void SetText(UINT, char *);
	char *GetText(int, char*, int);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CJSRPropertySheet*);
	void IssuePrint(FILE *outfile);

	COLORREF StaticColors[IDC_JSRSUMMARY_LAST-IDC_JSRSUMMARY_FIRST+1];
	bool m_bDebug;
	CISO *m_pParent;
	CJSRPropertySheet *m_pPropertySheet;

// Dialog Data
	//{{AFX_DATA(CJSRSummary)
	enum { IDD = IDD_JSRSUMMARY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CJSRSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJSRSummary)
	afx_msg void OnGotoDatastat();
	afx_msg void OnGotoModifyParametersPage();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // JSRSUMMARY_H
