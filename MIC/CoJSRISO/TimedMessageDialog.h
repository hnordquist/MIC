
#if !defined(AFX_TIMEDMESSAGEDIALOG_H__6BBA2E27_0537_4D49_BFB0_2BCAC67E1295__INCLUDED_)
#define AFX_TIMEDMESSAGEDIALOG_H__6BBA2E27_0537_4D49_BFB0_2BCAC67E1295__INCLUDED_

// TimedMessageDialog.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTimedMessageDialog dialog

class CTimedMessageDialog : public CDialog
{
// Construction
public:
	CTimedMessageDialog(
			const char *title,
			const char *message,
			int   seconds,
			CWnd* pParent = NULL);   // standard constructor

	CTimedMessageDialog(
			CString& title,
			CString& message,
			int   seconds,
			CWnd* pParent = NULL);   // standard constructor

	virtual ~CTimedMessageDialog();

	char m_cTitle[32];
	char m_cMessage[256];
	int  m_iTimeToGoInSeconds;
	CString m_csTitle;

// Dialog Data
	//{{AFX_DATA(CTimedMessageDialog)
	enum { IDD = IDD_MESSAGE_DIALOG };
	CString	m_MessageStatic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimedMessageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimedMessageDialog)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEDMESSAGEDIALOG_H__6BBA2E27_0537_4D49_BFB0_2BCAC67E1295__INCLUDED_)
