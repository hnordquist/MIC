
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
