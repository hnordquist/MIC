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

#include "stdafx.h"

#include <stdio.h>
//#include <conio.h>
//#include <string.h>
//#include <math.h>
//#include <stdlib.h>
//#include <io.h>
#include <iostream>
#include <assert.h>
#include "JulianT.h"
#include <AFXDISP.h>  //for COleDateTime
#include "BI0.h"


bool BI0Processor::mWriteBIDfromBI0 = false;
bool BI0Processor::mSkipSubSecondStart = false;

void BI0Processor::DecodeBI0Header(
								   char *FileName, 
								   FILE *infile, 
								   bool bDoAnalysis, 
								   FILE *outfile)
{
	BI0Header bioh;
	fread(&bioh,sizeof(BI0Header),1,infile);
	if (bDoAnalysis)
	{
		char temp[128];

		fprintf(outfile,"File Name: %s\n",FileName);
		fprintf(outfile,"File Type: DMG BI0\n");
		fprintf(outfile,"Header\n");
		fprintf(outfile," Size of Header: %d\n",sizeof(BI0Header));

		strncpy(temp,bioh.InsignificantChars,14);
		temp[14] = NULL;
		fprintf(outfile,"        unused: %s\n",temp);

		strncpy(temp,bioh.Unit,3);
		temp[3] = NULL;
		fprintf(outfile,"     Station ID: %s\n",temp);

		strncpy(temp,bioh.Year,3);
		temp[3] = NULL;
		fprintf(outfile,"           Year: %s\n",temp);

		strncpy(temp,bioh.Month,3);
		temp[3] = NULL;
		fprintf(outfile,"          Month: %s\n",temp);

		strncpy(temp,bioh.Day,3);
		temp[3] = NULL;
		fprintf(outfile,"            Day: %s\n",temp);

		strncpy(temp,bioh.FullYear,4);
		temp[4] = NULL;
		fprintf(outfile,"      Full Year: %s\n",temp);

		strncpy(temp,bioh.unused2,43);
		temp[43] = NULL;
		fprintf(outfile,"        unused2: %s\n",temp);
	}

	if (mWriteBIDfromBI0 && mOutBIDFile == NULL) 
	{
		char path_buffer[_MAX_PATH];
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
		_makepath(path_buffer,drive,dir,fname,"bid"); // combine path components, fname and extension

		if ((mOutBIDFile = fopen(path_buffer,"wb")) == NULL)
		{
			//m_cMessage += "Cannot open output file!";
			return;
		}
		char buff[80];
		sprintf(buff,"  69      2009 %c%c %c%c %c%c %c%c%c%c%c%c%42c",
			bioh.Unit[1],bioh.Unit[2],bioh.Year[1],bioh.Year[2],bioh.Month[1],bioh.Month[2],
			bioh.Day[1], bioh.Day[2],bioh.FullYear[0],bioh.FullYear[1],bioh.FullYear[2],bioh.FullYear[3],' ');
		fwrite(buff, sizeof(char), 73, mOutBIDFile);
	}
}



