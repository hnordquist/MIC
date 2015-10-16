// HHMRModParam.h

#ifndef HHMRMODPARAM_H
#define HHMRMODPARAM_H

#include <io.h>
#include <direct.h>
#include "TPropertySheet.h"
#include "ISO.h"
#include "SimpleYellowButton.h" 

#define HHMR_DEFAULT "HHMR Default"
///////////////////////////////////////////////////////////////////////////
// CModParam dialog
class CHHMRModParam : public CPropertyPage
{
	DECLARE_DYNCREATE(CHHMRModParam)

// Construction
public:
	CHHMRModParam(UINT id, CISO* pParent, bool DoingConfig = false);
	CHHMRModParam();
	~CHHMRModParam();

//Variables
	bool m_bCollectingParametersForNewISO;
	bool m_bSaved;
	//On configuring a new HHMR, m_pPropertySheet is CTPropertySheet.
	//All other times, m_pPropertySheet is CHHMRPropertySheet.
	CTPropertySheet *m_pPS;
	COLORREF StaticColors[IDC_HHMRMODPARAMS_LAST-IDC_HHMRMODPARAMS_FIRST+1];
	bool m_bDataSetup;
	bool m_bChange;
	CISO* m_pParent;
	CSimpleYellowButton	*m_pApplyButton;
	CSimpleYellowButton *m_pApplyHHMRButton;  

//Functions
	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void SetPropertySheet(CPropertySheet *pPS);//, char *pName = NULL);
	void IssuePrint(FILE *outfile);
	void WriteIniFileDefaultSection();
	void DoApply();
	void ResetAutoCloseTime();

// Dialog Data
	//{{AFX_DATA(CHHMRModParam)
	enum { IDD = IDD_HHMRMODPARAMS_PAGE };
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHHMRModParam)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

protected:

	void DoSpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, 
		int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above);
	void DoSpinI(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM);
	void DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, 
		int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above);
	void ResetItem(char *szName,char *szItem,char*szDefault,
		int dItem,int dMin, int dMax, int dTextItem);
	void ResetItemI(char *szName,char *szItem,char*szDefault,
		int dItem,int dMin, int dMax, int dTextItem);
	void ResetItem(char *szName,char *szItem, int dDefault,
		int dItem,int dMin, int dMax);
	void ResetItem(char *szName,char *szItem, int dDefault,
		int dItem);
	void ResetItem(char *szName,char *szItem, char *szDefault, 
		int dItem);
	void ResetItemL(char *szName,char *szItem, int szDefault, 
		int dItem);
	void SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox = false);
	void ResetItem(char *szName, char *szItem, char *szDefault,
						  int dItemSpin, int dItem);

	// Generated message map functions
	//{{AFX_MSG(CHHMRModParam)
	afx_msg void OnModparamsApply();
	afx_msg void OnModparamsApplyHHMR();
	afx_msg void OnModparamsBrowse();
	afx_msg void OnDeltaposModparamsHighvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsHighvolthSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDeltaposModparamsMostSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeModparamsMost();
	afx_msg void OnChangeModparamsCT();
	afx_msg void OnChangeModparamsSaveLoc();
	afx_msg void OnKillfocusModparamsFileid();
	afx_msg void OnChangeModparamsFileID();
	afx_msg void OnChangeModparamsPort();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	public:
	afx_msg void OnModparamsDefaultBut();
	afx_msg void OnModparamsDefault(bool SetPort = true);
	afx_msg void OnModparamsReset();

	DECLARE_MESSAGE_MAP()
};

#endif // HHMRMODPARAM_H