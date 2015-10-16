//Author:			Mehdi Mousavi
//Data of release:	8th of September, 2000
//Email:			Webmaster@modemmania.com

// CheckerCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CheckerCtrl.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckerCtrl

#define BLANKED_BLOCKS_COLOR	RGB(252, 252, 252)

CCheckerCtrl::CCheckerCtrl()
{
	//Sets the background brush of the client area
	m_backgroundBrush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	
	//Resets m_nyPos for scrolling purposes
	m_nyPos = 0;

	//Resets m_nBlockStartPos so that when the WM_PAINT message
	//is triggered, the control starts to show blocks
	//from m_nBlockStartPos position
	m_nBlockStartPos = 0;

	//Sets the starting index of blocks
	m_nStartIndex = 0;

	//Total number of blocks
	m_nNumberofBlocks = 0;

	//Offset for scrolling purposes
	m_nOffset = 0;

	m_nTotalVisibleBlocks = 0;

	m_nBlocksPerRow = 0;
}

CCheckerCtrl::~CCheckerCtrl()
{
}

BOOL CCheckerCtrl::Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID)
{
	//Postcondition:
	//	Creates a window after being registered, as well as
	//	setting all the required variables used hereinafter.
	static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW);
	BOOL bRet = CWnd::CreateEx(WS_EX_CLIENTEDGE, 
								className,
								NULL,
								dwStyle,
								rect.left,
								rect.top,
								rect.right - rect.left,
								rect.bottom - rect.top,
								pParentWnd->GetSafeHwnd(),
								(HMENU) nID);

	m_nID = nID;
	m_pParentWnd = pParentWnd;
	
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	
	return bRet;
}


BEGIN_MESSAGE_MAP(CCheckerCtrl, CWnd)
	//{{AFX_MSG_MAP(CCheckerCtrl)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCheckerCtrl message handlers

void CCheckerCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	//Fill the background color of the client area
	dc.FillRect(m_rcClient, &m_backgroundBrush);
	dc.SetBkMode(TRANSPARENT);

	UINT nColumn = 0, nRow = 0;
	
	//Calculate the index of the last visible block
	//within the client area
	UINT nBlockEndPos = m_nBlockStartPos + m_nTotalVisibleBlocks + m_nBlocksPerRow;
	if(nBlockEndPos > m_nNumberofBlocks)
		nBlockEndPos = m_nNumberofBlocks;

	for(UINT i = m_nBlockStartPos; i < nBlockEndPos; i++)
	{
		CString cString(m_crText.GetAt(i));
		if (cString.GetLength())
		{
			CBrush brush(m_crColor.GetAt(i));
			SetBlock(nRow, nColumn, brush, dc, cString);
		}
		if((i + 1 - m_nBlockStartPos) % m_nBlocksPerRow == 0)
		{
			nRow++;
			nColumn = 0;
		}
		else
			nColumn++;
	}
}

void CCheckerCtrl::SetTotalBlocks(const UINT nNumberofXBlocks, 
								  const UINT nNumberofYBlocks, 
								  const UINT nStartIndex)
{
	//Postcondition:
	//	Sets the member variable m_nNumberofBlocks to the specified
	//	number of blocks. Then creates an array of COLORREF, sized
	//	nNumberofBlocks, and initialize it with white color.
	//	Thereafter, it computes m_nOffset for scrolling purposes.
	m_nNumberofBlocks = nNumberofXBlocks * nNumberofYBlocks;

	m_crColor.SetSize(m_nNumberofBlocks);
	m_crText.SetSize(m_nNumberofBlocks);
	for(UINT i = 0; i < m_nNumberofBlocks; i++)
	{
		m_crColor.SetAt(i, BLANKED_BLOCKS_COLOR);
		m_crText.SetAt(i,"");
	}

	GetClientRect(m_rcClient);
//	m_nBlocksPerRow = m_rcClient.Width() / 9;
	m_nBlocksPerRow = nNumberofXBlocks;
//	m_nBlocksPerColumn = m_rcClient.Height() / 11;
	m_nBlocksPerColumn = nNumberofYBlocks;
	
	m_nTotalVisibleBlocks = m_nBlocksPerRow * m_nBlocksPerColumn;
	
	//Calculate the vertical scroll bar's range
	int nOffset = (m_nNumberofBlocks / m_nBlocksPerRow);
	if(m_nNumberofBlocks % m_nBlocksPerRow != 0)
		nOffset++;
	
	m_nOffset = nOffset - m_nBlocksPerColumn;
	if(m_nOffset > 0)
		SetScrollRange(SB_VERT, 0, m_nOffset);

	//	Sets the starting index of blocks
	m_nStartIndex = nStartIndex;
}



