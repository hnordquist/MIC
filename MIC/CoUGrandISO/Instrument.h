// Instrument.h

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "COM_Utilities.h"
#include "Genl4Stat.h"
#include "DataIStat.h"
#include "Data2Stat.h"
#include "DataStat.h"
#include "UGInfo.h"
#include "BMBInfo.h"
#include "PSBinfo.h"
#include "ESBInfo.h"
#include "UGInfo.h"
#include "User4Prog.h"
#include "CDData.h"
#include "ModParam.h"
#include "UGrandLegend.h"
#include "Summary.h"
#include "SetupData.h"
#include "UGrandSetupData.h"
#include "ISO.h"   //base class
#include "../Interfaces/ICSO.h"  //the communications component interface

#define UGRAND_DISCOVERY_TYPE "uGRAND"

/////////////////////////////////////////////////////////////////////////////
// CInstrument window

#define SOH_MGD		0x09

#pragma pack(1)

#define RADACQUIRERECORD_OPCODE		0x58
struct RADAcquireRecord
{
	BYTE	bLength;				//1 byte	//0x4C
	BYTE	bOpcode;				//1 byte	//0x58
	unsigned int uiJulianSeconds;	//4 bytes
	unsigned char ucJulianSec10ths;	//1 byte
	BYTE	bStatusByte0;			//1 byte
	BYTE	bStatusByte1;			//1 byte
	BYTE	bStatusByte2;			//1 byte
	BYTE	bStatusByte3;			//1 byte
	BYTE	bPPInputState;			//1 byte
	BYTE	bCh0Status;				//1 byte
	float	fCh0CountRate;			//4 bytes
	float	fCh0ShortCountRate;		//4 bytes
	float	fCh0LongCountRate;		//4 bytes
	BYTE	bCh1Status;				//1 byte
	float	fCh1CountRate;			//4 bytes
	float	fCh1ShortCountRate;		//4 bytes
	float	fCh1LongCountRate;		//4 bytes
	BYTE	bCh2Status;				//1 byte
	float	fCh2CountRate;			//4 bytes
	float	fCh2ShortCountRate;		//4 bytes
	float	fCh2LongCountRate;		//4 bytes
	BYTE	bCh3Status;				//1 byte
	float	fCh3CountRate;			//4 bytes
	float	fCh3ShortCountRate;		//4 bytes
	float	fCh3LongCountRate;		//4 bytes
	float	fCPUTemp;				//4 bytes
	float	fBOARDTemp;				//4 byres
	unsigned int uiCountTime10ths;	//4 bytes
};	
								// total: 76
struct RADAcquireTextRecord
{
	unsigned char chOpcode[2];				
	unsigned char cdJulianSeconds[10];		
	unsigned char cdJulianSec10ths;			
	unsigned char chStatusByte0[2];			
	unsigned char chStatusByte1[2];			
	unsigned char chStatusByte2[2];			
	unsigned char chStatusByte3[2];			
	unsigned char chPPInputState;			
	unsigned char chCh0Status[2];			
	unsigned char cfCh0CountRate[8];		
	unsigned char cfCh0ShortCountRate[8];	
	unsigned char cfCh0LongCountRate[8];	
	unsigned char chCh1Status[2];			
	unsigned char cfCh1CountRate[8];		
	unsigned char cfCh1ShortCountRate[8];	
	unsigned char cfCh1LongCountRate[8];	
	unsigned char chCh2Status[2];			
	unsigned char cfCh2CountRate[8];		
	unsigned char cfCh2ShortCountRate[8];	
	unsigned char cfCh2LongCountRate[8];	
	unsigned char chCh3Status[2];			
	unsigned char cfCh3CountRate[8];		
	unsigned char cfCh3ShortCountRate[8];	
	unsigned char cfCh3LongCountRate[8];	
	unsigned char cdCPUTemp[6];				
	unsigned char cdBOARDTemp[6];			
	unsigned char cdCountTime10ths[8];		
	unsigned char chCheckSum[2];			
	unsigned char cCR;
	unsigned char cLF;
};									 

#define ESBACQUIRERECORD_OPCODE			0x59
struct ESBAcquireRecord
{
	BYTE	bLength;					//0x2A
	BYTE	bOpcode;					//0x59
	unsigned int uiJulianSeconds;	
	BYTE	bJulianSec10ths;	
	BYTE	bStatusByte0;			
	BYTE	bStatusByte1;			
	BYTE	bStatusByte2;			
	BYTE	bStatusByte3;			
	unsigned short usAccelerometer;	
	BYTE	 ubHumidity;			
	unsigned short usLightInt;		
	unsigned short usMagnetometerX;	
	unsigned short usMagnetometerY;	
	unsigned short usMagnetometerZ;	
	unsigned short usSurfWetness;	
	float	fCPUTemp;				
	float	fBOARDTemp;				
	unsigned int uiLastRecIntervTime10ths;	
	BYTE     bESBSerialNumber[6];	
};									// total: 42

struct ESBAcquireTextRecord
{
	unsigned char chOpcode[2];				
	unsigned char cdJulianSeconds[10];	
	unsigned char cdJulianSec10ths;	
	unsigned char chStatusByte0[2];			
	unsigned char chStatusByte1[2];			
	unsigned char chStatusByte2[2];			
	unsigned char chStatusByte3[2];			
	unsigned char chAccelerometer[3];	
	unsigned char cdHumidity[3];			
	unsigned char chLightInt[3];		
	unsigned char chMagnetometerX[3];	
	unsigned char chMagnetometerY[3];	
	unsigned char chMagnetometerZ[3];	
	unsigned char chSurfWetness[3];	
	unsigned char cfCPUTemp[6];				
	unsigned char cfBOARDTemp[6];				
	unsigned char cfLastRecIntervTime10ths[8];
	unsigned char chESBSerialNumber[12];	
	unsigned char chCheckSum[2];			
	unsigned char cCR;
	unsigned char cLF;
};

