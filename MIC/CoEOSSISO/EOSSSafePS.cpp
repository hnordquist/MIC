// EOSSSafePS.cpp
//

#include "stdafx.h"
#include "ISO_definitions.h"
#include "INI_definitions.h"
#include "colors.h"
#include "ccbutton.h"
#include "JulianT.h"
#include "EOSSSafePS.h"
#include "EOSSInstrument.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define SINGLE_BINARY_FILE
#undef SINGLE_BINARY_FILE

#define PI2 6.283185307


//registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;


#define COLOR_GREENRED	1
#define COLOR_BLUERED   2
#define COLOR_REDYELLOW 3


/////////////////////////////////////////////////////////////////////////////
// CEOSSPropertySheet

IMPLEMENT_DYNAMIC(CEOSSPropertySheet, CPropertySheet)

CEOSSPropertySheet::CEOSSPropertySheet(
	UINT nIDCaption, 
	CEOSSInstrument* pParentWnd)
	:CPropertySheet(nIDCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	m_pCaption[0] = NULL;
	FinishConstruction();
}

CEOSSPropertySheet::CEOSSPropertySheet(
	LPCTSTR pszCaption, 
	CEOSSInstrument* pParentWnd)
	:CPropertySheet(pszCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	strcpy(m_pCaption,pszCaption);
	FinishConstruction();
}

void CEOSSPropertySheet::FinishConstruction()
{
	uIDC_RECEIVE = m_pParent->m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_pParent->m_uIDC_RECEIVE_OOB;

	Data_Tab		= 0;

	m_bNewFileName	= false;
	m_bTimeError	= false;

	m_dToClose				= 0;

	m_bStartOfFile			= true;

	m_szCurrentFileName[0]	= NULL;
	m_szPreviousGTime[0]	= NULL;
	m_dCurrentDay			= -1;

	m_pCloseButton    = NULL;
	m_pPauseButton    = NULL;
	m_pPrintButton    = NULL;
	m_pAckFlagsButton = NULL;
	m_pToClose        = NULL;
	m_pImageList      = NULL;
}

CEOSSPropertySheet::~CEOSSPropertySheet()
{
	delete m_pCloseButton;
	delete m_pPauseButton;
	delete m_pPrintButton;
	delete m_pAckFlagsButton;
	delete m_pToClose;
	delete m_pImageList;
}


COLORREF CEOSSPropertySheet::SetButtonColor(COLORREF color)
{
	if (color == COLOR_PAUSE)
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
	else
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);

	return m_pCloseButton->SetFaceColor(color);
}

void CEOSSPropertySheet::SetLimits(int time_err,char* Id)
{	
	m_dTime_Err=(long)time_err;
	strcpy(m_szCurrentID,Id);
}

BEGIN_MESSAGE_MAP(CEOSSPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CEOSSPropertySheet)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CLOSE_BUTTON,	OnCloseButton)
	ON_BN_CLICKED(ID_PAUSE_BUTTON,	OnPauseButton)
	ON_BN_CLICKED(ID_ACKFL_BUTTON,	OnAckButton)
	ON_BN_CLICKED(ID_PRINT_BUTTON,	OnPrintButton)
	ON_BN_CLICKED(ID_TOCLOSE,		OnToCloseButton)

	ON_MESSAGE(IDC_EXTENDCLOSE, OnExtendClose)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE,OnReceive)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE_OOB,OnReceiveOOB)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEOSSPropertySheet message handlers
void CEOSSPropertySheet::UpdateSummary(char *Message)
{
	CString Data;
	CEOSSSummary*  summarypage	 = m_pParent->m_pSummaryPage;

	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND10,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND11,Data);
	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND9,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND10,Data);
	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND8,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND9,Data);
	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND7,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND8,Data);
	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND6,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND7,Data);
	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND5,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND6,Data);
	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND4,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND5,Data);
	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND3,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND4,Data);
	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND3,Data);
	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_COMMAND1,Data);
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND2,Data);
	COleDateTime cNow = COleDateTime::GetCurrentTime();
	Data = cNow.Format("%Y.%m.%d %H:%M:%S ");
	Data += Message;
	summarypage->SetDlgItemText(IDC_EOSSSUMMARY_COMMAND1,Data);
}

