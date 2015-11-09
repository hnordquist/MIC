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
// EventSafePS.cpp
//

#include "stdafx.h"
#include "ISO_definitions.h"
#include "INI_definitions.h"
#include "colors.h"
#include "ccbutton.h"
#include "JulianT.h"
#include "EventSafePS.h"
#include "EventInstrument.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define SINGLE_BINARY_FILE
#undef SINGLE_BINARY_FILE

#define PI2 6.283185307


//registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CEventPropertySheet

IMPLEMENT_DYNAMIC(CEventPropertySheet, CPropertySheet)

CEventPropertySheet::CEventPropertySheet(
	UINT nIDCaption, 
	CEventInstrument* pParentWnd)
	:CPropertySheet(nIDCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	m_pCaption[0] = NULL;
	FinishConstruction();
}

CEventPropertySheet::CEventPropertySheet(
	LPCTSTR pszCaption, 
	CEventInstrument* pParentWnd)
	:CPropertySheet(pszCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	strcpy(m_pCaption,pszCaption);
	FinishConstruction();
}

void CEventPropertySheet::FinishConstruction()
{
	uIDC_RECEIVE = m_pParent->m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_pParent->m_uIDC_RECEIVE_OOB;

	Data_Tab		= 0;

	m_fDIRHigh		= 0.0;
	m_fDIRLow		= 0.0;
	m_dDIRFirst		= true;

	m_bBatteryOk	= true;
	m_bBatteryTolOk	= true;
	m_bBBMOk		= true;
	m_bCountRateTamperOk = true;
	m_bRMSSigmaTamperOk = true;
	m_bInMII		= false;
	m_bNewFileName	= false;
	m_bDataFiltered	= false;
	m_bTimeError	= false;

	m_dToClose				= 0;

	m_bStartOfFile			= true;
	m_dCWStart				= -1; //neither COLD START (0) nor WARM START (1)

	m_szCurrentFileName[0]	= NULL;
	m_szPreviousGTime[0]	= NULL;
	m_dCurrentDay			= -1;
	m_dPreviousEVTTime		= 0xa5a5a5a5;

	m_dAt					= 0;
	m_dAcquireRecordCount	= 0;

	m_dBinaryChecksum		= 0;
	m_dBinaryState			= 0;
	m_dBinaryAt				= 0;
	m_usLength				= 0;
	m_usMessageType			= 0;
	m_dRecLengthCount		= 0;
	m_dGivenChecksum		= 0;
	m_dChecksum				= 0;
	m_bCouldNotOpenBinaryFile = false;

	m_pCloseButton    = NULL;
	m_pPauseButton    = NULL;
	m_pPrintButton    = NULL;
	m_pAckFlagsButton = NULL;
	m_pToClose        = NULL;
	m_pImageList      = NULL;
}

CEventPropertySheet::~CEventPropertySheet()
{
	delete m_pCloseButton;
	delete m_pPauseButton;
	delete m_pPrintButton;
	delete m_pAckFlagsButton;
	delete m_pToClose;
	delete m_pImageList;
}


COLORREF CEventPropertySheet::SetButtonColor(COLORREF color)
{
	if (color == COLOR_PAUSE)
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
	else
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);

	return m_pCloseButton->SetFaceColor(color);
}

void CEventPropertySheet::SetLimits(int time_err,char* Id)
{	
	m_dTime_Err=(long)time_err;
	strcpy(m_szCurrentID,Id);
}

BEGIN_MESSAGE_MAP(CEventPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CEventPropertySheet)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CLOSE_BUTTON,	OnCloseButton)
	ON_BN_CLICKED(ID_PAUSE_BUTTON,	OnPauseButton)
	ON_BN_CLICKED(ID_ACKFL_BUTTON,	OnAckButton)
	ON_BN_CLICKED(ID_PRINT_BUTTON,	OnPrintButton)
	ON_BN_CLICKED(ID_TOCLOSE,		OnToCloseButton)

	ON_MESSAGE(IDC_EXTENDCLOSE, OnExtendClose)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE,OnReceive)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE_OOB,OnReceiveOOB)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventPropertySheet message handlers
void CEventPropertySheet::UpdateSummary(char *Message)
{
	CString Data;

	CEventSummary*  summarypage	 = m_pParent->m_pSummaryPage;

	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND10,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND11,Data);
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND9,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND10,Data);
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND8,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND9,Data);
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND7,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND8,Data);
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND6,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND7,Data);
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND5,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND6,Data);
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND4,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND5,Data);
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND3,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND4,Data);
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND3,Data);
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_COMMAND1,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_EVENTSUMMARY_COMMAND1,Message);
}

void CEventPropertySheet::SetChecksumDisplay()
{
//<<QA>> this doesn't do much
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CString cString;
	cString = m_cCheckSumMsg;;
	cString += " ";
	cString += m_cCommStatus;
//	generalpage->SetDlgItemText(IDC_GENERAL_CHECKSUM,cString);
}

void CEventPropertySheet::OnReceiveOOB(WPARAM WParam, LPARAM LParam)
{
//	CEventSummary*  summarypage	 = m_pParent->m_pSummaryPage;
//	CEventDataStat* datapage	 = m_pParent->m_pDataStatus;
//	CBinaryDataStat* bindatapage = m_pParent->m_pBinaryDataStatus;
//	CUGrandDataStat* ugranddatapage = m_pParent->m_pUGrandDataStatus;
//	CGPSDataStat* gpsdatapage	 = m_pParent->m_pGPSDataStatus;
//	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;

	char* cbuff = (char*)LParam;
	char cbuff2[1024];
	char cbuff3[1024];
	cbuff2[1023] = NULL;
	if ((WParam > 4) && (cbuff[0] == 'O') && (cbuff[1] == 'O') && (cbuff[2] == 'B') && (cbuff[3] == ' '))
	{
		strcpy(cbuff2,"41590 ");
		try
		{
			//build the string for the pfm file
			int length2 = strlen(cbuff2) + (int)WParam;
			strncat(cbuff2,cbuff,(int)WParam);
			cbuff2[length2] = NULL;

			//build the string for the display
			strncpy(cbuff3,cbuff,(int)WParam);
			cbuff3[(int)WParam] = NULL;
			m_cCommStatus = cbuff3;
			SetChecksumDisplay();
		}
		catch(...)
		{
			WriteToFile(TO_PFM,TYPE_COMP,NULL,"41580 Bad LParam Address Received (OOB)");
			if (m_pParent->m_bDoDump)
				WriteToFile(TO_DMP,TYPE_DUMP, NULL,"41580 Bad LParam Address Received (OOB)");
			CoTaskMemFree((void *)LParam);
			return;
		}

		//strcat(cbuff2,"\n\r");
		char cBuffx[1024];
		strcpy(cBuffx,"41581 ");
		strcat(cBuffx,cbuff2);

		WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuffx);
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL, cBuffx);
		UpdateSummary(cbuff2);

		CoTaskMemFree((void *)LParam);
	}
	else
	{
		OnReceive(WParam, LParam);
	}
}

void CEventPropertySheet::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//can do this without an "accumulator" because all "event" messages are complete within an 
	//ilon packet consequently we will receive the entire message at one time
	BYTE* cbuff =(BYTE*)LParam;
	char c;
	if (WParam <= 1)
		return;

	try
	{
		c = cbuff[0];
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP, NULL,"41600 Bad LParam Address Received (Start of msg block)");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"41600 Bad LParam Address Received (Start of msg block)");
		CoTaskMemFree((void *)LParam);
		return;
	}
	try
	{
		c = cbuff[WParam-1];
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP, NULL,"41600 Bad LParam Address Received (End of msg block)");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"41600 Bad LParam Address Received (End of msg block)");
		CoTaskMemFree((void *)LParam);
		return;
	}

	unsigned usBinaryAt = 1;

	unsigned short usMessageStatus = cbuff[0];


//for(int ii = 0; ii < WParam; ii++)
//{
//	if (ii % 16 == 15) 
//		TRACE("%02x\n", cbuff[ii]);
//	else
//		TRACE("%02x ", cbuff[ii]);
//}
//TRACE("\n\n");


	//get each of the messages in this block
	while (usBinaryAt < (unsigned short)WParam)
	{
		unsigned short usMessageType;
		unsigned short usMessageTypeSub;

		try
		{
			usMessageType = cbuff[usBinaryAt];
			usMessageTypeSub = cbuff[usBinaryAt+1];
		}
		catch(...)
		{
			WriteToFile(TO_PFM,TYPE_COMP,NULL,"XXXXX Failed on read message type");
			if (m_pParent->m_bDoDump)
				WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on cbuff[usBinaryAt]");
			CoTaskMemFree((void *)LParam);
			return;
		}

		if (usMessageType != 0x00)
			usMessageType = (unsigned short)((usMessageType << 8) | usMessageTypeSub);

		//protect the rest of memory
		switch (usMessageType)
		{
		case ECHO_REC://simple ack 0
			try
			{
				usBinaryAt += Echo_Rec(usMessageStatus);				
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP,NULL,"XXXXX Failed processing Echo_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing Echo_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;
		case DUMPOK_REC://ILON DUMP OK 3137
			try
			{
				usBinaryAt += DumpOk_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing DumpOk_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing DumpOk_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;
		case INQUIRE2_REC://INQUIRE2 Record 3145
			try
			{
				usBinaryAt += Inquire2_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing Inquire2_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing Inquire2_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;
		case BINARYEVENT_REC://binary event record 3332
			try
			{
				usBinaryAt += BinaryEvent_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing BinaryEvent_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing BinaryEvent_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}

//debug to gen some records
/*
			#pragma pack(1)
			struct GPS_Rec{
				char RecTypeA;
				char RecTypeB;
				unsigned int uiTime;
				unsigned short usNodeNumber;
				double dLatitude;
				double dLongitude;
				double dAltitude;
				double dClockBias;
				float  fFixTimeA;
				float  fEastVelocity;
				float  fNorthVelocity;
				float  fUpVelocity;
				float  fClockBiasRate;
				float  fFixTimeB;
				BYTE bChecksum;
			} AGPS_Rec;
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
			} AMG_Rec;

			struct VacossError_Rec{
				char RecTypeA;
				char RecTypeB;
				unsigned int uiTime;
				BYTE bResponseType;
				unsigned short usSealID;
				unsigned short usErrorCode;
				BYTE bReserved[4];
				BYTE bChecksum;
			} AVE_Rec;

			struct VacossEvent_Rec{
				char RecTypeA;
				char RecTypeB;
				unsigned int uiTime;
				BYTE bResponseType;
				unsigned short usNodeNumber;
				BYTE bSealData[36];
				BYTE bChecksum;
			} AV_Rec;

			struct VacossInfo_Rec{
				char RecTypeA;
				char RecTypeB;
				unsigned int uiTime;
				BYTE bResponseType;
				unsigned short usNodeNumber;
				BYTE bSealData[21];
				BYTE bChecksum;
			} AVI_Rec;
			#pragma pack()

			memset(&AGPS_Rec,NULL,sizeof(AGPS_Rec));
			memset(&AMG_Rec,NULL,sizeof(AMG_Rec));
			memset(&AVE_Rec,NULL,sizeof(AVE_Rec));
			memset(&AV_Rec,NULL,sizeof(AV_Rec));
			memset(&AVI_Rec,NULL,sizeof(AVI_Rec));

			AGPS_Rec.RecTypeA = '3';
			AGPS_Rec.RecTypeB = 'A';
			AGPS_Rec.bChecksum = 0x74;
			GPSData_Rec((BYTE*)&AGPS_Rec);

			AMG_Rec.RecTypeA = '4';
			AMG_Rec.RecTypeB = '0';
			MicroGRAND_Rec((BYTE*)&AMG_Rec);
			
			AVE_Rec.RecTypeA = '3';
			AVE_Rec.RecTypeB = '9';
			AVE_Rec.bChecksum = 0x6c;
			VacossSealError_Rec((BYTE*)&AVE_Rec);

			AV_Rec.RecTypeA = '3';
			AV_Rec.RecTypeB = '6';
			AV_Rec.bChecksum = 0x69;
			VacossSealEvent_Rec((BYTE*)&AV_Rec);

			AVI_Rec.RecTypeA = '3';
			AVI_Rec.RecTypeB = '5';
			AVI_Rec.bChecksum = 0x68;
			VacossSealInfo_Rec((BYTE*)&AVI_Rec);
*/
			break;

		case STATUS_REC://ILON STATUS Record 3334
			try
			{
				usBinaryAt += DumpStat_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing DumpStat_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing DumpStat_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;
		case VACOSSINFO_REC://vacoss seal info 3335
			try
			{
				usBinaryAt += VacossSealInfo_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing VacossSealInfo_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing VacossSealInfo_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;
		case VACOSSEVENT_REC://vacoss seal events 3336
			try
			{
				usBinaryAt += VacossSealEvent_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing VacossSealEvent_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing VacossSealEvent_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;
		case VACOSSERROR_REC://vacoss seal error 3339
			try
			{
				usBinaryAt += VacossSealError_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing VacossSealError_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing VacossSealError_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;

		case GPSDATA_REC://GPS Data Record 3341
			try
			{
				usBinaryAt += GPSData_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing GPSData_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing GPSData_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;

		case ENDDUMP_REC://ILON_ENDDUMP 3632
			try
			{
TRACE("ENDDUMP Record\n");
				usBinaryAt += ILONEndDump_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing ILONEndDump_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing ILONEndDump_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;

#ifdef RFIDDEMO
		case RADDATA_REC: //pulse counting chan
			try
			{
TRACE("RAD Record\n");
				usBinaryAt += RadData_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing RadData_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing RadData_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;

		case RAD2DATA_REC: //pulse counting chan
			try
			{
TRACE("RAD2 Record\n");
				usBinaryAt += Rad2Data_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing RadData2_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing RadData2_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;

		case RFIDDATA_REC: //pulse counting chan
			try
			{
TRACE("RFID Record\n");
				usBinaryAt += RFIDData_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing RFIDData_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing RFIDData_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;

		case LGPSDATA_REC: //pulse counting chan
			try
			{
TRACE("GPS Record\n");
				usBinaryAt += LGPSData_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing LGPSData_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing LGPSData_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
			break;
#endif
		default://unknown record
			try
			{
TRACE("UNKNOWN Record\n");
				usBinaryAt += Unknown_Rec(&cbuff[usBinaryAt]);
			}
			catch(...)
			{
				WriteToFile(TO_PFM,TYPE_COMP, NULL,"XXXXX Failed on processing Unknown_Rec");
				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP,TYPE_DUMP, NULL,"XXXXX Failed on processing Unknown_Rec");
				CoTaskMemFree((void *)LParam);
				return;
			}
		}
		if (usBinaryAt >= (unsigned)WParam)
		{
			//throw all of this away??? fail on the dumpbbm???? or do what!!!
		}
	}

	CoTaskMemFree((void *)LParam);
}

int CEventPropertySheet::Echo_Rec(unsigned short usMessageStatus)
{
	char cBuff[32];
//	TRACE("CEventPropertySheet::Echo_Rec\n");
	if (usMessageStatus == 0x31)
	{
		m_pParent->PostMessage(uIDC_RECEIVE,ECHO_RECORD,CHECKSUM_GOOD);
		strcpy(cBuff,"41610 ACK (1)");
	}
	else
	{
		m_pParent->PostMessage(uIDC_RECEIVE,ECHO_RECORD,CHECKSUM_BAD);
		strcpy(cBuff,"41620 NAK (0)");
	}
	UpdateSummary(cBuff);
//	m_pParent->PostToWatch(cBuff);
//	strcat(cBuff,"\n\r");
//	WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);
	if (m_pParent->m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);//dump file
	return ECHO_REC_LENGTH;
}

int CEventPropertySheet::BinaryEvent_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::BinaryEvent_Rec\n");

//	CEventSummary*  summarypage	 = m_pParent->m_pSummaryPage;
//	CEventDataStat* datapage	 = m_pParent->m_pDataStatus;
	CBinaryDataStat* bindatapage = m_pParent->m_pBinaryDataStatus;
//	CUGrandDataStat* ugranddatapage = m_pParent->m_pUGrandDataStatus;
//	CGPSDataStat* gpsdatapage	 = m_pParent->m_pGPSDataStatus;
//	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;

	//make sure we have a good message
	int CheckSum;

	if ((CheckSum = Checksum(bAt,BINARYEVENT_REC_LENGTH)) == CHECKSUM_GOOD)
	{
		//area to build text version of message into
		char cBuff[64];
	
		//for ease in referencing sub-items
		struct BinaryEventRec *pRec;
		pRec = (BinaryEventRec*)bAt;
	
		//change to little endian
		Reverse((BYTE*)&pRec->uiTime,4);
		Reverse((BYTE*)&pRec->usNode,2);
	
		//save copy to eventually write to evt file
		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
		AcquireRec[m_dAcquireRecordCount].usSize = BINARYEVENT_REC_LENGTH;
		AcquireRec[m_dAcquireRecordCount].uiJulian = pRec->uiTime;
		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),pRec,BINARYEVENT_REC_LENGTH);
		if (m_pParent->m_bInDumpBBM)
		{
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}
	
		//build a text version for the user interface, dump file, cev, pfm, etc
		sprintf(cBuff, "%c%c %10u %02x %04x %02x %02x %02x %02x",
			pRec->RecTypeA,
			pRec->RecTypeB,
			pRec->uiTime,
			pRec->bStatus,
			pRec->usNode,
			pRec->bState,
			pRec->bMask,
			pRec->bReserved,
			pRec->bChecksum);

		//show on the summary tab
		UpdateSummary(cBuff);

		//send to text version to the watcher (if there is one)
		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);

		//get the time of the record
		CJulianTime Julian;
		Julian.Init(pRec->uiTime);

		char cTemp[32];
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATADATE3,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_DATADATE4,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATADATE2,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_DATADATE3,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATADATE1,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_DATADATE2,cTemp);
		sprintf(cTemp,"%04d.%02d.%02d",
			Julian.Year(),Julian.Month(),Julian.Day());
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_DATADATE1,cTemp);

		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATATIME3,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_DATATIME4,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATATIME2,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_DATATIME3,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATATIME1,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_DATATIME2,cTemp);
		sprintf(cTemp,"%02d:%02d:%02d",
			Julian.Hour(),Julian.Minute(),Julian.Second());
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_DATATIME1,cTemp);
	
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT3,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT4,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT2,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT3,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT1,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT2,cTemp);
		sprintf(cTemp,"%d (0x%02x)",
			pRec->bStatus, pRec->bStatus);
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT1,cTemp);

		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT3,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT4,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT2,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT3,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT1,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT2,cTemp);
		sprintf(cTemp,"%d",
			pRec->usNode);
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT1,cTemp);

		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT3,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT4,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT2,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT3,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT1,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT2,cTemp);
		itoa(pRec->bState,cTemp,2);
		CString cString(cTemp);
		while (cString.GetLength()<8) cString = '0'+cString;
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT1,cString);
	
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR3,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR4,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR2,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR3,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR1,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR2,cTemp);
		itoa(pRec->bMask,cTemp,2);
		cString = cTemp;
		while (cString.GetLength()<8) cString = '0'+cString;
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR1,cString);

		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S3,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S4,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S2,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S3,cTemp);
		bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S1,cTemp,sizeof(cTemp));
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S2,cTemp);
		sprintf(cTemp,"%d (0x%02x)",
			pRec->bReserved, pRec->bReserved);
		bindatapage->SetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S1,cTemp);

		//put it in the appropriate file
		char cBuff2[128];
