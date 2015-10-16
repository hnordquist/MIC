#ifndef GPSDATASTAT_H
#define GPSDATASTAT_H

// GPSDataStat.h : header file
//
#include "EventSafePS.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDataStat dialog
class CGPSDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CGPSDataStat)

// Construction
public:
	CGPSDataStat(UINT id);
	CGPSDataStat();
	~CGPSDataStat();

	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();

	void SetPropertySheet(CEventPropertySheet*);

	COLORREF StaticColors[IDC_GPSDATA_LAST-IDC_GPSDATA_FIRST+1];
	CEventPropertySheet *m_pPropertySheet;

// Dialog Data
	//{{AFX_DATA(CGPSDataStat)
	enum { IDD = IDD_GPSDATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGPSDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGPSDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // GPSDATASTAT_H