void BI0Processor::DecodeOneBI0DataRecord(
	char *FileName,
	FILE *infile, 
	bool bDoAnalysis, 
	FILE *outfile)
{
	BI0DataRecordHeader biorh;
	char  biff[512];

	unsigned int tag;
	while (fread(&tag,sizeof(unsigned int),1,infile)) // scan to next data record
	{
		if (tag ^ 0xFFFFFFFF) // anything but the mask
		{
			sprintf(biff,"0x%08x; skipping\n",tag);
			OutputDebugString(biff);
		}
		else
			break;
	}

	if (fread(&biorh,sizeof(BI0DataRecordHeader),1,infile)) // got a data record
	{
		unsigned __int16 DCSv = 0, JSCSv = 0; 

		// doin that endian boogie thang
		biorh.JulianSeconds = _byteswap_ulong(biorh.JulianSeconds);
		biorh.JSCS = _byteswap_ushort(biorh.JSCS);
		byte* time = (byte*)(&biorh);	
		for (int i = 0; i < 4; i++)	JSCSv += time[i];
		bool timebad, databad;
		timebad = (JSCSv != biorh.JSCS);

		CJulianTime JulienneSchool;
		JulienneSchool.Init(biorh.JulianSeconds);  // convert time to time

		if (biorh.OpCode != 0x15)  // not an acquire record, read through it
		{
			byte dross[1024];
			fread(dross,biorh.Len - 4,1,infile); // data starts after the opcode, len was data len + 2 opcode + 2 data cs, so read len-4 to get to end of data, 2 more for data cs
			unsigned short datacs;
			fread(&datacs,2,1,infile);
			for (int i = 0; i < (biorh.Len - 4); i++) DCSv += dross[i];	
			datacs= _byteswap_ushort(datacs);
			databad = (DCSv != datacs);
			if (databad && timebad) // lame, gotta be a better way
				sprintf(biff,"%s %s %10u 0x%03x 0x%01x %d %d t|%03x!=%03x d|%03x!=%03x\n",
					JulienneSchool.Date(),JulienneSchool.Time(), biorh.JulianSeconds, biorh.JSCS, biorh.OpCode, biorh.OpCode, biorh.Len-4, JSCSv, biorh.JSCS, DCSv, datacs);
			else if (databad)
				sprintf(biff,"%s %s %10u 0x%03x 0x%01x %d %d d|%03x!=%03x\n",
					JulienneSchool.Date(),JulienneSchool.Time(),biorh.JulianSeconds, biorh.JSCS, biorh.OpCode, biorh.OpCode, biorh.Len-4, DCSv, datacs);
			else if (timebad)
				sprintf(biff,"%s %s %10u 0x%03x 0x%01x %d %d t|%03x!=%03x\n",
					JulienneSchool.Date(),JulienneSchool.Time(), biorh.JulianSeconds, biorh.JSCS, biorh.OpCode, biorh.OpCode, biorh.Len-4, JSCSv, biorh.JSCS);
			else
				sprintf(biff,"%s %s %10u 0x%03x 0x%01x %d %d\n",
					JulienneSchool.Date(),JulienneSchool.Time(),biorh.JulianSeconds, biorh.JSCS, biorh.OpCode, biorh.OpCode, biorh.Len-4);
			mNonAcqRecords++;
		}
		else if (biorh.OpCode == 0x15)  // acquire record
		{ 
			BI0DataAcqRecord bior;
			fread(&bior,sizeof(BI0DataAcqRecord),1,infile);

			bior.DCS = _byteswap_ushort(bior.DCS);
			bior.StatusBytes  = _byteswap_ushort(bior.StatusBytes);
			bior.NeutronChA.ul = _byteswap_ulong(bior.NeutronChA.ul);
			bior.NeutronChB.ul = _byteswap_ulong(bior.NeutronChB.ul);
			bior.NeutronChC.ul = _byteswap_ulong(bior.NeutronChC.ul);
			bior.GammaCh1.ul = _byteswap_ulong(bior.GammaCh1.ul);
			bior.GammaCh1Sigma.ul = _byteswap_ulong(bior.GammaCh1Sigma.ul);
			bior.GammaCh2.ul = _byteswap_ulong(bior.GammaCh2.ul);
			bior.GammaCh2Sigma.ul = _byteswap_ulong(bior.GammaCh2Sigma.ul);
			bior.CountTime  = _byteswap_ulong(bior.CountTime);

			byte* data = (byte*)(&bior);
			for (int i = 0; i < (biorh.Len - 4); i++) DCSv += data[i];	
			databad = (DCSv != bior.DCS);

			if (databad && timebad) // lame, gotta be a better way
			{
				mSkippedRecords++;
				sprintf(biff,"%s %s %10u 0x%02x! 0x%02x %10f %10f %10f %10f %10f %10f %10f %d 0x%02x!\n",
					JulienneSchool.Date(),JulienneSchool.Time(), biorh.JulianSeconds,biorh.JSCS,bior.StatusBytes, 
					bior.NeutronChA.f,bior.NeutronChB.f,(bior.NeutronChC.f), (bior.GammaCh1.f),(bior.GammaCh1Sigma.f),(bior.GammaCh2.f),bior.GammaCh2Sigma.f,
					bior.CountTime,	bior.DCS);
			}
			else if (databad)
			{
				mSkippedRecords++;
				sprintf(biff,"%s %s %10u 0x%03x 0x%02x %10f %10f %10f %10f %10f %10f %10f %d 0x%02x!\n",
					JulienneSchool.Date(),JulienneSchool.Time(), biorh.JulianSeconds,biorh.JSCS,bior.StatusBytes, 
					bior.NeutronChA.f,bior.NeutronChB.f,(bior.NeutronChC.f), (bior.GammaCh1.f),(bior.GammaCh1Sigma.f),(bior.GammaCh2.f),bior.GammaCh2Sigma.f,
					bior.CountTime,	bior.DCS);
			}
			else if (timebad)
			{
				mSkippedRecords++;
				sprintf(biff,"%s %s %10u 0x%02x! 0x%02x %10f %10f %10f %10f %10f %10f %10f %d 0x%02x\n",
					JulienneSchool.Date(),JulienneSchool.Time(), biorh.JulianSeconds,biorh.JSCS,bior.StatusBytes, 
					bior.NeutronChA.f,bior.NeutronChB.f,(bior.NeutronChC.f), (bior.GammaCh1.f),(bior.GammaCh1Sigma.f),(bior.GammaCh2.f),bior.GammaCh2Sigma.f,
					bior.CountTime,	bior.DCS);
			}
			else
			{
				mValidRecords++;
				sprintf(biff,"%s %s %10u 0x%03x 0x%02x %10f %10f %10f %10f %10f %10f %10f %d 0x%02x\n",
					JulienneSchool.Date(),JulienneSchool.Time(), biorh.JulianSeconds,biorh.JSCS,bior.StatusBytes, 
					bior.NeutronChA.f,bior.NeutronChB.f,(bior.NeutronChC.f), (bior.GammaCh1.f),(bior.GammaCh1Sigma.f),(bior.GammaCh2.f),bior.GammaCh2Sigma.f,
					bior.CountTime,	bior.DCS);
			}

			if (bDoAnalysis)
			{
				if (!(databad && timebad))  // test and count the good ones only
				{
					if (RecordCount == 0)
					{
						FirstTime = biorh.JulianSeconds;
						Interval  = bior.CountTime;
					}
					if (Interval != bior.CountTime) Interval = -1;
					LastTime = biorh.JulianSeconds;
					RecordCount++;
					TotalCountTime += bior.CountTime;
					if (biorh.JulianSeconds < uiPreviousJulian)
					{
						fprintf(outfile,"Preceding Record Out of Order Error\n");
						RecordNumber++;
					}
					if (uiPreviousJulian && (abs(int(biorh.JulianSeconds - uiPreviousJulian - uiPreviousCountTime)) > iAcceptGap))
					{
						fprintf(outfile,"Preceding Record Gap Error: %d\n",biorh.JulianSeconds - uiPreviousJulian - uiPreviousCountTime);
						GapNumber++;
					}
				}
				uiPreviousJulian = biorh.JulianSeconds; 
				uiPreviousCountTime = bior.CountTime;
			}

			if (mWriteBIDfromBI0 && mOutBIDFile)
			{
				if (mSkipSubSecondStart && (mSkipStartCount < 5)) // skip first 5 when writing to the BID file (RP: skip the first five to avoid ms problems in Rad Review)
				{
					mSkipStartCount++;
				}
				else
				{
					MyLameCopyToKeepFromChangingSourceCodeAcquireRecord acq;
					acq.JulianSeconds = biorh.JulianSeconds;	//4 bytes
					acq.StatusByte = bior.StatusBytes;		//2 bytes
					acq.NeutronChA = bior.NeutronChA.f;		//4 bytes
					acq.NeutronChB = bior.NeutronChB.f;		//4 bytes
					acq.NeutronChC = bior.NeutronChC.f;		//4 bytes
					acq.GammaCh1 = bior.GammaCh1.f;		//4 bytes
					acq.GammaCh1Sigma=bior.GammaCh1Sigma.f;	//4 bytes
					acq.GammaCh2= bior.GammaCh2.f;		//4 bytes
					acq.GammaCh2Sigma= bior.GammaCh2Sigma.f;	//4 bytes
					acq.CountTime= bior.CountTime;		//2 bytes
					fwrite(&acq, 1, sizeof(MyLameCopyToKeepFromChangingSourceCodeAcquireRecord), mOutBIDFile);
				}
			}
		}
		fprintf(outfile,biff);
	}
}

void  BI0Processor::WriteBI0SummaryInformationToOutputFile(char *FileName, FILE *infile, bool DoOutfile, FILE *outfile)
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
