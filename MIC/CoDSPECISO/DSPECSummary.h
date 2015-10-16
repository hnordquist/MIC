 
//
#ifndef DSPECSUMMARY_H
#define DSPECSUMMARY_H

// DSPECSummary.h

#include "DSPECSafePS.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CSummary dialog

class CDSPECSummary : public CPropertyPage
{
	DECLARE_DYNCREATE(CDSPECSummary)

// Construction
public:
	CDSPECSummary(UINT id, CISO *parent);
	CDSPECSummary();
	~CDSPECSummary();
	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CDSPECSafePropertySheet*);
	COLORREF StaticColors[IDC_DSPECSUMMARY_LAST-IDC_DSPECSUMMARY_FIRST+1];
	void IssuePrint(FILE *outfile);
	CISO *m_pParent;
// Dialog Data
	//{{AFX_DATA(CSummary)
	enum { IDD = IDD_DSPECSUMMARY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDSPECSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDSPECSafePropertySheet *m_pPropertySheet;

	// Generated message map functions
	//{{AFX_MSG(CDSPECSummary)
	afx_msg void OnGotoDatastat();
	afx_msg void OnGotoSetup();
	afx_msg void OnGotoUserstat();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // DSPECSUMMARY_H
