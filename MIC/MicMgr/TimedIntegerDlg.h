

#if !defined(AFX_TIMEDINTEGERDLG_H__8DE8A202_9637_4689_9BAA_C07CE0F048FD__INCLUDED_)
#define AFX_TIMEDINTEGERDLG_H__8DE8A202_9637_4689_9BAA_C07CE0F048FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimedIntegerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimedIntegerDlg dialog
#define TIMED_DLG	58
class CTimedIntegerDlg : public CDialog
{
// Construction
public:
	CTimedIntegerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimedIntegerDlg)
	enum { IDD = IDD_TIMED_INTEGER_DIALOG };
	CDateTimeCtrl	m_cDateTimeCtrl;
	CEdit	m_cEditNameCtrl;
	CEdit	m_cEditCtrlCtrl;
	CSpinButtonCtrl	m_cSpin1;
	CButton	m_cCancelButton;
	CString	m_cMsg1;
	CString	m_cMsg2;
	CString	m_cEditCtrl;
	CString	m_cMsg3;
	CString	m_cEditName;
	BOOL	m_bLimited;
	BOOL	m_bAppend;
	BOOL	m_bDoOnce;							//scr00079
	CTime	m_cTime;							//scr00079
	//}}AFX_DATA
	CString m_cMsg0;
	int m_iTimeCount;
	int m_iFullTimeCount;
	int m_iMin;
	int m_iMax;
	int m_iPos;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimedIntegerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimedIntegerDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnButtonBrowse();
	afx_msg void OnChangeEdit2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEDINTEGERDLG_H__8DE8A202_9637_4689_9BAA_C07CE0F048FD__INCLUDED_)