#define UGCONFIGRECORD_OPCODE	0x50
struct uGConfigRecord
{
	BYTE	bLength;				
	BYTE	bOpcode;					//0x50
	unsigned int uiJulianSeconds;	
	unsigned int uiAcqInterval10ths;
	BYTE	bBaudFrame;
	float	fPHDVSetPoint;
	BYTE	bCPUSpeed;
	unsigned int uiModeFlags;
	BYTE	bFilterControl;
	BYTE	bImmBuffSize;
	BYTE	bImmBuffSaveSize;
	BYTE	bLocalBGSize;
	BYTE	bLocalBGEnd;
	unsigned short	usStatusRecInterv;
	BYTE	bTimeSyncState;
	BYTE	bHourlyTimeSyncMM;	//may need to swap these two
	BYTE	bHourlyTimeSyncSS;	//depending on how bob does it
	BYTE	bDailyTimeSyncHH;	//also may need to swap HH
	BYTE	bDailyTimeSyncMM;	//with
	BYTE	bDailyTimeSyncSS;	//SS
	BYTE	bTimeSyncLowerTol;
	BYTE	bTimeSyncUpperTol;
	unsigned int uiInspectorID;
	BYTE	bUnitID;
	BYTE	bImpedanceSet;
	float	fCh0KalmanR;
	float	fCh1KalmanR;
	float	fCh2KalmanR;
	float	fCh3KalmanR;
	float	fCh0KalmanQ;
	float	fCh1KalmanQ;
	float	fCh2KalmanQ;
	float	fCh3KalmanQ;
};

