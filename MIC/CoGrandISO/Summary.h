// Summary.h

#ifndef SUMMARY_H
#define SUMMARY_H

#include "SafePS.h"
#include "resource.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CSummary dialog

class CSummary : public CPropertyPage
{
	DECLARE_DYNCREATE(CSummary)

// Construction
public:
	CSummary(UINT id, CISO *parent);
	CSummary(void);
	~CSummary();

	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;
	CISO *m_pParent;
//	void IssuePrint(FILE *outfile);
// Dialog Data
	//{{AFX_DATA(CSummary)
	enum { IDD = IDD_SUMMARY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
//	void GetStrings(int nId, CString &szName, char*szFlag);
	// Generated message map functions
	//{{AFX_MSG(CSummary)
	afx_msg void OnGotoDatastat();
	afx_msg void OnGotoGrand();
	afx_msg void OnGotoUserstat();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTodaydata();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // SUMMARY_H
