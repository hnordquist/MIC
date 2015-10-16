//NewIPXDialog.h 
 
#ifndef NEWIPXDIALOG_H
#define NEWIPXDIALOG_H

#include "resource.h"
#include "ccbutton.h"
//

/////////////////////////////////////////////////////////////////////////////
// CNewIPXDialog dialog

class CNewIPXDialog : public CDialog
{
// Construction
public:
	CNewIPXDialog(char *inifile, int dlgclose, char *name, CWnd* pParent = NULL);
	virtual ~CNewIPXDialog();

// Dialog Data
	//{{AFX_DATA(CNewIPXDialog)
	enum { IDD = IDD_NEWIPX_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewIPXDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	char m_szIniFile[_MAX_PATH];
	int  m_iDlgCloseMilliseconds;
	int  m_iNumber_of_IPX_detectors;
	bool m_bHavePort;
	char *m_pchName;
	CString m_csTitle;
	CYellowButton *m_pCreateButton;

	bool DiscoverIPXPortInformation();
	void PopulateIPXDetectorList();

	// Generated message map functions
	//{{AFX_MSG(CNewIPXDialog)
	afx_msg void OnSelchangePortList();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // NEWIPXDIALOG_H
