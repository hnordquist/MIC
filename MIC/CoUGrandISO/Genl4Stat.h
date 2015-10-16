// Genl4Stat.h

#if !defined(AFX_GENL4STAT_H__00F38780_981A_11D1_B39E_006008A70A6A__INCLUDED_)
#define AFX_GENL4STAT_H__00F38780_981A_11D1_B39E_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GenlStat.h : header file
//
#include "SafePS.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CGenl4Stat dialog
class CGenl4Stat : public CPropertyPage
{
	DECLARE_DYNCREATE(CGenl4Stat)

// Construction
public:
	CGenl4Stat(UINT id);
	CGenl4Stat();
	~CGenl4Stat();
	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;
//	void IssuePrint(FILE *outfile);

// Dialog Data
	//{{AFX_DATA(CGenl4Stat)
	enum { IDD = IDD_GENERAL4STAT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGenl4Stat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
	void GetStrings(int nId, CString &szName, char*szFlag);
	// Generated message map functions
	//{{AFX_MSG(CGenl4Stat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENL4STAT_H__00F38780_981A_11D1_B39E_006008A70A6A__INCLUDED_)
