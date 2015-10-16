#if !defined(AFX_IPADDRESSDLG_H__106A75C9_7078_49ED_B60D_E60453DEACEA__INCLUDED_)
#define AFX_IPADDRESSDLG_H__106A75C9_7078_49ED_B60D_E60453DEACEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IPAddressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIPAddressDlg dialog

class CIPAddressDlg : public CDialog
{
// Construction
public:
	CIPAddressDlg(CWnd* pParent = NULL);   // standard constructor
	char szAddress[32];
	char szMsg[128];
// Dialog Data
	//{{AFX_DATA(CIPAddressDlg)
	enum { IDD = IDD_DIALOG3 };
	CIPAddressCtrl	m_IPAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPAddressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIPAddressDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPADDRESSDLG_H__106A75C9_7078_49ED_B60D_E60453DEACEA__INCLUDED_)
