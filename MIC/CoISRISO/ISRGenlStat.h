#ifndef ISRGENLSTAT_H
#define ISRGENLSTAT_H

// ISRGenlStat.h : header file
//
#include "ISRSafePS.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CISRGenlStat dialog
class CISRGenlStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CISRGenlStat)

// Construction
public:
	CISRGenlStat(UINT id);
	CISRGenlStat();
	~CISRGenlStat();
	void SetText(int, char *);
	char *GetText(int, char*, int);
	void IssuePrint(FILE *outfile);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CISRPropertySheet*);
	COLORREF StaticColors[IDC_ISRSETUP_LAST-IDC_ISRSETUP_FIRST+1];

// Dialog Data
	//{{AFX_DATA(CISRGenlStat)
	enum { IDD = IDD_ISRGENERALSTAT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CISRGenlStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CISRPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CISRGenlStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
//////	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ISRGENLSTAT_H
