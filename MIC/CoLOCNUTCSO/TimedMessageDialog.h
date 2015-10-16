//$History: TimedMessageDialog.h $
 * 
 * *****************  Version 2  *****************
 * User: Pelowitz     Date: 4/29/08    Time: 2:14p
 * Updated in $/MIC 2004/CoLOCNUTCSO
 * final check in prior to retirement
//  
// *****************  Version 1  *****************
// User: Pelowitz     Date: 3/12/08    Time: 2:52p
// Created in $/MIC 2004/CoLOCNUTCSO
// At or near retirement (EOSS still being tested)
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 9/13/06    Time: 12:40p
// Created in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 1/11/06    Time: 7:47a
// Created in $/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Moore        Date: 8/09/05    Time: 4:12p
// Created in $/MIC 2.000/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Moore        Date: 7/07/05    Time: 12:41p
// Created in $/MIC 2.0/CoLOCNUTCSO
// Fixed problem with modeless message dialog 
// blocking other operations.


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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEDMESSAGEDIALOG_H__6BBA2E27_0537_4D49_BFB0_2BCAC67E1295__INCLUDED_)
