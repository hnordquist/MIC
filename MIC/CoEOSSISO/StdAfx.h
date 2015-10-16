// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__AF9D7069_9098_4463_AD0B_21A3A0C87E67__INCLUDED_)
#define AFX_STDAFX_H__AF9D7069_9098_4463_AD0B_21A3A0C87E67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#define _CRT_SECURE_NO_WARNINGS
#define WINVER 0x0502
#define _WIN32_WINNT 0x0502
#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxdisp.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include <commctrl.h>
#include <afxtempl.h>
#include <afxdlgs.h>
#include <afxcmn.h>
#include <io.h>
#include <direct.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__AF9D7069_9098_4463_AD0B_21A3A0C87E67__INCLUDED)
