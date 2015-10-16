
#ifndef XYCTRL_H
#define XYCTRL_H

// XYCtrl.h

class CXYCtrl : public CWnd
{
// Construction
public:
	CXYCtrl(UINT uMaxChannels = 16384);

// Attributes
public:
	COLORREF m_colorDefault;
	COLORREF m_colorLine;
	COLORREF m_crPixelColor;

	UINT m_uNumChannels, m_uMaxCountPerChannel;
	bool m_bRecalculateConstants;

	double m_d_x_axis_length_in_pixels;
	double m_d_y_axis_length_in_pixels;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXYCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTitle(const CString& str);
	void SetLeftLabel(const CString &str);
	void SetBottomLabel(const CString &str);
	void SetNumChannels(UINT u);
	CString m_strTitle, m_strLeftLabel, m_strBottomLabel;
	void GetItemColor(int i, COLORREF& color);
	void Reset();
	//void CalculatePoint(UINT ux, UINT uy, CPoint& pt);
	void CalculatePoint(CPoint& pt_in, CPoint& pt_out);
	BOOL AddPoint(UINT uX, UINT uY);

	void DrawThresholdLine(COLORREF cThresholdLineColor, UINT uValue);
	int m_iThresholdLineOffsetFromTopInPixels;
	COLORREF m_rgbThresholdLineColor;

#define MAX_NUM_GRID_LINES (20)
	void DrawHorizontalGridLines(COLORREF color);
	COLORREF m_rgbHorizonatlGridLinesColor;
	int m_iHorizontalGridLinesOffsetFromTopInPixels[MAX_NUM_GRID_LINES];

#define MAX_POINTS 16384
	CPoint m_point[MAX_POINTS];
	int m_index;

	double m_dxmax, m_dymax;

	void RecalcRect();
	CRect m_ClientRect;
	CRect m_PlotRect;

	static BOOL RegisterWndClass(HINSTANCE hInstance);
	virtual ~CXYCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXYCtrl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // XYCTRL_H