struct uGConfigTextRecord
{
	unsigned char chOpcode[2];				//50
	unsigned char cdJulianSeconds[10];	
	unsigned char cdAcqInterval10ths[10];
	unsigned char cdBaudFrame[2];
	unsigned char cdPHDVSetPoint[4];
	unsigned char chCPUSpeed[2];
	unsigned char chModeFlags[8];
	unsigned char chFilterControl[2];
	unsigned char cdImmBuffSize[2];
	unsigned char cdImmBuffSaveSize[2];
	unsigned char cdLocalBGSize[2];
	unsigned char cdLocalBGEnd[2];
	unsigned char cdStatusRecInterv[5];
	unsigned char chTimeSyncState;
	unsigned char cdHourlyTimeSync[4];
	unsigned char cdDailyTimeSync[6];
	unsigned char cdTimeSyncLowerTol[2];
	unsigned char cdTimeSyncUpperTol[2];
	unsigned char cdInspectorID[8];
	unsigned char chUnitID[2];
	unsigned char chImpedanceSet;
	unsigned char cfCh0KalmanR[10];
	unsigned char cfCh1KalmanR[10];
	unsigned char cfCh2KalmanR[10];
	unsigned char cfCh3KalmanR[10];
	unsigned char cfCh0KalmanQ[10];
	unsigned char cfCh1KalmanQ[10];
	unsigned char cfCh2KalmanQ[10];
	unsigned char cfCh3KalmanQ[10];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define UGCHCONFIGRECORD_OPCODE		0x51
struct uGChConfigRecord
{
	BYTE	bLength;				
	BYTE	bOpcode;				//0x51	
	unsigned int uiJulianSeconds;	
	BYTE	bUsedForAnalysis;
	BYTE	bThresh1TypeDir;
	BYTE	bThresh2TypeDir;
	float	fCh0Thresh1Value;
	float	fCh1Thresh1Value;
	float	fCh2Thresh1Value;
	float	fCh3Thresh1Value;
	float	fCh0Thresh2Value;
	float	fCh1Thresh2Value;
	float	fCh2Thresh2Value;
	float	fCh3Thresh2Value;
	BYTE	bCh0Thresh1EntryExit;
	BYTE	bCh1Thresh1EntryExit;
	BYTE	bCh2Thresh1EntryExit;
	BYTE	bCh3Thresh1EntryExit;	
	BYTE	bCh0Thresh2EntryExit;
	BYTE	bCh1Thresh2EntryExit;
	BYTE	bCh2Thresh2EntryExit;
	BYTE	bCh3Thresh2EntryExit;
	float	fCh0ChangSigMultip;
	float	fCh1ChangSigMultip;
	float	fCh2ChangSigMultip;
	float	fCh3ChangSigMultip;
	BYTE	bCh0ChangSigEntryExit;
	BYTE	bCh1ChangSigEntryExit;
	BYTE	bCh2ChangSigEntryExit;
	BYTE	bCh3ChangSigEntryExit;
	float	fCh0FilterLimit;
	float	fCh1FilterLimit;
	float	fCh2FilterLimit;
	float	fCh3FilterLimit;
	BYTE	bCh0Hysteresis;
	BYTE	bCh1Hysteresis;
	BYTE	bCh2Hysteresis;
	BYTE	bCh3Hysteresis;
};

struct uGChConfigTextRecord
{
	unsigned char chOpcode[2];				//51	
	unsigned char cdJulianSeconds[10];
	unsigned char chUsedForAnalysis;
	unsigned char chThresh1TypeDir[2];
	unsigned char chThresh2TypeDir[2];
	unsigned char cdfCh0Thresh1Value[9];
	unsigned char cdfCh1Thresh1Value[9];
	unsigned char cdfCh2Thresh1Value[9];
	unsigned char cdfCh3Thresh1Value[9];
	unsigned char cdfCh0Thresh2Value[9];
	unsigned char cdfCh1Thresh2Value[9];
	unsigned char cdfCh2Thresh2Value[9];
	unsigned char cdfCh3Thresh2Value[9];
	unsigned char chCh0Thresh1EntryExit[2];
	unsigned char chCh1Thresh1EntryExit[2];
	unsigned char chCh2Thresh1EntryExit[2];
	unsigned char chCh3Thresh1EntryExit[2];	
	unsigned char chCh0Thresh2EntryExit[2];
	unsigned char chCh1Thresh2EntryExit[2];
	unsigned char chCh2Thresh2EntryExit[2];
	unsigned char chCh3Thresh2EntryExit[2];
	unsigned char cdfCh0ChangSigMultip[4];
	unsigned char cdfCh1ChangSigMultip[4];
	unsigned char cdfCh2ChangSigMultip[4];
	unsigned char cdfCh3ChangSigMultip[4];
	unsigned char chCh0ChangSigEntryExit[2];
	unsigned char chCh1ChangSigEntryExit[2];
	unsigned char chCh2ChangSigEntryExit[2];
	unsigned char chCh3ChangSigEntryExit[2];
	unsigned char cdfCh0FilterLimit[7];
	unsigned char cdfCh1FilterLimit[7];
	unsigned char cdfCh2FilterLimit[7];
	unsigned char cdfCh3FilterLimit[7];
	unsigned char cdCh0Hysteresis[3];
	unsigned char cdCh1Hysteresis[3];
	unsigned char cdCh2Hysteresis[3];
	unsigned char cdCh3Hysteresis[3];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define ESBCONFIGRECORD_OPCODE		0x52
struct ESBConfigRecord
{
	BYTE	bLength;				
	BYTE	bOpcode;				//52	
	unsigned int uiJulianSeconds;
	unsigned short usNominalAcqInterval;
	unsigned short usAnomalousAcqInterval;
	unsigned short usLowPowerAcqInterval;
	unsigned short chModeFlags;
	BYTE	bSerialNumber[6];
};

struct ESBConfigTextRecord
{
	unsigned char chOpcode[2];				//52	
	unsigned char cdJulianSeconds[10];
	unsigned char cdNominalAcqInterval[5];
	unsigned char cdAnomalousAcqInterval[5];
	unsigned char cdLowPowerAcqInterval[5];
	unsigned char chModeFlags[4];
	unsigned char chSerialNumber[12];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define PSBCONFIGRECORD_OPCODE		0x53
struct PSBConfigRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x53	
	unsigned int uiJulianSeconds;
	unsigned short usModeFlags;
	float	fHVSetPoint;
	BYTE	bHVConfigState;
	BYTE	bSerialNumber[6];
};

struct PSBConfigTextRecord
{
	unsigned char chOpcode[2];				//53	
	unsigned char cdJulianSeconds[10];
	unsigned char chModeFlags[4];
	unsigned char cdfHVSetPoint[7];
	unsigned char chHVConfigState;
	unsigned char chSerialNumber[12];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define BMBCONFIGRECORD_OPCODE		0x54
struct BMBConfigRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;				//0x54	
	unsigned int uiJulianSeconds;
	unsigned short usModeFlags;
	unsigned short usPowerOnReportInter;
	unsigned short usPowerOffReportInterv;
	BYTE	bSerialNumber[6];
};

struct BMBConfigTextRecord
{
	unsigned char chOpcode[2];				//54	
	unsigned char cdJulianSeconds[10];
	unsigned char chModeFlags[4];
	unsigned char cdPowerOnReportInter[5];
	unsigned char cdPowerOffReportInterv[5];
	unsigned char chSerialNumber[12];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define UGSTATUSRECORD_OPCODE	0x55
struct uGStatusRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x55
	unsigned int uiJulianSeconds;	
	BYTE	bJulianSec10ths;
	float	f50VSupply;
	float	f33VSupply;
	float	f25VSupply;
	float	fPHDVoltage;
	BYTE	bStatus1Byte;
	BYTE	bStatus2Byte;
	BYTE	bStatus3Byte;
	BYTE	bStatus4Byte;
	unsigned int uiCPUUpTime;
	float	fCPUTemp;
	float	fBOARDTemp;
	unsigned int uiReserved;
};

struct uGStatusTextRecord
{
	unsigned char chOpcode[2];				
	unsigned char cdJulianSeconds[10];	
	unsigned char cdJulianSec10ths;
	unsigned char cdf50VSupply[4];
	unsigned char cdf33VSupply[4];
	unsigned char cdf25VSupply[4];
	unsigned char cdfPHDVoltage[4];
	unsigned char chStatus1Byte[2];
	unsigned char chStatus2Byte[2];
	unsigned char chStatus3Byte[2];
	unsigned char chStatus4Byte[2];
	unsigned char chCPUUpTime[10];
	unsigned char cdfCPUTemp[6];
	unsigned char cdfBOARDTemp[6];
	unsigned char chReserved[8];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define PSBSTATUSRECORD_OPCODE	0x56
struct PSBStatusRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x56
	unsigned int uiJulianSeconds;	
	BYTE	bJulianSec10ths;
	float	fExternPWRSupply;
	BYTE	bStatusByte;
	float	f50VOutput;
	float	f33VOutput;
	float	fHVOutputV;
	float	fHVOutputC;
	float	fCPUTemp;
	float	fBOARDTemp;
	BYTE	bSerialNumber[6];
	unsigned int uiReserved;
};

struct PSBStatusTextRecord
{
	unsigned char chOpcode[2];					//56
	unsigned char cdJulianSeconds[10];	
	unsigned char cdJulianSec10ths;
	unsigned char cdfExternPWRSupply[4];
	unsigned char chStatusByte[2];
	unsigned char cdf50VOutput[4];
	unsigned char cdf33VOutput[4];
	unsigned char cdfHVOutputV[7];
	unsigned char cdfHVOutputC[7];
	unsigned char cdfCPUTemp[6];
	unsigned char cdfBOARDTemp[6];
	unsigned char chSerialNumber[12];
	unsigned char chReserved[8];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define BMBSTATUSRECORD_OPCODE	0x57
struct BMBStatusRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x57
	unsigned int uiJulianSeconds;	
	BYTE	bJulianSec10ths;
	float	fChargeCurrent;
	float	fDischargeCurr;
	unsigned short usChDisCycles;
	float	fCPUTemp;
	float	fBOARDTemp;
	unsigned int uiTimeOnBatt;
	float	fBattTemp;
	BYTE	bBattStatus;
	BYTE	bSerialNumber[6];
	unsigned int uiReserved;
};

struct BMBStatusTextRecord
{
	unsigned char chOpcode[2];					//57
	unsigned char cdJulianSeconds[10];	
	unsigned char cdJulianSec10ths;
	unsigned char cdfChargeCurrent[7];
	unsigned char cdfDischargeCurr[7];
	unsigned char cdChDisCycles[4];
	unsigned char cdfCPUTemp[6];
	unsigned char cdfBOARDTemp[6];
	unsigned char cdTimeOnBatt[10];
	unsigned char cdfBattTemp[6];
	unsigned char chBattStatus[2];
	unsigned char chSerialNumber[12];
	unsigned char chReserved[8];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define INFORECORD_OPCODE 0x30
struct InfoRecord
{
	BYTE	bLength;
	BYTE	bOpcode;
	unsigned int uiJulianSeconds;
	BYTE	bFlagByte1;
	BYTE	bFlagByte2;
	BYTE	bFlagByte3;
	BYTE	bFlagByte4;
	BYTE	bInfoBytes[5];
};

struct InfoTextRecord
{
	unsigned char chOpcode[2];
	unsigned int  cdJulianSeconds[10];
	unsigned char chFlagByte1[2];
	unsigned char chFlagByte2[2];
	unsigned char chFlagByte3[2];
	unsigned char chFlagByte4[2];
	unsigned char chInfoBytes[10];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define UGINFORECORD_OPCODE	0x5a
struct uGInfoRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x5a
	unsigned int uiJulianSeconds;	
	unsigned short usActelVersion;
	BYTE		 b3dFirmwareVersion[3];
	unsigned short usFirmwareChkSum;
	BYTE		 bSerialNumber[6];
};

struct uGInfoTextRecord
{
	unsigned char chOpcode[2];					//5a
	unsigned char cdJulianSeconds[10];	
	unsigned char chActelVersion[4];
	unsigned char c3dFirmwareVersion[8];
	unsigned char cdFirmwareChkSum[4];
	unsigned char chSerialNumber[12];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define ESBINFORECORD_OPCODE	0x5b
struct ESBInfoRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x5b
	unsigned int uiJulianSeconds;	
	BYTE		 b3dFirmwareVersion[3];
	unsigned short usFirmwareChkSum;
	BYTE		 bSerialNumber[6];
};

struct ESBInfoTextRecord
{
	unsigned char chOpcode[2];					//5b
	unsigned char cdJulianSeconds[10];	
	unsigned char c3dFirmwareVersion[8];
	unsigned char cdFirmwareChkSum[4];
	unsigned char chSerialNumber[12];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define PSBINFORECORD_OPCODE	0x5c
struct PSBInfoRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x5c
	unsigned int uiJulianSeconds;	
	BYTE		 b3dFirmwareVersion[3];
	unsigned short usFirmwareChkSum;
	BYTE		 bSerialNumber[6];
};

struct PSBInfoTextRecord
{
	unsigned char chOpcode[2];					//5c
	unsigned char cdJulianSeconds[10];	
	unsigned char c3dFirmwareVersion[8];
	unsigned char cdFirmwareChkSum[4];
	unsigned char chSerialNumber[12];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define BMBINFORECORD_OPCODE 0x5d
struct BMBInfoRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x5d
	unsigned int uiJulianSeconds;	
	BYTE		 b3dFirmwareVersion[3];
	unsigned short usFirmwareChkSum;
	BYTE		 bSerialNumber[6];
};

struct BMBInfoTextRecord
{
	unsigned char chOpcode[2];					//5d
	unsigned char cdJulianSeconds[10];	
	unsigned char c3dFirmwareVersion[8];
	unsigned char cdFirmwareChkSum[4];
	unsigned char chSerialNumber[12];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define INFORMATIONRECORD_OPCODE	0x30
struct InformationRecord					//0x30
{
	BYTE bLength;
	BYTE bOpcode;
	unsigned int uiJulianSeconds;
	BYTE bFlagByte1;
	BYTE bFlagByte2;
	BYTE bFlagByte3;
	BYTE bFlagByte4;
	BYTE bInfoBytes[5];
};

struct InformationTextRecord 
{
	unsigned char chOpcode[2];			//30
	unsigned char cdJulianSeconds[10];
	unsigned char chFlagByte1[2];
	unsigned char chFlagByte2[2];
	unsigned char chFlagByte3[2];
	unsigned char chFlagByte4[2];
	unsigned char cbInfoBytes[10];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

#define TRIGGERCONFIGURERECORD_OPCODE	0x45
struct TriggerConfigureRecord
{
	BYTE	bLength;
	BYTE	bOpcode;				//0x45
	unsigned int uiJulianSeconds;
	BYTE	bTriggerPin4Logic;		
	BYTE	bTriggerPin4ChanType[8];
	BYTE	bTriggerPin5Logic;		
	BYTE	bTriggerPin5ChanType[8];
	BYTE	bTriggerPin6Logic;		
	BYTE	bTriggerPin6ChanType[8];
	BYTE	bTriggerPin7Logic;		
	BYTE	bTriggerPin7ChanType[8];
};

struct TriggerConfigureTextRecord		//45
{
	unsigned char chOpcode[2];
	unsigned char cdJulianSeconds[10];
	unsigned char chTriggerPin4Logic;
	unsigned char chTriggerPin4ChanType[16];
	unsigned char chTriggerPin5Logic;
	unsigned char chTriggerPin5ChanType[16];
	unsigned char chTriggerPin6Logic;
	unsigned char chTriggerPin6ChanType[16];
	unsigned char chTriggerPin7Logic;
	unsigned char chTriggerPin7ChanType[16];
	unsigned char chCheckSum[2];
	unsigned char cCR;
	unsigned char cLF;
};

union AcquireRecordX{
	struct {
		BYTE bLength;
		BYTE bOpcode;
		union {
			unsigned int uiJulianSeconds;
			BYTE bBytes[254];
		};
	};
	RADAcquireRecord RADAcquireRec;
	ESBAcquireRecord ESBAcquireRec;
	uGConfigRecord uGConfigRec;
	uGChConfigRecord uGChConfigRec;
	ESBConfigRecord ESBConfigRec;
	PSBConfigRecord PSBConfigRec;
	BMBConfigRecord BMBConfigRec;
	uGStatusRecord uGStatusRec;
	PSBStatusRecord PSBStatusRec;
	BMBStatusRecord BMBStatusRec;
	uGInfoRecord uGInfoRec;
	ESBInfoRecord ESBInfoRec;
	PSBInfoRecord PSBInfoRec;
	BMBInfoRecord BMBInfoRec;
	InformationRecord InformationRec;
	TriggerConfigureRecord TriggerConfigRec;
};

//pack structure at the byte level
struct AcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	unused2[47];
	//or instead of unused2
	//char FullYear[4];
	//char unused2[43];
};
#pragma pack()

class CInstrument : public CISO
{
// Construction
public:
	CInstrument(CWnd* Parent, char *inifile);
	CInstrument();
	virtual ~CInstrument();
	void FinishConstruction();

// Attributes
public:
	bool CollectParametersForNewISO(CString& csNameOut);
	bool CreateInstrument();
	void ShowInstrument(bool);
	void PauseInstrument(bool);
	bool UpdateParams(bool);
	void UpdateStrings();
	void DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	void FlagComm(int val, bool redraw);
	void FlagOthr(int val, bool redraw);
	void FlagTime(int val, bool redraw);
	void IssuePrint(FILE* outfile);
	void Reset();
	void SetLocalRegisteredMessageValues();
	void ReConfig();

#ifdef ALLOWTRIGGERHOLDOFF
	void AddExclusion(LPARAM lParam);
	void DelExclusion();
#endif

