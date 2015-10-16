//ReOrganize.h 

#if !defined(AFX_REORGANIZE_H__A52FDA72_EDA5_476B_80B9_5B5F9D34F59F__INCLUDED_)
#define AFX_REORGANIZE_H__A52FDA72_EDA5_476B_80B9_5B5F9D34F59F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReOrganize.h : header file
//

#include "mtreectl.h"
#include "globals.h"
#include "resource.h"
#include "COM_Utilities.h"
/////////////////////////////////////////////////////////////////////////////
// CReOrganize dialog

class CReOrganize : public CDialog
{
// Construction
public:
	CReOrganize(CWnd* pParent);

// Dialog Data
	//{{AFX_DATA(CReOrganize)
	enum { IDD = IDD_REORG_DLG };
	CMyTreeCtrl m_cTreeView;
	CButton	  m_cHideWatch;
	CListBox  m_cListBox;
	CButton	  m_cMoveUp;
	CButton	  m_cMoveDown;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReOrganize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DoReorg();

	BOOL MoveRow(int from, int to);
	BOOL CopyRow(int from, int to);
	CImageList* pImageList;

	CString m_cOriginalNames[MAX_INSTRUMENTS];
	int	m_iOriginalNodes[MAX_INSTRUMENTS];
	int m_iNewNodes[MAX_INSTRUMENTS];
	HTREEITEM m_hComItemDrop;
	// Generated message map functions
	//{{AFX_MSG(CReOrganize)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMoveDown();
	afx_msg void OnSelchangeList1();
	afx_msg void OnMoveUp();
	virtual BOOL OnInitDialog();
	afx_msg void OnOk2();
	afx_msg void OnCheckHidewatch();
	afx_msg void OnRenameSelected();
	afx_msg void OnRenameRevert();
	afx_msg void OnBegindragTreeView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList1();
	afx_msg void OnButtonRegister();
	//}}AFX_MSG
	afx_msg LRESULT OnPreDrop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPostDrop(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REORGANIZE_H__A52FDA72_EDA5_476B_80B9_5B5F9D34F59F__INCLUDED_)
