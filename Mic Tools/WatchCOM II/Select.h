#if !defined(AFX_SELECT_H__A0425F32_DBE0_11D2_B4C4_006008A70A6A__INCLUDED_)
#define AFX_SELECT_H__A0425F32_DBE0_11D2_B4C4_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Select.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelect dialog

class CSelect : public CDialog
{
// Construction
public:
	CSelect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelect)
	enum { IDD = IDD_DIALOG1 };
	CIPAddressCtrl	m_cIPAddress;
	CListBox	m_pListBox;
	UINT	m_uiTCPPort;
	//}}AFX_DATA
	int m_dParity;
	int m_dBaudRate;
	int m_dStopBits;
	int m_dDataBits;
	char m_szDestination[_MAX_PATH];
	char m_szFileName[_MAX_PATH];
	char m_szPort[16];
	char m_szIPAddress[32];
	bool m_bDeactivate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelect)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelect)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEditLongname();
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonDeactivate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECT_H__A0425F32_DBE0_11D2_B4C4_006008A70A6A__INCLUDED_)
