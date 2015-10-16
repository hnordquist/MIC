
#ifndef HIDEDLG_H
#define HIDEDLG_H

// HideDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CHideDlg dialog

class CHideDlg : public CDialog
{
// Construction
public:
	CHideDlg(CWnd* pParent = NULL);   // standard constructor
	CWnd* m_pParent;
// Dialog Data
	//{{AFX_DATA(CHideDlg)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	int m_dToClose;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHideDlg)
	public:

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHideDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnHiddenExtend();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // HIDEDLG_H
