#if !defined(AFX_CONFIG2_H__F2C8A551_C51D_11D3_8FB4_006008A70A6A__INCLUDED_)
#define AFX_CONFIG2_H__F2C8A551_C51D_11D3_8FB4_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Config2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfig2 dialog

class CConfig2 : public CDialog
{
// Construction
public:
	CConfig2(CWnd* pParent = NULL);   // standard constructor

	void SetValues(int Width, int Height, int Columns, int Flags);
	void GetValues(int *Width, int *Height, int *Columns, int *Flags);
// Dialog Data
	//{{AFX_DATA(CConfig2)
	enum { IDD = IDD_DIALOG_CONFIG2 };
	CSpinButtonCtrl	m_pSpinFlagSize;
	CSpinButtonCtrl	m_pSpinColumns;
	CSpinButtonCtrl	m_pSpinHeight;
	CSpinButtonCtrl	m_pSpinWidth;
	CString	m_cWidth;
	CString	m_cHeight;
	CString	m_cColumns;
	CString	m_cFlagSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfig2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_dButtonWidth;
	int m_dButtonHeight;
	int m_dButtonsWide;
	int m_dButtonFlagSize;

	// Generated message map functions
	//{{AFX_MSG(CConfig2)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIG2_H__F2C8A551_C51D_11D3_8FB4_006008A70A6A__INCLUDED_)
