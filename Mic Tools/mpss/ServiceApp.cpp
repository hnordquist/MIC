/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
// ServiceApp.cpp: implementation of the CServiceApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServiceApp.h"
#include <process.h>

#define USERVERSION
#define		VERSION		"1.7.1.0"
CServiceApp theApp;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceApp::CServiceApp()
#ifdef USERVERSION
	: CNTService(TEXT("MultiRunU"), TEXT("Multi-Program Startup USER"))
#else
	: CNTService(TEXT("MultiRun"), TEXT("Multi-Program Startup Service"))
#endif
	, m_hStop(0)
{
	m_bPaused = false;
	m_bRunning = false;
}

CServiceApp::~CServiceApp()
{
}


BOOL CServiceApp :: InitInstance() {
	RegisterService(__argc, __argv);
	return FALSE;
}


void CServiceApp :: Pause() {
	ReportStatus(SERVICE_PAUSED);
	m_bPaused = true;
}

void CServiceApp :: Continue() {
	ReportStatus(SERVICE_RUNNING);
	m_bPaused = false;
}


void CServiceApp :: Run( DWORD, LPTSTR *) {	
	// report to the SCM that we're about to start
	if (m_bRunning) return;
	m_bRunning = true;
	ReportStatus(SERVICE_START_PENDING);
	
	m_hStop = ::CreateEvent(0, TRUE, FALSE, 0);

	// You might do some more initialization here.
	// Parameter processing for instance ...
	

	// enter main-loop
	// If the Stop() method sets the event, then we will break out of
	// this loop.
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFName[_MAX_FNAME];
	char szExt[_MAX_EXT];
	char szIniFile[_MAX_PATH];
	char szResult[1024];
	char szIndex[8];
	int iIndex;
	char *szName;
	char *szCommandLine;
		int At = 0;
		char *argv[20];

		int dArgvIndex = 0;
		bool done = false;
		char cEnd;


		char szCurDir[_MAX_PATH];
//		char szDrive[_MAX_DRIVE];
//		char szDir[_MAX_DIR];
//		char szFName[_MAX_FNAME];
//		char szExt[_MAX_EXT];
		char szResultx[_MAX_PATH];
		char temp[_MAX_PATH];


	szCommandLine = GetCommandLine();
	if (szCommandLine[0] == '"') szCommandLine++;
	_splitpath(szCommandLine,szDrive,szDir,szFName,szExt);
	_makepath(szIniFile,szDrive,szDir,szFName,".ini");
	int dPauseTime = GetPrivateProfileInt("CONTROL","CYCLE",-1,szIniFile)*1000;
	//DR 163
	if(dPauseTime < 0)
	{
		WritePrivateProfileString("CONTROL","CYCLE","10",szIniFile);
		dPauseTime = 10*1000;
	}
	int dPauseTimeTemp;
	WritePrivateProfileString("CONTROL","VERSION",VERSION,szIniFile);
	// report SERVICE_RUNNING immediately before you enter the main-loop
	// DON'T FORGET THIS!
	ReportStatus(SERVICE_RUNNING);

	dPauseTimeTemp = GetPrivateProfileInt("CONTROL","INITIALPAUSE_SEC",-1,szIniFile);
	if (dPauseTimeTemp == -1)
	{
		WritePrivateProfileString("CONTROL","INITIALPAUSE_SEC","1",szIniFile);
		dPauseTimeTemp = 1000;
	}
	else
		dPauseTimeTemp = dPauseTimeTemp * 1000;


	while( ::WaitForSingleObject(m_hStop, dPauseTimeTemp) != WAIT_OBJECT_0 ) {
		dPauseTimeTemp = dPauseTime;
		if (!m_bPaused)
		{
			iIndex = 1;
			_itoa(iIndex,szIndex,10);
			GetPrivateProfileString("CONTROL",szIndex,"",szResult,sizeof(szResult),szIniFile);
			int dLength;
			while (dLength = strlen(szResult))
			{
				At = 0;
//				char *argv[20];
				for (int i = 0; i < 20; i++ ) argv[i] = NULL;
				//get the mutex name or dialog box name
				if (szResult[0] == '"') 
				{
					//we have a start
					szName = &szResult[1];
					At = 1;
					//find the end
					while ((At < dLength) && (szResult[At] != '"')) At++;
					//go to next if error
					if (At >= dLength) break;
					//terminate this string
					szResult[At] = NULL;
					//setup for the search for comma
					At++;
				}
				else
					//we have a start
					szName = szResult;
				//search for comma
				while ((At < dLength) && (szResult[At] != ',')) At++;
				//break if error
				if (At >= dLength) break;
				//set the comma to a null to terminate the previous
				szResult[At++] = NULL;
				//search for first non-blank char
				//this will give us the beginning of the
				//executable
				while ((At < dLength) && (szResult[At] == ' ')) At++;
				//break if error
				if (At >= dLength) break;

				dArgvIndex = 0;
				done = false;
//				char cEnd;
				//find all subsequent 
				do {
					//is this string enclosed in quotes
					if (szResult[At] == '"')
					{
						//yes
						cEnd = '"';
						At++;
					}
					else //no
						cEnd = ' ';

					argv[dArgvIndex++] = &szResult[At];
					argv[dArgvIndex] = NULL;
					while ((At < dLength) && (szResult[At] != cEnd)) At++;
					if (At < dLength)
					{
						done = false;
						szResult[At++] = NULL;
						//search for first non-white char
						while ((At < dLength) && (szResult[At] == ' ')) At++;
						if (At >= dLength) done = true;
					}
					else
						done = true;
				}	
				while (!done);

				if (!IsExistingInstanceOf(szName))
				{
//					char szCurDir[_MAX_PATH];
//					char szDrive[_MAX_DRIVE];
//					char szDir[_MAX_DIR];
//					char szFName[_MAX_FNAME];
//					char szExt[_MAX_EXT];
//					char szResultx[_MAX_PATH];
//					char temp[_MAX_PATH];

					GetCurrentDirectory(sizeof(szCurDir),szCurDir);
					_splitpath(argv[0],szDrive,szDir,szFName,szExt);
					_makepath(szResultx,szDrive,szDir,NULL,NULL);
					SetCurrentDirectory(szResultx);
					strcpy(temp,argv[0]);
					if (strchr(argv[0],' '))
					{
						strcpy(szResultx,"\"");
						strcat(szResultx,argv[0]);
						strcat(szResultx,"\"");
					}
					else
						strcpy(szResultx,argv[0]);
									
					argv[0] = szResultx;
					_spawnv(_P_NOWAIT,temp,argv);
					
					SetCurrentDirectory(szCurDir);
				}
			
				iIndex++;
				_itoa(iIndex,szIndex,10);
				GetPrivateProfileString("CONTROL",szIndex,"",szResult,sizeof(szResult),szIniFile);
			}
		}
	}

	if( m_hStop )
		::CloseHandle(m_hStop);
	m_bRunning = false;
}

