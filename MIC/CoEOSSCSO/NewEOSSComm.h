#ifndef NEWEOSSCOMM_H
#define NEWEOSSCOMM_H

// NewEOSSComm.h : header file

#include "resource.h"
#include "ccbutton.h"

/////////////////////////////////////////////////////////////////////////////
// CNewEOSSComm dialog

class CNewEOSSComm : public CDialog
{
	DECLARE_DYNCREATE(CNewEOSSComm)

// Construction
public:
	CNewEOSSComm(char *cIniFile, int iDlgClose, char *name);
	CNewEOSSComm();
	virtual ~CNewEOSSComm();

// Dialog Data
	//{{AFX_DATA(CNewEOSSComm)
	enum { IDD = IDD_EOSS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewEOSSComm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	CYellowButton *m_pCreateButton;

	int  m_dDlgCloseMilliseconds;

	char m_pName[128];
	char m_szPort[32];
	char m_szIniFile[_MAX_PATH];

	CString m_csTitle;
	int m_dCurSel_Serial;

	void PopulateSerialPortList();

	// Generated message map functions
	//{{AFX_MSG(CNewEOSSComm)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePortList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // NEWEOSSCOMM_H
