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
// CullFileDlg.cpp : implementation file
//
//Version 1.8.0.5 to 1.8.0.6 changed number of copy sets from 10 to 20

#include "stdafx.h"
#include "CullFile.h"
#include "CullFileDlg.h"
#include "DirDialog.h"
#include "SetNames.h"
#include "AddItem.h"
#include "dunzdll.h"
#include <WinVer.h>
#include <errno.h>
#include <direct.h>
#include <process.h>

//#include "Version.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__;
#endif

#define RECEIVE_PROGRESS	(WM_USER+1000)
#define MY_SELECTCOMMAND	(WM_APP+1000)						//SCR00504 dgp

bool g_bAbort;
bool g_bPause;													//SCR00351
bool g_bDoLog;
bool g_bDateFromName;
char g_cLogFile[_MAX_PATH];
char g_cIniFile[_MAX_PATH];
char g_szZipPath[_MAX_PATH];
char g_cMicIniFile[_MAX_PATH];

CListCtrl *g_pListDir;
CDateTimeCtrl*	g_pDateTimeFrom;
CDateTimeCtrl*	g_pDateTimeTo;

class MSKVersionInfo
{
public:
    MSKVersionInfo(LPCTSTR pModName = NULL); 
    ~MSKVersionInfo();
     
    CString Comments()        {return GetValue("Comments");} 
    CString CompanyName()     {return GetValue("CompanyName");} 
    CString FileDescription() {return GetValue("FileDescription");} 
    CString FileVersion()     {return GetValue("FileVersion");} 
    CString InternalName()    {return GetValue("InternalName");}
    CString OriginalFilename(){return GetValue("OriginalFilename");} 
    CString ProductName()     {return GetValue("ProductName");} 
    CString ProductVersion()  {return GetValue("ProductVersion");} 
    CString SpecialBuild()    {return GetValue("SpecialBuild");} 
    CString LegalCopyright()  {return GetValue("LegalCopyright");} 
    CString LegalTrademarks() {return GetValue("LegalTrademarks");} 
    CString PrivateBuild()    {return GetValue("PrivateBuild");}
     
    CString GetValue(CString const sSubBlock);
     
private:
    CString     m_sSubBlockHead;
    void *      m_VerData;
     
    MSKVersionInfo(const MSKVersionInfo &);     // private copy ctor (can't copy)
    operator=(const MSKVersionInfo &);      // private op=  (can't assign)
};


MSKVersionInfo::MSKVersionInfo(LPCTSTR lpszModuleName /* = NULL */)
        : m_VerData(NULL)
{
    HMODULE     hModule;
    if (lpszModuleName == NULL) {
        hModule = AfxGetInstanceHandle();
    } else {
        hModule = GetModuleHandle(lpszModuleName);
        if (hModule == NULL)
            AfxThrowUserException();
    }

    char    name[_MAX_PATH];
    GetModuleFileName(hModule, name, sizeof(name));

    DWORD   hInfo;
    DWORD   dwLen;
    dwLen = GetFileVersionInfoSize(name, &hInfo);

    if (dwLen == 0)
        AfxThrowUserException();

    m_VerData = (void *) new char[dwLen];
    if (m_VerData == NULL)
        AfxThrowMemoryException();

    GetFileVersionInfo(name, hInfo,  dwLen, m_VerData);


    struct {
        WORD    first;
        WORD    second;
    } *lpBuffer;
     
    VerQueryValue(m_VerData, "\\VarFileInfo\\Translation",
            (LPVOID*) &lpBuffer,(unsigned int *) &dwLen);
     
    if (dwLen==0)
        AfxThrowUserException();
     
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\", 
                    lpBuffer->first, lpBuffer->second);
    m_sSubBlockHead = name;
}

MSKVersionInfo::~MSKVersionInfo()
{
    delete m_VerData;
}
     
CString MSKVersionInfo::GetValue(CString const sSubBlock)
{
    CString sBlock = m_sSubBlockHead + sSubBlock; 
    CString sRet;
     
    unsigned int    uSize;
    LPCTSTR         lpBuffer;
    BOOL bRet = VerQueryValue(m_VerData, 
                    (char *)((LPCTSTR) sBlock), 
                    (void**)&lpBuffer, &uSize);
     
    if (bRet)
        sRet = lpBuffer;
     
    return sRet;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

int CAboutDlg::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DoModal();
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// **************************************************************************************
//
//  Procedure:  initUNZIPCmdStruct()
//
//  Purpose:  make the inputted UNZIP command structure "plain vanilla"
//
// **************************************************************************************
void initUNZIPCmdStruct(UNZIPCMDSTRUCT FAR *ucs){
  ucs->unzipStructSize = sizeof(UNZIPCMDSTRUCT);
  ucs->function = 0;  // intentionally out of range
  ucs->lpszZIPFile = (LPSTR)NULL ;
  ucs->zInfo = (ZIPINFO FAR *)NULL;
  ucs->lpszFilespec = (LPSTR)NULL ;
  ucs->index = -1;
  ucs->lpszDestination = (LPSTR)NULL ;
  ucs->freshenFlag = (BOOL)FALSE;
  ucs->updateFlag = (BOOL)FALSE;
  ucs->overWriteFlag = (BOOL)TRUE;
  ucs->quietFlag = (BOOL)TRUE;
  ucs->testFlag = (BOOL)FALSE;
  ucs->noDirectoryNamesFlag = (BOOL)TRUE;
  ucs->recurseFlag = (BOOL)FALSE;
  ucs->noDirectoryItemsFlag = (BOOL)TRUE;
  ucs->lpMinorStatus = (FARPROC)NULL;
  ucs->lpMinorUserData = (void FAR *)NULL;
  ucs->lpMajorStatus = (FARPROC)NULL;
  ucs->lpMajorUserData = (void FAR *)NULL;
  ucs->returnCount = 0;
  ucs->lpszReturnString = (LPSTR)NULL;
  ucs->bDiagnostic = FALSE;
  ucs->bLF2CRLFFlag = (BOOL)FALSE;
  ucs->decryptFlag = FALSE;
  ucs->lpszDecryptCode = (LPSTR)NULL;
  // rev 2.00
  ucs->lpMessageDisplay = (FARPROC)NULL;
  ucs->lpMessageDisplayData = (void FAR *)NULL;
  ucs->wUnzipSubOptions = (WORD)0;
  // rev 3.00
  ucs->lResv1 = 0L;
  ucs->lResv2 = 0L;
  ucs->lpRenameProc = (FARPROC)NULL;
  ucs->lpRenameUserData = (void FAR *)NULL;
  ucs->lpszExtProgTitle = (LPSTR)NULL;
  ucs->lpMemBlock = (void FAR *)NULL;
  ucs->lMemBlockSize = 0L;
  ucs->lStartingOffset = 0L;

//  // nothing cancelled yet
//  gbCancel = FALSE;
}

bool GetDateFromLongNameFormat(int *piYearIndex, int *piMonthIndex, int *piDaysIndex)
{
	static bool fDoInitialization = true;
	static int iYearIndex         = -1;
	static int iMonthIndex        = -1;
	static int iDaysIndex         = -1;

	// If need to do initialization
	if (fDoInitialization) {

		// Get long name format (from MIC .ini file)
		char szLongNameFormat[64];
		GetPrivateProfileString("CONFIGURATION", "LONGNAMEFORM", "",
			szLongNameFormat, sizeof(szLongNameFormat), g_cMicIniFile);

		// If long name format unspecified (in MIC .ini file)
		if (strlen(szLongNameFormat) == 0)
			return false;

		int iIndex = 0;

		// For each character in format string
		for (char *s = szLongNameFormat; *s != '\0'; s++) {

			// If beginning of template
			if (*s == '%') {

				// Depending on template character
				switch (*++s) {
				case 'y':
					iYearIndex = iIndex;
					iIndex += 4;
					break;
				case 'm':
					iMonthIndex = iIndex;
					iIndex += 2;
					break;
				case 'd':
					iDaysIndex = iIndex;
					iIndex += 2;
					break;
				case 'T':
				case 'i':
				case 'H':
				case 'M':
				case 'S':
					iIndex += 2;
					break;
				case 'I':
					iIndex += 3;
					break;
				default:
					iIndex += 1;
					break;
				}
			
			// Otherwise, text character
			} else
				iIndex += 1;
		}

		// Initialization complete
		fDoInitialization = false;
	}

	// Check for unspecified date components
	if (iYearIndex  == -1 || iMonthIndex == -1 || iDaysIndex  == -1)
		return false;

	// Return date components
	*piYearIndex  = iYearIndex;
	*piMonthIndex = iMonthIndex;
	*piDaysIndex  = iDaysIndex;
	return true;
}

void WalkDirDel(HWND hWnd, CString CurDir, bool *DoDelete, bool *DoQuestion, bool DoSub, FILE* Log)
{
	//pause this thread
	while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
	if (g_bAbort) 
		return;

	CString WorkingDir = CurDir;
	if (WorkingDir.Right(1) == _T("\\"))
		WorkingDir += "*.*";
	else
		WorkingDir += "\\*.*";
	WIN32_FIND_DATA FindFileData;
	HANDLE Find = FindFirstFile(WorkingDir,&FindFileData);
	CString Message;
	CString FullPath;
	if (Find != INVALID_HANDLE_VALUE)
	{
		BOOL bDone = FALSE;
		while (!bDone)
		{
			while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
			if (g_bAbort) 
			{
				bDone = true;
				break;
			}

			FullPath = CurDir;
			FullPath += "\\";
			FullPath += FindFileData.cFileName;
		
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ((strcmp(FindFileData.cFileName,".")!=0) && 
					(strcmp(FindFileData.cFileName,"..")!=0)
					)
					if (DoSub)
						WalkDirDel(hWnd, FullPath, DoDelete, DoQuestion, DoSub, Log);
				while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
				if (g_bAbort) 
				{
					bDone = true;
					break;
				}
			}
			else
			{
				//pause this thread
				while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
				if (g_bAbort) 
				{
					bDone = true;
					break;
				}

				// do what needs to be done for each file here
				if (*DoQuestion)
				{
					CString Question = CurDir;
					Question += " contains one or more files!\n"
						"Should I delete them and all others in\n"
						"this destination prior to proceeding?";
					if (AfxMessageBox(Question,MB_YESNO | MB_DEFBUTTON2 )==IDYES)
						*DoDelete = true;
					else
						*DoDelete = false;
					*DoQuestion = false;
				}
				if (*DoDelete)
				{
					//delete the file
					if (DeleteFile(FullPath))
						Message.Format("%s\tDELETED",FullPath);
					else
					{
						Message.Format("%s\tFAILED DELETION ATTEMPT",FullPath);
						PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,7); //SCR00504 dgp
					}

					if (Log) 
					{
						fprintf(Log,"%s\n",(LPCSTR)Message);
						fflush(Log);
					}
					char* msg = (char*)malloc(Message.GetLength()+1);
					strcpy(msg,(LPCSTR)Message);
					for (unsigned j= 0; j < strlen(msg); j++)
						if (msg[j] == '\t') msg[j] = '\n';
					PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+100,(long)msg);
				}
				else
					bDone = true;
			}
			if (!bDone)
				bDone = !FindNextFile(Find, &FindFileData);
		}
		FindClose(Find);
	}
}

