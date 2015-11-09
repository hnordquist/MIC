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
#include <iostream.h>
#include <assert.h>
#include "JulianT.h"
#include <AFXDISP.h>  //for COleDateTime

#include "EVENT.H"

////////////////
//
// Variables
//
////////////////
#pragma pack(1)
struct  GAM_CHN_HEADER 
{
	short   flag;              /* must be -1 */
	short   mca_number;
	short   segment_number;
	char    seconds[2];        /* ASCII seconds of start time */
	long    real_time;         /* in increments of 20 ms */
	long    live_time;         /* in increments of 20 ms */
	char    start_date[8];     /* DDMMMYYn where n designates the century */
	char    start_time[4];     /* HHMM */
	short   channel_offset;
	short   num_channels;
} sCHNHeader;
#pragma pack()

char   szString[8];
int    iYear;
char szMonth[4];
char szDay[3];
char szTime[9];


/////////////////
//
// Functions
//
/////////////////

////////////////////////
//
// Decode One Binary Record
//
////////////////////////
void DecodeOneCHNRecord(
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	int iValue;
	for (int i = 0; i < sCHNHeader.num_channels; i++)
	{
		if (fread(&iValue,sizeof(int),1,infile)==1)
			fprintf(outfile,"%d\n",iValue);
		else
		{
			fprintf(outfile,"Failed to read full spectrum\n");
			i = sCHNHeader.num_channels;
		}
	}
	while (!feof(infile))
		fread(&iValue,sizeof(int),1,infile);
}

void WriteCHNSummaryInformationToConsole(	
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	printf("\n");
	printf("                    File Name: %s\n",FileName);
	printf("                    File Type: CHN Spectra\n");
	printf("                   Start Date: %d %s %s\n",iYear,szMonth,szDay);
	printf("                   Start Time: %s\n",szTime);
	printf("                    Real Time: %d\n",sCHNHeader.live_time / 50L);
	printf("                    Live Time: %d\n",sCHNHeader.real_time / 50L);
	printf("               Channel Offset: %d\n",sCHNHeader.channel_offset);
	printf("               Number of Chan: %d\n",sCHNHeader.num_channels);
}

void WriteCHNSummaryInformationToOutputFile(	
	char *FileName,
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{
	fprintf(outfile,"File Name: %s\n",FileName);
	fprintf(outfile,"     Start Date: %d %s %s\n",iYear,szMonth,szDay);
	fprintf(outfile,"     Start Time: %s\n",szTime);
	fprintf(outfile,"      Real Time: %d\n",sCHNHeader.live_time / 50L);
	fprintf(outfile,"      Live Time: %d\n",sCHNHeader.real_time / 50L);
	fprintf(outfile," Channel Offset: %d\n",sCHNHeader.channel_offset);
	fprintf(outfile," Number of Chan: %d\n",sCHNHeader.num_channels);
}

////////////////////////
//
// Decode Binary Header
//
////////////////////////

void DecodeCHNHeader(
	char *FileName, 
	FILE *infile, 
	bool DoOutfile, 
	FILE *outfile)
{

	if (fread(&sCHNHeader,sizeof(sCHNHeader),1,infile) == 1)
	{
		szString[0] = sCHNHeader.start_date[5];
		szString[1] = sCHNHeader.start_date[6];
		szString[2] = '\0';
		iYear = atoi( szString );
		int century=19;
		if ( sCHNHeader.start_date[7]>='1'  &&  sCHNHeader.start_date[7]<='9' ) 
			century += (int)(sCHNHeader.start_date[7] - '0');
		iYear += 100 * century;

		szMonth[0] = sCHNHeader.start_date[2];
		szMonth[1] = sCHNHeader.start_date[3];
		szMonth[2] = sCHNHeader.start_date[4];
		szMonth[3] = NULL;

		szDay[0] = sCHNHeader.start_date[0];
		szDay[1] = sCHNHeader.start_date[1];
		szDay[2] = NULL;

		szTime[0] = sCHNHeader.start_time[0];
		szTime[1] = sCHNHeader.start_time[1];
		szTime[2] = ':';
		szTime[3] = sCHNHeader.start_time[2];
		szTime[4] = sCHNHeader.start_time[3];
		szTime[5] = ':';
		szTime[6] = sCHNHeader.seconds[0];
		szTime[7] = sCHNHeader.seconds[1];
		szTime[8] = NULL;

	}
}

void InitializeCHNFileVariables()
{
}

void CloseOutCHNFileVariables()
{
}