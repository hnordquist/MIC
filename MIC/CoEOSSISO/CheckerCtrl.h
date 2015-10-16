#if !defined(AFX_CHECKERCTRL_H__AD4BC4E3_7750_11D4_8718_008048CA4A77__INCLUDED_)
#define AFX_CHECKERCTRL_H__AD4BC4E3_7750_11D4_8718_008048CA4A77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckerCtrl.h : header file
//

#include "afxtempl.h" 
/////////////////////////////////////////////////////////////////////////////
// CCheckerCtrl window

#define WM_CHECKERCTRL_RBUTTONDOWN	WM_USER + 100

class CCheckerCtrl : public CWnd
{
// Construction
public:
	CCheckerCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckerCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTotalBlocks(const UINT nNumberofBlocks, const UINT nStartIndex = 0);
	void SetTotalBlocks(const UINT nNumberofXBlocks,const UINT nNumberofYBlocks, const UINT nStartIndex = 0);
	void Update(const UINT nBlockNumber);
	void Reset();
	void Refresh();
	void SetBlock(const UINT nBlockNumber, const COLORREF crColor, CString cString);
	COLORREF GetBlock(const UINT nBlockNumber) const;
	BOOL Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID);
	virtual ~CCheckerCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckerCtrl)
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL IsVisible(const UINT nBlockNumber);
	CRect GetRect(const UINT nRow, const UINT nColumn);
	void CreateSafeTooltipRect(int x, int y, LPCTSTR lpszText);
	void SetBlock(int nRow, int nColumn, CBrush &brush, CDC &dc, CString cString);

	CWnd	*m_pParentWnd;
	UINT	m_nID;
	UINT	m_nStartIndex;
	BOOL	m_bShouldUpdated;
	CRect	tooltipRect;
	UINT	m_nTotalVisibleBlocks;
	UINT	m_nBlockStartPos;
	int		m_nyPos; 
	int		m_nOffset;
	CRect	m_rcClient;
	UINT	m_nBlocksPerRow;
	UINT	m_nBlocksPerColumn;
	UINT	m_nNumberofBlocks;
	CBrush	m_backgroundBrush;
	CArray	<COLORREF, COLORREF> m_crColor;
	CArray	<CString,CString> m_crText;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKERCTRL_H__AD4BC4E3_7750_11D4_8718_008048CA4A77__INCLUDED_)