bool DoCopy(CString,CString);

bool DoCopyFind(WIN32_FIND_DATA *FindFileData,bool bOnlyIfChange)
{
	if (bOnlyIfChange)
		if (FindFileData->dwFileAttributes & ~FILE_ATTRIBUTE_ARCHIVE)
			return false;
	COleDateTime FileTime(FindFileData->ftLastWriteTime);
	return DoCopy(CString(FindFileData->cFileName),FileTime.Format("%m/%d/%Y %H:%M"));
}

bool DoCopy(CString szFileName, CString szFileDateTime)
{
	bool DoCopy;
	COleDateTime file;
	file.ParseDateTime(LPCSTR(szFileDateTime));

	if (g_bDateFromName)
	{
		//iff jpg type file
		CString RightFour(szFileName.Right(4));

		if (RightFour.CompareNoCase(".jpg") == 0)
		{
			COleDateTime Begin=g_pDateTimeFrom->GetOleDateTime();
			COleDateTime End  =g_pDateTimeTo->GetOleDateTime();
				//normalize times to 1990 as jpg is
			int YearBeginTemp = Begin.GetYear();
			while (YearBeginTemp >= 1993)YearBeginTemp -= 3;
			while (YearBeginTemp <  1990)YearBeginTemp += 3;
			Begin = COleDateTime(YearBeginTemp,
				Begin.GetMonth(),
				Begin.GetDay(),
				Begin.GetHour(),
				Begin.GetMinute(),
				Begin.GetSecond());
				int YearEndTemp = End.GetYear();
			while (YearEndTemp >= 1993)YearEndTemp -= 3;
			while (YearEndTemp <  1990)YearEndTemp += 3;
			End = COleDateTime(YearEndTemp,
				End.GetMonth(),
				End.GetDay(),
				End.GetHour(),
				End.GetMinute(),
				End.GetSecond());

			//get the file's date from the name
			int Year;
			int Month; 
			int Days;
			if (szFileName[1] <= '9')
				Year = szFileName[1] - '0';
			else
				Year = toupper(szFileName[1]) - 'A' + 10;

			Month = Year % 12 + 1;
			Year  = (int)(Year/12) + 1990;
			
			if (szFileName[2] <= '9') 
				Days = szFileName[2] - '0' + 1;
			else
				Days = toupper(szFileName[2]) - 'A' + 11;

			int Hours     = toupper(szFileName[3])-'A';

			int Minutes   = (szFileName[4]-'0')*10+
							(szFileName[5]-'0');

			int Seconds   = (szFileName[6]-'0')*10+
							(szFileName[7]-'0');

			COleDateTime FileTime = COleDateTime(Year,Month,Days,Hours,Minutes,Seconds);
			if (FileTime.GetStatus() == COleDateTime::valid)			//SCR00349
			{
				if (Begin <= End)
					//normal
					if ((FileTime >= Begin) && (FileTime < End))
						DoCopy = true;
					else
						DoCopy = false;
				else
					//reversed because we are looking across
					//the end of the ambiguity in the file name
					if ((FileTime >= Begin) || (FileTime < End))
						DoCopy = true;
					else
						DoCopy = false;
			}
			else														//SCR00349
			{															//SCR00349
				//normal												//SCR00349
				if ((file >= Begin) && (file < End))					//SCR00349
					DoCopy = true;										//SCR00349
				else													//SCR00349
					DoCopy = false;										//SCR00349
			}															//SCR00349
		}
		//iff cev,pfm,bin,jsr,isr,bid,dmp,mca
		else if	(
			(RightFour.CompareNoCase(".cev") == 0) ||
			(RightFour.CompareNoCase(".pfm") == 0) ||
			(RightFour.CompareNoCase(".bin") == 0) ||
			(RightFour.CompareNoCase(".isr") == 0) ||
			(RightFour.CompareNoCase(".jsr") == 0) ||
			(RightFour.CompareNoCase(".bid") == 0) ||
			(RightFour.CompareNoCase(".dmp") == 0) ||
			(RightFour.CompareNoCase(".mca") == 0) 
			)
		{
			int iYearIndex, iMonthIndex, iDaysIndex;

			//determine if "short" file name
			if (szFileName.GetLength() == 12)
			{
				//get the file's date from the name
				int Year;
				int Month; 
				int Days;
				if (szFileName[2] <= '9')
					Year = szFileName[2] - '0';
				else
					Year = toupper(szFileName[2]) - 'A' + 10;
				Year = Year + 1990;

				if (szFileName[3] <= '9') 
					Month = szFileName[3] - '0';
				else
					Month = toupper(szFileName[3]) - 'A' + 10;
			
				if (szFileName[4] <= '9') 
					Days = szFileName[4] - '0';
				else
					Days = toupper(szFileName[4]) - 'A' + 10;

//				int Hours     = toupper(szFileName[5])-'A';									//SCR00349
//																							//SCR00349
//				int Minutes   = (szFileName[6]-'0')*10+										//SCR00349
//								(szFileName[7]-'0');										//SCR00349
//																							//SCR00349
//				int Seconds   = 0;															//SCR00349
																						//SCR00349
				if (Year > 1990 && Year < 2028 &&											//SCR00349
					Month > 0   && Month < 13  &&											//SCR00349
					Days  > 0   && Days  < 32  )											//SCR00349
				{																			//SCR00349
					COleDateTime FileTime = COleDateTime(Year,Month,Days,23,59,59);			//SCR00349
																						//SCR00349
					if (FileTime.GetStatus() == COleDateTime::valid)						//SCR00349
					{																		//SCR00349
						if ((FileTime >= g_pDateTimeFrom->GetOleDateTime()) && (FileTime < g_pDateTimeTo->GetOleDateTime()))	//SCR00349
							DoCopy = true;													//SCR00349
						else																//SCR00349
							DoCopy = false;													//SCR00349
					}																		//SCR00349
					else																	//SCR00349
					{																		//SCR00349
						if	(																//SCR00349
							(file >= g_pDateTimeFrom->GetOleDateTime() ) && 				//SCR00349
							(file <  g_pDateTimeTo->GetOleDateTime()) 						//SCR00349
							) 																//SCR00349
							DoCopy = true;													//SCR00349
						else																//SCR00349
							DoCopy = false;													//SCR00349
					}																		//SCR00349
				}																			//SCR00349
				else																		//SCR00349
				{																			//SCR00349
					if	(																	//SCR00349
						(file >= g_pDateTimeFrom->GetOleDateTime() ) && 					//SCR00349
						(file <  g_pDateTimeTo->GetOleDateTime()) 							//SCR00349
						) 																	//SCR00349
						DoCopy = true;														//SCR00349
					else																	//SCR00349
						DoCopy = false;														//SCR00349
				}																			//SCR00349

			}
			//determine if "long" file name and determine date (Y/M/D) offsets
			else if (GetDateFromLongNameFormat(&iYearIndex, &iMonthIndex, &iDaysIndex))
			{
				//extract date from file name
				int iYear  = atoi(szFileName.Mid(iYearIndex,  4));
				int iMonth = atoi(szFileName.Mid(iMonthIndex, 2)); 
				int iDays  = atoi(szFileName.Mid(iDaysIndex,  2));

				//calculate date/time from file name date
				COleDateTime FileTime = COleDateTime(iYear,iMonth,iDays,23,59,59);

				//if file name date is valid
				if (iMonth > 0 && iDays > 0 && FileTime.GetStatus() == COleDateTime::valid)
				{
					//if file name date is in specified range 
					if (FileTime >= g_pDateTimeFrom->GetOleDateTime() &&
					    FileTime < g_pDateTimeTo->GetOleDateTime())
						DoCopy = true;
					else
						DoCopy = false;
				}
				else
				{
					//if file modification date is in specified range 
					if (file >= g_pDateTimeFrom->GetOleDateTime() &&
					    file <  g_pDateTimeTo->GetOleDateTime())
						DoCopy = true;
					else
						DoCopy = false;
				}
			}
			//otherwise ...
			else
			{
				//if file modification date is in specified range 
				if (file >= g_pDateTimeFrom->GetOleDateTime() &&
				    file <  g_pDateTimeTo->GetOleDateTime())
					DoCopy = true;
				else
					DoCopy = false;
			}	
		}
		//following else if added to
		else if (RightFour.CompareNoCase(".zip") == 0)
		{
			int Year;
			int Month;
			int Days;
			int Hours = 0;
			int Minutes = 0;
			int Seconds = 0;
			if (sscanf(szFileName,"%4d_%2d_%2d",&Year,&Month,&Days)==3)
			{
				COleDateTime FileTime = COleDateTime(Year,Month,Days,Hours,Minutes,Seconds);
				if (FileTime.GetStatus() == COleDateTime::valid)
				{
					if ((FileTime >= g_pDateTimeFrom->GetOleDateTime()) && (FileTime < g_pDateTimeTo->GetOleDateTime()))
						DoCopy = true;
					else
						DoCopy = false;
				}
				else
				{
					if	(
						(file >= g_pDateTimeFrom->GetOleDateTime() ) && 
						(file <  g_pDateTimeTo->GetOleDateTime()) 
						) 
						DoCopy = true;
					else
						DoCopy = false;
				}
			}
		}

		//all others use normal file date
		else
		{
			if	(
				(file >= g_pDateTimeFrom->GetOleDateTime() ) && 
				(file <  g_pDateTimeTo->GetOleDateTime()) 
				) 
				DoCopy = true;
			else
				DoCopy = false;
		}
	}
	else
	{
		if	(
			(file >= g_pDateTimeFrom->GetOleDateTime()) && 
			(file <  g_pDateTimeTo->GetOleDateTime()) 
			) 
			DoCopy = true;
		else
			DoCopy = false;
	}
	return DoCopy;
}

bool check(char *sString, char *sPattern)
{
	int i,j;
	bool result = true;
	int lengthPat = strlen(sPattern);
	int lengthStr = strlen(sString);
	if (lengthStr == 0)
		return false;
	for (i = 0, j = 0; i < lengthPat; i++)
	{
		if (j >= lengthStr)
			return false;
		switch(sPattern[i])
		{
		case '*':
			return result;
			break;
		case '?':
			j++;
			break;
		default:
			if  (
				(toupper(sPattern[i]) != toupper(sString[j]))
				)
				return false;
			j++;
		}
	}
	return result;
}


bool DoCopyType(char *cFileName, const char *FTypes)
{
	bool name = false;
	bool ext  = false;
	bool done = false;
	char cDrive[_MAX_DRIVE];
	char cPath[_MAX_DIR];
	char cName[_MAX_FNAME];
	char cExt[_MAX_EXT];
	char sTypes[_MAX_PATH];
	char sDrive[_MAX_DRIVE];
	char sPath[_MAX_DIR];
	char sName[_MAX_FNAME];
	char sExt[_MAX_EXT];
	strcpy(sTypes,FTypes);
	_splitpath(cFileName,cDrive,cPath,cName,cExt);
	char *at;
	if (strlen(sTypes)==0)
		return false;
	at = strtok(sTypes," /0,");
	while ((at) && !done)
	{
		_splitpath(at,sDrive,sPath,sName,sExt);
		//check name
		if (strlen(sName))
			name = check(cName,sName);
		else
			name = false;
		//check ext
		if (strlen(sExt))
			ext = check(cExt,sExt);
		else
			ext = false;
		if (name && ext)
			done = true;
		at = strtok(NULL," /0,");
	}
		
	return (name && ext)?true:false;
}

