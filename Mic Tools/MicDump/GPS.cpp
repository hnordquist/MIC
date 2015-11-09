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
// file GPS.cpp
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
	bool g_bGPS_OldStyle;

	static COleDateTime *pInitialization_date_time;

	struct GPS_OldStyleRec sGPS_OldStyleRec;
/////////////////
//
// Functions
//
/////////////////

void doGPSEVENT_OLDREC(bool DoOutfile, FILE *outfile)
{

//	CJulianTime Julian;
//	char chCreationTime[64];
//	Julian.Init(sGPS_OldStyleRec.dDateTime);
//	strcpy(chCreationTime, Julian.Date());
//	strcat(chCreationTime, " ");
//	strcat(chCreationTime, Julian.Time());
	COleDateTime cDateTime;
	cDateTime.m_dt = sGPS_OldStyleRec.dDateTime;
	cDateTime.SetStatus(COleDateTime::valid);

//	char chGPSDateTime[64];
//	sprintf(chGPSDateTime, "%s %s", Julian.Date(),Julian.Time());
//	strcpy(chGPSDateTime,cDateTime.Format("%Y\\%m\\%d %H:%M:%S"));

	if (DoOutfile)
	{
		fprintf(outfile,"\n\n");
		fprintf(outfile,">>> GPS EVENT RECORD %d\n\n", iEventRecordCount);
		fprintf(outfile, "        Time of record: %s\n",   cDateTime.Format("%Y.%m.%d %H:%M:%S"));
		fprintf(outfile, "           Status Byte: %02x\n", sGPS_OldStyleRec.StatusByte);
		fprintf(outfile, "    Latitude (degrees): %Lf\n",   sGPS_OldStyleRec.dLatitude);
		fprintf(outfile, "   Longitude (degrees): %Lf\n",   sGPS_OldStyleRec.dLongitude);
		fprintf(outfile, "     Altitude (meters): %Lf\n",   sGPS_OldStyleRec.dAltitude);
		fprintf(outfile, "             Satellite: %d\n",	sGPS_OldStyleRec.iSatellite);
		fprintf(outfile, "   Horizontal Dilution: %Lf\n",   sGPS_OldStyleRec.dHorDilution);
		fprintf(outfile, "              Duration: %Lf\n",   sGPS_OldStyleRec.dDuration);
	}

/*
	double			dDateTime;		//8 bytes
	unsigned short	StatusByte;		//2 bytes
	double			dLongitude;		//8 bytes
	double			dLatitude;		//8 bytes
	double			dAltitude;		//8 bytes
	int				iSatellite;		//4 bytes
	double			dHorDilution;	//8 bytes
	double			dDuration;		//8 bytes
*/

}

void doGPSEVENT_REC(bool DoOutfile, FILE *outfile)
{
	GPS_Rec *pInfo = (GPS_Rec *)(&event_acquire_record.bData);

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

	char chGPSDateTime[64];
	sprintf(chGPSDateTime, "%04d.%02d.%02d %02d:%02d:%02d",
		pInfo->usYear, pInfo->bMonth, pInfo->bDay,
		pInfo->bHour,pInfo->bMinute,pInfo->bSecond);

	unsigned int uiFixTimeA = (unsigned int)pInfo->fFixTimeA;
	unsigned int uiFixTimeB = (unsigned int)pInfo->fFixTimeB;

	if (DoOutfile)
	{
		fprintf(outfile,"\n\n");
		fprintf(outfile,">>> GPS EVENT RECORD %d\n\n", iEventRecordCount);
		fprintf(outfile, "                           Event record type: %s\n",   chRecordType);
		fprintf(outfile, "                     Time of record creation: %s\n",   chCreationTime);
		fprintf(outfile, "                            ILON node number: %u\n",   pInfo->usNode);
		fprintf(outfile, "                          Latitude (degrees): %f\n",   pInfo->dLatitude);
		fprintf(outfile, "                         Longitude (degrees): %f\n",   pInfo->dLongitude);
		fprintf(outfile, "                           Altitude (meters): %f\n",   pInfo->dAltitude);
		fprintf(outfile, "                           Fix date and time: %s\n",   chGPSDateTime);
		fprintf(outfile, "                        Satellites (bUnused): %u\n",   pInfo->bUnused);
		fprintf(outfile, "                                  Fix Time A: %d\n",   uiFixTimeA);
		fprintf(outfile, "               East velocity (meters/second): %f\n",   pInfo->fEastVelocity);
		fprintf(outfile, "              North velocity (meters/second): %f\n",   pInfo->fNorthVelocity);
		fprintf(outfile, "                 Up velocity (meters/second): %f\n",   pInfo->fUpVelocity);
		fprintf(outfile, "  Clock Bias Rate (meters/(second * second)): %f\n",   pInfo->fClockBiasRate);
		fprintf(outfile, "                Fix Time B (Seconds in week): %d\n",   uiFixTimeB);
		fprintf(outfile, "                                    Checksum: 0x%02x\n", pInfo->bChecksum);
	}
}

