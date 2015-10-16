// WinDump.h : main header file for the WINDUMP application
//

#if !defined(AFX_WINDUMP_H__2B2009E5_F9A6_4B55_A2D0_FC8C139A8260__INCLUDED_)
#define AFX_WINDUMP_H__2B2009E5_F9A6_4B55_A2D0_FC8C139A8260__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWinDumpApp:
// See WinDump.cpp for the implementation of this class
//

class CWinDumpApp : public CWinApp
{
public:
	CWinDumpApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinDumpApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWinDumpApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDUMP_H__2B2009E5_F9A6_4B55_A2D0_FC8C139A8260__INCLUDED_)
