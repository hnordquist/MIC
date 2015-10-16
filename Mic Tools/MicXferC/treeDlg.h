// treeDlg.h : header file
//

#if !defined(AFX_TREEDLG_H__A49AA586_D720_11D3_BBD3_00A024DA5B93__INCLUDED_)
#define AFX_TREEDLG_H__A49AA586_D720_11D3_BBD3_00A024DA5B93__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "FileSpec.h"
#include "Schedule.h"
#include "GetNumeric.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeDlg dialog

class CTreeDlg : public CDialog
{
// Construction
public:
	CTreeDlg(CWnd* pParent = NULL);	// standard constructor
	~CTreeDlg();
	char m_cCommandLine[_MAX_PATH];
	void UpdateDisplay();
	HTREEITEM InsertItem(char *szGroup,char *szItem, char *szPreText, HTREEITEM hItem, DWORD dwType);
	HTREEITEM InsertSet(char *szName);
	void InsertItemFromTos(char *szSetName, HTREEITEM hItem);
	void WriteSection(char*,HTREEITEM);

// Dialog Data
	//{{AFX_DATA(CTreeDlg)
	enum { IDD = IDD_TREE_DIALOG };
	CButton	m_ButtonAdd;
	CButton	m_ButtonDelete;
	CStatic	m_StaticImage;
	CButton	m_ButtonEdit;
	CTreeCtrl	m_Tree;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hIconCheck;
	HICON m_hIconQuest;
	HICON m_hIconWait;
	char m_szIniFile[_MAX_PATH];
	CScrollBar m_Grip;
	CFileSpec m_FileSpec;
	CSchedule m_Schedule;
	CGetNumeric m_GetNumber;
	bool m_bInitialized;
	CRect m_OldRect;
	CRect m_OldEdit;
	HTREEITEM m_CONFIG;
	HTREEITEM m_CALENDAR;
	HTREEITEM m_CEV;
	HTREEITEM m_PFM;
	HTREEITEM m_ACQUIRE;
	HTREEITEM m_JPG;
	HTREEITEM m_OTHER;


	// Generated message map functions
	//{{AFX_MSG(CTreeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonReload();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonAdd();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEDLG_H__A49AA586_D720_11D3_BBD3_00A024DA5B93__INCLUDED_)
