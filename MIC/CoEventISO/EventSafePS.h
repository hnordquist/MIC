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
#ifndef EVENTSAFEPS_H
#define EVENTSAFEPS_H

#include "ISO_definitions.h"

// EventSafePS.h : header file
#include "ccbutton.h"

#define NUMBEROFACQUIRERECS		64
#define ACCUMULATOR_SIZE		2048
#define BINARYACCUMULATOR_SIZE	2048

//these don't actually correspond to the first chars of a message
#define UNKNOWN_REC				(0xf8)
#define FORCEDATETIME			(0xfa)
#define ECHO_RECORD				(0xfd)

#define ECHO_REC				0x00
#define ECHO_REC_LENGTH			6

#define DUMPOK_REC				0x3137
#define DUMPOK_REC_LENGTH		10

#define INQUIRE2_REC			0x3145
#define INQUIRE2_REC_LENGTH		26

#define STATUS_REC				0x3334
#define STATUS_REC_LENGTH		15

#define BINARYEVENT_REC			0x3332
#define VACOSSINFO_REC			0x3335
#define VACOSSEVENT_REC			0x3336
#define VACOSSERROR_REC			0x3339
#define GPSDATA_REC				0x3341
#define ENDDUMP_REC				0x3632

#ifdef RFIDDEMO
#define RADDATA_REC				0x3445
#define RAD2DATA_REC			0x3448
#define LGPSDATA_REC			0x3446
#define RFIDDATA_REC			0x3447
#endif

//This group from MIC 1.9.0.7 SafePS.h
#define STATUS_RECORD			(0x12)
#define ACQUIRE_RECORD			(0x15)
#define END_DUMP_RECORD			(0x16)
#define END_DUMP2_RECORD		(0x61)
#define DUMP_OK_RECORD			(0x17)
#define MONITOR_ID2_RECORD		(0x1e)
#define ECHO_3_REC				(0xff)
//End group from MIC 1.9.0.7 SafePS.h

//status byte 1
#define BBMERROR			0x0008
//status byte 2
#define BBMFULL				0x0008
#define BBMBADRECSIZE		0x0004
#define BBMBADWRITE			0x0002
#define BBMBADOPCODE		0x0001

//#define UNUSED			0x0002
//#define INMII				0x0001

#define FAIL				"Fail"
#define OK					"Ok"
#define LOW					"Low"
#define YES					"Yes"
#define NO					"No"
#define INMII_TXT			"In MII"
#define NOTINMII_TXT		"Not in MII"
#define START_COLD			"Cold Start"
#define START_NORMAL		"Normal"

//status byte 2
#define DATAFILTER_FLAG		0x0080
#define BBMISFULLEDER_FLAG	0x0040
#define BBMNUMOFBYTES_FLAG	0x0020
#define BBMWRITEERROR_FLAG	0x0010

//#define DATAFILTER_YES		"Filtered"
//#define DATAFILTER_NO		"No Filter"
#define	BBMWRITEERROR		"Could not write value to BBM"
#define BBMNUMOFBYTES		"Number of bytes for opcode wrong"
#define BBMISFULLEDER		"BBM is filled"
#define BBMINVALIDOPC		"Invalid opcode read from BBM"

#define REDFLAG				"*"

#include "JulianT.h"

//pack structures at the byte level
#pragma pack(1)

struct BinaryEventRec {
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	BYTE bStatus;
	unsigned short usNode;
	BYTE bState;
	BYTE bMask;
	BYTE bReserved;
	BYTE bChecksum;
};
#define BINARYEVENT_REC_LENGTH	(sizeof(BinaryEventRec)) //13

struct GPS_Rec{
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	unsigned short usNode;
	double dLatitude;
	double dLongitude;
	double dAltitude;
//	double dClockBias;//bob
	BYTE	bHour;
	BYTE	bMinute;
	BYTE	bSecond;
	BYTE	bDay;
	BYTE	bMonth;
	unsigned short usYear;
	BYTE	bUnused;//# satelites
	float  fFixTimeA;
	float  fEastVelocity;
	float  fNorthVelocity;
	float  fUpVelocity;
	float  fClockBiasRate;
	float  fFixTimeB;
	BYTE bChecksum;
};
#define GPSDATA_REC_LENGTH (sizeof(GPS_Rec))  //65

#ifdef RFIDDEMO

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

struct RAD_Rec{
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	unsigned short usNode;
	float fCntRate;
	unsigned int uiElapsed; //100 msec units
	BYTE bReserved1;
	BYTE bReserved2;
	BYTE bChecksum;
};
#define RADDATA_REC_LENGTH (sizeof(RAD_Rec))

struct RAD2_Rec{
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	unsigned short usNode;
	unsigned int uiCounts[8];
	unsigned int uiElapsed; //100 msec units
	BYTE bReserved1;
	BYTE bReserved2;
	BYTE bChecksum;
};
#define RAD2DATA_REC_LENGTH (sizeof(RAD2_Rec))

