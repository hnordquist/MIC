#if !defined(AFX_CONFIGURE_H__6D2A6AE1_D427_11D3_8FB5_006008A70A6A__INCLUDED_)
#define AFX_CONFIGURE_H__6D2A6AE1_D427_11D3_8FB5_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Configure.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigure dialog

class CConfigure : public CDialog
{
// Construction
public:
	CConfigure(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigure)
	enum { IDD = IDD_DIALOG1 };
	CButton	m_pDeleteButton;
	CButton	m_pEditButton;
	CListCtrl	m_pList;
	CButton	m_pCheckHide;
	CSpinButtonCtrl	m_pSpinPause;
	CSpinButtonCtrl	m_pSpinSeconds;
	CSpinButtonCtrl	m_pSpinMinutes;
	CSpinButtonCtrl	m_pSpinHours;
	CSpinButtonCtrl	m_pSpinDays;
	UINT	m_dCycDays;
	UINT	m_dCycHours;
	UINT	m_dCycMinutes;
	UINT	m_dCycSeconds;
	UINT	m_dPause;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigure)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CString m_IniFile;
protected:
	int m_dSelectedItem;
	// Generated message map functions
	//{{AFX_MSG(CConfigure)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonEdit();
	afx_msg void OnSelchangeList1();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURE_H__6D2A6AE1_D427_11D3_8FB5_006008A70A6A__INCLUDED_)