LRESULT CEOSSPropertySheet::OnReceiveOOB(WPARAM WParam, LPARAM LParam)
{
	return OnReceive(WParam, LParam);
}

LRESULT CEOSSPropertySheet::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//can do this without an "accumulator" because all "event" messages are complete within an 
	//ilon packet consequently we will receive the entire message at one time
	BYTE* cbuff =(BYTE*)LParam;
	char c;
	if (WParam <= 1)
		return 0;

	try
	{
		c = cbuff[0];
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP, NULL,"47110 EOSS Bad LParam Address Received (Start of msg block)");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"47110 EOSS Bad LParam Address Received (Start of msg block)");
		CoTaskMemFree((void *)LParam);
		return 0;
	}
	try
	{
		c = cbuff[WParam-1];
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP, NULL,"47111 EOSS Bad LParam Address Received (End of msg block)");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"47111 EOSS Bad LParam Address Received (End of msg block)");
		CoTaskMemFree((void *)LParam);
		return 0;
	}

	CoTaskMemFree((void *)LParam);
	return 0;
}

void CEOSSPropertySheet::Reverse(BYTE *sz, int iLength)
{
	BYTE cTemp;
	for (int i = 0; i < (iLength/2); i++)
	{
		cTemp = sz[i];
		sz[i] = sz[iLength - i - 1];
		sz[iLength - i - 1] = cTemp;
	}
}

void
CEOSSPropertySheet::MakeMyPath(const char* path)
{
	//recursive routine to create all 
	//of the directories down the "path"
	//NOTE: existance of a "file" with the 
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
	CreateDirectory(path_buffer,NULL);
}

