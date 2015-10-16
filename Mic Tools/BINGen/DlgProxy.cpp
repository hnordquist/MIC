// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "WinDump.h"
#include "DlgProxy.h"
#include "WinDumpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlgAutoProxy

IMPLEMENT_DYNCREATE(CWinDumpDlgAutoProxy, CCmdTarget)

CWinDumpDlgAutoProxy::CWinDumpDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF(CWinDumpDlg, AfxGetApp()->m_pMainWnd);
	m_pDialog = (CWinDumpDlg*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

CWinDumpDlgAutoProxy::~CWinDumpDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CWinDumpDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CWinDumpDlgAutoProxy, CCmdTarget)
	//{{AFX_MSG_MAP(CWinDumpDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CWinDumpDlgAutoProxy, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CWinDumpDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IWinDump to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {2EF5C629-D91D-488E-84F4-30193FF0F6DC}
static const IID IID_IWinDump =
{ 0x2ef5c629, 0xd91d, 0x488e, { 0x84, 0xf4, 0x30, 0x19, 0x3f, 0xf0, 0xf6, 0xdc } };

BEGIN_INTERFACE_MAP(CWinDumpDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CWinDumpDlgAutoProxy, IID_IWinDump, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {F1A24F4A-9FA2-4176-8385-CE9FBE204B62}
IMPLEMENT_OLECREATE2(CWinDumpDlgAutoProxy, "WinDump.Application", 0xf1a24f4a, 0x9fa2, 0x4176, 0x83, 0x85, 0xce, 0x9f, 0xbe, 0x20, 0x4b, 0x62)

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlgAutoProxy message handlers
