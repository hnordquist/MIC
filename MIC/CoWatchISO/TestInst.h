// TestInst.h

#if !defined(AFX_TESTINST_H__0B89D6C1_9D71_11D1_B39F_006008A70A6A__INCLUDED_)
#define AFX_TESTINST_H__0B89D6C1_9D71_11D1_B39F_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TestInst.h : header file
//
#include <commctrl.h>
#include <afxtempl.h>
#include "CSO.h"
#include "ISO.h"
#include "TestDlg.h"

#define WATCH_DISCOVERY_TYPE "WATCH"

/////////////////////////////////////////////////////////////////////////////
// CTestInst window

class CTestInst : public CISO
{
	// Construction
public:
	CTestInst(CWnd *parent, char *inifile);
	CTestInst();
	virtual ~CTestInst();

// Attributes
public:

	virtual bool  CollectParametersForNewISO(CString& csNameOut);
	virtual bool  CreateInstrument(void);
	virtual bool  UnRegInstrument();
	virtual void  ShowInstrument(bool);
	virtual void  PauseInstrument(bool){;}
	virtual bool  UpdateParams(bool){return true;}
	virtual void  UpdateStrings(){;}
	virtual void  DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	virtual void  IssuePrint(FILE* outfile){;}
	virtual void  Reset(){;}
	virtual void  BlockWrite(bool bw){m_bBlockWrite = bw;}
	virtual char *GetSaveLoc(){return NULL;}
	virtual void  SetLocalRegisteredMessageValues();

	CTestDlg	*m_pTestDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestInst)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CTestInst)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTINST_H__0B89D6C1_9D71_11D1_B39F_006008A70A6A__INCLUDED_)