	CSummary    *m_pSummaryPage;
///	CGenl4Stat	*m_pGeneral4StatPage;
	CDataStat	*m_pDataStatus;
	CDataIStat	*m_pDataIStatus;
	CData2Stat	*m_pData2Status;
	CUser4Prog	*m_pUser4ProgStatus;
	CCDData		*m_pCDData;
	CModParam	*m_pModParams;
	CSetupData	*m_pSetupData;
	CUGINFO		*m_pUGInfo;
	CBMBInfo	*m_pBMBInfo;
	CPSBInfo	*m_pPSBInfo;
	CESBInfo	*m_pESBInfo;
	CLegend		*m_pLegend;

	CSafePropertySheet	*m_pPropertySheet;

	typedef enum PAGETYPE_TAG
	{
		SUMMARYPAGE,
		GENERALPAGE,
///		GENERAL4PAGE,
		USERPAGE,
		USER4PAGE,
		DATAPAGE,
		DATAIPAGE,
		DATA2PAGE,
		UGINFOPAGE,
		BMBINFOPAGE,
		PSBINFOPAGE,
		ESBINFOPAGE
	} PAGETYPE;

	COleDateTime m_pToday;
	char		m_pFullNameSpaces[128];
	bool		m_bIsPaused; 
	bool		m_bLogMII;
	bool		m_bLogFilter;
	bool		m_bInDumpLast;
	int			m_dSave_State;
	int			m_dSlowReset;
	int			m_dNumberOfAcquires;
	
