
// MicMgr.h : main header file for the MICMGR application
//

#if !defined(AFX_MICMGR_H__5A4286CB_E8F9_4A25_A5E0_1E97779C5F73__INCLUDED_)
#define AFX_MICMGR_H__5A4286CB_E8F9_4A25_A5E0_1E97779C5F73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMicMgrApp:
// See MicMgr.cpp for the implementation of this class
//

class CMicMgrApp : public CWinApp
{
public:
	CMicMgrApp();
	bool IsExistingInstanceOf(LPCSTR szcName, bool bKeep = false, bool bExactMutexName = true);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMicMgrApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMicMgrApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MICMGR_H__5A4286CB_E8F9_4A25_A5E0_1E97779C5F73__INCLUDED_)
