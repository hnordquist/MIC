// Data2Stat.h

#ifndef DATA2STAT_H
#define DATA2STAT_H

#include "SafePS.h"

/////////////////////////////////////////////////////////////////////////////
// CData2Stat dialog
class CData2Stat : public CPropertyPage
{
	DECLARE_DYNCREATE(CData2Stat)

// Construction
public:
	CData2Stat(UINT id);
	CData2Stat();
	~CData2Stat();
	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;

// Dialog Data
	//{{AFX_DATA(CData2Stat)
	enum { IDD = IDD_DATA2STATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CData2Stat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CData2Stat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // DATA2STAT_H

