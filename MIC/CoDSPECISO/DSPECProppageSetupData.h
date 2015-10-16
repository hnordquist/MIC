
#ifndef DSPECPROPPAGESETUPDATA_H
#define DSPECPROPPAGESETUPDATA_H

// DSPECProppageSetupData.h : header file
//
#include <io.h>
#include <direct.h>

#include "DSPECSafePS.h"
//#include "TPropertySheet.h"
#include "ListCtrlStyled.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDSPECProppageSetupData property page

class CDSPECProppageSetupData : public CPropertyPage
{
	DECLARE_DYNCREATE(CDSPECProppageSetupData)

// Construction
public:
//	CDSPECProppageSetupData(unsigned int id, CWnd* pParent, bool DoingConfig);
	CDSPECProppageSetupData(CWnd* pParent = NULL);
	CDSPECProppageSetupData(CWnd* pParent, char *infile);
	~CDSPECProppageSetupData();

	void SetPropertySheet(CPropertySheet *pPS, char *pName = NULL);
	void ResetColors();
//	CTPropertySheet *m_pPS;
	CDSPECSafePropertySheet *m_pPS;
	void IssuePrint(FILE *outfile);
	CDSPECInst *m_pParent;

// property page Data
	//{{AFX_DATA(CDSPECProppageSetupData)
	enum { IDD = IDD_DSPEC_SETTINGS_DIALOG };
	CButton	m_cEditSelected;
	CListCtrlStyled	m_cList;  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPECProppageSetupData)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void DoApply();

	void AddSetParam(
		char*szDesc,
		char*szIniName,
		char*szSetting,
		char*szCommand,
		char*szAllowedValues,
		unsigned int uLength);

	void CurrentSetParam(char*szIniName,char*szCurrent,bool bMatch=true);

protected:
	char m_pName[32];
//	CWnd* m_pParent;
	char m_szIniFile[_MAX_PATH];
	void SaveAll(bool bDoingConfig);

	// Generated message map functions
	//{{AFX_MSG(CDSPECProppageSetupData)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonEditselection();
	afx_msg void OnButtonSaveallreported();
	afx_msg void OnButtonLoaddefault();
	afx_msg void OnButtonRefreshreported();
	afx_msg void OnButtonSenddesired();
	afx_msg void OnDblclkListSet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSet(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // DSPECPROPPAGESETUPDATA_H
