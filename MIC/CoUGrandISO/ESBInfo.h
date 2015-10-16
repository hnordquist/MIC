#if !defined(AFX_ESBINFO_H__F37C9DA7_F667_49FF_87AC_AD2DCCD40C82__INCLUDED_)
#define AFX_ESBINFO_H__F37C9DA7_F667_49FF_87AC_AD2DCCD40C82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ESBInfo.h : header file
//
#include "SafePS.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CESBInfo dialog

class CESBInfo : public CPropertyPage
{
	DECLARE_DYNCREATE(CESBInfo)

// Construction
public:
	CESBInfo(UINT id);
	CESBInfo();
	~CESBInfo();
	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;

// Dialog Data
	//{{AFX_DATA(CESBInfo)
	enum { IDD = IDD_DIALOG_ESB };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CESBInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
	void GetStrings(int nId, CString &szName, char*szFlag);
	// Generated message map functions
	//{{AFX_MSG(CESBInfo)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ESBINFO_H__F37C9DA7_F667_49FF_87AC_AD2DCCD40C82__INCLUDED_)
