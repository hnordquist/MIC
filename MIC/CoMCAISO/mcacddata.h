#ifndef MCADDATA_H
#define MCADDATA_H

// CMCADData.h : header file
//

#include "MCASafePS.h"
#include "JulianT.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CMCACDData dialog
class CMCACDData : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCACDData)

// Construction
public:
	CMCACDData(UINT id, CISO *parent);
	CMCACDData();
	~CMCACDData();

	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void ResetColors();
	void SetPropertySheet(CMCAPropertySheet*);
	COLORREF StaticColors[IDC_MCACDDATA_LAST-IDC_MCACDDATA_FIRST+1];
	CISO *m_pParent;

// Dialog Data
	//{{AFX_DATA(CMCACDData)
	enum { IDD = IDD_MCACDDATA_PAGE };
	CButton	m_RadioConfig10;
	CButton	m_RadioConfig11;
	CButton	m_RadioConfig12;
	CButton	m_RadioConfig13;
	CButton m_RadioConfig14;
	CStatic	m_StaticConfig10;
	CStatic	m_StaticConfig14;
	CStatic	m_StaticConfig13;
	CStatic	m_StaticConfig12;
	CStatic	m_StaticConfig11;
	CEdit	m_pEditContents;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMCACDData)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString Text;
	CJulianTime JulianT;
	CMCAPropertySheet *m_pPropertySheet;
	char temp1[256];
	char temp2[32];
	CFont EditFont;
	void Concat(char* text, char* Source, int Start, int Length);
	// Generated message map functions
	//{{AFX_MSG(CMCACDData)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRadioConfig10();
	afx_msg void OnRadioConfig11();
	afx_msg void OnRadioConfig12();
	afx_msg void OnRadioConfig13();
	afx_msg void OnRadioConfig14();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetfocusEditContents();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // MCADDATA_H
