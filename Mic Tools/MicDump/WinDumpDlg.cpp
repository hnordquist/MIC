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
// WinDumpDlg.cpp : implementation file
//

#include "stdafx.h"
#include <afxole.h>

#include "WinDump.h"
#include "WinDumpDlg.h"
#include "DlgProxy.h"
#include "JulianT.h"
#include "cgfiltyp.h"
#include <shlwapi.h>
#include "BIOSettings.h"
#include "CmdLine.h"
#include "Help.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BATCHMEBABY	(WM_APP+3)

#define SOH_GPS		0x01
#define SOH_BINARY	0x02
#define SOH_VACOSS	0x03
#define SOH_MCA		0x04
#define SOH_GRAND	0x05
#define SOH_PSR		0x06
#define SOH_JSR		0x07
#define SOH_ISR		0x08
#define SOH_MGD		0x09

extern void InitializeCHNFileVariables();
extern void CloseOutCHNFileVariables();
extern void DecodeCHNHeader(char *filename,FILE *infile,bool DoOutfile,FILE *outfile);
extern void DecodeOneCHNRecord(char *fn,FILE *infn,bool dout,FILE *outf);
extern void WriteCHNSummaryInformationToConsole(char *fn,FILE *infn,bool dout,FILE *outf);
extern void WriteCHNSummaryInformationToOutputFile(char *fn,FILE *infn,bool dout,FILE *outf);

extern bool g_bGPS_OldStyle;
extern void InitializeGPSFileVariables();
extern void CloseOutGPSFileVariables();
extern void DecodeGPSHeader(char *filename,FILE *infile,bool DoOutfile,FILE *outfile);
extern void DecodeOneGPSRecord(char *fn,FILE *infn,bool dout,FILE *outf);
extern void DecodeOneOldStyleGPSRecord(char *fn,FILE *infn,bool dout,FILE *outf);
extern void WriteGPSSummaryInformationToConsole(char *fn,FILE *infn,bool dout,FILE *outf);
extern void WriteGPSSummaryInformationToOutputFile(char *fn,FILE *infn,bool dout,FILE *outf);

extern void InitializeBinaryFileVariables();
extern void CloseOutBinaryFileVariables();
extern void DecodeBinaryHeader(char *filename,FILE *infile,bool DoOutfile,FILE *outfile);
extern void DecodeOneBinaryRecord(char *fn,FILE *infn,bool dout,FILE *outf);
extern void WriteBinarySummaryInformationToConsole(char *fn,FILE *infn,bool dout,FILE *outf);
extern void WriteBinarySummaryInformationToOutputFile(char *fn,FILE *infn,bool dout,FILE *outf);

extern void InitializeVACOSSFileVariables();
extern void CloseOutVACOSSFileVariables();
extern void DecodeVACOSSHeader(char *filename,FILE *infile,bool DoOutfile,FILE *outfile);
extern void DecodeOneVACOSSRecord(char *fn,FILE *infn,bool dout,FILE *outf);
extern void WriteVACOSSSummaryInformationToConsole(char *fn,FILE *infn,bool dout,FILE *outf);
extern void WriteVACOSSSummaryInformationToOutputFile(char *fn,FILE *infn,bool dout,FILE *outf);

//pack structure at the byte level
#pragma pack(1)
struct AcquireRecord
{
	unsigned int JulianSeconds;	//4 bytes
	short	StatusByte;		//2 bytes
	float	NeutronChA;		//4 bytes
	float	NeutronChB;		//4 bytes
	float	NeutronChC;		//4 bytes
	float	GammaCh1;		//4 bytes
	float	GammaCh1Sigma;	//4 bytes
	float	GammaCh2;		//4 bytes
	float	GammaCh2Sigma;	//4 bytes
	short	CountTime;		//2 bytes
};

struct RabbitRecord
{
	unsigned int JulianSeconds;	//4 bytes
	short	StatusByte;		//2 bytes
	unsigned int uiCounts[8];	//4 bytes * 8 = 32 bytes
	short	CountTime;		//2 bytes
};

struct RFID_Rec{
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	unsigned short usNode;
	unsigned int bRFIDSerialNumber[12];
	BYTE bCount;
	BYTE bChecksum;
};
#define RFIDDATA_REC_LENGTH (sizeof(RFID_Rec))


struct AcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	unused2[47];
	//char FullYear[4];
	//char unused2[43];
};

struct ChkSumBeef {
	unsigned short usChkSum;
	unsigned short usBeef;
};

//pack structure at the byte level
struct MCAAcquireRecord
{
	unsigned int JulianSeconds;	//4 bytes
	unsigned char	StatusByte1;	//1 byte
	unsigned char	StatusByte2;	//1 byte
	unsigned char	Alarm;			//1 byte
	float	GrossCounts;	//4 bytes
	float	U235;			//4 bytes
	float	CS137;			//4 bytes
	float	U238;			//4 bytes
	float	Scaler;			//4 bytes
	float	U235Ratio;		//4 bytes
	float	CS137Ratio;		//4 bytes
	int		ElapsedTime;	//4 bytes
};
//pack structure at the byte level
#pragma pack(1)
struct MCAAcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	unused2[47];
// or replace unused2 with...
//	char	Year4[4];
//	char	unused2[43];
};

#pragma pack(2)
struct ISRAcquireRecord
{
	unsigned int	JulianSeconds;	//4 bytes
	unsigned short	StatusByte;		//2 bytes
	union {
	__int64			TotalsCount;	//8 bytes
	double			fTotalsCount;
	};
	union {
	__int64			Totals2Count;	//8 bytes
	double			fTotals2Count;
	};
	union {
	__int64			Totals3Count;	//8 bytes
	double			fTotals3Count;
	};
	union {
	__int64			RealAccCount;	//8 bytes
	double			fRealAccCount;
	};
	union {
	__int64			AccidenCount;	//8 bytes
	double			fAccidenCount;
	};
	union {
	__int64			ElapsedTime;	//8 bytes 1/10 sec increments
	double			fElapsedTime;
	};
};

#pragma pack(1)
struct ISRAcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	unused2[47];
// or replace unused2 with...
//	char	Year4[4];
//	char	unused2[43];
};

#pragma pack()

bool CWinDumpDlg::Unattended(unsigned short usCode)
{
	bool bLoggedOff = true;
	if ((usCode > 0) && (usCode < 19683))
		bLoggedOff = false;
	else if ((usCode > 20480) && (usCode < 40163))
		bLoggedOff = false;
	else if (usCode == 40960)
		bLoggedOff = true;
	return (bLoggedOff && (usCode != 40960) && (usCode != 41024));
}


void CWinDumpDlg::EossToSystemTime(SYSTEMTIME * pSystemTime, EOSSTIME * pEossTime)
{
	SYSTEMTIME SystemTime;
	
	SystemTime.wYear = 2000;
	SystemTime.wMonth = 1;
	SystemTime.wDay = 1;
	SystemTime.wHour = 0;
	SystemTime.wMinute = 0;
	SystemTime.wSecond = 0;
	SystemTime.wMilliseconds = 0;

	ULONGLONG FileTime;
	
	SystemTimeToFileTime(&SystemTime, (LPFILETIME)&FileTime);

	ULONGLONG EossTime = pEossTime->HalfDays * 43200 + (43200 - pEossTime->Seconds);

	FileTime += (EossTime * 10000000);
	
	FileTimeToSystemTime((LPFILETIME)&FileTime, pSystemTime);
}

CString CWinDumpDlg::GetEossMessage(unsigned short usCode)
{
	CString cString;
	char szTemp[64];
	if (usCode > 0 && usCode < 19683)
	{
		UnMakeID(usCode,szTemp);
		cString = szTemp;
		cString += " logged on as operator 1";
	}
	else if (usCode > 20480 && usCode < 40163)
	{
		UnMakeID(usCode - 20480,szTemp);
		cString = szTemp;
		cString += " logged on as operator 2";
	}
	else if (usCode == 40960)
		cString = "Operator(s) logged off.";
	else if (usCode == 40976) cString = "Date and time set.";
	else if (usCode == 40977) cString = "Date and time set.";
	else if (usCode == 40992) cString = "Battery compartment opened";
	else if (usCode == 40993) cString = "Battery compartment closed";
	else if (usCode == 41008) cString = "Seal tampered with. Key-set erased";
	else if (usCode == 41024) cString = "Access denied. Authorization failed";
	else if (usCode == 41040) cString = "New key-set generated";
	else if (usCode == 41056) cString = "Reset";
	else if (usCode == 41072) cString = "Key-set erased on operator request";
	else if (usCode == 41073) cString = "Temperator out of range. Key-set erased";
	else if (usCode == 41088) cString = "Entering deep sleep mode";
	if (Unattended(usCode)) cString += " WHILE NOT LOGGED IN";
	return cString;
}