int BuildAccess(const char *szDestinationDir)
{
	int ret = 0;
	if(_access(szDestinationDir,00)==-1)
	{
		switch (errno)
		{
		case ENOENT:
			char szTempDestDir[_MAX_PATH];
			char szDrive[_MAX_DRIVE];
			char szDir[_MAX_DIR];
			char szFname[_MAX_FNAME];
			char szExt[_MAX_EXT];
			_splitpath(szDestinationDir,szDrive,szDir,szFname,szExt);
			if (szDir[strlen(szDir)-1]== '\\')
				szDir[strlen(szDir)-1] = NULL;
			strcpy(szTempDestDir,szDrive);
			strcat(szTempDestDir,szDir);
			if ((ret = BuildAccess(szTempDestDir))==0)
				if (_mkdir(szDestinationDir) == -1) 
					if (errno != EEXIST) ret = -2;
			break;
		case EACCES:
			ret = -1;
			break;
		}
	}
	return ret;
}

bool ArchiveBitOn(char* pathname, bool reset)
{
	DWORD result = GetFileAttributes(pathname);
	if (result != 0xffffffff) 
		if (result & FILE_ATTRIBUTE_ARCHIVE)
		{
			//reset the archive bit
			if (reset) 
				//ignore fail (return of 0), micxfer will just
				//transfer the file again
				SetFileAttributes(pathname, result & ~FILE_ATTRIBUTE_ARCHIVE);
			return true;
		}
	return false;
}

void WalkDirCpy(HWND hWnd, CString Source, CString Destin, CString FTypes, 
				bool Recurse, long &FileCount, long &FileCopied, FILE*log, bool bOnlyIfChange)
{

	WIN32_FIND_DATA FindFileData;
	CString Message;
	while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
	if (g_bAbort) return;

	CString FullSourcePath(Source);
	CString FullDestinPath(Destin);
	if (FullSourcePath.Right(1) != '\\')
		FullSourcePath += '\\';
	if (FullDestinPath.Right(1) != '\\')
		FullDestinPath += '\\';
	CString FullSource(FullSourcePath);
	CString FullDestin(FullDestinPath);
	CString FullSourceTemp;
	CString FullDestinTemp;

	FullSourcePath += "*.*";

	HANDLE find = FindFirstFile(FullSourcePath,&FindFileData);
	if (find != INVALID_HANDLE_VALUE)
	{
		BOOL Ok = true;
		bool doCount = true;
		while (Ok)
		{
			while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
			if (g_bAbort)
			{
				Ok = false;
				break;
			}
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (Recurse)
				{
					if  (
						(strcmp(FindFileData.cFileName,".")!=0) &&
						(strcmp(FindFileData.cFileName,"..")!=0)
						)
						WalkDirCpy(hWnd, FullSource + FindFileData.cFileName,
							FullDestin + FindFileData.cFileName, FTypes, Recurse, FileCount, FileCopied, log, bOnlyIfChange);
				}
			}
			else
			{
				FileCount++;
				if (DoCopyFind(&FindFileData, bOnlyIfChange) && DoCopyType(FindFileData.cFileName,LPCSTR(FTypes)))
				{
					FullSourceTemp = FullSource + FindFileData.cFileName;
					FullDestinTemp = FullDestin + FindFileData.cFileName;
					Message.Format("Copy: %s to \t%s ",
						FullSourceTemp,FullDestinTemp);

					if (BuildAccess(LPCSTR(FullDestin))==0)
					{
						if ( _access(FullSourceTemp,4) == -1)							//SCR00504 dgp
						{																//SCR00504 dgp
							if (errno == EACCES)										//SCR00504 dgp
							{															//SCR00504 dgp
								Message += "SOURCE ACCESS DENIED";						//SCR00504 dgp
								PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,0);	//SCR00504 dgp
							}															//SCR00504 dgp
							else if (errno == ENOENT)									//SCR00504 dgp
							{															//SCR00504 dgp
								Message += "SOURCE PATH NOT FOUND";						//SCR00504 dgp
								PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,1);	//SCR00504 dgp
							}															//SCR00504 dgp
						}																//SCR00504 dgp
						else															//SCR00504 dgp
						{
							if (CopyFile(FullSourceTemp,FullDestinTemp,FALSE))
							{
								if (bOnlyIfChange)
									ArchiveBitOn((LPSTR)LPCSTR(FullSourceTemp),true);	
								Message += "\tCOPY GOOD";
								FileCopied++;
								PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+105,FileCopied);
							}
							else
							{
								Message += "\tCOPY FAILED";
								PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,2);	//SCR00504 dgp
							}
						}
					}
					else
					{
						Message += "\tACCESS DESTINATION FAILURE";
						PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,3);			//SCR00504 dgp
					}

					char* msg = (char*)malloc(Message.GetLength()+1);
					strcpy(msg,(LPCSTR)Message);
					for (unsigned j= 0; j < strlen(msg); j++)
						if (msg[j] == '\t') msg[j] = '\n';

					PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+100,(long)msg);
					if (log) 
						fprintf(log,"%s\n",(LPCSTR)Message);
				}
			}
			PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+104,FileCount);
			Ok = FindNextFile(find, &FindFileData);
		}
		FindClose(find);
	}
	else
	{
		if (log)
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			fprintf(log,"Error source %s:%s\n",FullSourcePath,lpMsgBuf);
			fflush(log);
			LocalFree(lpMsgBuf);
		}

		PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,7);	//SCR00504 dgp
	}
}