#ifdef _DEBUG
		strcpy(cBuff2,"41620 BINARY EVENT RECORD ");
#else
		strcpy(cBuff2,"41620 ");
#endif
		strcat(cBuff2,cBuff);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff2);

		//put it in the dump file if required
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);//dump file
	}

	//tell CEventInstrument we got an acquire(able) record
	m_pParent->PostMessage(uIDC_RECEIVE,ACQUIRE_RECORD,CheckSum);

	//tell the message cracker how many bytes for this message
	return BINARYEVENT_REC_LENGTH;
}

int CEventPropertySheet::DumpOk_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::DumpOk_Rec\n");

	//check for good message
	int CheckSum;
	if ((CheckSum = Checksum(bAt,DUMPOK_REC_LENGTH,true)) == CHECKSUM_GOOD)
	{
		//area to copy bbm text from message 
		char szTemp[7];

		//get the bbm
		memcpy(szTemp,&bAt[2],6);
		szTemp[6] = NULL;

		//convert to internal form
		int iBytesInBBM = atoi(szTemp);

		//tell the tab dialog button how much in bbm
		m_pCloseButton->SetNumeric(iBytesInBBM,true);

		//tell the main mic dialog how much in bbm
		m_pParent->m_pButton->put_Numeric((ULONG)iBytesInBBM);

		//tell CEventInstrument's state machine how much bbm there still is
		m_pParent->PostMessage(uIDC_RECEIVE,IDC_BBM_VALUE,iBytesInBBM);

		//tell CEventInstrument's state machine we've received this message
		m_pParent->PostMessage(uIDC_RECEIVE,DUMP_OK_RECORD,true);
	}

	//build to tell the user interface about this message
	char cBuff[32];
	memcpy(cBuff,bAt,DUMPOK_REC_LENGTH);
	cBuff[DUMPOK_REC_LENGTH] = NULL;

	//send this to the summary tab
	UpdateSummary(cBuff);

	//send to the watch window -- if there is one
//	strcat(cBuff,"\r\n");
//	m_pParent->PostToWatch(cBuff);

	//send to dump file if need be
	if (m_pParent->m_bDoDump)
	{
		char cBuff2[64];
#ifdef _DEBUG
		strcpy(cBuff2,"41630 DUMP OK REC ");
#else
		strcpy(cBuff2,"41630 ");
#endif
		strcat(cBuff2,cBuff);
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);//dump file
	}

	return DUMPOK_REC_LENGTH;
}

int CEventPropertySheet::Inquire2_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::Inquire2_Rec\n");
	char cBuff[INQUIRE2_REC_LENGTH+8];
	//save copy of full record
	memcpy(cBuff,bAt,INQUIRE2_REC_LENGTH);
	cBuff[INQUIRE2_REC_LENGTH] = NULL;

	int CheckSum = Checksum((unsigned char*)cBuff,INQUIRE2_REC_LENGTH,true);
	if (CheckSum == CHECKSUM_GOOD)
	{
		char cTemp[64];
		
		CEventSummary*  summarypage	 = m_pParent->m_pSummaryPage;
		CEventDataStat* datapage	 = m_pParent->m_pDataStatus;
//		CBinaryDataStat* bindatapage = m_pParent->m_pBinaryDataStatus;
//		CUGrandDataStat* ugranddatapage = m_pParent->m_pUGrandDataStatus;
//		CGPSDataStat* gpsdatapage	 = m_pParent->m_pGPSDataStatus;
//		CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;

		//get julian time
		memcpy(cTemp,&cBuff[2],10);
		cTemp[10] = NULL;
		CJulianTime Julian(cTemp);

		if (Julian.Year() < 1990)
		{
			m_pParent->PostMessage(uIDC_RECEIVE,IDC_BADYEAR_VALUE,Julian.Year());
			WriteToFile(TO_CEV|TO_PFM,TYPE_COMP,&Julian,"41640 Year < 1990");
		}

		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
			      Julian.Hour(), Julian.Minute(), Julian.Second() );

		if (t1.GetStatus() == COleDateTime::valid)
		{

				COleDateTime t2 = COleDateTime::GetCurrentTime();
				COleDateTimeSpan ts = t1 - t2;  // Subtract 2 COleDateTimes
				ts += HALF_SECOND;

				strcpy(cTemp,Julian.Date());
				strcat(cTemp," ");
				strcat(cTemp,Julian.Time());
				if ((ts.GetTotalSeconds() >= m_dTime_Err ) ||
					(ts.GetTotalSeconds() <= -m_dTime_Err))
				{
					//m_pParent->m_pButton->SetTime(true,false);
					m_pParent->m_pButton->ShowIcon_Clock(VARIANT_TRUE);
					m_pParent->FlagTime(1,false);			
					datapage->SetColor(IDC_EVENTDATA_INQUIRETIME,COLOR_RED);
					summarypage->SetColor(IDC_EVENTSUMMARY_INQUIRETIME,COLOR_RED);
				Data_Tab |= 0x0008;
			} 
			else
			{
				datapage->SetColor(IDC_EVENTDATA_INQUIRETIME,COLOR_BLACK);
				Data_Tab &= ~(0x0008);
			}

	//		char temp[64];
			long days = ts.GetDays();
			if (days != 0)
				strcat(cTemp,ts.Format(" Delta: %D days, %H:%M:%S"));
			else 
				strcat(cTemp,ts.Format(" Delta: %H:%M:%S"));
	//		else 
	//		{
	//			sprintf(temp," Delta: %ld days, %02d:%02d:%02d", 
	//				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
	//			strcat(cTemp,temp);
	//		}
	//
			summarypage->SetDlgItemText(IDC_EVENTSUMMARY_INQUIRETIME,cTemp);
			datapage->SetDlgItemText(IDC_EVENTDATA_INQUIRETIME,cTemp);

			//get Main Unit ID
			strncpy(cTemp,&cBuff[12],2);
			cTemp[2]=NULL;
			unsigned short usTemp;
	//try{
			sscanf(cTemp,"%x",&usTemp);
	//}
	//catch(...)
	//{
	//	AfxMessageBox("Error at 12",MB_OK);
	//	AfxMessageBox(cTemp,MB_OK);
	//}

			sprintf(cTemp,"%d(0x%02x)",usTemp,usTemp);
			datapage->SetDlgItemText(IDC_EVENTDATA_UNITID,cTemp);

			//get status byte 
			short Status1;
			strncpy(cTemp,&cBuff[14],2);
			cTemp[2] = NULL;
	//try{
			sscanf(cTemp,"%2hx",&Status1);
	//}
	//catch(...)
	//{
	//	AfxMessageBox("Error at 13",MB_OK);
	//	AfxMessageBox(cTemp,MB_OK);
	//}


			short Status2;
			strncpy(cTemp,&cBuff[16],2);
			cTemp[2] = NULL;
	//try{
			sscanf(cTemp,"%2hx",&Status2);
	//}
	//catch(...)
	//{
	//	AfxMessageBox("Error at 14",MB_OK);
	//	AfxMessageBox(cTemp,MB_OK);
	//}
			
			//bit 3 bbm
			if (Status1 & BBMERROR)
			{
				if (Status2 & BBMFULL)
					strcpy(cTemp,"41660 Full");
				else if (Status2 & BBMBADRECSIZE)
					strcpy(cTemp,"41661 Bad Record Size");
				else if (Status2 & BBMBADWRITE)
					strcpy(cTemp,"41662 Write Failure");
				else if (Status2 & BBMBADOPCODE)
					strcpy(cTemp,"41663 Bad Opcode Fetched");
				else 
					strcpy(cTemp,"41664 Unknown Error");
				datapage->SetDlgItemText(IDC_EVENTDATA_BBMOK,cTemp);
				datapage->SetColor(IDC_EVENTDATA_BBMOK,COLOR_RED);
				summarypage->SetDlgItemText(IDC_EVENTSUMMARY_BBMOK,cTemp);
				summarypage->SetColor(IDC_EVENTSUMMARY_BBMOK,COLOR_RED);
				if (m_bBBMOk)
				{
					//char cTemp2[128];
					//strcat(cTemp,"\n\r");
					//strcat(cTemp2,cTemp);
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "41650 STATUS CHANGE - BATTERY BACKED UP MEMORY ERROR (1E)");
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, cTemp);
				}
				m_bBBMOk = false;
				Data_Tab |= 0x0004;
			}
			else
			{
				datapage->SetDlgItemText(IDC_EVENTDATA_BBMOK,OK);
				datapage->SetColor(IDC_EVENTDATA_BBMOK,COLOR_BLACK);
				summarypage->SetDlgItemText(IDC_EVENTSUMMARY_BBMOK,OK);
	//			summarypage->SetColor(IDC_EVENTSUMMARY_BBMOK,COLOR_BLACK);
				if (!m_bBBMOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "41670 STATUS CHANGE - NO BATTERY BACKED UP MEMORY ERROR (1E)");
				m_bBBMOk = true;
				Data_Tab &= ~(0x0004);
			}

			//get bbm bytes used
			memcpy(cTemp,&cBuff[18],6);
			cTemp[6] = NULL;
			unsigned int uiNumBBM;
			uiNumBBM = atoi(cTemp);
			sprintf(cTemp,"%d",uiNumBBM);

			//set all the displays
			datapage->SetDlgItemText(IDC_EVENTDATA_BYTESMEM,cTemp);
			m_pCloseButton->SetNumeric(uiNumBBM,true);
			m_pParent->m_pButton->put_Numeric((ULONG)uiNumBBM);

			//send message to parent saying we got this type of record
			//use second parameter as flag for good or bad checksum
			if (Data_Tab)
				SetTabColor(IDC_EVENTDATA_TAB,COLOR_RED);
			m_pParent->PostMessage(uIDC_RECEIVE,IDC_BBM_VALUE,uiNumBBM);
		}
		else
			CheckSum = CHECKSUM_BAD;
	}
	m_pParent->PostMessage(uIDC_RECEIVE,MONITOR_ID2_RECORD,CheckSum);	

	UpdateSummary(cBuff);
	strcat(cBuff,"\r\n");
//	m_pParent->PostToWatch(cBuff);
//	WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);
	if (m_pParent->m_bDoDump)
	{
		char cBuff2[1024];
#ifdef _DEBUG
		strcpy(cBuff2,"41680 INQUIRE2 REC ");
#else
		strcpy(cBuff2,"41680 ");
#endif
		strcat(cBuff2,cBuff);
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);//dump file
	}
	return INQUIRE2_REC_LENGTH;
}

int CEventPropertySheet::DumpStat_Rec(BYTE* bAt)
{

//	CEventSummary*  summarypage	 = m_pParent->m_pSummaryPage;
	CEventDataStat* datapage	 = m_pParent->m_pDataStatus;
//	CBinaryDataStat* bindatapage = m_pParent->m_pBinaryDataStatus;
//	CUGrandDataStat* ugranddatapage = m_pParent->m_pUGrandDataStatus;
//	CGPSDataStat* gpsdatapage	 = m_pParent->m_pGPSDataStatus;
//	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;
	
//	TRACE("CEventPropertySheet::DumpStat_Rec\n");
	int CheckSum = Checksum(bAt,STATUS_REC_LENGTH);
	if (CheckSum == CHECKSUM_GOOD)
		m_pParent->PostMessage(uIDC_RECEIVE,STATUS_RECORD,true);
	char cBuff[64];
#pragma pack(1)
	struct DumpStatRec {
		char RecTypeA;
		char RecTypeB;
		unsigned int uiTime;
		BYTE bStatus[8];
		BYTE bChecksum;
	} *pRec;
#pragma pack()
	pRec = (DumpStatRec*)bAt;
	Reverse((BYTE*)&pRec->uiTime,4);
	sprintf(cBuff, "%c%c %10u %02x %02x %02x %02x %02x %02x %02x %02x %02x",
		pRec->RecTypeA,
		pRec->RecTypeB,
		pRec->uiTime,
		pRec->bStatus[0],
		pRec->bStatus[1],
		pRec->bStatus[2],
		pRec->bStatus[3],
		pRec->bStatus[4],
		pRec->bStatus[5],
		pRec->bStatus[6],
		pRec->bStatus[7],
		pRec->bChecksum);

	UpdateSummary(cBuff);
	strcat(cBuff,"\r\n");
	m_pParent->PostToWatch(cBuff);
	CJulianTime Julian;
	Julian.Init(pRec->uiTime);
	
	char cTemp[32];
	sprintf(cTemp,"%4d.%02d.%02d %02d:%02d:%02d",
		Julian.Year(),Julian.Month(),Julian.Day(),
		Julian.Hour(),Julian.Minute(),Julian.Second());

	datapage->SetDlgItemText(IDC_EVENTDATA_STATUSTIME,cTemp);
	sprintf(cTemp,"%02x",pRec->bStatus[0]);
	datapage->SetDlgItemText(IDC_EVENTDATA_OPERATION2,cTemp);
	sprintf(cTemp,"%02x",pRec->bStatus[1]);
	datapage->SetDlgItemText(IDC_EVENTDATA_OPERATION3,cTemp);
	sprintf(cTemp,"%02x",pRec->bStatus[2]);
	datapage->SetDlgItemText(IDC_EVENTDATA_OPERATION4,cTemp);
	sprintf(cTemp,"%02x",pRec->bStatus[3]);
	datapage->SetDlgItemText(IDC_EVENTDATA_OPERATION5,cTemp);
	sprintf(cTemp,"%02x",pRec->bStatus[4]);
	datapage->SetDlgItemText(IDC_EVENTDATA_OPERATION6,cTemp);
	sprintf(cTemp,"%02x",pRec->bStatus[5]);
	datapage->SetDlgItemText(IDC_EVENTDATA_OPERATION7,cTemp);
	sprintf(cTemp,"%02x",pRec->bStatus[6]);
	datapage->SetDlgItemText(IDC_EVENTDATA_OPERATION8,cTemp);
	sprintf(cTemp,"%02x",pRec->bStatus[7]);
	datapage->SetDlgItemText(IDC_EVENTDATA_OPERATION9,cTemp);

	char cBuff2[1024];
#ifdef _DEBUG
	strcpy(cBuff2,"41690 DUMPSTAT REC ");
#else
	strcpy(cBuff2,"41690 ");
#endif
	strcat(cBuff2,cBuff);
	WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff2);
	if (m_pParent->m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);//dump file
	return STATUS_REC_LENGTH;
}

