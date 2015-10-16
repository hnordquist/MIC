#if !defined(AFX_ADDEDIT_H__9F2A21F1_D4C3_11D3_8FB5_006008A70A6A__INCLUDED_)
#define AFX_ADDEDIT_H__9F2A21F1_D4C3_11D3_8FB5_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AddEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddEdit dialog

class CAddEdit : public CDialog
{
// Construction
public:
	CAddEdit(CWnd* pParent = NULL);   // standard constructor

	void SetTitle(char*title) {strcpy(m_szTitle,title);};
	void SetUp(UINT Days, UINT Hours, UINT Minutes, UINT Seconds, BOOL DoSub, BOOL ConsiderArchive, char* Path)
	{
		m_dCycDays = Days; 
		m_dCycHours = Hours; 
		m_dCycMinutes = Minutes; 
		m_dCycSeconds = Seconds; 
		m_bDoSub = DoSub; 
		m_bConsiderArchive = ConsiderArchive;
		m_cPath = Path;
	};
	void CheckIt();
	char m_szTitle[128];
// Dialog Data
	//{{AFX_DATA(CAddEdit)
	enum { IDD = IDD_DIALOG2 };
	CButton	m_pConsiderArchive;
	CButton	m_pDoSub;
	CEdit	m_pPath;
	CSpinButtonCtrl	m_pCycSeconds;
	CSpinButtonCtrl	m_pCycMinutes;
	CSpinButtonCtrl	m_pCycHours;
	CSpinButtonCtrl	m_pCycDays;
	UINT	m_dCycDays;
	UINT	m_dCycHours;
	UINT	m_dCycMinutes;
	UINT	m_dCycSeconds;
	CString	m_cPath;
	BOOL	m_bConsiderArchive;
	BOOL	m_bDoSub;
	//}}AFX_DATA

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddEdit)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDEDIT_H__9F2A21F1_D4C3_11D3_8FB5_006008A70A6A__INCLUDED_)
