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
// MCAXYCtrl.cpp

#include "stdafx.h"
#include "MCAXYCtrl.h"
#include "colors.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// XYCtrl

CMCAXYCtrl::CMCAXYCtrl(UINT uMaxChannels)
{
	//TRACE("CMCAXYCtrl::CTR\n");

	CMCAXYCtrl::RegisterWndClass(AfxGetInstanceHandle());

	m_colorLine = RGB(0,0,0);
	m_colorDefault = RGB(0,0,255);

	m_bRecalculateConstants = true;
	m_iChannel = -1;
	m_strTitle       = _T("");
	m_strLeftLabel   = _T("");
	m_strBottomLabel = _T("TEST");
	m_crPixelColor   = COLOR_RED;
	m_uMaxChannels   = uMaxChannels;
	m_uMaxCountPerChannel = 0xFFFFFFFF;  //DSPEC channels are 32-bits each
	m_bHasFocus = false;

	m_iThresholdLineOffsetFromTopInPixels = -1;

	for (int i = 0; i < MAX_NUM_GRID_LINES; i++)
		m_iHorizontalGridLinesOffsetFromTopInPixels[i] = -1;
}

CMCAXYCtrl::~CMCAXYCtrl()
{
	//TRACE("CMCAXYCtrl::DTR\n");
	Reset();
}


BEGIN_MESSAGE_MAP(CMCAXYCtrl, CWnd)
	//{{AFX_MSG_MAP(CMCAXYCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// XYCtrl message handlers

BOOL CMCAXYCtrl::RegisterWndClass(HINSTANCE hInstance)
{
	//TRACE("CMCAXYCtrl::RegisterWndClass\n");
	WNDCLASS wc;
	wc.lpszClassName = "MCAXY_CTRL"; // matches class name in client
	wc.hInstance = hInstance;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hCursor = ::LoadCursor(NULL, IDC_CROSS);
	wc.hIcon = 0;
	wc.lpszMenuName = NULL;
	wc.hbrBackground = (HBRUSH) ::GetStockObject(LTGRAY_BRUSH);
	wc.style = CS_BYTEALIGNWINDOW|CS_GLOBALCLASS; // To be modified
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	return (::RegisterClass(&wc) != 0);
}

void CMCAXYCtrl::OnSize(UINT nType, int cx, int cy) 
{
	//TRACE("CMCAXYCtrl::OnSize\n");
	CWnd::OnSize(nType, cx, cy);
	RecalcRect();
}

void CMCAXYCtrl::RecalcRect()
{
	//TRACE("CMCAXYCtrl::RecalcRect\n");

	CClientDC dc(this);
	CSize sz(0,0);

	GetClientRect(&m_ClientRect);

	if (!m_strTitle.IsEmpty())
		sz = dc.GetTextExtent(m_strTitle);
	//
	// determine the plot area rectangle dimensions
	//
	m_PlotRect = m_ClientRect;	       //Start out whole thing
	m_PlotRect.top += sz.cy;           //reduce by height of title

	sz = dc.GetTextExtent(m_strLeftLabel);//"HITS");        //reduce by width of left label
	m_PlotRect.DeflateRect(sz.cx,sz.cy);  //Same height as "CHANNELS"

	//only want to plot within the bounding box
	m_d_x_axis_length_in_pixels = (double)m_PlotRect.Width()-2;
	m_d_y_axis_length_in_pixels = (double)m_PlotRect.Height()-2;


	// Byte align the size of the client rect to speed up
	// the bitblt function in OnPaint
	//rect.top    &= 0xFFFFFFF8;
	//rect.bottom &= 0xFFFFFFF8;
	//rect.left   &= 0xFFFFFFF8;
	//rect.right  &= 0xFFFFFFF8;
}

