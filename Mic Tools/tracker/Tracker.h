// Tracker.h : main header file for the TRACKER application
//

#if !defined(AFX_TRACKER_H__156499A4_A4BC_11D2_B439_006008A70A6A__INCLUDED_)
#define AFX_TRACKER_H__156499A4_A4BC_11D2_B439_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

//#define SINGLERUN


/////////////////////////////////////////////////////////////////////////////
// CTrackerApp:
// See Tracker.cpp for the implementation of this class
//

class CTrackerApp : public CWinApp
{
public:
	CTrackerApp();
	bool IsExistingInstanceOf(LPCSTR szName, bool bKeep);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrackerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTrackerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACKER_H__156499A4_A4BC_11D2_B439_006008A70A6A__INCLUDED_)