void CWinDumpDlg::UnMakeID(unsigned short Id, char * szOperator)
{
	  szOperator[0] = NULL;
	  unsigned char Digit;
	  CString szId;

      while (Id) {
         Digit = Id % 27;
         if (Digit)
            szId = (char)(Digit + 64) + szId;
         else
            szId = " " + szId;
         Id = Id / 27;
      }
      strcpy(szOperator, szId);
}

bool CWinDumpDlg::ReadEOSSTable(FILE* infile, FILE* outfile)
{
	fprintf(outfile,"READ EOSS TABLE\n");
	bool bResult = false;
	if (fread(m_EOSSTable,sizeof(m_EOSSTable),1,infile) == 1)
	{
		//perform any further EOSS table analysis here
		bResult = true;
		int iIndex = 0;
		fprintf(outfile," - ");
		while ((m_EOSSTable[iIndex] != 0) && (iIndex < 32))
		{
			fprintf(outfile,"%d ",m_EOSSTable[iIndex]);
			iIndex++;
		}
		fprintf(outfile,"\n");
	}
	else
		fprintf(outfile,"%s\n",(feof(infile))?"END OF FILE":"READ ERROR");
	return bResult;
}

bool CWinDumpDlg::ReadStatusRec(FILE* infile, FILE* outfile)
{
	fprintf(outfile,"READ STATUS RECORD\n");
	bool bResult = false;
	if (fread(&m_StatusRec,sizeof(m_StatusRec),1,infile) == 1)
		bResult = !(memcmp(&m_StatusRec,"FAIL",4) == 0);
	//perform any further status rec analysis here
	if (bResult)
	{
		fprintf(outfile," - Seal line was %s during latest read\n",(m_StatusRec.Flags & FLAG_FOOPEN)?"OPEN":"CLOSED");
		fprintf(outfile," - Fiber optic %s\n",(m_StatusRec.Flags & FLAG_FOFAIL)?"FAILURE":"OK");
		fprintf(outfile," - Drill detection %s\n",(m_StatusRec.Flags & FLAG_HOPEN)?"OPEN":"NOT OPEN");
		fprintf(outfile," - Drill detection %s\n",(m_StatusRec.Flags & FLAG_SHORT)?"SHORTED":"NOT SHORTED");
		fprintf(outfile," - Normal battery consumed(uAH) %d\n",m_StatusRec.BatteryConsumed);
		fprintf(outfile," - Emergency battery consumed(uAH) %d\n",m_StatusRec.EBatteryConsumed);
		fprintf(outfile," - Flash memory warnings %d\n",m_StatusRec.FlashWarnings);
		fprintf(outfile," - Flash memory errors %d\n",m_StatusRec.FlashErrors);
		fprintf(outfile," - Majority vote memory warnings %d\n",m_StatusRec.MajoWarnings);
		fprintf(outfile," - Majority vote memory errors %d\n",m_StatusRec.MajoErrors);
	}
	else
		fprintf(outfile," - FAILED\n");

	return bResult;
}

bool CWinDumpDlg::ReadTimeRec(FILE* infile, FILE* outfile)
{
	fprintf(outfile,"READ TIME RECORD\n");
	bool bResult = false;
	bResult = (fread(&m_TimeRec,sizeof(m_TimeRec),1,infile) == 1)?true:false;
	//perform any further time rec analysis here
	COleDateTime cTime;
	cTime.m_dt = m_TimeRec.dt;
	cTime.m_status = m_TimeRec.status;
	fprintf(outfile," - %s %d\n",cTime.Format("%Y.%m.%d %H:%M:%S"),m_TimeRec.sOffset);
	fprintf(outfile," - Version: %d.%d\n",m_TimeRec.cVersion[0],m_TimeRec.cVersion[1]);
	return bResult;
}

bool CWinDumpDlg::ReadLogInfoRec(FILE* infile, FILE* outfile)
{
	fprintf(outfile,"READ LOGINFO RECORD\n");
	bool bResult = false;
	if (fread(&m_LogInfoRec,sizeof(m_LogInfoRec),1,infile) == 1)
		bResult = !(memcmp(&m_LogInfoRec,"FAIL",4) == 0);
	//perform any further LOGINFO page analysis here
	if (bResult)
	{
		fprintf(outfile," - Seal: Events:%d First:%d Last:%d Start:%d Curr:%d\n",
			m_LogInfoRec.SealEvents,m_LogInfoRec.SealLogFirstPage,
			m_LogInfoRec.SealLogLastPage,m_LogInfoRec.SealLogStartPage,
			m_LogInfoRec.SealLogCurrentPage);

		fprintf(outfile," - System: Events:%d First:%d Last:%d Start:%d Curr:%d\n",
			m_LogInfoRec.SystemEvents,m_LogInfoRec.SystemLogFirstPage,
			m_LogInfoRec.SystemLogLastPage,m_LogInfoRec.SystemLogStartPage,
			m_LogInfoRec.SystemLogCurrentPage);

		fprintf(outfile," - SoH: Records:%d First:%d Last:%d Start:%d Curr:%d\n",
			m_LogInfoRec.SoHRecords,m_LogInfoRec.SoHLogFirstPage,
			m_LogInfoRec.SoHLogLastPage,m_LogInfoRec.SoHLogStartPage,
			m_LogInfoRec.SoHLogCurrentPage);
	}
	else
		fprintf(outfile,"FAILED\n");
	return bResult;
}

bool CWinDumpDlg::ReadSealLogPage(FILE* infile, FILE* outfile)
{
	fprintf(outfile,"\tREAD SEAL LOG PAGE\n");
	bool bResult = false;
	if (fread(&m_SealLogPage,sizeof(m_SealLogPage),1,infile) == 1)
		bResult = !(memcmp(&m_SealLogPage,"FAIL",4) == 0);

	//perform any further SEAL log page analysis here
	if (m_SealLogPage.Header.Log != 70)
		bResult = false;

	if (bResult)
	{
		fprintf(outfile,"\t - Seal\t\t%u\n",m_SealLogPage.Header.Seal);
		fprintf(outfile,"\t - Log\t\t%u\n",m_SealLogPage.Header.Log);
		fprintf(outfile,"\t - Page\t\t%u\n",m_SealLogPage.Header.Page);
		
		SYSTEMTIME sTime;
		EossToSystemTime(&sTime, &m_SealLogPage.Header.Time);
		fprintf(outfile,"\t - Time\t\t%u\\%02u\\%02u %02u:%02u:%02u\n",
			sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);

		fprintf(outfile,"\t - Authenticator\t0x%02X%02X%02X%02X%02X%02X%02X%02X\n\n",
			m_SealLogPage.Header.Authenticator[0],
			m_SealLogPage.Header.Authenticator[1],
			m_SealLogPage.Header.Authenticator[2],
			m_SealLogPage.Header.Authenticator[3],
			m_SealLogPage.Header.Authenticator[4],
			m_SealLogPage.Header.Authenticator[5],
			m_SealLogPage.Header.Authenticator[6],
			m_SealLogPage.Header.Authenticator[7]);
	}
	else
		fprintf(outfile,"\t - FAILED\n\n");

	return bResult;
}

bool CWinDumpDlg::ReadSystemLogPage(FILE* infile, FILE* outfile)
{
	fprintf(outfile,"\tREAD SYSTEM LOG PAGE\n");
	bool bResult = false;
	if (fread(&m_SystemLogPage,sizeof(m_SystemLogPage),1,infile) == 1)
		bResult = !(memcmp(&m_SystemLogPage,"FAIL",4) == 0);

	//perform any further SYSTEM log page analysis here
	if (m_SystemLogPage.Header.Log != 83) 
		bResult = false;

	if (bResult)
	{
		fprintf(outfile,"\t - Seal\t\t%u\n",m_SystemLogPage.Header.Seal);
			
		fprintf(outfile,"\t - Log\t\t%u\n",m_SystemLogPage.Header.Log);
			
		fprintf(outfile,"\t - Page\t\t%u\n",m_SystemLogPage.Header.Page);
			
		SYSTEMTIME sTime;
		EossToSystemTime(&sTime, &m_SystemLogPage.Header.Time);
		fprintf(outfile,"\t - Time\t\t%u\\%02u\\%02u %02u:%02u:%02u\n",
			sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);

		fprintf(outfile,"\t - Authenticator\t0x%02X%02X%02X%02X%02X%02X%02X%02X\n\n",
			m_SystemLogPage.Header.Authenticator[0],
			m_SystemLogPage.Header.Authenticator[1],
			m_SystemLogPage.Header.Authenticator[2],
			m_SystemLogPage.Header.Authenticator[3],
			m_SystemLogPage.Header.Authenticator[4],
			m_SystemLogPage.Header.Authenticator[5],
			m_SystemLogPage.Header.Authenticator[6],
			m_SystemLogPage.Header.Authenticator[7]);
	}
	else
		fprintf(outfile,"\t - FAILED\n\n");

	return bResult;
}