BOOL CMCAXYCtrl::AddPiece(COLORREF colorBack, COLORREF colorText,
							 UINT uX, UINT uY, int iSize)
{
	//TRACE("CMCAXYCtrl::AddPiece\n");

	CMCAXYPiece* pItem = new CMCAXYPiece;
	pItem -> m_colorBack = colorBack;
	pItem -> m_colorText = colorText;
	pItem -> m_brushBack.CreateSolidBrush(colorBack);
	pItem -> m_uX = uX;
	pItem -> m_uY = uY;
	pItem -> m_iSize = iSize;

	try 
	{
		m_chartPieces.Add(pItem);
		return TRUE;
	}
	catch (CMemoryException* e)
	{
		if (pItem !=NULL) 
			delete pItem;
		e->Delete();
		return FALSE;
	}
}

BOOL CMCAXYCtrl::AddPoint(UINT uX, UINT uY)
{
	//TRACE("CMCAXYCtrl::AddPoint\n");
	m_uMaxCountPerChannel = max(uY, m_uMaxCountPerChannel);
	CMCAXYPiece* pItem = new CMCAXYPiece;
	pItem->m_uX = uX;
	pItem->m_uY = uY;

	try 
	{
		m_chartPieces.Add(pItem);
		return TRUE;
	}
	catch (CMemoryException* e)
	{
		if (pItem !=NULL) 
			delete pItem;
		e->Delete();
		return FALSE;
	}

}

void CMCAXYCtrl::CalculatePoint(UINT ux, UINT uy, CPoint& pt)
{
	//TRACE("CMCAXYCtrl::CalculatePoint\n");

	if (ux > m_uMaxChannels)
		ux = m_uMaxChannels;

	double dxmax = (double)m_uMaxChannels;
	double dxinput = (double)ux;

//	double dymax = log10((double)m_uMaxCountPerChannel);
//	double dyinput = log10((double)uy);
	double dymax = (double)(m_uMaxCountPerChannel-1);
	double dyinput = (double)uy;

	double dxpixels = (dxinput/dxmax) * m_d_x_axis_length_in_pixels;
	double dypixels = (dyinput/dymax) * (m_d_y_axis_length_in_pixels-2);

	pt.x = m_PlotRect.left + (int)(dxpixels) + 1;
	pt.y = m_PlotRect.top + (int)((m_d_y_axis_length_in_pixels - dypixels))+2;
}


void CMCAXYCtrl::Reset()
{
	//TRACE("CMCAXYCtrl::Reset\n");
	m_uMaxCountPerChannel = 1;
	int nCount = m_chartPieces.GetSize();
	
	for (int i = 0; i < nCount; i++)
		delete m_chartPieces.GetAt(i);
	m_chartPieces.RemoveAll();

	//m_iThresholdLineOffsetFromTopInPixels = -1;

	if (::IsWindow(m_hWnd))  //fails when DTR calls Reset()
		InvalidateRect(NULL, FALSE);
}

void CMCAXYCtrl::GetItemColor(int /*i*/, COLORREF& color)
{
	//TRACE("CMCAXYCtrl::GetItemColor\n");
	color = m_crPixelColor;
}

void CMCAXYCtrl::SetTitle(const CString & str)
{
	//TRACE("CMCAXYCtrl::SetTitle() = %s\n", str);
	m_strTitle = str;
	RecalcRect();
	InvalidateRect(NULL, FALSE);
}

void CMCAXYCtrl::SetLeftLabel(const CString &str)
{
	//TRACE("CMCAXYCtrl::SetLeftLabel() = %s\n", str);
	m_strLeftLabel = str;
	RecalcRect();
	InvalidateRect(NULL, FALSE);
}

void CMCAXYCtrl::SetBottomLabel(const CString &str)
{
	//TRACE("CMCAXYCtrl::SetBottomLabel() = %s\n", str);
	m_strBottomLabel = str;
	RecalcRect();
	InvalidateRect(NULL, FALSE);
}

