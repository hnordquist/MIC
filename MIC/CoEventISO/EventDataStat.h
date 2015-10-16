//EventDataStat.h
#ifndef EVENTDATASTAT_H
#define EVENTDATASTAT_H

#include "EventSafePS.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CEventDataStat dialog
class CEventDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CEventDataStat)

// Construction
public:
	CEventDataStat(UINT id);
	CEventDataStat();
	~CEventDataStat();
	void SetText(int, char *);
	char *GetText(int, char *,int);
	void IssuePrint(FILE *outfile);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CEventPropertySheet*);
	COLORREF StaticColors[IDC_EVENTDATA_LAST-IDC_EVENTDATA_FIRST+1];

// Dialog Data
	//{{AFX_DATA(CEventDataStat)
	enum { IDD = IDD_EVENTDATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEventDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CEventPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CEventDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EVENTDATASTAT_H