bool CWinDumpDlg::ReadSOHLogPage(FILE* infile, FILE* outfile)
{
	fprintf(outfile,"\tREAD SOH LOG PAGE\n");
	bool bResult = false;
	if (fread(&m_SOHLogPage,sizeof(m_SOHLogPage),1,infile) == 1)
		bResult = !(memcmp(&m_SOHLogPage,"FAIL",4) == 0);

	//perform any further SOH log page analysis here
	if (m_SOHLogPage.Header.Log != 72) 
		bResult = false;


	if (bResult)
	{
		fprintf(outfile,"\t - Seal\t\t%u\n",m_SOHLogPage.Header.Seal);

		fprintf(outfile,"\t - Log\t\t%u\n",m_SOHLogPage.Header.Log);
	
		fprintf(outfile,"\t - Page\t\t%u\n",m_SOHLogPage.Header.Page);
	
		SYSTEMTIME sTime;
		EossToSystemTime(&sTime, &m_SOHLogPage.Header.Time);
		fprintf(outfile,"\t - Time\t\t%u\\%02u\\%02u %02u:%02u:%02u\n",
			sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);

		fprintf(outfile,"\t - Authenticator\t0x%02X%02X%02X%02X%02X%02X%02X%02X\n\n",
			m_SOHLogPage.Header.Authenticator[0],
			m_SOHLogPage.Header.Authenticator[1],
			m_SOHLogPage.Header.Authenticator[2],
			m_SOHLogPage.Header.Authenticator[3],
			m_SOHLogPage.Header.Authenticator[4],
			m_SOHLogPage.Header.Authenticator[5],
			m_SOHLogPage.Header.Authenticator[6],
			m_SOHLogPage.Header.Authenticator[7]);
	}
	else
		fprintf(outfile,"\t - FAILED\n\n");

	return bResult;
}

void CWinDumpDlg::ProcessSealEvent(int iEventNumber, SEALEVENT* pSealEvent, FILE* infile, FILE* outfile)
{
	fprintf(outfile,"\t\tPROCESS SEAL EVENT ");

	SYSTEMTIME sTime;
	EossToSystemTime(&sTime, pSealEvent);
	fprintf(outfile,"\t - %s\t%04u\\%02u\\%02u %02u:%02u:%02u\n",(iEventNumber&1)?"Closed":"Opened",
		sTime.wYear,sTime.wMonth,sTime.wDay,
		sTime.wHour,sTime.wMinute,sTime.wSecond);
}

void CWinDumpDlg::ProcessSystemEvent(SYSTEMEVENT* pSystemEvent, FILE* infile, FILE* outfile)
{
	fprintf(outfile,"\t\tPROCESS SYSTEM EVENT\t");

	SYSTEMTIME sTime;
	EossToSystemTime(&sTime, &pSystemEvent->Time);
	fprintf(outfile," -  %04u\\%02u\\%02u %02u:%02u:%02u  %s\n",
		sTime.wYear,sTime.wMonth,sTime.wDay,
		sTime.wHour,sTime.wMinute,sTime.wSecond,
		GetEossMessage(pSystemEvent->Code));

}

void CWinDumpDlg::ProcessSOHRecord(SOHRECORD* pSOHRecord, FILE* infile, FILE* outfile)
{
	fprintf(outfile,"\t\tPROCESS SOH RECORD\t");

	SYSTEMTIME sTime;
	EOSSTIME eTime;
	eTime.HalfDays = pSOHRecord->Date;
	eTime.Seconds = 0;
	EossToSystemTime(&sTime, &eTime);
	fprintf(outfile," - %04u\\%02u\\%02u   %3d°C %3d°C   %3.1fv %3.1fv   %4u min. %3u MVEvents\n",
		sTime.wYear,sTime.wMonth,sTime.wDay,
		pSOHRecord->MaxTemperature,
		pSOHRecord->MinTemperature,
		(float)pSOHRecord->EmergencyBattery/10.0,
		(float)pSOHRecord->MainBattery/10.0,
		pSOHRecord->OnBatteries*12,
		pSOHRecord->MVEvents);
}

void CWinDumpDlg::ReadAndPrintESBAcqRecord(BinaryRecordX* pXRecord,FILE*infile,FILE*outfile, int* piRecordCount) 
{

	//read and print record ESB
	if (fread(&pXRecord->bBytes,1,pXRecord->bLength-2,infile))
	{
		ChkSumBeef XChkSumBeef;
		fread(&XChkSumBeef,sizeof(ChkSumBeef),1,infile);
		
		unsigned short usCheckSum = 0;
		 
		for (int i = 0; i < pXRecord->bLength-2; i++)
			usCheckSum += pXRecord->bBytes[i];
		char szCheck[16];
		char szBeef[16];
		strcpy(szCheck,(usCheckSum == XChkSumBeef.usChkSum)?"ChkSum:Ok  ":"ChkSum:Fail");
		strcpy(szBeef,(0xbeef == XChkSumBeef.usBeef)?"EOR:Ok":"EOR:Fail");

		*piRecordCount++;
		CJulianTime cJulianTime;
		cJulianTime.Init(pXRecord->ESBAcquireRec.uiJulianSeconds);

		fprintf(outfile,"%02x %s %s.%c %10u.%c "
			"0x%02x 0x%02x 0x%02x 0x%02x "
			"%4d:0x%03x %4d:0x%1x "
			"%4d:0x%03x "
			"%4d:0x%03x %4d:0x%03x %4d:0x%03x "
			"%4d:0x%03x "
			"%+6.1f "
			"%+6.1f "
			"%8.1f "
			"%02x%02x%02x%02x%02x%02x "
			"%s %s\n",
			pXRecord->bOpcode,
			cJulianTime.Date(),	
			cJulianTime.Time(),
			pXRecord->ESBAcquireRec.ucJulianSec10ths + '0',
			pXRecord->ESBAcquireRec.uiJulianSeconds,
			pXRecord->ESBAcquireRec.ucJulianSec10ths + '0',
			pXRecord->ESBAcquireRec.bStatusByte0,pXRecord->ESBAcquireRec.bStatusByte1,
			pXRecord->ESBAcquireRec.bStatusByte2,pXRecord->ESBAcquireRec.bStatusByte3,
			pXRecord->ESBAcquireRec.usAccelerometer,
			pXRecord->ESBAcquireRec.usAccelerometer,
			pXRecord->ESBAcquireRec.ubHumidity,
			pXRecord->ESBAcquireRec.ubHumidity,
			pXRecord->ESBAcquireRec.usLightInt,
			pXRecord->ESBAcquireRec.usLightInt,
			pXRecord->ESBAcquireRec.usMagnetometerX,
			pXRecord->ESBAcquireRec.usMagnetometerX,
			pXRecord->ESBAcquireRec.usMagnetometerY,
			pXRecord->ESBAcquireRec.usMagnetometerY,
			pXRecord->ESBAcquireRec.usMagnetometerZ,
			pXRecord->ESBAcquireRec.usMagnetometerZ,
			pXRecord->ESBAcquireRec.usSurfWetness,
			pXRecord->ESBAcquireRec.usSurfWetness,
			pXRecord->ESBAcquireRec.fCPUTemp,
			pXRecord->ESBAcquireRec.fBOARDTemp,
			((float)pXRecord->ESBAcquireRec.uiCountTime10ths)/10.0,
			pXRecord->ESBAcquireRec.bESBSerialNumber[0],
			pXRecord->ESBAcquireRec.bESBSerialNumber[1],
			pXRecord->ESBAcquireRec.bESBSerialNumber[2],
			pXRecord->ESBAcquireRec.bESBSerialNumber[3],
			pXRecord->ESBAcquireRec.bESBSerialNumber[4],
			pXRecord->ESBAcquireRec.bESBSerialNumber[5],
			szCheck, szBeef						
			);
	}
}
void CWinDumpDlg::ReadAndPrintRADAcqRecord(BinaryRecordX* pXRecord,FILE*infile,FILE*outfile, int* piRecordCount) 
{
	//read and print record type 1
	if (fread(&pXRecord->bBytes,1,pXRecord->bLength-2,infile))
	{
		ChkSumBeef XChkSumBeef;
		fread(&XChkSumBeef,sizeof(ChkSumBeef),1,infile);

		*piRecordCount++;
		
		unsigned short usCheckSum = 0;
		for (int i = 0; i < pXRecord->bLength-2; i++)
			usCheckSum += pXRecord->bBytes[i];
		char szCheck[16];
		char szBeef[16];
		strcpy(szCheck,(usCheckSum != XChkSumBeef.usChkSum)?"ChkSum:Fail":"ChkSum:Ok  ");
		strcpy(szBeef,(0xbeef != XChkSumBeef.usBeef)?"EOR:Fail":"EOR:Ok");

		CJulianTime cJulianTime;
		cJulianTime.Init(pXRecord->RADAcquireRec.uiJulianSeconds);
		
		fprintf(outfile,"%02x %s %s.%d %10u.%d %02x %02x %02x %02x %1x %02x %12g %12g %12g %02x %12g %12g %12g %02x %12g %12g %12g %02x %12g %12g %12g %+6.1f %+6.1f %8.1f %s %s\n",
			pXRecord->bOpcode,
			cJulianTime.Date(),
			cJulianTime.Time(),
			pXRecord->RADAcquireRec.ucJulianSec10ths,
			pXRecord->RADAcquireRec.uiJulianSeconds,
			pXRecord->RADAcquireRec.ucJulianSec10ths,
			pXRecord->RADAcquireRec.bStatusByte0,
			pXRecord->RADAcquireRec.bStatusByte1,
			pXRecord->RADAcquireRec.bStatusByte2,
			pXRecord->RADAcquireRec.bStatusByte3,
			pXRecord->RADAcquireRec.bPPInputState,
			pXRecord->RADAcquireRec.bCh0Status,
			pXRecord->RADAcquireRec.fCh0CountRate,
			pXRecord->RADAcquireRec.fCh0ShortCountRate,
			pXRecord->RADAcquireRec.fCh0LongCountRate,
			pXRecord->RADAcquireRec.bCh1Status,
			pXRecord->RADAcquireRec.fCh1CountRate,
			pXRecord->RADAcquireRec.fCh1ShortCountRate,
			pXRecord->RADAcquireRec.fCh1LongCountRate,
			pXRecord->RADAcquireRec.bCh2Status,
			pXRecord->RADAcquireRec.fCh2CountRate,
			pXRecord->RADAcquireRec.fCh2ShortCountRate,
			pXRecord->RADAcquireRec.fCh2LongCountRate,
			pXRecord->RADAcquireRec.bCh3Status,
			pXRecord->RADAcquireRec.fCh3CountRate,
			pXRecord->RADAcquireRec.fCh3ShortCountRate,
			pXRecord->RADAcquireRec.fCh3LongCountRate,
			pXRecord->RADAcquireRec.fCPUTemp,
			pXRecord->RADAcquireRec.fBOARDTemp,
			((float)pXRecord->RADAcquireRec.uiCountTime10ths)/10.0,
			szCheck,szBeef);
	}
}

