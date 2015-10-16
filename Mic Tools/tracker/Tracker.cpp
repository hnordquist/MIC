// Tracker.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Tracker.h"
#include "TrackerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrackerApp

BEGIN_MESSAGE_MAP(CTrackerApp, CWinApp)
	//{{AFX_MSG_MAP(CTrackerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrackerApp construction

CTrackerApp::CTrackerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTrackerApp object

CTrackerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTrackerApp initialization
bool CTrackerApp::IsExistingInstanceOf(LPCSTR szName, bool bKeep) 
{
	HANDLE hMutex = NULL;
	if ((hMutex = CreateMutex(NULL,TRUE,szName))!=NULL)
	{
		DWORD result = GetLastError();
		if (!bKeep)
		{
			CloseHandle(hMutex);
			ReleaseMutex(hMutex);
		}
		if (result == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hMutex);
			ReleaseMutex(hMutex);
			return TRUE;
		}
	}
	else
		return TRUE;

	char string[256];
	HWND handle;
	handle = FindWindowEx (NULL, NULL, NULL, NULL);
	while (handle)
	{
		::GetWindowText(handle, string, 256);
		if (strcmp(string,szName) == 0)
			return TRUE;
	 // test the string
	  handle = FindWindowEx (NULL, handle, NULL, NULL);
	}

	return FALSE;
}

BOOL CTrackerApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.


#ifdef SINGLERUN
	//if we are already running then quit now
	if (IsExistingInstanceOf("Tracker", true)) {
//		MessageBox(0,"You may only run a single instance of this program!",
//			"Tracker",MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
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
	wc.lpszClassName = "TrackerClass";
	::RegisterClass(&wc);

	CTrackerDlg dlg;
	m_pMainWnd = &dlg;

	if (m_lpCmdLine[0] != '\0')
		strcpy_s(dlg.m_cCommandLine,m_lpCmdLine);
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
