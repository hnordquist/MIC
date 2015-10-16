#ifndef DELETEINS_H
#define DELETEINS_H

// DeleteIns.h : header file
//
#include "globals.h"
#include "COM_Utilities.h"
#include "ccbutton.h"
/////////////////////////////////////////////////////////////////////////////
// CDeleteIns dialog

class CDeleteIns : public CDialog
{
// Construction
public:
	CDeleteIns(CWnd* pParent);
	~CDeleteIns();
// Dialog Data
	//{{AFX_DATA(CDeleteIns)
	enum { IDD = IDD_DELETEINS_DIALOG };
	CListBox	m_DeleteList;
	int			m_dSelect;
	CString		Data;
	//}}AFX_DATA
	char *			m_DeleteData[MAX_INSTRUMENTS];
	int				m_dCount;
	CYellowButton	*m_pDeleteButton;
	void AddString(char *);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteIns)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteIns)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeDeleteList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // DELETEINS_H