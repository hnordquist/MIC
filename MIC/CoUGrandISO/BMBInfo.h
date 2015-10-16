#if !defined(AFX_BMBINFO_H__8BDD2920_E8C0_494E_BB31_79AD9718D186__INCLUDED_)
#define AFX_BMBINFO_H__8BDD2920_E8C0_494E_BB31_79AD9718D186__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BMBInfo.h : header file
//
#include "SafePS.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CBMBInfo dialog

class CBMBInfo : public CPropertyPage
{
	DECLARE_DYNCREATE(CBMBInfo)

// Construction
public:
	CBMBInfo(UINT id);
	CBMBInfo();
	~CBMBInfo();
	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;
// Dialog Data
	//{{AFX_DATA(CBMBInfo)
	enum { IDD = IDD_DIALOG_BMB };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBMBInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
	void GetStrings(int nId, CString &szName, char*szFlag);
	// Generated message map functions
	//{{AFX_MSG(CBMBInfo)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMBINFO_H__8BDD2920_E8C0_494E_BB31_79AD9718D186__INCLUDED_)
