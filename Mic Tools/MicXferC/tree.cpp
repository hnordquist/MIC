// tree.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "tree.h"
#include "treeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeApp

BEGIN_MESSAGE_MAP(CTreeApp, CWinApp)
	//{{AFX_MSG_MAP(CTreeApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeApp construction

CTreeApp::CTreeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTreeApp object

CTreeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTreeApp initialization

BOOL CTreeApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	WNDCLASS wc;

	wc.style = CS_DBLCLKS | CS_SAVEBITS | CS_BYTEALIGNWINDOW;
	wc.lpfnWndProc = DefDlgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hIcon = LoadIcon(IDR_MAINFRAME);
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW + 1;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MicXferCClass";
	::RegisterClass(&wc);

	CTreeDlg dlg;
	m_pMainWnd = &dlg;

	if (m_lpCmdLine[0] != '\0')
		strcpy(dlg.m_cCommandLine,m_lpCmdLine);
	else
		dlg.m_cCommandLine[0] = NULL;

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
