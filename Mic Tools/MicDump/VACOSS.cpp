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
// file VACOSS.cpp
#include "stdafx.h"

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <assert.h>
#include "JulianT.h"
#include <AFXDISP.h>  //for COleDateTime

#include "event.h"

////////////////
//
// Variables
//
////////////////
	static EventAcquireHeader event_acquire_header;  //73 bytes
	static EventAcquireRecord event_acquire_record;  //73 bytes

	static int iRecordCount;
	static int iEventRecordCount;
	static int iInformationRecordCount;
	static int iErrorRecordCount;
	static int iMinutesSinceSealInitialization;
	static unsigned int uiJulianInitializatonTime;

	static COleDateTime *pInitialization_date_time;
/////////////////
//
// Functions
//
/////////////////

void GetVacossEvents(unsigned char* pAt, unsigned int *uiTime1, unsigned int *uiTime2)
{
	*uiTime1 = *pAt;
	*uiTime1 = (*uiTime1 << 8);
	*uiTime1 += *(pAt+1);
	*uiTime1 = (*uiTime1 << 4);
	*uiTime1 += ((*(pAt+4) & 0xf0) >> 4);

	*uiTime2 = *(pAt+2);
	*uiTime2 = (*uiTime2 << 8);
	*uiTime2 += *(pAt+3);
	*uiTime2 = (*uiTime2 << 4);
	*uiTime2 += (*(pAt+4) & 0x0f);
}

void AddMinutesToInitializationTime(unsigned int uiMinutes, char szDateTimeString[])
{
	if (uiMinutes == 0) //we have no event
	{
		sprintf(szDateTimeString, "NA");
	}
	else
	{
		//convert the time interval to a time span object.
		long long_days = 0;
		int nHours = 0;
		int nMins = uiMinutes;

		while(nMins >= 60)
		{
			nHours++;
			nMins -= 60;
		}
		while(nHours >= 24)
		{
			long_days++;
			nHours -= 24;
		}

		COleDateTimeSpan elapsed_time(long_days, nHours, nMins, 0);
		COleDateTime     event_time(*pInitialization_date_time);
		
		event_time += elapsed_time;// Add elapsed time to time of seal initialization

		sprintf(szDateTimeString,"%04d.%02d.%02d %02d:%02d:%02d",
			event_time.GetYear(),
			event_time.GetMonth(),
			event_time.GetDay(),
			event_time.GetHour(),
			event_time.GetMinute(),
			event_time.GetSecond());
	}
}

