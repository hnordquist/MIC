#ifndef EVENTSUMMARY_H
#define EVENTSUMMARY_H

// EventSummary.h : header file
//
#include "EventSafePS.h"
#include "resource.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CEventSummary dialog

class CEventSummary : public CPropertyPage
{
	DECLARE_DYNCREATE(CEventSummary)

// Construction
public:
	CEventSummary(UINT id, CISO *parent);
	CEventSummary();
	~CEventSummary();

	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CEventPropertySheet*);
	COLORREF StaticColors[IDC_EVENTSUMMARY_LAST-IDC_EVENTSUMMARY_FIRST+1];
	void IssuePrint(FILE *outfile);
	CISO *m_pParent;
	CEventPropertySheet *m_pPropertySheet;

// Dialog Data
	//{{AFX_DATA(CEventSummary)
	enum { IDD = IDD_EVENTSUMMARY_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEventSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEventSummary)
	afx_msg void OnGotoEventDatastat();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EVENTSUMMARY_H
