#if !defined(AFX_CONFIG_H__0B7423B1_5591_11D3_8F77_006008A70A6A__INCLUDED_)
#define AFX_CONFIG_H__0B7423B1_5591_11D3_8F77_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Config.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfig dialog
class CTrackerDlg;
class CConfig : public CDialog
{
// Construction
public:
	CConfig(char *cIniFile, CWnd* pParent = NULL);   // standard constructor
	CTrackerDlg* m_pParent;
	int		m_bDoUDP;
	char m_cIniFile[MAX_PATH];
// Dialog Data
	//{{AFX_DATA(CConfig)
	enum { IDD = IDD_DIALOG_CONFIG };
	CButton	m_cCheckSendStatus;
	CComboBox	m_cPushXMLCombo;
	CComboBox	m_cPushCombo;
	BOOL	m_bDoHTML;
	CString	m_sName;
	CString	m_szMailSlotName;
	int		m_dMSTime;
	int		m_dUDPPort;
	int		m_dUDP1;
	int		m_dUDP2;
	int		m_dUDP3;
	int		m_dUDP4;
	int		m_dHTMLTime;
	CString	m_szHTMLName;
	BOOL	m_bAllowForward;
	CString	m_cLogName;
	BOOL	m_bDoAlarm;
	//}}AFX_DATA
	bool	m_bToMIC;
	bool	m_bAdvanced;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfig)
	virtual void OnOK();
	afx_msg void OnCheckDoHTML();
	afx_msg void OnRadioDoMailSlot();
	afx_msg void OnRadioDoUDP();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAutoforward();
	afx_msg void OnRadioToMIC();
	afx_msg void OnRadioToTRACKER();
	afx_msg void OnButtonPushadd();
	afx_msg void OnButtonPushdelete();
	afx_msg void OnButtonPushxmladd();
	afx_msg void OnButtonPushxmldelete();
	afx_msg void OnButtonAdvanced();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIG_H__0B7423B1_5591_11D3_8F77_006008A70A6A__INCLUDED_)
