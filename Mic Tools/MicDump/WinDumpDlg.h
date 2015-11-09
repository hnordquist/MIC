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
// WinDumpDlg.h : header file
//

#if !defined(AFX_WINDUMPDLG_H__64BAAE51_B8C8_4305_90CB_13909A65E55B__INCLUDED_)
#define AFX_WINDUMPDLG_H__64BAAE51_B8C8_4305_90CB_13909A65E55B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MGD_Records.h"
#include "CmdLine.h"

class CWinDumpDlgAutoProxy;

#pragma pack(push)
#pragma pack(1)

typedef unsigned long ADDRESS;

typedef struct tagTIME {
    unsigned short HalfDays;
    unsigned short Seconds;
} EOSSTIME;

typedef unsigned char DESBLOCK[8];

typedef struct tagGETCHALLENGE {
    unsigned short Type;
} GETCHALLENGE;

/*
typedef struct tagCHALLENGE {
    unsigned short Type;
    unsigned long Seal;         // seal serial number
    unsigned long KeySetNumber; // key-set number
    EOSSTIME      KeySetTime;   // date and time of key-set generation
    unsigned char Version1;     // major version number
    unsigned char Version2;     // minor version standard
    DESBLOCK Token;
} CHALLENGE;
*/
typedef struct tagTIMEDATA {
    unsigned short HalfDays;
    unsigned short Seconds;
    unsigned long Reserved;
} TIMEDATA;

typedef struct tagINITIALS {
   unsigned short Operator1;  // ID of operator 1
   unsigned short Operator2;  // ID of operator 2 or zero
   unsigned short Reserved2;
   unsigned short Reserved3;
} INITIALS;

#define FLAG_FOOPEN  1
#define FLAG_FOFAIL  2
#define FLAG_HOPEN	4
#define FLAG_SHORT	8
#define FLAG_BROKEN 16

typedef struct tagSTATUS {
   ADDRESS Seal;					//4		0
   EOSSTIME Time;					//2+2	4

   unsigned short Operator1;		//2		8
   unsigned short Operator2;		//2		10
   unsigned char Flags;				//1		12
   signed char Temperature;			//1		13
   unsigned char EmergencyBattery;	//1		14
   unsigned char MainBattery;		//1		15

   unsigned short FlashWarnings;	//2
   unsigned short FlashErrors;		//2
   unsigned short MajoWarnings;		//2
   unsigned short MajoErrors;		//2

   unsigned short EPROMCrc16;		//2
   unsigned char BatID[8];			//8
   char BatteryType[8];				//8
   unsigned short BatteryMfg;		//2
   unsigned long BatteryConsumed;	//4

   unsigned short EBatteryMfg;		//2
   unsigned long EBatteryConsumed;	//4
   unsigned short BatteryInstalled;	//2

   unsigned short EBatteryInstalled;//2
   signed short T0Offset;			//2
   signed short F0Offset;			//2
   unsigned short LaserThreshold;	//2

   unsigned short Laser500uW;		//2
   unsigned short LaserPeak;		//2
   unsigned short LaserAdjDate;		//2
   char LaserId[16];				//16
   unsigned long VacossAttachments;	//4
   unsigned char VacossEnabled;		//1

   unsigned char Reserved[5];		//5
} STATUS;

#define PAGE_SIZE 64
#define PAGES_TOTAL 8192 / PAGE_SIZE

typedef struct tagPAGEHEADER {
   unsigned short Seal;
   unsigned char Log;
   unsigned char Page;
   EOSSTIME Time;
   DESBLOCK Authenticator;
} PAGEHEADER;

typedef PAGEHEADER * PPAGEHEADER;

typedef EOSSTIME SEALEVENT;

typedef struct tagSYSTEMEVENT {
	EOSSTIME Time;
	unsigned short Code;
} SYSTEMEVENT;

typedef struct tagSOHRECORD {
      unsigned short Date;
      signed char MinTemperature;
      signed char MaxTemperature;
      unsigned char EmergencyBattery;
      unsigned char MainBattery;
      unsigned char OnBatteries;
      unsigned char MVEvents;
} SOHRECORD;

#define SEALEVENTS_PER_PAGE ((PAGE_SIZE - sizeof(PAGEHEADER)) / sizeof(SEALEVENT))
#define SYSTEMEVENTS_PER_PAGE ((PAGE_SIZE - sizeof(PAGEHEADER)) / sizeof(SYSTEMEVENT))
#define SOHRECORDS_PER_PAGE ((PAGE_SIZE - sizeof(PAGEHEADER)) / sizeof(SOHRECORD))

typedef struct {
	PAGEHEADER Header;
	SEALEVENT Entries[SEALEVENTS_PER_PAGE];
} SEALLOGPAGE;

typedef struct tagSYSTEMLOGPAGE {
	PAGEHEADER Header;
	SYSTEMEVENT Entries[SYSTEMEVENTS_PER_PAGE];
} SYSTEMLOGPAGE;

typedef struct tagSOHLOGPAGE {
	PAGEHEADER Header;
	SOHRECORD Entries[SOHRECORDS_PER_PAGE];
} SOHLOGPAGE;

typedef union {
	SEALLOGPAGE SealLogPage;
	SYSTEMLOGPAGE SystemLogPage;
	SOHLOGPAGE SoHLogPage; 
} LOGPAGE;

