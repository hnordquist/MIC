//RenameISO.h 

#if !defined(AFX_RENAMEISO_H__137F4D3A_2806_4EAE_928F_621B63CB73C8__INCLUDED_)
#define AFX_RENAMEISO_H__137F4D3A_2806_4EAE_928F_621B63CB73C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenameISO.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRenameISO dialog

class CRenameISO : public CDialog
{
// Construction
public:
	CRenameISO(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRenameISO)
	enum { IDD = IDD_DIALOG_RENAME };
	CString m_cNewName;
	CString m_cCurrentName;
	CString m_cOriginalName;
	int     m_iNewNode;  // (was UINT) MIC_CB_028 to MIC 1.907 PJM 1/20/05
	CString m_cExistingNode;
	CString m_cOriginalNode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameISO)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRenameISO)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENAMEISO_H__137F4D3A_2806_4EAE_928F_621B63CB73C8__INCLUDED_)
