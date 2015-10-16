// GenlStat.h

#ifndef GENLSTAT_H
#define GENLSTAT_H

#include "SafePS.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGenlStat dialog
class CGenlStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CGenlStat)

// Construction
public:
	CGenlStat(UINT id);
	CGenlStat();
	~CGenlStat();
	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;

// Dialog Data
	//{{AFX_DATA(CGenlStat)
	enum { IDD = IDD_GENERALSTAT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGenlStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
//	void GetStrings(int nId, CString &szName, char*szFlag);
	// Generated message map functions
	//{{AFX_MSG(CGenlStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //GENLSTAT_H