	int			m_dBreakSentCount;
	int			m_dTimeOutCount;
	int			m_dInvalidTimeCount;

	int			m_dCurrent_State;
	CWnd*		m_pParent;
	unsigned int m_dPrevBIDTime;
	int			m_dStatusInt;
	int			m_dCycleInt;
	int			m_dMaxBBM;
	bool		m_bDoBinaryBBM;
	int			m_dTick_Count;
	int			m_dTickToInquire;
	int			m_dInquireInt;
	int			m_dTickToStatus;
	int			m_dCommFail;
	int			m_dTimeOut;
	float		m_fbatvolth;
	float		m_fbatvoltl;
	float		m_fsup_p25h;
	float		m_fsup_p25l;
	float		m_fsup_p33h;
	float		m_fsup_p33l;
	float		m_fsup_p05h;
	float		m_fsup_p05l;
	int			m_dtime_err;
	int			m_dRetransmit;
	int			m_dLongBreak;
	bool		m_bdoingBBM;
	bool		m_bdumpOK;
	bool		m_bChecksumFail;
	int			m_dChecksumCount;
	bool		m_bInHardFail;
	int			m_dHardFailCount;
	int			m_iLastRecordWritten;
	bool		m_bInitInitTimeOut;
	bool		m_bDriveToZero;
	bool		m_bDoAnalyzeAfterDT0;
	bool		m_bbbmVal;
	bool		m_bAllowAutoTime;
	bool		m_bDoingAutoTimeSet;
	int			m_dPreviousDay;
	COleDateTime	m_pNow;
#ifdef ALLOWTRIGGERHOLDOFF
	void ReadExclusion();
	void WriteExclusion();
	typedef struct EXCLUSIONRECTAG{
		unsigned int uiStartTime;
		unsigned int uiEndTime;
	} EXCLUSIONREC;
	CList< EXCLUSIONREC, EXCLUSIONREC > m_ExclusionList;
#endif

