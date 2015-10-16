#ifndef NEWILONCOMM_H
#define NEWILONCOMM_H

// NewILONComm.h : header file

#include "resource.h"
#include "ccbutton.h"

/////////////////////////////////////////////////////////////////////////////
// CNewILONComm dialog

class CNewILONComm : public CDialog
{
	DECLARE_DYNCREATE(CNewILONComm)

// Construction
public:
	CNewILONComm(char *cIniFile, int iDlgClose, char *name);
	CNewILONComm();
	virtual ~CNewILONComm();

// Dialog Data
	//{{AFX_DATA(CNewILONComm)
	enum { IDD = IDD_ILON_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewILONComm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	CYellowButton *m_pCreateButton;

	int  m_dDlgCloseMilliseconds;
	int  m_dParity;
	int  m_dDataBits;
	int  m_dStopBits;
	int  m_dBaudRate;

	char m_pName[128];
	char m_szPort[32];
	char m_szIniFile[_MAX_PATH];

	CString m_csTitle;
	int m_dCurSel_Serial;

	void PopulateSerialPortList();

	// Generated message map functions
	//{{AFX_MSG(CNewILONComm)
	afx_msg void On96e71Button();
	afx_msg void On96n81Button();
	virtual void OnOK();
	afx_msg void OnBaudRadio();
	virtual BOOL OnInitDialog();
	afx_msg void OnDatabitsRadio();
	afx_msg void OnParityRadio();
	afx_msg void OnSelchangePortList();
	afx_msg void OnStopbitsRadio();
	afx_msg void OnChangeEditWdnode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // NEWILONCOMM_H