void doVACOSSEVENT_REC(bool DoOutfile, FILE *outfile)
{
	VacossEvent_Rec *pInfo = (VacossEvent_Rec *)(&event_acquire_record.bData);

	char chRecordType[3];
	chRecordType[0] = pInfo->RecTypeA;
	chRecordType[1] = pInfo->RecTypeB;
	chRecordType[2] = NULL;

	CJulianTime Julian;
	char chCreationTime[64];
	Julian.Init(pInfo->uiTime);
	strcpy(chCreationTime, Julian.Date());
	strcat(chCreationTime, " ");
	strcat(chCreationTime, Julian.Time());

	int iResponseType = pInfo->bResponseType;
	int iNodeNumber = pInfo->usNodeNumber;
	int iSealID = ((pInfo->bSealData[0]) << 8) + pInfo->bSealData[1];

	/////// pull out the 10 "minutes from initialization" values

	unsigned int min_from_init[10];      //miniutes from initialization for 10 events
	char date_and_time[10][64]; // string of date and time calculated for each event.

	unsigned char* pAt = &pInfo->bSealData[2]; //beginning of raw data
	int ievent = 0;
	unsigned int uiTime[2];

	for (int j=0; j<5; j++)  // 5 sets of 2 events each
	{
		GetVacossEvents(pAt, &uiTime[0], &uiTime[1]);

		for (int i = 0; i < 2; i++)
		{
			min_from_init[ievent] = uiTime[i];
			AddMinutesToInitializationTime(uiTime[i], date_and_time[ievent]);
			ievent++;
		}

		pAt += 5;  //5 bytes per set
	}

	///<<QA>> hack until event 10 raw data gets fixed
	min_from_init[9] = 0;
	strcpy(date_and_time[9], "NA");

	//////////

	int iLRC = pInfo->bSealData[27];

	BYTE *p = &pInfo->bSealData[28];
	char chSignature[33];
	sprintf(chSignature,"%04x%04x%04x%04x%04x%04x%04x%04x",
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8]);


	unsigned short usChecksum1 = 0;
	unsigned short usChecksum2 = 0;
	for (int i = 0; i < VACOSSEVENT_REC_LENGTH - 1; i++)
	{
		usChecksum1 += *(((BYTE*)pInfo)+i);
		if (i == VACOSSEVENT_REC_LENGTH - 1 - 5)
			usChecksum2 = usChecksum1;
	}
	usChecksum1 &= 0x00ff;
	usChecksum2 &= 0x00ff;


	if (DoOutfile)
	{
		fprintf(outfile,"\n\n");
		fprintf(outfile,">>> VACOSS EVENT RECORD %d\n\n", iEventRecordCount);
		fprintf(outfile, "                       Event record type: %s\n",   chRecordType);
		fprintf(outfile, " Time of record creation by seal monitor: %s\n",   chCreationTime);
		fprintf(outfile, "                           Response type: %d\n",   iResponseType);
		fprintf(outfile, "                   Seal ILON node number: %d\n",   iNodeNumber);
		fprintf(outfile, "                          Seal ID number: %d\n",   iSealID);
		fprintf(outfile, "                                     LRC: 0x%x\n", iLRC);
		fprintf(outfile, "                               Signature: %s\n", chSignature);
		if (usChecksum1 == pInfo->bChecksum)
			fprintf(outfile, "                     Calculated Checksum: 0x%02x\n\n", 
				usChecksum1);
		else
			fprintf(outfile, "                     Calculated Checksum: 0x%02x or 0x%02x (not including last 5 bytes)\n\n", 
				usChecksum1, usChecksum2);
		fprintf(outfile, " Event Number   Minutes from Initialization   Event date and time\n");

		for (int i = 0; i < 10; i++)
		{
			fprintf(outfile,"     %2d            %10d                 %s\n",
				i+1, 
				min_from_init[i], 
				date_and_time[i]);
		}

	}
}


void doVACOSSERROR_REC(bool DoOutfile, FILE *outfile)
{
	if (DoOutfile)
	{
		fprintf(outfile,"\n\n");
		fprintf(outfile,">>> VACOSS ERROR RECORD\n\n");

		VacossError_Rec *pInfo = (VacossError_Rec *)(&event_acquire_record.bData);

		char chRecordType[3];
		chRecordType[0] = pInfo->RecTypeA;
		chRecordType[1] = pInfo->RecTypeB;
		chRecordType[2] = NULL;

		CJulianTime Julian;
		char chCreationTime[64];
		Julian.Init(pInfo->uiTime);
		strcpy(chCreationTime, Julian.Date());
		strcat(chCreationTime, " ");
		strcat(chCreationTime, Julian.Time());

		char chReserved[24];
		sprintf(chReserved, "%02x %02x %02x %02x", 
			pInfo->bReserved[0],pInfo->bReserved[1],pInfo->bReserved[2],pInfo->bReserved[3]);

		unsigned short usChecksum1 = 0;
		unsigned short usChecksum2 = 0;
		for (int i = 0; i < VACOSSERROR_REC_LENGTH - 1; i++)
		{
			usChecksum1 += *(((BYTE*)pInfo)+i);
			if (i == VACOSSERROR_REC_LENGTH - 1 - 5)
				usChecksum2 = usChecksum1;
		}
		usChecksum1 &= 0x00ff;
		usChecksum2 &= 0x00ff;

		fprintf(outfile, "                       Event record type: %s\n",     chRecordType);
		fprintf(outfile, " Time of record creation by seal monitor: %s\n",     chCreationTime);
		fprintf(outfile, "                           Response type: %u\n",     pInfo->bResponseType);
		fprintf(outfile, "                        ILON node number: %u\n",     pInfo->usNode);
		fprintf(outfile, "                          Seal ID number: %u\n",     pInfo->usSealID);
		fprintf(outfile, "                              Error code: %u\n",     pInfo->usErrorCode);
		fprintf(outfile, "                                Reserved: %s\n",     chReserved);
		fprintf(outfile, "                                Checksum: 0x%02x\n", pInfo->bChecksum);
		if (usChecksum1 == pInfo->bChecksum)
			fprintf(outfile, "                     Calculated Checksum: 0x%02x\n", 
				usChecksum1);
		else
			fprintf(outfile, "                     Calculated Checksum: 0x%02x or 0x%02x (not including last 5 bytes)\n", 
				usChecksum1, usChecksum2);
	}
}

