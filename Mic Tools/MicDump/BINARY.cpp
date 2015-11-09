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
// file Binary.cpp
#include "stdafx.h"

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <assert.h>
#include "JulianT.h"
#include <AFXDISP.h>  //for COleDateTime

#include "EVENT.H"

////////////////
//
// Variables
//
////////////////
	static EventAcquireHeader event_acquire_header;  //73 bytes
	static EventAcquireRecord event_acquire_record;  //73 bytes

	static int iRecordCount;
	static int iEventRecordCount;
/////////////////
//
// Functions
//
/////////////////

void doBinaryEVENT_REC(bool DoOutfile, FILE *outfile)
{
	BinaryEventRec *pInfo = (BinaryEventRec *)(&event_acquire_record.bData);

	// record type = "32" or 0x3332
	char chRecordType[3];
	chRecordType[0] = pInfo->RecTypeA;
	chRecordType[1] = pInfo->RecTypeB;
	chRecordType[2] = NULL;

	CJulianTime Julian;
	char chTime[64];
	Julian.Init(pInfo->uiTime);
	strcpy(chTime, Julian.Date());
	strcat(chTime, " ");
	strcat(chTime, Julian.Time());

	if (DoOutfile)
	{
		fprintf(outfile,"\n\n");
		fprintf(outfile,">>> BINARY EVENT RECORD %d\n\n", iEventRecordCount);
		fprintf(outfile, "     Event record type: %s\n",       chRecordType);
		fprintf(outfile, "                  Time: %s\n",       chTime);
		fprintf(outfile, "                Status: 0x%02x\n",   pInfo->bStatus);
		fprintf(outfile, "           Node number: %u\n",       pInfo->usNode);
		fprintf(outfile, "                 State: 0x%02x\n",   pInfo->bState);
		fprintf(outfile, "                  Mask: 0x%02x\n",   pInfo->bMask);
		fprintf(outfile, "              Reserved: 0x%02x\n",   pInfo->bReserved);
		fprintf(outfile, "              Checksum: 0x%02x\n", pInfo->bChecksum);
	}
}

////////////////////////
//
// Decode One Binary Record
//
////////////////////////
void DecodeOneBinaryRecord(
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	size_t binary_record_size = sizeof(BinaryEventRec) + sizeof(event_acquire_record.usSize);

	if (fread(&event_acquire_record,binary_record_size,1,infile))
	{
		iRecordCount++;

		char *pch = (char *)&event_acquire_record.bData;
		char RecTypeA = *pch++;
		char RecTypeB = *pch;
		unsigned int usMessageType = RecTypeB + (RecTypeA << 8);

		switch (usMessageType)
		{
			case BINARYEVENT_REC:	//	0x3332
				iEventRecordCount++;
				doBinaryEVENT_REC(DoOutfile, outfile);
				break;

//			default:
//				assert(false);

		} //end switch
	}
}

void WriteBinarySummaryInformationToConsole(	
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	printf("\n");
	printf("                    File Name: %s\n",FileName);
	printf("                    File Type: Binary Events\n");
	printf("      Total Number of Records: %d\n",iRecordCount);
	printf("      Number of Event Records: %d\n",iEventRecordCount);
}

void WriteBinarySummaryInformationToOutputFile(	
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	fprintf(outfile,"\n\n");
	fprintf(outfile,">>> SUMMARY\n\n");
	fprintf(outfile,"                    File Name: %s\n",FileName);
	fprintf(outfile,"                    File Type: Binary Events\n");
	fprintf(outfile,"      Total Number of Records: %d\n",iRecordCount);
	fprintf(outfile,"      Number of Event Records: %d\n",iEventRecordCount);
}

////////////////////////
//
// Decode Binary Header
//
////////////////////////

void DecodeBinaryHeader(
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
		fprintf(outfile,"File Type: Binary Events\n\n");
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

void InitializeBinaryFileVariables()
{
	iRecordCount = 0;
	iEventRecordCount = 0;
}

void CloseOutBinaryFileVariables()
{
}