#define MAGIC_PROCESS_NAME "multiinstcollect"

BOOL CServiceApp::IsExistingInstanceOf(LPCSTR szcName) 
{
	HANDLE hMutex = NULL;
	DWORD result;
	
	CHAR szName[512];
	strcpy_s(szName, szcName);

	if (stricmp(szcName, MAGIC_PROCESS_NAME) == 0)  // look for this magic process with the lower-case mutex name created by MicMgr 2.*
	{
		_strlwr_s(szName, 512);
	}
	//char buff[512];
	//sprintf(buff, "MPSS mutex check with %s", szName);
	//MessageBox(0,buff, "vlotz",MB_OK | MB_ICONEXCLAMATION);

	if (hMutex = CreateMutex(NULL,TRUE,szName))
	{
		result = GetLastError();	// jfl: this is whacked, there is no error if hMutex is nonzero
		CloseHandle(hMutex);		// jfl: but then dave close's it anyway and redoes it with the lower case name.
		ReleaseMutex(hMutex);
		if (result == ERROR_ALREADY_EXISTS)  // jfl: never true in this code locations LOL
			return TRUE;
		else//try the all lower case					//scr00064 -- MIC should be changed also.
		{												//scr00064
			char szTemp[256];							//scr00064 -- This code will try all lower
			for (int i = 0; szName[i] != NULL; i++)		//scr00064 -- but if MIC isn't doing lower
				szTemp[i] = tolower(szName[i]);			//scr00064 -- then the first test will
			if (hMutex = CreateMutex(NULL,TRUE,szTemp))	//scr00064 -- catch it.
			{											//scr00064 
				result = GetLastError();				//scr00064
				CloseHandle(hMutex);					//scr00064 
				ReleaseMutex(hMutex);					//scr00064 
				if (result == ERROR_ALREADY_EXISTS)		//scr00064 
					return TRUE;						//scr00064 
			}											//scr00064 
		}												//scr00064 	
	}
	else
	{
		result = GetLastError();
		if (
			(result == ERROR_ALREADY_EXISTS) ||
			(result == ERROR_ACCESS_DENIED)
			)
			return TRUE;
	}

	//look for window name
	char string[256];
	HWND handle;
	handle = FindWindowEx (NULL, NULL, NULL, NULL);
	while (handle)
	{
		::GetWindowText(handle, string, 256);
		if (stricmp(string,szName) == 0)				//SCR00064
			return TRUE;
	  handle = FindWindowEx (NULL, handle, NULL, NULL);
	}

	//look for class
	handle = FindWindowEx (NULL, NULL, NULL, NULL);
	while (handle)
	{
		::GetClassName(handle,string,256);
		if (stricmp(string,szName) == 0)				//scr00064
			return TRUE;
		handle = FindWindowEx (NULL, handle, NULL, NULL);
	}

	return FALSE;
}

void CServiceApp :: Stop() {
	// report to the SCM that we're about to stop
	// Note that the service might Sleep(), so we have to tell
	// the SCM
	//	"The next operation may take me up to 11 seconds. Please be patient."
	ReportStatus(SERVICE_STOP_PENDING, 11000);

	if( m_hStop )
		::SetEvent(m_hStop);
}
