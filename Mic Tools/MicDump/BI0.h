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

#pragma once

#pragma pack(1)

struct MyLameCopyToKeepFromChangingSourceCodeAcquireRecord
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

struct BI0Header
{
	char	InsignificantChars[14];
	char	Unit[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	FullYear[4];
	char	unused2[43];
};

typedef struct ulf {
	union {
		unsigned __int32	ul;		//4 bytes*
		float	f;		
	};
} ulfpair;

struct BI0DataRecordHeader
{
	unsigned __int32 JulianSeconds;
	unsigned __int16 JSCS;
	byte Len;
	byte OpCode;
};

struct BI0DataAcqRecord
{
	unsigned __int16	StatusBytes;	//2 bytes

	ulfpair NeutronChA, NeutronChB, NeutronChC;
	ulfpair GammaCh1, GammaCh1Sigma;
	ulfpair GammaCh2, GammaCh2Sigma;

	unsigned __int32	CountTime;		//4 bytes
	unsigned __int16	DCS;
};


#pragma pack()

class BI0Processor 
{
public:
	BI0Processor(int GapInterval, const CString& tg): mOutBIDFile(NULL), iAcceptGap(GapInterval), m_sTargetFolder(tg) {
		Gap	= RecordNumber = RecordCount = 0;
		GapNumber = TotalCountTime = 0;
		FirstTime = LastTime = Interval= 0;
		uiPreviousJulian = uiPreviousCountTime = 0;
		mValidRecords = mSkippedRecords = mNonAcqRecords = 0;
		mSkipStartCount = 0;
	}
	~BI0Processor() {if (mOutBIDFile) fclose(mOutBIDFile); mOutBIDFile = NULL; }

	static bool mWriteBIDfromBI0, mSkipSubSecondStart; 

	void DecodeBI0Header(char *FileName,FILE *infile,bool bDoAnalysis,FILE *outfile);
	void DecodeOneBI0DataRecord(char *FileName,FILE *infile, bool bDoAnalysis, FILE *outfile);
	void WriteBI0SummaryInformationToOutputFile(char *FileName, FILE *infile, bool DoOutfile, FILE *outfile);
	unsigned int  mValidRecords, mSkippedRecords, mNonAcqRecords;

private:
	FILE* mOutBIDFile;
	int Gap, RecordNumber, RecordCount, GapNumber, TotalCountTime, FirstTime, LastTime, Interval;
	int iAcceptGap, mSkipStartCount;
	unsigned int uiPreviousJulian;
	unsigned int uiPreviousCountTime;
	CString m_sTargetFolder;
};
