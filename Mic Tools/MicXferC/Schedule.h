/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-040. This software was exported from the United States in accordance with the Export
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
#if !defined(AFX_SCHEDULE_H__9DE89E24_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_)
#define AFX_SCHEDULE_H__9DE89E24_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Schedule.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSchedule dialog

class CSchedule : public CDialog
{
// Construction
public:
	CSchedule(CWnd* pParent = NULL);   // standard constructor
	void Setup(int,int,int,int,int,int,int,int);
// Dialog Data
	//{{AFX_DATA(CSchedule)
	enum { IDD = IDD_DIALOG_SCHEDULE };
	CEdit	m_pEditYear;
	CEdit	m_pEditSeconds;
	CEdit	m_pEditMonth;
	CEdit	m_pEditMinutes;
	CEdit	m_pEditHours;
	CEdit	m_pEditDay;
	CListBox	m_pSchedule;
	CListBox	m_pWeekDay;
	CSpinButtonCtrl	m_pSpinYear;
	CSpinButtonCtrl	m_pSpinSeconds;
	CSpinButtonCtrl	m_pSpinMonth;
	CSpinButtonCtrl	m_pSpinMinutes;
	CSpinButtonCtrl	m_pSpinHours;
	CSpinButtonCtrl	m_pSpinDay;
	CString	m_cWeekDay;
	CString	m_cSchedule;
	int		m_iDay;
	int		m_iHours;
	int		m_iMinutes;
	int		m_iMonth;
	int		m_iSeconds;
	int		m_iYear;
	//}}AFX_DATA

	int m_dSched;
	int m_dWeekd;
	int m_dYear;
	int m_dMonth;
	int m_dDay;
	int m_dHours;
	int m_dMinutes;
	int m_dSeconds;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSchedule)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSchedule)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListSchedule();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULE_H__9DE89E24_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_)