void CWinDumpDlg::DecodeOneESSRecordSet(char* /*FileName*/,FILE* infile,bool DoOutfile,FILE* outfile)
{
	while (ReadEOSSTable(infile, outfile)) //this checks for eof
	{
		unsigned uiTableIndex = 0;
		while ((m_EOSSTable[uiTableIndex] != 0) && (uiTableIndex < MAXNUMEOSS) && (m_EOSSTable[uiTableIndex] < 99999))
		{
			int iCount = 0;
			fprintf(outfile,"\nPROCESSING %d %d\n",m_EOSSTable[uiTableIndex], uiTableIndex);
			if (ReadStatusRec(infile, outfile))
			{
				if (!ReadTimeRec(infile, outfile)) continue;
				if (ReadLogInfoRec(infile, outfile))
				{
					//read seal log pages
					iCount = m_LogInfoRec.SealEvents;
					int iMaxCount = (m_LogInfoRec.SealLogLastPage - m_LogInfoRec.SealLogFirstPage + 1)*SEALEVENTS_PER_PAGE;
					if (iCount > iMaxCount)
					{
						iCount = iMaxCount - SEALEVENTS_PER_PAGE + iCount % SEALEVENTS_PER_PAGE;
						if (iCount % SEALEVENTS_PER_PAGE == 0)
							iCount += SEALEVENTS_PER_PAGE;
					}
					while (iCount > 0)
					{
						if (ReadSealLogPage(infile, outfile))
						{
							for (int iRecordIndex = 0; (iRecordIndex < SEALEVENTS_PER_PAGE) && (iCount > 0); iRecordIndex++, iCount--)
							{
								fprintf(outfile,"%d %d",iCount,iRecordIndex);
								ProcessSealEvent(iRecordIndex,&m_SealLogPage.Entries[iRecordIndex],infile, outfile);
							}
						}
						else
						{
							for (int iRecordIndex = 0; (iRecordIndex < SEALEVENTS_PER_PAGE) && (iCount > 0); iRecordIndex++, iCount--)
							{
								fprintf(outfile,"%d %d\tNOT PROCESSED",iCount,iRecordIndex);
							}
						}
					}
					if (m_SealLogPage.Header.Page != m_LogInfoRec.SealLogCurrentPage)
						fprintf(outfile,"ERROR - Didn't end on current page!\n");

					//read system log pages (nearly identical to above)
					iCount = m_LogInfoRec.SystemEvents;
					iMaxCount = (m_LogInfoRec.SystemLogLastPage - m_LogInfoRec.SystemLogFirstPage + 1)*SYSTEMEVENTS_PER_PAGE;
					if (iCount > iMaxCount)
					{
						iCount = iMaxCount - SYSTEMEVENTS_PER_PAGE + iCount % SYSTEMEVENTS_PER_PAGE;
						if (iCount % SYSTEMEVENTS_PER_PAGE == 0)
							iCount += SYSTEMEVENTS_PER_PAGE;
					}
					while (iCount > 0)
					{
						if (ReadSystemLogPage(infile, outfile))
						{
							for (int iRecordIndex = 0; (iRecordIndex < SYSTEMEVENTS_PER_PAGE) && (iCount > 0); iRecordIndex++, iCount--)
							{
								fprintf(outfile,"%d %d",iCount,iRecordIndex);
								ProcessSystemEvent(&m_SystemLogPage.Entries[iRecordIndex],infile, outfile);
							}
						}
						else
						{
							for (int iRecordIndex = 0; (iRecordIndex < SYSTEMEVENTS_PER_PAGE) && (iCount > 0); iRecordIndex++, iCount--)
							{
								fprintf(outfile,"%d %d\tNOT PROCESSED ",iCount,iRecordIndex);
							}
						}
					}
					if (m_SystemLogPage.Header.Page != m_LogInfoRec.SystemLogCurrentPage)
						fprintf(outfile,"ERROR - Didn't end on current page!\n");

					//read soh log pages (nearly identical to above)
					iCount = m_LogInfoRec.SoHRecords+1;//note the +1 here -- apparent error in firmware?
					iMaxCount = (m_LogInfoRec.SoHLogLastPage - m_LogInfoRec.SoHLogFirstPage + 1)*SOHRECORDS_PER_PAGE;
					if (iCount > iMaxCount)
					{
						iCount = iMaxCount - SOHRECORDS_PER_PAGE + iCount % SOHRECORDS_PER_PAGE;
						if (iCount % SOHRECORDS_PER_PAGE == 0)
							iCount += SOHRECORDS_PER_PAGE;
					}
					while (iCount > 0)
					{
						if (ReadSOHLogPage(infile, outfile))
						{
							for (int iRecordIndex = 0; (iRecordIndex < SOHRECORDS_PER_PAGE) && (iCount > 0); iRecordIndex++, iCount--)
							{
								fprintf(outfile,"%d %d",iCount,iRecordIndex);
								ProcessSOHRecord(&m_SOHLogPage.Entries[iRecordIndex],infile, outfile);
							}
						}
						else
						{
							for (int iRecordIndex = 0; (iRecordIndex < SOHRECORDS_PER_PAGE) && (iCount > 0); iRecordIndex++, iCount--)
							{
								fprintf(outfile,"%d %d\tNOT PROCESSED\n",iCount,iRecordIndex);
							}
						}
					}
					if (m_SOHLogPage.Header.Page != m_LogInfoRec.SoHLogCurrentPage)
						fprintf(outfile,"ERROR - Didn't end on current page!\n");
				}
			}		
			uiTableIndex++;
		}
	}
}


