#ifndef MCACONFIG2_H
#define MCACONFIG2_H

// MCAConfig2.h : header file
//
#include "TPropertySheet.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CMCAConfig2 dialog

class CMCAConfig2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCAConfig2)

// Construction
public:
	CMCAConfig2();
	CMCAConfig2(UINT id, CISO* pParent, bool DoingConfig = false);
	~CMCAConfig2();

	void SetPropertySheet(CTPropertySheet *pPS);
	CTPropertySheet *m_pPS;
	void DoApply();
	bool m_bAllowFinish;

// Dialog Data
	//{{AFX_DATA(CMCAConfig2)
	enum { IDD = IDD_MCACONFIG_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMCAConfig2)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SaveItem(char *szName, char *szItem, int dDlgItem);
	CISO* m_pParent;
	// Generated message map functions
	//{{AFX_MSG(CMCAConfig2)
	afx_msg void OnMcaConfig2Apply();
	afx_msg void OnMcaConfig2Reset();
	afx_msg void OnMcaConfig2Default();
	virtual BOOL OnInitDialog();
	afx_msg void OnMcaconfigClearall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // MCACONFIG2_H