//this function should go into the parent class
void 
CEOSSPropertySheet::BuildFileName(CString &cResult,
	CString cFormat,CString cPath,CString cType,
	CString cID,
	int iYear,	int iMonth,	int iDay,
	int iHour,	int iMinute,int iSecond,
	CString cRFID, CString cSuffix 
	)
{
	cResult.Empty();
	if (cPath.GetLength())
	{
		cResult = cPath;
		if (cResult.Right(1) != '\\')
			cResult += '\\';
	}
	CString cTemp;
	bool bHaveEscape = false;
	for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
	{
		if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
			bHaveEscape = true;
		else if (bHaveEscape)
		{
			bHaveEscape = false;
			switch (cFormat[iSourceIndex]) {
			case 'T':
				cResult += cType;
				break;
			case 'I':
				{
					int iLength = cID.GetLength();
					while (iLength < 3)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'i':
				{
					int iLength = cID.GetLength();
					while (iLength < 2)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'y':
				cTemp.Format("%04d",iYear);
				cResult += cTemp;
				break;
			case 'm':
				cTemp.Format("%02d",iMonth);
				cResult += cTemp;
				break;
			case 'd':
				cTemp.Format("%02d",iDay);
				cResult += cTemp;
				break;
			case 'H':
				cTemp.Format("%02d",iHour);
				cResult += cTemp;
				break;
			case 'M':
				cTemp.Format("%02d",iMinute);
				cResult += cTemp;
				break;
			case 'S':
				cTemp.Format("%02d",iSecond);
				cResult += cTemp;
				break;
			default:
				cResult += cFormat[iSourceIndex];
			}
		}
		else
			cResult += cFormat[iSourceIndex];
	}

	if (cSuffix.GetLength())
	{
		if ((cResult.Right(1) != '.') && (cSuffix.Left(1) != '.'))
			cResult += '.';
		cResult += cSuffix;
	}
}

bool CEOSSPropertySheet::WriteToESSFile(int iIndex)
{
//	TRACE("CEOSSPropertySheet::WriteToEVTFile(iIndex = %d)\n", iIndex);
	return false;
}

void
CEOSSPropertySheet::WriteToFile(int ToWhom, int Type, CJulianTime* Time, char* Msg)
{
	FILE *File;
	char Message[256];
	fpos_t position;
	Message[0] = NULL;
	COleDateTime TC = COleDateTime::GetCurrentTime();
	COleDateTime TG;
	COleDateTimeSpan TS;
	CString ArchiveFileName;

//	if (m_pParent->m_bBlockWrite) 
//		return;

	//default to the provided EOSS time
	if (Time)
	{
		int Year,Month,Day,Hour,Minute,Second;
		Year = Time->Year();
		Month = Time->Month();
		Day = Time->Day();
		Hour = Time->Hour();
		Minute = Time->Minute();
		Second = Time->Second();
		if (Year	< 100)  Year	= 1900;//COleDateTime limits year 100-9999
		if (Year    > 9999) Year    = 9999;//COleDateTime limits year 100-9999
		if (Month	< 1)	Month	= 1;
		if (Day		< 1)	Day		= 1;
		if (Hour	< 0)	Hour	= 0;
		if (Minute	< 0)	Minute	= 0;
		if (Second	< 0)	Second	= 0;
		TG = COleDateTime(Year, Month, Day, Hour, Minute, Second);
	}
	//if that wasn't available then get the computer time
	//this is the case in all TYPE_COMP messages
	else
	{
		TG = COleDateTime::GetCurrentTime();
	}

	if (ToWhom == TO_DMP)
	{
		//	build new file name and save it
		if (m_pParent->m_bUseShortFilename)
		{
			char cYear;
			int iYear = TC.GetYear();
			if ((iYear < 1990) || (iYear > 2025))
				cYear = '#';
			else if (iYear < 2000)
				cYear = (char)('0' + iYear - 1990);
			else 
				cYear = (char)('A' + iYear - 2000);

			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
	//			((TC.GetYear()-1990)<10)?
	//				((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
	//				'A'+(TC.GetYear()-2000),
				cYear,
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,"EO",
				m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,"",".");
			strcpy(m_szCurrentFileName,cResult);
		}
	}
	else
	{
		if (m_pParent->m_bUseShortFilename)
		{
			//	build new file name and save it
			char cYear;
			int iYear = TG.GetYear();
			if ((iYear < 1990) || (iYear > 2025))
				cYear = '#';
			else if (iYear < 2000)
				cYear = (char)('0' + iYear - 1990);
			else 
				cYear = (char)('A' + iYear - 2000);

			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				((CEOSSInstrument*)m_pParent)->m_szSaveLoc,	
				((CEOSSInstrument*)m_pParent)->m_pID,
				cYear,
				((TG.GetMonth()<10)?
					'0'+(TG.GetMonth()):
					'A'+(TG.GetMonth()-10)),
				((TG.GetDay()  <10)?
					'0'+(TG.GetDay()):
					'A'+(TG.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,"EO",
				m_pParent->m_pID,TG.GetYear(),TG.GetMonth(),TG.GetDay(),0,0,0,"",".");
			strcpy(m_szCurrentFileName,cResult);
		}

	}	

	//format string to send
	switch (Type){

	case TYPE_DUMP:
		sprintf(Message,"%4d.%02d.%02d %02d:%02d:%02d %s\n",
			TC.GetYear(),TC.GetMonth(),TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),
			Msg);
//		strcpy(Message,Msg);
		break;

	case TYPE_INST:
		//	message = Time::YY.MM.DD HH:MM:SS G (MESSAGE==NULL)?\r:MESSAGE

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d E %s\n",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"":Msg);
		break;

	case TYPE_COMP:
		//	message = MICTIME::YY.MM.DD HH:MM:SS C (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C %s\n",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),Msg[0]==NULL?"":Msg);
		break;

	case TYPE_GID2:
	case TYPE_TIME:
		//	message = Time::YY.MM.DD HH:MM:SS EOSS Time   "+
		//					"YY.MM.DD HH:MM:SS Computer Time   C - E = xxxx\r"
		//computer time
		//EOSS time
		TS = TC - TG;  // Subtract 2 COleDateTimes
		TS += HALF_SECOND;
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d E 47326 EOSS Time %04d.%02d.%02d %02d:%02d:%02d Computer Time   C - E = %.0f seconds\n",
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			TS.GetTotalSeconds());
		break;

	case TYPE_INVTIME:
		//	message = "INVALID TIME  "+
		//		"Previous Record Time Saved::YY.MM.DD HH:MM:SS "+
		//		"Current Record Time Time::YY.MM.DD HH:MM:SS\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 47327 EOSS INVALID TIME %04d.%02d.%02d %02d:%02d:%02d\n",
			((CJulianTime*)Msg)->Year(),	((CJulianTime*)Msg)->Month(),	((CJulianTime*)Msg)->Day(),
			((CJulianTime*)Msg)->Hour(),		((CJulianTime*)Msg)->Minute(),	((CJulianTime*)Msg)->Second(),
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond());
		break;

	case TYPE_START:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C EOSS COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 47411 EOSS COLLECT Version %s started\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion);
		break;

	case TYPE_ABNORMAL:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C EOSS COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 47412 EOSS COLLECT Version %s started from abnormal shutdown\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion);
		break;

	case TYPE_DELETE:
	//	message = MICNOW::YY.MM.DD HH:MM:SS C file %s<MESSAGE> deleted\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 47413 EOSS File %s deleted\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
		break;

	//just in case
	default:
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 47328 EOSS Unknown TYPE %s\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
	}

	//if to dmp do the write to todays file and get out
	if (ToWhom == TO_DMP)
	{
		//	open filename+dmp
		CString fn(m_szCurrentFileName);
		fn += DMP_SUFFIX;
//		CString ArchiveFileNameEx = ArchiveFileName + DMP_SUFFIX;
		if (_access(fn,0) != 0)
		{
//			if (_access(ArchiveFileNameEx,0)!=-1)
//				MoveFileEx(ArchiveFileNameEx,fn,
//					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			//make sure path exists
			MakeMyPath(fn);
		}

		if ((File = fopen(fn,"at+")) != NULL)
		{
			//	send string
			fprintf(File,Message);
			//	close cev
			fclose(File);
		}
		return;
	}

	//if to cev
	if (ToWhom & TO_CEV)
	{
		//	open filename+cev
		CString fn(m_szCurrentFileName);
		fn += CEV_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (_access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);

			if (m_pParent->m_bUseShortFilename)
			{
				//build archive path\name
				char cYear;
				int iYear = TC.GetYear();
				if ((iYear < 1990) || (iYear > 2025))
					cYear = '#';
				else if (iYear < 2000)
					cYear = (char)('0' + iYear - 1990);
				else 
					cYear = (char)('A' + iYear - 2000);

				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				((CEOSSInstrument*)m_pParent)->m_szSaveLoc,
				((CEOSSInstrument*)m_pParent)->m_pID,
				cYear,
	//			((TC.GetYear()-1990)<10)?
	//				((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
	//				'A'+(TC.GetYear()-2000),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,CEV_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = m_pParent->m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,
					m_pParent->m_csLongNameFormatString,
					cTemp,"EO",
					m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,"",CEV_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (_access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"at+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos(File,&position);
			if (position==0)
			{
				m_bStartOfFile	= true;
			}

			//	send string
			fprintf(File,Message);
			//	close cev
			fclose(File);
		}
	}

	//if to pfm
	if (ToWhom & TO_PFM)
	{
		//	open filename+pfm
		CString fn(m_szCurrentFileName);
		fn += PFM_SUFFIX;

//		CString ArchiveFileNameExt = ArchiveFileName + PFM_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (_access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);

			//check if it is in the archive directory
			//build file name
			if (m_pParent->m_bUseShortFilename)
			{
				char cYear;
				int iYear = TC.GetYear();
				if ((iYear < 1990) || (iYear > 2025))
					cYear = '#';
				else if (iYear < 2000)
					cYear = (char)('0' + iYear - 1990);
				else 
					cYear = (char)('A' + iYear - 2000);

				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_pParent->m_szSaveLoc,
				m_pParent->m_pID,
				cYear,
	//			((TC.GetYear()-1990)<10)?
	//				((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
	//				'A'+(TC.GetYear()-2000),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,PFM_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = ((CEOSSInstrument*)m_pParent)->m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,
					m_pParent->m_csLongNameFormatString,
					cTemp,"EO",
					m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,"",PFM_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (_access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"at+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos( File, &position );
			if (position==0)
			{
				m_bStartOfFile	= true;
			}
			//	send string
			fprintf(File,Message);
			//	close pfm
			fclose(File);
		}
	}

}

int CEOSSPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pCloseButton = new CColorButton();
	m_pCloseButton->SetBase(-1);
//	m_pCloseButton->SetPreNum("BBM: ");
	m_pCloseButton->SetPreNum("");

	m_pPauseButton		= new CButton();
	m_pAckFlagsButton	= new CButton();
	m_pPrintButton		= new CButton();
	m_pToClose			= new CButton();
	m_pImageList		= new CImageList();
	m_pImageList->Create( IDB_BMP_ALL, 18, 1, COLOR_GRAY);
	return 0;
}

