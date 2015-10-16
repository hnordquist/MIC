// MCAXYCtrl.h

#ifndef MCAXYCTRL_H
#define MCAXYCTRL_H

#include <afxtempl.h>

class CMCAXYPiece : public CObject
{
public:
	CMCAXYPiece() {;}
	~CMCAXYPiece() {if (m_brushBack.m_hObject) m_brushBack.DeleteObject();}
	//~CMCAXYPiece() {;}

	COLORREF m_colorBack;
	COLORREF m_colorText;
	CBrush m_brushBack;	
	UINT m_uX;
	UINT m_uY;
	int	m_iSize;
};

typedef CTypedPtrArray <CObArray, CMCAXYPiece*> CChartPieceArrayMCAXY;

class CMCAXYCtrl : public CWnd
{
// Construction
public:
	CMCAXYCtrl(UINT uMaxChannels = 1024);

// Attributes
public:
	COLORREF m_colorDefault;
	COLORREF m_colorLine;
	COLORREF m_crPixelColor;

	CChartPieceArrayMCAXY m_chartPieces;
	UINT m_uMaxChannels, m_uMaxCountPerChannel;
	bool m_bRecalculateConstants;

	double m_d_x_axis_length_in_pixels;
	double m_d_y_axis_length_in_pixels;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMCAXYCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTitle(const CString& str);
	void SetLeftLabel(const CString &str);
	void SetBottomLabel(const CString &str);
	void ClearChDisplay(){m_iChannel = -1; Invalidate();};
	//void SetNumChannels(UINT n); // <<QA>>
	CString m_strTitle, m_strLeftLabel, m_strBottomLabel;
	void GetItemColor(int i, COLORREF& color);
	void Reset();
	void CalculatePoint(UINT ux, UINT uy, CPoint& pt);
	BOOL AddPiece(COLORREF colorBack, COLORREF colorText,
		UINT ux, UINT uy, int iSize);
	BOOL AddPoint(UINT uX, UINT uY);

	void DrawThresholdLine(COLORREF cThresholdLineColor, UINT uValue);
	int m_iThresholdLineOffsetFromTopInPixels;
	COLORREF m_rgbThresholdLineColor;

#define MAX_NUM_GRID_LINES (20)
	void DrawHorizontalGridLines(COLORREF color);
	COLORREF m_rgbHorizonatlGridLinesColor;
	int m_iHorizontalGridLinesOffsetFromTopInPixels[MAX_NUM_GRID_LINES];

	void RecalcRect();
	CRect m_ClientRect;
	CRect m_PlotRect;
	int m_iChannel;

	static BOOL RegisterWndClass(HINSTANCE hInstance);
	virtual ~CMCAXYCtrl();

	// Generated message map functions
protected:
	bool m_bHasFocus;
	//{{AFX_MSG(CMCAXYCtrl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // MCAXYCTRL_H
