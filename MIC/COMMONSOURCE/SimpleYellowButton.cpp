// SimpleYellowButton.cpp

#include "stdafx.h"
#include "SimpleYellowButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimpleYellowButton

CSimpleYellowButton::CSimpleYellowButton(
	char *pcaption,
	UINT id, 
	//char *inifile, not used anymore 
	CWnd *parent,
	UINT RGBcolor) //Defaults to COLOR_YELLOW.  Can be any color.
{
//	TRACE("CSimpleYellowButton::COTR()\n");
	m_uID = id;
	m_pWnd = parent;
	m_csCaption = CString(pcaption);
	m_input_color = RGBcolor;
	Create();
}

CSimpleYellowButton::~CSimpleYellowButton()
{
}

BEGIN_MESSAGE_MAP(CSimpleYellowButton, CButton)
	//{{AFX_MSG_MAP(CSimpleYellowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSimpleYellowButton::Create()
{
//	TRACE("CSimpleYellowButton::Create()\n");
	CRect cRect;
	m_pWnd->GetDlgItem(m_uID)->GetWindowRect(&cRect);
	m_pWnd->ScreenToClient(&cRect);
	m_pWnd->GetDlgItem(m_uID)->ShowWindow(SW_HIDE);

	unsigned int dwStyle = 
		WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_DLGFRAME |
		BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER | BS_OWNERDRAW;

	//create the button and force OWNER DRAW
	BOOL success = CButton::Create(
		m_csCaption, 
		dwStyle,
		cRect, 
		m_pWnd, 
		m_uID );

	if (success)
	{
		EnableWindow(FALSE);

		m_Font.CreateFont(
			-12,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Microsoft Sans Serif");

		SetFont(&m_Font);
	}

	return success;
}

BOOL CSimpleYellowButton::EnableWindow(BOOL bEnable)
{
	BOOL returnval = TRUE;

	if (bEnable)
	{
		m_face_color = m_input_color;
	}
	else
	{
		m_face_color = COLOR_GRAY;
	}
	CButton::EnableWindow(bEnable);

	return returnval;
}

//the "real" work of displaying the owner draw button
//this function is called when the button is redrawn
void CSimpleYellowButton::DrawItem( LPDRAWITEMSTRUCT lpDI ) 
{
	//TRACE("CSimpleYellowButton::DrawItem(CtlID = %d)\n",lpDI->CtlID);

	CRect rupdown(lpDI->rcItem);//rectangle for up/down effect
	CRect rfillcolor(lpDI->rcItem);//rectangle for fill color
	CRect rfocus(lpDI->rcItem);//rectangle for focus 
	CRect rflags(lpDI->rcItem);//rectangle for flags
	CRect rtemp0(0,0,0,0);     //temp working rectangle 
	CRect rtemp1(0,0,0,0);     //temp working rectangle 
	rupdown.InflateRect(-1,-1); //rectangle for up/down effect
	rfillcolor.InflateRect(-2,-2); //rectangle for fill color
	rfocus.InflateRect(-5,-5); //rectangle for focus 
	rflags.InflateRect(-3,-3); //rectangle for flags
	char text[255];

	//Draw whole button
	if ((lpDI->itemAction & ODA_DRAWENTIRE) ||
		(lpDI->itemAction & ODA_SELECT))
	{
		//get a dc from handle hDC
		CDC dc;
		dc.m_hDC = lpDI->hDC;

		//save the current state
		int saveddc = dc.SaveDC();

		//create the text 
		CButton::GetWindowText(text,sizeof(text)-1);

		COLORREF text_color = GetSysColor(COLOR_BTNTEXT);

		//force black but could enhance to settable color
		dc.SetTextColor(text_color);
		//make sure the background shows through the text
		dc.SetBkMode(TRANSPARENT);

		//create and select a solid brush at the current color
		CBrush sb;
		sb.CreateSolidBrush(m_face_color);
		dc.SelectObject(&sb);

		//draw an outline in black
		if (lpDI->itemState & ODS_FOCUS)
			dc.Draw3dRect(&lpDI->rcItem,::GetSysColor(COLOR_3DDKSHADOW),::GetSysColor(COLOR_3DDKSHADOW));
		else
			dc.Draw3dRect(&lpDI->rcItem,::GetSysColor(COLOR_BTNHILIGHT),::GetSysColor(COLOR_3DDKSHADOW));

		//draw the raised or lowered edge of button
		if (lpDI->itemState & ODS_SELECTED)
		{
		  dc.Draw3dRect(rupdown,::GetSysColor(COLOR_BTNSHADOW),
		                   ::GetSysColor(COLOR_BTNHILIGHT));
		}
		else
		{
		  dc.Draw3dRect(rupdown,::GetSysColor(COLOR_BTNHILIGHT), 
		                   ::GetSysColor(COLOR_BTNSHADOW));
		}

		//fill with the desired brush/color
		dc.FillRect(rfillcolor,&sb);

		//calculate the bounding box of the text
		UINT uFormat;

		if (NULL == strchr(text,'\n'))
			uFormat = DT_CENTER|DT_WORDBREAK|DT_SINGLELINE |DT_VCENTER;  //singleline
		else
			uFormat = DT_CENTER|DT_WORDBREAK;  //multiline


		dc.DrawText(text, strlen(text), &rtemp1, DT_CALCRECT);

		rtemp0 = rflags;  //<<QA>> for new yellow button

		//offset the position of the text box if selected
		if (lpDI->itemState & ODS_SELECTED) 
		{
			rtemp0.right += 2;
			rtemp0.bottom += 2;
			rtemp0.top += 2;
			rtemp0.left += 2;
		}

		//draw the button's text
		dc.DrawText(text, strlen(text), &rtemp0, uFormat);

		if (lpDI->itemState & ODS_DISABLED) 
		{
			rflags = rtemp0;
			rflags.InflateRect(-2,-2,2,2);
			dc.SetTextColor(COLOR_WHITE);
			dc.DrawText(text,strlen(text),&rflags,uFormat);
			rflags.InflateRect(1,1,-1,-1);
			dc.SetTextColor(m_face_color);
			dc.DrawText(text,strlen(text),&rflags,uFormat);
		}

		//restore all of the dc's condition
		dc.RestoreDC(saveddc);
		if (lpDI->itemState & ODS_FOCUS)
			DrawFocusRect(lpDI->hDC,rfocus);

		//make sure our dc never gets used
		dc.m_hDC = NULL;
		return;
	}

	//Focus changing
	if (lpDI->itemAction & ODA_FOCUS) 
	{
		//only need to add or drop the focus rectangle
		//if it is already displayed redrawing it will
		//remove it.
		DrawFocusRect(lpDI->hDC, rfocus);
		CDC dc;
		dc.m_hDC = lpDI->hDC;
		dc.Draw3dRect(&lpDI->rcItem,::GetSysColor(COLOR_3DDKSHADOW),::GetSysColor(COLOR_3DDKSHADOW));
	}

}
