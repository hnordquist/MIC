//-----------------------------------------------------------------------------
//
// EOSS - Electronic Optical Sealing System
//
// MIC_EOSSReader Software
//
//
// (c) LANS LLC, 2009
// Written by David Pelowitz
// Modified by Joe Longo
//
// Based the Sample Seal Reader from
// (c) 2007 Dr. Neumann Consultants
// D-50259 Pulheim, Schiffgesweg 9, Germany
//
//-----------------------------------------------------------------------------
//
// This program uses the EOSS Crypto Dongle COM Server (TokCom.exe)
//
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "eoss.h"
#include "ATLComTime.h"
#include <conio.h>
#include <atlpath.h>

#define BUFSIZE MAX_PATH
#define VERBOSE
// this is supposed to be the version id of COEOSSISO.dll, but it is stored under 'reader version'
#define VERSION "2.0.1.1"
#define VERSIONX " 2011"

//the header we put at the 
//start of all LANL inst generated
//binary files
struct tagLANLHEADER
{
	//all blank filled right justified
	char	SizeOfHeader[4];//ascii length - these four. eg "  69"
	char	unused1[5];		//not used
	char	Version[5];		//4 char version sans "."	eg "2004"
	char	StationId[3];	//2 char station id			eg " 99"
	char	Year[3];		//2 char year.				eg " 08"		
	char	Month[3];		//2 char month.				eg " 01"
	char	Day[3];			//2 char day of month		eg " 01"
	char	FullYear[4];	//4 char year				eg "2008"
	char	unused2[43];	//blank filled
} LANLHEADER;

#include "depth.h"
#include <io.h>



CStringA LogFileName;


void MakeMyPath(const char* path)
{
	//recursive routine to create all 
	//of the directories down the "path"
	//NOTE: existence of a "file" with the 
	//same name as a node in the
	//path will cause this routine to fail
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];   
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];		
	if (_access(path,0)==0) return;
	_splitpath(path,drive,dir,fname,ext);
	if (strlen(dir)==0) return;
	_makepath(path_buffer,drive,dir,NULL,NULL);
	if (path_buffer[strlen(path_buffer)-1] == '\\')
		path_buffer[strlen(path_buffer)-1] = NULL;
	MakeMyPath(path_buffer);
	//make sure directory exists
//	_mkdir(path_buffer);
	::CreateDirectoryA(path_buffer,NULL);
}


void WriteToLogFile(USHORT noomber, char* Msg)
{
	FILE *File;
	char Message[512];
	Message[0] = NULL;
	COleDateTime TC;
		
	//	message = MICTIME::YY.MM.DD HH:MM:SS %u C (MESSAGE==NULL)?\r:MESSAGE
	TC = COleDateTime::GetCurrentTime();
	sprintf_s(Message,"%04d.%02d.%02d %02d:%02d:%02d %u C EOSSREAD %s",
		TC.GetYear(),
		TC.GetMonth(),	TC.GetDay(),
		TC.GetHour(),TC.GetMinute(),TC.GetSecond(),noomber, Msg[0]==NULL?"":Msg);
		

	//if fn does not exist (it may have been moved or we have a new day)
	if (_access(LogFileName,0) != 0)
	{
		//make sure path exists
		MakeMyPath(LogFileName);

		//CStringA ArchiveFileName;
		// to do stuff \\archive\\ in the file name here
				//if it exists in the subdirectory "archive" then move it and use it
		//if (_access(ArchiveFileName,0)==0)
		//	MoveFileEx(ArchiveFileName,fn,
		//		MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	}

	if ((File = fopen(LogFileName,"at+")) != NULL)
	{
		fseek(File,0,SEEK_END);
		fprintf(File,Message);
		fclose(File);
	}
}

void BuildESSFileName(CStringA &cFileNameFormat, CStringA& cFileName, CStringA& cInstID, COleDateTime cNow)
{
	bool bHaveEscape = false;
	CStringA cTemp;
	
	if (cFileNameFormat == CStringA("SHORTFORM"))
	{
		cFileName.Format("%c%c%c%c%cA00.ess",
			cInstID[0],cInstID[1],
			((cNow.GetYear()-1990)<10)?'0'+ cNow.GetYear()-1990 :'A' + cNow.GetYear()-2000,
			(cNow.GetMonth()      <10)?'0'+ cNow.GetMonth()     :'A' + cNow.GetMonth()-10,
			(cNow.GetDay()        <10)?'0'+ cNow.GetDay()       :'A' + cNow.GetDay()  -10 );
	}
	else
	{
		//build the physical file path and name from the format handed to us in the registry
		bool bDayFile = true;
		for (int iSourceIndex = 0; iSourceIndex < cFileNameFormat.GetLength(); iSourceIndex++)
		{
			if ((cFileNameFormat[iSourceIndex]=='%') && (!bHaveEscape))
				bHaveEscape = true;
			else if (bHaveEscape)
			{
				bHaveEscape = false;
				switch (cFileNameFormat[iSourceIndex]) 
				{
				case 't':
					cFileName += "EOSS";
					break;
				case 'T':
					cFileName += "EO";
					break;
				case 'I': // three digit station id, needs a leading 0 typically
					{
						CStringA xid;
						xid.Format("%03s", cInstID);
						cFileName += xid;
					}
					break;
     			case 'i':   // 2 digit station id
					{
						CStringA xid;
						xid.Format("%02s", cInstID);
						cFileName += xid;
					}
					break;
				case 'y':
					cTemp.Format("%04d",cNow.GetYear());
					cFileName += cTemp;
					break;
				case 'm':
					cTemp.Format("%02d",cNow.GetMonth());
					cFileName += cTemp;
					break;
				case 'd':
					cTemp.Format("%02d",cNow.GetDay());
					cFileName += cTemp;
					break;
				case 'H':
					cTemp.Format("%02d",cNow.GetHour());
					cFileName += bDayFile?"00":cTemp;
					bDayFile = true;
					break;
				case 'M':
					cTemp.Format("%02d",cNow.GetMinute());
					cFileName += bDayFile?"00":cTemp;
					bDayFile = true;
					break;
				case 'S':
					cTemp.Format("%02d",cNow.GetSecond());
					cFileName += bDayFile?"00":cTemp;
					bDayFile = true;
					break;
				case 'J':
					cFileName += "%J";
					break;
				case '\\':
				case '/':
				case '?':
				case '*':
				case ':':
				case ';':
				case '>':
				case '<':
				case '\"':
					break;
				default:
					cFileName += cFileNameFormat[iSourceIndex];
				}
			}
			else
			{
				if (cFileNameFormat[iSourceIndex] == '*')
					bDayFile = false;
				else if (strchr("\\/?:;><\"",cFileNameFormat[iSourceIndex]) == NULL)
					cFileName += cFileNameFormat[iSourceIndex];
			}
		}
	}
}

void ERLog(bool bUseLog, char* Msg, USHORT naamber)
{
	if (bUseLog)
	{
		if (!LogFileName.IsEmpty())
		{
			WriteToLogFile(naamber, Msg);
			return;
		}
	}

	printf(Msg);
}


//
// UpdateRegistry
//
// write LATEST READ and LATEST RESULTS to the general section
//

void UpdateRegistryLatest(char* szName, char* szResults, COleDateTime cNow, QueryLevel Level, bool logging)
{
	LONG lRet;
	HKEY hKey;

	//open the registry where the values are stored
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, szName, 0,KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	//if successful then write the two values' times
	if (lRet == ERROR_SUCCESS)
	{
			CStringA cTemp;
			cTemp = cNow.Format(_T("%Y\\%m\\%d %H:%M:%S"));
			lRet = RegSetValueExA(hKey,"LATEST READ",NULL,REG_SZ,(const BYTE *)(LPCSTR)cTemp,cTemp.GetLength()+1);
			lRet = RegSetValueExA(hKey,"LATEST RESULTS",NULL,REG_SZ,(BYTE*)szResults,(DWORD)(strlen(szResults)+1));
			cTemp.Format("%.64s", levelnames[Level]), 
			lRet = RegSetValueExA(hKey,QUERY_DEPTH,NULL,REG_SZ,(const BYTE *)(LPCSTR)cTemp,cTemp.GetLength()+1);
			RegCloseKey(hKey);
	}
	else
	{
		//dp: if not successful then there isn't any place to put an error message
		//jfl: can now use optional log file
		char buff[1024];
		sprintf_s(buff, "Failed to open registry at %s (%d)\n", szName, lRet);ERLog(logging, buff, 47700);
	}
}

//
//write LATEST READ and LATEST RESULTS to the specific EOSS's entries

void UpdateRegistryLatestForSeal(char* szName, char* szResults, COleDateTime cNow, unsigned int uiEOSS)
{
	LONG lRet;
	HKEY hKey;
	HKEY hResult;
	//open the registry where the values are stored
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		//if successful then create the EOSS's section (should already exist)
		//and write the two values' times
		DWORD dwDisposition;
		CStringA cTemp;
		cTemp.Format("%u",uiEOSS);
		lRet = RegCreateKeyExA(hKey,cTemp,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&hResult,&dwDisposition);
		if (lRet == ERROR_SUCCESS)
		{
			cTemp = cNow.Format(_T("%Y\\%m\\%d %H:%M:%S"));
			lRet = RegSetValueExA(hResult,"LATEST READ",NULL,REG_SZ,(const BYTE *)(LPCSTR)cTemp,cTemp.GetLength()+1);
			lRet = RegSetValueExA(hResult,"LATEST RESULTS",NULL,REG_SZ,(BYTE*)szResults,(DWORD)(strlen(szResults)+1));
			RegCloseKey(hResult);
		}
		RegCloseKey( hKey );
	}
}

