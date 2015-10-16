#ifndef VACOSSDATASTAT_H
#define VACOSSDATASTAT_H

// VACOSSDataStat.h : header file
//
#include "EventSafePS.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CVACOSSDataStat dialog
class CVACOSSDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CVACOSSDataStat)

// Construction
public:
	CVACOSSDataStat(UINT id);
	CVACOSSDataStat();
	~CVACOSSDataStat();

	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();

	void ShiftRight();
	void SetPropertySheet(CEventPropertySheet*);
	COLORREF StaticColors[IDC_VACOSSDATA_LAST-IDC_VACOSSDATA_FIRST+1];

	CEventPropertySheet *m_pPropertySheet;

// Dialog Data
	//{{AFX_DATA(CVACOSSDataStat)
	enum { IDD = IDD_VACOSSDATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVACOSSDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVACOSSDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // VACOSSDATASTAT_H