void WalkDirZip(HWND hWnd, CString Source, CString Destin, CString FTypes, 
				bool Recurse, bool Bulk, long &FileCount, long &FileCopied, FILE* log, bool bOnlyIfChange)
{
	//now look in the appropriate zip files
	WIN32_FIND_DATA FindFileData;
	CString Message;
	CString MessageDoing;
	while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
	if (g_bAbort)
		return;
	CString FullSourcePath(Source);
	CString FullDestinPath(Destin);
	if (FullSourcePath.Right(1) != '\\')
		FullSourcePath += '\\';
	if (FullDestinPath.Right(1) != '\\')
		FullDestinPath += '\\';
	CString FullSource(FullSourcePath);
	CString FullDestin(FullDestinPath);
	CString FullSourceTemp;
	CString FullDestinTemp;
	char szCurrentDirectory[_MAX_PATH];
//	int i;
	UNZIPCMDSTRUCT ucs;
	ZIPINFO zInfo;

	COleDateTime Begin = g_pDateTimeFrom->GetOleDateTime();
	COleDateTime BeginGate(Begin.GetYear(),Begin.GetMonth(),Begin.GetDay(),0,0,0);
	COleDateTime End = g_pDateTimeTo->GetOleDateTime();
	COleDateTime EndGate(End.GetYear(),End.GetMonth(),End.GetDay(),0,0,0);

	FullSourcePath += "*.*";

	HANDLE find = FindFirstFile(FullSourcePath,&FindFileData);
	//find zip files in current dir
	if (find != INVALID_HANDLE_VALUE)
	{
		BOOL Ok = TRUE;
		while (Ok)
		{
			while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
			if (g_bAbort) 
			{
				Ok = false;
				break;
			}

			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (Recurse)
				{
					if  (
						(strcmp(FindFileData.cFileName,".")!=0) &&
						(strcmp(FindFileData.cFileName,"..")!=0)
						)
						WalkDirZip(hWnd, FullSource + FindFileData.cFileName,
							FullDestin + FindFileData.cFileName, FTypes, 
							Recurse, Bulk, FileCount, FileCopied, log, bOnlyIfChange);
				}
			}
			else
			{
				//for each zip
				if (!bOnlyIfChange || (bOnlyIfChange && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)))
				{
					if (DoCopyType(FindFileData.cFileName, "*.ZIP"))
					{
						while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
						if (g_bAbort) 
						{
							Ok = false;
							break;
						}
						//if zip name in y/m/d portion of region
						int Year, Month, Day;
						sscanf(FindFileData.cFileName,"%4d_%2d_%2d",&Year,&Month,&Day);
						COleDateTime ZipDate(Year,Month,Day,0,0,0);				
						if (ZipDate.GetStatus() != COleDateTime::valid)				//SCR00349
						{															//SCR00349
							//use last written if can't crack date from the name	//SCR00349
							COleDateTime FileTime(FindFileData.ftLastWriteTime);	//SCR00349
							ZipDate = FileTime;										//SCR00349
						}															//SCR00349

						if (ZipDate >= BeginGate && ZipDate <= EndGate)
						{
							if (Bulk)
							{
								//build full path to zipfile
								FullSourceTemp = FullSource + FindFileData.cFileName;
								//build full path to destination
								FullDestinTemp = FullDestin + FindFileData.cFileName;
								Message.Format("Copied: %s to \t%s ",
									FullSourceTemp,FullDestinTemp);
								MessageDoing.Format("Copying: %s to %s",
									FullSourceTemp,FullDestinTemp);
								//copy zip file to destination directory
								if (BuildAccess(LPCSTR(FullDestin))==0)
								{

									char* msg = (char*)malloc(MessageDoing.GetLength()+1);
									strcpy(msg,(LPCSTR)MessageDoing);
									PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+100,(long)msg);
									if (CopyFile(FullSourceTemp,FullDestinTemp,FALSE))
									{
										if (bOnlyIfChange)
											ArchiveBitOn((LPSTR)LPCSTR(FullSourceTemp),true);	
										Message += "\tCOPY GOOD";
									}
									else
									{
										Message += "\tCOPY FAILED";
										PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,4);
										Ok = false;
									}
								}
								else
								{
									Message += "\tACCESS DESTINATION FAILURE";
									PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,5);
									Ok = false;
								}
								while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
								if (g_bAbort) 
								{
									Ok = false;
									break;
								}
								if (Ok)
								{
									int result = 0;
									//spawn pkzip25 and unpack all -- don't wait
									//get current directory
									GetCurrentDirectory(_MAX_PATH,szCurrentDirectory);
									//change direectory to where you want files to be
									if (SetCurrentDirectory(FullDestin))
									{
										MessageDoing.Format("Un-Zipping: %s",
											FullDestinTemp);

										char* msg = (char*)malloc(MessageDoing.GetLength()+1);
										strcpy(msg,(LPCSTR)MessageDoing);
										PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+100,(long)msg);

										CString cTemp1("\"");
										CString cTemp2("\"");
										cTemp1 += g_szZipPath;
										cTemp1 += "\"";
										cTemp2 += FullDestinTemp;
										cTemp2 += "\"";
										result = spawnl(_P_NOWAIT,g_szZipPath,cTemp1,
											"-over=all","-extract",
											LPCSTR(cTemp2),NULL);
	//									result = spawnl(_P_NOWAIT,g_szZipPath,g_szZipPath,
	//										"-over=all","-extract",
	//										LPCSTR(FullDestinTemp),NULL);
										if (result == 0)
										{
											Message += "Un-zip FAIL.";
											PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,6);	//SCR00504 dgp
										}
										else
										{
	//										Sleep(1000);
	//										BringWindowToTop(hWnd); 
	//										::SetActiveWindow(hWnd);
	//										::SetForegroundWindow(hWnd);
										}
									}
									else
									{
										Message += "Didn't Un-zip. CD FAIL.";
										PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+106,7);	//SCR00504 dgp
										Ok = false;
									}
									//restore current directory
									SetCurrentDirectory(szCurrentDirectory);
									//count number of files
									if (Ok)
									{
										//initialize unzip
										initUNZIPCmdStruct(&ucs);
			
										char Temp[_MAX_PATH];
										strcpy(Temp,Source);
										strcat(Temp,"\\");
										strcat(Temp,FindFileData.cFileName);
				
										ucs.lpszZIPFile = (LPSTR)Temp;
										ucs.function = UNZIP_COUNTNAMEDZIPMEMBERS;
										ucs.lpszFilespec = "*.*";
										//count "named" types in zip
										if (dunzip((UNZIPCMDSTRUCT FAR *)&ucs) == UE_OK)
										{
											//bump indicator and tell parent
											FileCount += ucs.returnCount;
											FileCopied  += ucs.returnCount;
											PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+104,FileCount);
											PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+105,FileCopied);
										}
		
										//wait on pkzip25 to finish
										WaitForSingleObject((HANDLE)result,INFINITE);
									}
								}
								//tell the world
								char* msg = (char*)malloc(Message.GetLength()+1);
								strcpy(msg,(LPCSTR)Message);
								for (unsigned j= 0; j < strlen(msg); j++)
									if (msg[j] == '\t') msg[j] = '\n';
								PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+100,(long)msg);
								if (log) 
								{
									fprintf(log,"%s\n",(LPCSTR)Message);
									fflush(log);
								}
							}
							else
							{
 								//create a list like zip needs
								CString FileSpecList("\"");
								for (int j = 0; j < FTypes.GetLength(); j++)
								{
									if (FTypes[j] == ',')
										FileSpecList += "\" \"";
									else
										FileSpecList += FTypes[j];
								}
								FileSpecList += "\"";
		
								//initialize unzip
								initUNZIPCmdStruct(&ucs);
		
								char Temp[_MAX_PATH];
								strcpy(Temp,Source);
								strcat(Temp,"\\");
								strcat(Temp,FindFileData.cFileName);
		
								ucs.lpszZIPFile = (LPSTR)Temp;
								ucs.function = UNZIP_COUNTNAMEDZIPMEMBERS;
								ucs.lpszFilespec = (LPSTR)(LPCSTR)FileSpecList;
								//count "named" types in zip
								if (dunzip((UNZIPCMDSTRUCT FAR *)&ucs) != UE_OK)
								{
									Ok = false;
									break;
								}
								else
								{
									//for each match
									FileCount += ucs.returnCount;
									PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+104,FileCount);
		
									ucs.lpszDestination = (LPSTR)(LPCSTR)Destin;
									for (int i = 0; i < ucs.returnCount; i++) 
									{
										while (g_bPause && !g_bAbort) Sleep(300);	//SCR00351 
										if (g_bAbort) 
										{
											Ok = false;
											break;
										}
										//get name
										ucs.function = UNZIP_GETNEXTNAMEDZIPINFO;
										ucs.lpszZIPFile = (LPSTR)Temp;
										ucs.zInfo = &zInfo;
										ucs.lpszFilespec = (LPSTR)(LPCSTR)FileSpecList;
										ucs.index = i;
										if (dunzip((UNZIPCMDSTRUCT FAR *)&ucs) != UE_OK)
										{
											Ok = false;
											break;
										}
										else
										{
											if (DoCopy(CString(zInfo.szFileName),zInfo.szDateTime))
											{
												ucs.function = UNZIP_EXTRACT;
												ucs.lpszFilespec = NULL;
												Message.Format("Copy: %s:%s to \t%s\\%s ",
												Temp,zInfo.szFileName,Destin,zInfo.szFileName);
												if (dunzip((UNZIPCMDSTRUCT FAR *)&ucs) != UE_OK)
												{
													//log error unzipping
													Message += "\tCOPY FAIL";
												}
												else
												{
													//log good unzip
													Message += "\tCOPY GOOD";
													FileCopied++;
												}	
												PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+105,FileCopied);
		
												char* msg = (char*)malloc(Message.GetLength()+1);
												strcpy(msg,(LPCSTR)Message);
												for (unsigned j= 0; j < strlen(msg); j++)
													if (msg[j] == '\t') msg[j] = '\n';
												PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+100,(long)msg);
												if (log) 
												{
													fprintf(log,"%s\n",(LPCSTR)Message);
													fflush(log);
												}
											}
										}
									}
								}
							}
						}
					}
					else
						Ok = TRUE;
				}
				else 
					Ok = TRUE;
			}
			if (Ok)
				Ok = FindNextFile(find, &FindFileData);
		}
		FindClose(find);
	}
}

///////////////////////////////////////////////////////////////////////////////////
//worker thread
UINT ThreadFunc(LPVOID pParam)
{
	HWND hWnd = (HWND)pParam;

	char Source[_MAX_PATH];
	char Destin[_MAX_PATH];
	char Ftypes[_MAX_PATH];
	char Recurs[4];
	char OnlyIfChange[4];
	char Bulk[4];
	CString Message;
	FILE* log;
	PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,WM_USER+103,0);

	if (g_bDoLog)
		log = fopen(g_cLogFile,"at");
	else
		log = NULL;

	bool fail = false;
	bool done = false;

	long FileCount = 0;
	long FileCopied = 0;
	bool DoDelete = false;										//SCR00325
	bool DoQuestion = true;										//SCR00325
	for (int i = 0; i < g_pListDir->GetItemCount(); i++)
	{
		while (g_bPause && !g_bAbort) Sleep(300);				//SCR00351 
		if (g_bAbort) 											//SCR00351 
			break;												//SCR00351 
		g_pListDir->GetItemText( i, 0, Source, sizeof(Source));
		g_pListDir->GetItemText( i, 1, Recurs, sizeof(Recurs));
		g_pListDir->GetItemText( i, 2, OnlyIfChange, sizeof(OnlyIfChange));
		g_pListDir->GetItemText( i, 3, Bulk,   sizeof(Bulk));
		g_pListDir->GetItemText( i, 4, Destin, sizeof(Destin));
		g_pListDir->GetItemText( i, 5, Ftypes, sizeof(Ftypes));

		//walk the destination tree and see if there
		//is stuff there that needs to be deleted
		//ask the user on the first pass only
		WalkDirDel(hWnd, CString(Destin), &DoDelete, &DoQuestion, 
			(toupper(Recurs[0])=='Y')?true:false/*dosub*/, log);
	}

	//added seperate from the previous so that if the destination is the same
	//for two in the same set and if the DoDelete is true then execution of the
	//second move wont clean out the results of the previous
	for (i = 0; i < g_pListDir->GetItemCount(); i++)								//SCR00325
	{																					//SCR00325
		while (g_bPause && !g_bAbort) Sleep(300);				//SCR00351 				//SCR00325
		if (g_bAbort) 											//SCR00351 				//SCR00325
			break;												//SCR00351 				//SCR00325
		g_pListDir->GetItemText( i, 0, Source, sizeof(Source));							//SCR00325
		g_pListDir->GetItemText( i, 1, Recurs, sizeof(Recurs));							//SCR00325
		g_pListDir->GetItemText( i, 2, OnlyIfChange, sizeof(OnlyIfChange));				//SCR00325
		g_pListDir->GetItemText( i, 3, Bulk,   sizeof(Bulk));							//SCR00325
		g_pListDir->GetItemText( i, 4, Destin, sizeof(Destin));							//SCR00325
		g_pListDir->GetItemText( i, 5, Ftypes, sizeof(Ftypes));							//SCR00325

		WalkDirCpy(hWnd, CString(Source), CString(Destin), CString(Ftypes), 			//SCR00325
			(toupper(Recurs[0])=='Y')?true:false, FileCount, FileCopied, log,			//SCR00325
			(toupper(OnlyIfChange[0])=='Y')?true:false);								//SCR00325
		WalkDirZip(hWnd, CString(Source), CString(Destin), CString(Ftypes),				//SCR00325
			(toupper(Recurs[0])=='Y')?true:false, 										//SCR00325
			(toupper(Bulk[0])=='Y')?true:false, 										//SCR00325
			FileCount, FileCopied, log, (toupper(OnlyIfChange[0])=='Y')?true:false );	//SCR00325
	}



	if (log)
	{
		if (g_bAbort)
		{
			fprintf(log,"Copy aborted by user.\n");
			fflush(log);
		}

		fclose(log);
	}

	PostMessage(hWnd,(UINT)RECEIVE_PROGRESS,g_bAbort?WM_USER+102:WM_USER+101,0);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CCullFileDlg dialog

CCullFileDlg::CCullFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCullFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCullFileDlg)
	m_bDoLog = FALSE;
	m_bDateFromName = TRUE;
	m_szList = _T("");
	m_szSelectionSet = _T("");
	m_uiSpan = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hAbortIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	g_cIniFile[0] = NULL;
	m_hThread = NULL;
	m_bHaveError = false;												//SCR00504 dgp
}

void CCullFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCullFileDlg)
	DDX_Control(pDX, IDC_BUTTON_EDITDIR, m_pEditDir);
	DDX_Control(pDX, IDC_COMBO1, m_pSelectionSet);
	DDX_Control(pDX, IDC_BUTTON_MONTHPLUS, m_pMonthPlusButton);
	DDX_Control(pDX, IDC_BUTTON_MONTHMINUS, m_pMonthMinusButton);
	DDX_Control(pDX, IDC_BUTTON_ADDDIR, m_pAddDirButton);
	DDX_Control(pDX, IDC_BUTTON_DOCOPY, m_pDoCopyButton);
	DDX_Control(pDX, IDC_BUTTON_ABORT, m_pAbortButton);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, m_pPauseButton);					//SCR00351
	DDX_Control(pDX, IDC_CHECK_DATEFROMNAME, m_pDateFromName);
	DDX_Control(pDX, IDC_CHECK_DOLOG, m_pDoLog);
	DDX_Control(pDX, IDC_LIST_DIRECTORIES, m_pListDir);
	DDX_Control(pDX, IDC_BUTTON_REMDIR, m_pRemDir);
	DDX_Check(pDX, IDC_CHECK_DOLOG, m_bDoLog);
	DDX_Check(pDX, IDC_CHECK_DATEFROMNAME, m_bDateFromName);
	DDX_CBString(pDX, IDC_COMBO1, m_szSelectionSet);
	DDX_Text(pDX, IDC_EDIT_SPAN, m_uiSpan);
	DDV_MinMaxUInt(pDX, m_uiSpan, 1, 120);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCullFileDlg, CDialog)
	//{{AFX_MSG_MAP(CCullFileDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADDDIR, OnButtonAdddir)
	ON_BN_CLICKED(IDC_BUTTON_REMDIR, OnButtonRemdir)
	ON_BN_CLICKED(IDC_BUTTON_DOCOPY, OnButtonDocopy)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DIRECTORIES, OnItemchangedListDirectories)
	ON_BN_CLICKED(IDC_BUTTON_MONTHPLUS, OnButtonMonthplus)
	ON_BN_CLICKED(IDC_BUTTON_MONTHMINUS, OnButtonMonthminus)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_CHECK_DATEFROMNAME, OnCheckDatefromname)
	ON_BN_CLICKED(IDC_BUTTON_ABORT, OnButtonAbort)
	ON_BN_CLICKED(IDC_BUTTON_ABORT, OnButtonPause)						//SCR00351
	ON_MESSAGE(RECEIVE_PROGRESS,OnProgressMsg)
	ON_BN_CLICKED(IDC_CHECK_DOLOG, OnCheckDolog)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelectionChange)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_EDITDIR, OnButtonEditdir)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_BUTTON_ZIPPATH, OnButtonZippath)
	ON_EN_CHANGE(IDC_EDIT_SPAN, OnChangeEditSpan)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	//}}AFX_MSG_MAP
	ON_MESSAGE(MY_SELECTCOMMAND,OnSelectCommand)					//SCR00504 dgp
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCullFileDlg message handlers
void CCullFileDlg::OnSelectCommand(WPARAM WParam, LPARAM LParam)	//SCR00504 dgp
{																	//SCR00504 dgp
	int nCount = m_pSelectionSet.GetCount();						//SCR00504 dgp
	if (nCount >= (int)WParam)										//SCR00504 dgp++
	{																//SCR00504 dgp
	   m_pSelectionSet.SetCurSel(WParam-1);							//SCR00504 dgp
	   OnSelectionChange();											//SCR00504 dgp
	}																//SCR00504 dgp
}																	//SCR00504 dgp

void CCullFileDlg::OnProgressMsg(WPARAM WParam, LPARAM LParam)
{
//	char *msg =(char*)LParam;
	switch (WParam)
	{
	case WM_USER+100://file copy message
		SetDlgItemText(IDC_STATIC_NAME,(char*)LParam);
		delete [] (char*)LParam;
		break;

	case WM_USER+101://done message
		SetDlgItemText(IDC_STATIC_NAME,m_bHaveError?"Finished with errors":"Finished");
		EndWaitCursor();
		if(m_dSelectedItem>0)
		{
			m_pRemDir.EnableWindow(true);
			m_pEditDir.EnableWindow(true);
		}
		m_pAddDirButton.EnableWindow(true);
		m_pDoCopyButton.EnableWindow(true);
		m_pAbortButton.EnableWindow(false);
		m_pPauseButton.EnableWindow(false);						//SCR00351
		m_pPauseButton.SetWindowText("Pause Copy");				//SCR00351
		m_pMonthPlusButton.EnableWindow(true);
		m_pMonthMinusButton.EnableWindow(true);
		m_DateTimeFrom.EnableWindow(true);
		m_DateTimeTo.EnableWindow(true);
		m_pDateFromName.EnableWindow(true);
		m_pSelectionSet.EnableWindow(true);
		m_pDoLog.EnableWindow(true);
		m_hThread = NULL;
		g_bPause = false;										//SCR00351
		break;

	case WM_USER+102://user abort message
		g_bAbort = false;
		g_bPause = false;										//SCR00351
		SetDlgItemText(IDC_STATIC_NAME,"User aborted copy");
		if(m_dSelectedItem>0)
		{
			m_pRemDir.EnableWindow(true);
			m_pEditDir.EnableWindow(true);
		}
		m_pAddDirButton.EnableWindow(true);
		m_pDoCopyButton.EnableWindow(true);
		m_pAbortButton.EnableWindow(false);
		m_pPauseButton.EnableWindow(false);						//SCR00351
		m_pPauseButton.SetWindowText("Pause Copy");				//SCR00351
		m_pMonthPlusButton.EnableWindow(true);
		m_pMonthMinusButton.EnableWindow(true);
		m_DateTimeFrom.EnableWindow(true);
		m_DateTimeTo.EnableWindow(true);
		m_pDateFromName.EnableWindow(true);
		m_pSelectionSet.EnableWindow(true);
		m_pDoLog.EnableWindow(true);
		m_hThread = NULL;
		break;

	case WM_USER+103://thread started so don't allow stuff
		SetDlgItemText(IDC_STATIC_NAME,"WORKING...\nIf there are a large number of files it may take a moment before the copy begins.");
		SetDlgItemInt(IDC_STATIC_LOOKEDAT,0);
		SetDlgItemInt(IDC_STATIC_COPIED,0);
		m_pRemDir.EnableWindow(false);
		m_pEditDir.EnableWindow(false);
		m_pAddDirButton.EnableWindow(false);
		m_pDoCopyButton.EnableWindow(false);
		m_pMonthPlusButton.EnableWindow(false);
		m_pMonthMinusButton.EnableWindow(false);
		m_DateTimeFrom.EnableWindow(false);
		m_DateTimeTo.EnableWindow(false);
		m_pDateFromName.EnableWindow(false);
		m_pSelectionSet.EnableWindow(false);
		m_pDoLog.EnableWindow(false);
		m_bHaveError = false;								//SCR00504 dgp
		break;

	case WM_USER+104://
		SetDlgItemInt(IDC_STATIC_LOOKEDAT,LParam);
		break;
	case WM_USER+105:
		SetDlgItemInt(IDC_STATIC_COPIED,LParam);
		break;
	case WM_USER+106:										//SCR00504 dgp
		m_bHaveError = true;								//SCR00504 dgp
		break;												//SCR00504 dgp
	}
}

