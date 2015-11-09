/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
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
