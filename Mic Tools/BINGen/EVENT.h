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
#ifndef EVENT_H
#define EVENT_H

typedef unsigned char BYTE;

#pragma pack(1)

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

struct VacossEvent_Rec{
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	BYTE bResponseType;
	unsigned short usNodeNumber;
	BYTE bSealData[36];
	BYTE bChecksum;
};

struct VacossInfo_Rec{
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	BYTE bResponseType;
	unsigned short usNodeNumber;
	BYTE bSealData[21];
	BYTE bChecksum;
};

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

struct MicroGrand_Rec {
	char RecTypeA;
	char RecTypeB;
	unsigned int uiTime;
	BYTE bStatus;
	unsigned short usNode;
	unsigned short usCounts;
	unsigned short usCycles;
	unsigned short usTemperature;
	unsigned short usVoltage;
};

#define VACOSSINFO_REC			0x3335
#define VACOSSEVENT_REC			0x3336
#define VACOSSERROR_REC			0x3339
#define BINARYEVENT_REC			0x3332
#define GPSDATA_REC				0x3341
#define MICROGRAND_REC			0x3430

#define GPSDATA_REC_LENGTH      (sizeof(GPS_Rec))  //65
#define MICROGRAND_REC_LENGTH   (sizeof(MicroGrand_Rec))	//18
#define BINARYEVENT_REC_LENGTH	(sizeof(BinaryEventRec)) //13
#define VACOSSERROR_REC_LENGTH  (sizeof(VacossError_Rec)) //18
#define VACOSSEVENT_REC_LENGTH	(sizeof(VacossEvent_Rec)) //46
#define VACOSSINFO_REC_LENGTH	(sizeof(VacossInfo_Rec))  //31

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

struct EventAcquireRecord 
{
	//unsigned int uiJulian;
	unsigned short usSize;
	union UNKNOWN 
	{
		BYTE	bBinaryData[BINARYEVENT_REC_LENGTH];
		BYTE	bVacossInfo[VACOSSINFO_REC_LENGTH];
		BYTE	bVacossData[VACOSSEVENT_REC_LENGTH];
		BYTE	bVacossErro[VACOSSERROR_REC_LENGTH];
		BYTE	bUGrandData[MICROGRAND_REC_LENGTH];
		BYTE	bGPSRecData[GPSDATA_REC_LENGTH];
	} bData;
};

#pragma pack()

#endif // EVENT_H