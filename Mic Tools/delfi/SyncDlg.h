// SyncDlg.h : header file
//

#if !defined(AFX_SYNCDLG_H__BB7D61A6_7350_11D2_94AB_EE5FE0791316__INCLUDED_)
#define AFX_SYNCDLG_H__BB7D61A6_7350_11D2_94AB_EE5FE0791316__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SystemTray.h"

#define NAME		"DelFi"
#define CONFIG		"CONFIGURATION"
#define AGE			"AGE"
#define INDIRECTORY	"INDIRECTORY"
#define DESTINATION	"DESTINATION"
#define CYCLETIME	"CYCLETIME"
#define MAXPAUSE	"MAXPAUSE"
#define SHOW		"SHOW"
#define CYCLETIMER	10000
#define INI_TIMER	20000
#define TIMER_MESSAGE (WM_USER+6)
#define HIDEDISP	  (WM_USER+7)

/////////////////////////////////////////////////////////////////////////////
// CSyncDlg dialog

class CSyncDlg : public CDialog
{
// Construction
public:
	CSyncDlg(CWnd* pParent = NULL);	// standard constructor

	char m_cCommandLine[_MAX_PATH];

// Dialog Data
	//{{AFX_DATA(CSyncDlg)
	enum { IDD = IDD_SYNC_DIALOG };
	CButton	m_pConfigButton;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	char	m_szIniFile[_MAX_DIR];
	bool	m_bPause;
	int		m_dPauseCnt;
	int		m_dMaxPause;
	bool	m_bShow;
	bool	m_bNewIni;
	COleDateTimeSpan m_Age;
	COleDateTimeSpan dCycleTime;
	CSystemTray m_SystemTray;
	// Generated message map functions
	//{{AFX_MSG(CSyncDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPause();
	afx_msg void OnButton2();
	afx_msg void OnButtonConfigure();
	afx_msg void OnButtonHidenow();
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void MsgFromTray(WPARAM WParam, LPARAM LParam);
	afx_msg void DoWork(WPARAM WParam, LPARAM LParam);
	void DoShow(bool);
	void CheckFiles(char *InDirectory, COleDateTimeSpan Age, COleDateTime TimeNow,
						  bool bConsiderArchiveBit, bool bDoSubDirectories);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCDLG_H__BB7D61A6_7350_11D2_94AB_EE5FE0791316__INCLUDED_)
