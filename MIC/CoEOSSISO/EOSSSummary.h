#ifndef EOSSSUMMARY_H
#define EOSSSUMMARY_H

// EOSSSummary.h : header file
//
#include "EOSSSafePS.h"
#include "resource.h"
#include "ISO.h"
#include "CheckerCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CEOSSSummary dialog

class CEOSSSummary : public CPropertyPage
{
	DECLARE_DYNCREATE(CEOSSSummary)

// Construction
public:
	CEOSSSummary(UINT id, CISO *parent);
	CEOSSSummary();
	~CEOSSSummary();

	void SetText(UINT, char *);
	//void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CEOSSPropertySheet*);
	void PopulateChecker();
	COLORREF StaticColors[IDC_EOSSSUMMARY_LAST-IDC_EOSSSUMMARY_FIRST+1];
	void IssuePrint(FILE *outfile);
	CISO *m_pParent;
	CEOSSPropertySheet *m_pPropertySheet;
	CCheckerCtrl m_ctrlChecker;
	bool m_bCheckerCreated;

// Dialog Data
	//{{AFX_DATA(CEOSSSummary)
	enum { IDD = IDD_EOSSSUMMARY_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEOSSSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COleDateTimeSpan m_cSpan;

	// Generated message map functions
	//{{AFX_MSG(CEOSSSummary)
	afx_msg void OnGotoEOSSDatastat();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnButtonEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EOSSSUMMARY_H
