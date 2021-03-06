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
