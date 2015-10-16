#ifndef MCAGENLSTAT_H
#define MCAGENLSTAT_H

// MCAGenlStat.h : header file
//
#include "MCASafePS.h"
#include "ISO.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMCAGenlStat dialog
class CMCAGenlStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCAGenlStat)

// Construction
public:

	CMCAGenlStat(UINT id, CISO *parent);
	CMCAGenlStat();
	~CMCAGenlStat();

	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void ResetColors();

	CISO *m_pParent;

	void SetPropertySheet(CMCAPropertySheet*);
	COLORREF StaticColors[IDC_MCAGENERAL_LAST-IDC_MCAGENERAL_FIRST+1];


// Dialog Data
	//{{AFX_DATA(CMCAGenlStat)
	enum { IDD = IDD_MCAGENERALSTAT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMCAGenlStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMCAPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CMCAGenlStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // MCAGENLSTAT_H
