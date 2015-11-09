/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-040. This software was exported from the United States in accordance with the Export
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
// DateTimeCtrl.cpp : implementation file
//
// MFC Wrapper class for the IE3 DateTimePicker control
// Check out http://www.microsoft.com/msdn/sdk/inetsdk/help/itt/CommCtls/DateTime/DateTime.htm#ch_dtp_make
// for details.
//
//	To use in a dialog:
//	  Create a custom control with classname "SysDateTimePick32"
//    You will then need to subclass the custom control (in the InitDialog fn). 
//    eg.
//
//      m_DateTime.SubclassDlgItem(IDC_CUSTOM1, this);
//
//  To use if creating with "Create":
//      call "CDateTimeCtrl::Create(dwStyle, rect, pParentWnd, nID)"
// 
//  DateTime styles:
//
//		DTS_UPDOWN           - use UPDOWN instead of MONTHCAL
//		DTS_SHOWNONE         - allow a NONE selection
//		DTS_SHORTDATEFORMAT  - use the short date format (app must forward WM_WININICHANGE messages)
//		DTS_LONGDATEFORMAT   - use the long date format (app must forward WM_WININICHANGE messages)
//		DTS_TIMEFORMAT       - use the time format (app must forward WM_WININICHANGE messages)
//		DTS_APPCANPARSE      - allow user entered strings (app MUST respond to DTN_USERSTRING)
//		DTS_RIGHTALIGN       - right-align popup instead of left-align it
//
//  For the CDateTimeCtrl::SetFormat(LPCTSTR szFmt) specs, see the functions GetDateFormat 
//  and GetTimeFormat for the format of szFmt.
// 
//  For the Set- and Get-Colour functions, use the following for the nColIndex:
// 
//		MCSC_BACKGROUND   0   // the background color (between months)
//		MCSC_TEXT         1   // the dates
//		MCSC_TITLEBK      2   // background of the title
//		MCSC_TITLETEXT    3	  // title text
//		MCSC_MONTHBK      4   // background within the month cal
//		MCSC_TRAILINGTEXT 5   // the text color of header & trailing days
//
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DateTimeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI DDX_DateTime(CDataExchange *pDX, int nIDC, SYSTEMTIME &SysTime)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	ASSERT (hWndCtrl != NULL);

	if (pDX->m_bSaveAndValidate)
	{
		::SendMessage(hWndCtrl, DTM_GETSYSTEMTIME, 0, (LPARAM)&SysTime);
	}
	else // initializing
	{
		::SendMessage(hWndCtrl, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&SysTime);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDateTimeCtrl

bool CDateTimeCtrl::nCommonControlsInitialised = false;

CDateTimeCtrl::CDateTimeCtrl()
{
	if (!nCommonControlsInitialised)
	{
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_DATE_CLASSES;
		::InitCommonControlsEx(&icex);
		nCommonControlsInitialised = true;
	}

	m_CurrentTime = COleDateTime::GetCurrentTime();
}

CDateTimeCtrl::~CDateTimeCtrl()
{
}

BOOL CDateTimeCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
//	dwStyle |= DTS_APPCANPARSE;
	return CWnd::Create(DATETIMEPICK_CLASS, "", dwStyle, rect, pParentWnd, nID);
}

BEGIN_MESSAGE_MAP(CDateTimeCtrl, CWnd)
	//{{AFX_MSG_MAP(CDateTimeCtrl)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT_EX(DTN_DATETIMECHANGE, OnDateTimeChangeNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDateTimeCtrl (public) operations

CTime CDateTimeCtrl::GetDateTime() 
{
	SYSTEMTIME SysTime;
	SysTimeFromCOleDateTime(m_CurrentTime, &SysTime);

	return CTime(SysTime); 
}

COleDateTime CDateTimeCtrl::GetOleDateTime() 
{
	return m_CurrentTime; 
}

BOOL CDateTimeCtrl::SetDateTime(CTime& time)
{
	SYSTEMTIME SysTime;
	SysTimeFromCTime(time, &SysTime);

	m_CurrentTime = SysTime;
	return SetSystemTime(SysTime);
}

BOOL CDateTimeCtrl::SetDateTime(COleDateTime& time)
{
	m_CurrentTime = time;
	return SetSystemTime(time);
}

/////////////////////////////////////////////////////////////////////////////
// CDateTimeCtrl (protected) implementation

COleDateTime CDateTimeCtrl::GetSystemTime()
{	
	SYSTEMTIME SysTime;
	if (DateTime_GetSystemtime(m_hWnd, &SysTime) == GDT_VALID)
		return COleDateTime(SysTime);
	else
		return COleDateTime::GetCurrentTime();
}

void CDateTimeCtrl::SysTimeFromCOleDateTime(COleDateTime& time, SYSTEMTIME *SysTime)
{
	SysTime->wYear   = (unsigned short)time.GetYear();
    SysTime->wMonth  = (unsigned short)time.GetMonth();
    SysTime->wDay    = (unsigned short)time.GetDay();
    SysTime->wHour   = (unsigned short)time.GetHour();
    SysTime->wMinute = (unsigned short)time.GetMinute();
    SysTime->wSecond = (unsigned short)time.GetSecond();
    SysTime->wMilliseconds = 0;
}

void CDateTimeCtrl::SysTimeFromCTime(CTime& time, SYSTEMTIME *SysTime)
{
	SysTime->wYear   = (unsigned short)time.GetYear();
    SysTime->wMonth  = (unsigned short)time.GetMonth();
    SysTime->wDay    = (unsigned short)time.GetDay();
    SysTime->wHour   = (unsigned short)time.GetHour();
    SysTime->wMinute = (unsigned short)time.GetMinute();
    SysTime->wSecond = (unsigned short)time.GetSecond();
    SysTime->wMilliseconds = 0;
}

