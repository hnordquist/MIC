// WinDumpDlg.h : header file
//

#if !defined(AFX_WINDUMPDLG_H__64BAAE51_B8C8_4305_90CB_13909A65E55B__INCLUDED_)
#define AFX_WINDUMPDLG_H__64BAAE51_B8C8_4305_90CB_13909A65E55B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWinDumpDlgAutoProxy;

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlg dialog

class CWinDumpDlg : public CDialog
{
	DECLARE_DYNAMIC(CWinDumpDlg);
	friend class CWinDumpDlgAutoProxy;

// Construction
public:
	CWinDumpDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CWinDumpDlg();
	char m_szCmdLine[MAX_PATH*2];
	CString m_cMessage;

// Dialog Data
	//{{AFX_DATA(CWinDumpDlg)
	enum { IDD = IDD_WINDUMP_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinDumpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWinDumpDlgAutoProxy* m_pAutoProxy;
	enum Types {CHN,GRAND,MCA,ISR,JSR,PSR,VACOSS,BINARY,GPS,UNKNOWN} eThisType;
	HICON m_hIcon;
	BOOL CanExit();
	void DoIt(char* FileName, Types eThisType);
	void DoAFile(char* FileName);
	int  ReadALine(FILE* infile, char *szLineBuff, int iBufSize);


	// Generated message map functions
	//{{AFX_MSG(CWinDumpDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDUMPDLG_H__64BAAE51_B8C8_4305_90CB_13909A65E55B__INCLUDED_)