////////////////////////
//
// Decode One GPS Record
//
////////////////////////
void DecodeOneGPSRecord(
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	size_t gps_record_size = sizeof(GPS_Rec) + sizeof(event_acquire_record.usSize);

	if (fread(&event_acquire_record,gps_record_size,1,infile))
	{
		// spinner
//			char ch;
//			char backspace = 8;
//			switch (iRecordCount & 0x000000c0) 
//				{
//					case 0x00000000: ch = '|'; break;
//					case 0x00000040: ch = '/'; break;
//					case 0x00000080: ch = '-'; break;
//					case 0x000000c0: ch = '\\'; break;
//				}
//			cerr << backspace << ch << flush;


		iRecordCount++;

		char *pch = (char *)&event_acquire_record.bData;
		char RecTypeA = *pch++;
		char RecTypeB = *pch;
		unsigned int usMessageType = RecTypeB + (RecTypeA << 8);

		switch (usMessageType & 0x0000ffff)
		{
			case GPSDATA_REC:	//	0x3341
				iEventRecordCount++;
				doGPSEVENT_REC(DoOutfile, outfile);
				break;

			default:;
//				iEventRecordCount++;
//				doGPSEVENT_OLDREC(DoOutfile, outfile);

		} //end switch
	}
}
void DecodeOneOldStyleGPSRecord(
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	if (fread(&sGPS_OldStyleRec,sizeof(GPS_OldStyleRec),1,infile))
	{
		iRecordCount++;

		iEventRecordCount++;
		doGPSEVENT_OLDREC(DoOutfile, outfile);
	}
}

void WriteGPSSummaryInformationToConsole(	
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	printf("\n");
	printf("                    File Name: %s\n",FileName);
	printf("                    File Type: GPS Events\n");
	printf("      Total Number of Records: %d\n",iRecordCount);
	printf("      Number of Event Records: %d\n",iEventRecordCount);;
}

void WriteGPSSummaryInformationToOutputFile(	
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	fprintf(outfile,"\n\n");
	fprintf(outfile,">>> SUMMARY\n\n");
	fprintf(outfile,"                    File Name: %s\n",FileName);
	fprintf(outfile,"                    File Type: GPS Events\n");
	fprintf(outfile,"      Total Number of Records: %d\n",iRecordCount);
	fprintf(outfile,"      Number of Event Records: %d\n",iEventRecordCount);
}

////////////////////////
//
// Decode GPS Header
//
////////////////////////

void DecodeGPSHeader(
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
		fprintf(outfile,"File Type: GPS Events\n\n");
		fprintf(outfile,">>> HEADER\n\n");
		fprintf(outfile," Size of Header: %s\n",temp);

		strncpy(temp,event_acquire_header.unused1,5);
		temp[5] = NULL;
		fprintf(outfile,"        unused1: %s\n",temp);

		strncpy(temp,event_acquire_header.Version,5);
		temp[5] = NULL;
		fprintf(outfile,"    MIC Version: %s\n",temp);
		float fType;
		sscanf(temp,"%f",&fType);
		g_bGPS_OldStyle = (fType < 1.799)?true:false;

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

void InitializeGPSFileVariables()
{
	iRecordCount = 0;
	iEventRecordCount = 0;
}

void CloseOutGPSFileVariables()
{
	int x = 0;
}