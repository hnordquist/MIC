// EOSSLegend.h 

#ifndef EOSSLEGEND_H
#define EOSSLEGEND_H

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CLegend dialog

class CLegend : public CPropertyPage
{
	DECLARE_DYNCREATE(CLegend)

// Construction
public:
	CLegend();
	~CLegend();
	void Substitute(char *szString,char *szSubStr,COLORREF cTXTColor, bool bWingdings);
	void SetText(char *text);

// Dialog Data
	//{{AFX_DATA(CLegend)
	enum { IDD = IDD_PROPPAGE_LEGEND };
	CRichEditCtrl	m_cRichEditIcons;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLegend)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLegend)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // EOSSLEGEND_H
