#ifndef JSRDATASTAT_H
#define JSRDATASTAT_H

// JSRDataStat.h : header file
//
#include "JSRSafePS.h"
#include "resource.h"
#include "ISO.h"
/////////////////////////////////////////////////////////////////////////////
// CJSRDataStat dialog
class CJSRDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CJSRDataStat)

// Construction
public:
	CJSRDataStat(UINT id, CISO* parent);
	CJSRDataStat();
	~CJSRDataStat();
	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CJSRPropertySheet*);
	COLORREF StaticColors[IDC_JSRDATA_LAST-IDC_JSRDATA_FIRST+1];
	CISO *m_pParent;

// Dialog Data
	//{{AFX_DATA(CJSRDataStat)
	enum { IDD = IDD_JSRDATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CJSRDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CJSRPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CJSRDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonWatch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // JSRDATASTAT_H
