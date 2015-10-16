// WatchCom.h : main header file for the WATCHCOM application
//

#if !defined(AFX_WATCHCOM_H__335375D4_DAF8_11D2_B4B7_006008A70A6A__INCLUDED_)
#define AFX_WATCHCOM_H__335375D4_DAF8_11D2_B4B7_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define COLOR_BKGROUND (RGB(255,255,255))

/////////////////////////////////////////////////////////////////////////////
// CWatchComApp:
// See WatchCom.cpp for the implementation of this class
//

class CWatchComApp : public CWinApp
{
public:
	CWatchComApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchComApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWatchComApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHCOM_H__335375D4_DAF8_11D2_B4B7_006008A70A6A__INCLUDED_)