int CEventPropertySheet::ILONEndDump_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::ILONEndDump_Rec\n");

	int CheckSum;
	if ((CheckSum = Checksum(bAt,ENDDUMP_REC_LENGTH)) == CHECKSUM_GOOD)
	{
		//area to build text version of message into
		char cBuff[16];

		//for ease in referencing sub-items
		struct EndDump_Rec *pRec;
		pRec = (EndDump_Rec*)bAt;

		//build a text version for the user interface, dump file, cev, pfm, etc
		sprintf(cBuff, "%c%c %3d %02x",
			pRec->RecTypeA,
			pRec->RecTypeB,
			pRec->bCount,
			pRec->bChecksum);

		m_pParent->m_dNumberOfAcquires = pRec->bCount;
		
		//show on the summary tab
		UpdateSummary(cBuff);

		//send to text version to the watcher (if there is one)
		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);

		//put it in the dump file if required
		if (m_pParent->m_bDoDump)
		{
			char cBuff2[1024];
#ifdef _DEBUG
			strcpy(cBuff2,"41700 END DUMP REC ");
#else
			strcpy(cBuff2,"41700 ");
#endif
			strcat(cBuff2,cBuff);
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);//dump file
		}

//		//next batch will start saving at index 0
//		m_dAcquireRecordCount = 0;
	}
	m_pParent->PostMessage(uIDC_RECEIVE,END_DUMP_RECORD,CheckSum);

	return ENDDUMP_REC_LENGTH;
}

int CEventPropertySheet::Unknown_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::Unknown_Rec\n");
	char cBuff[64];
	sprintf(cBuff, "41710 %c%c UNKNOWN RECORD TYPE",bAt[0],bAt[1]);
	UpdateSummary(cBuff);
	strcat(cBuff,"\r\n");
	m_pParent->PostToWatch(cBuff);

	WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, cBuff);
	if (m_pParent->m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);//dump file
	return INT_MAX;
}

int CEventPropertySheet::VacossSealError_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::VacossSealError_Rec\n");

	int CheckSum;
	if ((CheckSum = Checksum(bAt,VACOSSERROR_REC_LENGTH)) == CHECKSUM_GOOD)  //<<QA>>
	{
		//area to build text version of message info
		char cBuff[64];

		CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;

		//for ease in referencing sub-items
		struct VacossError_Rec *pRec;
		pRec = (VacossError_Rec*)bAt;

		//change to little endian
		Reverse((BYTE*)&pRec->uiTime,4);
		Reverse((BYTE*)&pRec->usNode,2);
		Reverse((BYTE*)&pRec->usSealID,2);
		Reverse((BYTE*)&pRec->usErrorCode,2);

		//save copy to eventually write to evt file
		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
		AcquireRec[m_dAcquireRecordCount].uiJulian = pRec->uiTime;
		AcquireRec[m_dAcquireRecordCount].usSize = VACOSSERROR_REC_LENGTH;
		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),pRec,VACOSSERROR_REC_LENGTH);

		if (m_pParent->m_bInDumpBBM)
		{
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) 
				m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}

		//build a text version for the user interface, dump file, cev, pfm, etc
		//pRec->usSealID = 255;
		sprintf(cBuff, "%c%c %10u %02x %05u %05u %04x %02x%02x%02x%02x %02x",  //<<QA>>
			pRec->RecTypeA,
			pRec->RecTypeB,
			pRec->uiTime,
			pRec->bResponseType,
			pRec->usNode,			//<<QA>>
			pRec->usSealID,
			pRec->usErrorCode,
			pRec->bReserved[0],
			pRec->bReserved[1],
			pRec->bReserved[2],
			pRec->bReserved[3],
			pRec->bChecksum
			);

		//show on the summary tab
		UpdateSummary(cBuff);

		//send to text version to the watcher (if there is one)
		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);

		//get the time of the record
		CJulianTime Julian;
		Julian.Init(pRec->uiTime);

		//put it in the appropriate file
		char cBuff2[1024];
#ifdef _DEBUG
		strcpy(cBuff2,"41730 VACOSS ERROR REC ");
#else
		strcpy(cBuff2,"41730 ");
#endif
		strcat(cBuff2,cBuff);
		WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, cBuff2);

		//place it in the tabbed dialog 
		sprintf(cBuff,"%05u", pRec->usSealID);
		vacossdatastat->ShiftRight();
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_DATADATE1))->SetWindowText(Julian.m_pDate);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_DATATIME1))->SetWindowText(Julian.m_pTime);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_SEALID1))->SetWindowText(cBuff);  //<<QA>>
		sprintf(cBuff,"%05u", pRec->usNode);                                         //<<QA>>
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_NODE1))->SetWindowText(cBuff);    //<<QA>>
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_TYPE1))->SetWindowText("Error");

		sprintf(cBuff,"%04x:",pRec->usErrorCode);
		if (pRec->usErrorCode & 0x0001)
			strcat(cBuff,"Info");        //<<QA>> 
		if (pRec->usErrorCode & 0x0002)
			strcat(cBuff,"Events");      //<<QA>>
		if (pRec->usErrorCode & 0x0004)
			strcat(cBuff,"Status");      //<<QA>>
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_ERRDATE1))->SetWindowText(cBuff);

		sprintf(cBuff,"%02X %02X %02X %02X",
			pRec->bReserved[0],pRec->bReserved[1],
			pRec->bReserved[2],pRec->bReserved[3]);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_RESTIME1))->SetWindowText(cBuff);

		//put it in the dump file if required
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);//dump file
	}
	//tell the message cracker how many bytes for this message
	return VACOSSERROR_REC_LENGTH;  //<<QA>> 18
}

void CEventPropertySheet::GetVacossEvents(unsigned char* pAt, unsigned int *uiTime1, unsigned int *uiTime2)
{
	*uiTime1 = *pAt;
	*uiTime1 = (*uiTime1 << 8);
	*uiTime1 += *(pAt+1);
	*uiTime1 = (*uiTime1 << 4);
	*uiTime1 += ((*(pAt+4) & 0xf0) >> 4);

	*uiTime2 = *(pAt+2);
	*uiTime2 = (*uiTime2 << 8);
	*uiTime2 += *(pAt+3);
	*uiTime2 = (*uiTime2 << 4);
	*uiTime2 += (*(pAt+4) & 0x0f);
}

int CEventPropertySheet::VacossSealEvent_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::VacossSealEvent_Rec\n");

	int CheckSum;
	if ((CheckSum = Checksum(bAt,VACOSSEVENT_REC_LENGTH)) == CHECKSUM_GOOD)
	{
		//area to build text version of message into
		char cBuff[1024];

		CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;

		//for ease in referencing sub-item
		struct VacossEvent_Rec *pRec;
		pRec = (VacossEvent_Rec*) bAt;

		//change to little endian
		Reverse((BYTE*)&pRec->uiTime,4);
		Reverse((BYTE*)&pRec->usNode,2);

		//save copy to eventually write to evt file
		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
		AcquireRec[m_dAcquireRecordCount].uiJulian = pRec->uiTime;
		AcquireRec[m_dAcquireRecordCount].usSize = VACOSSEVENT_REC_LENGTH;

		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),pRec,VACOSSEVENT_REC_LENGTH);

		if (m_pParent->m_bInDumpBBM)
		{
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) 
				m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}

		//get the time of the record
		CJulianTime Julian;
		Julian.Init(pRec->uiTime);

		//place it in the tabbed dialog 
		//node number
		sprintf(cBuff,"%05u",pRec->usNode);
		vacossdatastat->ShiftRight();
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_DATADATE1))->SetWindowText(Julian.m_pDate);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_DATATIME1))->SetWindowText(Julian.m_pTime);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_NODE1))->SetWindowText(cBuff);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_TYPE1))->SetWindowText("Event");

		//seal ID
		unsigned short usId;
		usId = (pRec->bSealData[0] << 8) + pRec->bSealData[1];
		sprintf(cBuff, "%u", usId);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_SEALID1))->SetWindowText(cBuff);

		//get events 1 and 2
		unsigned int uiTime1;
		unsigned int uiTime2;
		GetVacossEvents(&(pRec->bSealData[2]),&uiTime1,&uiTime2);
		sprintf(cBuff,"%010u",uiTime1);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT01_1))->SetWindowText(cBuff);
		sprintf(cBuff,"%010u",uiTime2);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT02_1))->SetWindowText(cBuff);

		//get events 3 and 4
		GetVacossEvents(&(pRec->bSealData[7]),&uiTime1,&uiTime2);
		sprintf(cBuff,"%010u",uiTime1);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT03_1))->SetWindowText(cBuff);
		sprintf(cBuff,"%010u",uiTime2);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT04_1))->SetWindowText(cBuff);

		//get events 5 and 6
		GetVacossEvents(&(pRec->bSealData[12]),&uiTime1,&uiTime2);
		sprintf(cBuff,"%010u",uiTime1);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT05_1))->SetWindowText(cBuff);
		sprintf(cBuff,"%010u",uiTime2);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT06_1))->SetWindowText(cBuff);

		//get events 7 and 8
		GetVacossEvents(&(pRec->bSealData[17]),&uiTime1,&uiTime2);
		sprintf(cBuff,"%010u",uiTime1);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT07_1))->SetWindowText(cBuff);
		sprintf(cBuff,"%010u",uiTime2);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT08_1))->SetWindowText(cBuff);

		//get events 9 and 10
		GetVacossEvents(&(pRec->bSealData[22]),&uiTime1,&uiTime2);
		sprintf(cBuff,"%010u",uiTime1);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT09_1))->SetWindowText(cBuff);
		sprintf(cBuff,"%010u",uiTime2);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT10_1))->SetWindowText(cBuff);

		//get LRC
		sprintf(cBuff,"%02x",pRec->bSealData[27]);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_LRC1))->SetWindowText(cBuff);

		//get signature
		sprintf(cBuff,"%02x%02x%02x%02x\n%02x%02x%02x%02x",
			pRec->bSealData[28],pRec->bSealData[29],pRec->bSealData[30],pRec->bSealData[31],
			pRec->bSealData[32],pRec->bSealData[33],pRec->bSealData[34],pRec->bSealData[35]);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_SIG1))->SetWindowText(cBuff);
		
		//build a text version for the user interface, dump file, cev, pfm, etc
		//pRec->usNodeNumber = 255;
		sprintf(cBuff, "%c%c %10u %02x %05u %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
			"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
			"%02x%02x%02x%02x%02x%02x%02x %02x",
			pRec->RecTypeA,
			pRec->RecTypeB,
			pRec->uiTime,
			pRec->bResponseType,
			pRec->usNode,
			pRec->bSealData[ 0],pRec->bSealData[ 1],pRec->bSealData[ 2],pRec->bSealData[ 3],
			pRec->bSealData[ 4],pRec->bSealData[ 5],pRec->bSealData[ 6],pRec->bSealData[ 7],
			pRec->bSealData[ 8],pRec->bSealData[ 9],pRec->bSealData[10],pRec->bSealData[11],
			pRec->bSealData[12],pRec->bSealData[13],pRec->bSealData[14],pRec->bSealData[15],
			pRec->bSealData[16],pRec->bSealData[17],pRec->bSealData[18],pRec->bSealData[19],
			pRec->bSealData[20],pRec->bSealData[21],pRec->bSealData[22],pRec->bSealData[23],
			pRec->bSealData[24],pRec->bSealData[25],pRec->bSealData[26],pRec->bSealData[27],
			pRec->bSealData[28],pRec->bSealData[29],pRec->bSealData[30],pRec->bSealData[31],
			pRec->bSealData[32],pRec->bSealData[33],pRec->bSealData[34],pRec->bSealData[35],
			pRec->bChecksum
		);

		//show on the summary tab
		UpdateSummary(cBuff);

		//send to text version to the watcher (if there is one)
		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);

//		//get the time of the record
//		CJulianTime Julian;
//		Julian.Init(pRec->uiTime);

		//put it in the appropriate file
		char cBuff2[1024];
#ifdef _DEBUG
		strcpy(cBuff2,"41740 VACOSS EVENT REC ");
#else
		strcpy(cBuff2,"41740 ");
#endif
		strcat(cBuff2,cBuff);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff2);

		//put it in the dump file if required
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);//dump file
	}
	//tell CEventInstrument we got an acquire(able) record
	m_pParent->PostMessage(uIDC_RECEIVE,ACQUIRE_RECORD,CheckSum);

	//tell the message cracker how many bytes for this message
	return VACOSSEVENT_REC_LENGTH;
}

int CEventPropertySheet::VacossSealInfo_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::VacossSealInfo_Rec\n");

	//make sure we have a good message
	int CheckSum;
	if ((CheckSum = Checksum(bAt,VACOSSINFO_REC_LENGTH)) == CHECKSUM_GOOD)
	{
		//area to build text version of message into
		char cBuff[128];

		CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;

		//for ease in referencing sub-items
		struct VacossInfo_Rec *pRec;
		pRec = (VacossInfo_Rec*)bAt;

		//change to little endian
		Reverse((BYTE*)&pRec->uiTime,4);
		Reverse((BYTE*)&pRec->usNode,2);

		//save copy to eventually write to evt file
		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
		AcquireRec[m_dAcquireRecordCount].uiJulian      = pRec->uiTime;		
		AcquireRec[m_dAcquireRecordCount].usSize        = VACOSSINFO_REC_LENGTH;
		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),pRec,VACOSSINFO_REC_LENGTH);
		//save it only if we are in dumpbbm as opposed to dumpstat or some manually commanded
		//mode that triggered this type of message
		if (m_pParent->m_bInDumpBBM)
		{
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) 
				m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}

		//get the time of the record
		CJulianTime Julian;
		Julian.Init(pRec->uiTime);

		//place it in the tabbed dialog 
		sprintf(cBuff,"%05u",pRec->usNode);
		vacossdatastat->ShiftRight();
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_DATADATE1))->SetWindowText(Julian.m_pDate);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_DATATIME1))->SetWindowText(Julian.m_pTime);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_NODE1))->SetWindowText(cBuff);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_TYPE1))->SetWindowText("Information");

		//seal id
		unsigned short usId;
		usId = (pRec->bSealData[0] << 8) + pRec->bSealData[1];
		sprintf(cBuff, "%u", usId);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_SEALID1))->SetWindowText(cBuff);

		//initialization date and time
		unsigned int uiYear = pRec->bSealData[2] & 0x7f;
		if (uiYear < 90) 
			uiYear += 2000;
		else
			uiYear += 1900;
		unsigned int uiMonth = (pRec->bSealData[3] >> 4) & 0x0f;
		unsigned int uiDay = (pRec->bSealData[4] >> 3) & 0x1f;
		unsigned int uiHour = (pRec->bSealData[4] & 0x03) << 2;
		uiHour += (pRec->bSealData[5] >> 6) & 0x03;
		unsigned int uiMinutes = (pRec->bSealData[5] & 0x3f);
		unsigned int uiSeconds = (pRec->bSealData[6] & 0x1f);
		sprintf(cBuff,"%4u,%02u,%02u",uiYear,uiMonth,uiDay);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_ERRDATE1))->SetWindowText(cBuff);
		sprintf(cBuff,"%02u:%02u:%02u",uiHour,uiMinutes,uiSeconds);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_RESTIME1))->SetWindowText(cBuff);

		//# reader attachments since initialization
		sprintf(cBuff,"%u",pRec->bSealData[7]);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT01_1))->SetWindowText(cBuff);

		//minutes since init or case closeure/status
		unsigned int uiTimeStamp;
		uiTimeStamp = pRec->bSealData[8] << 8;
		uiTimeStamp += pRec->bSealData[9];
		uiTimeStamp = uiTimeStamp << 4;
		uiTimeStamp = uiTimeStamp & 0x000ffff0;
		sprintf(cBuff,"%u",uiTimeStamp);          //<<QA>>
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT02_1))->SetWindowText(cBuff);

		//status
		sprintf(cBuff,"%02x=",pRec->bSealData[10]);

		if (pRec->bSealData[10] & 0x80)
			strcat(cBuff,"FC"); //fiber closed
		else
			strcat(cBuff,"FO");

		strcat(cBuff,":");

		if (pRec->bSealData[10] & 0x20)
			strcat(cBuff,"BL");  //battery low
		else
			strcat(cBuff,"BN");

		strcat(cBuff,":");

		if (pRec->bSealData[10] & 0x10)
			strcat(cBuff,"CO");  //case opened
		else
			strcat(cBuff,"CC");

		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT03_1))->SetWindowText(cBuff);

		//events since init
		sprintf(cBuff,"%u",pRec->bSealData[11]);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_EVENT04_1))->SetWindowText(cBuff);

		//LRC
		sprintf(cBuff,"%02x",pRec->bSealData[12]);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_LRC1))->SetWindowText(cBuff);

		//sig
		sprintf(cBuff,"%02x%02x%02x%02x\n%02x%02x%02x%02x",
			pRec->bSealData[13],pRec->bSealData[14],
			pRec->bSealData[15],pRec->bSealData[16],
			pRec->bSealData[17],pRec->bSealData[18],
			pRec->bSealData[19],pRec->bSealData[20]);
		(vacossdatastat->GetDlgItem(IDC_VACOSSDATA_SIG1))->SetWindowText(cBuff);

		//build a text version for the user interface, dump file, cev, pfm, etc
		sprintf(cBuff, "%c%c %10u %02x %05u "
			"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
			"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
			"%02x %02x",
			pRec->RecTypeA,
			pRec->RecTypeB,
			pRec->uiTime,
			pRec->bResponseType,
			pRec->usNode,
			pRec->bSealData[ 0],pRec->bSealData[ 1],pRec->bSealData[ 2],pRec->bSealData[ 3],
			pRec->bSealData[ 4],pRec->bSealData[ 5],pRec->bSealData[ 6],pRec->bSealData[ 7],
			pRec->bSealData[ 8],pRec->bSealData[ 9],pRec->bSealData[10],pRec->bSealData[11],
			pRec->bSealData[12],pRec->bSealData[13],pRec->bSealData[14],pRec->bSealData[15],
			pRec->bSealData[16],pRec->bSealData[17],pRec->bSealData[18],pRec->bSealData[19],
			pRec->bSealData[20],
			pRec->bChecksum
		);

		//show on the summary tab
		UpdateSummary(cBuff);

		//send text version to the watcher (if there is one)
		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);


		//put it in the appropriate file
		char cBuff2[1024];
