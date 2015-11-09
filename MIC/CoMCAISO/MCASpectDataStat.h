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
#ifndef MCASPECDATASTAT_H
#define MCASPECDATASTAT_H

// MCASpecDataStat.h

#include "MCASafePS.h"
#include "MCAXYCtrl.h"
//#include "MICStdSpc.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CMCASPECDATASTAT_HDataStat dialog

class CMCASpecDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCASpecDataStat)

// Construction
public:
	CMCASpecDataStat(UINT id, CISO *parent);
	CMCASpecDataStat();
	~CMCASpecDataStat();

	void SetReceivingSpectraName(LPCSTR szString);
	void AddNewSpectraName(LPCSTR szString);

	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CMCAPropertySheet*);
	COLORREF StaticColors[IDC_MCASPECDATA_LAST-IDC_MCASPECDATA_FIRST+1];
	void IssuePrint(FILE *outfile);

//	bool ProcessSpectrumBufferForRealTimeGraphic(CMICStdSpc *pspec);
	bool ProcessSpectrumBufferForRealTimeGraphic(LONG *buf, UINT uiChans);
	int m_iShowSpectra;
	CISO *m_pParent;

//	void SetInhibitRealtimeDataButton(bool paused);

	CMCAXYCtrl   m_XYCtrl;

	// Dialog Data
	//{{AFX_DATA(CMCASpecDataStat)
	enum { IDD = IDD_MCASPECSTATUS_PAGE };
	CListCtrl	m_cListHistory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MCASpecCDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bBlockIncoming;

	CMCAPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CMCASpecDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowSpectra();
	afx_msg void OnButtonRefresh();
	afx_msg void OnClickListSpectra(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSpectra(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // MCASPECDATASTAT_H