void CMCAXYCtrl::DrawThresholdLine(COLORREF cThresholdLineColor, UINT uValue)
{
	m_rgbThresholdLineColor = cThresholdLineColor;

	if (uValue == 0)
		uValue = 1;

	double dymax = log10((double)m_uMaxCountPerChannel);
	double dyinput = log10((double)uValue);

	//double dxpixels = (dxinput/dxmax) * m_d_x_axis_length_in_pixels;
	double dypixels = (dyinput/dymax) * m_d_y_axis_length_in_pixels;

	//pt.y = m_PlotRect.top + (int)((m_d_y_axis_length_in_pixels - dypixels));

	m_iThresholdLineOffsetFromTopInPixels = 
		m_PlotRect.top + (int)((m_d_y_axis_length_in_pixels - dypixels));
}

void CMCAXYCtrl::DrawHorizontalGridLines(COLORREF color)
{
	m_rgbHorizonatlGridLinesColor = color;
	double dyvalue, dypixels;
	double dValue = 10.;
	double dMaxCountPerChannel = (double)m_uMaxCountPerChannel;
	double dymax = log10(dMaxCountPerChannel);

	for (int j = 0; j < MAX_NUM_GRID_LINES; j++)
	{
		dyvalue = log10(dValue);
		dypixels = (dyvalue/dymax) * m_d_y_axis_length_in_pixels;

		m_iHorizontalGridLinesOffsetFromTopInPixels[j] = 
			m_PlotRect.top + (int)((m_d_y_axis_length_in_pixels - dypixels));

		dValue *= 10.;
		if (dValue > dMaxCountPerChannel)
			break;
	}

//	UINT m_iHorizontalGridLinesOffsetFromTopInPixels[MAX_NUM_GRID_LINES];	
}