BOOL CDateTimeCtrl::SetSystemTime(SYSTEMTIME& time)
{
	return DateTime_SetSystemtime(m_hWnd, GDT_VALID, &time);
}

BOOL CDateTimeCtrl::SetSystemTime(CTime& time)
{
	SYSTEMTIME SysTime;
	SysTimeFromCTime(time, &SysTime);
	return DateTime_SetSystemtime(m_hWnd, GDT_VALID, &SysTime);
}

BOOL CDateTimeCtrl::SetSystemTime(COleDateTime& time)
{
	SYSTEMTIME SysTime;
	SysTimeFromCOleDateTime(time, &SysTime);
	return DateTime_SetSystemtime(m_hWnd, GDT_VALID, &SysTime);
}

// See GetDateFormat and GetTimeFormat for the format of szFmt.
BOOL CDateTimeCtrl::SetFormat(LPCTSTR szFmt)
{
	return DateTime_SetFormat(m_hWnd, szFmt);
}

// Posible values for nColIndex:
//	MCSC_BACKGROUND   0   // the background color (between months)
//	MCSC_TEXT         1   // the dates
//	MCSC_TITLEBK      2   // background of the title
//	MCSC_TITLETEXT    3
//	MCSC_MONTHBK      4   // background within the month cal
//	MCSC_TRAILINGTEXT 5   // the text color of header & trailing days

COLORREF CDateTimeCtrl::SetMonthCalColour(int nColIndex, COLORREF colour)
{
	return DateTime_SetMonthCalColor(m_hWnd, nColIndex, colour);
}

COLORREF CDateTimeCtrl::GetMonthCalColour(int nColIndex)
{
	return DateTime_GetMonthCalColor(m_hWnd, nColIndex);
}

void CDateTimeCtrl::SetMonthCalFont(CFont& font, BOOL bRedraw /*=TRUE*/)
{
	DateTime_SetMonthCalFont(m_hWnd, (HFONT)font, bRedraw);
}

CFont* CDateTimeCtrl::GetMonthCalFont()	
{
	return CFont::FromHandle( (HFONT)DateTime_GetMonthCalFont(m_hWnd) );
}

// Returns TRUE if success for both times, FALSE otherwise
BOOL CDateTimeCtrl::SetRange(CTime* pMinTime, CTime* pMaxTime)
{
	SYSTEMTIME SysTimeRange[2];
	int nWhich = 0;

	if (pMinTime) {
		nWhich |= GDTR_MIN;
		SysTimeFromCTime( *pMinTime, &(SysTimeRange[0]) );
	}

	if (pMaxTime) {
		nWhich |= GDTR_MAX;
		SysTimeFromCTime( *pMaxTime, &(SysTimeRange[1]) );
	}

	return DateTime_SetRange(m_hWnd, nWhich, SysTimeRange);
}

// Returns TRUE if success for both times, FALSE otherwise
BOOL CDateTimeCtrl::SetRange(COleDateTime* pMinTime, COleDateTime* pMaxTime)
{
	SYSTEMTIME SysTimeRange[2];
	int nWhich = 0;

	if (pMinTime) {
		nWhich |= GDTR_MIN;
		SysTimeFromCOleDateTime( *pMinTime, &(SysTimeRange[0]) );
	}

	if (pMaxTime) {
		nWhich |= GDTR_MAX;
		SysTimeFromCOleDateTime( *pMaxTime, &(SysTimeRange[1]) );
	}

	return DateTime_SetRange(m_hWnd, nWhich, SysTimeRange);
}

// returns GDTR_MIN|GDTR_MAX if there is a minimum|maximum limit
DWORD CDateTimeCtrl::GetRange(CTime* pMinTime, CTime* pMaxTime)
{
	SYSTEMTIME SysTimeRange[2];
	DWORD result = DateTime_GetRange(m_hWnd, SysTimeRange);

	*pMinTime = CTime(SysTimeRange[0]);	// systime is 0 if no limit
	*pMaxTime = CTime(SysTimeRange[1]);

	return result;
}

// returns GDTR_MIN|GDTR_MAX if there is a minimum|maximum limit
DWORD CDateTimeCtrl::GetRange(COleDateTime* pMinTime, COleDateTime* pMaxTime)
{
	SYSTEMTIME SysTimeRange[2];
	DWORD result = DateTime_GetRange(m_hWnd, SysTimeRange);

	*pMinTime = COleDateTime(SysTimeRange[0]);	// systime is 0 if no limit
	*pMaxTime = COleDateTime(SysTimeRange[1]);

	return result;
}

/////////////////////////////////////////////////////////////////////////////
// CDateTimeCtrl notification handlers

// This traps the DTN_DATETIMECHANGE notification and checks that
// the date/time has actually changed. If it has, then the message
// is passed on to the parent as normal, otherwise it is blocked.
BOOL CDateTimeCtrl::OnDateTimeChangeNotify(LPNMDATETIMECHANGE dtmh, LRESULT* /*result*/)
{
	if (dtmh->dwFlags == GDT_VALID) 
	{
		COleDateTime time(dtmh->st);
		if (time != m_CurrentTime)
		{
			m_CurrentTime = time;
			return FALSE;	// Let the parent see the message
		}
	}
	return TRUE;	// Don't let parent see message, since date hasn't changed
}
