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
// MCAModParam.h

#ifndef MCAMODPARAM_H
#define MCAMODPARAM_H

#include "TPropertySheet.h"
#include "ccbutton.h"
#include "ISO.h"

#define MCA_DEFAULT		"MCA Default"

/////////////////////////////////////////////////////////////////////////////
// CModParam dialog
class CMCAModParam : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCAModParam)

// Construction
public:

	CMCAModParam(UINT id, CISO* pParent, bool DoingConfig = false);
	CMCAModParam();
	~CMCAModParam();

	//void SetText(UINT, char *);
	//void SetColor(UINT, COLORREF);

	void SetPropertySheet(CPropertySheet *pPS, char *pName = NULL);
	void DoApply();
	void WriteIniFileDefaultSection();

	bool m_bCollectingParametersForNewISO;
	bool m_bSaved;
	bool m_bAllowFinish;
	CTPropertySheet *m_pPS;
	CYellowButton	*m_pApplyButton;
	COLORREF StaticColors[IDC_MODPARAMS_LAST-IDC_MODPARAMS_FIRST+1];

// Dialog Data
	//{{AFX_DATA(CMCAModParam)
	enum { IDD = IDD_MCAMODPARAMS_PAGE };
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMCAModParam)
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
	bool m_bChange;
	int  m_iSpectrumFileType;
	CISO* m_pParent;

	void DoSpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, 
		int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above);
	void DoSpinI(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM);
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
	//{{AFX_MSG(CMCAModParam)
	afx_msg void OnModparamsApply();
	afx_msg void OnModparamsBrowse();
	afx_msg void OnDeltaposModparamsBatvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsBatvolthSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsSupM15hSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsSupM15lSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsSupP05hSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsSupP05lSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsSupP15hSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsSupP15lSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsMaxbbmSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDeltaposModparamsMostSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeModparamsMost();
	afx_msg void OnChangeModparamsPort();
	afx_msg void OnChangeModparamsSaveloc();
	afx_msg void OnKillfocusModparamsFileid();
	afx_msg void OnChangeModparamsFileID();
	afx_msg void OnModparamsDatetime();
	afx_msg void OnCheckAllowautodate();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboSpectrafile();
	//}}AFX_MSG
	public:
	afx_msg void OnModparamsDefaultBut();
	afx_msg void OnModparamsDefault(bool SetPort = true);
	afx_msg void OnModparamsReset();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //MCAMODPARAM_H