#endif

struct EndDump_Rec {
	char RecTypeA;
	char RecTypeB;
	BYTE bCount;
	BYTE bChecksum;
};
#define ENDDUMP_REC_LENGTH (sizeof(EndDump_Rec)) //4

//////////////////////////////////////////////////////////
//
// VACOSS structures as they come from the Event Instrument
//

struct VacossError_Rec{
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	BYTE bResponseType;
	unsigned short usNode;  //<<QA>>
	unsigned short usSealID;
	unsigned short usErrorCode;
	BYTE bReserved[4];
	BYTE bChecksum;
};
#define VACOSSERROR_REC_LENGTH  (sizeof(VacossError_Rec)) //18  (was 16)

struct VacossEvent_Rec{
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	BYTE bResponseType;
	unsigned short usNode;
	BYTE bSealData[36];
	BYTE bChecksum;
};
#define VACOSSEVENT_REC_LENGTH	(sizeof(VacossEvent_Rec)) //46

struct VacossInfo_Rec{
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	BYTE bResponseType;
	unsigned short usNode;
	BYTE bSealData[21];
	BYTE bChecksum;
};
#define VACOSSINFO_REC_LENGTH	(sizeof(VacossInfo_Rec)) //31

//
// End VACOSS structures
//
/////////////////////////////////////////////////////////////////////

#ifdef RFIDDEMO
struct GRANDAcquire_Rec
{
	char RecTypeA;
	char RecTypeB;
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
#define GRANDDATA_REC_LENGTH (sizeof(GRANDAcquire_Rec))

struct RABBITAcquire_Rec
{
	char RecTypeA;
	char RecTypeB;
	unsigned int JulianSeconds;	//4 bytes
	short	StatusByte;		//2 bytes
	unsigned int uiCounts[8];	//4 bytes
	short	CountTime;		//2 bytes
};
#define RABBITDATA_REC_LENGTH (sizeof(RABBITAcquire_Rec))

struct LGPS_Rec {
   char RecTypeA;
   char RecTypeB;
   unsigned int uiTime;  // Rabbit time seconds from 1952,1,1,0,0,0
   unsigned short usNode; //last octet of IP address
   short   iLatitudeDegrees;
   float fLatitudeMinutes;
   short   iLongitudeDegrees;
   float fLongitudeMinutes;
   float fAltitude;      // Meters
   unsigned char bHour;   // GPS time
   unsigned char bMinute;
   unsigned char bSecond;
   unsigned char bDay;
   unsigned char bMonth;
   unsigned short usYear; //2
   unsigned char bNumSatellites;
   float fFixTimeA;
   float fEastVelocity;
   float fNorthVelocity;
   float fUpVelocity;
   float fFixTimeB;
   unsigned char bChecksum;
};
#define LGPSDATA_REC_LENGTH (sizeof(LGPS_Rec))

#endif

struct EventAcquireRecord 
{
	unsigned int uiJulian;
	unsigned short usSize;
	//unsigned short usMessageType;
	union UNKNOWN 
	{
		BYTE	bBinaryData[BINARYEVENT_REC_LENGTH];
		BYTE	bVacossInfo[VACOSSINFO_REC_LENGTH];
		BYTE	bVacossData[VACOSSEVENT_REC_LENGTH];
		BYTE	bVacossErro[VACOSSERROR_REC_LENGTH];
//		BYTE	bUGrandData[MICROGRAND_REC_LENGTH];
		BYTE	bGPSRecData[GPSDATA_REC_LENGTH];
#ifdef RFIDDEMO
		BYTE	bRADData[RADDATA_REC_LENGTH];
		BYTE	bRFIDData[RFIDDATA_REC_LENGTH];
		BYTE	bGRADData[GRANDDATA_REC_LENGTH];
#endif
	} bData;
};

struct EventAcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	//char	unused2[47];
	//or instead of unused2
	char	FullYear[4];
	char	unused2[43];
};
#pragma pack()

class CEventInstrument;
/////////////////////////////////////////////////////////////////////////////
// CEventPropertySheet
// property sheet for the EVENT instruments
class CEventPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CEventPropertySheet)

