//PSRConfig2.h

#ifndef PSRCONFIG2_H
#define PSRCONFIG2_H

#include "PSRSafePS.h"
#include "SimpleYellowButton.h"  
#include "ISO.h"

///////////////////////////////////////////////////////////////////////////
// CPSRConfig2 dialog

class CPSRConfig2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPSRConfig2)

// Construction
public:
	CPSRConfig2();
	CPSRConfig2(UINT id, CISO* pParent, bool DoingConfig = false);
	~CPSRConfig2();

//Variables
	bool m_bCollectingParametersForNewISO;
	CPSRPropertySheet *m_pPS;
	CISO* m_pParent;
	bool m_bInitDone;
	CSimpleYellowButton *m_pCameraApplyButton; 

	void SetPropertySheet(CPSRPropertySheet *pPS, bool bDoingConfig);
	void DoApply();
	void SaveItem(char *szName, char *szItem, int dDlgItem);
	void OnPSRConfig(char *Name);

	void ResetAutoCloseTime();
	void BlockCamera();

// Dialog Data
	//{{AFX_DATA(CPSRConfig2)
	enum { IDD = IDD_PSRCONFIG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPSRConfig2)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPSRConfig2)
	afx_msg void OnChangeConfigMost();
	afx_msg void OnChangeConfigCameraPorts();
	afx_msg void OnPSRConfig2Apply();
	afx_msg void OnPSRConfig2Default();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPSRConfig2Current();
	afx_msg void OnPSRCameraApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTestport();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // PSRCONFIG2_H
