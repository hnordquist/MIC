// CDData.h

#ifndef CDDATA_H
#define CDDATA_H

/////////////////////////////////////////////////////////////////////////////
// CCDData dialog
class CCDData : public CPropertyPage
{
	DECLARE_DYNCREATE(CCDData)

// Construction
public:
	CCDData(UINT id);
	CCDData();
	~CCDData();
	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	COLORREF StaticColors[IDC_CDDATA_LAST-IDC_CDDATA_FIRST+1];

// Dialog Data
	//{{AFX_DATA(CCDData)
	enum { IDD = IDD_CDDATA_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCDData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCDData)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // CDDATA_H
