//JSRConfig2.h

// User: Moore        Date: 9/20/05    Time: 10:16a
// Repaired SCR00114: JSR-12 Apply buttons ambiguous.  In doing this,
// created a new class CSimpleYellowButton to replace CYellowButton.
// CYellowButton could not do multi-line buttons, so the JSR button
// to send the commands to the JSR, being multiline, did not have the
// capability to change color so it was not clear when sending the
// commands to the JSR was appropriate or allowed.
// 

#ifndef JSRCONFIG2_H
#define JSRCONFIG2_H

#include "JSRSafePS.h"
#include "SimpleYellowButton.h"  //SCR00114
#include "ISO.h"

///////////////////////////////////////////////////////////////////////////
// CJSRConfig2 dialog

class CJSRConfig2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CJSRConfig2)

// Construction
public:
	CJSRConfig2();
	CJSRConfig2(UINT id, CISO* pParent, bool DoingConfig = false);
	~CJSRConfig2();

//Variables
	bool m_bCollectingParametersForNewISO;
	CJSRPropertySheet *m_pPS;
	CISO* m_pParent;
	bool m_bInitDone;
	CSimpleYellowButton *m_pCameraApplyButton; //SCR00114

	void SetPropertySheet(CJSRPropertySheet *pPS, bool bDoingConfig);
	void DoApply();
	void SaveItem(char *szName, char *szItem, int dDlgItem);
	void OnJSRConfig(char *Name);

	void ResetAutoCloseTime();
	void BlockCamera();

// Dialog Data
	//{{AFX_DATA(CJSRConfig2)
	enum { IDD = IDD_JSRCONFIG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CJSRConfig2)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJSRConfig2)
	afx_msg void OnChangeConfigMost();
	afx_msg void OnChangeConfigCameraPorts();
	afx_msg void OnJSRConfig2Apply();
	afx_msg void OnJSRConfig2Default();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnJSRConfig2Current();
	afx_msg void OnJSRCameraApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTestport();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // JSRCONFIG2_H
