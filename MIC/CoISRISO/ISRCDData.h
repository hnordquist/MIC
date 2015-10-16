#ifndef ISRCDDATA_H
#define ISRCDDATA_H

// CISRDData.h : header file
//

#include "ISRSafePS.h"
/////////////////////////////////////////////////////////////////////////////
// CISRCDData dialog
class CISRCDData : public CPropertyPage
{
	DECLARE_DYNCREATE(CISRCDData)

// Construction
public:
	CISRCDData(UINT id);
	CISRCDData();
	~CISRCDData();
	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	COLORREF StaticColors[IDC_CDDATA_LAST-IDC_CDDATA_FIRST+1];

// Dialog Data
	//{{AFX_DATA(CISRCDData)
	enum { IDD = IDD_ISRCDDATA_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CISRCDData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CISRCDData)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ISRCDDATA_H