BOOL CCullFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	MSKVersionInfo VersionInfo;
	CString Name    = VersionInfo.InternalName();
	CString Version = VersionInfo.FileVersion();
	Version.Format("%c.%c.%c.%c",Version[0],Version[3],Version[6],Version[9]);
	Name = Name + " - ";
	Name += Version;

	SetWindowText((LPCSTR)Name);
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_dSelectedItem = -1;

	//build the ini file name
	//get full path and name for this instance of application
	GetModuleFileName(AfxGetInstanceHandle(),g_cIniFile,sizeof(g_cIniFile));
	if (m_cCommandLine[0] != NULL)
	{
		char cDrive[_MAX_DRIVE];
		char cPath[_MAX_PATH];
		char cName[_MAX_FNAME];
		char cExt[_MAX_EXT];
		_splitpath(g_cIniFile,cDrive,cPath,cName,cExt);
		for (char *p = m_cCommandLine; p < m_cCommandLine + strlen(m_cCommandLine);p++)
			if (islower(*p)) p[0] = _toupper(*p);
		if (strstr(m_cCommandLine,".INI"))
		{
			_makepath(g_cIniFile,cDrive,cPath,m_cCommandLine,NULL);
			_makepath(g_cLogFile,cDrive,cPath,m_cCommandLine,NULL);
			char *t = strstr(g_cLogFile,".INI");
			*t = NULL;
			strcat(g_cLogFile,".LOG");
		}
		else
		{
			_makepath(g_cIniFile,cDrive,cPath,m_cCommandLine,".INI");
			_makepath(g_cLogFile,cDrive,cPath,m_cCommandLine,".LOG");
		}
	} 
	else
	{
		char *c = strrchr(g_cIniFile,'.');//find the . in the .exe
		*c = '\0';						//make it null
		strcpy(g_cLogFile,g_cIniFile);
		strcat(g_cLogFile,".LOG");
		strcat(g_cIniFile,".INI");
	}

	//build MIC ini file name
	{
		char cDrive[_MAX_DRIVE];
		char cPath[_MAX_PATH];
		char cName[_MAX_FNAME];
		char cExt[_MAX_EXT];

		_splitpath(g_cIniFile,cDrive,cPath,cName,cExt);
		_makepath(g_cMicIniFile,cDrive,cPath,"MIC","INI");
	}

	//if the ini file isn't there create a "default" one
	if (access(g_cIniFile,0)!=0)
	{
		FILE* temp;
		if ((temp = fopen(g_cIniFile,"wt"))!=NULL)
		{
			fprintf(temp,"[CONFIGURATION]\n");
			fprintf(temp,"ZIPPATH=C:\\MIC\\PKZIP25.EXE\n");
			fprintf(temp,"ATX=0\n");
			fprintf(temp,"ATY=0\n");
			fprintf(temp,"LOGDEFAULT=1\n");
			fprintf(temp,"LOG_NAMELOC=\n");
			fprintf(temp,"TIMESPANMONTHS=1\n");
			fprintf(temp,"DATEFROMNAME=1\n\n");

			fprintf(temp,"[MOVES]\n");
			fprintf(temp,"NAME=Select Set #1\n\n");
			fprintf(temp,"[MOVES2]\n");
			fprintf(temp,"NAME=Select Set #2\n\n");
			fprintf(temp,"[MOVES3]\n");
			fprintf(temp,"NAME=Select Set #3\n\n");
			fprintf(temp,"[MOVES4]\n");
			fprintf(temp,"NAME=Select Set #4\n\n");
			fprintf(temp,"[MOVES5]\n");
			fprintf(temp,"NAME=Select Set #5\n\n");
			fprintf(temp,"[MOVES6]\n");
			fprintf(temp,"NAME=Select Set #6\n\n");
			fprintf(temp,"[MOVES7]\n");
			fprintf(temp,"NAME=Select Set #7\n\n");
			fprintf(temp,"[MOVES8]\n");
			fprintf(temp,"NAME=Select Set #8\n\n");
			fprintf(temp,"[MOVES9]\n");
			fprintf(temp,"NAME=Select Set #9\n\n");
			fprintf(temp,"[MOVES10]\n");
			fprintf(temp,"NAME=Select Set #10\n\n");
			fprintf(temp,"[MOVES11]\n");
			fprintf(temp,"NAME=Select Set #11\n\n");
			fprintf(temp,"[MOVES12]\n");
			fprintf(temp,"NAME=Select Set #12\n\n");
			fprintf(temp,"[MOVES13]\n");
			fprintf(temp,"NAME=Select Set #13\n\n");
			fprintf(temp,"[MOVES14]\n");
			fprintf(temp,"NAME=Select Set #14\n\n");
			fprintf(temp,"[MOVES15]\n");
			fprintf(temp,"NAME=Select Set #15\n\n");
			fprintf(temp,"[MOVES16]\n");
			fprintf(temp,"NAME=Select Set #16\n\n");
			fprintf(temp,"[MOVES17]\n");
			fprintf(temp,"NAME=Select Set #17\n\n");
			fprintf(temp,"[MOVES18]\n");
			fprintf(temp,"NAME=Select Set #18\n\n");
			fprintf(temp,"[MOVES19]\n");
			fprintf(temp,"NAME=Select Set #19\n\n");
			fprintf(temp,"[MOVES20]\n");
			fprintf(temp,"NAME=Select Set #20\n\n");
			fclose(temp);
		}
	}

	//get the last position
	GetWindowRect(&m_sInitialRect);
	int dLastX = GetPrivateProfileInt("CONFIGURATION","ATX",0,g_cIniFile);
	int dLastY = GetPrivateProfileInt("CONFIGURATION","ATY",0,g_cIniFile);

	//get the time span to default to
	m_iTimeSpanMonths = GetPrivateProfileInt("CONFIGURATION","TIMESPANMONTHS",-1,g_cIniFile);
	if (m_iTimeSpanMonths < 0)
	{
		WritePrivateProfileString("CONFIGURATION","TIMESPANMONTHS","1",g_cIniFile);
		m_iTimeSpanMonths = 1;
	}
	//SetDlgItemInt(IDC_EDIT_SPAN,m_iTimeSpanMonths);
	m_uiSpan = m_iTimeSpanMonths;

	//get the zip path
	GetPrivateProfileString("CONFIGURATION","ZIPPATH","",g_szZipPath,sizeof(g_szZipPath),g_cIniFile);
	if (strlen(g_szZipPath)==0)
	{
		strcpy(g_szZipPath,"C:\\MIC\\PKZIP25.EXE");
		WritePrivateProfileString("CONFIGURATION","ZIPPATH",g_szZipPath,g_cIniFile);
	}
	SetDlgItemText(IDC_STATIC_PATH,g_szZipPath);

	//log default
	g_bDoLog = GetPrivateProfileInt("CONFIGURATION","LOGDEFAULT",0,g_cIniFile)==1;
	m_pDoLog.SetCheck(g_bDoLog?1:0);

	//see if ini file wants a unique log file
	char szTemp[MAX_PATH];											//SCR00504 dgp
	GetPrivateProfileString("CONFIGURATION","LOG_NAMELOC","C:\\EZ_COPY %Y_%m_%d LOG.txt",szTemp,sizeof(szTemp),g_cIniFile);//SCR00504 dgp
	if (strlen(szTemp) > 0)											//SCR00504 dgp
	{																//SCR00504 dgp
		COleDateTime cNow = COleDateTime::GetCurrentTime();			//SCR00504 dgp
		strcpy(g_cLogFile,cNow.Format(szTemp));						//SCR00504 dgp
	}																//SCR00504 dgp

	//date from name default
	g_bDateFromName = GetPrivateProfileInt("CONFIGURATION","DATEFROMNAME",0,g_cIniFile)==1;
	m_pDateFromName.SetCheck(g_bDateFromName?1:0);
	
	m_sInitialRect.left = min(dLastX,
		::GetSystemMetrics(SM_CXSCREEN)-GetSystemMetrics(SM_CXICON));
	m_sInitialRect.top  = min(dLastY,
		::GetSystemMetrics(SM_CYSCREEN)-GetSystemMetrics(SM_CYICON));
	SetWindowPos(&wndTop,
		m_sInitialRect.left,m_sInitialRect.top,
		0,0,
		SWP_NOSIZE);

	//set columns in listcontrol
	m_pListDir.InsertColumn(1, "From",		LVCFMT_LEFT,	270,	0);
	m_pListDir.InsertColumn(2, "Sub-Dir",   LVCFMT_LEFT,    65,     1);
	m_pListDir.InsertColumn(3, "Only If Change",  LVCFMT_LEFT,  70,     2);
	m_pListDir.InsertColumn(4, "Bulk UnZip",LVCFMT_LEFT,    65,		3);
	m_pListDir.InsertColumn(5, "To",		LVCFMT_LEFT,	275,	4);
	m_pListDir.InsertColumn(6, "File Types",LVCFMT_LEFT,	245,	5);

    m_DateTimeFrom.SubclassDlgItem(IDC_CUSTOM_FROM, this);
    m_DateTimeTo.SubclassDlgItem(IDC_CUSTOM_TO, this);
	m_DateTimeFrom.SetFormat("ddd',' MMM dd',' yyyy 'at' HH:mm:ss");
	m_DateTimeTo.SetFormat("ddd',' MMM dd',' yyyy 'at' HH:mm:ss");

	szTemp[0] = NULL;
	char szTemp1[64];
	char szTemp2[16];
	GetPrivateProfileString("MOVES","NAME","Select Set #1",szTemp,sizeof(szTemp),g_cIniFile);
	m_pSelectionSet.AddString(szTemp);
	for (int i = 2; i <= 20; i++)
	{
		sprintf(szTemp1,"Select Set #%d",i);
		sprintf(szTemp2,"MOVES%d",i);
		GetPrivateProfileString(szTemp2,"NAME",szTemp1,szTemp,sizeof(szTemp),g_cIniFile);
		m_pSelectionSet.AddString(szTemp);
	}

	m_pSelectionSet.SetCurSel(0);
	OnSelectionChange();

	//set start and end dates
	COleDateTime now = COleDateTime::GetCurrentTime();
	now = COleDateTime(now.GetYear(),now.GetMonth(),1,0,0,0);
	m_DateTimeTo.SetDateTime(now);
	COleDateTime xnow;// = COleDateTime(now.GetYear(),now.GetMonth()-m_iTimeSpanMonths,1,0,0,0);

	int iYear = now.GetYear();
	int iMonth = now.GetMonth()-m_iTimeSpanMonths;
	while (iMonth < 1)
	{
		iMonth += 12;
		iYear--;
	}
	xnow = COleDateTime(iYear,iMonth,1,0,0,0);
	
	m_DateTimeFrom.SetDateTime(xnow);
	OnButtonMonthplus();

	g_bAbort = false;
	g_bPause = false;										//SCR00351
	
	m_pListDir.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,(LPARAM)
		LVS_EX_FULLROWSELECT);

	g_pListDir = &m_pListDir;
	g_pDateTimeFrom = &m_DateTimeFrom;
	g_pDateTimeTo = &m_DateTimeTo;

	SetDlgItemInt(IDC_EDIT_SPAN,m_iTimeSpanMonths);

	SetDlgItemText(IDC_STATIC_NAME,
		"NOTE: Nearly all files created by MIC using the short form name have an hour and minute of "
		"0:00 coded as \"A00\" and therefore if the FROM time is set to something other than 00:00:00 "
		"and \"Get Date from Name\" is selected then the first day's files may be missed.\r\n"
		"When ready, select the desired set and click on the \"Do File Copy\" button.");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCullFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCullFileDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCullFileDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCullFileDlg::OnButtonAdddir() 
{
	// TODO: Add your control notification handler code here
	AddItem Add;
	Add.SetTitle("Add New Entry");
	char szBuff[256];													//scr00348
	GetPrivateProfileString("CONFIGURATION","DEFAULT","*.cev,*.pfm,"	//scr00348
		"*.bin,*.isr,*.mca,*.jsr,*.bid,*.bny,*.ess,*.jpg",szBuff,sizeof(szBuff),g_cIniFile);//scr00348
	Add.SetUp("",FALSE,FALSE,"",szBuff,g_cIniFile,FALSE);				//scr00348
	if (Add.DoModal() == IDOK)
	{
		LV_ITEM lvitem;

		//add to display 
		int i = m_pListDir.GetItemCount();
		//so create it
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 0;
		lvitem.pszText = (char*)(LPCSTR)Add.m_cSourceDir; 
		lvitem.iImage = 0;
		m_pListDir.InsertItem(&lvitem);

		//create the 2nd column entry
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 1;
		lvitem.iImage = 0;
		lvitem.pszText = (Add.m_bDoSubDirs)?"Yes":"No";
		m_pListDir.SetItem(&lvitem);

		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 2;
		lvitem.iImage = 0;
		lvitem.pszText = (Add.m_bOnlyIfChange)?"Yes":"No";
		m_pListDir.SetItem(&lvitem);

		//create the 5th column entry
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 3;
		lvitem.iImage = 0;
		lvitem.pszText = (Add.m_bDoBulk)?"Yes":"No";
		m_pListDir.SetItem(&lvitem);

		//create the 3rd column entry
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 4;
		lvitem.iImage = 0;
		lvitem.pszText = (char*)(LPCSTR)Add.m_cDestinationDir;
		m_pListDir.SetItem(&lvitem);

		//create the 4th column entry
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 5;
		lvitem.iImage = 0;
		lvitem.pszText = (char*)(LPCSTR)Add.m_cFileTypes;
		m_pListDir.SetItem(&lvitem);

		char Moves[16];

		if (m_pSelectionSet.GetCurSel()== 0)
			strcpy(Moves,"MOVES");
		else
			sprintf(Moves,"MOVES%d",m_pSelectionSet.GetCurSel()+1);

		CString Temp;
		Temp.Format("SOURCE%d",i+1);
		WritePrivateProfileString(Moves,Temp,Add.m_cSourceDir,g_cIniFile);
		Temp.Format("RECURS%d",i+1);
		WritePrivateProfileString(Moves,Temp,Add.m_bDoSubDirs?"Yes":"No",g_cIniFile);
		Temp.Format("CHANGE%d",i+1);
		WritePrivateProfileString(Moves,Temp,Add.m_bOnlyIfChange?"Yes":"No",g_cIniFile);
		Temp.Format("DESTIN%d",i+1);
		WritePrivateProfileString(Moves,Temp,Add.m_cDestinationDir,g_cIniFile);
		Temp.Format("FTYPES%d",i+1);
		WritePrivateProfileString(Moves,Temp,Add.m_cFileTypes,g_cIniFile);
		Temp.Format("BULK%d",i+1);
		WritePrivateProfileString(Moves,Temp,Add.m_bDoBulk?"Yes":"No",g_cIniFile);

	}
	if (m_pListDir.GetItemCount() <= 0)
		m_pDoCopyButton.EnableWindow(false);
	else
		m_pDoCopyButton.EnableWindow(true);
}