#ifdef _DEBUG
		strcpy(cBuff2,"41750 VACOSS INFO  REC ");
#else
		strcpy(cBuff2,"41750 ");
#endif
		strcat(cBuff2,cBuff);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff2);


		//put it in the dump file if required
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);//dump file
	}

	//tell CEventInstrument we got an acquire(able) record
	m_pParent->PostMessage(uIDC_RECEIVE,ACQUIRE_RECORD,CheckSum);


	//tell the message cracker how many bytes for this message
	return VACOSSINFO_REC_LENGTH;
}

#ifdef RFIDDEMO

int CEventPropertySheet::RFIDData_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::RFIDData_Rec\n");

	int CheckSum;
	if ((CheckSum = Checksum(bAt, RFIDDATA_REC_LENGTH)) == CHECKSUM_GOOD)
	{
		char cBuff[256];

		//for ease in referencing sub-items
		struct RFID_Rec *pRec;
		pRec = (RFID_Rec*)bAt;

		//change to little endian
//		Reverse((BYTE*)&pRec->uiTime,4);
//		Reverse((BYTE*)&pRec->usNode,2);
//		Reverse((BYTE*)&pRec->bRFIDSerialNumber,8);

		//save copy to eventually write to evt file
		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
		AcquireRec[m_dAcquireRecordCount].usSize = RFIDDATA_REC_LENGTH;
		AcquireRec[m_dAcquireRecordCount].uiJulian = pRec->uiTime;
		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),pRec,RFIDDATA_REC_LENGTH);

		if (m_pParent->m_bInDumpBBM)
		{
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}

		//get the time of the record
		CJulianTime Julian;
		Julian.Init(pRec->uiTime);

		//build a text version for the user interface, dump file, cev, pfm, etc
		sprintf(cBuff, "%c%c %10u %d %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %d %02x",
			pRec->RecTypeA,
			pRec->RecTypeB,
			pRec->uiTime,
			pRec->usNode,
			pRec->bRFIDSerialNumber[0],
			pRec->bRFIDSerialNumber[1],
			pRec->bRFIDSerialNumber[2],
			pRec->bRFIDSerialNumber[3],
			pRec->bRFIDSerialNumber[4],
			pRec->bRFIDSerialNumber[5],
			pRec->bRFIDSerialNumber[6],
			pRec->bRFIDSerialNumber[7],
			pRec->bRFIDSerialNumber[8],
			pRec->bRFIDSerialNumber[9],
			pRec->bRFIDSerialNumber[10],
			pRec->bRFIDSerialNumber[11],
			pRec->bCount,
			pRec->bChecksum);

		//show on the summary tab
		UpdateSummary(cBuff);

		//send text version to the watcher (if there is one)
		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);

		//put it in the appropriate file
		char cBuff2[1024];
#ifdef _DEBUG
		strcpy(cBuff2,"41xxx RFID DATA REC ");
#else
		strcpy(cBuff2,"41xxx ");
#endif
//		strcat(cBuff2,cBuff);
//		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff2);

		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,RFIDDATA_REC,CheckSum);
	return RFIDDATA_REC_LENGTH;
}

int CEventPropertySheet::RadData_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::RadData_Rec\n");

	int CheckSum;
	if ((CheckSum = Checksum(bAt, RADDATA_REC_LENGTH)) == CHECKSUM_GOOD)
	{
		char cBuff[128];

		//for ease in referencing sub-items
		struct RAD_Rec *pRec;
		pRec = (RAD_Rec*)bAt;

		if (pRec->RecTypeB != 'E')
		{
			//change to little endian
			Reverse((BYTE*)&pRec->uiTime,4);
			Reverse((BYTE*)&pRec->usNode,2);
			Reverse((BYTE*)&pRec->fCntRate,4);
			Reverse((BYTE*)&pRec->uiElapsed,4);
		}

		//save copy to eventually write to evt file
		//do this for UNIQUE RAD file else do below for BID file format
//		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
//		AcquireRec[m_dAcquireRecordCount].usSize = RADDATA_REC_LENGTH;
//		AcquireRec[m_dAcquireRecordCount].uiJulian = pRec->uiTime;
//		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),pRec,RADDATA_REC_LENGTH);

		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
		AcquireRec[m_dAcquireRecordCount].usSize = GRANDDATA_REC_LENGTH;
		AcquireRec[m_dAcquireRecordCount].uiJulian = pRec->uiTime;
		GRANDAcquire_Rec ARec;
		
		ARec.RecTypeA = pRec->RecTypeA;
		ARec.RecTypeB = pRec->RecTypeB;
		ARec.JulianSeconds = pRec->uiTime;
		ARec.StatusByte = pRec->usNode;
		ARec.NeutronChA = 0.0;
		ARec.NeutronChB = 0.0;
		ARec.NeutronChC = 0.0;
		ARec.GammaCh1 = pRec->fCntRate;
		ARec.GammaCh1Sigma = 0.0;
		ARec.GammaCh2 = 0.0;
		ARec.GammaCh2Sigma = 0.0;
		ARec.CountTime = pRec->uiElapsed;
		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),&ARec,GRANDDATA_REC_LENGTH);

		if (m_pParent->m_bInDumpBBM)
		{
			m_dAcquireRecordCount++;
TRACE("AcquireRecCount: %d NUMBEROFACQUIRERECS %d\n",m_dAcquireRecordCount, NUMBEROFACQUIRERECS);
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}

		//get the time of the record
		CJulianTime Julian;
		Julian.Init(pRec->uiTime);

		//build a text version for the user interface, dump file, cev, pfm, etc
		sprintf(cBuff, "%c%c %10u %02x %10.8f %10u %02x %02x %02x",
			pRec->RecTypeA,
			pRec->RecTypeB,
			pRec->uiTime,
			pRec->usNode,
			pRec->fCntRate,
			pRec->uiElapsed,
			pRec->bReserved1,
			pRec->bReserved2,
			pRec->bChecksum);

		//show on the summary tab
		UpdateSummary(cBuff);

		//send text version to the watcher (if there is one)
		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);


		//put it in the appropriate file
		char cBuff2[1024];
#ifdef _DEBUG
		strcpy(cBuff2,"41xxx RAD DATA REC ");
#else
		strcpy(cBuff2,"41xxx ");
#endif
		strcat(cBuff2,cBuff);
//		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff2);

		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,RADDATA_REC,CheckSum);
TRACE("%s\n",CheckSum?"GOOD":"BAD");
	return RADDATA_REC_LENGTH;
}

int CEventPropertySheet::Rad2Data_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::RadData_Rec\n");

	int CheckSum;
	if ((CheckSum = Checksum(bAt, RAD2DATA_REC_LENGTH)) == CHECKSUM_GOOD)
	{
		char cBuff[128];

		//for ease in referencing sub-items
		struct RAD2_Rec *pRec;
		pRec = (RAD2_Rec*)bAt;

		//don't need to do this if coming from a "rabbit" board
		//everything will be little endian
//		if (pRec->RecTypeB != 'E')
//		{
//			//change to little endian
//			Reverse((BYTE*)&pRec->uiTime,4);
//			Reverse((BYTE*)&pRec->usNode,2);
//			Reverse((BYTE*)&pRec->fCntRate,4);
//			Reverse((BYTE*)&pRec->uiElapsed,4);
//		}


		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
		AcquireRec[m_dAcquireRecordCount].usSize = RABBITDATA_REC_LENGTH;
		AcquireRec[m_dAcquireRecordCount].uiJulian = pRec->uiTime;
		RABBITAcquire_Rec ARec;
		
		ARec.RecTypeA = pRec->RecTypeA;
		ARec.RecTypeB = pRec->RecTypeB;
		ARec.JulianSeconds = pRec->uiTime;
		ARec.StatusByte = pRec->usNode;

//		ARec.NeutronChA = 0.0;
//		ARec.NeutronChB = 0.0;
//		ARec.NeutronChC = 0.0;
//		ARec.GammaCh1 = pRec->fCntRate;
//		ARec.GammaCh1Sigma = 0.0;
//		ARec.GammaCh2 = 0.0;
//		ARec.GammaCh2Sigma = 0.0;
		for (int i = 0; i < 8; i++)
			ARec.uiCounts[i] = pRec->uiCounts[i];

		ARec.CountTime = pRec->uiElapsed;

		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),&ARec,RABBITDATA_REC_LENGTH);

		if (m_pParent->m_bInDumpBBM)
		{
			m_dAcquireRecordCount++;
TRACE("RabbitRecCount: %d NUMBEROFACQUIRERECS %d\n",m_dAcquireRecordCount, NUMBEROFACQUIRERECS);
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}

		//get the time of the record
		CJulianTime Julian;
		Julian.Init(pRec->uiTime);

		//build a text version for the user interface, dump file, cev, pfm, etc
		sprintf(cBuff, "%c%c %10u %02x %10u %10u %10u %10u %10u  %10u %10u %10u %10u %02x %02x %02x",
			pRec->RecTypeA,
			pRec->RecTypeB,
			pRec->uiTime,
			pRec->usNode,
			pRec->uiCounts[0],
			pRec->uiCounts[1],
			pRec->uiCounts[2],
			pRec->uiCounts[3],
			pRec->uiCounts[4],
			pRec->uiCounts[5],
			pRec->uiCounts[6],
			pRec->uiCounts[7],
			pRec->uiElapsed,
			pRec->bReserved1,
			pRec->bReserved2,
			pRec->bChecksum);

		//show on the summary tab
		UpdateSummary(cBuff);

		//send text version to the watcher (if there is one)
		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);


		//put it in the appropriate file
		char cBuff2[1024];
#ifdef _DEBUG
		strcpy(cBuff2,"41xxx RAD2 DATA REC ");
#else
		strcpy(cBuff2,"41xxx ");
#endif
		strcat(cBuff2,cBuff);
//		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff2);

		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,RAD2DATA_REC,CheckSum);
TRACE("%s\n",CheckSum?"GOOD":"BAD");
	return RAD2DATA_REC_LENGTH;
}



int CEventPropertySheet::LGPSData_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::GPSData_Rec\n");

//	CEventSummary*  summarypage	 = m_pParent->m_pSummaryPage;
//	CEventDataStat* datapage	 = m_pParent->m_pDataStatus;
//	CBinaryDataStat* bindatapage = m_pParent->m_pBinaryDataStatus;
//	CUGrandDataStat* ugranddatapage = m_pParent->m_pUGrandDataStatus;
	CGPSDataStat* gpsdatapage	 = m_pParent->m_pGPSDataStatus;
//	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;

	int CheckSum;
	if ((CheckSum = Checksum(bAt, LGPSDATA_REC_LENGTH)) == CHECKSUM_GOOD)
	{
		//area to build text version of message into
		char cBuff[1024];

		//for ease in referencing sub-items
		struct LGPS_Rec *pLRec;
		struct GPS_Rec *pRec;
		pLRec = (LGPS_Rec*)bAt;

		struct GPS_Rec sRec;
		pRec = &sRec;
		BYTE* pBytes = (BYTE*)&sRec;

		pRec->RecTypeA =	0x33;//pLRec->RecTypeA;
		pRec->RecTypeB =	0x41;//pLRec->RecTypeB;
		pRec->uiTime =		pLRec->uiTime;
		pRec->usNode =		pLRec->usNode;
		pRec->dLatitude =	pLRec->iLatitudeDegrees + pLRec->fLatitudeMinutes/60.0;
		pRec->dLongitude =	pLRec->iLongitudeDegrees + pLRec->fLongitudeMinutes/60.0;
		pRec->dAltitude =	pLRec->fAltitude;
		pRec->bHour =		pLRec->bHour;
		pRec->bMinute =		pLRec->bMinute;
		pRec->bSecond =		pLRec->bSecond;
		pRec->bDay =		pLRec->bDay;
		pRec->bMonth =		pLRec->bMonth;
		pRec->usYear =		pLRec->usYear + 2000;
		pRec->bUnused =		pLRec->bNumSatellites;//# satelites
		pRec->fFixTimeA =	pLRec->fFixTimeA;
		
		//1 knots * 1.852 = km/h = 1852 m/h = * 60 m/min  = * 3600
		//1 knot * 1852* 3600 = M/S
		pRec->fEastVelocity = (float)(pLRec->fEastVelocity * sin((pLRec->fNorthVelocity/360.0)*PI2)*1852.0*3600.0);
		pRec->fNorthVelocity = (float)(pLRec->fEastVelocity * cos((pLRec->fNorthVelocity/360.0)*PI2)*1852.0*3600.0);

		pRec->fUpVelocity = pLRec->fUpVelocity;
		pRec->fClockBiasRate = 0.0;
		pRec->fFixTimeB = pLRec->fFixTimeB;
		
		pRec->bChecksum = 0;
		for (int i = 0; i < sizeof(GPS_Rec)-1; i++)
			pRec->bChecksum += pBytes[i];

		//save copy to eventually write to evt file
		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
		AcquireRec[m_dAcquireRecordCount].usSize = GPSDATA_REC_LENGTH;
		AcquireRec[m_dAcquireRecordCount].uiJulian = pRec->uiTime;
		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),pRec,GPSDATA_REC_LENGTH);

		if (m_pParent->m_bInDumpBBM)
		{
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}

		//get the time of the record
		CJulianTime Julian;
		Julian.Init(pRec->uiTime);

		char cTemp[32];
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATADATE4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATADATE3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATADATE2,cTemp);
		sprintf(cTemp,"%04d.%02d.%02d",
			Julian.Year(),Julian.Month(),Julian.Day());
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATADATE1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATATIME4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATATIME3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATATIME2,cTemp);
		sprintf(cTemp,"%02d:%02d:%02d",
			Julian.Hour(),Julian.Minute(),Julian.Second());
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATATIME1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEACNT4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEACNT3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEACNT2,cTemp);
		sprintf(cTemp,"%d", pRec->usNode);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEACNT1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEBCNT4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEBCNT3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEBCNT2,cTemp);
		sprintf(cTemp,"%11.7f",	pRec->dLatitude);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEBCNT1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSECCNT4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSECCNT3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSECCNT2,cTemp);
		sprintf(cTemp,"%11.7f",	pRec->dLongitude);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSECCNT1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1GR4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1GR3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1GR2,cTemp);
		sprintf(cTemp,"%11.7f",	pRec->dAltitude);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1GR1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S2,cTemp);