void CCheckerCtrl::SetTotalBlocks(const UINT nNumberofBlocks, const UINT nStartIndex)
{
	//Postcondition:
	//	Sets the member variable m_nNumberofBlocks to the specified
	//	number of blocks. Then creates an array of COLORREF, sized
	//	nNumberofBlocks, and initialize it with white color.
	//	Thereafter, it computes m_nOffset for scrolling purposes.
	m_nNumberofBlocks = nNumberofBlocks;

	m_crColor.SetSize(m_nNumberofBlocks);
	for(UINT i = 0; i < m_nNumberofBlocks; i++)
		m_crColor.SetAt(i, BLANKED_BLOCKS_COLOR);

	GetClientRect(m_rcClient);
//	m_nBlocksPerRow = m_rcClient.Width() / 9;
	m_nBlocksPerRow = 50;
//	m_nBlocksPerColumn = m_rcClient.Height() / 11;
	m_nBlocksPerColumn = 20;
	
	m_nTotalVisibleBlocks = m_nBlocksPerRow * m_nBlocksPerColumn;
	
	//Calculate the vertical scroll bar's range
	int nOffset = (m_nNumberofBlocks / m_nBlocksPerRow);
	if(m_nNumberofBlocks % m_nBlocksPerRow != 0)
		nOffset++;
	
	m_nOffset = nOffset - m_nBlocksPerColumn;
	if(m_nOffset > 0)
		SetScrollRange(SB_VERT, 0, m_nOffset);

	//	Sets the starting index of blocks
	m_nStartIndex = nStartIndex;
}

void CCheckerCtrl::SetBlock(int nRow, int nColumn, CBrush &brush, CDC &dc, CString cString)
{
	//Postcondition:
	//	Places a block on nRow, nColumn ordered pair, 
	//	and paints it with brush color
	CRect rect = GetRect(nRow, nColumn);

	dc.Rectangle(&rect);
	
	rect.left++;
	rect.top++;
	rect.bottom--;
	rect.right--;
	
	dc.FillRect(rect, &brush);
	if (cString)
		dc.TextOut(rect.left,rect.top,cString);
}

void CCheckerCtrl::SetBlock(const UINT nBlockNumber, const COLORREF crColor, CString cString)
{
	//Postcondition:
	//	Sets the color of a specified block number to crColor
	ASSERT(nBlockNumber - m_nStartIndex < m_nNumberofBlocks);
	m_crColor.SetAt(nBlockNumber - m_nStartIndex, crColor);
	m_crText.SetAt(nBlockNumber - m_nStartIndex, cString);
}

void CCheckerCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	int nyOffset;
	switch(nSBCode)
	{
	case SB_PAGEDOWN:
		if(m_nyPos < m_nOffset)
		{
			if(m_nyPos + 2 < m_nOffset)
				nyOffset = 2;
			else
				nyOffset = m_nOffset - m_nyPos;

			m_nyPos += nyOffset;
			m_nBlockStartPos += m_nBlocksPerRow * nyOffset;

			SetScrollPos(SB_VERT, m_nyPos);
			ScrollWindow(0, -nyOffset * 11);
		}
		break;

	case SB_PAGEUP:
		if(m_nyPos > 0)
		{
			if(m_nyPos - 2 > 0)
				nyOffset = 2;
			else
				nyOffset = m_nyPos;

			m_nBlockStartPos -= m_nBlocksPerRow * nyOffset;

			m_nyPos -= nyOffset;
			SetScrollPos(SB_VERT, m_nyPos);
			ScrollWindow(0, nyOffset * 11);
		}
		break;

	case SB_LINEUP:
		if(m_nyPos > 0)
		{
			m_nBlockStartPos -= m_nBlocksPerRow;

			m_nyPos--;
			SetScrollPos(SB_VERT, m_nyPos);
			ScrollWindow(0, 11);
		}
		break;

	case SB_LINEDOWN:
		if(m_nyPos < m_nOffset)
		{
			m_nBlockStartPos += m_nBlocksPerRow;

			m_nyPos++;
			SetScrollPos(SB_VERT, m_nyPos);
			ScrollWindow(0, -11);
		}
		break;
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CCheckerCtrl::Refresh()
{
	//Postcondition:
	//	Refreshes the client area of the control
	Invalidate();
}

COLORREF CCheckerCtrl::GetBlock(const UINT nBlockNumber) const
{
	//Precondition:
	//	nBlockNumber must be in the range of the defined blocks
	//Postcondition:
	//	Takes the color of the specified index
	ASSERT(nBlockNumber >= m_nStartIndex && nBlockNumber - m_nStartIndex <= m_nNumberofBlocks);
	return m_crColor.GetAt(nBlockNumber - m_nStartIndex);
}

void CCheckerCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();

	DWORD dwPos = GetMessagePos();
	CPoint clickedPoint((int)(short)LOWORD(dwPos), (int)(short)HIWORD(dwPos));
	ScreenToClient(&clickedPoint);
	
	{
		CRect cRect;
		GetWindowRect(&cRect);
		ScreenToClient(&cRect);

		UINT nY = UINT(((float)clickedPoint.y / (float(cRect.Height())/float(m_nBlocksPerColumn)))+.3);

		UINT nX = UINT(((float)clickedPoint.x / (float(cRect.Width())/float(m_nBlocksPerRow)))+.3);

		UINT nIndex = nY * m_nBlocksPerRow + nX + m_nyPos * m_nBlocksPerRow;

		if(nIndex < m_nNumberofBlocks && nX < m_nBlocksPerRow)
		{

			CString strNumber;
//			strNumber.Format("%d", nIndex + m_nStartIndex+1);
			strNumber = (m_crText.GetAt(nIndex + m_nStartIndex));

			if (strNumber.GetLength())
			{
				SetCapture();
				CreateSafeTooltipRect(clickedPoint.x, clickedPoint.y, strNumber);

				GetParent()->PostMessage(9876,(WPARAM)(nIndex+m_nStartIndex+1), 0);
			
				CClientDC dc(this);
				CBrush brush;
				brush.CreateSolidBrush(GetSysColor(COLOR_INFOBK));
				dc.Rectangle(tooltipRect);
				
				tooltipRect.left++;
				tooltipRect.top++;
				tooltipRect.bottom--;
				tooltipRect.right--;
				
				dc.FillRect(tooltipRect, &brush);
				
				CFont font;
				LOGFONT logFont;
				strcpy(logFont.lfFaceName, "Verdana");
				logFont.lfHeight = -MulDiv(12, GetDeviceCaps(dc.m_hDC, LOGPIXELSY), 72);
				logFont.lfWidth = 0;
				logFont.lfEscapement = 0;
				logFont.lfItalic = FALSE;
				logFont.lfStrikeOut = FALSE;
				logFont.lfUnderline = FALSE;
				logFont.lfWeight = FW_BOLD;
				
				font.CreateFontIndirect(&logFont);
				
				dc.SelectObject(font);
				
				dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));
				dc.SetBkColor(GetSysColor(COLOR_INFOBK));
				dc.DrawText(strNumber, tooltipRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
				
				tooltipRect.left--;
				tooltipRect.top--;
				tooltipRect.bottom++;
				tooltipRect.right++;

				m_bShouldUpdated = TRUE;
			}
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CCheckerCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bShouldUpdated)
	{
		CClientDC dc(this);
		InvalidateRect(&tooltipRect, FALSE);
		ReleaseCapture();
		
		m_bShouldUpdated = FALSE;
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CCheckerCtrl::CreateSafeTooltipRect(int x, int y, LPCTSTR lpszText)
{
	int nTextLength = strlen(lpszText);
	int nTextWidth = nTextLength;
	if(nTextWidth < 5)
		nTextWidth = 5;
	
	if(x + 12 * nTextWidth <= m_rcClient.right)
		tooltipRect.right = x + 12 * nTextWidth;
	else
		tooltipRect.right = x - 12 * nTextWidth;

	if(y + 25 <= m_rcClient.bottom)
		tooltipRect.bottom = y + 25;
	else
		tooltipRect.bottom = y - 25;

	tooltipRect.left = x;
	tooltipRect.top = y;

	if(tooltipRect.left > tooltipRect.right)
	{
		int nTemp = tooltipRect.left;
		tooltipRect.left = tooltipRect.right;
		tooltipRect.right = nTemp;
	}

	if(tooltipRect.top > tooltipRect.bottom)
	{
		int nTemp = tooltipRect.bottom;
		tooltipRect.bottom = tooltipRect.top;
		tooltipRect.top = nTemp;
	}
}

void CCheckerCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND)));
	CWnd::OnMouseMove(nFlags, point);
}