//
//Clear Registry
//
//Open the key
//Delete (Clear) all of the entries in the uiAddress/EOSS
//Close the key
//
void ClearRegistry(char* szName,unsigned int uiAddress, bool bVerbose, bool logging)
{
	LONG lRet;
	HKEY hKey;
	char szTemp[16];
	sprintf_s(szTemp,sizeof(szTemp),"%u",uiAddress);
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_ALL_ACCESS/*KEY_QUERY_VALUE | KEY_SET_VALUE*/, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		lRet = RegDeleteKeyA(hKey,szTemp);
		if (bVerbose)
		{
			if (lRet != ERROR_SUCCESS)
			{
				char buff[512];
				sprintf_s(buff, "Failed to clear reg: %s %s %x\n",szName,szTemp, lRet); ERLog(logging, buff, 47747);  // todo, pass arg
			}
		}
		RegCloseKey(hKey);
	}
	else
	{	
		if (bVerbose) {
			char buff[512];
			sprintf_s(buff, "Failed to open reg: %s %s %x\n",szName,szTemp, lRet);ERLog(logging, buff, 47748);
		}
	}
}

//
//Update Registry
//
//Open the key
//Add/Modify a value in the registry
//Close the key
//
void UpdateRegistry(char* szName, char* szType, BYTE* szResults, DWORD dwSize,unsigned int uiEOSS)
{
	LONG lRet;
	HKEY hKey;
	HKEY hResult;

	//open the registry where the values are stored
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		if (uiEOSS)
		{	
			DWORD dwDisposition;
			CStringA cTemp;
			cTemp.Format("%u",uiEOSS);
			lRet = RegCreateKeyExA(hKey,cTemp,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&hResult,&dwDisposition);
			if (lRet == ERROR_SUCCESS)
			{
				lRet = RegSetValueExA(hResult,szType,NULL,REG_BINARY,(BYTE*)szResults,dwSize);
				RegCloseKey(hResult);
			}
			RegCloseKey( hKey );
		}
		else
		{
			lRet = RegSetValueExA(hKey,szType,NULL,REG_BINARY,(BYTE*)szResults,dwSize);
			RegCloseKey( hKey );
		}
	}
}
//
// KillEOSS
//
//This kills the process "EOSS Crypto Token" if it can be found and can be used to
//kill the process that Dr Neumann's stuff starts up.  I've left this code here just
//in case we need to add it later.  Currently, MIC will watch for the "EOSS Crypto Token"
//process a few seconds prior to the schedule of this program and MIC will kill it if found.
//

void KillEOSS() 
{
	//	FILE* outfile;
	//	fopen_s(&outfile,"C:\\KillEOSSOut.txt","at");

	char string[256];
	HWND handle;
	handle = FindWindowEx (NULL, NULL, NULL, NULL);
	while (handle)
	{
		::GetWindowTextA(handle, string,256);
		if (strlen(string))  
		{
			//			fprintf(outfile,"\n%s ",string);
			if (strlen(string))
			{
				if (strstr(string,"EOSS Crypto Token"))
				{
					char szTemp[MAX_PATH];
					GetClassNameA(handle,szTemp,sizeof(szTemp));
					::PostMessage(handle,WM_QUIT,0,0);
					//					fprintf(outfile,"KILL THIS ONE");
				}
			}
		}
		handle = FindWindowEx (NULL, handle, NULL, NULL);
	}

	CloseHandle(handle);

}

int parseCSVList (char *szEOSSBuffer,  ADDRESS* pEOSSEs, int& iSealCount)
{
	char* next_token = NULL;
	char* szAt = strtok_s(szEOSSBuffer,",",&next_token);
	while ((szAt) && (iSealCount < MAX_EOSS))
	{
		sscanf_s(szAt,"%lu", &(pEOSSEs[iSealCount]), strlen(szAt));
		szAt = strtok_s(NULL,",",&next_token);
		iSealCount++;
	}
	return iSealCount;
}


#define LOGIT(n, m) ERLog(bLogToFile, m, n)

