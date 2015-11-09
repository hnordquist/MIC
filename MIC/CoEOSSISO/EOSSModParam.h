/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
   conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
   conditions and the following disclaimer in the documentation and/or other materials provided
   with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
   may be used to endorse or promote products derived from this software without specific prior
   written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef EOSSMODPARAM_H
#define EOSSMODPARAM_H

// EOSSModParam.h : header file
//
#include "TPropertySheet.h"
#include "ccbutton.h"
#include "ISO.h"
#include "EOSSSafePS.h"

#define MBATVOLTL_SPIN_MAX	2000
#define MBATVOLTL_SPIN_MIN	0
#define MBATVOLTL_SPIN_DEF	1500

#define MBATVOLTC_SPIN_MAX	2000
#define MBATVOLTC_SPIN_MIN	0
#define MBATVOLTC_SPIN_DEF	1750

#define EBATVOLTL_SPIN_MAX	500
#define EBATVOLTL_SPIN_MIN	0
#define EBATVOLTL_SPIN_DEF	120

#define EBATVOLTC_SPIN_MAX	750
#define EBATVOLTC_SPIN_MIN	0
#define EBATVOLTC_SPIN_DEF	650

#define TIMEERROR_SPIN_MAX  600
#define TIMEERROR_SPIN_MIN	0
#define TIMEERROR_SPIN_DEF  60


/////////////////////////////////////////////////////////////////////////////
// CModParam dialog
class CEOSSModParam : public CPropertyPage
{
	DECLARE_DYNCREATE(CEOSSModParam)

// Construction
public:
	CEOSSModParam(UINT id, CISO* pParent, bool DoingConfig = false);
	CEOSSModParam();
	~CEOSSModParam();

	void SetPropertySheet(CPropertySheet *pPS, char *pName = NULL);
	void DoApply();
	void WriteIniFileDefaultSection();
	void SetEOSSEstimateListTime();
	void SetEOSSScheduleText();

	bool m_bCollectingParametersForNewISO;
	bool m_bAllowFinish;
	CTPropertySheet *m_pPS;
	CYellowButton	*m_pApplyButton;

	//COLORREF StaticColors[IDC_EOSSMODPARAMS_LAST-IDC_EOSSMODPARAMS_FIRST+1];

	//<<QA>> delete these??
	bool m_bSaved;
	//void SetText(UINT, char *);
	//void SetColor(UINT, COLORREF);

// Dialog Data
	//{{AFX_DATA(CEOSSModParam)
	enum { IDD = IDD_EOSSMODPARAMS_PAGE };
	CComboBox	m_cCombo_Offset;
	CComboBox	m_cCombo_Minutes;
	CComboBox	m_cCombo_Hours;
	CComboBox	m_cCombo_DOW;
	CComboBox	m_cCombo_DOM;
	CComboBox	m_cComboWhen;
	CSpinButtonCtrl	m_EOSSModParamsFileIDSpin;
	CEdit	m_EOSSModParamsFileID;
	CButton	m_bReaderLogging;
	CButton	m_bFullDataInterval;
	CEdit	m_EditInterval;

	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEOSSModParam)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:


protected:
	bool m_datasetup;
	bool m_bChange, m_bIntervalChange;
	CISO* m_pParent;

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
	void SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox = false);
	void ResetItem(char *szName, char *szItem, char *szDefault,
						  int dItemSpin, int dItem);

	// Generated message map functions
	//{{AFX_MSG(CEOSSModParam)
	afx_msg void OnModparamsApply();
	afx_msg void OnModparamsBrowse();
	afx_msg void OnDeltaposModparamsBatvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsBatvolthSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDeltaposModparamsMostSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeModparamsMaxInq();
	afx_msg void OnChangeModparamsMost();
	afx_msg void OnChangeModparamsFileID();
	afx_msg void OnChangeModparamsPort();
	afx_msg void OnChangeModparamsSaveloc();
	afx_msg void OnKillfocusModparamsFileid();
	virtual BOOL OnInitDialog();
	afx_msg void OnEditchangeComboWhen();
	afx_msg void OnSelchangeComboWhen();
	afx_msg void OnChangeEossmodparamsAccount();
	afx_msg void OnChangeEossmodparamsPw();
	afx_msg void OnSelchangeEossmodparamsDom();
	afx_msg void OnSelchangeEossmodparamsHours();
	afx_msg void OnSelchangeEossmodparamsMinutes();
	afx_msg void OnChangeEossmodparamsMainbatlow();
	afx_msg void OnChangeEossmodparamsMainbatcrit();
	afx_msg void OnChangeEossmodparamsEmerbatlow();
	afx_msg void OnChangeEossmodparamsEmerbatcrit();
	afx_msg void OnDeltaposEossmodparamsMainbatlowSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposEossmodparamsMainbatcritSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposEossmodparamsEmerbatlowSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposEossmodparamsEmerbatcritSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposEossmodparamsTimeerrorSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEossmodparamsTimeerror();
	afx_msg void OnEditchangeEossmodparamsOffsethours();
	afx_msg void OnBnClickedEossmodparamsFullIntervalCheck();
	afx_msg void OnChangeEossmodparamsInterval();
	afx_msg void OnBnClickedEossmodparamsReaderLog();
	afx_msg void OnKillfocusEossmodparamsInterval();

	//}}AFX_MSG
	public:
	afx_msg void OnModparamsReset();
	afx_msg void OnModparamsDefaultBut();
	afx_msg void OnModparamsDefault(bool SetPort = true);
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EOSSMODPARAM_H
