// WatchAboutDlg.h 

#ifndef WATCHABOUTDLG_H
#define WATCHABOUTDLG_H

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CWatchAboutDlg dialog used for App About

class CWatchAboutDlg : public CDialog
{
public:
	CWatchAboutDlg(int dlgclosemilliseconds, CString* nameversiondate);

	int m_iDlgCloseMilliseconds;
	CString *m_pcsNameVersionDate;

    // Dialog Data
	//{{AFX_DATA(CWatchAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_cStaticVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWatchAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //WATCHABOUTDLG_H