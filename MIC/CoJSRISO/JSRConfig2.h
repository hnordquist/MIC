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
