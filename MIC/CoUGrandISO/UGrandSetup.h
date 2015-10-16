// UGrandSetup.h

#if !defined(AFX_UGRANDSETUP_H__C9D381CF_EF92_40A3_9779_423D6AC4725A__INCLUDED_)
#define AFX_UGRANDSETUP_H__C9D381CF_EF92_40A3_9779_423D6AC4725A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UGrandSetup.h : header file
//
#include "ListCtrlStyled.h"

/////////////////////////////////////////////////////////////////////////////
// CUGrandSetup dialog

class CUGrandSetup : public CDialog
{
// Construction
public:
	CUGrandSetup(CWnd* pParent, char *inifile);
	~CUGrandSetup();
	char m_szName[32];
	char m_szIniFile[_MAX_PATH];
	char m_szVersion[16];

// Dialog Data
	//{{AFX_DATA(CUGrandSetup)
	enum { IDD = IDD_UGRANDSETUP_DIALOG };
	CButton	m_cEditSelected;
	CListCtrlStyled	*m_pList;
	//}}AFX_DATA
	void SaveSetup();
	void AddSetParam(char*szDesc,char*szIniName,char*szSetting,char*szCommand,unsigned int uLength);
	void CurrentSetParam(char*szIniName,char*szCurrent,bool bMatch=true, bool bTextOnly=true);
	void SetVersion(char* szVersion){strcpy(m_szVersion,szVersion);};
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUGrandSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd* m_pParent;
	void SaveAll(bool bDoingConfig);
	// Generated message map functions
	//{{AFX_MSG(CUGrandSetup)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonEditselection();
	afx_msg void OnButtonSaveall();
	afx_msg void OnButtonHide();
	afx_msg void OnButtonLoadDefault();
	afx_msg void OnDblclkListSet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSet(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UGRANDSETUP_H__C9D381CF_EF92_40A3_9779_423D6AC4725A__INCLUDED_)
