#ifndef PSRDATASTAT_H
#define PSRDATASTAT_H

// PSRDataStat.h : header file
//
#include "PSRSafePS.h"
#include "resource.h"
#include "ISO.h"
/////////////////////////////////////////////////////////////////////////////
// CPSRDataStat dialog
class CPSRDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CPSRDataStat)

// Construction
public:
	CPSRDataStat(UINT id, CISO* parent);
	CPSRDataStat();
	~CPSRDataStat();
	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CPSRPropertySheet*);
	COLORREF StaticColors[IDC_PSRDATA_LAST-IDC_PSRDATA_FIRST+1];
	CISO *m_pParent;

// Dialog Data
	//{{AFX_DATA(CPSRDataStat)
	enum { IDD = IDD_PSRDATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPSRDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPSRPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CPSRDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonWatch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // PSRDATASTAT_H
