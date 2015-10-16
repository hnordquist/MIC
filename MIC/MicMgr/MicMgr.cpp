//MicMgr.cpp

#include "stdafx.h"
#include "MicMgr.h"
#include "MicMgrDlg.h"
#include "mic_definitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMicMgrApp

BEGIN_MESSAGE_MAP(CMicMgrApp, CWinApp)
	//{{AFX_MSG_MAP(CMicMgrApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMicMgrApp construction

CMicMgrApp::CMicMgrApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMicMgrApp object

CMicMgrApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMicMgrApp initialization

BOOL CMicMgrApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.



	//if WAIT commandline then holdup here for 2 seconds 
	//usually until the calling MIC shuts down
	if (strcmp(m_lpCmdLine, "WAIT") == 0)
		Sleep(2000);

	//if a command line parameter of kill is there then 
	//tell a previous instance to shutdown 
	//then go away ourselves
	if (strcmp(m_lpCmdLine,"kill")==0) 
	{
		char string[256];
		HWND handle;
		bool bDone = false;
		handle = FindWindowEx (NULL, NULL, NULL, NULL);
		while (handle && !bDone)
		{
			::GetWindowText(handle, string,256);
			if (strlen(string)) 
			{
				if (strstr(string," - MIC - "))
				{
					bDone = true;
					CWnd* pWndPrev = NULL;
					CWnd* pWndChild = NULL;
					int dCount = 0;
					pWndPrev = CWnd::FindWindow(NULL,string);
					while (pWndPrev && (dCount < 5))
					{
						dCount++;
						pWndPrev->SetForegroundWindow();
						pWndPrev->SendMessage(WM_SYSCOMMAND,IDM_EXIT_ALT,0);
						pWndPrev = CWnd::FindWindow(NULL,string);
					}
				}
			}
			// test the string
			handle = FindWindowEx (NULL, handle, NULL, NULL);
		}
		return FALSE;
	}

	//if we are already running then quit now
	if (IsExistingInstanceOf("multiinstcollect", true, false)) {					//SCR00064 -- do all lower case
		MessageBox(0,"You may only run a single instance of this program!",	//SCR00064 -- for the 1.7.0.2 MPSS
			MIC_NAMELONG,MB_OK | MB_ICONEXCLAMATION);						//SCR00064 -- leave the mixed case in
		return FALSE;														//SCR00064 -- so that the old MPSS will work
	}
	if (IsExistingInstanceOf("MultiInstCollect", true, true)) {
		MessageBox(0,"You may only run a single instance of this program - 'MultiInstCollect'!",
			MIC_NAMELONG,MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

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
	wc.lpszClassName = "MICClass";
	::RegisterClass(&wc);


	CMicMgrDlg dlg;
	m_pMainWnd = &dlg;
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

bool CMicMgrApp::IsExistingInstanceOf(LPCSTR szcName, bool bKeep, bool bExactMutexName) 
{
	HANDLE hMutex = NULL;
	CHAR szName[512];

	strcpy_s(szName, szcName);

	if (!bExactMutexName)
		_strlwr_s(szName, 512);

	//char buff[512];
	//sprintf(buff, "mutex check with %s (%c)", szName, (bExactMutexName ? 'x' : 'l'));
	//MessageBox(0,buff, MIC_NAMELONG,MB_OK | MB_ICONEXCLAMATION);

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
			//char buff[512];
			//sprintf(buff, "mutex check with %s", szName);
			//MessageBox(0,buff, MIC_NAMELONG,MB_OK | MB_ICONEXCLAMATION);
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
		if (_stricmp(string,szName) == 0)
		{
			return TRUE;
		}
	 // test the string
	  handle = FindWindowEx (NULL, handle, NULL, NULL);
	}

	return FALSE;
}