//		sprintf(cTemp,"%11.7f",	pRec->dClockBias);
		sprintf(cTemp,"%04d.%02d.%02d %02d:%02d:%02d",
			pRec->usYear,pRec->bMonth,pRec->bDay,
			pRec->bHour,pRec->bMinute,pRec->bSecond);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_SATELITES4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_SATELITES3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_SATELITES2,cTemp);
		sprintf(cTemp,"%u", pRec->bUnused);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_SATELITES1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S7,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S8,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S6,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S7,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S5,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S6,cTemp);
//		sprintf(cTemp,"%11.0f", pRec->fFixTimeA);
//		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S5,cTemp);
		sprintf(cTemp,"%02d:%02d:%02d",
//			pRec->usYear,pRec->bMonth,pRec->bDay,
			pRec->bHour,pRec->bMinute,pRec->bSecond);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S5,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S15,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S16,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S14,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S15,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S13,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S14,cTemp);
		sprintf(cTemp,"%11.7f",pRec->fEastVelocity);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S13,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S19,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S20,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S18,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S19,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S17,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S18,cTemp);
		sprintf(cTemp,"%11.7f",pRec->fNorthVelocity);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S17,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S23,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S24,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S22,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S23,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S21,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S22,cTemp);
		sprintf(cTemp,"%11.7f",pRec->fUpVelocity);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S21,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S27,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S28,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S26,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S27,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S25,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S26,cTemp);
		sprintf(cTemp,"%11.7f",pRec->fClockBiasRate);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S25,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S31,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S32,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S30,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S31,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S29,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S30,cTemp);
		sprintf(cTemp,"%11.0f",pRec->fFixTimeB);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S29,cTemp);

		//build a text version for the user interface, dump file, cev, pfm, etc
		sprintf(cBuff, "%c%c %10u %5hu %11.7f %11.7f %11.7f %4d.%2d.%2d %02d:%02d:%02d %d %11.0f %11.7f %11.7f %11.7f %11.7f %11.0f %02x",
			pRec->RecTypeA,		pRec->RecTypeB,		pRec->uiTime,		
			pRec->usNode,		pRec->dLatitude,	pRec->dLongitude,	
			pRec->dAltitude,	

			pRec->usYear,pRec->bMonth,pRec->bDay,
			pRec->bHour,pRec->bMinute,pRec->bSecond,

			pRec->bUnused,
			
			pRec->fFixTimeA,	
			pRec->fEastVelocity,pRec->fNorthVelocity,pRec->fUpVelocity,	
			pRec->fClockBiasRate,	pRec->fFixTimeB,	pRec->bChecksum
		);

		//show on the summary tab
		UpdateSummary(cBuff);

		//send to text version to the watcher (if there is one)
		if (strlen(cBuff)>256)
		{
			cBuff[256]='\0';
		}

		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);


//		//put it in the appropriate file
//		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);

		//put it in the dump file if required
		if (m_pParent->m_bDoDump)
		{
			char cBuff2[1024];
#ifdef _DEBUG
			strcpy(cBuff2,"41760 GPS DATA REC ");
#else
			strcpy(cBuff2,"41760 ");
#endif
			strcat(cBuff2,cBuff);
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);//dump file
		}
	}
	//tell CEventInstrument we got an acquire(able) record
	m_pParent->PostMessage(uIDC_RECEIVE,ACQUIRE_RECORD,CheckSum);

	//tell the message cracker how many bytes for this message
	return LGPSDATA_REC_LENGTH;
}


#endif


int CEventPropertySheet::GPSData_Rec(BYTE* bAt)
{
//	TRACE("CEventPropertySheet::GPSData_Rec\n");

//	CEventSummary*  summarypage	 = m_pParent->m_pSummaryPage;
//	CEventDataStat* datapage	 = m_pParent->m_pDataStatus;
//	CBinaryDataStat* bindatapage = m_pParent->m_pBinaryDataStatus;
//	CUGrandDataStat* ugranddatapage = m_pParent->m_pUGrandDataStatus;
	CGPSDataStat* gpsdatapage	 = m_pParent->m_pGPSDataStatus;
//	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;

	int CheckSum;
	if ((CheckSum = Checksum(bAt, GPSDATA_REC_LENGTH)) == CHECKSUM_GOOD)
	{
		//area to build text version of message into
		char cBuff[1024];

		//for ease in referencing sub-items
		struct GPS_Rec *pRec;
		pRec = (GPS_Rec*)bAt;

		//change to little endian
		Reverse((BYTE*)&pRec->uiTime,4);
		Reverse((BYTE*)&pRec->usNode,2);
		Reverse((BYTE*)&pRec->dLatitude,8);
		Reverse((BYTE*)&pRec->dLongitude,8);
		Reverse((BYTE*)&pRec->dAltitude,8);
		//Reverse((BYTE*)&pRec->dClockBias,8);
		Reverse((BYTE*)&pRec->usYear,2);
		Reverse((BYTE*)&pRec->fFixTimeA,4);
		Reverse((BYTE*)&pRec->fEastVelocity,4);
		Reverse((BYTE*)&pRec->fNorthVelocity,4);
		Reverse((BYTE*)&pRec->fUpVelocity,4);
		Reverse((BYTE*)&pRec->fClockBiasRate,4);
		Reverse((BYTE*)&pRec->fFixTimeB,4);

		pRec->dLatitude*=180.0/3.1415926535898;	// convert from rad to degrees  //PJM
		pRec->dLongitude*=180.0/3.1415926535898;	// convert from rad to degrees //PJM

		//save copy to eventually write to evt file
		memset(&AcquireRec[m_dAcquireRecordCount],NULL,sizeof(EventAcquireRecord));
		AcquireRec[m_dAcquireRecordCount].usSize = GPSDATA_REC_LENGTH;
		AcquireRec[m_dAcquireRecordCount].uiJulian = pRec->uiTime;
		memcpy(&(AcquireRec[m_dAcquireRecordCount].bData),pRec,GPSDATA_REC_LENGTH);

		if (m_pParent->m_bInDumpBBM)
		{
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}

		//get the time of the record
		CJulianTime Julian;
		Julian.Init(pRec->uiTime);

		char cTemp[32];
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATADATE4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATADATE3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATADATE2,cTemp);
		sprintf(cTemp,"%04d.%02d.%02d",
			Julian.Year(),Julian.Month(),Julian.Day());
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATADATE1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATATIME4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATATIME3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATATIME2,cTemp);
		sprintf(cTemp,"%02d:%02d:%02d",
			Julian.Hour(),Julian.Minute(),Julian.Second());
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_DATATIME1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEACNT4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEACNT3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEACNT2,cTemp);
		sprintf(cTemp,"%d", pRec->usNode);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEACNT1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEBCNT4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEBCNT3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEBCNT2,cTemp);
		sprintf(cTemp,"%11.7f",	pRec->dLatitude);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSEBCNT1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSECCNT4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSECCNT3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSECCNT2,cTemp);
		sprintf(cTemp,"%11.7f",	pRec->dLongitude);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_PULSECCNT1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1GR4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1GR3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1GR2,cTemp);
		sprintf(cTemp,"%11.7f",	pRec->dAltitude);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1GR1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S2,cTemp);
//		sprintf(cTemp,"%11.7f",	pRec->dClockBias);
		sprintf(cTemp,"%4d.%2d.%2d %02d:%02d:%02d",
			pRec->usYear,pRec->bMonth,pRec->bDay,
			pRec->bHour,pRec->bMinute,pRec->bSecond);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES3,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_SATELITES4,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES2,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_SATELITES3,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES1,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_SATELITES2,cTemp);
		sprintf(cTemp,"%u", pRec->bUnused);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_SATELITES1,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S7,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S8,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S6,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S7,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S5,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S6,cTemp);
//		sprintf(cTemp,"%11.0f", pRec->fFixTimeA);
//		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S5,cTemp);
		sprintf(cTemp,"%02d:%02d:%02d",
//			pRec->usYear,pRec->bMonth,pRec->bDay,
			pRec->bHour,pRec->bMinute,pRec->bSecond);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S5,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S15,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S16,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S14,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S15,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S13,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S14,cTemp);
		sprintf(cTemp,"%11.7f",pRec->fEastVelocity);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S13,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S19,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S20,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S18,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S19,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S17,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S18,cTemp);
		sprintf(cTemp,"%11.7f",pRec->fNorthVelocity);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S17,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S23,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S24,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S22,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S23,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S21,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S22,cTemp);
		sprintf(cTemp,"%11.7f",pRec->fUpVelocity);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S21,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S27,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S28,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S26,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S27,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S25,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S26,cTemp);
		sprintf(cTemp,"%11.7f",pRec->fClockBiasRate);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S25,cTemp);

		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S31,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S32,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S30,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S31,cTemp);
		gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S29,cTemp,sizeof(cTemp));
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S30,cTemp);
		sprintf(cTemp,"%11.0f",pRec->fFixTimeB);
		gpsdatapage->SetDlgItemText(IDC_GPSDATA_GAMMA1S29,cTemp);

		//build a text version for the user interface, dump file, cev, pfm, etc
		sprintf(cBuff, "%c%c %10u %5hu %11.7f %11.7f %11.7f %4d.%2d.%2d %02d:%02d:%02d %d %11.0f %11.7f %11.7f %11.7f %11.7f %11.0f %02x",
			pRec->RecTypeA,		pRec->RecTypeB,		pRec->uiTime,		
			pRec->usNode,		pRec->dLatitude,	pRec->dLongitude,	
			pRec->dAltitude,	

			pRec->usYear,pRec->bMonth,pRec->bDay,
			pRec->bHour,pRec->bMinute,pRec->bSecond,

			pRec->bUnused,
			
			pRec->fFixTimeA,	
			pRec->fEastVelocity,pRec->fNorthVelocity,pRec->fUpVelocity,	
			pRec->fClockBiasRate,	pRec->fFixTimeB,	pRec->bChecksum
		);

		//show on the summary tab
		UpdateSummary(cBuff);

		//send to text version to the watcher (if there is one)
		if (strlen(cBuff)>256)
		{
			cBuff[256]='\0';
		}

		strcat(cBuff,"\r\n");
		m_pParent->PostToWatch(cBuff);


//		//put it in the appropriate file
//		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);

		//put it in the dump file if required
		if (m_pParent->m_bDoDump)
		{
			char cBuff2[1024];
#ifdef _DEBUG
			strcpy(cBuff2,"41760 GPS DATA REC ");
#else
			strcpy(cBuff2,"41760 ");
#endif
			strcat(cBuff2,cBuff);
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);//dump file
		}
	}
	//tell CEventInstrument we got an acquire(able) record
	m_pParent->PostMessage(uIDC_RECEIVE,ACQUIRE_RECORD,CheckSum);

	//tell the message cracker how many bytes for this message
	return GPSDATA_REC_LENGTH;
}

void CEventPropertySheet::Reverse(BYTE *sz, int iLength)
{
	BYTE cTemp;
	for (int i = 0; i < (iLength/2); i++)
	{
		cTemp = sz[i];
		sz[i] = sz[iLength - i - 1];
		sz[iLength - i - 1] = cTemp;
	}
}

int CEventPropertySheet::AppendCheckSum(char *pBinaryAt)
{
    unsigned short sum;		/* sum of all bytes in checksum */
	sum = 0;
	for (unsigned int i=0; i < strlen(pBinaryAt); i++)
		sum = (unsigned short)(sum + pBinaryAt[i]);
	sum &= 0x00ff;	/* keep only relevant part of checksum */
	char cTemp[4];
	sprintf(cTemp,"%02hx",sum);
	strcat(pBinaryAt,cTemp);
	return sum;
}

int 
CEventPropertySheet::Checksum(BYTE* pAt, int iLength, bool bASCIICheckSum)
{
	//return CHECKSUM_GOOD;  //Hack

    unsigned short usSum;		/* sum of all bytes in checksum */
	unsigned short usCSOffset;
	unsigned short usCheckSum;
    /* checksum is a one byte sum of all bytes up to the checksum byte */
	usSum = 0;
	usCSOffset = 1;
	if (bASCIICheckSum)
		usCSOffset = 2;

	for (int i=0; i<(iLength-usCSOffset); i++)
		usSum = (unsigned short)(usSum + pAt[i]);

	usSum &= 0x00ff;	/* keep only relevant part of checksum */
	if (bASCIICheckSum)
	{
		char szTemp[3];
		try{
			//sscanf((char*)&pAt[iLength-usCSOffset],"%2hx",&usCheckSum);
			memcpy(szTemp,&pAt[iLength-2],2);
			szTemp[2] = NULL;
			sscanf(szTemp,"%hx",&usCheckSum);
		}
		catch(...)
		{
			FILE* outfile = fopen("C:\\errorout.txt","at");
			COleDateTime cNow = COleDateTime::GetCurrentTime();
			fprintf(outfile,"At %s Length %d Offset %d>>>\n",cNow.Format("%Y %m %d %H:%M:%S"),iLength,usCSOffset);
			try{
				for (i = 0; i < iLength; i++) 
					fprintf(outfile,"%2d 0x%02x %c\n",i,pAt[i],pAt[i]);
			}
			catch(...)
			{
				fprintf(outfile,"error at i=%d",i);
			}
			fprintf(outfile,"<<<");
			fclose(outfile);
			usCheckSum = usSum^0x00ff;
		}
	}
	else
		usCheckSum = pAt[iLength-1];
	if (usSum == usCheckSum)
		/* calculated checksum matches checksum in record */
		return CHECKSUM_GOOD;

//	CEventSummary*  summarypage	 = m_pParent->m_pSummaryPage;
//	CEventDataStat* datapage	 = m_pParent->m_pDataStatus;
//	CBinaryDataStat* bindatapage = m_pParent->m_pBinaryDataStatus;
//	CUGrandDataStat* ugranddatapage = m_pParent->m_pUGrandDataStatus;
//	CGPSDataStat* gpsdatapage	 = m_pParent->m_pGPSDataStatus;
//	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;

//	COleDateTime Now = COleDateTime::GetCurrentTime();
//	CString DateTime = Now.Format("CheckSum: %d/%H:%M:%S\n");
//	DateTime += Record;
//	generalpage->SetDlgItemText(IDC_GENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
//	generalpage->SetColor(IDC_GENERAL_CHECKSUM,COLOR_RED);
//	generalpage->Invalidate();
//	m_pParent->FlagComm(1,false);			

//	SetTabColor(IDC_GENERAL_TAB,COLOR_RED);

	WriteToFile(TO_PFM, TYPE_COMP, NULL, "41770 CHECKSUM ERROR\r\n");
//	WriteToFile(TO_PFM, TYPE_COMP, NULL, Record);

	if (((CEventInstrument*)m_pParent)->m_bDoDump)
	{
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "41770 CHECKSUM ERROR\r\n");
//		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);
	}

//debug to test ILON problem -- force pause on checksum error
//	AfxGetMainWnd()->PostMessage(IDC_FORCE_PAUSE, NULL, NULL);
//end debug to test ILON problem

    return CHECKSUM_BAD;	       /* checksums do not match */
}

