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
