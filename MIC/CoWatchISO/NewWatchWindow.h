// NewWatchWindow.h

#ifndef NEWWATCHWINDOW_H
#define NEWWATCHWINDOW_H

#include "resource.h"
#include "ccbutton.h"

/////////////////////////////////////////////////////////////////////////////
// CNewWatchWindow dialog

class CNewWatchWindow : public CDialog
{
// Construction
public:
	CNewWatchWindow(
		char * szIniFile,
		int iDlgClose,
		char *name, 
		CWnd* pParent = NULL);

	virtual ~CNewWatchWindow();

	char m_szIniFile[MAX_PATH];
	int m_iDlgCloseInMilliseconds;
	char m_pName[32];
	char m_pCommName[32];
	CYellowButton *m_pCreateButton;

// Dialog Data
	//{{AFX_DATA(CNewWatchWindow)
	enum { IDD = IDD_NEWWATCHWINDOW_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewWatchWindow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewWatchWindow)
	afx_msg void OnSelchangeCommnamesList();
	afx_msg void OnChangeNodeEdit();
	afx_msg void OnDeltaposNodeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // NEWWATCHWINDOW_H
