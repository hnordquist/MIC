// CullFile.h : main header file for the CULLFILE application
//

#if !defined(AFX_CULLFILE_H__3B9FD264_2BD3_11D3_BBD3_00A024DA5B93__INCLUDED_)
#define AFX_CULLFILE_H__3B9FD264_2BD3_11D3_BBD3_00A024DA5B93__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCullFileApp:
// See CullFile.cpp for the implementation of this class
//

class CCullFileApp : public CWinApp
{
public:
	CCullFileApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCullFileApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCullFileApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CULLFILE_H__3B9FD264_2BD3_11D3_BBD3_00A024DA5B93__INCLUDED_)