void CCullFileDlg::OnButtonRemdir() 
{
	// TODO: Add your control notification handler code here
	if (m_dSelectedItem >= 0) 
	{
		int dSelectedItem = m_dSelectedItem;
		m_pListDir.DeleteItem(m_dSelectedItem);

		CString Source;
		CString Recurs;
		CString Destin;
		CString Ftypes;
		CString Change;
		CString Bulk;
		CString Temp;
		bool Ok = true;
		char Moves[16];
		char szName[64];

		//which section are we working with
		if (m_pSelectionSet.GetCurSel()== 0)
			strcpy(Moves,"MOVES");
		else
			sprintf(Moves,"MOVES%d",m_pSelectionSet.GetCurSel());

		//get and save the name for this move
		GetPrivateProfileString(Moves,"NAME","",szName,sizeof(szName),g_cIniFile);

		//clear out all of the existing information
		WritePrivateProfileString(Moves,NULL,NULL,g_cIniFile);

		//save the name back to the section
		WritePrivateProfileString(Moves,"NAME",szName,g_cIniFile);

		//flush the ini cache
		WritePrivateProfileString(NULL,NULL,NULL,g_cIniFile);

		//write each of the items remaining in the set
		for (int i = 0; i < m_pListDir.GetItemCount(); i++)
		{
			Source = m_pListDir.GetItemText( i, 0);
			Recurs = m_pListDir.GetItemText( i, 1);
			Change = m_pListDir.GetItemText( i, 2);
			Bulk   = m_pListDir.GetItemText( i, 3);
			Destin = m_pListDir.GetItemText( i, 4);
			Ftypes = m_pListDir.GetItemText( i, 5);
			Temp.Format("SOURCE%d",i+1);
			WritePrivateProfileString(Moves,Temp,Source,g_cIniFile);
			Temp.Format("DESTIN%d",i+1);
			WritePrivateProfileString(Moves,Temp,Destin,g_cIniFile);
			Temp.Format("FTYPES%d",i+1);
			WritePrivateProfileString(Moves,Temp,Ftypes,g_cIniFile);
			Temp.Format("RECURS%d",i+1);
			WritePrivateProfileString(Moves,Temp,Recurs,g_cIniFile);
			Temp.Format("BULK%d",i+1);
			WritePrivateProfileString(Moves,Temp,Bulk,g_cIniFile);
			Temp.Format("CHANGE%d",i+1);
			WritePrivateProfileString(Moves,Temp,Change,g_cIniFile);
		}
	}
	if (m_pListDir.GetItemCount() <= 0)
		m_pDoCopyButton.EnableWindow(false);
	else
		m_pDoCopyButton.EnableWindow(true);
}

void CCullFileDlg::OnOK() 
{
	// TODO: Add extra validation here
	g_bAbort = true;
	if (m_hThread)
		WaitForSingleObject(m_hThread,1000);
	CDialog::OnOK();
}

void CCullFileDlg::OnButtonDocopy() 
{
	// TODO: Add your control notification handler code here
	if (_access(g_szZipPath,4)==0)													//DR00350
	{						//DR00350
		m_pAbortButton.EnableWindow(true);	
		m_pPauseButton.EnableWindow(true);											//SCR00351
		CWinThread* pThread;
		m_hThread = NULL;
		pThread = AfxBeginThread(ThreadFunc, m_hWnd);
		m_hThread = pThread->m_hThread;
	}																				//DR00350
	else																			//DR00350
		AfxMessageBox("PkZip25.exe not found. Please, reset the path.",MB_OK,0);	//DR00350
}

void CCullFileDlg::OnItemchangedListDirectories(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (m_pListDir.GetSelectedCount())
	{
		m_dSelectedItem = pNMListView->iItem;
		m_pRemDir.EnableWindow(true);
		m_pEditDir.EnableWindow(true);
		m_pDoCopyButton.EnableWindow(true);
	}
	else
	{
		m_dSelectedItem = -1;
		m_pRemDir.EnableWindow(false);
		m_pEditDir.EnableWindow(false);
		if (m_pListDir.GetItemCount())
			m_pDoCopyButton.EnableWindow(true);
		else
			m_pDoCopyButton.EnableWindow(false);
	}
	*pResult = 0;
}

void CCullFileDlg::OnButtonMonthplus() 
{
	// TODO: Add your control notification handler code here
	COleDateTime from = m_DateTimeFrom.GetOleDateTime();
	int oldmonth = from.GetMonth();
	int newmonth = oldmonth;
	while (newmonth == oldmonth)
	{
		from = from + COleDateTimeSpan(1,0,0,0);
		newmonth = from.GetMonth();
	}
	m_DateTimeFrom.SetDateTime(from);

	from = m_DateTimeTo.GetOleDateTime();
	oldmonth = from.GetMonth();
	newmonth = oldmonth;
	while (newmonth == oldmonth)
	{
		from = from + COleDateTimeSpan(1,0,0,0);
		newmonth = from.GetMonth();
	}
	m_DateTimeTo.SetDateTime(from);
}

void CCullFileDlg::OnButtonMonthminus() 
{
	// TODO: Add your control notification handler code here
	COleDateTime from = m_DateTimeFrom.GetOleDateTime();
	int oldmonth = from.GetMonth();
	int newmonth = oldmonth;
	while (newmonth == oldmonth)
	{
		from = from - COleDateTimeSpan(1,0,0,0);
		newmonth = from.GetMonth();
	}
	oldmonth = newmonth;
	while (newmonth == oldmonth)
	{
		from = from - COleDateTimeSpan(1,0,0,0);
		newmonth = from.GetMonth();
	}
	from = from + COleDateTimeSpan(1,0,0,0);
	m_DateTimeFrom.SetDateTime(from);

	from = m_DateTimeTo.GetOleDateTime();
	oldmonth = from.GetMonth();
	newmonth = oldmonth;
	while (newmonth == oldmonth)
	{
		from = from - COleDateTimeSpan(1,0,0,0);
		newmonth = from.GetMonth();
	}
	oldmonth = newmonth;
	while (newmonth == oldmonth)
	{
		from = from - COleDateTimeSpan(1,0,0,0);
		newmonth = from.GetMonth();
	}
	from = from + COleDateTimeSpan(1,0,0,0);
	m_DateTimeTo.SetDateTime(from);
	
}

void CCullFileDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
	if (strlen(g_cIniFile)>0)
	{
		GetWindowRect(&m_sInitialRect);
		char temp[16];
		itoa(m_sInitialRect.left,temp,10);
		WritePrivateProfileString("CONFIGURATION","ATX",temp,g_cIniFile);
		itoa(m_sInitialRect.top,temp,10);
		WritePrivateProfileString("CONFIGURATION","ATY",temp,g_cIniFile);
	}
}

void CCullFileDlg::OnCheckDatefromname() 
{
	// TODO: Add your control notification handler code here
	if (((CButton*)GetDlgItem(IDC_CHECK_DATEFROMNAME))->GetCheck()==1)
		g_bDateFromName = true;
	else
		g_bDateFromName = false;
	WritePrivateProfileString("CONFIGURATION","DATEFROMNAME",g_bDateFromName?"1":"0",g_cIniFile);
}

void CCullFileDlg::OnButtonAbort() 
{
	// TODO: Add your control notification handler code here
	BeginWaitCursor();
	SetDlgItemText(IDC_STATIC_NAME,"ABORTING -- this may take a few moments.  If running, the current COPY or UNZIP action must complete.");
	g_bAbort = true;
}

void CCullFileDlg::OnButtonPause()					//SCR00351
{													//SCR00351
	g_bPause = !g_bPause;							//SCR00351
	if (g_bPause)									//SCR00351
	{												//SCR00351
		CString cString;							//SCR00351
		GetDlgItemText(IDC_STATIC_NAME,cString);	//SCR00351
		cString += "\nPaused...";					//SCR00351
		SetDlgItemText(IDC_STATIC_NAME,cString);	//SCR00351
		m_pPauseButton.SetWindowText("Continue");	//SCR00351
	}												//SCR00351
	else											//SCR00351
		m_pPauseButton.SetWindowText("Pause Copy");	//SCR00351
}													//SCR00351

void CCullFileDlg::OnCheckDolog() 
{
	// TODO: Add your control notification handler code here
	if (((CButton*)GetDlgItem(IDC_CHECK_DOLOG))->GetCheck()==1)
		g_bDoLog = true;
	else
		g_bDoLog = false;
	WritePrivateProfileString("CONFIGURATION","LOGDEFAULT",g_bDoLog?"1":"0",g_cIniFile);
}


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	MSKVersionInfo VersionInfo;
	CString Name    = VersionInfo.InternalName();
	CString Version = VersionInfo.FileVersion();
	Version.Format("%c.%c.%c.%c",Version[0],Version[3],Version[6],Version[9]);
	Name = Name + " - ";
	Name += Version;
	SetDlgItemText(IDC_STATIC_VERSION,(LPCSTR)Name);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCullFileDlg::OnSelectionChange() 
{
	// TODO: Add your control notification handler code here
	//putinfo in list box
	int index = 1;
	char Source[_MAX_PATH];
	char Destin[_MAX_PATH];
	char Ftypes[_MAX_PATH];
	char Moves[16];
	char Recurs[8];
	char Change[8];
	char Bulk[8];
	CString Temp;

	for (int i = m_pListDir.GetItemCount(); i > 0; i--)
		m_pListDir.DeleteItem(i-1);

	m_dSelectedItem = -1;

	if (m_pSelectionSet.GetCurSel()== 0)
		strcpy(Moves,"MOVES");
	else
		sprintf(Moves,"MOVES%d",m_pSelectionSet.GetCurSel()+1);

	bool Ok = true;
	do
	{
		Temp.Format("SOURCE%d",index);
		GetPrivateProfileString(Moves,Temp,"",Source,sizeof(Source),g_cIniFile);
		Temp.Format("RECURS%d",index);
		GetPrivateProfileString(Moves,Temp,"No",Recurs,sizeof(Recurs),g_cIniFile);
		Temp.Format("DESTIN%d",index);
		GetPrivateProfileString(Moves,Temp,"",Destin,sizeof(Destin),g_cIniFile);
		Temp.Format("FTYPES%d",index);
		GetPrivateProfileString(Moves,Temp,"",Ftypes,sizeof(Ftypes),g_cIniFile);
		Temp.Format("BULK%d",index);
		GetPrivateProfileString(Moves,Temp,"No",Bulk,sizeof(Bulk),g_cIniFile);
		Temp.Format("CHANGE%d",index);
		GetPrivateProfileString(Moves,Temp,"No",Change,sizeof(Change),g_cIniFile);
		index++;
		if (strlen(Source) && strlen(Destin) && strlen(Ftypes))
		{

			LV_ITEM lvitem;
			int NewItem;
			//add to display 
			//so create it
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = index;
			lvitem.iSubItem = 0;
			lvitem.pszText = Source; 
			lvitem.iImage = 0;
			NewItem = m_pListDir.InsertItem(&lvitem);

			//create the 2nd column entry
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = NewItem;
			lvitem.iSubItem = 1;
			lvitem.iImage = 0;
			lvitem.pszText = Recurs;
			m_pListDir.SetItem(&lvitem);

			//create the 4th column entry
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = NewItem;
			lvitem.iSubItem = 2;
			lvitem.iImage = 0;
			lvitem.pszText = Change;
			m_pListDir.SetItem(&lvitem);

			//create the 4th column entry
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = NewItem;
			lvitem.iSubItem = 3;
			lvitem.iImage = 0;
			lvitem.pszText = Bulk;
			m_pListDir.SetItem(&lvitem);

			//create the 3rd column entry
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = NewItem;
			lvitem.iSubItem = 4;
			lvitem.iImage = 0;
			lvitem.pszText = Destin;
			m_pListDir.SetItem(&lvitem);

			//create the 4th column entry
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = NewItem;
			lvitem.iSubItem = 5;
			lvitem.iImage = 0;
			lvitem.pszText = Ftypes;
			m_pListDir.SetItem(&lvitem);

			m_pDoCopyButton.EnableWindow(true);
		}
		else
			Ok = false;
	} while (Ok);
	
}

