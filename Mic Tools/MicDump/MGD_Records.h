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
#pragma pack(1)

#define RADACQUIRE_OPCODE 0x58
struct RADAcquireRecord
{
	BYTE	bLength;				//1 byte
	BYTE	bOpcode;				//1 byte
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

#define ESBACQUIRE_OPCODE 0x59									
struct ESBAcquireRecord
{
	BYTE	bLength;				//1 byte
	BYTE	bOpcode;				//1 byte
	unsigned int uiJulianSeconds;	//4 bytes
	unsigned char ucJulianSec10ths;	//1 byte
	BYTE	bStatusByte0;			//1 byte
	BYTE	bStatusByte1;			//1 byte
	BYTE	bStatusByte2;			//1 byte
	BYTE	bStatusByte3;			//1 byte
	unsigned short usAccelerometer;	//2 bytes
	BYTE	 ubHumidity;			//1 byte
	unsigned short usLightInt;		//2 bytes
	unsigned short usMagnetometerX;	//2 bytes
	unsigned short usMagnetometerY;	//2 bytes
	unsigned short usMagnetometerZ;	//2 bytes
	unsigned short usSurfWetness;	//2 bytes
	float	fCPUTemp;				//4 bytes
	float	fBOARDTemp;				//4 byres
	unsigned int uiCountTime10ths;	//4 bytes
	BYTE     bESBSerialNumber[6];	//6 bytes
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
	float	f25VOutput;
	float	fHVOutputV;
	float	fHVOutputC;
	float	fCPUTemp;
	float	fBOARDTemp;
	BYTE	bSerialNumber[6];
	unsigned int uiReserved;
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

#define UGINFORECORD_OPCODE	0x5a
struct uGInfoRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x5a
	unsigned int uiJulianSeconds;	
	unsigned int uiActelVersion;
	BYTE		 b3dFirmwareVersion[3];
	unsigned short uiFirmwareChkSum;
	BYTE		 bSerialNumber[6];
};

#define ESBINFORECORD_OPCODE	0x5b
struct ESBInfoRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x5b
	unsigned int uiJulianSeconds;	
	BYTE		 b3dFirmwareVersion[3];
	unsigned short uiFirmwareChkSum;
	BYTE		 bSerialNumber[6];
};

#define PSBINFORECORD_OPCODE	0x5c
struct PSBInfoRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x5c
	unsigned int uiJulianSeconds;	
	BYTE		 b3dFirmwareVersion[3];
	unsigned short uiFirmwareChkSum;
	BYTE		 bSerialNumber[6];
};

#define BMBINFORECORD_OPCODE 0x5d
struct BMBInfoRecord
{
	BYTE	bLength;					
	BYTE	bOpcode;					//0x5d
	unsigned int uiJulianSeconds;	
	BYTE		 b3dFirmwareVersion[3];
	unsigned short uiFirmwareChkSum;
	BYTE		 bSerialNumber[6];
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

union BinaryRecordX{
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

#pragma pack()