void CWinDumpDlg::DecodeOneMGDRecord(char* /*FileName*/,FILE *infile,bool DoOutfile,FILE *outfile,int*piRecordCount)
{
//TRACE("In DecodeOneMGDRecord\r\n");
	BinaryRecordX XRecord;
	ChkSumBeef XChkSumBeef;

	unsigned int PreviousJulian = 0;
	unsigned int PreviousCountTime = 0;
	int iLength, iOpcode;
	if ((iLength = fgetc(infile)) != EOF)
	{
		XRecord.bLength = (BYTE)iLength; //fgetc returns an int or a char
		if ((iOpcode = fgetc(infile)) != EOF)
		{
			XRecord.bOpcode = (BYTE)iOpcode;
			if (XRecord.bLength == sizeof(RADAcquireRecord) && XRecord.bOpcode == RADACQUIRE_OPCODE)
				ReadAndPrintRADAcqRecord(&XRecord,infile,outfile,piRecordCount);
			else if (XRecord.bLength == sizeof(ESBAcquireRecord) && XRecord.bOpcode == ESBACQUIRE_OPCODE)
				ReadAndPrintESBAcqRecord(&XRecord,infile,outfile,piRecordCount);
			else 
			{
				unsigned short usCheckSum = 0;
				BYTE bChar;
				//read and convert for display iLength bytes
				fprintf(outfile,"Unknown: %02x %02x ",XRecord.bLength,XRecord.bOpcode);

				for (int i = 2; i < XRecord.bLength; i++)
				{
					fprintf(outfile,"%02x ",bChar = fgetc(infile));
					usCheckSum += bChar;
				}

				fread(&XChkSumBeef,sizeof(ChkSumBeef),1,infile);

				fprintf(outfile,"%s ",(usCheckSum != XChkSumBeef.usChkSum)?"ChkSum:Fail":"ChkSum:Ok  ");
				fprintf(outfile,"%s",(0xbeef != XChkSumBeef.usBeef)?"EOR:Fail":"EOR:Ok");
				fprintf(outfile,"\n");
			}
		}
	}
}