typedef struct tagLOGBOOKINFO {
   unsigned long SealEvents;			//4
   unsigned char SealLogFirstPage;		//1
   unsigned char SealLogLastPage;		//1
   unsigned char SealLogStartPage;		//1
   unsigned char SealLogCurrentPage;	//1

   unsigned long SystemEvents;			//4
   unsigned char SystemLogFirstPage;	//1	
   unsigned char SystemLogLastPage;		//1
   unsigned char SystemLogStartPage;	//1
   unsigned char SystemLogCurrentPage;	//1

   unsigned short SoHRecords;			//2
   unsigned char SoHLogFirstPage;		//1
   unsigned char SoHLogLastPage;		//1
   unsigned char SoHLogStartPage;		//1
   unsigned char SoHLogCurrentPage;		//1

   unsigned char TotalPadPages;			//1
   unsigned char ExStatusFlags;			//1

   unsigned long KeysetNumber;			//4
   EOSSTIME KeysetTime;					//4
} LOGINFO;


typedef struct tagTIMEREC
{
	double dt;
	COleDateTime::DateTimeStatus status;
	short sOffset;
	char cVersion[2];
} TIMEREC;

#define MAXNUMEOSS 32

#pragma pack(pop)

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlg dialog

class CWinDumpDlg : public CDialog
{
	DECLARE_DYNAMIC(CWinDumpDlg);
	friend class CWinDumpDlgAutoProxy;

// Construction
public:
	CWinDumpDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CWinDumpDlg();
	CString m_cMessage;
	CString m_sTargetFolder;
// Dialog Data
	//{{AFX_DATA(CWinDumpDlg)
	enum { IDD = IDD_WINDUMP_DIALOG };
	CButton	m_AcceptGap;
	BOOL	m_bGapAnalysis;
	BOOL	m_bOpenOutput;
	int		m_iGapSeconds;

	//}}AFX_DATA


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinDumpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWinDumpDlgAutoProxy* m_pAutoProxy;
	enum Types {UNKNOWN,MGD,CHN,GRAND,MCA,ISR,JSR,PSR,HMR,VACOSS,BINARY,GPS,SOH,ESS,RAD,RFI,BI0} eThisType;
	HICON m_hIcon;
	BOOL CanExit();
	void DoIt(char* FileName, Types eThisType, BOOL bOpenFile, BOOL bDoAnalysis, int iAcceptGap);
	void DoAFile(char* FileName);
	void GeneralHexDump(char* FileName, FILE* infile, bool DoOutfile, FILE *outfile, int*RecordCount);
	void DecodeOneMGDRecord(char *FileName,FILE *infile,bool DoOutfile,FILE *outfile,int*piRecordCount);
	void DecodeOneESSRecordSet(char* /*FileName*/,FILE* infile,bool DoOutfile,FILE* outfile);
	void ReadAndPrintRADAcqRecord(BinaryRecordX* pXRecord,FILE*infile,FILE*outfile,int*piRecordCount); 
	void ReadAndPrintRFIAcqRecord(BinaryRecordX* pXRecord,FILE*infile,FILE*outfile,int*piRecordCount);
	void ReadAndPrintESBAcqRecord(BinaryRecordX* pXRecord,FILE*infile,FILE*outfile,int*piRecordCount); 
	BYTE m_bInstType;

	unsigned int m_EOSSTable[MAXNUMEOSS];
	STATUS m_StatusRec;
	TIMEREC m_TimeRec;
	LOGINFO m_LogInfoRec;
	SEALLOGPAGE m_SealLogPage;
	SYSTEMLOGPAGE m_SystemLogPage;
	SOHLOGPAGE	m_SOHLogPage;

	bool ReadHeader(FILE* infile, FILE* outfile);
	bool ReadEOSSTable(FILE* infile, FILE* outfile);
	bool ReadStatusRec(FILE* infile, FILE* outfile);
	bool ReadTimeRec(FILE* infile, FILE* outfile);
	bool ReadLogInfoRec(FILE* infile, FILE* outfile);
	bool ReadSealLogPage(FILE* infile, FILE* outfile);
	bool ReadSystemLogPage(FILE* infile, FILE* outfile);
	bool ReadSOHLogPage(FILE* infile, FILE* outfile);
	void ProcessSealEvent(int iEventNumber, SEALEVENT* pSealEvent, FILE* infile, FILE* outfile);
	void ProcessSystemEvent(SYSTEMEVENT* pSystemEvent, FILE* infile, FILE* outfile);
	void ProcessSOHRecord(SOHRECORD* pSOHRecord, FILE* infile, FILE* outfile);
	bool Unattended(unsigned short usCode);
	void EossToSystemTime(SYSTEMTIME * pSystemTime, EOSSTIME * pEossTime);
	CString GetEossMessage(unsigned short usCode);
	void UnMakeID(unsigned short Id, char * szOperator);

	TransformCmdLine mTcl;
	void BuildFileList(CStringList& list, CString dir, const CString& pattern);
	bool HasSuffix(const CString& fname, const CString& sfx);

	// Generated message map functions
	//{{AFX_MSG(CWinDumpDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckGapanalysis();
	afx_msg LRESULT BatchProcessor(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDUMPDLG_H__64BAAE51_B8C8_4305_90CB_13909A65E55B__INCLUDED_)
