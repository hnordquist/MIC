// DSPECModParam.h

#ifndef DSPECMODPARAM_H
#define DSPECMODPARAM_H

#include "TPropertySheet.h"
#include "ccbutton.h"
#include "resource.h"
#include "ISO.h"
/////////////////////////////////////////////////////////////////////////////
// CDSPECModParam dialog
class CDSPECModParam : public CPropertyPage
{
	DECLARE_DYNCREATE(CDSPECModParam)

// Construction
public:
	CDSPECModParam(UINT id, CISO* pParent, bool DoingConfig = false);
	CDSPECModParam();
	~CDSPECModParam();

	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void SetPropertySheet(CPropertySheet *pPS);

	void SaveSetup();
	bool m_bCollectingParametersForNewISO;
	bool m_bSaved;
	bool m_bAllowFinish;
	CTPropertySheet *m_pPS;
	CYellowButton	*m_pApplyButton;

	COLORREF StaticColors[IDC_DSPECMODPARAMS_LAST-IDC_DSPECMODPARAMS_FIRST+1];
	void IssuePrint(FILE *outfile);

	void SaveValuesToINIFile(char *name);
	void ResetAllButPort(char *ini_section_name);
	void WriteIniFileDefaultSection();

// Dialog Data
	//{{AFX_DATA(CDSPECModParam)
	enum { IDD = IDD_DSPECMODPARAMS_PAGE };
//	CString	m_cSaveLoc;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDSPECModParam)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnInitDialog();
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void DoApply();

protected:
	bool m_datasetup;
	bool m_bChange;
	CISO* m_pParent;
	void MakeMyPath(const char *path);

	void DoSpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, 
		int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above);
	void DoSpinI(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM);
	void DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, 
		int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above);

	void ResetItem(char *szName, char *szItem, char*szDefault,
		int dItem,int dMin, int dMax, int dTextItem);
	void ResetItemI(char *szName, char *szItem, char*szDefault,
		int dItem,int dMin, int dMax, int dTextItem);
	void ResetItem(char *szName,char *szItem, int dDefault,
		int dItem, int dMin, int dMax);
	void ResetItem(char *szName, char *szItem, int dDefault,
		int dItem);
	void ResetItem(char *szName,char *szItem, char *szDefault, 
		int dItem);
	void SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox = false);
	void ResetItem(char *szName, char *szItem, char *szDefault,
						  int dItemSpin, int dItem);

public:
	// Generated message map functions
	//{{AFX_MSG(CDSPECModParam)
	afx_msg void OnModparamsReset();
	afx_msg void OnModparamsApply();
	afx_msg void OnModparamsBrowse();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnModparamsDefaultBut();
	afx_msg void OnModparamsDefault(bool SetPort = true);

	//Commfail
	afx_msg void OnDeltaposModparamsCommfailSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Fileid
	afx_msg void OnChangeModparamsFileid();
	afx_msg void OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusModparamsFileid();
	//HV fail
	afx_msg void OnDeltaposModparamsHVfailSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//HV high error limit
	afx_msg void OnDeltaposModparamsHVhigherrorlimitSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//HV low error limit
	afx_msg void OnDeltaposModparamsHVlowerrorlimitSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Longdwell
	afx_msg void OnDeltaposModparamsLongdwellSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Maxcycle is constant at 1 second
	//Maxpause
	afx_msg void OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Maxstatus
	afx_msg void OnDeltaposModparamsMaxstatusSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Node
	//Port
	afx_msg void OnChangeModparamsPort();
	//Realtimedwell
	afx_msg void OnDeltaposModparamsRealtimedwellSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Saveloc
	afx_msg void OnChangeModparamsSaveloc();
	//Shortdwell
	afx_msg void OnDeltaposModparamsShortdwellSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Slowreset
	afx_msg void OnDeltaposModparamsSlowresetSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Threshold
	afx_msg void OnDeltaposModparamsThresholdSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Timeout
	afx_msg void OnDeltaposModparamsTimeoutSpin(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnDeltaposModparamsMostSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeModparamsMost();

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // DSPECMODPARAM_H
