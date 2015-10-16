//HHMRConfig2.h

#ifndef HHMRCONFIG2_H
#define HHMRCONFIG2_H

#include "HHMRSafePS.h"
#include "SimpleYellowButton.h"  
#include "ISO.h"

///////////////////////////////////////////////////////////////////////////
// CHHMRConfig2 dialog

class CHHMRConfig2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CHHMRConfig2)

// Construction
public:
	CHHMRConfig2();
	CHHMRConfig2(UINT id, CISO* pParent, bool DoingConfig = false);
	~CHHMRConfig2();

//Variables
	bool m_bCollectingParametersForNewISO;
	CHHMRPropertySheet *m_pPS;
	CISO* m_pParent;
	bool m_bInitDone;
	CSimpleYellowButton *m_pCameraApplyButton; 

	void SetPropertySheet(CHHMRPropertySheet *pPS, bool bDoingConfig);
	void DoApply();
	void SaveItem(char *szName, char *szItem, int dDlgItem);
	void OnHHMRConfig(char *Name);

	void ResetAutoCloseTime();
	void BlockCamera();

// Dialog Data
	//{{AFX_DATA(CHHMRConfig2)
	enum { IDD = IDD_HHMRCONFIG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHHMRConfig2)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHHMRConfig2)
	afx_msg void OnChangeConfigMost();
	afx_msg void OnChangeConfigCameraPorts();
	afx_msg void OnHHMRConfig2Apply();
	afx_msg void OnHHMRConfig2Default();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHHMRConfig2Current();
	afx_msg void OnHHMRCameraApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTestport();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // HHMRCONFIG2_H