void CEOSSPropertySheet::OnCloseButton()
{
	ShowWindow(SW_HIDE);
	KillTimer(1);
}

void CEOSSPropertySheet::SetTabColor(int Tab,COLORREF Color)
{
	if (Tab != IDC_EOSSDATA_TAB) return;
	int ColorIndex;
	int tab = 0;
	switch(Color) {
	case COLOR_RED:
		ColorIndex = 0;
		break;
	case COLOR_GREEN:
		ColorIndex = 1;
		break;
	case COLOR_BLUE:
		ColorIndex = 2;
		break;
	case COLOR_YELLOW:
		ColorIndex = 3;
		break;
	case COLOR_CYAN:
		ColorIndex = 4;
		break;
	case COLOR_MAGENTA:
		ColorIndex = 5;
		break;
	case COLOR_WHITE:
		ColorIndex = 6;
		break;
	case COLOR_BLACK:
		ColorIndex = 7;
		break;
	case COLOR_GREENRED:
		ColorIndex = 8;
		break;
	case COLOR_BLUERED:
		ColorIndex = 9;
		break;
	case COLOR_REDYELLOW: // jfl, note map of status 3 to image 10
		ColorIndex = 10;
		break;
	default:
		ColorIndex = 6;
	};
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = ColorIndex;
	switch (Tab) {
	case IDC_EOSSDATA_TAB:			tab = 1; break;
//	case IDC_BINARYEOSSDATA_TAB:   tab = 2; break;
//	case IDC_UGRANDEOSSDATA_TAB:	tab = 3; break;
//	case IDC_GPSDATA_TAB:			tab = 3; break;
//	case IDC_VACOSSDATA_TAB:		tab = 4; break;
	};
	GetTabControl()->SetItem(tab,&pTabCtrlItem);
	GetTabControl()->Invalidate();
}


