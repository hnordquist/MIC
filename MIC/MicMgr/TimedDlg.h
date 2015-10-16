#if !defined(AFX_TIMEDDLG_H__9C248550_798B_4A47_9A8D_FE7EB967B99E__INCLUDED_)
#define AFX_TIMEDDLG_H__9C248550_798B_4A47_9A8D_FE7EB967B99E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimedDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimedDlg dialog

class CTimedDlg : public CDialog
{
// Construction
public:
	CTimedDlg(
			const char *title,
			const char *message,
			int   seconds,
			CWnd* pParent = NULL);   // standard constructor

	CTimedDlg(
			CString& title,
			CString& message,
			int   seconds,
			CWnd* pParent = NULL);   // standard constructor

	virtual ~CTimedDlg();

	char m_cTitle[32];
	char m_cMessage[256];
	int  m_iTimeToGoInSeconds;
	CString m_csTitle;

// Dialog Data
	//{{AFX_DATA(CTimedDlg)
	enum { IDD = IDD_DIALOG3 };
	CString	m_cStaticMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTimedDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEDDLG_H__9C248550_798B_4A47_9A8D_FE7EB967B99E__INCLUDED_)
