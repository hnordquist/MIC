#if !defined(AFX_LEGEND_H__12A248E0_F76D_11D2_B4DC_006008A70A6A__INCLUDED_)
#define AFX_LEGEND_H__12A248E0_F76D_11D2_B4DC_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Legend.h : header file
//
#define COLOR_RED				RGB(255,0,0)
#define COLOR_GREEN				RGB(0,255,0)
#define COLOR_BLUE				RGB(0,0,255)

#define COLOR_GRAY				RGB(192,192,192)
#define COLOR_YELLOW			RGB(255,255,0)
#define COLOR_CYAN				RGB(0,255,255)
#define COLOR_MAGENTA			RGB(255,0,255)
#define COLOR_BLACK				RGB(0,0,0)
#define COLOR_WHITE				RGB(255,255,255)

#define COLOR_PAUSE				COLOR_GRAY
#define COLOR_WAIT				COLOR_YELLOW
#define COLOR_OK				COLOR_GREEN
#define COLOR_ERROR				COLOR_RED
#define COLOR_DEFAULT_TEXT		COLOR_BLACK

/////////////////////////////////////////////////////////////////////////////
// CLegend dialog

class CLegend : public CDialog
{
// Construction
public:
	CLegend(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLegend)
	enum { IDD = IDD_DIALOG1 };
	CRichEditCtrl	m_RichEdit;
	//}}AFX_DATA

	CString m_cName;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLegend)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Substitute(char* szString, char* szSubStr, COLORREF cTXTColor, bool bWingdings);

	// Generated message map functions
	//{{AFX_MSG(CLegend)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEGEND_H__12A248E0_F76D_11D2_B4DC_006008A70A6A__INCLUDED_)
