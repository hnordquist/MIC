/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-040. This software was exported from the United States in accordance with the Export
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
#include <windows.h>
#include "NTService.h"

#ifndef _CONSOLE
	#include <stdlib.h>		// for declaration of __argv and __argc
#endif


class CSampleService : public CNTService {
	// the service control manager (SCM) starts "Run()" in a
	// different thread than "Stop()", so we need something
	// for syncronization. This sample uses an "Event". You
	// might use other syncronization methods too (mutexes
	// or whatever you like).
	HANDLE	m_hStop;

	public:
		// construct a new service.
		// The first parameter to the baseclass' constructor is the
		// name of the service. This appears in the event-log for
		// instance. The second parameter is the display name, thus
		// what you can read in the "Services" control panel applet.
		// If you ommit the second parameter, then it defaults to be
		// equal to the first one.
		CSampleService()
			: CNTService(TEXT("ProgStartService"), TEXT("Program Startup Service"))
			, m_hStop(0)
		{
		}


		// We have to implement at least these two methods:
		virtual void	Run(DWORD, LPTSTR *);
		virtual void	Stop();
		BOOL IsExistingInstanceOf(LPCSTR szName);
};


// ignore parameters in this sample service
void CSampleService :: Run(DWORD, LPTSTR *) {
	// report to the SCM that we're about to start
	ReportStatus(SERVICE_START_PENDING);
	
	m_hStop = ::CreateEvent(0, TRUE, FALSE, 0);

	// You might do some more initialization here.
	// Parameter processing for instance ...
	
	// report SERVICE_RUNNING immediately before you enter the main-loop
	// DON'T FORGET THIS!
	ReportStatus(SERVICE_RUNNING);

	// enter main-loop
	// If the Stop() method sets the event, then we will break out of
	// this loop.
	while( ::WaitForSingleObject(m_hStop, 10) != WAIT_OBJECT_0 ) {
		// popup a small message box every 10 seconds
		if (0==CopyFile("C:\\FACLOG.TXT","C:\\FacLog1.txt",FALSE))
			::MessageBox(0, TEXT("Copy Failed "), TEXT("MFC SampleService"),MB_OK);
		else
			::MessageBox(0, TEXT("Hi, here is your very first MFC based NT-service"), TEXT("MFC SampleService"), MB_OK);
		::Sleep( 10000 );
	}

	if( m_hStop )
		::CloseHandle(m_hStop);
}

BOOL CSampleService::IsExistingInstanceOf(LPCSTR szName) 
{
	if (OpenMutex (MUTEX_ALL_ACCESS, FALSE, szName) == NULL) 
			return FALSE;
	return TRUE;
}



void CSampleService :: Stop() {
	// report to the SCM that we're about to stop
	// Note that the service might Sleep(), so we have to tell
	// the SCM
	//	"The next operation may take me up to 11 seconds. Please be patient."
	ReportStatus(SERVICE_STOP_PENDING, 11000);

	if( m_hStop )
		::SetEvent(m_hStop);
}


#ifdef _CONSOLE
	int main( int argc, char ** argv ) {
#else
	int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	// non of the arguments is used when running on Win95
	int argc = __argc;
	char ** argv = __argv;
#endif	// _CONSOLE

	// create the service-object
	CSampleService serv;
	
	// RegisterService() checks the parameterlist for predefined switches
	// (such as -d or -i etc.; see NTService.h for possible switches) and
	// starts the services functionality.
	// You can use the return value from "RegisterService()"
	// as your exit-code.
	return serv.RegisterService(argc, argv);
}
