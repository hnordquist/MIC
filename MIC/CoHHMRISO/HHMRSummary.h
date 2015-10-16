#ifndef HHMRSUMMARY_H
#define HHMRSUMMARY_H

// HHMRSummary.h : header file
//
#include "HHMRSafePS.h"
#include "resource.h"
#include "ISO.h"
/////////////////////////////////////////////////////////////////////////////
// CSummary dialog
class CHHMRSummary : public CPropertyPage
{
	DECLARE_DYNCREATE(CHHMRSummary)

// Construction
public:
	CHHMRSummary(UINT id, CISO *parent);
	CHHMRSummary(void);
	~CHHMRSummary();

	void SetText(UINT, char *);
	char *GetText(int, char*, int);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CHHMRPropertySheet*);
	void IssuePrint(FILE *outfile);

	COLORREF StaticColors[IDC_HHMRSUMMARY_LAST-IDC_HHMRSUMMARY_FIRST+1];
	bool m_bDebug;
	CISO *m_pParent;
	CHHMRPropertySheet *m_pPropertySheet;

// Dialog Data
	//{{AFX_DATA(CHHMRSummary)
	enum { IDD = IDD_HHMRSUMMARY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHHMRSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHHMRSummary)
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

#endif // HHMRSUMMARY_H
