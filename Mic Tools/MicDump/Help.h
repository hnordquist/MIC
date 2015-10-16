#pragma once
#include "afxwin.h"


// Help dialog

class HelpDlg : public CDialog
{
	DECLARE_DYNAMIC(HelpDlg)

public:
	HelpDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~HelpDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString help;
	CEdit tertst;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