	CString m_cStatusRec;
	CString m_cOffsetRec;
	CString m_cIDRec;
	CString m_cAcquireRec1[NUMBEROFACQUIRERECS1DISPLAYED];
	CString m_cAcquireRec2[NUMBEROFACQUIRERECS2DISPLAYED];
	CString m_cBatteryRec;
	CString m_cTripNSRec;
//	CString m_cDualIonRec;
	CString m_cGenStatRec;
	CString m_cMon_A_Rec;
	CString m_cTripN2Rec;
	CString m_cID2_Record;
	CString m_cID3_Record;
	CString m_cAnalyzeOkRec;
	CString m_cAnalyzeOk3Rec;
	CString m_cInformRec;
	CString m_cMon_B_Rec;
	CString m_cUGChConfigRec;

	CString m_cuGConfigRecord;

	CString m_cESBConfigRecord1;
	CString m_cESBConfigRecord2;
	CString m_cESBConfigRecord3;
	CString m_cESBConfigRecord4;

	CString m_cPSBConfigRecord1;
	CString m_cPSBConfigRecord2;
	CString m_cPSBConfigRecord3;
	CString m_cPSBConfigRecord4;

	CString m_cBMBConfigRecord1;
	CString m_cBMBConfigRecord2;
	CString m_cBMBConfigRecord3;
	CString m_cBMBConfigRecord4;

	CString m_cUGStatusRecord;
	CString m_cUGInfoRecord;

	CString m_cESBInfoRecord1;
	CString m_cPSBInfoRecord1;
	CString m_cBMBInfoRecord1;

	CString m_cESBInfoRecord2;
	CString m_cPSBInfoRecord2;
	CString m_cBMBInfoRecord2;

	CString m_cESBInfoRecord3;
	CString m_cPSBInfoRecord3;
	CString m_cBMBInfoRecord3;

	CString m_cESBInfoRecord4;
	CString m_cPSBInfoRecord4;
	CString m_cBMBInfoRecord4;

	CString m_cPSBStatusRecord1;
	CString m_cBMBStatusRecord1;

	CString m_cPSBStatusRecord2;
	CString m_cBMBStatusRecord2;

	CString m_cPSBStatusRecord3;
	CString m_cBMBStatusRecord3;

	CString m_cPSBStatusRecord4;
	CString m_cBMBStatusRecord4;

	CString m_cInstrumentStatusRecord;
	CString m_cInstrumentInfoRecord;
//	CString m_cDualCurrentModeConfigRecord;
	CString m_cTriggerConfigRecord;
	CString m_cChannel0ConfigRecord;
	CString m_cChannel1ConfigRecord;
	CString m_cChannel2ConfigRecord;
	CString m_cChannel3ConfigRecord;
	CString m_cChannel4ConfigRecord;
	CString m_cChannel5ConfigRecord;
	CString m_cChannel6ConfigRecord;
	CString m_cSummaryRecent[6];
	CString m_cGENERAL_CHECKSUM_text;
	CString m_cSetParamMatchText;

	COLORREF SummaryStaticColors[IDC_SUMMARY_LAST-IDC_SUMMARY_FIRST+1];
///	COLORREF General4StaticColors[IDC_GENERAL4_LAST-IDC_GENERAL4_FIRST+1];
	COLORREF User4StaticColors[IDC_USER4PROG_LAST-IDC_USER4PROG_FIRST+1];
	COLORREF DataStaticColors[IDC_DATA_LAST-IDC_DATA_FIRST+1];
	COLORREF DataIStaticColors[IDC_DATAI_LAST-IDC_DATAI_FIRST+1];
	COLORREF Data2StaticColors[IDC_DATA2_LAST-IDC_DATA2_FIRST+1];
	COLORREF UGInfoStaticColors[IDC_UGINFO_LAST-IDC_UGINFO_FIRST+1];
	COLORREF BMBInfoStaticColors[IDC_BMBINFO_LAST-IDC_BMBINFO_FIRST+1];
	COLORREF PSBInfoStaticColors[IDC_PSBINFO_LAST-IDC_PSBINFO_FIRST+1];
	COLORREF ESBInfoStaticColors[IDC_ESBINFO_LAST-IDC_ESBINFO_FIRST+1];
	COLORREF Setup4StaticColors[SET_TOP-SET_BASE+1];

