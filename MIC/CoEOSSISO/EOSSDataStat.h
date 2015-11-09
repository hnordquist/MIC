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
//EOSSDataStat.h
#ifndef EOSSDATASTAT_H
#define EOSSDATASTAT_H

#include "EOSSSafePS.h"
#include "eossdefs.h"
#include "ISO.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CEOSSDataStat dialog
class CEOSSDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CEOSSDataStat)

// Construction
public:
	CEOSSDataStat(UINT id, CISO *parent);
	CEOSSDataStat();
	~CEOSSDataStat();
	void SetText(int, char *);
	char *GetText(int, char *,int);
	void IssuePrint(FILE *outfile);
	void SetColor(int, COLORREF);
	void ResetColors();
	CISO *m_pParent;
	void SetPropertySheet(CEOSSPropertySheet*);
	COLORREF StaticColors[IDC_EOSSDATA_LAST-IDC_EOSSDATA_FIRST+1];
	void PopulateTree();
	int  ColorFlagToInt(unsigned char ucFlag);
	void SetUpdateDataDisplay(bool value = true);

// Dialog Data
	//{{AFX_DATA(CEOSSDataStat)
	enum { IDD = IDD_EOSSDATASTATUS_PAGE };
	CTreeCtrl	m_cTreeCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEOSSDataStat)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CImageList		m_cImages;
	unsigned int	EOSSes[32];

protected:
	CEOSSPropertySheet *m_pPropertySheet;
	void BuildPath(HTREEITEM hTreeItem, char* szFinal, char*szTemp, int iLevel = 0);

	// Generated message map functions
	//{{AFX_MSG(CEOSSDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangingTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRemovedata();
	afx_msg void OnButtonRunnow();
	afx_msg void OnButtonRunnow2();
	afx_msg void OnButtonUpdateDataDisplay();
	afx_msg void OnItemexpandingTree1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EOSSDATASTAT_H
