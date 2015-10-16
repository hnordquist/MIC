#ifndef ISRSUMMARY_H
#define ISRSUMMARY_H

// ISRSummary.h : header file
//
#include "ISRSafePS.h"
#include "resource.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CSummary dialog

class CISRSummary : public CPropertyPage
{
	DECLARE_DYNCREATE(CISRSummary)

// Construction
public:
	CISRSummary(UINT id, CISO *parent);
	CISRSummary();
	~CISRSummary();

	void SetText(UINT, char *);
	char *GetText(int, char*, int);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CISRPropertySheet*);
	COLORREF StaticColors[IDC_ISRSUMMARY_LAST-IDC_ISRSUMMARY_FIRST+1];
	CISO *m_pParent;
	CISRPropertySheet *m_pPropertySheet;

// Dialog Data
	//{{AFX_DATA(CISRSummary)
	enum { IDD = IDD_ISRSUMMARY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CISRSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CISRSummary)
	afx_msg void OnGotoDatastat();
	afx_msg void OnGotoISRSetupPage();
	afx_msg void OnGotoMonitorParametersPage();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ISRSUMMARY_H