void CEOSSPropertySheet::OnAckButton()
{
	m_dToClose = 0;

	//acknowledge the four button flags
	m_pParent->m_pButton->ShowIcon_Bell(VARIANT_FALSE);
	m_pParent->FlagComm(0,false);
	m_pParent->FlagOthr(0,false);
	m_pParent->FlagTime(0,false);

	m_pParent->m_pButton->ShowIcon_Disk(VARIANT_FALSE);
	m_pParent->m_pButton->put_Flash(VARIANT_FALSE);

	CEOSSSummary*  summarypage		= m_pParent->m_pSummaryPage;
	CEOSSDataStat* datapage		= m_pParent->m_pDataStatus;

	summarypage->ResetColors();
	datapage->ResetColors();
	
	SetTabColor(IDC_EOSSDATA_TAB,COLOR_GREEN);

	Data_Tab = 0x0000;
	
	m_pParent->OnAck();

	Invalidate();
}

void CEOSSPropertySheet::OnPauseButton()
{
	m_dToClose = 0;

//	CEOSSModParam* modparpage = m_pParent->m_pModParams;

	if (m_pParent->TogglePause())
	{
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
//		modparpage->GetDlgItem(IDC_EOSSMODPARAMS_DATETIME)->EnableWindow(TRUE);
	}
	else
	{
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
//		modparpage->GetDlgItem(IDC_EOSSMODPARAMS_DATETIME)->EnableWindow(FALSE);
	}
}