void CCullFileDlg::OnButtonEdit() 
{
	// TODO: Add your control notification handler code here
	CSetNames SetNames;
	char szTemp[64];
	GetPrivateProfileString("MOVES","NAME","Select Set #1",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet1 = szTemp;
	GetPrivateProfileString("MOVES2","NAME","Select Set #2",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet2 = szTemp;
	GetPrivateProfileString("MOVES3","NAME","Select Set #3",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet3 = szTemp;
	GetPrivateProfileString("MOVES4","NAME","Select Set #4",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet4 = szTemp;
	GetPrivateProfileString("MOVES5","NAME","Select Set #5",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet5 = szTemp;
	GetPrivateProfileString("MOVES6","NAME","Select Set #6",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet6 = szTemp;
	GetPrivateProfileString("MOVES7","NAME","Select Set #7",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet7 = szTemp;
	GetPrivateProfileString("MOVES8","NAME","Select Set #8",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet8 = szTemp;
	GetPrivateProfileString("MOVES9","NAME","Select Set #9",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet9 = szTemp;
	GetPrivateProfileString("MOVES10","NAME","Select Set #10",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet10 = szTemp;
	GetPrivateProfileString("MOVES11","NAME","Select Set #11",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet11 = szTemp;
	GetPrivateProfileString("MOVES12","NAME","Select Set #12",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet12 = szTemp;
	GetPrivateProfileString("MOVES13","NAME","Select Set #13",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet13 = szTemp;
	GetPrivateProfileString("MOVES14","NAME","Select Set #14",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet14 = szTemp;
	GetPrivateProfileString("MOVES15","NAME","Select Set #15",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet15 = szTemp;
	GetPrivateProfileString("MOVES16","NAME","Select Set #16",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet16 = szTemp;
	GetPrivateProfileString("MOVES17","NAME","Select Set #17",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet17 = szTemp;
	GetPrivateProfileString("MOVES18","NAME","Select Set #18",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet18 = szTemp;
	GetPrivateProfileString("MOVES19","NAME","Select Set #19",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet19 = szTemp;
	GetPrivateProfileString("MOVES20","NAME","Select Set #20",szTemp,sizeof(szTemp),g_cIniFile);
	SetNames.m_cSet20 = szTemp;

	if (SetNames.DoModal()==IDOK)
	{
		//save to ini file
		WritePrivateProfileString("MOVES","NAME",LPCSTR(SetNames.m_cSet1),g_cIniFile);
		WritePrivateProfileString("MOVES2","NAME",LPCSTR(SetNames.m_cSet2),g_cIniFile);
		WritePrivateProfileString("MOVES3","NAME",LPCSTR(SetNames.m_cSet3),g_cIniFile);
		WritePrivateProfileString("MOVES4","NAME",LPCSTR(SetNames.m_cSet4),g_cIniFile);
		WritePrivateProfileString("MOVES5","NAME",LPCSTR(SetNames.m_cSet5),g_cIniFile);
		WritePrivateProfileString("MOVES6","NAME",LPCSTR(SetNames.m_cSet6),g_cIniFile);
		WritePrivateProfileString("MOVES7","NAME",LPCSTR(SetNames.m_cSet7),g_cIniFile);
		WritePrivateProfileString("MOVES8","NAME",LPCSTR(SetNames.m_cSet8),g_cIniFile);
		WritePrivateProfileString("MOVES9","NAME",LPCSTR(SetNames.m_cSet9),g_cIniFile);
		WritePrivateProfileString("MOVES10","NAME",LPCSTR(SetNames.m_cSet10),g_cIniFile);
		WritePrivateProfileString("MOVES11","NAME",LPCSTR(SetNames.m_cSet11),g_cIniFile);
		WritePrivateProfileString("MOVES12","NAME",LPCSTR(SetNames.m_cSet12),g_cIniFile);
		WritePrivateProfileString("MOVES13","NAME",LPCSTR(SetNames.m_cSet13),g_cIniFile);
		WritePrivateProfileString("MOVES14","NAME",LPCSTR(SetNames.m_cSet14),g_cIniFile);
		WritePrivateProfileString("MOVES15","NAME",LPCSTR(SetNames.m_cSet15),g_cIniFile);
		WritePrivateProfileString("MOVES16","NAME",LPCSTR(SetNames.m_cSet16),g_cIniFile);
		WritePrivateProfileString("MOVES17","NAME",LPCSTR(SetNames.m_cSet17),g_cIniFile);
		WritePrivateProfileString("MOVES18","NAME",LPCSTR(SetNames.m_cSet18),g_cIniFile);
		WritePrivateProfileString("MOVES19","NAME",LPCSTR(SetNames.m_cSet19),g_cIniFile);
		WritePrivateProfileString("MOVES20","NAME",LPCSTR(SetNames.m_cSet20),g_cIniFile);

		//delete all in list
		int CurSel = m_pSelectionSet.GetCurSel();
		m_pSelectionSet.ResetContent();
		//add these to list
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet1));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet2));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet3));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet4));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet5));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet6));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet7));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet8));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet9));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet10));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet11));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet12));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet13));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet14));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet15));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet16));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet17));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet18));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet19));
		m_pSelectionSet.AddString(LPCSTR(SetNames.m_cSet20));
		m_pSelectionSet.SetCurSel(CurSel);
	}
}

void CCullFileDlg::OnButtonEditdir() 
{
	// TODO: Add your control notification handler code here
	if (m_dSelectedItem >=0)
	{
		AddItem Add;
		Add.SetTitle("Edit Existing Entry");
		CString cSource(m_pListDir.GetItemText(m_dSelectedItem,0));
		CString cDoSub(m_pListDir.GetItemText(m_dSelectedItem,1));
		CString cOnlyIfChange(m_pListDir.GetItemText(m_dSelectedItem,2));
		CString cBulk(m_pListDir.GetItemText(m_dSelectedItem,3));
		CString cDestination(m_pListDir.GetItemText(m_dSelectedItem,4));
		CString cTypes(m_pListDir.GetItemText(m_dSelectedItem,5));
		Add.SetUp(cSource,
			(toupper(cDoSub[0]) == 'Y')?TRUE:FALSE,
			(toupper(cBulk[0]) == 'Y')?TRUE:FALSE,
			cDestination,cTypes,
			g_cIniFile,
			(toupper(cOnlyIfChange[0]) == 'Y')?TRUE:FALSE);
		if (Add.DoModal() == IDOK)
		{
			//add to display
			m_pListDir.SetItemText(m_dSelectedItem,0,Add.m_cSourceDir);
			m_pListDir.SetItemText(m_dSelectedItem,1,Add.m_bDoSubDirs?"Yes":"No");
			m_pListDir.SetItemText(m_dSelectedItem,2,Add.m_bOnlyIfChange?"Yes":"No");
			m_pListDir.SetItemText(m_dSelectedItem,3,Add.m_bDoBulk?"Yes":"No");
			m_pListDir.SetItemText(m_dSelectedItem,4,Add.m_cDestinationDir);
			m_pListDir.SetItemText(m_dSelectedItem,5,Add.m_cFileTypes);

			//modify ini file
			char Moves[16];
			if (m_pSelectionSet.GetCurSel()== 0)
				strcpy(Moves,"MOVES");
			else
				sprintf(Moves,"MOVES%d",m_pSelectionSet.GetCurSel()+1);
			CString cTemp;
			cTemp.Format("SOURCE%d",m_dSelectedItem+1);
			WritePrivateProfileString(Moves,cTemp,Add.m_cSourceDir,g_cIniFile);
			cTemp.Format("DESTIN%d",m_dSelectedItem+1);
			WritePrivateProfileString(Moves,cTemp,Add.m_cDestinationDir,g_cIniFile);
			cTemp.Format("FTYPES%d",m_dSelectedItem+1);
			WritePrivateProfileString(Moves,cTemp,Add.m_cFileTypes,g_cIniFile);
			cTemp.Format("RECURS%d",m_dSelectedItem+1);
			WritePrivateProfileString(Moves,cTemp,Add.m_bDoSubDirs?"Yes":"No",g_cIniFile);
			cTemp.Format("BULK%d",m_dSelectedItem+1);
			WritePrivateProfileString(Moves,cTemp,Add.m_bDoBulk?"Yes":"No",g_cIniFile);
			cTemp.Format("CHANGE%d",m_dSelectedItem+1);
			WritePrivateProfileString(Moves,cTemp,Add.m_bOnlyIfChange?"Yes":"No",g_cIniFile);
		}
	}
}

BOOL CCullFileDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if (g_bAbort)
	{
		SetCursor(m_hAbortIcon);
		return TRUE;
	}
	else if (m_hThread)
	{
		CRect rcBtn1, rcBtn2, rcBtn3;						//SCR00351
		CPoint	ptCursor;
		CWnd* pBtn = GetDlgItem(IDC_BUTTON_ABORT);
		pBtn->GetWindowRect(rcBtn1);

		pBtn = GetDlgItem(IDOK);
		pBtn->GetWindowRect(rcBtn2);

		pBtn = GetDlgItem(IDC_BUTTON_PAUSE);				//SCR00351
		pBtn->GetWindowRect(rcBtn3);						//SCR00351

		GetCursorPos(&ptCursor);
		if ((rcBtn1.PtInRect(ptCursor) == FALSE) &&
			(rcBtn2.PtInRect(ptCursor) == FALSE) &&
			(rcBtn3.PtInRect(ptCursor) == FALSE)			//SCR00351
			)
		{
			BeginWaitCursor();
			return TRUE;
		}
	}

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CCullFileDlg::OnButtonZippath() 
{
	// TODO: Add your control notification handler code here
	CFileDialog cFileDialog( TRUE, ".EXE", "PKZIP25.EXE", OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "PKZIP25.EXE|PKZIP25.EXE|All files|*.*||", this );

	if (cFileDialog.DoModal()==IDOK)
	{
		CString cTemp;
		cTemp = cFileDialog.GetPathName();
		SetDlgItemText(IDC_STATIC_PATH,cTemp);
		strcpy(g_szZipPath,cTemp);
		WritePrivateProfileString("CONFIGURATION","ZIPPATH",cTemp,g_cIniFile);
	}
}

void CCullFileDlg::OnChangeEditSpan() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	COleDateTime cTimeFrom;
	COleDateTime cTimeTo = g_pDateTimeTo->GetOleDateTime();
	m_iTimeSpanMonths = GetDlgItemInt(IDC_EDIT_SPAN);
	CString cString;
	GetDlgItemText(IDC_EDIT_SPAN,cString);
	if (m_iTimeSpanMonths <= 0)
	{
		m_iTimeSpanMonths = 1;
		SetDlgItemInt(IDC_EDIT_SPAN,1);
		return;
	}
	int iYear = cTimeTo.GetYear();
	int iMonth = cTimeTo.GetMonth()-m_iTimeSpanMonths;
	while (iMonth < 1)
	{
		iMonth += 12;
		iYear--;
	}
	cTimeFrom = COleDateTime(iYear,iMonth,1,0,0,0);
	
	m_DateTimeFrom.SetDateTime(cTimeFrom);

	WritePrivateProfileString("CONFIGURATION","TIMESPANMONTHS",cString,g_cIniFile);
		
}