void doVACOSSINFO_REC(bool DoOutfile, FILE *outfile)	//	0x3335
{
	VacossInfo_Rec *pInfo = (VacossInfo_Rec *)(&event_acquire_record.bData);

	char chRecordType[3];
	chRecordType[0] = pInfo->RecTypeA;
	chRecordType[1] = pInfo->RecTypeB;
	chRecordType[2] = NULL;

	CJulianTime Julian;
	char chCreationTime[64];
	Julian.Init(pInfo->uiTime);
	strcpy(chCreationTime, Julian.Date());
	strcat(chCreationTime, " ");
	strcat(chCreationTime, Julian.Time());

	int iResponseType = pInfo->bResponseType;
	int iNodeNumber = pInfo->usNodeNumber;
	int iSealID = ((pInfo->bSealData[0]) << 8) + pInfo->bSealData[1];

	///////////////////////////////////////////////////////////
	/// Seal initialization date and time...

		int iYear     = ((0x3f & pInfo->bSealData[2 + 0]) >> 0) + 2000;
		int iMonth    = (0xf0 & pInfo->bSealData[2 + 1]) >> 4;
		int iDay      = 0x1f & (pInfo->bSealData[2 + 2] >> 3);
		int iHoursMid = (0x03 & pInfo->bSealData[2 + 2]) << 2;
		int iHoursLow = (0xc0 & pInfo->bSealData[2 + 3]) >> 6;
		int iHour     = iHoursMid + iHoursLow;
		int iMin      = (0x1f & pInfo->bSealData[2 + 3]) >> 0;
		int iSec      = (0x0f & pInfo->bSealData[2 + 4]) >> 0; //always zero

		if (pInitialization_date_time != NULL)
		{
			delete pInitialization_date_time;
			pInitialization_date_time = NULL;
		}

		pInitialization_date_time = new COleDateTime(iYear, iMonth, iDay, iHour, iMin, iSec);

		char chSealInitDateTime[64];

		sprintf(chSealInitDateTime,"%04d.%02d.%02d %02d:%02d:%02d",
			pInitialization_date_time->GetYear(),
			pInitialization_date_time->GetMonth(),
			pInitialization_date_time->GetDay(),
			pInitialization_date_time->GetHour(),
			pInitialization_date_time->GetMinute(),
			pInitialization_date_time->GetSecond());
	//
	//////////////////////////////////////////////////////////////////////

	int iAttachments = pInfo->bSealData[7];

	//minutes since init or case closure/status
	unsigned int uiTimeStamp;
	uiTimeStamp = pInfo->bSealData[8] << 8;
	uiTimeStamp += pInfo->bSealData[9];
	uiTimeStamp = uiTimeStamp << 4;
	uiTimeStamp = uiTimeStamp & 0x000ffff0;
	iMinutesSinceSealInitialization = uiTimeStamp;

	unsigned char status = pInfo->bSealData[8 + 2];
	char chFiberClosedOpen[8]  = "open";    //must hold "closed"
	char chBatteryLowNormal[8] = "normal";  //must hold "low"
	char chCaseClosedOpen[8]   = "closed";  //must hold "open"

	if (0x80 & pInfo->bSealData[8 + 2])     //bit 7 high = fiber closed
		strcpy(chFiberClosedOpen,"closed");
	if (0x20 & pInfo->bSealData[8 + 2])     //bit 5 high = battery low
		strcpy(chBatteryLowNormal,"low");
	if (0x10 & pInfo->bSealData[8 + 2])     //bit 4 high = case opened
		strcpy(chCaseClosedOpen, "open");

	int iEvents = pInfo->bSealData[11];
	int iLRC = pInfo->bSealData[12];

	BYTE *p = &pInfo->bSealData[13];
	char chSignature[33];
	sprintf(chSignature,"%04x%04x%04x%04x%04x%04x%04x%04x",
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8]);

	unsigned short usChecksum1 = 0;
	unsigned short usChecksum2 = 0;
	for (int i = 0; i < VACOSSINFO_REC_LENGTH - 1; i++)
	{
		usChecksum1 += *(((BYTE*)pInfo)+i);
		if (i == VACOSSINFO_REC_LENGTH - 1 - 5)
			usChecksum2 = usChecksum1;
	}
	usChecksum1 &= 0x00ff;
	usChecksum2 &= 0x00ff;
	
	
	if (DoOutfile)
	{
		fprintf(outfile,"\n\n");
		fprintf(outfile,">>> VACOSS INFORMATION RECORD %d\n\n", iInformationRecordCount);
		fprintf(outfile, "                 Information record type: %s\n",   chRecordType);
		fprintf(outfile, " Time of record creation by seal monitor: %s\n",   chCreationTime);
		fprintf(outfile, "                           Response type: %d\n",   iResponseType);
		fprintf(outfile, "                   Seal ILON node number: %d\n",   iNodeNumber);
		fprintf(outfile, "                          Seal ID number: %d\n",   iSealID);
		fprintf(outfile, "       Seal Initialization date and time: %s\n",   chSealInitDateTime);
		fprintf(outfile, " Reader attachments since initialization: %d\n",   iAttachments);
		fprintf(outfile, "       Minutes since seal initialization: %d\n",   iMinutesSinceSealInitialization);
		fprintf(outfile, "                            Fiber status: %s\n",   chFiberClosedOpen);
		fprintf(outfile, "                          Battery status: %s\n",   chBatteryLowNormal);
		fprintf(outfile, "                             Case status: %s\n",   chCaseClosedOpen);
		fprintf(outfile, "   Number of events since initialization: %d\n",   iEvents);
		fprintf(outfile, "                                     LRC: 0x%x\n", iLRC);
		fprintf(outfile, "                               Signature: %s\n",   chSignature);
	}
}