void CEOSSPropertySheet::IssuePrint(FILE* outfile)
{

	CString temp, temp1, temp2, temp3, temp4;

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");

	CEOSSSummary* summarypage		= m_pParent->m_pSummaryPage;
	CEOSSDataStat*	datapage		= m_pParent->m_pDataStatus;
//	CBinaryDataStat* bindatapage	= m_pParent->m_pBinaryDataStatus;
//	CGPSDataStat* gpsdatapage		= m_pParent->m_pGPSDataStatus;
//	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;
	CEOSSModParam* modparpage		= m_pParent->m_pModParams;


	fprintf(outfile,"* = Displayed in RED\n");
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"SUMMARY\n");

//	fprintf(outfile," Data Status\n");
//	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_OOMII,temp);
//	temp += (summarypage->StaticColors[IDC_EOSSSUMMARY_OOMII-IDC_EOSSSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
//	fprintf(outfile,"   Measurement Interval of Interest: %s\n",LPCTSTR(temp));

//	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_ACPOWER,temp);
//	temp += (summarypage->StaticColors[IDC_EOSSSUMMARY_ACPOWER-IDC_EOSSSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
//	fprintf(outfile,"                     External Power: %s\n",LPCTSTR(temp));

//	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_BATTERYOK,temp);
//	temp += (summarypage->StaticColors[IDC_EOSSSUMMARY_BATTERYOK-IDC_EOSSSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
//	fprintf(outfile,"                            Battery: %s\n",LPCTSTR(temp));

//	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_BBMOK,temp);
//	temp += (summarypage->StaticColors[IDC_EOSSSUMMARY_BBMOK-IDC_EOSSSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
//	fprintf(outfile,"           Battery Backed Up Memory: %s\n",LPCTSTR(temp));

//	summarypage->GetDlgItemText(IDC_EOSSSUMMARY_INQUIRETIME,temp);
//	temp += (summarypage->StaticColors[IDC_EOSSSUMMARY_INQUIRETIME-IDC_EOSSSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
//	fprintf(outfile,"  Last INQUIRE Status (Y.M.D H:M:S):\n  %s\n",LPCTSTR(temp));

	fprintf(outfile,"  Recent Commands and Responses:\n");

	for (int i = IDC_EOSSSUMMARY_COMMAND11; i <= IDC_EOSSSUMMARY_COMMAND1; i--)
	{
		summarypage->GetDlgItemText(i,temp);
		temp += (summarypage->StaticColors[i-IDC_EOSSSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
		fprintf(outfile,"  %s\n",LPCTSTR(temp));
		temp.Empty();
	}

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"DATA STATUS\n");
//	fprintf(outfile,"Most Recent INQUIRE2 Record\n");

//	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"MODIFY PARAMETERS -- Caution: These values may not have been applied!\n");
	fprintf(outfile," Communications\n");
	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_PORT,temp);
	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_NODE,temp1);
	fprintf(outfile,"  Port: %s\n  Node: %s\n\n",temp,temp1);
	fprintf(outfile," File Output\n");
	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_SAVELOC,temp);
	fprintf(outfile,"  Location: %s\n",temp);
	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_FILEID,temp);
	fprintf(outfile,"  Station ID: %s\n",temp);
//	if(((CButton*)modparpage->GetDlgItem(IDC_EOSSMODPARAMS_DODUMP))->GetCheck()==0)
//		temp = NO;
//	else
//		temp = YES;
//	fprintf(outfile,"  Do Dump File: %s\n",temp);
	fprintf(outfile," Error Limits\n");
//	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_SLOW_RESET,temp);
//	fprintf(outfile,"  Slow Reset (Sec.): %6s\n\n",temp);
//	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_TIME_ERR,temp);
//	fprintf(outfile,"  Time Delta (Sec.): %6s\n\n",temp);
	fprintf(outfile," General\n");