// Construction
public:
	CEventPropertySheet(
		UINT nIDCaption, 
		CEventInstrument* pParentWnd);

	CEventPropertySheet(
		LPCTSTR pszCaption, 
		CEventInstrument* pParentWnd);

	void FinishConstruction();

	virtual ~CEventPropertySheet();

	COLORREF SetButtonColor(COLORREF Color);
	void WriteToFile(int ToWhom, int Type, CJulianTime* Time = NULL, char* Message = NULL);
	bool WriteToEVTFile(int iIndex);
	void SetTabColor(int Tab, COLORREF Color);
	void SetLimits(int time, char * pId);
	void IssuePrint(FILE* outfile);

	CString *GetVersionString();

	virtual void SetToClose(int set);
	virtual void ForceDateTime(void);

	virtual void UpdateSummary(char *);
	void    RestartAutoCloseTimer(){m_dToClose = 0;}

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	CColorButton	*m_pCloseButton;
	CButton			*m_pPauseButton;
	CButton			*m_pPrintButton;
	CButton			*m_pAckFlagsButton;
	CButton			*m_pToClose;
	int				m_dToClose;
	CEventInstrument *m_pParent;
	CImageList		*m_pImageList;

	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentEVTFileName[_MAX_PATH];

	int				m_dBinaryAt;
	int				m_dAt;
	int				m_dAcquireRecordCount;
	struct			EventAcquireRecord AcquireRec[NUMBEROFACQUIRERECS];
	int				m_dPreviousEVTTime;

protected:
	int General_Tab;
	int Data_Tab;
	int UserProg_Tab;

	void GetVacossEvents(unsigned char* pAt, unsigned int *uiTime1, unsigned int *uiTime2);
	int Echo_Rec(unsigned short usMessageStatus);
	int DumpOk_Rec(BYTE* pAt);
	int Inquire2_Rec(BYTE* pAt);
	int DumpStat_Rec(BYTE* pAt);
	int VacossSealInfo_Rec(BYTE* pAt);
	int VacossSealEvent_Rec(BYTE* pAt);
	int VacossSealError_Rec(BYTE* pAt);
	int ILONEndDump_Rec(BYTE* pAt);
	int BinaryEvent_Rec(BYTE* pAt);
	int Unknown_Rec(BYTE* pAt);
	int GPSData_Rec(BYTE* pAt);
#ifdef RFIDDEMO
	int RadData_Rec(BYTE* pAt);
	int Rad2Data_Rec(BYTE* bAt);
	int RFIDData_Rec(BYTE* pAt);
	int LRadData_Rec(BYTE* pAt);
	int LRFIDData_Rec(BYTE* pAt);
	int LGPSData_Rec(BYTE* pAt);
#endif
	void Reverse(BYTE*,int);
	int AppendCheckSum(char*);

	int m_dBinaryChecksum;
	int m_dBinaryState;
	unsigned short m_usLength;
	unsigned short m_usMessageType;
	unsigned short m_usNumberOfRecordsInDUMPBBM;
	int m_dRecLengthCount;
	int m_dGivenChecksum;
	int m_dChecksum;

	int  Checksum(BYTE * pAt, int bLength, bool bASCIICheckSum = false);
	bool LengthOk(char* Record, int length);
	void MakeMyPath(const char *);
	void SetChecksumDisplay();
	void BuildFileName(CString &cResult,CString cFormat,CString cPath,
		CString cType,CString cID,int iYear,int iMonth,int iDay,
		int iHour,int iMinute,int iSecond,
		CString cRFID,CString cSuffix);

	long  m_dTime_Err;			//Difference between MIC and Inst 

	float m_fDIRHigh;
	float m_fDIRLow;
	int   m_dDIRFirst;

	char  m_szCurrentID[3];
	int	  m_dCurrentYear;
	int   m_dCurrentMonth;
	int   m_dCurrentDay;
//	int	  m_dCurrentHour;
//	int   m_dCurrentMinute;

	bool  m_bStartOfFile;

	bool  m_bBatteryOk;
	bool  m_bBatteryTolOk;
//	bool  m_bP5VoltOk;
//	bool  m_bP15VoltOk;
//	bool  m_bM15VoltOk;
//	bool  m_bACPowerOk;
//	bool  m_bACPowerLoss;
	bool  m_bTimeError;
	bool  m_bBBMOk;
	bool  m_bCountRateTamperOk;
	bool  m_bRMSSigmaTamperOk;
	int   m_dCWStart;
	bool  m_bNewFileName;
	bool  m_bInMII;
	bool  m_bDataFiltered;
	char  m_szPreviousGTime[16];
	char  m_pAccumulator[ACCUMULATOR_SIZE];
	BYTE  m_pBinaryAccumulator[BINARYACCUMULATOR_SIZE];
	char  m_pCaption[256];
	CString m_cCommStatus;
	CString m_cCheckSumMsg;
	bool  m_bCouldNotOpenBinaryFile;

	//{{AFX_MSG(CEventPropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCloseButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPauseButton();
	afx_msg void OnAckButton();
	afx_msg void OnPrintButton();
	afx_msg void OnToCloseButton();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg void OnReceive(WPARAM WParam, LPARAM LParam);
	afx_msg void OnReceiveOOB(WPARAM WParam, LPARAM LParam);
	afx_msg void OnExtendClose(WPARAM WParam, LPARAM LParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EVENTSAFEPS_H