bool 
CEventPropertySheet::LengthOk(char* Record, int length)
{
	//Check the length of the record	
	if (strlen(Record) == (unsigned)length)
		return true;

	//must have failed the length check
//	CEventSummary*  summarypage		= m_pParent->m_pSummaryPage;
//	CEventDataStat* datapage		= m_pParent->m_pDataStatus;
//	CBinaryDataStat* bindatapage	= m_pParent->m_pBinaryDataStatus;
//	CUGrandDataStat* ugranddatapage = m_pParent->m_pUGrandDataStatus;
//	CGPSDataStat* gpsdatapage		= m_pParent->m_pGPSDataStatus;
//	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;
//	CEventModParam* modparpage		= m_pParent->m_pModParams;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("Length: %d/%H:%M:%S\n");
	DateTime += Record;
	DateTime = DateTime.Left(DateTime.GetLength()-2);
/*
	generalpage->SetDlgItemText(IDC_GENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
	generalpage->SetColor(IDC_GENERAL_CHECKSUM,COLOR_RED);
	generalpage->GetDlgItem(IDC_GENERAL_CHECKSUM)->Invalidate();
//	m_pParent->FlagComm(1,false);			
	SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
*/

	WriteToFile(TO_PFM, TYPE_COMP, NULL, "41780 LENGTH ERROR\r\n");
	char cBuff[2048];
	strcpy(cBuff,"41790 ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

	if (m_pParent->m_bDoDump)
	{
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "41780 LENGTH ERROR");
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
	}
//debug to test ILON problem -- force pause on length error
//	AfxGetMainWnd()->PostMessage(IDC_FORCE_PAUSE, NULL, NULL);
//end debug to test ILON problem
    return false;
}

void
CEventPropertySheet::MakeMyPath(const char* path)
{
	//recursive routine to create all 
	//of the directories down the "path"
	//NOTE: existance of a "file" with the 
	//same name as a node in the
	//path will cause this routine to fail
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];   
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];		
	if (access(path,0)==0) return;
	_splitpath(path,drive,dir,fname,ext);
	if (strlen(dir)==0) return;
	_makepath(path_buffer,drive,dir,NULL,NULL);
	if (path_buffer[strlen(path_buffer)-1] == '\\')
		path_buffer[strlen(path_buffer)-1] = NULL;
	MakeMyPath(path_buffer);
	//make sure directory exists
//	_mkdir(path_buffer);
	CreateDirectory(path_buffer,NULL);
}

//this function should go into the parent class
void 
CEventPropertySheet::BuildFileName(CString &cResult,
	CString cFormat,CString cPath,CString cType,
	CString cID,
	int iYear,	int iMonth,	int iDay,
	int iHour,	int iMinute,int iSecond,
	CString cRFID, CString cSuffix 
	)
{
	cResult.Empty();
	if (cPath.GetLength())
	{
		cResult = cPath;
		if (cResult.Right(1) != '\\')
			cResult += '\\';
	}
	CString cTemp;
	bool bHaveEscape = false;
	for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
	{
		if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
			bHaveEscape = true;
		else if (bHaveEscape)
		{
			bHaveEscape = false;
			switch (cFormat[iSourceIndex]) {
			case 'T':
				cResult += cType;
				break;
			case 'I':
				{
					int iLength = cID.GetLength();
					while (iLength < 3)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'i':
				{
					int iLength = cID.GetLength();
					while (iLength < 2)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'y':
				cTemp.Format("%04d",iYear);
				cResult += cTemp;
				break;
			case 'm':
				cTemp.Format("%02d",iMonth);
				cResult += cTemp;
				break;
			case 'd':
				cTemp.Format("%02d",iDay);
				cResult += cTemp;
				break;
			case 'H':
				cTemp.Format("%02d",iHour);
				cResult += cTemp;
				break;
			case 'M':
				cTemp.Format("%02d",iMinute);
				cResult += cTemp;
				break;
			case 'S':
				cTemp.Format("%02d",iSecond);
				cResult += cTemp;
				break;
			default:
				cResult += cFormat[iSourceIndex];
			}
		}
		else
			cResult += cFormat[iSourceIndex];
	}
//	if (!cRFID.IsEmpty())
//		if ((cResult.Right(1) != '.') && (cSuffix.Left(1) != '.'))
//			cResult += '.';
//		cResult += cRFID;

	if (cSuffix.GetLength())
	{
		if ((cResult.Right(1) != '.') && (cSuffix.Left(1) != '.'))
			cResult += '.';
		cResult += cSuffix;
	}
}

bool CEventPropertySheet::WriteToEVTFile(int iIndex)
{
//	TRACE("CEventPropertySheet::WriteToEVTFile(iIndex = %d)\n", iIndex);

	CJulianTime Time;
	Time.Init(AcquireRec[iIndex].uiJulian);

#ifndef SINGLE_BINARY_FILE
	char szEvtSuffix[4];
	unsigned short usNode;
	CString cType;
	//CString cRFID;
//	cRFID.Empty();

	char *pch = (char *)&AcquireRec[iIndex].bData;
	char RecTypeA = *pch++;
	char RecTypeB = *pch;
	unsigned int usMessageType = RecTypeB + (RecTypeA << 8);

	switch (usMessageType)
	{
	case GPSDATA_REC:				//0x3341
		strcpy(szEvtSuffix,GPS_SUFFIX);
		usNode = ((GPS_Rec*)(&AcquireRec[iIndex].bData))->usNode;
#ifdef RFIDDEMO
		//or
		usNode = ((CEventInstrument*)m_pParent)->m_iNode;
		//or
		usNode = atoi(((CEventInstrument*)m_pParent)->m_pID);
#endif
		cType = "GP";
		break;

	case VACOSSEVENT_REC:            //0x3336
		strcpy(szEvtSuffix,VCS_SUFFIX);
		usNode = ((VacossEvent_Rec*)(&AcquireRec[iIndex].bData))->usNode;
		cType = "VC";
		break;

	case VACOSSINFO_REC:               //0x3335
		strcpy(szEvtSuffix,VCS_SUFFIX);
		usNode = ((VacossInfo_Rec*)(&AcquireRec[iIndex].bData))->usNode;
		cType = "VC";
		break;

	case VACOSSERROR_REC:               //0x3339
		strcpy(szEvtSuffix,VCS_SUFFIX); //18
		usNode = ((VacossError_Rec*)(&AcquireRec[iIndex].bData))->usNode;
		cType = "VC";
		break;

	case BINARYEVENT_REC:   //0x3332:
		strcpy(szEvtSuffix,BNY_SUFFIX);
		usNode = ((BinaryEventRec*)(&AcquireRec[iIndex].bData))->usNode;
		cType = "BN";
		break;

#ifdef RFIDDEMO
	case RADDATA_REC:		//3445
		strcpy(szEvtSuffix,RAD_SUFFIX);
		usNode = ((GRANDAcquire_Rec*)(&AcquireRec[iIndex].bData))->StatusByte;
		((GRANDAcquire_Rec*)(&AcquireRec[iIndex].bData))->StatusByte = 0;
		// or 
		usNode = ((CEventInstrument*)m_pParent)->m_iNode;
		//or
		usNode = atoi(((CEventInstrument*)m_pParent)->m_pID);
		cType = "RD";
		break;

	case RAD2DATA_REC:		//3448
		strcpy(szEvtSuffix,RAD_SUFFIX);
		usNode = ((RABBITAcquire_Rec*)(&AcquireRec[iIndex].bData))->StatusByte;
		((RABBITAcquire_Rec*)(&AcquireRec[iIndex].bData))->StatusByte = 0;
		// or 
		usNode = ((CEventInstrument*)m_pParent)->m_iNode;
		//or
		usNode = atoi(((CEventInstrument*)m_pParent)->m_pID);
		cType = "R2";
		break;

	case RFIDDATA_REC:		//3447
		strcpy(szEvtSuffix,RFI_SUFFIX);
		usNode = ((RAD_Rec*)(&AcquireRec[iIndex].bData))->usNode;
		//or
		usNode = ((CEventInstrument*)m_pParent)->m_iNode;
		//or
		usNode = atoi(((CEventInstrument*)m_pParent)->m_pID);
		cType = "RF";
//		{
//			CString cTemp;
//			for (int i = 0; i < AcquireRec[iIndex].bData.; i++)
//			{
//				cTemp.Format("%02x",AcquireRec[iIndex].bData.bRFIDData[i]);
//				cRFID += cTemp;
//			}
//		}
		break;
#endif

	// Force all others to the standard "EVT" filename
	default:
		strcpy(szEvtSuffix,EVT_SUFFIX);
		cType = "EV";
		usNode = 255;
		break;
	}
#endif

	FILE* File;
	fpos_t position;
	if (m_pParent->m_bBlockWrite) return false;
	struct EventAcquireHeader AcquireHead;
	
	//build filename
	char cYear;
	int iYear = Time.Year();
	if ((iYear < 1990) || (iYear > 2025))
		cYear = '#';
	else if (iYear < 2000)
		cYear = (char)('0' + iYear - 1990);
	else 
		cYear = (char)('A' + iYear - 2000);

	if (m_pParent->m_bUseShortFilename)
	{
#ifdef SINGLE_BINARY_FILE
		sprintf(m_szCurrentEVTFileName,"%s\\%s%c%c%c%c%02d.%3s",
				((CEventInstrument*)m_pParent)->m_szSaveLoc,	
				((CEventInstrument*)m_pParent)->m_pID,
				cYear,
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,EVT_SUFFIX);
#else
#ifdef RFIDDEMO
		if (cType == "RF")
		{
			sprintf(m_szCurrentEVTFileName,"%s\\%02x%c%c%c%c%02d.%3s",
					m_pParent->m_szSaveLoc,	
					usNode,
					cYear,
					((Time.Month()<10)?
						'0'+(Time.Month()):
						'A'+(Time.Month()-10)),
					((Time.Day()  <10)?
						'0'+(Time.Day()):
						'A'+(Time.Day()  -10)),
					'A',0,//cRFID,
					szEvtSuffix);
		}
		else
#endif
		{
			sprintf(m_szCurrentEVTFileName,"%s\\%02x%c%c%c%c%02d.%3s",
					m_pParent->m_szSaveLoc,	
					usNode,
					cYear,
					((Time.Month()<10)?
						'0'+(Time.Month()):
						'A'+(Time.Month()-10)),
					((Time.Day()  <10)?
						'0'+(Time.Day()):
						'A'+(Time.Day()  -10)),
					'A',0,szEvtSuffix);
		}
#endif
	}
	else
	{
		CString cResult;
#ifdef SINGLE_BINARY_FILE
		BuildFileName(cResult,
			m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,"EV",
			m_pParent->m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,"",EVT_SUFFIX);
#else
		CString cTemp;
		cTemp.Format("%03d",usNode);
		BuildFileName(cResult,
			m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,cType,
			cTemp,Time.Year(),Time.Month(),Time.Day(),0,0,0,""/*cRFID*/,szEvtSuffix);
#endif
		strcpy(m_szCurrentEVTFileName,cResult);
	}

	//is the file there
	// '0' checks for existance only
	if (access(m_szCurrentEVTFileName,0) != 0)
	{
		//make sure path exists
		MakeMyPath(m_szCurrentEVTFileName);

		//build archive file name
		char cYear;
		int iYear = Time.Year();
		if ((iYear < 1990) || (iYear > 2025))
			cYear = '#';
		else if (iYear < 2000)
			cYear = (char)('0' + iYear - 1990);
		else 
			cYear = (char)('A' + iYear - 2000);

		CString ArchiveName;
		if (m_pParent->m_bUseShortFilename)
		{
#ifdef SINGLE_BINARY_FILE
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				((CEventInstrument*)m_pParent)->m_szSaveLoc,	
				((CEventInstrument*)m_pParent)->m_pID,
				cYear,
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,EVT_SUFFIX);
#else
#ifdef RFIDDEMO				
			if (cType == "RF")
			{
				ArchiveName.Format("%s\\archive\\%02x%c%c%c%c%02d.%3s",
					((CEventInstrument*)m_pParent)->m_szSaveLoc,	
					usNode,
					cYear,
					((Time.Month()<10)?
						'0'+(Time.Month()):
						'A'+(Time.Month()-10)),
					((Time.Day()  <10)?
						'0'+(Time.Day()):
						'A'+(Time.Day()  -10)),
					'A',0,szEvtSuffix);
			}
			else
#endif
			{
				ArchiveName.Format("%s\\archive\\%02x%c%c%c%c%02d.%3s",
					((CEventInstrument*)m_pParent)->m_szSaveLoc,	
					usNode,
					cYear,
					((Time.Month()<10)?
						'0'+(Time.Month()):
						'A'+(Time.Month()-10)),
					((Time.Day()  <10)?
						'0'+(Time.Day()):
						'A'+(Time.Day()  -10)),
					'A',0,szEvtSuffix);
			}
#endif
		}
		else
		{
#ifdef SINGLE_BINARY_FILE
			CString cTemp;
			cTemp = m_pParent->m_szSaveLoc;
			cTemp += "\\archive\\";
			BuildFileName(ArchiveName,g_cLongName,cTemp,"EV",
				m_pParent->m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,"",EVT_SUFFIX);
#else
			CString cTemp;
			CString cTempNode;
			cTempNode.Format("%03d",usNode);
			cTemp = m_pParent->m_szSaveLoc;
			cTemp += "\\archive\\";
			BuildFileName(ArchiveName,
				m_pParent->m_csLongNameFormatString,cTemp,cType,
				cTempNode,Time.Year(),Time.Month(),Time.Day(),0,0,0,"",szEvtSuffix);
#endif
		}

		//if the file is in the archive directory
		if (access(ArchiveName,0)==0)
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentEVTFileName,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	}

	int Attempts = 5;
	File = NULL;
	while ((Attempts > 0) && (File == NULL))
	{
		File = fopen(m_szCurrentEVTFileName,"ab+");
		if (File == NULL)
		{
			Sleep(200);
		}
		Attempts--;
	}

	if (File != NULL)
	{
		//get file position
		fseek(File,0,SEEK_END);

		if (fgetpos( File, &position )==0)
		{
			//if new file (e.g. no header there yet)
			if (position == 0)
			{
				//initialize header
				char temp[32];
				memset( &AcquireHead, ' ', sizeof(EventAcquireHeader));

				//fill header
				//size of header beyond first four bytes
				sprintf(temp,"%4d",sizeof(EventAcquireHeader)-4);
				memcpy(AcquireHead.SizeOfHeader,temp,4);

				//Fix for new version number formatting inside of binary file
				//not converting to a float for the review software.
				//Incompatible format for binary file came out "2.0.0".
				//Compatible format is " 2000".
				//Prior to 2.0.0.0, the version number was formatted "1.907".
				//This code reformats the newer version format to look like the old.
				//SK uses a float to read the value from the binary file.
				//Fix provided by DP and incorporated by PJM 8/5/2005
				//
				//sprintf(temp,"%5s",m_pParent->m_csVersion);
				short sA,sB,sC,sD;
//try{
				sscanf(m_pParent->m_csVersion,"%d.%d.%d.%d",&sA,&sB,&sC,&sD);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 16",MB_OK);
//	AfxMessageBox(m_pParent->m_csVersion,MB_OK);
//}
				sprintf(temp,"%2d%1d%1d%1d",sA,sB,sC,sD);  //no decimal point
				//End fix of 8/5/2005

				memcpy(AcquireHead.Version,temp,5);
				//sprintf(temp,"%3s",m_szCurrentID);
				sprintf(temp,"%3d",usNode);//per Shirley 16 Jun 2005  dgp

				memcpy(AcquireHead.StationId,temp,3);
				sprintf(temp,"%3d",Time.Year()%100);
				memcpy(AcquireHead.Year,temp,3);
				sprintf(temp,"%3d",Time.Month());
				memcpy(AcquireHead.Month,temp,3);
				sprintf(temp,"%3d",Time.Day());
				memcpy(AcquireHead.Day,temp,3);
				sprintf(temp,"%4d",Time.Year());
				memcpy(AcquireHead.FullYear,temp,4);
//#ifdef RFIDDEMO				
//				if (cType == "RF")
//					strcpy(AcquireHead.unused2+sizeof(AcquireHead.unused2)-strlen(cRFID)-1,cRFID);
//#endif
				//write header
				fwrite(&AcquireHead,sizeof(EventAcquireHeader),1,File);
			}
		}

		//send record to the EVT file
		unsigned short usLength = (unsigned short)( (AcquireRec[iIndex].usSize + sizeof(unsigned short)) );
		size_t maxbytes = usLength; //sizeof(EventAcquireRecord);
		BYTE* pRec = (BYTE*)&AcquireRec[iIndex].usSize;

		int iWriteResult;
		//if ((iWriteResult = fwrite(bRec,usLength,1,File)) != 1)
#ifdef RFIDDEMO
		if (usMessageType == RADDATA_REC)
		{
			pRec += 4;
			maxbytes -= 4;
		}
		else if (usMessageType == RAD2DATA_REC)
		{
			pRec += 4;
			maxbytes -= 4;
		}
		else if (usMessageType == RFIDDATA_REC)
		{
			pRec += 2;
			maxbytes -= 2;
		}
#endif
		if ((iWriteResult = fwrite(pRec,maxbytes,1,File)) != 1)
		{
			//if failed it could be because MicXfer is trying to
			//clear the archive bit, so wait a short time and try
			//again.
			Sleep(100);
			//if ((iWriteResult = fwrite(bRec,usLength,1,File)) != 1)
			if ((iWriteResult = fwrite(pRec,maxbytes,1,File)) != 1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(200);
				//if ((iWriteResult = fwrite(bRec,usLength,1,File)) != 1)
				if ((iWriteResult = fwrite(pRec,maxbytes,1,File)) != 1)
				{
					//if failed on write then show a file write problem
					//on the instrument's button
					m_pParent->m_pButton->SetDiskIcon_Hourglass();
					m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
					m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
					//write the error to the pfm and cev files
					WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "41800 Could not write to binary file");
					//close the file anyway
					fclose(File);
					//save the fact that we couildn't so that we can tell PFM/CEV when good
					m_bCouldNotOpenBinaryFile = true;
					//return write failure 
					return false;
				}
			}
		}
		if ((iWriteResult == 1) && m_bCouldNotOpenBinaryFile)
		{
			m_bCouldNotOpenBinaryFile = false;
			WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "41810 Successful write to binary file");
		}
		//close the EVT file
		fclose(File);
		//return good write
		return true;
	}
	else
	{
		//save the fact that we couildn't so that we can tell PFM/CEV when good
		m_bCouldNotOpenBinaryFile = true;
		//couldn't open the file so...
		//show a file write problem
		//on the instrument's button
		m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
		m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
		WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "41820 Could not open binary file");
	}
	return false;
}

