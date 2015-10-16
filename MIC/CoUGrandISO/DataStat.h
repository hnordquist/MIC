// DataStat.h

#ifndef DATASTAT_H
#define DATASTAT_H

#include "SafePS.h"

/////////////////////////////////////////////////////////////////////////////
// CDataStat dialog
class CDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CDataStat)

// Construction
public:
	CDataStat(UINT id);
	CDataStat();
	~CDataStat();
	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;
	CFont m_cVertFont;
	BOOL m_bCheckVector[16];

// Dialog Data
	//{{AFX_DATA(CDataStat)
	enum { IDD = IDD_DATASTATUS_PAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
	void CountChecks(int nID);
	// Generated message map functions
	//{{AFX_MSG(CDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCheckc0cr();
	afx_msg void OnCheckc0scr();
	afx_msg void OnCheckc0lcr();
	afx_msg void OnCheckc1cr();
	afx_msg void OnCheckc1lcr();
	afx_msg void OnCheckc1scr();
	afx_msg void OnCheckc2cr();
	afx_msg void OnCheckc2lcr();
	afx_msg void OnCheckc2scr();
	afx_msg void OnCheckc3cr();
	afx_msg void OnCheckc3lcr();
	afx_msg void OnCheckc3scr();
	afx_msg void OnCheckcputemp2();
	afx_msg void OnCheckcputemp();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // DATASTAT_H

