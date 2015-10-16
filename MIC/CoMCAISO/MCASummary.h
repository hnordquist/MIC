// MCASummary.h

#ifndef MCASUMMARY_H
#define MCASUMMARY_H

#include "MCASafePS.h"
#include "resource.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CSummary dialog

class CMCASummary : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCASummary)

// Construction
public:
	CMCASummary(UINT id, CISO *parent);
	CMCASummary();
	~CMCASummary();

	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CMCAPropertySheet*);
	COLORREF StaticColors[IDC_MCASUMMARY_LAST-IDC_MCASUMMARY_FIRST+1];

	CMCAPropertySheet *m_pPropertySheet;
	CISO *m_pParent;

// Dialog Data
	//{{AFX_DATA(CMCASummary)
	enum { IDD = IDD_MCASUMMARY_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMCASummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMCASummary)
	afx_msg void OnGotoDataStatusPage();//OnGotoDatastat(); IDC_GOTO_DATASTAT
	afx_msg void OnGotoMCAStatusPage();//OnGotoGrand(); IDC_GOTO_GRAND
	afx_msg void OnGotoMCAConfigurationPage();//OnGotoUserstat();IDC_GOTO_USERSTAT
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // MCASUMMARY_H
