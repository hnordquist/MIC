// SetupData.h

#if !defined(AFX_SETUPDATA_H__9B6BF85B_A45B_4102_A7EB_807B89700300__INCLUDED_)
#define AFX_SETUPDATA_H__9B6BF85B_A45B_4102_A7EB_807B89700300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <io.h>
#include <direct.h>
#include "resource.h"
#include "TPropertySheet.h"
#include "ListCtrlStyled.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupData property page

class CSetupData : public CPropertyPage
{
	DECLARE_DYNCREATE(CSetupData)

// Construction
public:
	CSetupData();
	CSetupData(CWnd* pParent, char *inifile);
	~CSetupData();

	void SetPropertySheet(CPropertySheet *pPS, char *pName = NULL);
	void ResetColors();
	void UpdateColors(COLORREF* cColors);
	CTPropertySheet *m_pPS;
	void SetVersion(char* szVersion){strcpy(m_szVersion,szVersion);};
//	void IssuePrint(FILE *outfile,UINT iWidth=32);
	CString GetMatchString(){return m_cMatchString;};

// property page Data
	//{{AFX_DATA(CSetupData)
	enum { IDD = IDD_UGRANDSETUP_DIALOG };
	CButton	m_cEditSelected;
	CListCtrlStyled* m_pList;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupData)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void DoApply();
	void AddSetParam(char*szDesc,char*szIniName,char*szSetting,char*szCommand,unsigned int uLength);
	bool CurrentSetParam(char*szIniName,char*szCurrent,bool bMatch=true, bool bTextOnly=true);
	void CurrentSetParamColor(char*szIniName,char*szCurrent,bool bRed);
	char m_szIniFile[_MAX_PATH];

protected:
	CString m_cMatchString;
	char m_szVersion[16];
	char m_pName[32];
	CWnd* m_pParent;
	void SaveAll(bool bDoingConfig);

	// Generated message map functions
	//{{AFX_MSG(CSetupData)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonEditselection();
	afx_msg void OnButtonSaveall();
	afx_msg void OnButtonLoaddefault();
	afx_msg void OnDblclkListSet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSet(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDATA_H__9B6BF85B_A45B_4102_A7EB_807B89700300__INCLUDED_)
