// TestDlg.h

#if !defined(AFX_TESTDLG_H__0B89D6C2_9D71_11D1_B39F_006008A70A6A__INCLUDED_)
#define AFX_TESTDLG_H__0B89D6C2_9D71_11D1_B39F_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TestDlg.h : header file
//
#include "CCButton.h"
#include "../Interfaces/ICSO.h"
#include "../Interfaces/IISO.h"
#include "resource.h"
#include "EditSend.h"
#include <afxcmn.h>

class CTestInst;

#define MAX_CEDIT_HIGH 120000
#define MAX_CEDIT_DECR 5000

#define COMMANDSTACKSIZE 10

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

class CTestDlg : public CDialog
{
	//DECLARE_DYNAMIC(CTestDlg)

// Construction
public:
	CTestDlg(CWnd* pParent, ICSO* pComm, int Node );
// Dialog Data
	//{{AFX_DATA(CTestDlg)
	enum { IDD = IDD_WATCH_DIALOG };
	CProgressCtrl   m_Progress;
	CSpinButtonCtrl m_EditSpin;
	CEdit           m_EditDelay;
	CEditSend	    m_pSend;
	CEdit           m_pReceive;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDlg)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTestDlg();

protected:
	CTestInst* m_pParent;
	ICSO    *m_pComm;
	FILE    *m_Finfile;
	long	m_dFileSize;
	int		m_dTotalBytesRead;
	int		m_dBytesRemainingToSend;
	int		m_dBytesSent;
	bool	m_bDoingTransfer;
	int		m_dNode;
	bool	m_bNl;
	int		m_dLength;
	char	m_cLastChar;
	char	m_cTempBuf[2048];
	bool	m_bIn33;
	int		m_dIn33Count;
	int		m_dPostBEEFCount;
	bool	m_bInitialized;
	CColorButton *m_pCloseButton;
	CFont        EditFont;
	CRect        m_OldRect;
	CScrollBar   m_Grip;
	CString m_cLastCommands[COMMANDSTACKSIZE];
	int		m_iLastCommand;

	int DLG_COM_Transmit(int ilength,char *output,int Node, bool Quiet = false);  //<<QA>>

	// Generated message map functions
	//{{AFX_MSG(CTestDlg)
	virtual void OnOK();
	afx_msg void OnTestCloseButton();
	afx_msg void OnTestPrintButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnTestClearButton();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTestSendfileButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeEditDelay();
	afx_msg void OnRadioMost();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnReceive(WPARAM, LPARAM);
	afx_msg void OnReceiveOOB(WPARAM,LPARAM);
	afx_msg void OnSysChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnSpecialKey(WPARAM, LPARAM);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDLG_H__0B89D6C2_9D71_11D1_B39F_006008A70A6A__INCLUDED_)
