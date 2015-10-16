#if !defined(AFX_UGINFO_H__8264FAE0_40E3_430E_81F2_486F3D3EFB0C__INCLUDED_)
#define AFX_UGINFO_H__8264FAE0_40E3_430E_81F2_486F3D3EFB0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UGINFO.h : header file
//
#include "SafePS.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CUGINFO dialog

class CUGINFO : public CPropertyPage
{
	DECLARE_DYNCREATE(CUGINFO)

// Construction
public:
	CUGINFO(UINT id);
	CUGINFO();
	~CUGINFO();
	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CSafePropertySheet*);
	COLORREF *StaticColors;

// Dialog Data
	//{{AFX_DATA(CUGINFO)
	enum { IDD = IDD_DIALOG_UG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUGINFO)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSafePropertySheet *m_pPropertySheet;
	void GetStrings(int nId, CString &szName, char*szFlag);
	// Generated message map functions
	//{{AFX_MSG(CUGINFO)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UGINFO_H__8264FAE0_40E3_430E_81F2_486F3D3EFB0C__INCLUDED_)
