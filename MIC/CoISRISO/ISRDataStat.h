#ifndef ISRDATASTAT_H
#define ISRDATASTAT_H

// ISRDataStat.h : header file
//
#include "ISRSafePS.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CISRDataStat dialog
class CISRDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CISRDataStat)

// Construction
public:
	CISRDataStat(UINT id);
	CISRDataStat();
	~CISRDataStat();

	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();

	void IssuePrint(FILE *outfile);
	void SetPropertySheet(CISRPropertySheet*);
	void ToRate(double,int);
	void ToRaw(double,int);

	bool m_bDoingRates;
	COLORREF StaticColors[IDC_ISRDATA_LAST-IDC_ISRDATA_FIRST+1];

	CISRPropertySheet *m_pPropertySheet;

// Dialog Data
	//{{AFX_DATA(CISRDataStat)
	enum { IDD = IDD_ISRDATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CISRDataStat)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CISRDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioRates();
	afx_msg void OnRadioRaw();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ISRDATASTAT_H
