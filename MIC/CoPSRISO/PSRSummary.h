#ifndef PSRSUMMARY_H
#define PSRSUMMARY_H

// PSRSummary.h : header file
//
#include "PSRSafePS.h"
#include "resource.h"
#include "ISO.h"
/////////////////////////////////////////////////////////////////////////////
// CSummary dialog
class CPSRSummary : public CPropertyPage
{
	DECLARE_DYNCREATE(CPSRSummary)

// Construction
public:
	CPSRSummary(UINT id, CISO *parent);
	CPSRSummary(void);
	~CPSRSummary();

	void SetText(UINT, char *);
	char *GetText(int, char*, int);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CPSRPropertySheet*);
	void IssuePrint(FILE *outfile);

	COLORREF StaticColors[IDC_PSRSUMMARY_LAST-IDC_PSRSUMMARY_FIRST+1];
	bool m_bDebug;
	CISO *m_pParent;
	CPSRPropertySheet *m_pPropertySheet;

// Dialog Data
	//{{AFX_DATA(CPSRSummary)
	enum { IDD = IDD_PSRSUMMARY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPSRSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPSRSummary)
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

#endif // PSRSUMMARY_H