	CString SummaryStrings[IDC_SUMMARY_LAST-IDC_SUMMARY_FIRST+1];
///	CString General4Strings[IDC_GENERAL4_LAST-IDC_GENERAL4_FIRST+1];
	CString User4Strings[IDC_USER4PROG_LAST-IDC_USER4PROG_FIRST+1];
	CString DataStrings[IDC_DATA_LAST-IDC_DATA_FIRST+1];
	CString DataIStrings[IDC_DATAI_LAST-IDC_DATAI_FIRST+1];
	CString Data2Strings[IDC_DATA2_LAST-IDC_DATA2_FIRST+1];
	CString UGInfoStrings[IDC_UGINFO_LAST-IDC_UGINFO_FIRST+1];
	CString BMBInfoStrings[IDC_BMBINFO_LAST-IDC_BMBINFO_FIRST+1];
	CString PSBInfoStrings[IDC_PSBINFO_LAST-IDC_PSBINFO_FIRST+1];
	CString ESBInfoStrings[IDC_ESBINFO_LAST-IDC_ESBINFO_FIRST+1];

	CString SetupStrings[SET_TOP-SET_BASE+1];
	CString SetupINIStrings[SET_TOP-SET_BASE+1];

	void SummaryPageIssuePrint(FILE *outfile);
///	void General4PageIssuePrint(FILE *outfile);
	void DataPageIssuePrint(FILE *outfile);
	void DataIPageIssuePrint(FILE *outfile);
	void Data2PageIssuePrint(FILE *outfile);
	void UGInfoPageIssuePrint(FILE *outfile);
	void BMBInfoPageIssuePrint(FILE *outfile);
	void ESBInfoPageIssuePrint(FILE *outfile);
	void PSBInfoPageIssuePrint(FILE *outfile);
	void User4PageIssuePrint(FILE *outfile);
	void SetupPageIssuePrint(FILE *outfile);

///	int	m_iGeneral4_Tab;
	int m_iData_Tab;
	int m_iDataI_Tab;
	int m_iData2_Tab;
	int m_iUGInfo_Tab;
	int m_iBMBInfo_Tab;
	int m_iPSBInfo_Tab;
	int m_iESBInfo_Tab;
	int m_iUser4Prog_Tab;
	int m_iInst4Setup_Tab;

	unsigned int m_uiuGConfigRecordTime;
	unsigned int m_uiuGChConfigRecordTime;		
	unsigned int m_uiESBConfigRecordTime;		
	unsigned int m_uiPSBConfigRecordTime;		
	unsigned int m_uiBMBConfigRecordTime;		
	double		 m_duGStatusRecordTime;
	double		 m_duPSBStatusRecordTime;		
	double		 m_duBMBStatusRecordTime;		
	unsigned int m_uiuGInfoRecordTime;			
	unsigned int m_uiESBInfoRecordTime;			
	unsigned int m_uiPSBInfoRecordTime;			
	unsigned int m_uiBMBInfoRecordTime;			
	unsigned int m_uiInformationRecordTime;		
	unsigned int m_uiTriggerConfigureRecordTime;	

	char GetFlags(PAGETYPE ePageType, int nId);
	void SetString(PAGETYPE ePageType, UINT uiID, CString cString);
	void SetString(PAGETYPE ePageType,UINT uiDlgItem,unsigned char* szText,int iLength);

	CString GetString(PAGETYPE ePageType, UINT uiID);
	void GetStrings(PAGETYPE ePageType, UINT uiDlgItem, CString& cString, char* cFlag);
	void UpdateDisplay();
	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentBIDFileName[_MAX_PATH];
	char			m_szCurrentSOHFileName[_MAX_PATH];

	int				m_dBinaryAt;
	int				m_dAt;
	int				m_dAcquireRecordCountX;
	int				m_dRecordCountX;
	AcquireRecordX	AcquireRecX[NUMBEROFACQUIRERECS];
	CJulianTime		AcquireTime[NUMBEROFACQUIRERECS];
	AcquireRecordX	RecordX[NUMBEROFSOHRECS];
	
	short			AcquireCS[NUMBEROFACQUIRERECS];
	int				m_dPreviousBIDTime;
//	int				m_dPreviousBIDTime1;
//	int				m_dPreviousBIDTime2;
//	enum MONVER		m_eMonitorVers;
	void AckButton();
	void SetLimits(
				float batvolth, float batvoltl,
				float sup_p25h, float sup_p25l,
				float sup_p33h, float sup_p33l,
				float sup_p05h, float sup_p05l,
				int   time,		char * pId);

	bool m_bFirst15Rec;
	bool m_bFirst15Rec2;

	bool CurrentSetParam(UINT uiIniName, char*szIniName,char*szValue,bool bDoMatch);
	bool CurrentSetParam(UINT uiIniName, CString cIniName,CString cValue, bool bDoMatch);

	void BuildFileName(CString &cResult,CString cFormat,CString cPath,
		CString cType,CString cID,int iYear,int iMonth,int iDay,
		int iHour,int iMinute,int iSecond,
		CString cSuffix);


	//*******************************************************************//
	//REMOVE WHEN uGRAND IS ACTUALLY AVAILABLE AND WE ARE USING ID3_Record
	void AcquirRec(char *);
	void ID2_Record(char *);
	void DumpOk(char *);
	//*******************************************************************//
	void DumpEnd2(char *);
	void ID3_Record(char *);
	void AnalyzeOk(char *);
	void SetColor(PAGETYPE ePageType,UINT uiDlgItem,COLORREF rgbColor);

	void DumpOk3_Record(char *);
	void AnalyzeOk3Record(char *);

	void StrFloatConvert(char*cString, int iFieldSize);
	void Reverse(BYTE*,int);
	int AppendCheckSum(char*);
	char* TrunkExp(char* szString, int iDigits);

