// Sync.h : main header file for the SYNC application
//

#if !defined(AFX_SYNC_H__BB7D61A4_7350_11D2_94AB_EE5FE0791316__INCLUDED_)
#define AFX_SYNC_H__BB7D61A4_7350_11D2_94AB_EE5FE0791316__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

extern char g_szInDir[_MAX_PATH];
extern char g_szOutDir[_MAX_PATH];
extern HINSTANCE g_hInstance;
/////////////////////////////////////////////////////////////////////////////
// CSyncApp:
// See Sync.cpp for the implementation of this class
//

class CSyncApp : public CWinApp
{
public:
	CSyncApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSyncApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNC_H__BB7D61A4_7350_11D2_94AB_EE5FE0791316__INCLUDED_)