void
CEventPropertySheet::WriteToFile(int ToWhom, int Type, CJulianTime* Time, char* Msg)
{
	FILE *File;
	char Message[256];
	fpos_t position;
	Message[0] = NULL;
	COleDateTime TC = COleDateTime::GetCurrentTime();
	COleDateTime TG;
	COleDateTimeSpan TS;
	CString ArchiveFileName;

	if (m_pParent->m_bBlockWrite) 
		return;

	//default to the provided Event time
	if (Time)
	{
		int Year,Month,Day,Hour,Minute,Second;
		Year = Time->Year();
		Month = Time->Month();
		Day = Time->Day();
		Hour = Time->Hour();
		Minute = Time->Minute();
		Second = Time->Second();
		if (Year	< 100)  Year	= 1900;//COleDateTime limits year 100-9999
		if (Year    > 9999) Year    = 9999;//COleDateTime limits year 100-9999
		if (Month	< 1)	Month	= 1;
		if (Day		< 1)	Day		= 1;
		if (Hour	< 0)	Hour	= 0;
		if (Minute	< 0)	Minute	= 0;
		if (Second	< 0)	Second	= 0;
		TG = COleDateTime(Year, Month, Day, Hour, Minute, Second);
	}
	//if that wasn't available then get the computer time
	//this is the case in all TYPE_COMP messages
	else
	{
		TG = COleDateTime::GetCurrentTime();
	}

	if (ToWhom == TO_DMP)
	{
		//	build new file name and save it
		if (m_pParent->m_bUseShortFilename)
		{
			char cYear;
			int iYear = TC.GetYear();
			if ((iYear < 1990) || (iYear > 2025))
				cYear = '#';
			else if (iYear < 2000)
				cYear = (char)('0' + iYear - 1990);
			else 
				cYear = (char)('A' + iYear - 2000);

			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
	//			((TC.GetYear()-1990)<10)?
	//				((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
	//				'A'+(TC.GetYear()-2000),
				cYear,
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,"EV",
				m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,"",".");
			strcpy(m_szCurrentFileName,cResult);
		}
	}
	else
	{
		if (m_pParent->m_bUseShortFilename)
		{
			//	build new file name and save it
			char cYear;
			int iYear = TG.GetYear();
			if ((iYear < 1990) || (iYear > 2025))
				cYear = '#';
			else if (iYear < 2000)
				cYear = (char)('0' + iYear - 1990);
			else 
				cYear = (char)('A' + iYear - 2000);

			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				((CEventInstrument*)m_pParent)->m_szSaveLoc,	
				((CEventInstrument*)m_pParent)->m_pID,
				cYear,
				((TG.GetMonth()<10)?
					'0'+(TG.GetMonth()):
					'A'+(TG.GetMonth()-10)),
				((TG.GetDay()  <10)?
					'0'+(TG.GetDay()):
					'A'+(TG.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,"EV",
				m_pParent->m_pID,TG.GetYear(),TG.GetMonth(),TG.GetDay(),0,0,0,"",".");
			strcpy(m_szCurrentFileName,cResult);
		}

	}	

	//format string to send
	switch (Type){

	case TYPE_DUMP:
		sprintf(Message,"%4d.%02d.%02d %02d:%02d:%02d %s\n",
			TC.GetYear(),TC.GetMonth(),TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),
			Msg);
//		strcpy(Message,Msg);
		break;

	case TYPE_INST:
		//	message = Time::YY.MM.DD HH:MM:SS G (MESSAGE==NULL)?\r:MESSAGE

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d E %s\n",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"":Msg);
		break;

	case TYPE_COMP:
		//	message = MICTIME::YY.MM.DD HH:MM:SS C (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C %s\n",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),Msg[0]==NULL?"":Msg);
		break;

	case TYPE_GID2:
	case TYPE_TIME:
		//	message = Time::YY.MM.DD HH:MM:SS Event Time   "+
		//					"YY.MM.DD HH:MM:SS Computer Time   C - E = xxxx\r"
		//computer time
		//Event time
		TS = TC - TG;  // Subtract 2 COleDateTimes
		TS += HALF_SECOND;
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d E 40002 Event Time %04d.%02d.%02d %02d:%02d:%02d Computer Time   C - E = %.0f seconds\n",
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			TS.GetTotalSeconds());
		break;

	case TYPE_INVTIME:
		//	message = "INVALID TIME  "+
		//		"Previous Record Time Saved::YY.MM.DD HH:MM:SS "+
		//		"Current Record Time Time::YY.MM.DD HH:MM:SS\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 40001 EVENT INVALID TIME %04d.%02d.%02d %02d:%02d:%02d\n",
			((CJulianTime*)Msg)->Year(),	((CJulianTime*)Msg)->Month(),	((CJulianTime*)Msg)->Day(),
			((CJulianTime*)Msg)->Hour(),		((CJulianTime*)Msg)->Minute(),	((CJulianTime*)Msg)->Second(),
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond());
		break;

	case TYPE_START:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C EVENT COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 40000 EVENT COLLECT Version %s started\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion);
		break;

	case TYPE_ABNORMAL:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C EVENT COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 40010 EVENT COLLECT Version %s started from abnormal shutdown\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion);
		break;

	case TYPE_DELETE:
	//	message = MICNOW::YY.MM.DD HH:MM:SS C file %s<MESSAGE> deleted\r"

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 40011 EVENT File %s deleted\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
		break;

	//just in case
	default:
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 40012 EVENT Unknown TYPE %s\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
	}

	//if to dmp do the write to todays file and get out
	if (ToWhom == TO_DMP)
	{
		//	open filename+dmp
		CString fn(m_szCurrentFileName);
		fn += DMP_SUFFIX;
//		CString ArchiveFileNameEx = ArchiveFileName + DMP_SUFFIX;
		if (access(fn,0) != 0)
		{
//			if (access(ArchiveFileNameEx,0)!=-1)
//				MoveFileEx(ArchiveFileNameEx,fn,
//					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			//make sure path exists
			MakeMyPath(fn);
		}

		if ((File = fopen(fn,"at+")) != NULL)
		{
			//	send string
			fprintf(File,Message);
			//	close cev
			fclose(File);
		}
		return;
	}

	//if to cev
	if (ToWhom & TO_CEV)
	{
		//	open filename+cev
		CString fn(m_szCurrentFileName);
		fn += CEV_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);

			if (m_pParent->m_bUseShortFilename)
			{
				//build archive path\name
				char cYear;
				int iYear = TC.GetYear();
				if ((iYear < 1990) || (iYear > 2025))
					cYear = '#';
				else if (iYear < 2000)
					cYear = (char)('0' + iYear - 1990);
				else 
					cYear = (char)('A' + iYear - 2000);

				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				((CEventInstrument*)m_pParent)->m_szSaveLoc,
				((CEventInstrument*)m_pParent)->m_pID,
				cYear,
	//			((TC.GetYear()-1990)<10)?
	//				((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
	//				'A'+(TC.GetYear()-2000),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,CEV_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = m_pParent->m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,
					m_pParent->m_csLongNameFormatString,
					cTemp,"EV",
					m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,"",CEV_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"at+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos(File,&position);
			if (position==0)
			{
				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
				m_bBatteryTolOk	= true;
//				m_bP15VoltOk	= true;
//				m_bM15VoltOk	= true;
//				m_bACPowerOk	= true;
//				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
				m_bCountRateTamperOk = true;
				m_bRMSSigmaTamperOk  = true;
				m_bInMII		= false;
				m_bDataFiltered	= false;
				((CEventInstrument*)m_pParent)->m_dBreakSentCount = 0;
				((CEventInstrument*)m_pParent)->m_dTimeOutCount = 0;
				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}

			//	send string
			fprintf(File,Message);
			//	close cev
			fclose(File);
		}
	}

	//if to pfm
	if (ToWhom & TO_PFM)
	{
		//	open filename+pfm
		CString fn(m_szCurrentFileName);
		fn += PFM_SUFFIX;

//		CString ArchiveFileNameExt = ArchiveFileName + PFM_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);

			//check if it is in the archive directory
			//build file name
			if (m_pParent->m_bUseShortFilename)
			{
				char cYear;
				int iYear = TC.GetYear();
				if ((iYear < 1990) || (iYear > 2025))
					cYear = '#';
				else if (iYear < 2000)
					cYear = (char)('0' + iYear - 1990);
				else 
					cYear = (char)('A' + iYear - 2000);

				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_pParent->m_szSaveLoc,
				m_pParent->m_pID,
				cYear,
	//			((TC.GetYear()-1990)<10)?
	//				((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
	//				'A'+(TC.GetYear()-2000),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,PFM_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = ((CEventInstrument*)m_pParent)->m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,
					m_pParent->m_csLongNameFormatString,
					cTemp,"EV",
					m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,"",PFM_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"at+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos( File, &position );
			if (position==0)
			{
				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
				m_bBatteryTolOk	= true;
//				m_bP15VoltOk	= true;
//				m_bM15VoltOk	= true;
//				m_bACPowerOk	= true;
//				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
				m_bCountRateTamperOk = true;
				m_bRMSSigmaTamperOk  = true;
				m_bInMII		= false;
				m_bDataFiltered	= false;
				((CEventInstrument*)m_pParent)->m_dBreakSentCount = 0;
				((CEventInstrument*)m_pParent)->m_dTimeOutCount = 0;
				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}
			//	send string
			fprintf(File,Message);
			//	close pfm
			fclose(File);
		}
	}

}

int CEventPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pCloseButton = new CColorButton();
	m_pCloseButton->SetBase(10);
	m_pCloseButton->SetPreNum("BBM: ");

	m_pPauseButton		= new CButton();
	m_pAckFlagsButton	= new CButton();
	m_pPrintButton		= new CButton();
	m_pToClose			= new CButton();
	m_pImageList		= new CImageList();
	m_pImageList->Create( IDB_BMP_ALL, 18, 1, COLOR_GRAY);
	return 0;
}

void CEventPropertySheet::OnCloseButton()
{
	ShowWindow(SW_HIDE);
	KillTimer(1);
}

void CEventPropertySheet::SetTabColor(int Tab,COLORREF Color)
{
	if ((Tab != IDC_EVENTDATA_TAB) && 
		(Tab != IDC_BINARYEVENTDATA_TAB) &&
		(Tab != IDC_VACOSSDATA_TAB) &&
		(Tab != IDC_GPSDATA_TAB)) return;
	int ColorIndex;
	int tab = 0;
	switch(Color) {
	case COLOR_RED:
		ColorIndex = 0;
		break;
	case COLOR_GREEN:
		ColorIndex = 1;
		break;
	case COLOR_BLUE:
		ColorIndex = 2;
		break;
	case COLOR_YELLOW:
		ColorIndex = 3;
		break;
	case COLOR_CYAN:
		ColorIndex = 4;
		break;
	case COLOR_MAGENTA:
		ColorIndex = 5;
		break;
	case COLOR_WHITE:
		ColorIndex = 6;
		break;
	case COLOR_BLACK:
		ColorIndex = 7;
		break;
	default:
		ColorIndex = 6;
	};
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = ColorIndex;
	switch (Tab) {
	case IDC_EVENTDATA_TAB:			tab = 1; break;
	case IDC_BINARYEVENTDATA_TAB:   tab = 2; break;
//	case IDC_UGRANDEVENTDATA_TAB:	tab = 3; break;
	case IDC_GPSDATA_TAB:			tab = 3; break;
	case IDC_VACOSSDATA_TAB:		tab = 4; break;
	};
	GetTabControl()->SetItem(tab,&pTabCtrlItem);
	GetTabControl()->Invalidate();
}


void CEventPropertySheet::OnAckButton()
{
	m_dToClose = 0;

	//acknowledge the four button flags
	m_pParent->m_pButton->ShowIcon_Bell(VARIANT_FALSE);
	m_pParent->FlagComm(0,false);
	m_pParent->FlagOthr(0,false);
	m_pParent->FlagTime(0,false);

	m_pParent->m_pButton->ShowIcon_Disk(VARIANT_FALSE);
	m_pParent->m_pButton->put_Flash(VARIANT_FALSE);

	CEventSummary*  summarypage		= m_pParent->m_pSummaryPage;
	CEventDataStat* datapage		= m_pParent->m_pDataStatus;
	CBinaryDataStat* bindatapage	= m_pParent->m_pBinaryDataStatus;
	CGPSDataStat* gpsdatapage		= m_pParent->m_pGPSDataStatus;
	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;
//	CEventModParam* modparpage		= m_pParent->m_pModParams;

	summarypage->ResetColors();
	datapage->ResetColors();
	bindatapage->ResetColors();
	gpsdatapage->ResetColors();
	vacossdatastat->ResetColors();
	
	SetTabColor(IDC_EVENTDATA_TAB,COLOR_GREEN);
	SetTabColor(IDC_BINARYEVENTDATA_TAB,COLOR_GREEN);
	SetTabColor(IDC_GPSDATA_TAB,COLOR_GREEN);
	SetTabColor(IDC_VACOSSDATA_TAB,COLOR_GREEN);

	Data_Tab = 0x0000;
	General_Tab = 0x0000;
	UserProg_Tab = 0x0000;

	Invalidate();
}

void CEventPropertySheet::OnPauseButton()
{
	m_dToClose = 0;

	CEventModParam* modparpage = m_pParent->m_pModParams;

	if (m_pParent->TogglePause())
	{
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
		modparpage->GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->EnableWindow(TRUE);
	}
	else
	{
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
		modparpage->GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->EnableWindow(FALSE);
	}
}