//	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_MAXCYCLE,temp);
//	fprintf(outfile,"     Message Cycle Time (mSec): %s\n",temp);
	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_MAXPAUSE,temp);
	fprintf(outfile,"      Maximum Pause Time (Sec): %s\n",temp);
//	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_MAXBBM,temp);
//	fprintf(outfile,"           Maximum BBM (Bytes): %s\n",temp);

//	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_COMMFAIL,temp);
//	fprintf(outfile,"  # of Retransmits Before Fail: %s\n",temp);

//	if(((CButton*)modparpage->GetDlgItem(IDC_CHECK_ALLOWAUTODATE))->GetCheck()==0)
//		temp = "No";
//	else
//		temp = "Yes";
//	fprintf(outfile,"    Allow Auto Date & Time Set: %s\n",temp);
	
//	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_MAXINQUIRE,temp);
//	fprintf(outfile,"          Inquire Every(units): %s\n",temp);
//	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_TIMEOUT,temp);
//	fprintf(outfile,"     Response Time-Out (units): %s\n",temp);
//	modparpage->GetDlgItemText(IDC_EOSSMODPARAMS_MAXSTATUS,temp);
//	fprintf(outfile,"     Take Status Every (units): %s\n",temp);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"CURRENT CONFIGURATION (from INI file)\n");
	char buff[32768];
	GetPrivateProfileSection(m_pParent->m_pName,buff,sizeof(buff),m_pParent->m_szIniFile);
	char* pAt = buff;
	while (strlen(pAt))
	{
		fprintf(outfile,"\t%s\n",pAt);
		pAt = pAt + strlen(pAt) + 1;
	}

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
}

void CEOSSPropertySheet::OnPrintButton()
{
	FILE* outfile;
	char *at;
	while ((at = strchr(m_pCaption,':'))!=NULL) at[0] = '_';

	m_dToClose = 0;

	CFileDialog FileDialog(FALSE,"txt",m_pCaption,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",this);

	if (FileDialog.DoModal()!=IDCANCEL)
	{
		CString filename = FileDialog.GetPathName();
		outfile = fopen(filename,"w");

		m_pParent->IssuePrint(outfile);

		fclose(outfile);
	}
}

void CEOSSPropertySheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 	CPropertySheet::OnShowWindow(bShow, nStatus);

	m_dToClose = 0;

	if (bShow && nStatus==0) 
	{
		SetActivePage(0);
		m_dToClose = 0;
		SetTimer(1,1000,NULL);
		CEOSSModParam* modparpage = m_pParent->m_pModParams;

//		if (m_pParent->IsPaused())
//			modparpage->GetDlgItem(IDC_EOSSMODPARAMS_DATETIME)->EnableWindow(true);
//		else
//			modparpage->GetDlgItem(IDC_EOSSMODPARAMS_DATETIME)->EnableWindow(false);
	}
	else
		KillTimer(1);
}

void CEOSSPropertySheet::OnToCloseButton()
{
	char temp[32];
	m_dToClose -= BUMP_EXTEND;  //Bump_extend = 300 seconds = 5 more minutes

    int seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;

	if (seconds_to_go > MAX_EXTEND) //MAX_EXTEND = 7200 = 120 minutes
	{
		m_dToClose   = (m_pParent->m_iDlgCloseMilliseconds/1000) - MAX_EXTEND;
		seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;
	}

	int minutes = seconds_to_go / 60;
	int seconds = seconds_to_go % 60;

	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	m_pToClose->SetWindowText(temp);
}

LRESULT CEOSSPropertySheet::OnExtendClose(WPARAM /*WParam*/, LPARAM /*LParam*/)
{
	m_dToClose = 0;
	return 0;
}

void CEOSSPropertySheet::OnTimer(UINT nIDEOSS) 
{
	if (nIDEOSS == 1)
	{
		if (m_dToClose >= m_pParent->m_iDlgCloseMilliseconds/1000)
		{
			m_dToClose = 0;
			ShowWindow(SW_HIDE);
			KillTimer(1);
		} 
		else
		{
			char temp[32];
			int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
			int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;
			sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
			m_pToClose->SetWindowText(temp);
			m_dToClose++;
		}
	}
	CPropertySheet::OnTimer(nIDEOSS);
}

void CEOSSPropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//watch for alt-f4 to try to close us -- don't let it
	if (nID == SC_CLOSE)
		ShowWindow(SW_HIDE);
	else
  		CPropertySheet::OnSysCommand(nID, lParam);
}

BOOL CEOSSPropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	CRect WindowRect;
	CRect FinalRect;
	GetWindowRect(&WindowRect);
	WindowRect.bottom += 60;
	MoveWindow(&WindowRect);
	int Interspace = 6;

	int ButtonWidth = (WindowRect.right - WindowRect.left)/5 - 6*Interspace;
	if (ButtonWidth < 100) 
		ButtonWidth = 116;
	else
		ButtonWidth = 157;

	FinalRect = WindowRect;
	ScreenToClient(FinalRect);
	int Y1 = FinalRect.bottom - 60;
	int Y2 = FinalRect.bottom - 10;

	DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE |
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER;

	m_pCloseButton->Create( "Close", dwStyle,
		  CRect(Interspace, Y1,
				Interspace + ButtonWidth,Y2),
		  this,
		  ID_CLOSE_BUTTON);

	m_pPauseButton->Create(COLLECTINGBEGIN, dwStyle,
		  CRect(2*Interspace + ButtonWidth,Y1,
				2*Interspace + 2*ButtonWidth,Y2),
		  this,
		  ID_PAUSE_BUTTON);

	m_pAckFlagsButton->Create("Acknowledge Flags",dwStyle,
		  CRect(3*Interspace + 2*ButtonWidth,Y1,
				3*Interspace + 3*ButtonWidth,Y2),
		  this,
		  ID_ACKFL_BUTTON);

	m_pPrintButton->Create("Snapshot Status\nTo File",dwStyle,
		  CRect(4*Interspace + 3*ButtonWidth,Y1,
				4*Interspace + 4*ButtonWidth,Y2),
		  this,
		  ID_PRINT_BUTTON);

	char temp[32];
	int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
	int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;
	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);

	m_pToClose->Create(temp,dwStyle,
		  CRect(5*Interspace + 4*ButtonWidth,Y1,
				5*Interspace + 5*ButtonWidth,Y2),
		this,
		ID_TOCLOSE);

	CMenu* pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		pSysMenu->DeleteMenu( SC_SIZE, MF_BYCOMMAND);
		pSysMenu->DeleteMenu( SC_MAXIMIZE, MF_BYCOMMAND);
	}
	
	GetTabControl( )->SetImageList(m_pImageList);
	GetTabControl( )->ModifyStyle(TCS_MULTILINE,TCS_SINGLELINE);
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(1,&pTabCtrlItem);

	return bResult;
}

BOOL CEOSSPropertySheet::DestroyWindow() 
{
	GetTabControl( )->SetImageList(NULL);
	return CPropertySheet::DestroyWindow();
}

bool CEOSSPropertySheet::EditEOSSList()
{
	bool bReturn;
	bReturn = m_pParent->EditEOSSList();
	//if true then do what ever is necessary at this level
	if (bReturn)
	{

	}
	return bReturn;
}

void CEOSSPropertySheet::RefreshDataDisplay()
{
		CEOSSDataStat*  datapage = m_pParent->m_pDataStatus;
		if (datapage)
			datapage->SetUpdateDataDisplay();
}

void CEOSSPropertySheet::ResetReaderCount()
{
	if (m_pParent)
		m_pParent->ResetReaderCount();
}

void CEOSSPropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)
		m_dToClose = set;
}

void CEOSSPropertySheet::ForceDateTime(void)
{
	m_dToClose = 0;
	m_pParent->PostMessage(uIDC_RECEIVE,FORCEDATETIME,CHECKSUM_GOOD);
}

CString *CEOSSPropertySheet::GetVersionString()
{
	return &m_pParent->m_csVersion;
}