	int OnBinaryRADAcquireRec(BYTE *pBinaryAt);
	int OnBinaryESBAcquireRec(BYTE *pBinaryAt);
	int OnBinaryuGConfigRecord(BYTE *pBinaryAt);
	int OnBinaryuGChConfigRecord(BYTE *pBinaryAt);
	int OnBinaryESBConfigRecord(BYTE *pBinaryAt);
	int OnBinaryPSBConfigRecord(BYTE *pBinaryAt);
	int OnBinaryBMBConfigRecord(BYTE *pBinaryAt);
	int OnBinaryuGStatusRecord(BYTE *pBinaryAt);
	int OnBinaryPSBStatusRecord(BYTE *pBinaryAt);
	int OnBinaryBMBStatusRecord(BYTE *pBinaryAt);
	int OnBinaryuGInfoRecord(BYTE *pBinaryAt);
	int OnBinaryESBInfoRecord(BYTE *pBinaryAt);
	int OnBinaryPSBInfoRecord(BYTE *pBinaryAt);
	int OnBinaryBMBInfoRecord(BYTE *pBinaryAt);
	int OnBinaryInfoRecord(BYTE *pBinaryAt);
	int OnBinaryTriggerConfigRecord(BYTE *pBinaryAt);

	void OnRADAcquireRecord(char *pBinaryAt);
	void OnESBAcquireRecord(char *pBinaryAt);
	void OnuGConfigRecord(char *pBinaryAt);
	void OnuGChConfigRecord(char *pBinaryAt);
	void OnESBConfigRecord(char *pBinaryAt);
	void OnPSBConfigRecord(char *pBinaryAt);
	void OnBMBConfigRecord(char *pBinaryAt);
	void OnuGStatusRecord(char *pBinaryAt);
	void OnPSBStatusRecord(char *pBinaryAt);
	void OnBMBStatusRecord(char *pBinaryAt);
	void OnuGInfoRecord(char *pBinaryAt);
	void OnESBInfoRecord(char *pBinaryAt);
	void OnPSBInfoRecord(char *pBinaryAt);
	void OnBMBInfoRecord(char *pBinaryAt);
	void OnInfoRecord(char *pBinaryAt);
	void OnTriggerConfigRecord(char *pBinaryAt);

	int m_dBinaryChecksum;
	int m_dBinaryState;
	int m_dBinaryAT;
	unsigned short m_usLength;
	unsigned short m_usMessageType;
	int m_dRecLengthCount;
	int m_dGivenChecksum;
	int m_dChecksum;

	void Echo_Rec(char *);
	void Echo_2_Rec(char *);
	void Echo_3_Rec(char *);
//	void Echo_4_Rec(char *);
	void Echo_5_Rec(char *);
//	void Echo_6_Rec(char *);
	int  Checksum(char *);
	bool LengthOk(char* Record, int length);
	void SetChecksumDisplay();

	float m_fBatVoltH;			//battery voltage High
	float m_fBatVoltL;			//battery voltage Low
	float m_fSup_P25H;			//+2.5 volt supply High
	float m_fSup_P25L;			//+2.5 volt supply Low
	float m_fSup_P33H;			//+3.3 volt supply High
	float m_fSup_P33L;			//+3.3 volt supply Low
	float m_fSup_P05H;			//  5 volt supply High
	float m_fSup_P05L;			//  5 volt supply Low
	long  m_dTime_Err;			//Difference between MIC and Inst 

	float m_fLowBV;
	float m_fCurBV;
	float m_fHighBV;

	float m_fFBLow;
	float m_fFBCur;
	float m_fFBHigh;
	bool  m_bFBFirst;

	float m_fLowP25;
	float m_fCurP25;
	float m_fHighP25;
	float m_fLowP33;
	float m_fCurP33;
	float m_fHighP33;
	float m_fLowP5;
	float m_fCurP5;
	float m_fHighP5;
	bool  m_bFirstBV;
	bool  m_bFirstP25;
	bool  m_bFirstP33;
	bool  m_bFirstP5;

	float m_fTNSRHigh;
	float m_fTNSRLow;
	int   m_dTNSRFirst;

	float m_fDIRHigh;
	float m_fDIRLow;
	int   m_dDIRFirst;

	char  m_szCurrentID[3];
	int	  m_dCurrentYear;
	int   m_dCurrentMonth;
	int   m_dCurrentDay;

	bool  m_bStartOfFile;

	bool  m_bBatteryOk;
	bool  m_bBatteryTolOk;
	bool  m_bP5VoltOk;
	bool  m_bP25VoltOk;
	bool  m_bP33VoltOk;
	bool  m_bACPowerOk;
	bool  m_bACPowerLoss;
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
	CString m_cCommStatus;
	CString m_cCheckSumMsg;
	bool  m_bCouldNotOpenBinaryFile;

	bool  m_bFCHVError;
	bool  m_bICHVError;
	int	  m_iFCHVError;
	int	  m_iICHVError;
	void  IssuePrint2(FILE* outfile);

	int m_iMessageLimiter[512];//look for the end one, see WriteToFileEx
	bool WriteToFileEx(unsigned uiIndex, int ToWhom, int Type, CJulianTime* Time, char* Msg, bool bStale = false);
	void WriteToFile(int ToWhom, int Type, CJulianTime* Time = NULL, char* Message = NULL, bool bStale = false);
	bool WriteToMGDFileX(AcquireRecordX* AcquireRec, short AcquireCS);
	bool WriteToSOHFileX(AcquireRecordX* AcquireRec);
	virtual void	UpdateSummary(char *);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrument)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CInstrument)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg void OnReceiveOOB(WPARAM WParam, LPARAM LParam);
	afx_msg void OnReceive(WPARAM WParam,LPARAM LParam);
	afx_msg void OnReceiveData(WPARAM WParam,LPARAM LParam);
	DECLARE_MESSAGE_MAP()
};

#endif // INSTRUMENT_H