////////////////////////
//
// Decode One VACOSS Record
//
////////////////////////
void DecodeOneVACOSSRecord(
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	// to start, read in only event_acquire_record.usSize
	if (fread(&event_acquire_record,sizeof(unsigned short),1,infile))
		{
			iRecordCount++;

			// then read in the rest of the record (usSize bytes more).
			fread(&event_acquire_record.bData, event_acquire_record.usSize, 1, infile);

			char *pch = (char *)&event_acquire_record.bData;
			char RecTypeA = *pch++;
			char RecTypeB = *pch;
			unsigned int usMessageType = RecTypeB + (RecTypeA << 8);

			switch (usMessageType)
			{
				case VACOSSINFO_REC:	//	0x3335
					iInformationRecordCount++;
					doVACOSSINFO_REC(DoOutfile, outfile);
					break;

				case VACOSSEVENT_REC:	//	0x3336
					iEventRecordCount++;
					doVACOSSEVENT_REC(DoOutfile, outfile);
					break;

				case VACOSSERROR_REC:	//	0x3339
					iErrorRecordCount++;
					doVACOSSERROR_REC(DoOutfile, outfile);
					break;

				default:
					assert(false);

			} //end switch
		}

	// spinner
//	char ch;
//	char backspace = 8;
//	switch (iRecordCount & 0x000000c0) 
//	{
//		case 0x00000000: ch = '|'; break;
//		case 0x00000040: ch = '/'; break;
//		case 0x00000080: ch = '-'; break;
//		case 0x000000c0: ch = '\\'; break;
//	}
//	cerr << backspace << ch << flush;
}