void CWinDumpDlg::GeneralHexDump(char* FileName, FILE* infile, bool bDoOutFile, FILE* outfile, int *iRecordCount)
{
	fprintf(outfile,"Unknown file type or bad header in SOH file: %s.\n",FileName);
	fpos_t pos;
	pos = 0;
	fsetpos(infile, &pos);
	int iCh;
	int iCharCount = 0;
	char szLine1[64];
	char szLine2[64];
	char szTemp[8];
	szLine1[0] = NULL;
	szLine2[0] = NULL;
	while (!feof(infile))
	{
		if ((iCh = fgetc(infile)) != EOF)
		{
			iCharCount++;
			sprintf(szTemp,"%02x ",iCh);
			strcat(szLine1,szTemp);
			if (iCharCount == 8)
				strcat(szLine1," ");
			if (isprint(iCh))
				sprintf(szTemp,"%c",iCh);
			else
				sprintf(szTemp,".");
			strcat(szLine2,szTemp);
		}
		if (feof(infile) || (iCharCount == 16))
		{
			while (strlen(szLine1) < 49) strcat(szLine1," ");
			fprintf(outfile,"%s %s\n",szLine1,szLine2);
			szLine1[0] = NULL;
			szLine2[0] = NULL;
			iCharCount = 0;
		}
	}
}
#include "BI0.h"
void CWinDumpDlg::DoIt(char* FileName, Types eType, BOOL bOpenFile, BOOL bDoAnalysis, int iAcceptGap)
{
	FILE* infile;
	FILE* outfile;
	AcquireHeader Header;
	AcquireRecord Record;
	RabbitRecord RRecord;
	RFID_Rec RFIRecord;
	ISRAcquireRecord ISRRecord;
	MCAAcquireRecord MCARecord;
	BI0Processor* bi0 = NULL;
	int Gap				= 0;
	int RecordNumber	= 0;
	int RecordCount		= 0;
	int GapNumber		= 0;
	__int64 TotalCountTime  = 0;
	int FirstTime       = 0;
	int LastTime		= 0;
	int Interval		= 0;
	char bs = 8;
	char temp[64];
	double Version;
	m_bInstType		= 0;

	char path_buffer[_MAX_PATH];

	if ((infile = fopen(FileName,"rb")) == NULL)
	{
		m_cMessage += "Cannot open input file!";
		return;
	}

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	if (m_sTargetFolder.IsEmpty())
	{
		_splitpath(FileName,drive,dir,fname,ext);
	}
	else
	{
		char _bfname[_MAX_FNAME];
		_splitpath(FileName,drive,dir,fname,ext); // get fname only
		_splitpath(m_sTargetFolder,drive,dir,_bfname,ext); // get path components, ignore fname
	}
	_makepath(path_buffer,drive,dir,fname,"txt"); // combine path components, fname and extension


	if (strcmp(FileName,path_buffer)==0)
	{
		strcat(path_buffer,".txt");
	}

	if ((outfile = fopen(path_buffer,"wt")) == NULL)
	{
		m_cMessage += "Cannot open output file!";
		return;
	}

	//////////////////////////////////////////////////////
	//
	// (1) Decode the header and write the data to the output file
	//
	//////////////////////////////////////////////////////
	g_bGPS_OldStyle = false;
	switch (eThisType) 
	{
	case CHN:
		InitializeCHNFileVariables();
		DecodeCHNHeader(FileName, infile, TRUE, outfile);
		break;

	case BINARY:
		InitializeBinaryFileVariables();
		DecodeBinaryHeader(FileName, infile, TRUE, outfile);
		break;

	case BI0:
		bi0 = new BI0Processor(iAcceptGap, m_sTargetFolder);
		bi0->DecodeBI0Header(FileName, infile, bDoAnalysis, outfile);
		break;

	case VACOSS:
		InitializeVACOSSFileVariables();
		DecodeVACOSSHeader(FileName, infile, TRUE, outfile);
		break;

	case GPS:
		InitializeGPSFileVariables();
		DecodeGPSHeader(FileName, infile, TRUE, outfile);
		break;

	case MGD:
	case ISR:
	case JSR:
	case PSR:
	case GRAND:
	case MCA:
	case HMR:
	case ESS:
	case RAD:
	case RFI:
		if (fread(&Header,sizeof(Header),1,infile) == 1)
		{
			memcpy(temp,Header.SizeOfHeader,4);
			temp[4] = NULL;
			if (bDoAnalysis)
			{
				fprintf(outfile,"File Name: %s\n",FileName);
				fprintf(outfile,"Header\n");
				fprintf(outfile," Size of Header: %s\n",temp);
			}
			memcpy(temp,Header.Version,5);
			temp[5] = NULL;
			Version = atof(temp);
			if (bDoAnalysis)
				fprintf(outfile,"        Version: %s\n",temp);
			memcpy(temp,Header.StationId,3);
			temp[3] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"     Station ID: %s\n",temp);
			memcpy(temp,Header.Year,3);
			temp[3] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"           Year: %s\n",temp);
			memcpy(temp,Header.Month,3);
			temp[3] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"          Month: %s\n",temp);
			memcpy(temp,Header.Day,3);
			temp[3] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"            Day: %s\n",temp);
			memcpy(temp,Header.unused2,47);
			temp[47] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"       unused2: %s\n",temp);
		}
		break;

	case SOH:
		if (fread(&Header,sizeof(Header),1,infile) == 1)
		{
			memcpy(temp,Header.SizeOfHeader,4);
			temp[4] = NULL;
			if (bDoAnalysis)
			{
				fprintf(outfile,"File Name: %s\n",FileName);
				fprintf(outfile,"Header\n");
				fprintf(outfile," Size of Header: %s\n",temp);
			}
			memcpy(temp,Header.Version,5);
			temp[5] = NULL;
			Version = atof(temp);
			if (bDoAnalysis)
				fprintf(outfile,"        Version: %s\n",temp);
			memcpy(temp,Header.StationId,3);
			temp[3] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"     Station ID: %s\n",temp);
			memcpy(temp,Header.Year,3);
			temp[3] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"           Year: %s\n",temp);
			memcpy(temp,Header.Month,3);
			temp[3] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"          Month: %s\n",temp);
			memcpy(temp,Header.Day,3);
			temp[3] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"            Day: %s\n",temp);
			memcpy(temp,Header.unused2,47);
			m_bInstType = temp[46];
			temp[46] = NULL;
			if (bDoAnalysis)
				fprintf(outfile,"       unused2: %s\n",temp);

		}
		break;
	default:
		GeneralHexDump(FileName, infile, true, outfile, &RecordCount);
		bDoAnalysis = false;
	}
	
	//////////////////////////////////////////////////////
	//
	// (2) Output the data records to the output file one at a time
	//
	//////////////////////////////////////////////////////
	unsigned int uiPreviousJulian = 0;
	unsigned int uiPreviousCountTime = 0;

	while(!feof(infile))
	{
		switch (eThisType) 
		{
		case ESS:
			DecodeOneESSRecordSet(FileName, infile, TRUE, outfile);
			break;
		case SOH:
			{
				switch (m_bInstType) {
				case SOH_MGD: 
					DecodeOneMGDRecord(FileName, infile, TRUE, outfile, &RecordCount);
					break;
				case SOH_ISR: 
					break;
				case SOH_JSR: 
					break;
				case SOH_PSR: 
					break;
				case SOH_GRAND: 
					break;
				case SOH_MCA: 
					break;
				case SOH_VACOSS: 
					DecodeOneVACOSSRecord(FileName, infile, TRUE, outfile);
					break;
//				case SOH_BINARY: 
//					DecodeOneCHNRecord(FileName, infile, TRUE, outfile);
					break;
				case SOH_GPS: 
					DecodeOneGPSRecord(FileName, infile, TRUE, outfile);
					break;
				default:
					GeneralHexDump(FileName, infile, TRUE, outfile, &RecordCount);
				}
			}
			break;

		case CHN:
			DecodeOneCHNRecord(FileName, infile, TRUE, outfile);
			break;

		case BINARY:
			DecodeOneBinaryRecord(FileName, infile, true, outfile);
			break;

		case BI0:
			bi0->DecodeOneBI0DataRecord(FileName, infile, TRUE, outfile);
			break;

		case VACOSS:
			DecodeOneVACOSSRecord(FileName, infile, TRUE, outfile);
			break;

		case GPS:
			if (g_bGPS_OldStyle)
				DecodeOneOldStyleGPSRecord(FileName, infile, TRUE, outfile);
			else
			DecodeOneGPSRecord(FileName, infile, TRUE, outfile);
			break;

		case MGD:
			DecodeOneMGDRecord(FileName, infile, TRUE, outfile, &RecordCount);
			break;

		case GRAND:
			if (fread(&Record,sizeof(AcquireRecord),1,infile))
			{
				if (bDoAnalysis)
				{
					if (RecordCount == 0)
					{
						FirstTime = Record.JulianSeconds;
						Interval  = Record.CountTime;
					}
					if (Interval != Record.CountTime) Interval = -1;
					LastTime = Record.JulianSeconds;
					RecordCount++;
					TotalCountTime += Record.CountTime;
				}
				CJulianTime cJulianTime;
				cJulianTime.Init(Record.JulianSeconds);

				fprintf(outfile,"%s %s %10u %02x %10f %10f %10f %10f %10f %10f %10f %d\n",
					cJulianTime.Date(),
					cJulianTime.Time(),
					Record.JulianSeconds, 
					Record.StatusByte, 
					Record.NeutronChA,
					Record.NeutronChB, 
					Record.NeutronChC, 
					Record.GammaCh1, 
					Record.GammaCh1Sigma, 
					Record.GammaCh2, 
					Record.GammaCh2Sigma,
					Record.CountTime);
				if (bDoAnalysis)
				{
					if (Record.JulianSeconds < uiPreviousJulian)
					{
						fprintf(outfile,"Preceding Record Out of Order Error\n");
						RecordNumber++;
					}
					if (uiPreviousJulian && (abs(int(Record.JulianSeconds - uiPreviousJulian - uiPreviousCountTime)) > iAcceptGap))
					{
						fprintf(outfile,"Preceding Record Gap Error: %d\n",Record.JulianSeconds - uiPreviousJulian - uiPreviousCountTime);
						GapNumber++;
					}

				uiPreviousJulian = Record.JulianSeconds; 
				uiPreviousCountTime = Record.CountTime;
				}
			}
			break;

		case MCA:
			if (fread(&MCARecord,sizeof(MCARecord),1,infile))
			{
				if (bDoAnalysis)
				{
					if (RecordCount == 0)
					{
						FirstTime = MCARecord.JulianSeconds;
						Interval  = MCARecord.ElapsedTime;
					}
					if (Interval != MCARecord.ElapsedTime) Interval = -1;
					LastTime = MCARecord.JulianSeconds;
					RecordCount++;
					TotalCountTime += MCARecord.ElapsedTime;
				}
				CJulianTime cJulianTime;
				cJulianTime.Init(MCARecord.JulianSeconds);
				fprintf(outfile,"%s %s %10u %02x %02x %02x %8.0f %8.0f %8.0f %8.0f %8.0f %12f %12f %d\n",
					cJulianTime.Date(),
					cJulianTime.Time(),
					MCARecord.JulianSeconds, 
					MCARecord.StatusByte1, 
					MCARecord.StatusByte2, 
					MCARecord.Alarm,
					MCARecord.GrossCounts, 
					MCARecord.U235,
					MCARecord.CS137, 
					MCARecord.U238, 
					MCARecord.Scaler, 
					MCARecord.U235Ratio, 
					MCARecord.CS137Ratio,
					MCARecord.ElapsedTime);
				if (bDoAnalysis)
				{
					if (MCARecord.JulianSeconds < uiPreviousJulian)
					{
						fprintf(outfile,"Preceding Record Out of Order Error\n");
						RecordNumber++;
					}
					if (uiPreviousJulian && (abs(int(MCARecord.JulianSeconds - uiPreviousJulian - uiPreviousCountTime)) > iAcceptGap))
					{
						fprintf(outfile,"Preceding Record Gap Error: %d\n",MCARecord.JulianSeconds - uiPreviousJulian - uiPreviousCountTime);
						GapNumber++;
					}

					uiPreviousJulian = MCARecord.JulianSeconds; 
					uiPreviousCountTime = MCARecord.ElapsedTime;
				}
			}
			break;


		case RFI:
			if (fread(&RFIRecord,sizeof(RFIRecord),1,infile))
			{
				CJulianTime cJulianTime;
				cJulianTime.Init(RFIRecord.uiTime);

				fprintf(outfile,"%s %s %10u %02x %10u %10u %10u %10u %10u %10u %10u %10u %10u %10u %10u %10u %d\n",
					cJulianTime.Date(),
					cJulianTime.Time(),
					RFIRecord.uiTime,
					RFIRecord.usNode, 
					RFIRecord.bRFIDSerialNumber[0],
					RFIRecord.bRFIDSerialNumber[1],
					RFIRecord.bRFIDSerialNumber[2],
					RFIRecord.bRFIDSerialNumber[3],
					RFIRecord.bRFIDSerialNumber[4],
					RFIRecord.bRFIDSerialNumber[5],
					RFIRecord.bRFIDSerialNumber[6],
					RFIRecord.bRFIDSerialNumber[7],
					RFIRecord.bRFIDSerialNumber[8],
					RFIRecord.bRFIDSerialNumber[9],
					RFIRecord.bRFIDSerialNumber[10],
					RFIRecord.bRFIDSerialNumber[11],
					RFIRecord.bCount);
				if (bDoAnalysis)
				{

				}
			}
			break;

		case RAD:
			if (fread(&RRecord,sizeof(RabbitRecord),1,infile))
			{
				if (bDoAnalysis)
				{
					if (RecordCount == 0)
					{
						FirstTime = RRecord.JulianSeconds;
						Interval  = RRecord.CountTime;
					}
					if (Interval != RRecord.CountTime) Interval = -1;
					LastTime = RRecord.JulianSeconds;
					RecordCount++;
					TotalCountTime += RRecord.CountTime;

				}
				CJulianTime cJulianTime;
				cJulianTime.Init(RRecord.JulianSeconds);

				fprintf(outfile,"%s %s %10u %02x %10u %10u %10u %10u %10u %10u %10u %10u %d\n",
					cJulianTime.Date(),
					cJulianTime.Time(),
					RRecord.JulianSeconds, 
					RRecord.StatusByte, 
					RRecord.uiCounts[0],
					RRecord.uiCounts[1],
					RRecord.uiCounts[2],
					RRecord.uiCounts[3],
					RRecord.uiCounts[4],
					RRecord.uiCounts[5],
					RRecord.uiCounts[6],
					RRecord.uiCounts[7],
					RRecord.CountTime);
				if (bDoAnalysis)
				{
					if (RRecord.JulianSeconds < uiPreviousJulian)
					{
						fprintf(outfile,"Preceding Record Out of Order Error\n");
						RecordNumber++;
					}
					if (uiPreviousJulian && (abs(int(RRecord.JulianSeconds - uiPreviousJulian - uiPreviousCountTime/10)) > iAcceptGap))
					{
						fprintf(outfile,"Preceding Record Gap Error: %d\n",RRecord.JulianSeconds - uiPreviousJulian - uiPreviousCountTime);
						GapNumber++;
					}

				uiPreviousJulian = RRecord.JulianSeconds; 
				uiPreviousCountTime = RRecord.CountTime;
				}
			}
			break;

		case JSR:
		case ISR:
		case PSR:
		case HMR:
			if (fread(&ISRRecord,sizeof(ISRRecord),1,infile))
			{
				if (Version >= 1.5)
				{
					ISRRecord.ElapsedTime = (__int64)ISRRecord.fElapsedTime;
					ISRRecord.TotalsCount = (__int64)ISRRecord.fTotalsCount;
					ISRRecord.Totals2Count= (__int64)ISRRecord.fTotals2Count;
					ISRRecord.Totals3Count= (__int64)ISRRecord.fTotals3Count;
					ISRRecord.RealAccCount= (__int64)ISRRecord.fRealAccCount;
					ISRRecord.AccidenCount= (__int64)ISRRecord.fAccidenCount;
				}
				if (bDoAnalysis)
				{
					if (RecordCount == 0)
					{
						FirstTime = ISRRecord.JulianSeconds;
						//Interval  = (int)(((float)ISRRecord.ElapsedTime+5.0)/10.0);
					}
					//if (Interval != ((int)(((float)ISRRecord.ElapsedTime+5.0)/10.0))) Interval = -1;
					LastTime = ISRRecord.JulianSeconds;
					RecordCount++;
					TotalCountTime += ISRRecord.ElapsedTime;
				}
				CJulianTime cJulianTime;
				cJulianTime.Init(ISRRecord.JulianSeconds);
				fprintf(outfile,"%s %s %10u %02x %11I64d %10I64d %10I64d %15I64d %15I64d %10I64d\n",
					cJulianTime.Date(),
					cJulianTime.Time(),
					ISRRecord.JulianSeconds, 
					ISRRecord.StatusByte, 
					ISRRecord.TotalsCount,
					ISRRecord.Totals2Count, 
					ISRRecord.Totals3Count, 
					ISRRecord.RealAccCount, 
					ISRRecord.AccidenCount, 
					ISRRecord.ElapsedTime);
				if (bDoAnalysis)
				{
					if (ISRRecord.JulianSeconds < uiPreviousJulian)
					{
						fprintf(outfile,"Preceding Record Out of Order Error\n");
						RecordNumber++;
					}
					if (uiPreviousJulian && (abs(int(ISRRecord.JulianSeconds - uiPreviousJulian - uiPreviousCountTime)) > iAcceptGap))
					{
						fprintf(outfile,"Preceding Record Gap Error: %d\n",ISRRecord.JulianSeconds - uiPreviousJulian - uiPreviousCountTime);
						GapNumber++;
					}

					uiPreviousJulian = ISRRecord.JulianSeconds; 
					uiPreviousCountTime = (unsigned int)(((float)ISRRecord.ElapsedTime));
				}
			}
			break;
		}  //end switch
	} //end while(!feof(infile))
	//////////////////////////////////////////////////////
	//
	// (4) output the same information to the output file
	//
	///////////////////////////////////////////////////////

	if (bDoAnalysis && (eThisType != ESS))
	{
		if (eThisType == BINARY)
			WriteBinarySummaryInformationToOutputFile(FileName, infile, TRUE, outfile);

		else if (eThisType == VACOSS)
			WriteVACOSSSummaryInformationToOutputFile(FileName, infile, TRUE, outfile);

		else if (eThisType == GPS)
			WriteGPSSummaryInformationToOutputFile(FileName, infile, TRUE, outfile);

		else if (eThisType == CHN)
			WriteCHNSummaryInformationToOutputFile(FileName, infile, TRUE, outfile);

		else if (eThisType == MGD)
		{
			fprintf(outfile,"                File Name: %s\n",FileName);
			fprintf(outfile,"  Total Number of Records: %10d\n",RecordCount);
		}
		else if (eThisType == SOH)
		{
			fprintf(outfile,"                File Name: %s\n",FileName);
			fprintf(outfile,"  Total Number of Records: %10d\n",RecordCount);
		}
		else if (eThisType == RFI)
		{
			fprintf(outfile,"                File Name: %s\n",FileName);
			fprintf(outfile,"  Total Number of Records: %10d\n",RecordCount);
		}
		else if (eThisType == BI0)
		{
			bi0->WriteBI0SummaryInformationToOutputFile(FileName, infile, true, outfile);
		}
		else
		{
			fprintf(outfile,"                File Name: %s\n",FileName);
			fprintf(outfile,"  Total Number of Records: %10d\n",RecordCount);
			fprintf(outfile,"       Average Count Time: %10.4f\n",(float)TotalCountTime/(float)RecordCount);
			if (Interval > 0)
			{
				int Expected = ((LastTime-FirstTime)/(Interval)) + 1;
				fprintf(outfile,"  Expected Number Of Recs: %10d\n",Expected);
				fprintf(outfile,"  Ratio Expected / Actual: %10.3f\n", (float)Expected/(float)RecordCount);
			}
			fprintf(outfile,"Out of Order Record Count: %10d\n",RecordNumber);
			fprintf(outfile,"                Gap Count: %10d\n",GapNumber);
			fprintf(outfile,"        Percent Gap <=%3d: %10.2f\n",iAcceptGap,((float)(RecordCount - GapNumber)/(float)RecordCount)*100.0);
		}

		fprintf(outfile, 
			"\n>>>>>>>>>>>>>>>> End of output for file %s  <<<<<<<<<<<<<<<<<<<<<<\n\n",FileName);
	}

	fclose(infile);
	fclose(outfile);
	m_cMessage += path_buffer;

	if (bOpenFile)
		ShellExecute(m_hWnd,"OPEN",path_buffer,NULL,NULL,SW_SHOW);

	if (bi0)
	{
		char buff[128];
		sprintf(buff, "\r\n  Found %d valid records and %d invalid records.\r\n",bi0->mValidRecords,bi0->mSkippedRecords);
		m_cMessage += buff;
		sprintf(buff, "  %d valid records were skipped because they were not acquire records.\r\n",bi0->mNonAcqRecords);
		m_cMessage += buff;
		delete bi0;
	}
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
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
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

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlg dialog