void CEventPropertySheet::IssuePrint(FILE* outfile)
{

	CString temp, temp1, temp2, temp3, temp4;

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");

	CEventSummary* summarypage		= m_pParent->m_pSummaryPage;
	CEventDataStat*	datapage		= m_pParent->m_pDataStatus;
	CBinaryDataStat* bindatapage	= m_pParent->m_pBinaryDataStatus;
	CGPSDataStat* gpsdatapage		= m_pParent->m_pGPSDataStatus;
	CVACOSSDataStat* vacossdatastat = m_pParent->m_pVACOSSDataStatus;
	CEventModParam* modparpage		= m_pParent->m_pModParams;


	fprintf(outfile,"* = Displayed in RED\n");
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"SUMMARY\n");

	fprintf(outfile," Data Status\n");
	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_OOMII,temp);
	temp += (summarypage->StaticColors[IDC_EVENTSUMMARY_OOMII-IDC_EVENTSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
	fprintf(outfile,"   Measurement Interval of Interest: %s\n",LPCTSTR(temp));

	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_ACPOWER,temp);
	temp += (summarypage->StaticColors[IDC_EVENTSUMMARY_ACPOWER-IDC_EVENTSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
	fprintf(outfile,"                     External Power: %s\n",LPCTSTR(temp));

	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_BATTERYOK,temp);
	temp += (summarypage->StaticColors[IDC_EVENTSUMMARY_BATTERYOK-IDC_EVENTSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
	fprintf(outfile,"                            Battery: %s\n",LPCTSTR(temp));

	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_BBMOK,temp);
	temp += (summarypage->StaticColors[IDC_EVENTSUMMARY_BBMOK-IDC_EVENTSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
	fprintf(outfile,"           Battery Backed Up Memory: %s\n",LPCTSTR(temp));

	summarypage->GetDlgItemText(IDC_EVENTSUMMARY_INQUIRETIME,temp);
	temp += (summarypage->StaticColors[IDC_EVENTSUMMARY_INQUIRETIME-IDC_EVENTSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
	fprintf(outfile,"  Last INQUIRE Status (Y.M.D H:M:S):\n  %s\n",LPCTSTR(temp));

	fprintf(outfile,"  Recent Commands and Responses:\n");

	for (int i = IDC_EVENTSUMMARY_COMMAND11; i <= IDC_EVENTSUMMARY_COMMAND1; i--)
	{
		summarypage->GetDlgItemText(i,temp);
		temp += (summarypage->StaticColors[i-IDC_EVENTSUMMARY_FIRST]==COLOR_RED)?REDFLAG:" ";
		fprintf(outfile,"  %s\n",LPCTSTR(temp));
		temp.Empty();
	}

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"DATA STATUS\n");
	fprintf(outfile,"Most Recent INQUIRE2 Record\n");

	datapage->GetDlgItemText(IDC_EVENTDATA_UNITID,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_UNITID-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"                           Main Unit ID: %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_INQUIRETIME,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_INQUIRETIME-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"     Last INQUIRE2 Record (Y.M.D H:M:S): %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_BYTESMEM,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_BYTESMEM-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"            Bytes of Data in BBM Memory: %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_BBMOK,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_BBMOK-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"               Battery Backed Up Memory: %s\n",temp);

	fprintf(outfile,"Most Recent STATUS Record\n");

	datapage->GetDlgItemText(IDC_EVENTDATA_STATUSTIME,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_STATUSTIME-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"       Last STATUS Record (Y.M.D H:M:S): %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_OPERATION2,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_OPERATION2-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"                         Status Byte #1: %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_OPERATION3,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_OPERATION3-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"                         Status Byte #2: %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_OPERATION4,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_OPERATION4-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"                         Status Byte #3: %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_OPERATION5,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_OPERATION5-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"                         Status Byte #4: %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_OPERATION6,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_OPERATION6-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"                         Status Byte #5: %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_OPERATION7,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_OPERATION7-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"                         Status Byte #6: %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_OPERATION8,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_OPERATION8-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"                         Status Byte #7: %s\n",temp);

	datapage->GetDlgItemText(IDC_EVENTDATA_OPERATION9,temp);
	temp += (datapage->StaticColors[IDC_EVENTDATA_OPERATION9-IDC_EVENTDATA_FIRST]!=COLOR_DEFAULT_TEXT)?REDFLAG:" ";
	fprintf(outfile,"                         Status Byte #8: %s\n",temp);

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"GPS DATA STATUS\n");
	fprintf(outfile,"Most Recent GPS Records\n");

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE1,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE2,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE3,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATADATE4,temp3);
	fprintf(outfile," Date of Record (Y.M.D): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME1,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME2,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME3,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_DATATIME4,temp3);
	fprintf(outfile," Time of Record (H:M:S): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT1,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT2,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT3,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEACNT4,temp3);
	fprintf(outfile,"            Node Number: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT1,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT2,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT3,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSEBCNT4,temp3);
	fprintf(outfile,"            Latitude(*): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT1,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT2,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT3,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_PULSECCNT4,temp3);
	fprintf(outfile,"           Longitude(*): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR1,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR2,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR3,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1GR4,temp3);
	fprintf(outfile,"           Altitude (M): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S1,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S2,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S3,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S4,temp3);
	fprintf(outfile,"               Fix Date: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S5,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S6,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S7,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S8,temp3);
	fprintf(outfile,"               Fix Time: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES1,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES2,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES3,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_SATELITES4,temp3);
	fprintf(outfile,"             Satellites: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S13,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S14,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S15,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S16,temp3);
	fprintf(outfile,"     East Velocity(M/S): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S17,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S18,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S19,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S20,temp3);
	fprintf(outfile,"     East Velocity(M/S): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S21,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S22,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S23,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S24,temp3);
	fprintf(outfile,"       Up Velocity(M/S): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S25,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S26,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S27,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S28,temp3);
	fprintf(outfile," Clock Bias Rate(M/s^2): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S29,temp);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S30,temp1);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S31,temp2);
	gpsdatapage->GetDlgItemText(IDC_GPSDATA_GAMMA1S32,temp3);
	fprintf(outfile," Fix Time (sec in Week): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"BINARY DATA\n");
	fprintf(outfile,"Most Recent Binary Records\n");
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATADATE1,temp);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATADATE2,temp1);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATADATE3,temp2);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATADATE4,temp3);
	fprintf(outfile," Date of Record (Y.M.D): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATATIME1,temp);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATATIME2,temp1);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATATIME3,temp2);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_DATATIME4,temp3);
	fprintf(outfile," Time of Record (H:M:S): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT1,temp);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT2,temp1);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT3,temp2);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEACNT4,temp3);
	fprintf(outfile,"                 Status: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT1,temp);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT2,temp1);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT3,temp2);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSEBCNT4,temp3);
	fprintf(outfile,"            Node Number: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT1,temp);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT2,temp1);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT3,temp2);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_PULSECCNT4,temp3);
	fprintf(outfile,"                  State: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR1,temp);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR2,temp1);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR3,temp2);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1GR4,temp3);
	fprintf(outfile,"                   Mask: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S1,temp);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S2,temp1);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S3,temp2);
	bindatapage->GetDlgItemText(IDC_BINARYEVENTDATA_GAMMA1S4,temp3);
	fprintf(outfile,"               Reserved: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);

	fprintf(outfile,"VACOSS DATA\n");
	fprintf(outfile,"Most Recent VACOSS Records\n");
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_DATADATE1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_DATADATE2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_DATADATE3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_DATADATE4,temp3);
	fprintf(outfile,"     Date of Record (Y.M.D): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_DATADATE1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_DATADATE2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_DATADATE3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_DATADATE4,temp3);
	fprintf(outfile,"     Time of Record (H:M:S): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_NODE1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_NODE2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_NODE3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_NODE4,temp3);
	fprintf(outfile,"                Node Number: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_SEALID1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_SEALID2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_SEALID3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_SEALID4,temp3);
	fprintf(outfile,"                    Seal ID: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_TYPE1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_TYPE2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_TYPE3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_TYPE4,temp3);
	fprintf(outfile,"              Response Type: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_ERRDATE1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_ERRDATE2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_ERRDATE3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_ERRDATE4,temp3);
	fprintf(outfile,"     Error Code \\ Init Date: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_RESTIME1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_RESTIME2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_RESTIME3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_RESTIME4,temp3);
	fprintf(outfile,"       Reserved \\ Init Time: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT01_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT01_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT01_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT01_4,temp3);
	fprintf(outfile,"Init to Event 1(min)\\# Atch: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT02_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT02_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT02_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT02_4,temp3);
	fprintf(outfile,"Init to Event 2(min)\\T init: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT03_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT03_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT03_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT03_4,temp3);
	fprintf(outfile,"Init to Event 3(min)\\Status: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT04_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT04_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT04_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT04_4,temp3);
	fprintf(outfile," Init to Event 4(min)\\#Evnt: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT05_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT05_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT05_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT05_4,temp3);
	fprintf(outfile,"       Init to Event 5(min): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT06_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT06_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT06_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT06_4,temp3);
	fprintf(outfile,"       Init to Event 6(min): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT07_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT07_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT07_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT07_4,temp3);
	fprintf(outfile,"       Init to Event 7(min): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT08_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT08_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT08_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT08_4,temp3);
	fprintf(outfile,"       Init to Event 8(min): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT09_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT09_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT09_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT09_4,temp3);
	fprintf(outfile,"       Init to Event 9(min): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT10_1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT10_2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT10_3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_EVENT10_4,temp3);
	fprintf(outfile,"      Init to Event 10(min): %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_LRC1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_LRC2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_LRC3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_LRC4,temp3);
	fprintf(outfile,"                        LRC: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3);

	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_SIG1,temp);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_SIG2,temp1);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_SIG3,temp2);
	vacossdatastat->GetDlgItemText(IDC_VACOSSDATA_SIG4,temp3);
	fprintf(outfile,"         Signature 1st half: %10s %10s %10s %10s\n",
		temp.Left(8),temp1.Left(8),temp2.Left(8),temp3.Left(8));
	fprintf(outfile,"         Signature 2nd half: %10s %10s %10s %10s\n",
		temp.Right(8),temp1.Right(8),temp2.Right(8),temp3.Right(8));
		
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"MODIFY PARAMETERS -- Caution: These values may not have been applied!\n");
	fprintf(outfile," Communications\n");
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_PORT,temp);
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_NODE,temp1);
	fprintf(outfile,"  Port: %s\n  Node: %s\n\n",temp,temp1);
	fprintf(outfile," File Output\n");
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_SAVELOC,temp);
	fprintf(outfile,"  Location: %s\n",temp);
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_FILEID,temp);
	fprintf(outfile,"  Station ID: %s\n",temp);
	if(((CButton*)modparpage->GetDlgItem(IDC_EVENTMODPARAMS_DODUMP))->GetCheck()==0)
		temp = NO;
	else
		temp = YES;
	fprintf(outfile,"  Do Dump File: %s\n",temp);
	fprintf(outfile," Error Limits\n");
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_SLOW_RESET,temp);
	fprintf(outfile,"  Slow Reset (Sec.): %6s\n\n",temp);
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_TIME_ERR,temp);
	fprintf(outfile,"  Time Delta (Sec.): %6s\n\n",temp);
	fprintf(outfile," General\n");
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_MAXCYCLE,temp);
	fprintf(outfile,"     Message Cycle Time (mSec): %s\n",temp);
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_MAXPAUSE,temp);
	fprintf(outfile,"      Maximum Pause Time (Sec): %s\n",temp);
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_MAXBBM,temp);
	fprintf(outfile,"           Maximum BBM (Bytes): %s\n",temp);

	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_COMMFAIL,temp);
	fprintf(outfile,"  # of Retransmits Before Fail: %s\n",temp);

	if(((CButton*)modparpage->GetDlgItem(IDC_CHECK_ALLOWAUTODATE))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"    Allow Auto Date & Time Set: %s\n",temp);
	
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_MAXINQUIRE,temp);
	fprintf(outfile,"          Inquire Every(units): %s\n",temp);
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_TIMEOUT,temp);
	fprintf(outfile,"     Response Time-Out (units): %s\n",temp);
	modparpage->GetDlgItemText(IDC_EVENTMODPARAMS_MAXSTATUS,temp);
	fprintf(outfile,"     Take Status Every (units): %s\n",temp);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"CURRENT CONFIGURATION (from INI file)\n");
	char buff[32768];
	GetPrivateProfileSection(m_pParent->m_pName,buff,sizeof(buff),m_pParent->m_szIniFile);
	char* pAt = buff;
	while (strlen(pAt))
	{
		fprintf(outfile,"\t%s\n",pAt);
		pAt = pAt + strlen(pAt) + 1;
	}

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
}

void CEventPropertySheet::OnPrintButton()
{
	FILE* outfile;
	char *at;
	while ((at = strchr(m_pCaption,':'))!=NULL) at[0] = '_';

	m_dToClose = 0;

	CFileDialog FileDialog(FALSE,"txt",m_pCaption,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",this);

	if (FileDialog.DoModal()!=IDCANCEL)
	{
		CString filename = FileDialog.GetPathName();
		outfile = fopen(filename,"w");

		m_pParent->IssuePrint(outfile);

		fclose(outfile);
	}
}

void CEventPropertySheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 	CPropertySheet::OnShowWindow(bShow, nStatus);

	m_dToClose = 0;

	if (bShow && nStatus==0) 
	{
		SetActivePage(0);
		m_dToClose = 0;
		SetTimer(1,1000,NULL);
		CEventModParam* modparpage = m_pParent->m_pModParams;

		if (m_pParent->IsPaused())
			modparpage->GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->EnableWindow(true);
		else
			modparpage->GetDlgItem(IDC_EVENTMODPARAMS_DATETIME)->EnableWindow(false);
	}
	else
		KillTimer(1);
}

void CEventPropertySheet::OnToCloseButton()
{
	char temp[32];
	m_dToClose -= BUMP_EXTEND;  //Bump_extend = 300 seconds = 5 more minutes

    int seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;

	if (seconds_to_go > MAX_EXTEND) //MAX_EXTEND = 7200 = 120 minutes
	{
		m_dToClose   = (m_pParent->m_iDlgCloseMilliseconds/1000) - MAX_EXTEND;
		seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;
	}

	int minutes = seconds_to_go / 60;
	int seconds = seconds_to_go % 60;

	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	m_pToClose->SetWindowText(temp);
}

void CEventPropertySheet::OnExtendClose(WPARAM /*WParam*/, LPARAM /*LParam*/)
{
	m_dToClose = 0;
}

void CEventPropertySheet::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		if (m_dToClose >= m_pParent->m_iDlgCloseMilliseconds/1000)
		{
			m_dToClose = 0;
			ShowWindow(SW_HIDE);
			KillTimer(1);
		} 
		else
		{
			char temp[32];
			int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
			int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;
			sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
			m_pToClose->SetWindowText(temp);
			m_dToClose++;
		}
	}
	CPropertySheet::OnTimer(nIDEvent);
}

void CEventPropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//watch for alt-f4 to try to close us -- don't let it
	if (nID == SC_CLOSE)
		ShowWindow(SW_HIDE);
	else
  		CPropertySheet::OnSysCommand(nID, lParam);
}

BOOL CEventPropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	CRect WindowRect;
	CRect FinalRect;
	GetWindowRect(&WindowRect);
	WindowRect.bottom += 60;
	MoveWindow(&WindowRect);
	int Interspace = 6;

	int ButtonWidth = (WindowRect.right - WindowRect.left)/5 - 6*Interspace;
	if (ButtonWidth < 100) 
		ButtonWidth = 116;
	else
		ButtonWidth = 157;

	FinalRect = WindowRect;
	ScreenToClient(FinalRect);
	int Y1 = FinalRect.bottom - 60;
	int Y2 = FinalRect.bottom - 10;

	DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE |
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER;

	m_pCloseButton->Create( "Close", dwStyle,
		  CRect(Interspace, Y1,
				Interspace + ButtonWidth,Y2),
		  this,
		  ID_CLOSE_BUTTON);

	m_pPauseButton->Create(COLLECTINGBEGIN, dwStyle,
		  CRect(2*Interspace + ButtonWidth,Y1,
				2*Interspace + 2*ButtonWidth,Y2),
		  this,
		  ID_PAUSE_BUTTON);

	m_pAckFlagsButton->Create("Acknowledge Flags",dwStyle,
		  CRect(3*Interspace + 2*ButtonWidth,Y1,
				3*Interspace + 3*ButtonWidth,Y2),
		  this,
		  ID_ACKFL_BUTTON);

	m_pPrintButton->Create("Snapshot Status\nTo File",dwStyle,
		  CRect(4*Interspace + 3*ButtonWidth,Y1,
				4*Interspace + 4*ButtonWidth,Y2),
		  this,
		  ID_PRINT_BUTTON);

	char temp[32];
	int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
	int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;
	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);

	m_pToClose->Create(temp,dwStyle,
		  CRect(5*Interspace + 4*ButtonWidth,Y1,
				5*Interspace + 5*ButtonWidth,Y2),
		this,
		ID_TOCLOSE);

	CMenu* pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		pSysMenu->DeleteMenu( SC_SIZE, MF_BYCOMMAND);
		pSysMenu->DeleteMenu( SC_MAXIMIZE, MF_BYCOMMAND);
	}
	
	GetTabControl( )->SetImageList(m_pImageList);
	GetTabControl( )->ModifyStyle(TCS_MULTILINE,TCS_SINGLELINE);
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(1,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(2,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(3,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(4,&pTabCtrlItem);

	return bResult;
}

BOOL CEventPropertySheet::DestroyWindow() 
{
	GetTabControl( )->SetImageList(NULL);
	return CPropertySheet::DestroyWindow();
}

void CEventPropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)
		m_dToClose = set;
}

void CEventPropertySheet::ForceDateTime(void)
{
	m_dToClose = 0;
	m_pParent->PostMessage(uIDC_RECEIVE,FORCEDATETIME,CHECKSUM_GOOD);
}

CString *CEventPropertySheet::GetVersionString()
{
	return &m_pParent->m_csVersion;
}