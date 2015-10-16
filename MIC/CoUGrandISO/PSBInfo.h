#if !defined(AFX_PSBINFO_H__94F2224B_015A_4326_B8D1_62F1E306F4C9__INCLUDED_)
#define AFX_PSBINFO_H__94F2224B_015A_4326_B8D1_62F1E306F4C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PSBInfo.h : header file
//
#include "SafePS.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CPSBInfo dialog

class CPSBInfo : public CPropertyPage
{
	DECLARE_DYNCREATE(CPSBInfo)

// Construction
public:
	CPSBInfo(UINT id);
	CPSBInfo();
	~CPSBInfo();
	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;

// Dialog Data
	//{{AFX_DATA(CPSBInfo)
	enum { IDD = IDD_DIALOG_PSB };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPSBInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
	void GetStrings(int nId, CString &szName, char*szFlag);
	// Generated message map functions
	//{{AFX_MSG(CPSBInfo)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSBINFO_H__94F2224B_015A_4326_B8D1_62F1E306F4C9__INCLUDED_)