IMPLEMENT_DYNAMIC(CWinDumpDlg, CDialog);

CWinDumpDlg::CWinDumpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinDumpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinDumpDlg)
	m_bGapAnalysis = FALSE;
	m_bOpenOutput = FALSE;
	m_iGapSeconds = 2;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CWinDumpDlg::~CWinDumpDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CWinDumpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinDumpDlg)
	DDX_Control(pDX, IDC_CHECK_GAPANALYSIS, m_AcceptGap);
	DDX_Check(pDX, IDC_CHECK_GAPANALYSIS, m_bGapAnalysis);
	DDX_Check(pDX, IDC_CHECK_OPENOUTPUT, m_bOpenOutput);
	DDX_Text(pDX, IDC_EDIT_ACCEPTGAP, m_iGapSeconds);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWinDumpDlg, CDialog)
	//{{AFX_MSG_MAP(CWinDumpDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_CHECK_GAPANALYSIS, OnCheckGapanalysis)
	ON_MESSAGE(BATCHMEBABY,BatchProcessor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlg message handlers

BOOL CWinDumpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
			pSysMenu->AppendMenu(MF_STRING, IDM_BIDBI0GOGOGO, "BI0 Conversion Settings"); 
			pSysMenu->AppendMenu(MF_STRING, IDM_ASSOCIATE, "Associate File Extensions"); 
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Extra initialization added here
	DragAcceptFiles( TRUE );

	SetDlgItemInt(IDC_EDIT_ACCEPTGAP,2);
	GetDlgItem(IDC_EDIT_ACCEPTGAP)->EnableWindow(FALSE);

	AfxGetApp()->ParseCommandLine(mTcl);

	m_bOpenOutput = mTcl.output;
	m_bGapAnalysis = mTcl.gap;
	m_iGapSeconds = mTcl.interval;

	m_sTargetFolder = mTcl.bidfullpath;  // can be null
	if (!m_sTargetFolder.IsEmpty())
	{
		if (m_sTargetFolder.Right(1) != '\\')
			m_sTargetFolder += '\\';
	}

	BI0Processor::mWriteBIDfromBI0 = mTcl.bid;
	BI0Processor::mSkipSubSecondStart = mTcl.trim;

	if (mTcl.direct)
		return TRUE;  // return TRUE  unless you set the focus to a control
	else
	{
		PostMessage(BATCHMEBABY,0,0);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CWinDumpDlg::BatchProcessor(WPARAM wParam, LPARAM lParam)
{
	Sleep(100);
	DWORD close = 0;
	if (mTcl.help)
	{
		HelpDlg dlg;
		dlg.help = mTcl.ToString();
		dlg.DoModal();
		close = 100;
	}
	else if (!mTcl.dir) // if it is a single file, then do it (it's a single file if a dir was not found for the first param)
	{
		DoAFile(mTcl.fullpath);
		GetDlgItem(IDC_EDIT_LOG)->UpdateWindow();
		close = 1000;
	}
	else if (mTcl.dir)// (gotta be a dir dude!)
	{
		//generate list of matching files
		CStringList list; list.RemoveAll();
		CString patt("."); patt.Append(mTcl.suffix);
		::PathAddBackslash(mTcl.fullpath);
		BuildFileList(list, CString(mTcl.fullpath), patt);

		//	call DoAFile(filename) on each matching file
		POSITION pos = list.GetHeadPosition();
		while (pos) //For each patt file found in the dir tree, process it baby!
		{
			CString& s = list.GetNext(pos);
			DoAFile(s.GetBuffer());
			GetDlgItem(IDC_EDIT_LOG)->SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
			GetDlgItem(IDC_EDIT_LOG)->UpdateWindow();
		}
		close = 2000;
	}

	if (close > 0)
	{
		Sleep(close);
		EndDialog(0);
	}
	return 0;
}

bool CWinDumpDlg::HasSuffix(const CString& fname, const CString& sfx)
{
	CString ext(PathFindExtension(fname));
	int r = ext.CompareNoCase(sfx);
	return r == 0;
}


void 	CWinDumpDlg::BuildFileList(CStringList& list, CString dir, const CString& pattern) 
{
	CFileFind find;
	BOOL bWorking;
	CString dirpat = dir; dirpat.Append("*.*");
	bWorking = find.FindFile(dirpat);
	do
	{
		bWorking = find.FindNextFile();
		if(!find.IsDots())
		{
			if(find.IsDirectory())
			{
				CString subdir(find.GetFilePath()); subdir.Append("\\");
				OutputDebugString(subdir + "\r\n");
				BuildFileList(list, subdir, pattern);
			}
			else if (HasSuffix(find.GetFileName(), pattern))
			{
				CString FileName = dir + find.GetFileName(); 
				OutputDebugString(find.GetFilePath() + "\r\n");
				list.AddTail(FileName);
			}
		}		
	}
	while(bWorking != 0);
}

void CWinDumpDlg::OnDropFiles(HDROP hDropInfo)
{
    TCHAR szFileName[_MAX_PATH + 1];

    if (NULL != hDropInfo)
    {
        UINT nFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

		m_cMessage.Empty();

        for(UINT nNames = 0; nNames < nFiles; nNames++)
        {
            ZeroMemory(szFileName, _MAX_PATH + 1);
            DragQueryFile
                (hDropInfo, nNames, (LPTSTR)szFileName, _MAX_PATH + 1);

			UpdateData();
			DoAFile(szFileName);
			//GetDlgItem(IDC_EDIT_LOG)->Update();
        }
//      GlobalUnlock(hDropInfo);
	}
}

void CWinDumpDlg::DoAFile(char*  szFileName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(szFileName, drive, dir, fname, ext);
	_strupr(ext);

	if (strstr(ext,".BID"))
		eThisType = GRAND;
	else if (strstr(ext,".BI0"))
		eThisType = BI0;
	else if (strstr(ext,"SOH"))
		eThisType = SOH;
	else if (strstr(ext,"MCA"))
		eThisType = MCA;
	else if (strstr(ext,"ISR"))
		eThisType = ISR;
	else if (strstr(ext,"JSR"))
		eThisType = JSR;
	else if (strstr(ext,"PSR"))
		eThisType = PSR;
	else if (strstr(ext,"VCS"))
		eThisType = VACOSS;
	else if (strstr(ext,"BNY"))
		eThisType = BINARY;
	else if (strstr(ext,"GPS"))
		eThisType = GPS;
	else if (strstr(ext,"CHN"))
		eThisType = CHN;
	else if (strstr(ext,"HMR"))
		eThisType = HMR;
	else if (strstr(ext,"ESS"))
		eThisType = ESS;
	else if (strstr(ext,"RAD"))
		eThisType = RAD;
	else if (strstr(ext,"RFI"))
		eThisType = RFI;
//	else if (strstr(ext,"MGD"))
//		eThisType = MGD;
	else
		eThisType = UNKNOWN;

//	if (eThisType != UNKNOWN)
	{
		m_cMessage += "Translating...\r\n";
		m_cMessage += szFileName;
		m_cMessage += "\r\nto\r\n";
		DoIt(szFileName,eThisType, m_bOpenOutput, m_bGapAnalysis, m_iGapSeconds );
	}
//	else
//	{
//		m_cMessage += "File extension not recognized...\r\n";
//		m_cMessage += szFileName;
//	}
	m_cMessage += "\r\n\r\n";
	SetDlgItemText(IDC_EDIT_LOG,m_cMessage);
}


void CWinDumpDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	
	if ((nID & 0xFFF0) == IDM_BIDBI0GOGOGO)
	{
		CBioSettings dlg;
		dlg.DoModal();
	}
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	if ((nID & 0xFFF0) == IDM_ASSOCIATE)
	{
		CGCFileTypeAccess TheFTA;

		// get full file path to program executable file
		TCHAR	szProgPath[MAX_PATH * 2];
		::GetModuleFileName(NULL, szProgPath, sizeof(szProgPath)/sizeof(TCHAR));

		CString csTempText;

		TheFTA.SetExtension("BID");

		// just pass file path in quotes on command line
		csTempText.Format("\"%s\" %s",szProgPath,"\"%1\"");


		TheFTA.SetShellOpenCommand(csTempText);
		TheFTA.SetDocumentShellOpenCommand(csTempText);

		TheFTA.SetDocumentClassName("MICDump.Document");

		// use first icon in program
		csTempText  = szProgPath;
		csTempText += ",0";
		TheFTA.SetDocumentDefaultIcon(csTempText);

		// set the necessary registry entries	
		TheFTA.RegSetAllInfo();
		TheFTA.SetExtension("RFI");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("SOH");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("ISR");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("JSR");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("HMR");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("CHN");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("MCA");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("PSR");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("VCS");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("GPS");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("BNY");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("ESS");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("RAD");
		TheFTA.RegSetExtension();
		TheFTA.SetExtension("BI0");
		TheFTA.RegSetExtension();

		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0); 

	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWinDumpDlg::OnPaint() 
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
HCURSOR CWinDumpDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CWinDumpDlg::OnClose() 
{
	if (CanExit())
		CDialog::OnClose();
}

void CWinDumpDlg::OnOK() 
{
	if (CanExit())
		CDialog::OnOK();
}

void CWinDumpDlg::OnCancel() 
{
	if (CanExit())
		CDialog::OnCancel();
}

BOOL CWinDumpDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void CWinDumpDlg::OnCheckGapanalysis() 
{
	GetDlgItem(IDC_EDIT_ACCEPTGAP)->EnableWindow(m_AcceptGap.GetCheck()==1);
}


