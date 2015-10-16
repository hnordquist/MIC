#ifndef MCADATASTAT_H
#define MCADATASTAT_H

// MCADataStat.h : header file
//
#include "MCASafePS.h"
#include "resource.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CMCADataStat dialog
class CMCADataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCADataStat)

// Construction
public:
	CMCADataStat(UINT id, CISO *parent);
	CMCADataStat();
	~CMCADataStat();

	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();

	void SetPropertySheet(CMCAPropertySheet*);
	COLORREF StaticColors[IDC_MCADATA_LAST-IDC_MCADATA_FIRST+1];

	CISO *m_pParent;

// Dialog Data
	//{{AFX_DATA(CMCADataStat)
	enum { IDD = IDD_MCADATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMCADataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMCAPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CMCADataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // MCADATASTAT_H
