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
#ifndef MCADDATA_H
#define MCADDATA_H

// CMCADData.h : header file
//

#include "MCASafePS.h"
#include "JulianT.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CMCACDData dialog
class CMCACDData : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCACDData)

// Construction
public:
	CMCACDData(UINT id, CISO *parent);
	CMCACDData();
	~CMCACDData();

	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CMCAPropertySheet*);
	COLORREF StaticColors[IDC_MCACDDATA_LAST-IDC_MCACDDATA_FIRST+1];
	CISO *m_pParent;

// Dialog Data
	//{{AFX_DATA(CMCACDData)
	enum { IDD = IDD_MCACDDATA_PAGE };
	CButton	m_RadioConfig10;
	CButton	m_RadioConfig11;
	CButton	m_RadioConfig12;
	CButton	m_RadioConfig13;
	CButton m_RadioConfig14;
	CStatic	m_StaticConfig10;
	CStatic	m_StaticConfig14;
	CStatic	m_StaticConfig13;
	CStatic	m_StaticConfig12;
	CStatic	m_StaticConfig11;
	CEdit	m_pEditContents;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMCACDData)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString Text;
	CJulianTime JulianT;
	CMCAPropertySheet *m_pPropertySheet;
	char temp1[256];
	char temp2[32];
	CFont EditFont;
	void Concat(char* text, char* Source, int Start, int Length);
	// Generated message map functions
	//{{AFX_MSG(CMCACDData)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRadioConfig10();
	afx_msg void OnRadioConfig11();
	afx_msg void OnRadioConfig12();
	afx_msg void OnRadioConfig13();
	afx_msg void OnRadioConfig14();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetfocusEditContents();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // MCADDATA_H
