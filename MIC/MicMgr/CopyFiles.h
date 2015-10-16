
// User: Moore        Date: 11/14/05   Time: 3:19p
// Repaired DR00159 Not all drives are selectable in the Copy Files


#ifndef COPYFILES_H
#define COPYFILES_H

// CopyFiles.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CCopyFiles dialog

class CCopyFiles : public CDialog
{
// Construction
public:
	CCopyFiles(CWnd* pParent = NULL);
	FILE* m_pIPFile;
// Dialog Data
	//{{AFX_DATA(CCopyFiles)
	enum { IDD = IDD_COPYFILES1_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	public:
	virtual int DoModal(CFileMgr* pFileMgrParent);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCopyFiles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFileMgr* m_pFileMgrParent;
	COleDateTime theTime;
	CString m_pDateTime;
	CString m_pInspectorName;
	CString m_pInspectorID;
	CString m_pInspection;
	CString m_pDestination;
	CString m_pPrevious;
	HICON m_hInspectState;
	HICON m_hDumpState;
	HICON m_hMediaState;
	HICON m_hDestState;
	HICON m_hIconYellow1;
	HICON m_hIconYellow2;
	HICON m_hIconRed;
	HICON m_hIconGreen;
	void InspectionIconUpdate();
	int   m_dTotalBytesinBBM;
	int	  m_dToClose;
	bool  m_bDoingDoFileCopy;

	// Generated message map functions
	//{{AFX_MSG(CCopyFiles)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnDrive1Radio();
	afx_msg void OnDrive2Radio();
	afx_msg void OnDrive3Radio();
	afx_msg void OnDrive4Radio();
	afx_msg void OnDrive5Radio();
	afx_msg void OnDrive6Radio();
	afx_msg void OnDrive7Radio();
	afx_msg void OnDrive8Radio();
	afx_msg void OnChangeInspectionEdit();
	afx_msg void OnChangeInspectoridEdit();
	afx_msg void OnChangeInspectornameEdit();
	afx_msg void OnDrivedefRadio();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTillcloseButton();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonCfBrowse();  //DR00159
	afx_msg LRESULT OnQueryCancelAutoPlay(WPARAM wParam, LPARAM lParam);//DR00175
	//}}AFX_MSG
	void OnDriveRadio(UINT,int);
	BOOL OnDeviceChange(UINT nEventType, DWORD dwData);	//DR00175
	void TestDrive();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // COPYFILES_H
