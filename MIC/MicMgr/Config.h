 

#if !defined(AFX_CONFIG_H__D42A2B51_8778_11D1_B39B_006008A70A6A__INCLUDED_)
#define AFX_CONFIG_H__D42A2B51_8778_11D1_B39B_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Config.h : header file
//
#include "COM_Utilities.h"
#include "ccbutton.h"
/////////////////////////////////////////////////////////////////////////////
// CConfig dialog
class CConfig : public CDialog
{
// Construction
public:
	CConfig(CWnd* pParent);
	~CConfig();

	bool m_bApplied;  //for test in MicMgrDlg

// Dialog Data
	//{{AFX_DATA(CConfig)
	enum { IDD = IDD_CONFIGURE_DIALOG };
	CButton	m_cCheckUseShort;
	CEdit	m_cEditLongName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd* m_pParent;
	bool m_bChanged;
	void Changed(bool change);
	CString m_csTitle;
	CYellowButton *m_pApplyButton;

	// Generated message map functions
	//{{AFX_MSG(CConfig)
	afx_msg void OnApplyButton();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeFacilitynameEdit();
	afx_msg void OnAddinstButton();
	afx_msg void OnDelinstButton();
	afx_msg void OnAddcommButton();
	afx_msg void OnDelcommButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBrowsedestButton();
	afx_msg void OnCheckUseshort();
	afx_msg void OnChangeEditLongname();
	afx_msg void OnChangeMost();
	afx_msg void OnSpinMost(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIG_H__D42A2B51_8778_11D1_B39B_006008A70A6A__INCLUDED_)