//
//main
//
//The execution thread
//
int _tmain(int argc, _TCHAR* argv[])
{
	int iReturn = 0;
	
	FILE *outfile;
	COleDateTime cNow;

	CStringA cFileName;
	CStringA cFileNameFormat;

	//default instrument ID is 999
	CStringA cInstID("999");
	//default time offset to UTC is 0 hours
	CStringA cTimeOffset("000");
	//default com port is 1
	int iComPort = 1;
	//default path to write results out to is c: 
	CStringA cPath("c:\\");

	char szId[MAX_PATH];
	char szTemp2[MAX_PATH];
	char szName[MAX_PATH];
	char szMicIniFileArg[MAX_PATH];
	char cmdlinerep[MAX_PATH+128];
	char szLogLoc[MAX_PATH];

	_TCHAR wId[MAX_PATH];

	const char fflag[] = "-f";  // name of ini file with MIC config 
	const char logflag[] = "-log";  // asking for a log of output rather than prtinf to console (NYI)
	const char vflag[] = "-v";     // output plenty of status and progress messages
	const char dbgflag[] = "-dbg"; // pop up debug interruption dialog
	const char lvlflag[] = "-lvl";  // specfy query level 0 -> seals there or not there, 1 Seal challenge good, 2 1 + actual seal status, 3 2 + log, 4 3 + delta of log, 5 is as much as possible

	//mutex; MIC and this program will use this to detect if this program is running
	HANDLE hMutex = NULL;

	HKEY hKey;
	char szString[BUFSIZE];
	DWORD dwBufLen=BUFSIZE;
	LONG lRet;
	HKEY hResult;
	BOOL bAnyErrors = false;
	BOOL bAnyLocalErrors;
	int iSealCount;
	ADDRESS EOSSes[MAX_EOSS];
	
	//
	//if this program is already running then wait here until previous
	//run finishes -- also used to tell MIC it is running
	while (hMutex == NULL)
	{
		//if we can't create the mutex (eg it already exists) then
		//if it already exists then repeat wait 1 second and try again
		hMutex = CreateMutexA(NULL,TRUE,"MIC_EOSSReader");
		DWORD dwResult = GetLastError();
		if (hMutex == NULL)
		{
			//should never do this first test unless the documentation is wrong
			if (dwResult == ERROR_ALREADY_EXISTS)
				Sleep(1000);
			else
				return(-2);
		}
		else
		{
			if (dwResult == ERROR_ALREADY_EXISTS)
			{
				CloseHandle(hMutex);
				hMutex = NULL;
				Sleep(1000);
			}
		}
	}

	//init array of EOSSes to indicate none currently found
	memset(EOSSes,0,sizeof(EOSSes));
	memset(szMicIniFileArg,0,sizeof(szMicIniFileArg));
	memset(wId,0,sizeof(wId));

	//concatenate all of the command line parameters up to the -f, -dbg or -v tokens to form the MIC ISO name 
	size_t iI;
	szId[0] = NULL;
	bool bVerbose = false;
	bool bDbg = false;
	bool bLogToFile = false;
	bool bLevelFlagOverride = false;

	bool bUseInterval = false;
	UINT Interval = 1;
	QueryLevel Level = eReadAllData;
	UINT ReaderCount = 0;

	cmdlinerep[0] = 0;

	char LogBuffer[512];
	
	try
	{
	// args follow this scheme  sectionname [-dbg|-v|-log] [-f mic ini filename] [-lvl [0..5]]
	if (argc > 1)
	{
		bool vFileNameFlag = false, vLevelFlag = false;
		int i = 1;
		while (i < argc)
		{
			wcstombs_s(&iI,szTemp2,argv[i],sizeof(szTemp2));
			strcat_s(cmdlinerep, szTemp2);strcat_s(cmdlinerep, " ");
			if (!vFileNameFlag && !vLevelFlag)
			{
				//if (!bDbg || !bVerbose)
				{
					if (_strnicmp(szTemp2,dbgflag,sizeof(dbgflag)) == 0)  // pop-up a simple OK dialog to cause debugger intervention opportunity
					{
						bDbg = true;
					}
					else if (_strnicmp(szTemp2,vflag,sizeof(vflag)) == 0)  // output to console some drivel
					{
						bVerbose = true;
					}
					else if (_strnicmp(szTemp2,fflag,sizeof(fflag)) == 0)  // prepare to gather id list
					{
						vFileNameFlag = true;
					}
					else if (_strnicmp(szTemp2,logflag,sizeof(logflag)) == 0)  // prepare to log to a file rather than the console
					{
						bLogToFile = true;
					}
					else if (_strnicmp(szTemp2,lvlflag,sizeof(lvlflag)) == 0)  // specify the query depth and level
					{
						vLevelFlag= true;
					}
					else // gather name text
					{
						if (szId[0] != NULL)
							strcat_s(szId,sizeof(szId)," ");
						strcat_s(szId,sizeof(szId),szTemp2);
						wcsncat_s(wId, argv[i], MAX_PATH);
					}
				}
			}
			else if (vFileNameFlag) // one entry for the path: all text up to the first -
			{
				if (szMicIniFileArg[0] != NULL)
					strcat_s(szMicIniFileArg,sizeof(szMicIniFileArg)," ");
				strcat_s(szMicIniFileArg,sizeof(szMicIniFileArg),szTemp2); 
				vFileNameFlag = false;
			}
			else if (vLevelFlag) // one number
			{
				int l = _wtoi(argv[i]);
				if (l >= 0 && l <= eQueryLevelMax) 
					Level = QueryLevel (_wtoi(argv[i]));
				vLevelFlag = false;
				bLevelFlagOverride = true;
			}
			i++;
		}
	}
	else
	{
		strcpy_s(szId,"UNKNOWN");
		wcsncpy_s(wId,L"UNKNOWN", MAX_PATH);
	}
	if (strlen(szMicIniFileArg) < 2)
		strcpy_s(szMicIniFileArg,"C:\\Mic\\Mic.ini");

	if (bDbg ||
#ifdef _DEBUG
		true
#else
		false
#endif
		)
		::MessageBox(NULL, _T("debug break"), _T("MIC EOSS Reader"), 0);


	//this is where we will place all of our registry stuff
	strcpy_s(szName,"SOFTWARE\\Los Alamos National Laboratory\\MIC_GM");

	//save the time we started
	cNow = COleDateTime::GetCurrentTime();

	//open the registry where the values are stored
	//	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_QUERY_VALUE, &hKey);
	DWORD dwDisposition;
	lRet = RegCreateKeyExA(HKEY_LOCAL_MACHINE,szName,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition);

	strcat_s(szName,"\\");
	strcat_s(szName,szId);

	//if we couldn't open it then we have a new one and must add default values
	//typically, this should never happen as MIC should have created it for us
	if (lRet == ERROR_SUCCESS)
	{
		DWORD dwDisposition;
		lRet = RegCreateKeyExA(hKey,szId,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&hResult,&dwDisposition);
		if (lRet == ERROR_SUCCESS)
		{
			if(dwDisposition == REG_CREATED_NEW_KEY)
			{
				lRet = RegSetValueExA(hResult,"COMPORT",NULL,REG_SZ,(BYTE*)"1",2);
				lRet = RegSetValueExA(hResult,"PATH",NULL,REG_SZ,(BYTE*)"C:\\",4);
				lRet = RegSetValueExA(hResult,"FORMAT",NULL,REG_SZ,(BYTE*)"EOSS %I %y_%m_%d.ess",21);
				lRet = RegSetValueExA(hResult,"INSTRUMENT ID",NULL,REG_SZ,(BYTE*)"999",4);
				lRet = RegSetValueExA(hResult,"LATEST READ",NULL,REG_SZ,(BYTE*)"NEVER",6);
				lRet = RegSetValueExA(hResult,"LATEST RESULTS",NULL,REG_SZ,(BYTE*)"NONE",5);
				lRet = RegSetValueExA(hResult,"SPECIFIED_EOSSes",NULL,REG_SZ,(BYTE*)"",512);
				lRet = RegSetValueExA(hResult,"READER VERSION",NULL,REG_SZ,(BYTE*)VERSION,(DWORD)strlen(VERSION)+1);
				lRet = RegSetValueExA(hResult,"TIME OFFSET",NULL,REG_SZ,(BYTE*)"000",4);
				lRet = RegSetValueExA(hResult,QUERY_DEPTH,NULL,REG_SZ,(BYTE*)levelnames[Level],(DWORD)strlen(levelnames[Level])+1);
			}
			RegCloseKey( hResult );
		}
		RegCloseKey(hKey);
	}
	else
	{
		//exit gracefully
		sprintf_s(LogBuffer, "Failed to open registry at %s\n", szName);LOGIT(47700, LogBuffer);
		UpdateRegistryLatest(szName,"Error: Failed to open registry",cNow,Level,bLogToFile);
		if (hMutex)
		{
			CloseHandle(hMutex);
			ReleaseMutex(hMutex);
		}
		iReturn = 1;
		goto SAUL_GOODMAN;
	}

	iSealCount = 0;

	//open the registry where the values are stored
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	if ((lRet == ERROR_SUCCESS))// && (dwBufLen <= BUFSIZE))
	{
		//get the com port
		dwBufLen = BUFSIZE-1;
		lRet = RegQueryValueExA(hKey,"COMPORT",NULL,NULL,(LPBYTE)szString,&dwBufLen);
		if ((lRet == ERROR_SUCCESS) && (dwBufLen <= BUFSIZE))
			sscanf_s(szString,"%d",&iComPort);

		//get the path for the day file
		dwBufLen = BUFSIZE-1;
		lRet = RegQueryValueExA(hKey,"PATH",NULL,NULL,(LPBYTE)szString,&dwBufLen);
		if ((lRet == ERROR_SUCCESS) && (dwBufLen <= BUFSIZE))
			cPath = szString;

		//get the file name format
		dwBufLen = BUFSIZE-1;
		lRet = RegQueryValueExA(hKey,"FORMAT",NULL,NULL,(LPBYTE)szString,&dwBufLen);
		if ((lRet == ERROR_SUCCESS) && (dwBufLen <= BUFSIZE))
			cFileNameFormat = szString;

		//get the instrument ID
		dwBufLen = BUFSIZE-1;
		lRet = RegQueryValueExA(hKey,"INSTRUMENT ID",NULL,NULL,(LPBYTE)szString,&dwBufLen);
		if ((lRet == ERROR_SUCCESS) && (dwBufLen <= BUFSIZE))
			cInstID = szString;

		//get the time offset
		dwBufLen = BUFSIZE-1;
		lRet = RegQueryValueExA(hKey,"TIME OFFSET",NULL,NULL,(LPBYTE)szString,&dwBufLen);
		if ((lRet == ERROR_SUCCESS) && (dwBufLen <= BUFSIZE))
			cTimeOffset = szString;

		//get the EOSSes from MIC
		char szEOSSBuffer[512];
		dwBufLen = sizeof(szEOSSBuffer)-1;
		lRet = RegQueryValueExA(hKey,"SPECIFIED_EOSSes",NULL,NULL,(LPBYTE)szEOSSBuffer,&dwBufLen);
		if ((lRet == ERROR_SUCCESS) && (dwBufLen <= sizeof(szEOSSBuffer)-1))
		{
			parseCSVList(szEOSSBuffer, (ADDRESS *)EOSSes, iSealCount);
		}

		dwBufLen = BUFSIZE-1;
		lRet = RegQueryValueExA(hKey,"TIME OFFSET",NULL,NULL,(LPBYTE)szString,&dwBufLen);
		if ((lRet == ERROR_SUCCESS) && (dwBufLen <= BUFSIZE))
			cTimeOffset = szString;

		// get the interval settings
		{
			DWORD dwType;
			UINT ulv = 0;
			UINT ulvflag = 0;
			ULONG ulBuffSize = 0;
			ulBuffSize = sizeof(ulv);
						
			lRet = RegQueryValueExA(hKey, READ_COUNT,NULL,&dwType,(LPBYTE)&ulv,&ulBuffSize);
			if (lRet == ERROR_SUCCESS)
				ReaderCount = ulv;

			lRet = RegQueryValueExA(hKey, FULL_DATA_INTERVAL,NULL,&dwType,(LPBYTE)&ulv,&ulBuffSize);
			if (lRet == ERROR_SUCCESS)
				Interval = ulv;

			ulBuffSize = sizeof(ulvflag);
			lRet = RegQueryValueExA(hKey,USE_FULL_DATA_INTERVAL,NULL,&dwType,(LPBYTE)&ulvflag,&ulBuffSize);
			if (lRet == ERROR_SUCCESS)
				bUseInterval = (ulvflag ? true : false);
			
			lRet = RegQueryValueExA(hKey,USE_OVERRIDE_QUERY_DEPTH,NULL,&dwType,(LPBYTE)&ulvflag,&ulBuffSize);
			if (lRet == ERROR_SUCCESS)
			{
				if (!bLevelFlagOverride) // no -lvl on cmd line, it takes precedence over this registry flag from MIC COM
				{
					bLevelFlagOverride = (ulvflag ? true : false);
					if (bLevelFlagOverride) // if registry overrides default state and no -lvl on cmd line, then use the registry entry
					{
						ulBuffSize = sizeof(ulv);
						lRet = RegQueryValueExA(hKey, OVERRIDE_QUERY_DEPTH,NULL,&dwType,(LPBYTE)&ulv,&ulBuffSize);
						if (lRet == ERROR_SUCCESS)
						{
							if (ulv >= 0 && ulv <= eQueryLevelMax) 
								Level = QueryLevel (ulv);
						}
					}
				}
			}
		}

		//set the version 
		lRet = RegSetValueExA(hKey,"READER VERSION",NULL,REG_SZ,(BYTE*)VERSION,(DWORD)strlen(VERSION)+1);

		RegCloseKey( hKey );
	}
	else
	{
		//exit gracefully
		//RegCloseKey( hKey );failed to open so don't need to close
		sprintf_s(LogBuffer, "Failed to open registry at %s\n",szName);LOGIT(47700, LogBuffer);
		UpdateRegistryLatest(szName,"Error: Failed to open registry",cNow,Level,bLogToFile);
		if (hMutex)
		{
			CloseHandle(hMutex);
			ReleaseMutex(hMutex);
		}
		iReturn = 1;
		goto SAUL_GOODMAN;
	}
	if (!bLevelFlagOverride)
	{
		if (bUseInterval) // set depth by interval mod technique
		{
			bool bFull = true;
			if (Interval > 0)
				bFull = (((ReaderCount + 1) % Interval) == 0);
			Level = (bFull ? eReadAllData : eStatusAndLog);
		}
	}

	::GetPrivateProfileStringA(szId,"SAVE_LOC","C:\\DATA",szLogLoc,sizeof(szLogLoc),szMicIniFileArg);

	BuildESSFileName(cFileNameFormat,cFileName, cInstID, cNow);
	if (bLogToFile)
	{
		LogFileName = cFileName;
		LogFileName.Replace(".ess", ".rlog"); // todo change default -log in COM to cue on boolean toggle, unhide boolean toggle in UI)
		if (cPath.Right(1) != "\\") cPath += "\\";
		LogFileName = cPath + LogFileName;
	}
	
	if (bVerbose)
	{
		sprintf_s(LogBuffer, "MIC_EOSSReader starting\n", cmdlinerep);LOGIT(47701, LogBuffer);

		sprintf_s(LogBuffer, "PARAMETERS: %s\n", cmdlinerep);LOGIT(47710, LogBuffer);
		sprintf_s(LogBuffer, "COMPORT: %d\n",iComPort);LOGIT(47711, LogBuffer);
		sprintf_s(LogBuffer, "PATH: %s\n",cPath);LOGIT(47712, LogBuffer);
		// bad %% &c; printf behavior, avoid for now sprintf_s(LogBuffer, "FORMAT: %.64s\n",cFileNameFormat);LOGIT(LogBuffer);
		sprintf_s(LogBuffer, "INST ID: %s\n",cInstID);LOGIT(47713, LogBuffer);
		sprintf_s(LogBuffer, "Query depth: %s\n",levelnames[Level]);LOGIT(47714, LogBuffer);
		if (bLevelFlagOverride) 
		{
			sprintf_s(LogBuffer, "Query depth override\n");  LOGIT(47715, LogBuffer); //why did it override? flag or registry?
			
		}
		else
		{
			if (bUseInterval)
			{
					sprintf_s(LogBuffer, "Interval use is %d; Interval is %d; Count %d\n",bUseInterval, Interval, ReaderCount + 1);			LOGIT(47716, LogBuffer);
			}
			else 
			{
				sprintf_s(LogBuffer, "Not using interval depth collection (Interval is %d; Count %d)\n",bUseInterval, Interval, ReaderCount + 1);			LOGIT(47717, LogBuffer);
			}

		}
		if (bLogToFile)
		{
			sprintf_s(LogBuffer, "LOG FILE: %s\n", LogFileName);LOGIT(47718, LogBuffer);
		}
	}

	if (iSealCount <= 0)
	{
		char szEOSSes[512];
		szEOSSes[0]=0;
		::GetPrivateProfileStringA(szId,"EOSSes","",szEOSSes,sizeof(szEOSSes),szMicIniFileArg);
		if (bVerbose)
		{
			sprintf_s(LogBuffer, "ID list: %.256s\n",szEOSSes);LOGIT(47719, LogBuffer);
		}
		parseCSVList(szEOSSes,(ADDRESS *)EOSSes, iSealCount);
	}

	//check comport
	if (iComPort < 1)
	{
		//exit gracefully
		sprintf_s(LogBuffer, "Failed due to invalid com port number (less than 1)\n");LOGIT(47720, LogBuffer);
		UpdateRegistryLatest(szName,"Error: Failed due to invalid com port number (less than 1)",cNow,Level,bLogToFile);
		if (hMutex)
		{
			CloseHandle(hMutex);
			ReleaseMutex(hMutex);
		}
		iReturn = 1;
		goto SAUL_GOODMAN;
	}

	}
	catch (...)
	{
		//exit gracefully
		sprintf_s(LogBuffer, "Exception in seal query preparation\n");LOGIT(47721, LogBuffer);
		UpdateRegistryLatest(szName,"Error: Exception in seal query preparation",cNow,Level,bLogToFile);
		if (hMutex)
		{
			CloseHandle(hMutex);
			ReleaseMutex(hMutex);
		}
		iReturn = 1;
		goto SAUL_GOODMAN;
	}


	CoInitialize(NULL);

	//here is where the real work begins
	try
	{
		ADDRESS Address = 0;

		//
		// comport object, constructor may throw an exception
		// the com port of the EOSS devices
		CComport Comport(iComPort, 1000);

		//
		// crypto dongle object, constructor may throw an exception
		// 
		CDongle Dongle; 

		//
		// seal object
		// 
		CEoss Eoss(0, &Comport, &Dongle);

		//
		// get list of seals on this port
		//

		//		if (!Eoss.GetSealInfo())
		if (iSealCount <= 0)
		{
			if (bVerbose) {
				sprintf_s(LogBuffer, "No seal IDs found in the registry nor the Mic ini file\n");LOGIT(47722, LogBuffer);
			}
			UpdateRegistryLatest(szName,"No seal IDs found in the registry nor the Mic ini file",cNow,Level,bLogToFile);
			if (hMutex)
			{
				CloseHandle(hMutex);
				ReleaseMutex(hMutex);
			}
			iReturn = 1;
			goto TYRONE_SLOTHROP;
		}
		for (int i = 0; i < iSealCount; i++)
			Eoss.EOSSes[i] = EOSSes[i];
		//
		//build path/filename and write LANL header if new file
		//
		if (cPath.Right(1) != "\\") cPath += "\\";
		cPath += cFileName;	
		bool bFileOpen = (fopen_s(&outfile,cPath,"a+b") == 0);
		
		if (bFileOpen)
		{
			//go to the end of the binary output file
			fseek(outfile,0,SEEK_END);
			fpos_t pos;
			//if at the start of the file then write the header
			if (fgetpos(outfile,&pos)==0)
			{
				if (pos == 0)
				{
					//initialize header
					char temp[64];
					memset( &LANLHEADER, ' ', sizeof(LANLHEADER));

					//fill header
					//size of header beyond first four bytes
					sprintf_s(temp,"%4d",sizeof(LANLHEADER)-4);
					memcpy(LANLHEADER.SizeOfHeader,temp,4);

					memcpy(LANLHEADER.Version,VERSIONX,5);
					sprintf_s(temp,"%3s",cInstID);
					memcpy(LANLHEADER.StationId,temp,3);
					sprintf_s(temp,"%3d",cNow.GetYear()%100);
					memcpy(LANLHEADER.Year,temp,3);
					sprintf_s(temp,"%3d",cNow.GetMonth());
					memcpy(LANLHEADER.Month,temp,3);
					sprintf_s(temp,"%3d",cNow.GetDay());
					memcpy(LANLHEADER.Day,temp,3);
					sprintf_s(temp,"%4d",cNow.GetYear());
					memcpy(LANLHEADER.FullYear,temp,4);
					//write header
					fwrite(&LANLHEADER,sizeof(LANLHEADER),1,outfile);
				}
			}
		}
		else
		{
			ULONG val = 0;
			_get_doserrno(&val);
			char tempO[260];
			char tempP[512];
			_strerror_s(tempO,256,"Unable to open data file");
			CStringA::PCXSTR p = cPath.GetString();
			sprintf_s(tempP, sizeof(tempP), "%.128s; %.256s\n", (char*)tempO, p);
			if (bVerbose)
			{
				LOGIT(47723, tempP);
			}
			UpdateRegistryLatest(szName,tempP,cNow,Level,bLogToFile);

		}

		//
		// save to registry the block of found seals
		//
		if (bFileOpen)
			fwrite(Eoss.EOSSes,sizeof(Eoss.EOSSes),1,outfile);
		//		UpdateRegistry(szName, "SPECIFIED_EOSSes", (BYTE*)Eoss.EOSSes, (DWORD)sizeof(Eoss.EOSSes),0);

		//
		//for each of the seals read 
		//Status/Log info/all seal log pages/all system log pages/all soh log pages
		//
		int iIndex = 0;
		unsigned int uiAddress;
		bool bIdentifiedAtLeastOneSeal = false;
		STATUS Status;
#pragma pack(1)
		struct tagTIMEREC
		{
			double dt;						//COleDateTime::dt
			COleDateTime::DateTimeStatus status;
			short sOffset;					//hours to correct for UTC
			char cVersion[2];				//Version of this software
		} sTimeRec;
#pragma pack()
		LOGINFO LogInfo;
		LOGPAGE LogPage;
		int iLogPageCounter = 0;
		char szPage[16];

		while (((uiAddress = Eoss.GetAddress(iIndex)) != 0) && (iIndex < MAX_EOSS))
		{
			iLogPageCounter = 0;
			bAnyLocalErrors = false;
			if (bVerbose) {

				sprintf_s(LogBuffer, "Set Current Address[%d]: %d\n",iIndex,uiAddress);LOGIT(47724, LogBuffer);
			}
			//let the Eoss software know which seal we'll be talking to
			//after this iIndex will point to the next seal
			Eoss.SetCurrentAddress(iIndex++);

			//delete any existing registry information for this seal
			ClearRegistry(szName,uiAddress,bVerbose, bLogToFile);

			//
			// initialize challenge-response protocol
			//
			//if we fail challenge then go on to the next EOSS
			//
			if (bVerbose) {
				sprintf_s(LogBuffer,"Get Challenge\n");LOGIT(47725, LogBuffer);
			}
			if (!Eoss.GetChallenge())
			{
				bAnyLocalErrors = bAnyErrors = true;
				UpdateRegistryLatestForSeal(szName,"Failed Challenge",cNow,uiAddress);
				memset(&Status,0,sizeof(Status));
				memcpy(&Status,"FAILED CHALLENGE",16);
				if (bFileOpen)
					fwrite(&Status,sizeof(Status),1,outfile);
				if (bVerbose) {
					sprintf_s(LogBuffer,"Failed Challenge: [%d]:%d %S\n",iIndex,uiAddress, Eoss.GetNotes() );LOGIT(47726, LogBuffer);Eoss.ResetNotes();
				}				
				continue;//continue at end of while (on the next address)
			}
			else
			{
				bIdentifiedAtLeastOneSeal = true;
			}

			if (Level < eStatus) // eSealChallenge: done for now, move on to next one
			{
				UpdateRegistryLatestForSeal(szName,"No errors occurred in the seal challenge query",cNow,uiAddress);
				continue;
			}
			//
			// log on
			// 
			//if we can't log onto this EOSS then issue a status block
			//indicating the problem and continue with the next EOSS
			//
			if (bVerbose) {
				sprintf_s(LogBuffer,"Log on\n");LOGIT(47727, LogBuffer);
			}
			if (!Eoss.Logon("MIC", "MIC"))
			{
				if (bVerbose) {
					sprintf_s(LogBuffer,"Failed 1st LOGON: %d %S\n",uiAddress, Eoss.GetNotes() );LOGIT(47728, LogBuffer);Eoss.ResetNotes();
				}
				Sleep(3000);
				if (!Eoss.Logon("MIC", "MIC"))
				{	
					if (bVerbose) {
						sprintf_s(LogBuffer,"Failed 2nd LOGON: %d %S\n",uiAddress, Eoss.GetNotes() );LOGIT(47729, LogBuffer);Eoss.ResetNotes();
					}
					Sleep(4000);
					if (!Eoss.Logon("MIC", "MIC"))
					{
						if (bVerbose) {
							sprintf_s(LogBuffer,"Failed 3rd LOGON: %d %S\n",uiAddress, Eoss.GetNotes() );LOGIT(47730, LogBuffer);Eoss.ResetNotes();
						}
						bAnyLocalErrors = bAnyErrors = true;
						UpdateRegistryLatestForSeal(szName,"Failed to log on",cNow,uiAddress);
						memset(&Status,0,sizeof(Status));
						memcpy(&Status,"FAILED LOGON",13);
						if (bFileOpen)
							fwrite(&Status,sizeof(Status),1,outfile);
						if (bVerbose) {
							sprintf_s(LogBuffer,"Failed LOGON: %d\n",uiAddress);LOGIT(47731, LogBuffer);
						}
						continue;//continue at end of while with next address
					}
				}
			}

			//
			// save the key info
			//
			UpdateRegistry(szName,"KEY NUMBER",(BYTE*)&Eoss.m_KeysetNumber,sizeof(Eoss.m_KeysetNumber),uiAddress);
			UpdateRegistry(szName,"KEY TIME",(BYTE*)&Eoss.m_KeysetTime,sizeof(Eoss.m_KeysetTime),uiAddress);
			UpdateRegistry(szName,"VERSION",(BYTE*)&Eoss.m_Version,sizeof(Eoss.m_Version),uiAddress);

			// get seal status
			//
			//if we can't get the status record then go on to the next EOSS
			//
			if (bVerbose) {
				sprintf_s(LogBuffer,"Get Status\n");LOGIT(47732, LogBuffer);
			}
			cNow = COleDateTime::GetCurrentTime();
			if (!Eoss.GetStatus(&Status))
			{
				//try a second time to get status
				if (!Eoss.GetStatus(&Status))
				{
					bAnyLocalErrors = bAnyErrors = true;
					UpdateRegistryLatestForSeal(szName,"Failed to get status",cNow,uiAddress);
					memset(&Status,0,sizeof(Status));
					memcpy(&Status,"FAILED READ",11);
					if (bFileOpen)
						fwrite(&Status,sizeof(Status),1,outfile);
					if (bVerbose) {
						sprintf_s(LogBuffer,"Failed STATUS read [%d]: %d\n",iIndex,uiAddress);LOGIT(47733, LogBuffer);
					}
					continue;
				}
			}

			//
			// save the status block
			//
			UpdateRegistry(szName, "STATUS", (BYTE*)&Status, sizeof(Status),uiAddress);
			if (bFileOpen)
				fwrite(&Status,sizeof(Status),1,outfile);

			//
			//save out the local time record! 
			//
			//tell the registry and the binary file what time we started all of this
			sTimeRec.dt = cNow.m_dt;
			sTimeRec.status = cNow.m_status;
			sTimeRec.sOffset = (short)atoi(cTimeOffset);
			sTimeRec.cVersion[0] = Eoss.m_Version[0];
			sTimeRec.cVersion[1] = Eoss.m_Version[1];
			UpdateRegistry(szName,"LOCAL TIME",(BYTE*)&sTimeRec,sizeof(sTimeRec),uiAddress);
			if (bFileOpen)
				fwrite(&sTimeRec,sizeof(sTimeRec),1,outfile);	

			if (Level < eStatusAndLog) // eStatus: done for now, move on to next one
			{
				UpdateRegistryLatestForSeal(szName,"No errors occurred in the seal status query",cNow,uiAddress);
				continue;
			}

			//
			// get log information
			//
			//Must have log info to read the logs, therefor if we error here simply go
			//to the next EOSS
			//
			if (bVerbose) {
				sprintf_s(LogBuffer,"Get Log Info\n");LOGIT(47734, LogBuffer);
			}
			if (!Eoss.GetLogInfo(&LogInfo))
			{
				//try a second time to get log info
				if (!Eoss.GetLogInfo(&LogInfo))
				{
					bAnyLocalErrors = bAnyErrors = true;
					UpdateRegistryLatestForSeal(szName,"Failed to get log info",cNow,uiAddress);
					memset(&LogInfo,0,sizeof(LogInfo));
					memcpy(&LogInfo,"FAILED READ",11);
					if (bFileOpen)
						fwrite(&LogInfo,sizeof(LogInfo),1,outfile);
					if (bVerbose) {
						sprintf_s(LogBuffer, "Failed LOGINFO read [%d]: %d\n",iIndex,uiAddress);LOGIT(47735, LogBuffer);
					}
					continue;
				}
			}

			//
			//save the log info block
			//
			UpdateRegistry(szName, "LOGINFO", (BYTE*)&LogInfo, sizeof(LogInfo),uiAddress);
			if (bFileOpen)
				fwrite(&LogInfo,sizeof(LogInfo),1,outfile);

			if (Level < eStatusAndLogAndLastReadDelta) // eStatusAndLog: done for now, move on to next one
			{
				UpdateRegistryLatestForSeal(szName,"No errors occurred in the seal status and log page query",cNow,uiAddress);
				continue;
			}			//
			// get seal log pages
			//
			bool bDone = false;
			unsigned char ucPage = LogInfo.SealLogStartPage;
			//this corrects for an apparent firmware error in the "extraction" point of the ring buffer
			if (LogInfo.SealEvents > (LogInfo.SealLogLastPage - LogInfo.SealLogFirstPage + 1)*SEALEVENTS_PER_PAGE)
			{
				ucPage = LogInfo.SealLogCurrentPage+1;
				if (ucPage > LogInfo.SealLogLastPage)
					ucPage = LogInfo.SealLogFirstPage;
			}


			while (!bDone)
			{
				if (bVerbose) {
					sprintf_s(LogBuffer,"Get Seal Log Page: %d\n",ucPage);LOGIT(47736, LogBuffer);
				}
				if (!Eoss.GetLogPage(&LogPage, ucPage))
				{
					//try a second time to get log page
					if (!Eoss.GetLogPage(&LogPage, ucPage))
					{
						//
						//warn any readers we had a problem by NULLING out the record and 
						//writing FAILED READ in the page
						//
						bAnyLocalErrors = bAnyErrors = true;
						UpdateRegistryLatestForSeal(szName,"Failed Seal Log Page Read",cNow,uiAddress);
						memset(&LogPage,0,sizeof(LogPage));
						memcpy(&LogPage,"FAILED READ",12);
						if (bVerbose) {
							sprintf_s(LogBuffer,"Failed LOGPAGE read [%d]: %d Page:%d\n",iIndex,uiAddress,ucPage);LOGIT(47737, LogBuffer);
						}
					}
				}
				//add the results of the read to the registry
				sprintf_s(szPage,sizeof(szPage),"SEALLOG_%03d",++iLogPageCounter);
				UpdateRegistry(szName, szPage, (BYTE*)&LogPage, sizeof(LogPage),uiAddress);
				//and to the binary file
				if (bFileOpen)
					fwrite(&LogPage,sizeof(LogPage),1,outfile);
				//adjust for wrap,current,and last page, the order is critical
				if (ucPage == LogInfo.SealLogCurrentPage) bDone = true;
				else if (ucPage == LogInfo.SealLogLastPage) ucPage = LogInfo.SealLogFirstPage;
				else ucPage++;
			}

			//
			// get system log pages
			//
			bDone = false;
			ucPage = LogInfo.SystemLogStartPage;
			//this corrects for an apparent firmware error in the "extraction" point of the ring buffer
			if (LogInfo.SystemEvents > (LogInfo.SystemLogLastPage - LogInfo.SystemLogFirstPage + 1)*SYSTEMEVENTS_PER_PAGE)
			{
				ucPage = LogInfo.SystemLogCurrentPage+1;
				if (ucPage > LogInfo.SystemLogLastPage)
					ucPage = LogInfo.SystemLogFirstPage;
			}

			iLogPageCounter = 0;
			while (!bDone)
			{
				if (bVerbose) {
					sprintf_s(LogBuffer,"Get System Log Page: %d\n",ucPage);LOGIT(47738, LogBuffer);
				}
				if (!Eoss.GetLogPage(&LogPage, ucPage))
				{
					//try a second time to get log page
					if (!Eoss.GetLogPage(&LogPage, ucPage))
					{
						//
						//warn any readers we had a problem
						//
						bAnyLocalErrors = bAnyErrors = true;
						UpdateRegistryLatestForSeal(szName,"Failed System Log Page Read",cNow,uiAddress);
						memset(&LogPage,0,sizeof(LogPage));
						memcpy(&LogPage,"FAILED READ",12);
						if (bVerbose) {
							sprintf_s(LogBuffer,"Failed SYSTEMLOGPAGE read [%d]: %d Page:%d\n",iIndex,uiAddress,ucPage);LOGIT(47739, LogBuffer);
						}
					}
				}
				//
				//save the page and update the page index
				//
				sprintf_s(szPage,sizeof(szPage),"SYSTEMLOG_%03d",++iLogPageCounter);
				UpdateRegistry(szName, szPage, (BYTE*)&LogPage, sizeof(LogPage),uiAddress);
				if (bFileOpen)
					fwrite(&LogPage,sizeof(LogPage),1,outfile);
				if (ucPage == LogInfo.SystemLogCurrentPage) bDone = true;
				else if (ucPage == LogInfo.SystemLogLastPage) ucPage = LogInfo.SystemLogFirstPage;
				else ucPage++;
			}

			//
			// get soh log pages
			//
			bDone = false;
			ucPage = LogInfo.SoHLogStartPage;
			//this corrects for an apparent firmware error in the "extraction" point of the ring buffer
			if (LogInfo.SoHRecords > (LogInfo.SoHLogLastPage - LogInfo.SoHLogFirstPage + 1)*SOHRECORDS_PER_PAGE)
			{
				ucPage = LogInfo.SoHLogCurrentPage+1;
				if (ucPage > LogInfo.SoHLogLastPage)
					ucPage = LogInfo.SoHLogFirstPage;
			}

			iLogPageCounter = 0;
			while (!bDone)
			{
				if (bVerbose) {
					sprintf_s(LogBuffer,"Get SOH Log Page: %d\n",ucPage);LOGIT(47740, LogBuffer);
				}
				if (!Eoss.GetLogPage(&LogPage, ucPage))
				{
					if (!Eoss.GetLogPage(&LogPage, ucPage))
					{
						//
						//warn any readers we had a problem
						//
						bAnyLocalErrors = bAnyErrors = true;
						UpdateRegistryLatestForSeal(szName,"Failed SOH Log Page Read",cNow,uiAddress);
						memset(&LogPage,0,sizeof(LogPage));
						memcpy(&LogPage,"FAILED READ",12);
						if (bVerbose) {
							sprintf_s(LogBuffer,"Failed SOHLOGPAGE read [%d]: %d Page:%d\n",iIndex,uiAddress,ucPage);LOGIT(47741, LogBuffer);
						}
					}
				}
				//
				//save the page and update the page index
				//
				sprintf_s(szPage,sizeof(szPage),"SOHLOG_%03d",++iLogPageCounter);
				UpdateRegistry(szName, szPage, (BYTE*)&LogPage, sizeof(LogPage),uiAddress);
				if (bFileOpen)
					fwrite(&LogPage,sizeof(LogPage),1,outfile);
				if (ucPage == LogInfo.SoHLogCurrentPage) bDone = true;
				else if (ucPage == LogInfo.SoHLogLastPage) ucPage = LogInfo.SoHLogFirstPage;
				else ucPage++;
			}

			//
			// log off
			//

			if (bVerbose) {
				sprintf_s(LogBuffer,"Log off\n");LOGIT(47742, LogBuffer);
			}
			Eoss.Logoff();
			if (!bAnyLocalErrors)
				UpdateRegistryLatestForSeal(szName,"No errors",cNow,uiAddress);
		}
		if (bAnyErrors && !bIdentifiedAtLeastOneSeal)
		{
			UpdateRegistryLatest(szName,"Error: No EOSS seals detected, but one or more were expected",cNow,Level,bLogToFile);
		}
		else
		{
			UpdateRegistryLatest(szName,bAnyErrors?"Problems encountered reading one or more seals":"No errors",cNow,Level,bLogToFile);
		}
		if (bFileOpen)
		{
			fclose(outfile);
			bFileOpen = false;
		}
		if (!bLevelFlagOverride)
		{ // increment read counter
			lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
			if (lRet == ERROR_SUCCESS)
			{
				if (ReaderCount < UINT_MAX) ReaderCount++; else ReaderCount = 0;
				lRet = RegSetValueExA(hKey,READ_COUNT, NULL,REG_BINARY,(BYTE*)&ReaderCount,sizeof(ReaderCount));
				sprintf_s(LogBuffer, "Interval reader count incremented %d\n",ReaderCount);LOGIT(47743, LogBuffer);
				RegCloseKey(hKey);
			}
		}

	}
	catch(DWORD Error)
	{
		char szTemp[128];
		if (bVerbose) {
			sprintf_s(LogBuffer,"Error: Serial Port %d -> %d during seal processing\n",iComPort, Error);LOGIT(47744, LogBuffer);
		}
		sprintf_s(szTemp,sizeof(szTemp),"Error: Serial Port %d -> %d during seal processing",iComPort, Error);
		UpdateRegistryLatest(szName,szTemp,cNow,Level,bLogToFile);
	}
	catch (char *s)
	{
		char szTemp[320];
		if (bVerbose) {
			sprintf_s(LogBuffer,"Error: Exception %s during seal processing\n",s);LOGIT(47745, LogBuffer);
		}
		sprintf_s(szTemp,sizeof(szTemp),"Error: Exception %.256s during seal processing",s);
		UpdateRegistryLatest(szName,szTemp,cNow,Level,bLogToFile);
	}
	catch (...)
	{
		if (bVerbose) {
			sprintf_s(LogBuffer,"Error: Exception during seal processing\n");LOGIT(47746, LogBuffer);
		}
		UpdateRegistryLatest(szName,"Error: Exception during seal processing",cNow,Level,bLogToFile);
	}

	if (bLevelFlagOverride)	 // always clear depth flag from MIC COM
	{
		lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,szName,0,KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
		int flag = 0;
		if (lRet == ERROR_SUCCESS)
		{
			lRet = RegSetValueExA(hKey,USE_OVERRIDE_QUERY_DEPTH, NULL,REG_BINARY,(BYTE*)&flag,sizeof(flag));
			RegCloseKey(hKey);
		}
	}
	// JFL May 2009 commented out, dongle->Release() should terminate the server in the absence of other clients, no?
	//KillEOSS();
#ifdef _DEBUG
	if (bVerbose) {
		//Beep(440,334);Beep(440,334);Beep(400,334);
		printf("\n\npress enter...\n\n");
		_getch();
	}
#endif
	
	if (hMutex)
	{
		CloseHandle(hMutex);
		ReleaseMutex(hMutex);
	}

TYRONE_SLOTHROP:
	CoUninitialize();

SAUL_GOODMAN:
	if (bVerbose) {
		sprintf_s(LogBuffer, "MIC_EOSSReader exiting\n\n");LOGIT(47702, LogBuffer);
	}

	if (bVerbose && bDbg) {
		printf("\n\npress enter...\n\n");
		_getch();
	}
	return iReturn;
}
