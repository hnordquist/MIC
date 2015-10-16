#ifndef BINARYDATASTAT_H
#define BINARYDATASTAT_H

// BinaryDataStat.h : header file
//
#include "EventSafePS.h"
#include "resource.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CDataStat dialog
class CBinaryDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CBinaryDataStat)

// Construction
public:
	CBinaryDataStat(UINT id, CISO *parent);
	CBinaryDataStat();
	~CBinaryDataStat();

	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();

	void IssuePrint(FILE *outfile);
	void SetPropertySheet(CEventPropertySheet*);
	COLORREF StaticColors[IDC_BINARYEVENTDATA_LAST-IDC_BINARYEVENTDATA_FIRST+1];

	CEventPropertySheet *m_pPropertySheet;
	CISO *m_pParent;

// Dialog Data
	//{{AFX_DATA(CBinaryDataStat)
	enum { IDD = IDD_BINARYEVENTDATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBinaryDataStat)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBinaryDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // BINARYDATASTAT_H