void WriteVACOSSSummaryInformationToConsole(	
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	printf("\n");
	printf("                    File Name: %s\n",FileName);
	printf("                    File Type: VACOSS Seal Events\n");
	printf("      Total Number of Records: %d\n",iRecordCount);
	printf("      Number of Event Records: %d\n",iEventRecordCount);
	printf("Number of Information Records: %d\n",iInformationRecordCount);
	printf("      Number of Error Records: %d\n",iErrorRecordCount);
}

void WriteVACOSSSummaryInformationToOutputFile(	
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	fprintf(outfile,"\n\n");
	fprintf(outfile,">>> SUMMARY\n\n");
	fprintf(outfile,"                    File Name: %s\n",FileName);
	fprintf(outfile,"                    File Type: VACOSS Seal Events\n");
	fprintf(outfile,"      Total Number of Records: %d\n",iRecordCount);
	fprintf(outfile,"      Number of Event Records: %d\n",iEventRecordCount);
	fprintf(outfile,"Number of Information Records: %d\n",iInformationRecordCount);
	fprintf(outfile,"      Number of Error Records: %d\n",iErrorRecordCount);
}

////////////////////////
//
// Decode VACOSS Header
//
////////////////////////

void DecodeVACOSSHeader(
	char *FileName, 
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	fread(&event_acquire_header,sizeof(event_acquire_header),1,infile);

	if (DoOutfile)
	{
		char temp[128];

		strncpy(temp,event_acquire_header.SizeOfHeader,4);
		temp[4] = NULL;

		fprintf(outfile,"File Name: %s\n",FileName);
		fprintf(outfile,"File Type: VACOSS Seal Events\n\n");
		fprintf(outfile,">>> HEADER\n\n");
		fprintf(outfile," Size of Header: %s\n",temp);

		strncpy(temp,event_acquire_header.unused1,5);
		temp[5] = NULL;
		fprintf(outfile,"        unused1: %s\n",temp);

		strncpy(temp,event_acquire_header.Version,5);
		temp[5] = NULL;
		fprintf(outfile,"    MIC Version: %s\n",temp);

		strncpy(temp,event_acquire_header.StationId,3);
		temp[3] = NULL;
		fprintf(outfile,"     Station ID: %s\n",temp);

		strncpy(temp,event_acquire_header.Year,3);
		temp[3] = NULL;
		fprintf(outfile,"           Year: %s\n",temp);

		strncpy(temp,event_acquire_header.Month,3);
		temp[3] = NULL;
		fprintf(outfile,"          Month: %s\n",temp);

		strncpy(temp,event_acquire_header.Day,3);
		temp[3] = NULL;
		fprintf(outfile,"            Day: %s\n",temp);

		strncpy(temp,event_acquire_header.FullYear,4);
		temp[4] = NULL;
		fprintf(outfile,"      Full Year: %s\n",temp);

		strncpy(temp,event_acquire_header.unused2,43);
		temp[43] = NULL;
		fprintf(outfile,"        unused2: %s\n",temp);
	}
}

void InitializeVACOSSFileVariables()
{
	static bool bFirstTimeThrough = true;

	iRecordCount = 0;
	iEventRecordCount = 0;
	iInformationRecordCount = 0;
	iErrorRecordCount = 0;

	if (!bFirstTimeThrough)
		delete pInitialization_date_time;

	pInitialization_date_time = NULL;
	bFirstTimeThrough = false;
}

void CloseOutVACOSSFileVariables()
{
	if (pInitialization_date_time != NULL)
		delete pInitialization_date_time;
}