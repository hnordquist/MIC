#if !defined(AFX_EDITEOSSLIST_H__A09ACB70_5511_4320_A564_8F84EAB8F8F6__INCLUDED_)
#define AFX_EDITEOSSLIST_H__A09ACB70_5511_4320_A564_8F84EAB8F8F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditEOSSList.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CEditEOSSList dialog

class CEditEOSSList : public CDialog
{
// Construction
public:
	CEditEOSSList(CWnd* pParent = NULL);   // standard constructor
	void AdjustIndex();
	void AdjustButtons();
	CString m_czEOSSes;

	static const int maxEOSSCount;

// Dialog Data
	//{{AFX_DATA(CEditEOSSList)
	enum { IDD = IDD_DIALOG1 };
	CButton	m_cButtonMoveUp;
	CButton	m_cButtonMoveDown;
	CButton	m_cButtonDelete;
	CButton	m_cButtonAdd;
	CListCtrl	m_ListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditEOSSList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditEOSSList)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonMoveup();
	afx_msg void OnButtonMovedown();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITEOSSLIST_H__A09ACB70_5511_4320_A564_8F84EAB8F8F6__INCLUDED_)