void CMCAXYCtrl::OnPaint() 
{
	CPoint   cTextExtent, cpoint;
	CPaintDC dc(this);
	CDC      memoryDC;
	CBitmap  bitmap;
	CPen     pen, *oldpen;
	CPen	 RedPen;
	CPen	 GrayPen;
	CPen	 BluePen;
	CBrush   defaultbrush, whitebrush;

	if (m_hWnd == ::GetFocus())
		m_bHasFocus = true;

	if (m_bRecalculateConstants)
	{
		RecalcRect();
		// <<QA>> could tuck some other 1-time calculations in here
		m_bRecalculateConstants = false;
	}

	//Create memory device context compatible with a painting device context
	memoryDC.CreateCompatibleDC(&dc);

	//Create bitmap that will correspond to the clientRect and select it into memory DC
	bitmap.CreateCompatibleBitmap( &dc, m_ClientRect.Width(), m_ClientRect.Height() );
	memoryDC.SelectObject(&bitmap);

	memoryDC.FillSolidRect(m_ClientRect, /*COLOR_GREEN*/GetSysColor(COLOR_3DFACE));

	memoryDC.SelectStockObject(NULL_PEN);

	defaultbrush.CreateSolidBrush(m_colorDefault);
	memoryDC.SelectObject(&defaultbrush);

	whitebrush.CreateSolidBrush(RGB(255,255,255));  //white
	memoryDC.SelectObject(&whitebrush);
	memoryDC.Rectangle(m_PlotRect);

	pen.CreatePen(PS_SOLID, 1, m_colorLine);
	memoryDC.SelectObject(&pen);
	memoryDC.SelectStockObject(NULL_BRUSH);
	memoryDC.Rectangle(m_PlotRect);


	GrayPen.CreatePen(PS_SOLID, 1, RGB(191,191,191));
	memoryDC.SelectObject(&GrayPen);
	for (int index = 100; index < 1024; index+= 100)
	{
		CalculatePoint(index,0, cpoint);
		memoryDC.MoveTo(cpoint.x,2);
		memoryDC.LineTo(cpoint.x,m_ClientRect.bottom-2);
	}

	if (m_iChannel >= 0)
	{
		if (m_bHasFocus)
		{
			memoryDC.SetTextColor(COLOR_WHITE);
			memoryDC.SetBkMode(OPAQUE);
			memoryDC.SetBkColor(COLOR_BLUE);
		}
		else
		{
			memoryDC.SetTextColor(COLOR_BLACK);
			memoryDC.SetBkMode(TRANSPARENT);
		}
		CRect cRect(m_ClientRect.left,1,m_ClientRect.right,m_ClientRect.bottom);
		int i,j;
		CPoint cPointTemp;
		CalculatePoint(m_iChannel,0, cpoint);
		cPointTemp = cpoint;
		i = 0;
		while (cpoint.x == cPointTemp.x)
		{
			i++;
			CalculatePoint(m_iChannel+i,0,cPointTemp);
		}

		cPointTemp = cpoint;
		j = 0;
		while (cpoint.x == cPointTemp.x)
		{
			j--;
			CalculatePoint(m_iChannel+j,0,cPointTemp);
		}

		i--;
		j++;
		CString cString;
		if (i==j)
			cString.Format(" Channel: %d ",m_iChannel+j);
		else
			cString.Format(" Channel: %d to %d ",
			m_iChannel+j, m_iChannel+i);
//			m_iChannel, m_iChannel+(int)((double)1024 / (double)m_d_x_axis_length_in_pixels));
		memoryDC.DrawText(cString,cRect,DT_CENTER);
		memoryDC.SetBkColor(COLOR_WHITE);
//		if (m_bHasFocus)
//		{
//			cRect.DeflateRect(5,5,5,5);
//			memoryDC.DrawFocusRect(&cRect);
//		}
		

		BluePen.CreatePen(PS_DASH, 1, RGB(127,127,255));
		memoryDC.SelectObject(&BluePen);
		memoryDC.MoveTo(cpoint.x,2);
		memoryDC.LineTo(cpoint.x,m_ClientRect.bottom-2);
	}


	RedPen.CreatePen(PS_SOLID, 1, m_crPixelColor);
	memoryDC.SelectObject(&RedPen);
	memoryDC.SelectStockObject(NULL_BRUSH);

	CMCAXYPiece* pItem;
	int nCount = m_chartPieces.GetSize();
	CalculatePoint(1,1,cpoint);
	memoryDC.MoveTo(cpoint);
	for (int i=0; i < nCount; i++)
	{
		pItem = m_chartPieces.GetAt(i);
		CalculatePoint(i,pItem->m_uY, cpoint);

		cpoint.y -= 2;//3;//5;

//		memoryDC.SetPixel(cpoint,m_crPixelColor);
		memoryDC.LineTo(cpoint);
	}
/*
	//Draw threshold line
	if (m_iThresholdLineOffsetFromTopInPixels != -1)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 5, m_rgbThresholdLineColor);
		CPen *oldpen = memoryDC.SelectObject(&pen);
		memoryDC.MoveTo(m_PlotRect.left,m_iThresholdLineOffsetFromTopInPixels);
		memoryDC.LineTo(m_PlotRect.right,m_iThresholdLineOffsetFromTopInPixels);
		memoryDC.SelectObject(oldpen);
	}
*/
	//Draw Horizontal Grid Lines
	CPen apen;
	apen.CreatePen(PS_DASH, 1, m_rgbHorizonatlGridLinesColor);
	/*CPen **/oldpen = memoryDC.SelectObject(&apen);
/*
	for (int j = 0; j < MAX_NUM_GRID_LINES; j++)
	{
		if (m_iHorizontalGridLinesOffsetFromTopInPixels[j] == -1)
			break;

		memoryDC.MoveTo(m_PlotRect.left,m_iHorizontalGridLinesOffsetFromTopInPixels[j]);
		memoryDC.LineTo(m_PlotRect.right,m_iHorizontalGridLinesOffsetFromTopInPixels[j]);
	}
*/
	memoryDC.SelectObject(oldpen);

	//Draw left and bottom labels
	CPoint centerPoint(m_PlotRect.CenterPoint());
	cTextExtent = memoryDC.GetTextExtent(m_strBottomLabel);                              //"CHANNEL");
	memoryDC.TextOut(centerPoint.x-cTextExtent.x/2,m_PlotRect.bottom,m_strBottomLabel);  //"CHANNEL");
	cTextExtent = memoryDC.GetTextExtent(m_strLeftLabel);                                //("HITS");
	memoryDC.TextOut(m_PlotRect.left-cTextExtent.x,centerPoint.y-cTextExtent.y/2,m_strLeftLabel);//"HITS");

	//Draw Title
	if (!m_strTitle.IsEmpty())
	{
		memoryDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		memoryDC.SetBkMode(TRANSPARENT);
		memoryDC.DrawText(m_strTitle,CRect(m_ClientRect.left,1,m_ClientRect.right,m_ClientRect.bottom),DT_CENTER);
	}
	dc.BitBlt( 0, 0, m_ClientRect.Width(), m_ClientRect.Height(), &memoryDC, 0, 0, SRCCOPY );

}

void CMCAXYCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_iChannel = (int)(((double)point.x/(double)m_d_x_axis_length_in_pixels) * (double)1024.0);
	CWnd::OnLButtonDown(nFlags, point);
	SetFocus();
	m_bHasFocus = true;
	Invalidate(FALSE);
}

//I don't know why this isn't getting called in version 2.+. but on key down solves the problem
BOOL CMCAXYCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ((pMsg->message == 0x101) && (pMsg->wParam == 0x09))
	{
//		m_bHasFocus = true;
		Invalidate(FALSE);
	}
	else if ((pMsg->message == 0x100) && (pMsg->wParam == 0x27))//right
	{
		if (m_iChannel <= 1023 && m_bHasFocus)
		{
			CPoint cPoint2;
			CPoint cPoint1;
			CalculatePoint(m_iChannel,0, cPoint1);
			cPoint2 = cPoint1;
			while (cPoint1.x == cPoint2.x)
			{
				CalculatePoint(++m_iChannel,0,cPoint2);
			}
			if (m_iChannel > 1023)
				m_iChannel = 1023;
			Invalidate(FALSE);
			return true;
		}
	}
	else if ((pMsg->message == 0x100) && (pMsg->wParam == 0x28))//down
	{
		if (m_bHasFocus)
			return true;
	}
	else if ((pMsg->message == 0x100) && (pMsg->wParam == 0x26))//up
	{
		if (m_bHasFocus)
			return true;
	}
	else if ((pMsg->message == 0x0f) && (pMsg->wParam == 0x00))
	{
		if (!m_bHasFocus)
		{
			Invalidate(FALSE);
		}
	}
	else if ((pMsg->message == 0x100) && (pMsg->wParam == 0x25))//left
	{
		if (m_iChannel > 0 && m_bHasFocus)
		{
			CPoint cPoint2;
			CPoint cPoint1;
			CalculatePoint(m_iChannel,0, cPoint1);
			cPoint2 = cPoint1;
			while (cPoint1.x == cPoint2.x)
			{
				CalculatePoint(--m_iChannel,0,cPoint2);
			}
			Invalidate(FALSE);
			return true;
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CMCAXYCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	m_bHasFocus = false;
	Invalidate(FALSE);
}

void CMCAXYCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	if (m_bHasFocus)
	{
		if ((nChar == 102) || (nChar == 39))//right
		{
			if (m_iChannel <= 1023 && m_bHasFocus)
			{
				CPoint cPoint2;
				CPoint cPoint1;
				CalculatePoint(m_iChannel,0, cPoint1);
				cPoint2 = cPoint1;
				while (cPoint1.x == cPoint2.x)
				{
					CalculatePoint(++m_iChannel,0,cPoint2);
				}
				if (m_iChannel > 1023)
					m_iChannel = 1023;
				Invalidate(FALSE);
			}
		}
		else if ((nChar == 100) || (nChar == 37))//left
		{
			if (m_iChannel > 0 && m_bHasFocus)
			{
				CPoint cPoint2;
				CPoint cPoint1;
				CalculatePoint(m_iChannel,0, cPoint1);
				cPoint2 = cPoint1;
				while (cPoint1.x == cPoint2.x)
				{
					CalculatePoint(--m_iChannel,0,cPoint2);
				}
				Invalidate(FALSE);

			}
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