void CCheckerCtrl::Reset()
{
	for(UINT i = 0; i < m_nNumberofBlocks; i++)
		m_crColor.SetAt(i, BLANKED_BLOCKS_COLOR);
}

void CCheckerCtrl::Update(const UINT nBlockNumber)
{
	//Precondition:
	//	nBlockNumber must be in the range of the defined blocks
	//Postcondition:
	//	Updates the color of a specified index on CRT, if and only
	//	if it's already visible
	ASSERT(nBlockNumber >= m_nStartIndex && nBlockNumber - m_nStartIndex <= m_nNumberofBlocks);

	if(IsVisible(nBlockNumber))
	{
		UINT nRow = (nBlockNumber - m_nStartIndex) / m_nBlocksPerRow - m_nyPos;
		UINT nColumn = (nBlockNumber - m_nStartIndex) % m_nBlocksPerRow;
		
		CClientDC dc(this);
		CRect rect = GetRect(nRow, nColumn);
		
		rect.left++;
		rect.top++;
		rect.bottom--;
		rect.right--;
		
		CBrush brush;
		brush.CreateSolidBrush(m_crColor.GetAt(nBlockNumber - m_nStartIndex));
		dc.FillRect(rect, &brush);
	}
}

CRect CCheckerCtrl::GetRect(const UINT nRow, const UINT nColumn)
{
	//Postcondition:
	//	Returns the rectangular area of a specified block
	//	placed in nRow(th) row and nColumn(th) column.
	CRect cRect;
	GetWindowRect(cRect);
	ScreenToClient(&cRect);
	float fWidth = (float)(cRect.Width()-1)/(float)m_nBlocksPerRow;
	float fHeight = (float)(cRect.Height()-1)/(float)m_nBlocksPerColumn;

	CRect rect;
	//	rect.left = nColumn * 9 + 1;
	rect.left = long(fWidth * nColumn);
	//	rect.top = nRow * 11 + 1;
	rect.top = long(fHeight * nRow);
	//	rect.right = rect.left + 7;
	rect.right = long(rect.left + fWidth);
	//	rect.bottom = rect.top + 9;
	rect.bottom = long(rect.top + fHeight);

	rect.DeflateRect(0,0,2,2);
	return rect;
}

BOOL CCheckerCtrl::IsVisible(const UINT nBlockNumber)
{
	//Calculate the index of the last visible block
	//within the client area
	UINT nBlockEndPos = m_nBlockStartPos + m_nTotalVisibleBlocks + m_nBlocksPerRow;
	if(nBlockEndPos > m_nNumberofBlocks)
		nBlockEndPos = m_nNumberofBlocks;

	if(nBlockNumber >= m_nBlockStartPos && nBlockNumber <= nBlockEndPos)
		return TRUE;
	else
		return FALSE;
}

void CCheckerCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	if(!(point.x % 9 == 0 || point.y % 11 == 0))
	{
		UINT nY = point.y / 11;
		UINT nX = point.x / 9;

		UINT nIndex = nY * m_nBlocksPerRow + nX + m_nyPos * m_nBlocksPerRow;
		if(nIndex < m_nNumberofBlocks && nX < m_nBlocksPerRow)
			m_pParentWnd->PostMessage(WM_CHECKERCTRL_RBUTTONDOWN, m_nID, nIndex + m_nStartIndex);
	}

	CWnd::OnRButtonDown(nFlags, point);
}