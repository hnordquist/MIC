 
#ifndef NEWSERIALCOMM_H
#define NEWSERIALCOMM_H

// file NewSerialComm.h

#include "resource.h"
#include "ccbutton.h"

class CNewSerialComm : public CDialog
{
	DECLARE_DYNCREATE(CNewSerialComm)

// Construction
public:
	CNewSerialComm(char *cIniFile, int iDlgClose, char *name);
	CNewSerialComm();
	virtual ~CNewSerialComm();

// Dialog Data

	//{{AFX_DATA(CNewSerialComm)
	enum { IDD = IDD_DIRECT_SERIAL_COLLECT_PARAMETERS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewSerialComm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CYellowButton *m_pCreateButton;

	int m_dDlgCloseMilliseconds;
	int m_dParity;
	int m_dDataBits;
	int m_dStopBits;
	int m_dBaudRate;
	bool m_bHavePort;

	char m_pName[128];
	char m_szPort[32];
	char m_szIniFile[_MAX_PATH];

	CString m_csTitle;
	int m_dCurSel_Serial;

	void PopulateSerialPortList();

	// Generated message map functions
	//{{AFX_MSG(CNewSerialComm)
	afx_msg void On96e71Button();
	afx_msg void On96n81Button();
	virtual void OnOK();
	afx_msg void OnBaudRadio();
	virtual BOOL OnInitDialog();
	afx_msg void OnDatabitsRadio();
	afx_msg void OnParityRadio();
	afx_msg void OnSelchangePortList();
	afx_msg void OnStopbitsRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // NEWSERIALCOMM_H
