#ifndef HHMRDATASTAT_H
#define HHMRDATASTAT_H

// HHMRDataStat.h : header file
//
#include "HHMRSafePS.h"
#include "resource.h"
#include "ISO.h"
/////////////////////////////////////////////////////////////////////////////
// CHHMRDataStat dialog
class CHHMRDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CHHMRDataStat)

// Construction
public:
	CHHMRDataStat(UINT id, CISO* parent);
	CHHMRDataStat();
	~CHHMRDataStat();
	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CHHMRPropertySheet*);
	COLORREF StaticColors[IDC_HHMRDATA_LAST-IDC_HHMRDATA_FIRST+1];
	CISO *m_pParent;

// Dialog Data
	//{{AFX_DATA(CHHMRDataStat)
	enum { IDD = IDD_HHMRDATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHHMRDataStat)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CHHMRPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CHHMRDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonWatch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // HHMRDATASTAT_H
