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
// MCASafePS.cpp

#include "stdafx.h"
#include "ISO_definitions.h"
#include "colors.h"
#include "ccbutton.h"
#include "JulianT.h"
#include "MCASafePS.h"
#include "MCAInst.h"
#include "INI_definitions.h"
#include "gamnew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CMCAPropertySheet

IMPLEMENT_DYNAMIC(CMCAPropertySheet, CPropertySheet)

CMCAPropertySheet::CMCAPropertySheet(
	UINT nIDCaption, 
	CMCAInst* pParentWnd)
	:CPropertySheet(nIDCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	m_pCaption[0] = NULL;
	FinishConstruction();
}

CMCAPropertySheet::CMCAPropertySheet(
	LPCTSTR pszCaption, 
	CMCAInst* pParentWnd)
	:CPropertySheet(pszCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	strncpy(m_pCaption,pszCaption,sizeof(m_pCaption));
	FinishConstruction();
}

void CMCAPropertySheet::FinishConstruction()
{
	uIDC_RECEIVE = m_pParent->m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_pParent->m_uIDC_RECEIVE_OOB;

	General_Tab		= 0;
	Data_Tab		= 0;
	UserProg_Tab	= 0;

	m_fTNSRHigh		= 0.0;
	m_fTNSRLow		= 0.0;
	m_dTNSRFirst	= true;

	m_fDIRHigh		= 0.0;
	m_fDIRLow		= 0.0;
	m_dDIRFirst		= true;

	m_bBatteryOk	= true;
//	m_bBatteryTolOk	= true;
	m_bP5VoltOk		= true;
	m_bP15VoltOk	= true;
	m_bM15VoltOk	= true;
	m_bACPowerOk	= true;
	m_bACPowerLoss	= false;
	m_bBBMOk		= true;

	m_bFiltering    = false;
	m_bSpectChange  = false;
	m_bRestartReq	= false;
	m_bBBMError		= false;
	m_bMonError		= false;
	m_bAlarmU235	= false;
	m_bAlarmU238	= false;
	m_bAlarmCs137	= false;
	m_bAlarmScaler	= false;
	m_bAlarmGrossCnt= false;
	m_bAlarmRatio1	= false;
	m_bAlarmRatio2	= false;

	m_bGDCSOHError	= false;
	m_bGDCSOHEnable = false;
	m_bGDCSOHOnHold = false;
	m_bHVCSOHError	= false;
	m_bHVCSOHEnable = false;
	m_bHVCSOHOnHold = false;

	m_bInMIIU235	= false;
	m_bInMIIU238	= false;
	m_bInMIICs137	= false;
	m_bInMIIScaler	= false;
	m_bInMIIGrossCounts = false;
	m_bInMIIRatio1	= false;
	m_bInMIIRatio2	= false;

	m_bNewFileName	= false;
	m_bDataFiltered	= false;
	m_bTimeError	= false;

	m_fLowBV		= 0.0;
	m_fCurBV		= 0.0;
	m_fHighBV		= 0.0;
	m_fLowP15		= 0.0;
	m_fCurP15		= 0.0;
	m_fHighP15		= 0.0;
	m_fLowM15		= 0.0;
	m_fCurM15		= 0.0;
	m_fHighM15		= 0.0;
	m_fLowP5		= 0.0;
	m_fCurP5		= 0.0;
	m_fHighP5		= 0.0;
	m_bFirstBV		= true;
	m_bFirstP15		= true;
	m_bFirstM15		= true;
	m_bFirstP5		= true;
	
	m_dToClose		= 0;

	m_bStartOfFile	= true;
	m_dCWStart		= -1;//neither COLD START (0) nor WARM START (1)

	m_szCurrentFileName[0] = NULL;
	m_szPreviousGTime[0] = NULL;
	m_dCurrentDay = -1;

	m_dAt			= 0;
	m_dAcquireRecordCount = 0;
	m_bCouldNotOpenBinaryFile = false;
	m_bReceivingSpectra = false;

	m_pCloseButton	= NULL;
	m_pPauseButton	= NULL;
	m_pPrintButton	= NULL;
	m_pAckFlagsButton	= NULL;
	m_pToClose	= NULL;
	m_pImageList	= NULL;
}

CMCAPropertySheet::~CMCAPropertySheet()
{
	delete m_pCloseButton;
	delete m_pPauseButton;
	delete m_pPrintButton;
	delete m_pAckFlagsButton;
	delete m_pToClose;
	delete m_pImageList;
}


COLORREF CMCAPropertySheet::SetButtonColor(COLORREF color)
{
	if (color == COLOR_PAUSE)
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
	else
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
	return m_pCloseButton->SetFaceColor(color);
}

void CMCAPropertySheet::SetLimits(
	float batvolth, float batvoltl,
	float sup_p15h, float sup_p15l,
	float sup_m15h, float sup_m15l,
	float sup_p05h, float sup_p05l,
	int time_err,	char* Id)
{	
	m_fBatVoltH=batvolth;
	m_fBatVoltL=batvoltl;
	m_fSup_P15H=sup_p15h;
	m_fSup_P15L=sup_p15l;
	m_fSup_M15H=sup_m15h;
	m_fSup_M15L=sup_m15l;
	m_fSup_P05H=sup_p05h;
	m_fSup_P05L=sup_p05l;
	m_dTime_Err=(long)time_err;
	strcpy(m_szCurrentID,Id);
}

BEGIN_MESSAGE_MAP(CMCAPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMCAPropertySheet)
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
	ON_MESSAGE(IDC_EXTENDCLOSE,		OnExtendClose)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE_OOB, OnReceiveOOB)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE, OnReceive)
END_MESSAGE_MAP()


void CMCAPropertySheet::UpdateSummary(char *Message)
{
	CString Data;
//	CMCASummary*  SummaryPage = m_pParent->m_pSummaryPage;

	CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
	CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//	CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CMCAModParam* modparpage  = m_pParent->m_pModParams;


	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND5,Data);
//	summarypage->StaticColors[IDC_MCASUMMARY_COMMAND6-IDC_MCASUMMARY_FIRST] = 
//		summarypage->StaticColors[IDC_MCASUMMARY_COMMAND5-IDC_MCASUMMARY_FIRST];
	summarypage->SetDlgItemText(IDC_MCASUMMARY_COMMAND6,Data);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND4,Data);
	summarypage->SetDlgItemText(IDC_MCASUMMARY_COMMAND5,Data);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND3,Data);
	summarypage->SetDlgItemText(IDC_MCASUMMARY_COMMAND4,Data);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_MCASUMMARY_COMMAND3,Data);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND1,Data);
	summarypage->SetDlgItemText(IDC_MCASUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_MCASUMMARY_COMMAND1,Message);

	if (strstr(Message,"LONG BREAK SET"))
	{
		COleDateTime Now = COleDateTime::GetCurrentTime();
		CString DateTime;
		DateTime = Now.Format("Long Break Sent: %d/%H:%M:%S");
		m_cCheckSumMsg = DateTime;
		m_cCommStatus = "Comm Status Unknown";
		SetChecksumDisplay();
//		generalpage->SetDlgItemText(IDC_MCAGENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
		generalpage->SetColor(IDC_MCAGENERAL_CHECKSUM,COLOR_RED);
		SetTabColor(IDC_MCAGENERAL_TAB,COLOR_RED);
	}

	if (strstr(Message,"LOCAL LONG BREAK"))
	{
		COleDateTime Now = COleDateTime::GetCurrentTime();
		CString DateTime;
		DateTime = Now.Format("Local Long Break Sent: %d/%H:%M:%S");
		m_cCheckSumMsg = DateTime;
		SetChecksumDisplay();
//		generalpage->SetDlgItemText(IDC_MCAGENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
		generalpage->SetColor(IDC_MCAGENERAL_CHECKSUM,COLOR_RED);
		SetTabColor(IDC_MCAGENERAL_TAB,COLOR_RED);
	}
}

void CMCAPropertySheet::SetChecksumDisplay()
{
	CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CString cString;
	cString = m_cCheckSumMsg;
	cString += " ";
	cString += m_cCommStatus;
	generalpage->SetDlgItemText(IDC_MCAGENERAL_CHECKSUM,cString);
}

void CMCAPropertySheet::OnReceiveOOB(WPARAM WParam, LPARAM LParam)
{
//	CMCASummary*  SummaryPage = m_pParent->m_pSummaryPage;
//	CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;
	char* cbuff = (char*)LParam;
	char cbuff2[1024];
	char cbuff3[1024];
	cbuff2[1023] = NULL;
//	int length = min(sizeof(cbuff2)-1,(int)WParam);
	strcpy(cbuff2,"34160 OOB: ");
	int length2 = strlen(cbuff2) + (int)WParam;
	try
	{
		strncat(cbuff2,cbuff,(int)WParam);
		strncpy(cbuff3,cbuff,(int)WParam);
		cbuff3[(int)WParam] = NULL;
		cbuff2[length2] = NULL;
		m_cCommStatus = cbuff3;
		SetChecksumDisplay();
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP,NULL,"34150 Bad LParam Address Received (OOB)");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"34150 Bad LParam Address Received (OOB)");
		CoTaskMemFree((void *)LParam);
		return;
	}

	strcat(cbuff2,"\n\r");

	WriteToFile(TO_PFM, TYPE_COMP, NULL, cbuff2);
	UpdateSummary(cbuff2);
	if (m_pParent->m_bDoDump)
		WriteToFile(TO_DMP,TYPE_DUMP, NULL, cbuff2);

	CoTaskMemFree((void *)LParam);
}

void CMCAPropertySheet::OnReceive(WPARAM WParam, LPARAM LParam)
{
	char* cbuff =(char*)LParam;
	char c;
	try
	{
		c = cbuff[0];
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP, NULL,"34170 Bad LParam Address Received (1000)");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"34170 Bad LParam Address Received (1000)");
		CoTaskMemFree((void *)LParam);
		return;
	}

	for (unsigned int index = 0; index < WParam; index ++)
	{
		m_pAccumulator[m_dAt] = (char)(cbuff[index] & 0x7f);
		if	(
			(m_dAt>=1) 
			&&
			(
			  ((m_pAccumulator[m_dAt]==EOL1)&&(m_pAccumulator[m_dAt-1]==EOL2)) ||
			  ((m_pAccumulator[m_dAt]==EOL2)&&(m_pAccumulator[m_dAt-1]==EOL1)) 
			)
			)
		{
			short message_type = UNKNOWN_REC;
			char* cbuff2 = (char*)malloc(m_dAt+2);
			memmove(cbuff2,m_pAccumulator,m_dAt+1);
			cbuff2[m_dAt+1] = NULL;

			if (m_pParent->m_bDoDump)
			{
				char cBuff[1024];
				strcpy(cBuff,"34180 ");
				strcat(cBuff,cbuff2);
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);//dump file
			}

			UpdateSummary(cbuff2);
//try{
			sscanf(cbuff2,"%2hx",&message_type);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 112",MB_OK);
//	AfxMessageBox(cbuff2,MB_OK);
//}

			switch(message_type)
			{
			case MCACONFIG1_RECORD:		Config1Rec(cbuff2);	break;//	10
			case MCACONFIG2_RECORD:		Config2Rec(cbuff2);	break;//	11
			case MCACONFIG3_RECORD:		Config3Rec(cbuff2);	break;//	12
			case MCACONFIG4_RECORD:		Config4Rec(cbuff2);	break;//	13
			case MCACONFIG5_RECORD:		Config5Rec(cbuff2);	break;//	14
			case ACQUIRE_RECORD:		AcquirRec(cbuff2);	break;//	15
			case END_DUMP_RECORD:		DumpEnd(cbuff2);	break;//	16

			case SPECT_HEAD_RECORD:		SpectHead(cbuff2);	break;//	20
			case SPECT_DATA_RECORD:		SpectData(cbuff2);	break;//	21
			case SPECT_TAIL_RECORD:		SpectTail(cbuff2);	break;//	22

			case END_DUMP2_RECORD:		DumpEnd2(cbuff2);	break;//	61
			case DUMP_OK_RECORD:		DumpOk(cbuff2);		break;//	17
			case BATTERY_RECORD:		BatteryRec(cbuff2);	break;//	18
			case GEN_STATUS_RECORD:		GenStatRec(cbuff2);	break;//	1b
			case MONITOR_ID2_RECORD:	ID2_Record(cbuff2);	break;//	1e
			case ANALYZE_OK_RECORD:		AnalyzeOk(cbuff2);	break;//	1f
			case INFORMATION_RECORD:	InformRec(cbuff2);	break;//	30
			case NINETY:				Echo_4_Rec(cbuff2);	break;//	90
			default:
				if (strstr(cbuff2,"BUSY"))
				{
					Busy_Rec(cbuff2);
				}
				else if (strstr(cbuff2,"TIME ="))
				{
					Echo_Rec(cbuff2);
				}
				else if (strstr(cbuff2,"??"))
				{
					Echo_3_Rec(cbuff2);
				}
				else if (strstr(cbuff2,"MINIADC"))
				{
					Echo_5_Rec(cbuff2);
				}
				else if (strncmp(cbuff2,"90",2)==0)
//				else if (strstr(cbuff2,"90"))
				{
					Echo_4_Rec(cbuff2);
				}
				else if (strlen(cbuff2)==0)
				{
				}
				else 
				{
					Echo_2_Rec(cbuff2);
				}
			}							
			delete [] cbuff2;
			m_dAt = 0;
		}
		else
		{
			m_dAt++;
			if (m_dAt >= MCAACCUMULATOR_SIZE) m_dAt = 0;
		}
	}

	CoTaskMemFree((void *)LParam);
}

void CMCAPropertySheet::Busy_Rec(char * /*Record*/)
{
	m_pParent->PostMessage(uIDC_RECEIVE,BUSY_RECORD,CHECKSUM_GOOD);
}

void CMCAPropertySheet::Echo_Rec(char * /*Record*/)
{
	m_pParent->PostMessage(uIDC_RECEIVE,ECHO_RECORD,CHECKSUM_GOOD);
}

void CMCAPropertySheet::Echo_5_Rec(char * /*Record*/)
{
	m_pParent->PostMessage(uIDC_RECEIVE,MINIADC_REC,CHECKSUM_GOOD);
}

void CMCAPropertySheet::Echo_4_Rec(char * /*Record*/)
{
	m_pParent->PostMessage(uIDC_RECEIVE,ECHO_4_REC,CHECKSUM_GOOD);
}

void CMCAPropertySheet::Echo_3_Rec(char * /*Record*/)
{
	m_pParent->PostMessage(uIDC_RECEIVE,ECHO_3_REC,CHECKSUM_GOOD);
}

void CMCAPropertySheet::Echo_2_Rec(char * /*Record*/)
{
	m_pParent->PostMessage(uIDC_RECEIVE,ECHO_2_REC,CHECKSUM_GOOD);
}

//record 15 Acquire Record 
void CMCAPropertySheet::AcquirRec(char * Record)
{
//	struct AcquireRecord AcquireRec;
	int CheckSum;
	if (LengthOk(MCAACQUIRE_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	
	if (CheckSum == CHECKSUM_GOOD)
	{
		//get pointers to page 2 and summary page
//		CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
//		CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//		CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CMCAModParam* modparpage  = m_pParent->m_pModParams;

		char temp[32];

 		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		int AcquireRecordTarget = m_dAcquireRecordCount;
		AcquireTime[AcquireRecordTarget].Init(temp);
		
		//save time in binary to acquire record
		MCAAcquireRec[AcquireRecordTarget].JulianSeconds = 
			AcquireTime[AcquireRecordTarget].TotalSeconds();

		//get status byte 1
		short Status;
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 113",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		MCAAcquireRec[m_dAcquireRecordCount].StatusByte1 = (BYTE)Status;

		//get status byte 2
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 114",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		MCAAcquireRec[m_dAcquireRecordCount].StatusByte2 = (BYTE)Status;

		//get alarm bytes
		short Alarm;
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Alarm);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 115",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		MCAAcquireRec[m_dAcquireRecordCount].Alarm = (BYTE)Alarm;

		//move all the date displays down one
		datapage->GetDlgItemText(IDC_MCADATA_DATADATE3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_DATADATE4,temp);
		datapage->GetDlgItemText(IDC_MCADATA_DATADATE2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_DATADATE3,temp);
		datapage->GetDlgItemText(IDC_MCADATA_DATADATE1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_DATADATE2,temp);
		datapage->SetDlgItemText(IDC_MCADATA_DATADATE1,AcquireTime[m_dAcquireRecordCount].Date());
	
		//move all the time displays down one
		datapage->GetDlgItemText(IDC_MCADATA_DATATIME3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_DATATIME4,temp);
		datapage->GetDlgItemText(IDC_MCADATA_DATATIME2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_DATATIME3,temp);
		datapage->GetDlgItemText(IDC_MCADATA_DATATIME1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_DATATIME2,temp);
		datapage->SetDlgItemText(IDC_MCADATA_DATATIME1,AcquireTime[m_dAcquireRecordCount].Time());
	
		//move all the status-alarm displays down one
		datapage->GetDlgItemText(IDC_MCADATA_STATUSALARM3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_STATUSALARM4,temp);
		datapage->GetDlgItemText(IDC_MCADATA_STATUSALARM2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_STATUSALARM3,temp);
		datapage->GetDlgItemText(IDC_MCADATA_STATUSALARM1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_STATUSALARM2,temp);
		strncpy(&temp[0],&Record[12],2);
		strncpy(&temp[2],"-",1);
		strncpy(&temp[3],&Record[14],2);
		strncpy(&temp[5],"-",1);
		strncpy(&temp[6],&Record[16],2);
		temp[8] = NULL;
		datapage->SetDlgItemText(IDC_MCADATA_STATUSALARM1,temp);

		//move all the CHAN1 displays down one
		datapage->GetDlgItemText(IDC_MCADATA_U2353,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_U2354,temp);
		datapage->GetDlgItemText(IDC_MCADATA_U2352,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_U2353,temp);
		datapage->GetDlgItemText(IDC_MCADATA_U2351,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_U2352,temp);
		strncpy(temp,&Record[18],10);
		temp[10]=NULL;
		datapage->SetDlgItemText(IDC_MCADATA_U2351,temp);
		//set the acquire record value
		MCAAcquireRec[m_dAcquireRecordCount].U235 = (float)atof(temp);
	
		//move all the CHAN2 displays down one
		datapage->GetDlgItemText(IDC_MCADATA_CS1373,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_CS1374,temp);
		datapage->GetDlgItemText(IDC_MCADATA_CS1372,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_CS1373,temp);
		datapage->GetDlgItemText(IDC_MCADATA_CS1371,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_CS1372,temp);
		strncpy(temp,&Record[28],10);
		temp[10]=NULL;
		datapage->SetDlgItemText(IDC_MCADATA_CS1371,temp);
		//set the acquire record value
		MCAAcquireRec[m_dAcquireRecordCount].CS137 = (float)atof(temp);
	
		//move all the CHAN3 displays down one
		datapage->GetDlgItemText(IDC_MCADATA_U2383,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_U2384,temp);
		datapage->GetDlgItemText(IDC_MCADATA_U2382,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_U2383,temp);
		datapage->GetDlgItemText(IDC_MCADATA_U2381,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_U2382,temp);
		strncpy(temp,&Record[38],10);
		temp[10]=NULL;
		datapage->SetDlgItemText(IDC_MCADATA_U2381,temp);
		//set the acquire record value
		MCAAcquireRec[m_dAcquireRecordCount].U238 = (float)atof(temp);
	
		//move all the CHAN7 displays down one //gross counts
		datapage->GetDlgItemText(IDC_MCADATA_UNKNOWNR3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_UNKNOWNR4,temp);
		datapage->GetDlgItemText(IDC_MCADATA_UNKNOWNR2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_UNKNOWNR3,temp);
		datapage->GetDlgItemText(IDC_MCADATA_UNKNOWNR1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_UNKNOWNR2,temp);
		strncpy(temp,&Record[48],10);
		temp[10]=NULL;
		datapage->SetDlgItemText(IDC_MCADATA_UNKNOWNR1,temp);
		//set the acquire record value
		MCAAcquireRec[m_dAcquireRecordCount].Unknown = (float)atof(temp);

		//move all the CHAN4 displays down one
		datapage->GetDlgItemText(IDC_MCADATA_SCALER3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_SCALER4,temp);
		datapage->GetDlgItemText(IDC_MCADATA_SCALER2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_SCALER3,temp);
		datapage->GetDlgItemText(IDC_MCADATA_SCALER1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_SCALER2,temp);
		strncpy(temp,&Record[58],10);
		temp[10]=NULL;
		datapage->SetDlgItemText(IDC_MCADATA_SCALER1,temp);
		//set the acquire record value
		MCAAcquireRec[m_dAcquireRecordCount].Scaler = (float)atof(temp);
	
		//move all the CHAN5 displays down one
		datapage->GetDlgItemText(IDC_MCADATA_U235RATIO3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_U235RATIO4,temp);
		datapage->GetDlgItemText(IDC_MCADATA_U235RATIO2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_U235RATIO3,temp);
		datapage->GetDlgItemText(IDC_MCADATA_U235RATIO1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_U235RATIO2,temp);
		strncpy(temp,&Record[68],10);
		temp[10]=NULL;
		datapage->SetDlgItemText(IDC_MCADATA_U235RATIO1,temp);
		//set the acquire record value
		MCAAcquireRec[m_dAcquireRecordCount].U235Ratio = (float)atof(temp);
	
		//move all the CHAN6 displays down one
		datapage->GetDlgItemText(IDC_MCADATA_CS137RATIO3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_CS137RATIO4,temp);
		datapage->GetDlgItemText(IDC_MCADATA_CS137RATIO2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_CS137RATIO3,temp);
		datapage->GetDlgItemText(IDC_MCADATA_CS137RATIO1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_CS137RATIO2,temp);
		strncpy(temp,&Record[78],10);
		temp[10]=NULL;
		datapage->SetDlgItemText(IDC_MCADATA_CS137RATIO1,temp);
		//set the acquire record value
		MCAAcquireRec[m_dAcquireRecordCount].CS137Ratio = (float)atof(temp);
	

		//move all Elapsed Time displays down one
		datapage->GetDlgItemText(IDC_MCADATA_ET3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_ET4,temp);
		datapage->GetDlgItemText(IDC_MCADATA_ET2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_ET3,temp);
		datapage->GetDlgItemText(IDC_MCADATA_ET1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_MCADATA_ET2,temp);
		strncpy(temp,&Record[88],5);
		temp[5]=NULL;
		datapage->SetDlgItemText(IDC_MCADATA_ET1,temp);
		//set the acquire record value
		MCAAcquireRec[m_dAcquireRecordCount].ElapsedTime = atoi(temp);
	
		//if not in dumplast sequence then write this record to BID file
		if (!m_pParent->m_bInDumpLast)
		{
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= MCAACQUIRERECCOUNT) 
				m_dAcquireRecordCount = MCAACQUIRERECCOUNT-1;
		}
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ACQUIRE_RECORD,CheckSum);	
}

//record 16
void CMCAPropertySheet::DumpEnd(char *Record)
{
	int CheckSum;
	if (LengthOk(MCAEND_DUMP_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	m_pParent->m_dNumberOfAcquires = -1;
	m_pParent->PostMessage(uIDC_RECEIVE,END_DUMP_RECORD,CheckSum);
}

//record 61
void CMCAPropertySheet::DumpEnd2(char *Record)
{
	int CheckSum;
	if (LengthOk(MCAEND_DUMP2_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	char temp[2];
	if (CheckSum == CHECKSUM_GOOD)
	{
		temp[0] = Record[2];
		temp[1] = NULL;
//try{
		sscanf(temp,"%1x",&(m_pParent->m_dNumberOfAcquires));
//}
//catch(...)
//{
//	AfxMessageBox("Error at 116",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
	}
	else
		m_pParent->m_dNumberOfAcquires = -1;

	m_pParent->PostMessage(uIDC_RECEIVE,END_DUMP2_RECORD,CheckSum);
}

//record 17
void CMCAPropertySheet::DumpOk(char *Record)
{
	int CheckSum;
	if (LengthOk(MCADUMP_OK_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[8];
//		CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
//		CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//		CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CMCAModParam* modparpage  = m_pParent->m_pModParams;

		//get bbm bytes
		int bbm;
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		bbm = atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
		m_pParent->m_pButton->put_Numeric((ULONG)bbm);
		datapage->SetDlgItemText(IDC_MCADATA_BYTESMEM,temp);
	
		//check checksum
			//send message to instrument if bad
		//send message to parent saying we got this record
		//use second parameter as flag for good or bad checksum
		m_pParent->PostMessage(uIDC_RECEIVE,IDC_BBM_VALUE,bbm);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,DUMP_OK_RECORD,CheckSum);
}

//record 18
void CMCAPropertySheet::BatteryRec(char *Record)
{
	int CheckSum;
	if (LengthOk(MCABATTERY_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[32];

		CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
		CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//		CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CMCAModParam* modparpage  = m_pParent->m_pModParams;

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		strncpy(temp,&Record[12],4);
		temp[4] = NULL;
		bool CurrentRed = false;
		bool CurrentLow = false;
		bool CurrentHigh= false;

		m_fCurBV=(float)atof(temp);
		if (m_bFirstBV)
		{
			m_bFirstBV = false;
			m_fLowBV = m_fHighBV = m_fCurBV;
		}
		if (m_fCurBV < m_fLowBV)
		{
			CurrentLow = true;
			m_fLowBV = m_fCurBV;
		}
		else
			CurrentLow = false;
		if (m_fCurBV > m_fHighBV)
		{
			CurrentHigh = true;
			m_fHighBV= m_fCurBV;
		}
		else
			CurrentHigh = false;
		if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
		{
			CurrentRed = true;
			m_pParent->FlagOthr(1,true);			
			generalpage->SetColor(IDC_MCAGENERAL_BATTERY2,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_BATTERY2,COLOR_RED);
			if (m_bBatteryOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34190 STATUS CHANGE - BATTERY OUT OF TOLERANCE (18)\n\r");
			m_bBatteryOk = false;
			General_Tab |= 0x1000;
		}
		else
		{
			CurrentRed = false;
			if (!m_bBatteryOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34200 STATUS CHANGE - BATTERY IN TOLERANCE (18)\n\r");
			m_bBatteryOk = true;
			General_Tab &= ~(0x1000);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowBV <= m_fBatVoltL) || (m_fLowBV >= m_fBatVoltH))
		   )
		{
			generalpage->SetColor(IDC_MCAGENERAL_BATTERY1,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_BATTERY1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighBV <= m_fBatVoltL) || (m_fHighBV >= m_fBatVoltH))
		   )
		{
			generalpage->SetColor(IDC_MCAGENERAL_BATTERY3,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_BATTERY3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowBV);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_BATTERY1,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_BATTERY1,temp);
		sprintf(temp,"%5.1f",m_fCurBV);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_BATTERY2,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_BATTERY2,temp);
		sprintf(temp,"%5.1f",m_fHighBV);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_BATTERY3,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_BATTERY3,temp);

		char cBuff[1024];
		strcpy(cBuff,"34210 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);

		if (General_Tab)
			SetTabColor(IDC_MCAGENERAL_TAB,COLOR_RED);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,BATTERY_RECORD,CheckSum);
}

//record 1b
void CMCAPropertySheet::GenStatRec(char *Record)
{
	int CheckSum;
	if (LengthOk(MCAGEN_STATUS_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		short Status;

		CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
		CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//		CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CMCAModParam* modparpage  = m_pParent->m_pModParams;

		char temp[64];
		//get julian time and date
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		generalpage->SetDlgItemText(IDC_MCAGENERAL_TIME,temp);

		//status byte 1
		strncpy(temp,&Record[12],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 117",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//ac power loss
		if (Status & MCAACPOWERLOSS)
		{
			strcpy(temp,"Fail");
			generalpage->SetColor(IDC_MCAGENERAL_ACPOWER,COLOR_RED);
			General_Tab |= 0x0001;
		}
		else 
		{
			strcpy(temp,"Ok");
			General_Tab &= ~(0x0001);
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_ACPOWER,temp);
	
		//battery low bit
		if (Status & MCABATTERYLOW) 
		{
			strcpy(temp,"Low");
			generalpage->SetColor(IDC_MCAGENERAL_BATSTAT,COLOR_RED);
			General_Tab |= 0x0002;
		}
		else
		{
			strcpy(temp,"Ok");
			General_Tab &= ~(0x0002);
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_BATSTAT,temp);

		//Filtering on or off
		if (Status & MCAFILTEROFF) 
		{
			strcpy(temp,"No");
			General_Tab &= ~(0x0004);
		}
		else
		{
			strcpy(temp,"Yes");
			generalpage->SetColor(IDC_MCAGENERAL_FILTERING,COLOR_RED);
			General_Tab |= 0x0004;
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_FILTERING,temp);

		//Spectrum changing
		if (Status & MCASPECTRUMCH) 
		{
			strcpy(temp,"Yes");
			generalpage->SetColor(IDC_MCAGENERAL_SPECTCHANG,COLOR_RED);
			General_Tab |= 0x0008;
		}
		else
		{
			strcpy(temp,"No");
			General_Tab &= ~(0x0008);
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_SPECTCHANG,temp);

		//get bbm error from status byte 1
		bool BBMErrorInS1 = false;
		if (Status & MCABBMERROR)
			BBMErrorInS1 = true;
		else
			BBMErrorInS1 = false;

		//restart required
		if (Status & MCARESTARTREQ) 
		{
			strcpy(temp,"Yes");
			generalpage->SetColor(IDC_MCAGENERAL_RESTART,COLOR_RED);
			General_Tab |= 0x0008;
		}
		else
		{
			strcpy(temp,"No");
			General_Tab &= ~(0x0008);
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_RESTART,temp);

		//get status byte 2
		strncpy(temp,&Record[14],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 118",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//BBM error
		if (BBMErrorInS1) 
		{
			int BBMErr = Status | MCABBMERRMASK;
			switch (BBMErr) 
			{
			case MCABBMBADREAD:
				strcpy(temp,MCABBMBADREAD_TEXT);
				break;
			case MCABBMBADWRITE:
				strcpy(temp,MCABBMBADWRITE_TEXT);
				break;
			case MCABBMEMPTY:
				strcpy(temp,MCABBMEMPTY_TEXT);
				break;
			case MCABBMFULL:
				strcpy(temp,MCABBMFULL_TEXT);
				break;
			case MCABBMBADBUFSZ:
				strcpy(temp,MCABBMBADBUFSZ_TEXT);
				break;
			case MCABBMBADOPCOD:
				strcpy(temp,MCABBMBADOPCOD_TEXT);
				break;
			default:
				strcpy(temp,UNKNOWN_TEXT);
			}
			generalpage->SetColor(IDC_MCAGENERAL_BBMSTAT,COLOR_RED);
			General_Tab |= 0x0010;
		}
		else
		{
			strcpy(temp,"Ok");
			General_Tab &= ~(0x0010);
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_BBMSTAT,temp);

		//MONITOR error
		int MonErr = (Status & MCAMONERRMASK);
		if (MonErr) 
		{
			switch (MonErr) 
			{
			case MCAMONACQFAIL:
				strcpy(temp,MCAMONACQFAIL_TEXT);
				break;
			case MCAMONMONFAIL:
				strcpy(temp,MCAMONMONFAIL_TEXT);
				break;
//			case MCAMONTHVNOTS:
//				strcpy(temp,MCAMONTHVNOTS_TEXT);
//				break;
			case MCAMONHVCFAIL:
				strcpy(temp,MCAMONHVCFAIL_TEXT);
				break;
			case MCAMONGDCFAIL:
				strcpy(temp,MCAMONGDCFAIL_TEXT);
				break;
//			case MCAMONHVNOTST:
//				strcpy(temp,MCAMONHVNOTST_TEXT);
//				break;
			default:
				strcpy(temp,MCAMONUNKNOWN_TEXT);
			}
			summarypage->SetColor(IDC_MCASUMMARY_MONSTATUS,COLOR_RED);
			generalpage->SetColor(IDC_MCAGENERAL_MONITORSTAT,COLOR_RED);
			General_Tab |= 0x0020;
		}
		else
		{
			strcpy(temp,"Ok");
			General_Tab &= ~(0x0020);
		}
		summarypage->SetDlgItemText(IDC_MCASUMMARY_MONSTATUS,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_MONITORSTAT,temp);
			
		//get alarm flags
		strncpy(temp,&Record[16],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 119",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//alarm U235
		if (Status & MCAALARMU235)
		{
			strcpy(temp,"Over");
			generalpage->SetColor(IDC_MCAGENERAL_U235STATE,COLOR_RED);
			if (!m_bInMIIU235 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_U235STATE,szTemp1,sizeof(szTemp1));	//DR356
				// or may do this on each of these												//DR356
				//GetPrivateProfileString(name,"ALARM1","",buff,sizeof(buff),inifile);			//DR356
				sprintf(szTemp2,"34220 STATUS CHANGE - IN ALARM %s (1b)\n\r",szTemp1);			//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
			}
			General_Tab |= (0x0040);
			m_bInMIIU235 = true;
		}
		else 
		{
			strcpy(temp,"No");
//			generalpage->SetColor(IDC_MCAGENERAL_U235STATE,COLOR_BLACK);
			if (m_bInMIIU235 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_U235STATE,szTemp1,sizeof(szTemp1));	//DR356
				sprintf(szTemp2,"34230 STATUS CHANGE - NOT IN ALARM %s (1b)\n\r",szTemp1);		//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
			}
			General_Tab &= ~(0x0040);
			m_bInMIIU235 = false;
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_U235STATE,temp);

		//alarm U238
		if (Status & MCAALARMU238)
		{
			strcpy(temp,"Over");
			generalpage->SetColor(IDC_MCAGENERAL_U238STATE,COLOR_RED);
			if (!m_bInMIIU238 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_U238STATE,szTemp1,sizeof(szTemp1));	//DR356
				sprintf(szTemp2,"34240 STATUS CHANGE - IN ALARM %s (1b)\n\r",szTemp1);			//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34240 STATUS CHANGE - IN ALARM U238 (1b)\n\r");//DR236
			}
			General_Tab |= (0x0080);
			m_bInMIIU238 = true;
		}
		else 
		{
			strcpy(temp,"No");
//			generalpage->SetColor(IDC_MCAGENERAL_U238STATE,COLOR_BLACK);
			if (m_bInMIIU238 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_U238STATE,szTemp1,sizeof(szTemp1));	//DR356
				sprintf(szTemp2,"34250 STATUS CHANGE - NOT IN ALARM %s (1b)\n\r",szTemp1);			//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34250 STATUS CHANGE - NOT IN ALARM U238 (1b)\n\r");
			}
			General_Tab &= ~(0x0080);
			m_bInMIIU238 = false;
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_U238STATE,temp);

		//alarm Cs137
		if (Status & MCAALARMCS137)
		{
			strcpy(temp,"Over");
			generalpage->SetColor(IDC_MCAGENERAL_CS137STATE,COLOR_RED);
			if (!m_bInMIICs137 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_CS137STATE,szTemp1,sizeof(szTemp1));	//DR356
				sprintf(szTemp2,"34260 STATUS CHANGE - IN ALARM %s (1b)\n\r",szTemp1);			//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34260 STATUS CHANGE - IN ALARM Cs137 (1b)\n\r");
			}
			General_Tab |= (0x0100);
			m_bInMIICs137 = true;
		}
		else 
		{
			strcpy(temp,"No");
//			generalpage->SetColor(IDC_MCAGENERAL_CS137STATE,COLOR_BLACK);
			if (m_bInMIICs137 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_CS137STATE,szTemp1,sizeof(szTemp1));	//DR356
				sprintf(szTemp2,"34270 STATUS CHANGE - NOT IN ALARM %s (1b)\n\r",szTemp1);		//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34270 STATUS CHANGE - NOT IN ALARM Cs137 (1b)\n\r");
			}
			General_Tab &= ~(0x0100);
			m_bInMIICs137 = false;
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_CS137STATE,temp);

		//alarm Scaler
		if (Status & MCAALARMSCALER)
		{
			strcpy(temp,"Over");
			generalpage->SetColor(IDC_MCAGENERAL_SCALESTATE,COLOR_RED);
			if (!m_bInMIIScaler && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_SCALESTATE,szTemp1,sizeof(szTemp1));	//DR356
				sprintf(szTemp2,"34280 STATUS CHANGE - IN ALARM %s (1b)\n\r",szTemp1);			//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34280 STATUS CHANGE - IN ALARM Scaler (1b)\n\r");
			}
			General_Tab |= (0x0200);
			m_bInMIIScaler = true;
		}
		else 
		{
			strcpy(temp,"No");
//			generalpage->SetColor(IDC_MCAGENERAL_SCALESTATE,COLOR_BLACK);
			if (m_bInMIIScaler && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_SCALESTATE,szTemp1,sizeof(szTemp1));	//DR356
				sprintf(szTemp2,"34290 STATUS CHANGE - NOT IN ALARM %s (1b)\n\r",szTemp1);		//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34290 STATUS CHANGE - NOT IN ALARM Scaler (1b)\n\r");
			}
			General_Tab &= ~(0x0200);
			m_bInMIIScaler = false;
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_SCALESTATE,temp);

		//alarm Gross Counts
		if (Status & MCAALARMGROSSC)
		{
			strcpy(temp,"Over");
			generalpage->SetColor(IDC_MCAGENERAL_GROSSCOUNTS,COLOR_RED);
			if (!m_bInMIIGrossCounts && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_GROSSCOUNTS,szTemp1,sizeof(szTemp1));//DR356
				sprintf(szTemp2,"34300 STATUS CHANGE - IN ALARM %s (1b)\n\r",szTemp1);			//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34300 STATUS CHANGE - IN ALARM Gross Counts (1b)\n\r");
			}
			General_Tab |= (0x0200);
			m_bInMIIGrossCounts = true;
		}
		else 
		{
			strcpy(temp,"No");
//			generalpage->SetColor(IDC_MCAGENERAL_GROSSCOUNTS,COLOR_BLACK);
			if (m_bInMIIGrossCounts && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_GROSSCOUNTS,szTemp1,sizeof(szTemp1));//DR356
				sprintf(szTemp2,"34310 STATUS CHANGE - NOT IN ALARM %s (1b)\n\r",szTemp1);		//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34310 STATUS CHANGE - NOT IN ALARM Gross Counts (1b)\n\r");
			}
			General_Tab &= ~(0x0200);
			m_bInMIIGrossCounts = false;
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_GROSSCOUNTS,temp);

		//alarm Ratio 1
		if (Status & MCAALARMRATIO1)
		{
			strcpy(temp,"Over");
			generalpage->SetColor(IDC_MCAGENERAL_RATIO1,COLOR_RED);
			if (!m_bInMIIRatio1 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_RATIO1,szTemp1,sizeof(szTemp1));		//DR356
				sprintf(szTemp2,"34320 STATUS CHANGE - IN ALARM %s (1b)\n\r",szTemp1);			//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34320 STATUS CHANGE - IN ALARM Ratio 1 (1b)\n\r");
			}
			General_Tab |= (0x0200);
			m_bInMIIRatio1 = true;
		}
		else 
		{
			strcpy(temp,"No");
//			generalpage->SetColor(IDC_MCAGENERAL_RATIO1,COLOR_BLACK);
			if (m_bInMIIRatio1 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_RATIO1,szTemp1,sizeof(szTemp1));		//DR356
				sprintf(szTemp2,"34330 STATUS CHANGE - NOT IN ALARM %s (1b)\n\r",szTemp1);		//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34330 STATUS CHANGE - NOT IN ALARM Ratio 1 (1b)\n\r");
			}
			General_Tab &= ~(0x0200);
			m_bInMIIRatio1 = false;
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_RATIO1,temp);

		//alarm Ratio 2
		if (Status & MCAALARMRATIO2)
		{
			strcpy(temp,"Over");
			generalpage->SetColor(IDC_MCAGENERAL_RATIO2,COLOR_RED);
			if (!m_bInMIIRatio2 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_RATIO2,szTemp1,sizeof(szTemp1));		//DR356
				sprintf(szTemp2,"34340 STATUS CHANGE - IN ALARM %s (1b)\n\r",szTemp1);			//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34340 STATUS CHANGE - IN ALARM Ratio 2 (1b)\n\r");
			}
			General_Tab |= (0x0200);
			m_bInMIIRatio2 = true;
		}
		else 
		{
			strcpy(temp,"No");
//			generalpage->SetColor(IDC_MCAGENERAL_RATIO2,COLOR_BLACK);
			if (m_bInMIIRatio2 && m_pParent->m_bLogMII)
			{
				char szTemp1[16];																//DR356
				char szTemp2[64];																//DR356
				generalpage->GetDlgItemText(IDC_MCAGENERAL_RATIO2,szTemp1,sizeof(szTemp1));		//DR356
				sprintf(szTemp2,"34350 STATUS CHANGE - NOT IN ALARM %s (1b)\n\r",szTemp1);			//DR356
				WriteToFile(TO_PFM, TYPE_INST, &Julian, szTemp2);								//DR356
//				WriteToFile(TO_PFM, TYPE_INST, &Julian, "34350 STATUS CHANGE - NOT IN ALARM Ratio 2 (1b)\n\r");
			}
			General_Tab &= ~(0x0200);
			m_bInMIIRatio1 = false;
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_RATIO2,temp);
		
		//battery voltage
		strncpy(temp,&Record[18],4);
		temp[4] = NULL;
		bool CurrentRed = false;
		bool CurrentLow = false;
		bool CurrentHigh= false;
		m_fCurBV=(float)atof(temp);
		if (m_bFirstBV)
		{
			m_bFirstBV = false;
			m_fLowBV = m_fHighBV = m_fCurBV;
		}
		if (m_fCurBV < m_fLowBV) 
		{
			CurrentLow = true;
			m_fLowBV = m_fCurBV;
		}
		else
			CurrentLow = false;
		if (m_fCurBV > m_fHighBV)
		{
			CurrentHigh = true;
			m_fHighBV= m_fCurBV;
		}
		else
			CurrentHigh = false;
		if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
		{
			CurrentRed = true;
			m_pParent->FlagOthr(1,true);			
			generalpage->SetColor(IDC_MCAGENERAL_BATTERY2,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_BATTERY2,COLOR_RED);
			if (m_bBatteryOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34360 STATUS CHANGE - BATTERY OUT OF TOLERANCE (1b)\n\r");
			m_bBatteryOk = false;
			General_Tab |= 0x1000;
		}
		else
		{
			CurrentRed = false;
			if (!m_bBatteryOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34370 STATUS CHANGE - BATTERY IN TOLERANCE (1b)\n\r");
			m_bBatteryOk = true;
			General_Tab &= ~(0x1000);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowBV <= m_fBatVoltL) || (m_fLowBV >= m_fBatVoltH))
			)
		{
			generalpage->SetColor(IDC_MCAGENERAL_BATTERY1,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_BATTERY1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighBV <= m_fBatVoltL) || (m_fHighBV >= m_fBatVoltH))
		   )
		{
			generalpage->SetColor(IDC_MCAGENERAL_BATTERY3,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_BATTERY3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowBV);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_BATTERY1,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_BATTERY1,temp);
		sprintf(temp,"%5.1f",m_fCurBV);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_BATTERY2,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_BATTERY2,temp);
		sprintf(temp,"%5.1f",m_fHighBV);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_BATTERY3,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_BATTERY3,temp);

		//+5 volts
		strncpy(temp,&Record[22],4);
		temp[4] = NULL;
		m_fCurP5=(float)atof(temp);
		if (m_bFirstP5)
		{
			m_bFirstP5 = false;
			m_fLowP5 = m_fHighP5 = m_fCurP5;
		}
		if (m_fCurP5 < m_fLowP5)
		{
			CurrentLow = true;
			m_fLowP5  = m_fCurP5;
		}
		else
			CurrentLow = false;
		if (m_fCurP5 > m_fHighP5)
		{
			CurrentHigh = true;
			m_fHighP5 = m_fCurP5;
		}
		else
			CurrentHigh = false;
		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		{
			CurrentRed = true;
			m_pParent->FlagOthr(1,true);			
			generalpage->SetColor(IDC_MCAGENERAL_P5VOLT2,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_P5VOLT2,COLOR_RED);
			if (m_bP5VoltOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34380 STATUS CHANGE - Plus 5 Volt Out Of Tolerance (1b)\n\r");
			m_bP5VoltOk = false;
			General_Tab |= 0x0800;
		}
		else
		{
			CurrentRed = false;
			if (m_bP5VoltOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34390 STATUS CHANGE - Plus 5 Volt in Tolerance (1b)\n\r");
			m_bP5VoltOk = false;
			General_Tab &= ~(0x0800);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowP5 <= m_fSup_P05L) || (m_fLowP5 >= m_fSup_P05H))
		   )
		{
			generalpage->SetColor(IDC_MCAGENERAL_P5VOLT1,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_P5VOLT1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighP5 <= m_fSup_P05L) || (m_fHighP5 >= m_fSup_P05H))
		   )
		{
			generalpage->SetColor(IDC_MCAGENERAL_P5VOLT3,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_P5VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowP5);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_P5VOLT1,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_P5VOLT1,temp);
		sprintf(temp,"%5.1f",m_fCurP5);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_P5VOLT2,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_P5VOLT2,temp);
		sprintf(temp,"%5.1f",m_fHighP5);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_P5VOLT3,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_P5VOLT3,temp);

		//+15 volts
		strncpy(temp,&Record[26],4);
		temp[4] = NULL;
		m_fCurP15=(float)atof(temp);
		if (m_bFirstP15)
		{
			m_bFirstP15 = false;
			m_fLowP15 = m_fHighP15 = m_fCurP15;
		}
		if (m_fCurP15 < m_fLowP15)
		{
			CurrentLow = true;
			m_fLowP15  = m_fCurP15;
		}
		else
			CurrentLow = false;
		if (m_fCurP15 > m_fHighP15)
		{
			CurrentHigh = true;
			m_fHighP15 = m_fCurP15;
		}
		else
			CurrentHigh = false;
		if ((m_fCurP15 <= m_fSup_P15L) || (m_fCurP15 >= m_fSup_P15H))
		{
			CurrentRed = true;
			m_pParent->FlagOthr(1,true);			
			generalpage->SetColor(IDC_MCAGENERAL_P15VOLT2,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_P15VOLT2,COLOR_RED);
			if (m_bP15VoltOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34400 STATUS CHANGE - Plus 15 Volt Out Of Tolerance (1b)\n\r");
			m_bP15VoltOk = false;
			General_Tab |= 0x1000;
		}
		else
		{
			CurrentRed = false;
			if (m_bP15VoltOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34410 STATUS CHANGE - Plus 15 Volt In Tolerance (1b)\n\r");
			m_bP15VoltOk = false;
			General_Tab &= ~(0x1000);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowP15 <= m_fSup_P15L) || (m_fLowP15 >= m_fSup_P15H))
		   )
		{
			generalpage->SetColor(IDC_MCAGENERAL_P15VOLT1,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_P15VOLT1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighP15 <= m_fSup_P15L) || (m_fHighP15 >= m_fSup_P15H))
		   )
		{
			generalpage->SetColor(IDC_MCAGENERAL_P15VOLT3,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_P15VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowP15);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_P15VOLT1,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_P15VOLT1,temp);
		sprintf(temp,"%5.1f",m_fCurP15);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_P15VOLT2,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_P15VOLT2,temp);
		sprintf(temp,"%5.1f",m_fHighP15);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_P15VOLT3,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_P15VOLT3,temp);

		//-15 volt
		strncpy(temp,&Record[30],5);
		temp[5] = NULL;
		m_fCurM15=(float)atof(temp);
		if (m_bFirstM15)
		{
			m_bFirstM15 = false;
			m_fLowM15 = m_fHighM15 = m_fCurM15;
		}
		if (m_fCurM15 < m_fLowM15)
		{
			CurrentLow = true;
			m_fLowM15  = m_fCurM15;
		}
		else
			CurrentLow = false;

		if (m_fCurM15 > m_fHighM15)
		{
			CurrentHigh = true;
			m_fHighM15 = m_fCurM15;
		}
		else
			CurrentHigh = false;
		if ((m_fCurM15 <= m_fSup_M15L) || (m_fCurM15 >= m_fSup_M15H))
		{
			CurrentRed = true;
			m_pParent->FlagOthr(1,true);			
			generalpage->SetColor(IDC_MCAGENERAL_M15VOLT2,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_M15VOLT2,COLOR_RED);
			if (m_bM15VoltOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34420 STATUS CHANGE - Minus 15 Volt Out Of Tolerance (1b)\n\r");
			m_bM15VoltOk = false;
			General_Tab |= 0x2000;
		}
		else
		{
			CurrentRed = false;
			if (m_bM15VoltOk)
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34430 STATUS CHANGE - Minus 15 Volt In Tolerance (1b)\n\r");
			m_bM15VoltOk = false;
			General_Tab &= ~(0x2000);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowM15 <= m_fSup_M15L) || (m_fLowM15 >= m_fSup_M15H))
		   )
		{
			generalpage->SetColor(IDC_MCAGENERAL_M15VOLT1,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_M15VOLT1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighM15 <= m_fSup_M15L) || (m_fHighM15 >= m_fSup_M15H))
		   )
		{
			generalpage->SetColor(IDC_MCAGENERAL_M15VOLT3,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_M15VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowM15);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_M15VOLT1,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_M15VOLT1,temp);
		sprintf(temp,"%5.1f",m_fCurM15);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_M15VOLT2,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_M15VOLT2,temp);
		sprintf(temp,"%5.1f",m_fHighM15);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_M15VOLT3,temp);
		generalpage->SetDlgItemText(IDC_MCAGENERAL_M15VOLT3,temp);


		//actual hv
		strncpy(temp,&Record[35],3);
		temp[3] = NULL;
		generalpage->SetDlgItemText(IDC_MCAGENERAL_HIGHVACTUAL,temp);

		//desired hv
		strncpy(temp,&Record[38],3);
		temp[3] = NULL;
		generalpage->SetDlgItemText(IDC_MCAGENERAL_HIGHVDESIRED,temp);

		//temp immediate
		strncpy(temp,&Record[41],3);
		temp[3] = NULL;
		generalpage->SetDlgItemText(IDC_MCAGENERAL_TEMPIMM,temp);

		//temp effective
		strncpy(temp,&Record[44],3);
		temp[3] = NULL;
		generalpage->SetDlgItemText(IDC_MCAGENERAL_TEMPEFF,temp);

		//gdc adj factor
		strncpy(temp,&Record[47],5);
		temp[5] = NULL;
		generalpage->SetDlgItemText(IDC_MCAGENERAL_GDCADJ,temp);

		//rp area
		strncpy(temp,&Record[52],6);
		temp[6] = NULL;
		generalpage->SetDlgItemText(IDC_MCAGENERAL_RPAREA,temp);

		//rp centroid
		strncpy(temp,&Record[58],3);
		temp[3] = NULL;
		generalpage->SetDlgItemText(IDC_MCAGENERAL_RPCENT,temp);

		//gdc system soh
		strncpy(temp,&Record[61],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 120",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}


		//gdc enabled
		if (Status & MCAGDCENABLE)
			strcpy(temp,"Yes");
		else 
			strcpy(temp,"No");
		generalpage->SetDlgItemText(IDC_MCAGENERAL_GDCENABLE,temp);

		//gdc on hold
		if (Status & MCAGDCONHOLD)
			strcpy(temp,"Yes");
		else 
			strcpy(temp,"No");
		generalpage->SetDlgItemText(IDC_MCAGENERAL_GDCHOLD,temp);

		if (Status & MCAGDCSOHMASK)
		{
			switch (Status & MCAGDCSOHMASK)
			{
			case MCAGDCRPAREA :
				strcpy(temp,MCAGDCRPAREA_TEXT);
				break;
			case MCAGDCRPCROWD:
				strcpy(temp,MCAGDCRPCROWD_TEXT);
				break;
			case MCAGDCRPANALY:
				strcpy(temp,MCAGDCRPANALY_TEXT);
				break;
			case MCAGDCPEAKSEA:
				strcpy(temp,MCAGDCPEAKSEA_TEXT);
				break;
			case MCAGDCOORANGE:
				strcpy(temp,MCAGDCOORANGE_TEXT);
				break;
			case MCAGDCROIOOLI:
				strcpy(temp,MCAGDCROIOOLI_TEXT);
				break;
			case MCAGDCRPOBSCU:
				strcpy(temp,MCAGDCRPOBSCU_TEXT);
				break;
			default:
				strcpy(temp,UNKNOWN_TEXT);
			}

			m_pParent->FlagOthr(1,true);			
			generalpage->SetColor(IDC_MCAGENERAL_GDCSTATUS,COLOR_RED);
			General_Tab |= 0x4000;
		}
		else
		{
			strcpy(temp,"Ok");
//			generalpage->SetColor(IDC_MCAGENERAL_GDCSTATUS,COLOR_BLACK);
			General_Tab &= ~(0x4000);
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_GDCSTATUS,temp);

		//hvc system soh
		strncpy(temp,&Record[63],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 121",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//hvc enabled
		if (Status & MCAHVCENABLE)
		{
			switch (Status & MCAHVCENABLE)
			{
			case MCAHVCAUTO:
				strcpy(temp,MCAHVCAUTO_TEXT);
				break;
			case MCAHVCFIXED:
				strcpy(temp,MCAHVCFIXED_TEXT);
				break;
			case MCAHVCTEMP:
				strcpy(temp,MCAHVCTFIXED_TEXT);
				break;
			default:
				strcpy(temp,UNKNOWN_TEXT);
			}
		}
		else 
			strcpy(temp,"No");
		generalpage->SetDlgItemText(IDC_MCAGENERAL_HVCENABLE,temp);

		//HVC on hold
		if (Status & MCAHVCONHOLD)
			strcpy(temp,"Yes");
		else 
			strcpy(temp,"No");
		generalpage->SetDlgItemText(IDC_MCAGENERAL_HVCHOLD,temp);

		if (Status & MCAHVCSOHMASK)
		{
			switch (Status & MCAHVCSOHMASK)
			{
			case MCAHVCTEFFOOR:
				strcpy(temp,MCAHVCTEFFOOR_TEXT);
				break;
			case MCAHVCTIMMTOO:
				strcpy(temp,MCAHVCTIMMTOO_TEXT);
				break;
			case MCAHVCTEFFTOO:
				strcpy(temp,MCAHVCTEFFTOO_TEXT);
				break;
			case MCAHVCISDRIFT:
				strcpy(temp,MCAHVCISDRIFT_TEXT);
				break;
			case MCAHVCTARNOTR:
				strcpy(temp,MCAHVCTARNOTR_TEXT);
				break;
			case MCAHVCTIMMOFF:
				strcpy(temp,MCAHVCTIMMOFF_TEXT);
				break;
			case MCAHVCPARAMCS:
				strcpy(temp,MCAHVCPARAMCS_TEXT);
				break;
			default:
				strcpy(temp,UNKNOWN_TEXT);
			}
			m_pParent->FlagOthr(1,true);			
			generalpage->SetColor(IDC_MCAGENERAL_HVCSTATUS,COLOR_RED);
			General_Tab |= 0x8000;
		}
		else
		{
			strcpy(temp,"Ok");
//			generalpage->SetColor(IDC_MCAGENERAL_HVCSTATUS,COLOR_BLACK);
			General_Tab &= ~(0x8000);
		}
		generalpage->SetDlgItemText(IDC_MCAGENERAL_HVCSTATUS,temp);

		if (General_Tab)
			SetTabColor(IDC_MCAGENERAL_TAB,COLOR_RED);

		char cBuff[1024];
		strcpy(cBuff,"34440 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//general stat record 1b
	}
	m_pParent->PostMessage(uIDC_RECEIVE,GEN_STATUS_RECORD,CheckSum);
}

//record 1e
void CMCAPropertySheet::ID2_Record(char *Record)
{
	int CheckSum;
	if (LengthOk(MCAMONITOR_ID2_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		bool AnyChange = false;
		CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
		CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//		CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CMCAModParam* modparpage  = m_pParent->m_pModParams;

		char temp[64];
		unsigned short Status;

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);
		if (Julian.Year() < 1990)
		{
//			m_pParent->PostMessage(IDC_RECEIVE,FORCEDATETIME,CHECKSUM_GOOD);
			m_pParent->PostMessage(uIDC_RECEIVE,IDC_BADYEAR_VALUE,Julian.Year());
		}

		//post invalid time to CEV if necessary
		if (m_szPreviousGTime[0] != NULL) 
			if (strcmp(m_szPreviousGTime,temp)>0)
			{
				m_pParent->m_dInvalidTimeCount++;
				CJulianTime JulianT(m_szPreviousGTime);
				if (m_pParent->m_dInvalidTimeCount < m_pParent->m_iMaxRepetition)
					WriteToFile(TO_CEV|TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
				if (m_pParent->m_dInvalidTimeCount == m_pParent->m_iMaxRepetition)
				{
					WriteToFile(TO_CEV|TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
					WriteToFile(TO_CEV|TO_PFM, TYPE_INST,&JulianT,"34450 INVALID TIME Filter On\n\r");
				}
				Data_Tab |= 0x0001;
				m_pParent->FlagTime(1,false);			
				datapage->SetColor(IDC_MCADATA_INQUIRETIME,COLOR_RED);
				summarypage->SetColor(IDC_MCASUMMARY_INQUIRETIME,COLOR_RED);
				if (!m_bTimeError)
					m_bTimeError = true;
			}
			else
			{
				m_pParent->m_dInvalidTimeCount = 0;
			}

		if (!Julian.IsValid())
		{
			CJulianTime JulianT(m_szPreviousGTime);
			WriteToFile(TO_CEV|TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
			Data_Tab |= 0x0001;
			m_pParent->FlagTime(1,false);			
			datapage->SetColor(IDC_MCADATA_INQUIRETIME,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_INQUIRETIME,COLOR_RED);
			if (!m_bTimeError)
				m_bTimeError = true;
		}

		strcpy(m_szPreviousGTime,temp);

		if ((m_bStartOfFile)||(Julian.Day()!=m_dCurrentDay))
		{
			char cBuff[1024];
			strcpy(cBuff,"34460 ");
			strcat(cBuff,Record);

			WriteToFile(TO_CEV | TO_PFM, TYPE_GID2, &Julian, cBuff);//id2 record if start of file
			m_bStartOfFile = false;
			m_dCurrentDay = Julian.Day();
		}

		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
			      Julian.Hour(), Julian.Minute(), Julian.Second() );
		COleDateTime t2 = COleDateTime::GetCurrentTime();
		COleDateTimeSpan ts = t1 - t2;  // Subtract 2 Times
		ts += HALF_SECOND;
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());

		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
			(ts.GetTotalSeconds() < -m_dTime_Err ))
		{
			m_pParent->FlagTime(1,false);			
			datapage->SetColor(IDC_MCADATA_INQUIRETIME,COLOR_RED);
			summarypage->SetColor(IDC_MCASUMMARY_INQUIRETIME,COLOR_RED);
			Data_Tab |= 0x0001;
			if (!m_bTimeError)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34470 STATUS CHANGE - Time Out of Tolerance (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);//id2 record
				AnyChange = true;
				m_bTimeError = true;
			}
		} 

		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
			(ts.GetTotalSeconds() > -m_dTime_Err ))
		{
//			datapage->SetColor(IDC_MCADATA_INQUIRETIME,COLOR_BLACK);
//			summarypage->SetColor(IDC_MCASUMMARY_INQUIRETIME,COLOR_BLACK);
			Data_Tab &= ~(0x0001);
			if (m_bTimeError)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34480 STATUS CHANGE - Time In Tolerance (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);
				AnyChange = true;
			}
			m_bTimeError = false;
		}

		long days = ts.GetDays();
		if (days > 0)
			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
		else if (days == 0)
			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
		else 
		{
			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
		}
//		strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
		datapage->SetDlgItemText(IDC_MCADATA_INQUIRETIME,temp);
		summarypage->SetDlgItemText(IDC_MCASUMMARY_INQUIRETIME,temp);

		//status byte 1
		strncpy(temp,&Record[12],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 122",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//ac power loss
		if (Status & MCAACPOWERLOSS)
		{
			strcpy(temp,"Fail");
			summarypage->SetColor(IDC_MCASUMMARY_ACPOWER,COLOR_RED);
			datapage->SetColor(IDC_MCADATA_ACPOWER,COLOR_RED);
			if (!m_bACPowerLoss)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34490 STATUS CHANGE - Extenal POWER LOSS (1e)\n\r");
				AnyChange = true;
				m_bACPowerLoss = true;
			}
			Data_Tab |= 0x0002;
		}
		else 
		{
			strcpy(temp,"Ok");
//			summarypage->SetColor(IDC_MCASUMMARY_ACPOWER,COLOR_BLACK);
//			datapage->SetColor(IDC_MCADATA_ACPOWER,COLOR_BLACK);
			if (m_bACPowerLoss)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34500 STATUS CHANGE - External POWER OK (1e)\n\r");
				AnyChange = true;
				m_bACPowerLoss = false;
			}
			Data_Tab &= ~(0x0002);
		}
		summarypage->SetDlgItemText(IDC_MCASUMMARY_ACPOWER,temp);
		datapage->SetDlgItemText(IDC_MCADATA_ACPOWER,temp);
	
		//battery low bit
		if (Status & MCABATTERYLOW) 
		{
			strcpy(temp,"Low");
			summarypage->SetColor(IDC_MCASUMMARY_BATTERYOK,COLOR_RED);
			datapage->SetColor(IDC_MCADATA_BATSTAT,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34510 STATUS CHANGE - BATTERY LOW (1e)\n\r");
				AnyChange = true;
				m_bBatteryOk = false;
			}
			Data_Tab |= 0x0004;
		}
		else
		{
			strcpy(temp,"Ok");
			if (!m_bBatteryOk)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34520 STATUS CHANGE - BATTERY OK (1e)\n\r");
				AnyChange = true;
				m_bBatteryOk = true;
			}
			Data_Tab &= ~(0x0004);
		}
		summarypage->SetDlgItemText(IDC_MCASUMMARY_BATTERYOK,temp);
		datapage->SetDlgItemText(IDC_MCADATA_BATSTAT,temp);

		//Filtering on or off
		if (Status & MCAFILTEROFF) 
		{
			strcpy(temp,"No");
			if (!m_bFiltering)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34530 STATUS CHANGE - FILTERING OFF (1e)\n\r");
				AnyChange = true;
				m_bFiltering = true;
			}
		}
		else
		{
			strcpy(temp,"Yes");
			if (m_bFiltering)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34540 STATUS CHANGE - FILTERING ON (1e)\n\r");
				AnyChange = true;
				m_bFiltering = false;
			}
		}
		datapage->SetDlgItemText(IDC_MCADATA_FILTERING,temp);

		//Spectrum changing
		if (Status & MCASPECTRUMCH) 
		{
			strcpy(temp,"Yes");
			datapage->SetColor(IDC_MCADATA_SPECTCHANG,COLOR_RED);
			if (!m_bSpectChange)//if false
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34550 STATUS CHANGE - SPECTRUM CHANGING (1e)\n\r");
				AnyChange = true;
				m_bSpectChange = true;//don't allow spectrum change messages
			}
			Data_Tab |= 0x10000;
		}
		else
		{
			strcpy(temp,"No");
			if (m_bSpectChange)//if true
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34560 STATUS CHANGE - SPECTRUM NOT CHANGING (1e)\n\r");
				AnyChange = true;
				m_bSpectChange = false;//allow spectrum change messages
			}
			Data_Tab &= ~(0x10000);
		}
		datapage->SetDlgItemText(IDC_MCADATA_SPECTCHANG,temp);

		//Restart Required
		if (Status & MCARESTARTREQ) 
		{
			strcpy(temp,"Yes");
			datapage->SetColor(IDC_MCADATA_RESTART,COLOR_RED);
			if (!m_bRestartReq)//if false
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34570 STATUS CHANGE - RESTART REQUIRED (1e)\n\r");
				AnyChange = true;
				m_bRestartReq = true;
			}
			Data_Tab |= 0x20000;
		}
		else
		{
			strcpy(temp,"No");
			if (m_bRestartReq)//if true
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34580 STATUS CHANGE - RESTART NOT REQUIRED (1e)\n\r");
				AnyChange = true;
				m_bRestartReq = false;
			}
			Data_Tab &= ~(0x20000);
		}
		datapage->SetDlgItemText(IDC_MCADATA_RESTART,temp);
		
		//get bbm error from status byte 1
		bool BBMErrorInS1 = false;
		if (Status & MCABBMERROR)
			BBMErrorInS1 = true;
		else
			BBMErrorInS1 = false;

		//get status byte 2
		strncpy(temp,&Record[14],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 123",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//BBM error
		if (BBMErrorInS1) 
		{
			int BBMErr = Status | MCABBMERRMASK;
			char cBuff[1024];
			switch (BBMErr) 
			{
			case MCABBMBADREAD:
				strcpy(temp,MCABBMBADREAD_TEXT);strcpy(cBuff,"34590");
				break;
			case MCABBMBADWRITE:
				strcpy(temp,MCABBMBADWRITE_TEXT);strcpy(cBuff,"34591");
				break;
			case MCABBMEMPTY:
				strcpy(temp,MCABBMEMPTY_TEXT);strcpy(cBuff,"34592");
				break;
			case MCABBMFULL:
				strcpy(temp,MCABBMFULL_TEXT);strcpy(cBuff,"34593");
				break;
			case MCABBMBADBUFSZ:
				strcpy(temp,MCABBMBADBUFSZ_TEXT);strcpy(cBuff,"34594");
				break;
			case MCABBMBADOPCOD:
				strcpy(temp,MCABBMBADOPCOD_TEXT);strcpy(cBuff,"34595");
				break;
			default:
				strcpy(temp,UNKNOWN_TEXT);strcpy(cBuff,"34596");
			}
			summarypage->SetColor(IDC_MCASUMMARY_BBMOK,COLOR_RED);
			datapage->SetColor(IDC_MCADATA_BBMSTAT,COLOR_RED);
			if (m_bBBMError)
			{	
				strcat(cBuff," STATUS CHANGE - BBM Error: ");
				strcat(cBuff,temp);
				strcat(cBuff," (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, cBuff);
				AnyChange = true;
				m_bBBMError = false;
			}
			Data_Tab |= 0x0008;
		}
		else
		{
			strcpy(temp,"Ok");
//			summarypage->SetColor(IDC_MCASUMMARY_BBMOK,COLOR_BLACK);
//			datapage->SetColor(IDC_MCADATA_BBMSTAT,COLOR_BLACK);
			if (!m_bBBMError)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34600 STATUS CHANGE - BBM No Error (1e)\n\r");
				AnyChange = true;
				m_bBBMError = true;
			}
			Data_Tab &= ~(0x0008);
		}
		summarypage->SetDlgItemText(IDC_MCASUMMARY_BBMOK,temp);
		datapage->SetDlgItemText(IDC_MCADATA_BBMSTAT,temp);

		//MONITOR error
		char cTempNumber[8];
		int MonErr = (Status & MCAMONERRMASK);
		if (MonErr) 
		{
			switch (MonErr) 
			{
			case MCAMONACQFAIL:
				strcpy(temp,MCAMONACQFAIL_TEXT);
				strcpy(cTempNumber,"34610");
				break;
			case MCAMONMONFAIL:
				strcpy(temp,MCAMONMONFAIL_TEXT);
				strcpy(cTempNumber,"34620");
				break;
//			case MCAMONTHVNOTS:
//				strcpy(temp,MCAMONTHVNOTS_TEXT);
//				break;
			case MCAMONHVCFAIL:
				strcpy(temp,MCAMONHVCFAIL_TEXT);
				strcpy(cTempNumber,"34630");
				break;
			case MCAMONGDCFAIL:
				strcpy(temp,MCAMONGDCFAIL_TEXT);
				strcpy(cTempNumber,"34640");
				break;
//			case MCAMONHVNOTST:
//				strcpy(temp,MCAMONHVNOTST_TEXT);
//				break;
			default:
//				strcpy(temp,MCAMONHVNOTSTAB_TEXT);
				strcpy(temp,MCAMONUNKNOWN_TEXT);
				strcpy(cTempNumber,"3465");
			}
			summarypage->SetColor(IDC_MCASUMMARY_MONSTAT,COLOR_RED);
			datapage->SetColor(IDC_MCADATA_MONSTAT,COLOR_RED);
			
			m_pParent->PostMessage(uIDC_RECEIVE,MONITORFAIL_RECORD,CHECKSUM_GOOD);

			if (m_bMonError)
			{
				char temp2[256];
				strcpy(temp2,cTempNumber);
				strcat(temp2," STATUS CHANGE - MONITOR Error: ");
				strcat(temp2,temp);
				strcat(temp2," (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,temp2);
				AnyChange = true;
				m_bMonError = false;
			}
			Data_Tab |= 0x0010;
		}
		else
		{
			strcpy(temp,"Ok");
//			summarypage->SetColor(IDC_MCASUMMARY_MONSTAT,COLOR_BLACK);
//			datapage->SetColor(IDC_MCADATA_MONSTAT,COLOR_BLACK);
			if (!m_bMonError)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34560 STATUS CHANGE - MONITOR No Error (1e)\n\r");
				AnyChange = true;
				m_bMonError = true;
			}
			Data_Tab &= ~(0x0010);
		}
		summarypage->SetDlgItemText(IDC_MCASUMMARY_MONSTAT,temp);
		datapage->SetDlgItemText(IDC_MCADATA_MONSTAT,temp);
			
		//get alarm flags
		strncpy(temp,&Record[16],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 124",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		temp[0] = NULL;
		//alarm U235
		char szTemp1[16];																				//DR356
		szTemp1[0] = NULL;																				//DR356
		generalpage->GetDlgItemText(IDC_STATIC_ALARM1,szTemp1,sizeof(szTemp1));							//DR356
		if (Status & MCAALARMU235)
		{
			strcat(temp,szTemp1);
			strcat(temp," ");
//			strcat(temp,"U235 ");
//			if (!m_bAlarmU235)
//			{
//				if (m_pParent->m_bLogMII)
//					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34570 STATUS CHANGE - ALARM U235 Over Threshold (1e)\n\r");
//				AnyChange = true;
//				m_bAlarmU235 = true;
//			}
			if (!m_bAlarmU235)																			//DR356
			{																							//DR356
				char szTemp2[64];																		//DR356
				sprintf(szTemp2,"34570 STATUS CHANGE - ALARM %s Over Threshold (1b)\n\r",szTemp1);		//DR356
				if (m_pParent->m_bLogMII)																//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, szTemp2);							//DR356
				AnyChange = true;																		//DR356
				m_bAlarmU235 = true;																	//DR356
			}																							//DR356
			Data_Tab |= (0x0020);
		}
		else 
		{
//			if (m_bAlarmU235)
//			{
//				if (m_pParent->m_bLogMII)
//					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34580 STATUS CHANGE - ALARM U235 Under Threshold (1e)\n\r");
//				AnyChange = true;
//				m_bAlarmU235 = false;
//			}
			if (m_bAlarmU235)																			//DR356
			{																							//DR356
				char szTemp2[64];																		//DR356
				sprintf(szTemp2,"34580 STATUS CHANGE - ALARM %s Under Threshold (1e)\n\r",szTemp1);		//DR356
				if (m_pParent->m_bLogMII)																//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, szTemp2);							//DR356
				AnyChange = true;																		//DR356
				m_bAlarmU235 = false;																	//DR356
			}																							//DR356
			Data_Tab &= ~(0x0020);
		}

		//alarm U238
		szTemp1[0] = NULL;																				//DR356
		generalpage->GetDlgItemText(IDC_STATIC_ALARM2,szTemp1,sizeof(szTemp1));							//DR356
		if (Status & MCAALARMU238)
		{
			strcat(temp,szTemp1);																		//DR356
			strcat(temp," ");																			//DR356
			//strcat(temp,"U238 ");																		//DR356
			if (!m_bAlarmU238)
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34590 STATUS CHANGE - ALARM %s Over Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmU238 = true;
			}
			Data_Tab |= (0x0040);
		}
		else 
		{
			if (m_bAlarmU238)
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34600 STATUS CHANGE - ALARM %s Under Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmU238 = false;
			}
			Data_Tab &= ~(0x0040);
		}

		//alarm Cs137
		szTemp1[0] = NULL;																				//DR356
		generalpage->GetDlgItemText(IDC_STATIC_ALARM3,szTemp1,sizeof(szTemp1));							//DR356
		if (Status & MCAALARMCS137)
		{
			strcat(temp,szTemp1);																		//DR356
			strcat(temp," ");																			//DR356
			//strcat(temp,"Cs137 ");																	//DR356
			if (!m_bAlarmCs137)
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34610 STATUS CHANGE - ALARM %s Over Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmCs137 = true;
			}
			Data_Tab |= (0x0080);
		}
		else 
		{
			if (m_bAlarmCs137)		
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];;																	//DR356
					sprintf(szTemp2,"34620 STATUS CHANGE - ALARM %s Under Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmCs137 = false;
			}
			Data_Tab &= ~(0x0080);
		}

		//alarm Scaler
		szTemp1[0] = NULL;																				//DR356
		generalpage->GetDlgItemText(IDC_STATIC_ALARM4,szTemp1,sizeof(szTemp1));							//DR356
		if (Status & MCAALARMSCALER)
		{
			//strcat(temp,"Scaler ");																	//DR356
			strcat(temp,szTemp1);																		//DR356
			strcat(temp," ");																			//DR356
			if (!m_bAlarmScaler)
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34630 STATUS CHANGE - ALARM %s Over Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmScaler = true;
			}
			Data_Tab |= (0x0100);
		}
		else 
		{
			if (m_bAlarmScaler )
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34640 STATUS CHANGE - ALARM %s Under Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmScaler = false;
			}
			Data_Tab &= ~(0x0100);
		}

		//alarm Gross Counts
		szTemp1[0] = NULL;																				//DR356
		generalpage->GetDlgItemText(IDC_STATIC_ALARM5,szTemp1,sizeof(szTemp1));							//DR356
		if (Status & MCAALARMGROSSC)
		{
			//strcat(temp,"Grs. Cnt ");																	//DR356
			strcat(temp,szTemp1);																		//DR356
			strcat(temp," ");																			//DR356
			if (!m_bAlarmGrossCnt)
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34650 STATUS CHANGE - ALARM %s Over Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmGrossCnt = true;
			}
			Data_Tab |= (0x0200);
		}
		else 
		{
			if (m_bAlarmGrossCnt )
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34660 STATUS CHANGE - ALARM %s Under Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmGrossCnt = false;
			}
			Data_Tab &= ~(0x0200);
		}

		//alarm Ratio 1
		szTemp1[0] = NULL;																				//DR356
		generalpage->GetDlgItemText(IDC_STATIC_ALARM6,szTemp1,sizeof(szTemp1));							//DR356
		if (Status & MCAALARMRATIO1)
		{
			//strcat(temp,"Ratio 1 ");																	//DR356
			strcat(temp,szTemp1);																		//DR356
			strcat(temp," ");																			//DR356
			if (!m_bAlarmRatio1)
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34670 STATUS CHANGE - ALARM %s Over Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmRatio1 = true;
			}
			Data_Tab |= (0x0400);
		}
		else 
		{
			if (m_bAlarmRatio1 )
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34680 STATUS CHANGE - ALARM %s Under Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmRatio1 = false;
			}
			Data_Tab &= ~(0x0400);
		}

		//alarm Ratio 2
		szTemp1[0] = NULL;																				//DR356
		generalpage->GetDlgItemText(IDC_STATIC_ALARM7,szTemp1,sizeof(szTemp1));							//DR356
		if (Status & MCAALARMRATIO2)
		{
			//strcat(temp,"Ratio 2 ");
			strcat(temp,szTemp1);
			strcat(temp," ");
			if (!m_bAlarmRatio2)
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34690 STATUS CHANGE - ALARM %s Over Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmRatio2 = true;
			}
			Data_Tab |= (0x0800);
		}
		else 
		{
			if (m_bAlarmRatio2 )
			{
				if (m_pParent->m_bLogMII)
				{
					char szTemp2[64];																	//DR356
					sprintf(szTemp2,"34700 STATUS CHANGE - ALARM %s Under Threshold (1e)\n\r",szTemp1);	//DR356
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,szTemp2);							//DR356
				}
				AnyChange = true;
				m_bAlarmRatio2 = false;
			}
			Data_Tab &= ~(0x0800);
		}

		summarypage->SetColor(IDC_MCASUMMARY_OVERTHRESH,strlen(temp)?COLOR_RED:COLOR_BLACK);
		datapage->SetColor(IDC_MCADATA_OVERTHRESH,strlen(temp)?COLOR_RED:COLOR_BLACK);
//		if (strlen(temp))
//		{
//			summarypage->SetColor(IDC_MCASUMMARY_OVERTHRESH,COLOR_RED);
//			datapage->SetColor(IDC_MCADATA_OVERTHRESH,COLOR_RED);
//		}
		//m_pParent->m_pButton->ShowIcon_Bell(strlen(temp)?VARIANT_FALSE:VARIANT_TRUE);  //July 26 2006 pjm dgp
		m_pParent->m_pButton->ShowIcon_Bell(strlen(temp)?VARIANT_TRUE:VARIANT_FALSE);

		if (strlen(temp) == 0) strcpy(temp,"None");
		summarypage->SetDlgItemText(IDC_MCASUMMARY_OVERTHRESH,temp);
		datapage->SetDlgItemText(IDC_MCADATA_OVERTHRESH,temp);

		//gdc system soh
		strncpy(temp,&Record[18],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 125",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//gdc enabled
		if (Status & MCAGDCENABLE)
		{
			strcpy(temp,"Yes");
			if (!m_bGDCSOHEnable)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34710 STATUS CHANGE - GDC System Enabled (1e)\n\r");
				AnyChange = true;
				m_bGDCSOHEnable = true;
			}
		}
		else 
		{
			strcpy(temp,"No");
			if (m_bGDCSOHEnable)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34720 STATUS CHANGE - GDC System Disabled (1e)\n\r");
				AnyChange = true;
				m_bGDCSOHEnable = false;
			}
		}
		datapage->SetDlgItemText(IDC_MCADATA_GDCENABLE,temp);

		//gdc on hold
		if (Status & MCAGDCONHOLD)
		{
//			if ((m_bGDCSOHOnHold)&&(m_bGDCSOHEnable))
//			{
//				//if time > ? set to red  and issue error
//				if (Julian.TotalSeconds > (m_dGDCOnHold.TotalSeconds() + 60*5))
//				{
//					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34730 STATUS CHANGE - GDC System Enabled -- On Hold > 5 Minutes (1e)\n\r");
//				}
//			}
			strcpy(temp,"Yes");
			if (!m_bGDCSOHOnHold)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34740 STATUS CHANGE - GDC System On Hold (1e)\n\r");
				AnyChange = true;
				m_bGDCSOHOnHold = true;
//				m_dGDCOnHold.Init(Julian.TotalSeconds());
			}
		}
		else 
		{
			strcpy(temp,"No");
			if (m_bGDCSOHOnHold)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34750 STATUS CHANGE - GDC System Not On Hold (1e)\n\r");
				m_bGDCSOHOnHold = false;
//				m_dGDCOnHold.Init(Julian.TotalSeconds());
			}
		}
		datapage->SetDlgItemText(IDC_MCADATA_GDCHOLD,temp);
		if (Status & MCAGDCSOHMASK)
		{
			char cBuffx[8];
			switch (Status & MCAGDCSOHMASK)
			{
			case MCAGDCRPAREA :
				strcpy(temp,MCAGDCRPAREA_TEXT);
				strcpy(cBuffx,"34760");
				break;
			case MCAGDCRPCROWD:
				strcpy(temp,MCAGDCRPCROWD_TEXT);
				strcpy(cBuffx,"34770");
				break;
			case MCAGDCRPANALY:
				strcpy(temp,MCAGDCRPANALY_TEXT);
				strcpy(cBuffx,"34780");
				break;
			case MCAGDCPEAKSEA:
				strcpy(temp,MCAGDCPEAKSEA_TEXT);
				strcpy(cBuffx,"34790");
				break;
			case MCAGDCOORANGE:
				strcpy(temp,MCAGDCOORANGE_TEXT);
				strcpy(cBuffx,"34800");
				break;
			case MCAGDCROIOOLI:
				strcpy(temp,MCAGDCROIOOLI_TEXT);
				strcpy(cBuffx,"34810");
				break;
			case MCAGDCRPOBSCU:
				strcpy(temp,MCAGDCRPOBSCU_TEXT);
				strcpy(cBuffx,"34820");
				break;
			default:
				strcpy(temp,UNKNOWN_TEXT);
				strcpy(cBuffx,"34830");
			}

			summarypage->SetColor(IDC_MCASUMMARY_GDCSTATUS,COLOR_RED);
			datapage->SetColor(IDC_MCADATA_GDCSTATUS,COLOR_RED);
			if (m_bGDCSOHError)
			{
				char temp2[256];
				strcpy(temp2,cBuffx);
				strcat(temp2," STATUS CHANGE - GDC SOH Error: ");
				strcat(temp2,temp);
				strcat(temp2," (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,temp2);
				AnyChange = true;
				m_bGDCSOHError = false;
			}
			Data_Tab |= 0x1000;
		}
		else
		{
			strcpy(temp,"Ok");
//			summarypage->SetColor(IDC_MCASUMMARY_GDCSTATUS,COLOR_BLACK);
//			datapage->SetColor(IDC_MCADATA_GDCSTATUS,COLOR_BLACK);
			if (!m_bGDCSOHError)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "34840 STATUS CHANGE - GDC SOH No Error (1e)\n\r");
				AnyChange = true;
				m_bGDCSOHError = true;
			}
			Data_Tab &= ~(0x1000);
		}
		summarypage->SetDlgItemText(IDC_MCASUMMARY_GDCSTATUS,temp);
		datapage->SetDlgItemText(IDC_MCADATA_GDCSTATUS,temp);

		//hvc system soh
		strncpy(temp,&Record[20],2);
		temp[2] = NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 126",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//hvc enabled
		if (Status & MCAHVCENABLE)
		{
			char temp2[256];
			char cBuffx[8];
			switch (Status & MCAHVCENABLE)
			{
			case MCAHVCAUTO:
				strcpy(temp,MCAHVCAUTO_TEXT);
				strcpy(cBuffx,"34850");
				break;
			case MCAHVCFIXED:
				strcpy(temp,MCAHVCFIXED_TEXT);
				strcpy(cBuffx,"34860");
				break;
			case MCAHVCTEMP:
				strcpy(temp,MCAHVCTFIXED_TEXT);
				strcpy(cBuffx,"34870");
				break;
			default:
				strcpy(temp,UNKNOWN_TEXT);
				strcpy(cBuffx,"34880");
			}
			
			if (!m_bHVCSOHEnable)
			{
				strcpy(temp2,cBuffx);
				strcat(temp2," STATUS CHANGE - HVC System Enabled, ");
				strcat(temp2,temp);
				strcat(temp2, " (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,temp2);
				AnyChange = true;
				m_bHVCSOHEnable = true;
			}
//			Data_Tab |= (0x2000);
		}
		else 
		{
			strcpy(temp,"No");
			if (m_bHVCSOHEnable)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34890 STATUS CHANGE - HVC System Disabled (1e)\n\r");
				AnyChange = true;
				m_bHVCSOHEnable = false;
			}
//			Data_Tab &= ~(0x2000);
		}
		datapage->SetDlgItemText(IDC_MCADATA_HVCENABLE,temp);

		//HVC on hold
		if (Status & MCAHVCONHOLD)
		{
//			if ((m_bHVCSOHOnHold)&&(m_bHVCSOHEnable))
//			{
//				//if time on hold > 5 minutes
//				//set to red and isure error
//				if (Julian.TotalSeconds > (m_dHVCOnHold.TotalSeconds() + 60*5))
//				{
//					summarypage->SetColor(IDC_MCASUMMARY_HVCHOLD,COLOR_RED);
//					datapage->SetColor(IDC_MCADATA_HVCHOLD,COLOR_RED);
//					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34900 STATUS CHANGE - HVC System Enabled -- On Hold > 5 Minutes (1e)\n\r");
//				}
//			}
			strcpy(temp,"Yes");
			if (!m_bHVCSOHOnHold)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34910 STATUS CHANGE - HVC System On Hold (1e)\n\r");
				AnyChange = true;
				m_bHVCSOHOnHold = true;
//				m_dHVCOnHold.Init(Julian.TotalSeconds());
			}
			Data_Tab |= (0x4000);
		}
		else 
		{
			strcpy(temp,"No");
			if (m_bHVCSOHOnHold)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"34920 STATUS CHANGE - HVC System Not On Hold (1e)\n\r");
				AnyChange = true;
				m_bHVCSOHOnHold = false;
//				m_dHVCOnHold.Init(Julian.TotalSeconds());
//				summarypage->SetColor(IDC_MCASUMMARY_HVCHOLD,COLOR_NORMAL);
//				datapage->SetColor(IDC_MCADATA_HVCHOLD,COLOR_NORMAL);
			}
			Data_Tab &= ~(0x4000);
		}
		datapage->SetDlgItemText(IDC_MCADATA_HVCHOLD,temp);

		if (Status & MCAHVCSOHMASK)
		{
			char cBuff[8];
			switch (Status & MCAHVCSOHMASK)
			{
			case MCAHVCTEFFOOR:
				strcpy(temp,MCAHVCTEFFOOR_TEXT);
				strcpy(cBuff,"34930");
				break;
			case MCAHVCTIMMTOO:
				strcpy(temp,MCAHVCTIMMTOO_TEXT);
				strcpy(cBuff,"34940");
				break;
			case MCAHVCTEFFTOO:
				strcpy(temp,MCAHVCTEFFTOO_TEXT);
				strcpy(cBuff,"34950");
				break;
			case MCAHVCISDRIFT:
				strcpy(temp,MCAHVCISDRIFT_TEXT);
				strcpy(cBuff,"34960");
				break;
			case MCAHVCTARNOTR:
				strcpy(temp,MCAHVCTARNOTR_TEXT);
				strcpy(cBuff,"34970");
				break;
			case MCAHVCTIMMOFF:
				strcpy(temp,MCAHVCTIMMOFF_TEXT);
				strcpy(cBuff,"34980");
				break;
			case MCAHVCPARAMCS:
				strcpy(temp,MCAHVCPARAMCS_TEXT);
				strcpy(cBuff,"34990");
				break;
			default:
				strcpy(temp,UNKNOWN_TEXT);
				strcpy(cBuff,"35000");
			}

			summarypage->SetColor(IDC_MCASUMMARY_HVCSTATUS,COLOR_RED);
			datapage->SetColor(IDC_MCADATA_HVCSTATUS,COLOR_RED);
			if (m_bHVCSOHError)
			{
				char temp2[128];
				strcpy(temp2,cBuff);
				strcat(temp2," STATUS CHANGE - HVC SOH Error: ");
				strcat(temp2,temp);
				strcat(temp2," (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,temp2);
				AnyChange = true;
				m_bHVCSOHError = false;
			}
			Data_Tab |= 0x8000;
		}
		else
		{
			strcpy(temp,"Ok");
//			summarypage->SetColor(IDC_MCASUMMARY_HVCSTATUS,COLOR_BLACK);
//			datapage->SetColor(IDC_MCADATA_HVCSTATUS,COLOR_BLACK);
			if (!m_bGDCSOHError)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "35010 STATUS CHANGE - HVC SOH No Error (1e)\n\r");
				AnyChange = true;
				m_bHVCSOHError = true;
			}
			Data_Tab &= ~(0x8000);
		}
		summarypage->SetDlgItemText(IDC_MCASUMMARY_HVCSTATUS,temp);
		datapage->SetDlgItemText(IDC_MCADATA_HVCSTATUS,temp);

		//write to pfm file if there was a change in the status bits
		if (AnyChange)
		{
			char cBuff[1024];
			strcpy(cBuff,"35011 ");
			strcat(cBuff,Record);
			WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//any change to status
		}
	
		//get bbm
		strncpy(temp,&Record[22],6);
		temp[6]=NULL;
		datapage->SetDlgItemText(IDC_MCADATA_BYTESMEM,temp);
		int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
		m_pParent->m_pButton->put_Numeric((ULONG)bbm);
	
		if (Data_Tab != 0)
		{
			SetTabColor(IDC_DATA_TAB,COLOR_RED);
//for debug
//			sprintf(temp,"%8x",Data_Tab);
//			datapage->SetDlgItemText(IDC_MCADATA_BYTESMEM,temp);
//end debug
		}
	
		m_pParent->PostMessage(uIDC_RECEIVE,IDC_BBM_VALUE,bbm);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,MONITOR_ID2_RECORD,CheckSum);	
}

//record 1f
void CMCAPropertySheet::AnalyzeOk(char *Record)
{
	int CheckSum;
	if (LengthOk(MCAANALYZE_OK_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
//		CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
//		CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//		CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CMCACDData*   configpage  = m_pParent->m_pCDData;
//		CMCAModParam* modparpage  = m_pParent->m_pModParams;
		char temp[8];
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		datapage->SetDlgItemText(IDC_MCADATA_BYTESMEM,temp);
		int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
		m_pParent->m_pButton->put_Numeric((ULONG)bbm);
		m_pParent->PostMessage(uIDC_RECEIVE,IDC_BBM_VALUE,bbm);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ANALYZE_OK_RECORD,CheckSum);
}

//record 20
void CMCAPropertySheet::SpectHead(char *Record)
{
	int CheckSum;
	if (LengthOk(MCASPECTHEAD_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	
	if (CheckSum == CHECKSUM_GOOD)
	{
		//get pointers to page 2 and summary page
//		CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
//		CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CMCADataStat* datapage    = m_pParent->m_pDataStatus;
		CMCASpecDataStat* specdatapage = m_pParent->m_pSpecDataStatus;
//		CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CMCAModParam* modparpage  = m_pParent->m_pModParams;

		char temp[32];

 		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10] = NULL;
		m_cSpectJulian.Init(temp);
		//place time into structure

		//get meaning //ascii hex
		strncpy(temp,&Record[12],2);
		temp[2] = NULL;
		//place meaning into structure

		//status byte 1 //ascii hex
		strncpy(temp,&Record[14],2);
		temp[2] = NULL;
		//place status 1 into structure

		//status byte 2 //ascii hex
		strncpy(temp,&Record[16],2);
		temp[2] = NULL;
		//place status 2 into structure

		//alarm status byte //ascii hex
		strncpy(temp,&Record[18],2);
		temp[2] = NULL;
		//place alarm status byte into structure

		//get number of channels in spectrum //ascii
		strncpy(temp,&Record[20],4);
		temp[4] = NULL;
//try{
		sscanf(temp,"%x",&m_iSpectNumChan);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 127",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//place number of channels into structure

		//get number of data records to be received
		strncpy(temp,&Record[24],2);
		temp[2] = NULL;

		//zero out the spectrum buffer
		memset(m_uiSpectrum,NULL,sizeof(m_uiSpectrum));

		if (m_bReceivingSpectra)
		{
			WriteToFile(TO_CEV | TO_PFM, TYPE_INST,NULL,"35035 SECOND SPECTRUM HEAD RECEIVED WITHOUT INTERVENING TAIL MESSAGE\n\r");
			if (m_pParent->m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35035 SECOND SPECTRUM HEAD RECEIVED WITHOUT INTERVENING TAIL MESSAGE\n\r");
		}
		m_bReceivingSpectra = true;
		CString cString;
		cString.Format("Receiving Spectrum: %s %s",m_cSpectJulian.Date(), m_cSpectJulian.Time());
		specdatapage->SetReceivingSpectraName(cString);
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35032 SPECTRUM HEAD RECORD\n\r");
	}
	m_pParent->PostMessage(uIDC_RECEIVE,SPECT_HEAD_RECORD,CheckSum);
}

//record 21
void CMCAPropertySheet::SpectData(char *Record)
{
	int CheckSum;
	UINT uiLength;
	UINT uiSize;
	UINT uiCount;
	UINT uiStartAddress;
	char cTemp[32];
	CheckSum = CHECKSUM_BAD;

//	CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
//	CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CMCADataStat* datapage    = m_pParent->m_pDataStatus;
	CMCASpecDataStat* specdatapage    = m_pParent->m_pSpecDataStatus;
//	CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CMCAModParam* modparpage  = m_pParent->m_pModParams;

	if (m_bReceivingSpectra)
	{
		if (strlen(Record) > 13)
		{
			strncpy(cTemp,&Record[2],4);
			cTemp[4] = NULL;
//try{
			sscanf(cTemp,"%x",&uiStartAddress);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 128",MB_OK);
//	AfxMessageBox(cTemp,MB_OK);
//}
			strncpy(cTemp,&Record[6],2);
			cTemp[2] = NULL;
//try{
			sscanf(cTemp,"%2x",&uiCount);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 129",MB_OK);
//	AfxMessageBox(cTemp,MB_OK);
//}
			strncpy(cTemp,&Record[8],1);
			cTemp[1] = NULL;
//try{
			sscanf(cTemp,"%x",&uiSize);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 130",MB_OK);
//	AfxMessageBox(cTemp,MB_OK);
//}
			uiSize *=2;
//			UINT uiLengthTemp = strlen(Record);
			uiLength = uiCount * uiSize + 13;

			if (LengthOk(uiLength,Record))
				CheckSum = Checksum(Record);
			else
				CheckSum = CHECKSUM_BAD;
			if (CheckSum == CHECKSUM_GOOD)
			{
				//place data into array
				for (UINT i = 0; i < uiCount; i++)
				{
					strncpy(cTemp,&Record[9+(i*uiSize)],uiSize);
					cTemp[uiSize] = NULL;
//try{
					sscanf(cTemp,"%x",&m_uiSpectrum[uiStartAddress + i]);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 131",MB_OK);
//	AfxMessageBox(cTemp,MB_OK);
//}
				}

				if (m_pParent->m_bDoDump)
					WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35033 SPECTRUM DATA RECORD\n\r");
			}
		}
		specdatapage->m_XYCtrl.m_crPixelColor = COLOR_DARKGREEN;
		specdatapage->ProcessSpectrumBufferForRealTimeGraphic(m_uiSpectrum, m_iSpectNumChan);
	}
	else
	{
		WriteToFile(TO_CEV | TO_PFM, TYPE_INST,NULL,"35034 SPECTRUM DATA RECEIVED WITHOUT HEADER MESSAGE\n\r");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35034 SPECTRUM DATA RECEIVED WITHOUT HEADER MESSAGE\n\r");
	}
	m_pParent->PostMessage(uIDC_RECEIVE,SPECT_DATA_RECORD,CheckSum);

}

//record 22
void CMCAPropertySheet::SpectTail(char *Record)
{
	int CheckSum;
	if (LengthOk(MCASPECTTAIL_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (m_bReceivingSpectra)
	{
		if (CheckSum == CHECKSUM_GOOD)
		{
	//		CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
	//		CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	//		CMCADataStat* datapage    = m_pParent->m_pDataStatus;
			CMCASpecDataStat* specdatapage    = m_pParent->m_pSpecDataStatus;
	//		CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
	//		CMCAModParam* modparpage  = m_pParent->m_pModParams;

			if (m_pParent->m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35031 SPECTRUM TAIL RECORD\n\r");

			//write to chn file
			COleDateTime t1;

			t1.SetDateTime( m_cSpectJulian.Year(), m_cSpectJulian.Month(), m_cSpectJulian.Day(), 
			      m_cSpectJulian.Hour(), m_cSpectJulian.Minute(), m_cSpectJulian.Second() );
			t1 += HALF_SECOND;

			CString cString;
			if (t1.GetStatus()==COleDateTime::valid)
				cString.Format("Most Recent Spectrum: %s",t1.Format("%Y.%m.%d %H:%M:%S"));
			else
			{
				cString.Format("Most Recent Spectrum: INVALID TIME RECEIVED");
				t1 = COleDateTime::GetCurrentTime();
			}
			CString cResult;
			BuildFileName(cResult,m_pParent->m_csLongNameFormatString,"","MC",
				m_pParent->m_pID ,
				t1.GetYear(),t1.GetMonth(),t1.GetDay(),
				t1.GetHour(),t1.GetMinute(),t1.GetSecond(),
				"");
			specdatapage->SetReceivingSpectraName(cResult);
			
			CString cFile;
			int iSpectrumFileType = GetPrivateProfileInt(
					m_pParent->m_pName,"SPECTRAFILETYPE",0,m_pParent->m_szIniFile);
			
			switch (iSpectrumFileType)
			{
				case 1: //.csv
					{
						cFile.Format("%s\\%s.%s",
							m_pParent->m_szSaveLoc,	
							cResult,
							"csv");
							
						specdatapage->AddNewSpectraName(cFile);

						FILE* outfile;
						if ((outfile = fopen(cFile,"wt"))!=0)
						{
							for (int i=0,j=0; i < m_iSpectNumChan; i++,j++)
							{
								if (j == 32)
								{
									j = 0;
									fprintf(outfile,"\n%u,",m_uiSpectrum[i]);
								}
								else
									fprintf(outfile,"%u,",m_uiSpectrum[i]);
							
							}
							fclose(outfile);
							CString cMsg;
							cMsg.Format("35036 WRITE SPECTRUM FILE: %s\n\r",cFile);
							WriteToFile(TO_PFM, TYPE_INST,NULL,(LPSTR)LPCSTR(cMsg));
							if (m_pParent->m_bDoDump)
								WriteToFile(TO_DMP, TYPE_DUMP, NULL,(LPSTR)LPCSTR(cMsg));
						}
						else
						{
							CString cMsg;
							cMsg.Format("35036 WRITE SPECTRUM FILE FAILED: %s\n\r",cFile);
							WriteToFile(TO_CEV | TO_PFM, TYPE_INST,NULL,(LPSTR)LPCSTR(cMsg));
							if (m_pParent->m_bDoDump)
								WriteToFile(TO_DMP, TYPE_DUMP, NULL, (LPSTR)LPCSTR(cMsg));
						}
					}
					break;

				case 2:	//.txt
					{
						cFile.Format("%s\\%s.%s",
							m_pParent->m_szSaveLoc,	
							cResult,
							"txt");

						specdatapage->AddNewSpectraName(cFile);

						FILE* outfile;
						if ((outfile = fopen(cFile,"wt"))!=0)
						{
							//fprintf(outfile,"%s\n",cResult);
							for (int i=0; i < m_iSpectNumChan; i++)
								fprintf(outfile,"%u\n",m_uiSpectrum[i]);
							fclose(outfile);
							CString cMsg;
							cMsg.Format("35036 WRITE SPECTRUM FILE: %s\n\r",cFile);
							WriteToFile(TO_PFM, TYPE_INST,NULL,(LPSTR)LPCSTR(cMsg));
							if (m_pParent->m_bDoDump)
								WriteToFile(TO_DMP, TYPE_DUMP, NULL,(LPSTR)LPCSTR(cMsg));

						}
						else
						{
							CString cMsg;
							cMsg.Format("35036 WRITE SPECTRUM FILE FAILED: %s\n\r",cFile);
							WriteToFile(TO_CEV | TO_PFM, TYPE_INST,NULL,(LPSTR)LPCSTR(cMsg));
							if (m_pParent->m_bDoDump)
								WriteToFile(TO_DMP, TYPE_DUMP, NULL, (LPSTR)LPCSTR(cMsg));
						}
					}
					break;

				default://.chn
				{
					cFile.Format("%s\\%s.%s",
						m_pParent->m_szSaveLoc,	
						cResult,
						"chn");

					specdatapage->AddNewSpectraName(cFile);
				
//					long     Live=0L;
//					long     True=0L;

					CChnSpcEx   Spec1(m_iSpectNumChan, m_uiSpectrum);
					Spec1.SetNumChans(m_iSpectNumChan);
					Spec1.SetTime(t1.GetYear(),t1.GetMonth(),t1.GetDay(),t1.GetHour(),t1.GetMinute(),t1.GetSecond(),0);

					if (!Spec1.Write((char*)LPCSTR(cFile), NULL, 0))
					{
						CString cMsg;
						cMsg.Format("35036 WRITE SPECTRUM FILE FAILED: %s\n\r",cFile);
						WriteToFile(TO_CEV | TO_PFM, TYPE_INST,NULL,(LPSTR)LPCSTR(cMsg));
						if (m_pParent->m_bDoDump)
							WriteToFile(TO_DMP, TYPE_DUMP, NULL, (LPSTR)LPCSTR(cMsg));
					}
					else 
					{
						CString cMsg;
						cMsg.Format("35036 WRITE SPECTRUM FILE: %s\n\r",cFile);
						WriteToFile(TO_PFM, TYPE_INST,NULL,(LPSTR)LPCSTR(cMsg));
						if (m_pParent->m_bDoDump)
							WriteToFile(TO_DMP, TYPE_DUMP, NULL,(LPSTR)LPCSTR(cMsg));
					}

				}
			}
		}
	}
	else
	{
		WriteToFile(TO_CEV | TO_PFM, TYPE_INST,NULL,"35035 SPECTRUM TAIL RECEIVED WITHOUT HEADER MESSAGE\n\r");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35035 SPECTRUM TAIL RECEIVED WITHOUT HEADER MESSAGE\n\r");
	}
	m_bReceivingSpectra = false;
	m_pParent->PostMessage(uIDC_RECEIVE,SPECT_TAIL_RECORD,CheckSum);
}


//record 30
void CMCAPropertySheet::InformRec(char *Record)
{
	int CheckSum;
	if (LengthOk(MCAINFORMATION_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[32];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);
		char cBuff[1024];
		strcpy(cBuff,"35020 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//InformRec

		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35030 INFORMATION RECORD\n\r");
	}
	m_pParent->PostMessage(uIDC_RECEIVE,INFORMATION_RECORD,CheckSum);
}

//record 10
void CMCAPropertySheet::Config1Rec(char *Record)
{
	int CheckSum;
	int To;
	if (LengthOk(MCACONFIG1_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		To = TO_PFM;
		char temp[256];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		CMCACDData*   configpage  = m_pParent->m_pCDData;
		configpage->m_StaticConfig10.SetWindowText(Record);
		configpage->m_RadioConfig10.SetCheck(0);

		strncpy(temp,&Record[12],MCACONFIG1_RECORD_LENGTH-12-4);
		temp[MCACONFIG1_RECORD_LENGTH-12-4] = NULL;
		if (!m_LastConfig10.IsEmpty())
		{
			if (m_LastConfig10 != temp)
			{
				To = (TO_CEV | TO_PFM);
				WriteToFile(To, TYPE_INST, &Julian, "35040 CONFIGURATION CHANGE - CONFIG 10 (10)\n\r");
				configpage->SetColor(IDC_MCACD_CONFIG10,COLOR_RED);
				SetTabColor(IDC_CDDATA_TAB,COLOR_RED);
			}
		}
		m_LastConfig10 = temp;

		char cBuff[1024];
		strcpy(cBuff,"35041 ");
		strcat(cBuff,Record);
		WriteToFile(To, TYPE_INST, &Julian, cBuff);//config Rec

		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35050 CONFIG 10 RECORD\n\r");
	}
	m_pParent->PostMessage(uIDC_RECEIVE,MCACONFIG1_RECORD,CheckSum);
}

//record 11
void CMCAPropertySheet::Config2Rec(char *Record)
{
	int CheckSum;
	int To;
	if (LengthOk(MCACONFIG2_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		To = TO_PFM;
		char temp[256];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		CMCACDData*   configpage  = m_pParent->m_pCDData;
		configpage->m_StaticConfig11.SetWindowText(Record);
		configpage->m_RadioConfig11.SetCheck(0);

		strncpy(temp,&Record[12],MCACONFIG2_RECORD_LENGTH-12-4);
		temp[MCACONFIG2_RECORD_LENGTH-12-4] = NULL;
		if (!m_LastConfig11.IsEmpty())
		{
			if (m_LastConfig11 != temp)
			{
				To = (TO_CEV | TO_PFM);
				WriteToFile(To, TYPE_INST, &Julian, "35055 CONFIGURATION CHANGE - CONFIG 11 (11)\n\r");
				configpage->SetColor(IDC_MCACD_CONFIG11,COLOR_RED);
				SetTabColor(IDC_CDDATA_TAB,COLOR_RED);
			}
		}
		m_LastConfig11 = temp;

		char cBuff[1024];
		strcpy(cBuff,"35060 ");
		strcat(cBuff,Record);
		WriteToFile(To, TYPE_INST, &Julian, cBuff);//config 11 Rec

		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35070 CONFIG 11 RECORD\n\r");
	}
	m_pParent->PostMessage(uIDC_RECEIVE,MCACONFIG2_RECORD,CheckSum);
}

//record 12
void CMCAPropertySheet::Config3Rec(char *Record)
{
	int CheckSum;
	int To;
	if (LengthOk(MCACONFIG3_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		To = TO_PFM;
		char temp[256];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		CMCACDData*   configpage  = m_pParent->m_pCDData;
		configpage->m_StaticConfig12.SetWindowText(Record);
		configpage->m_RadioConfig12.SetCheck(0);

		strncpy(temp,&Record[12],MCACONFIG3_RECORD_LENGTH-12-4);
		temp[MCACONFIG3_RECORD_LENGTH-12-4] = NULL;
		if (!m_LastConfig12.IsEmpty())
		{
			if (m_LastConfig12 != temp)
			{
				To = (TO_CEV | TO_PFM);
				WriteToFile(To, TYPE_INST, &Julian, "35080 CONFIGURATION CHANGE - CONFIG 12 (12)\n\r");
				configpage->SetColor(IDC_MCACD_CONFIG12,COLOR_RED);
				SetTabColor(IDC_CDDATA_TAB,COLOR_RED);
			}
		}
		m_LastConfig12 = temp;

		char cBuff[1024];
		strcpy(cBuff,"35090 ");
		strcat(cBuff,Record);
		WriteToFile(To, TYPE_INST, &Julian, cBuff);//config 12 Rec

		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35100 CONFIG 12 RECORD\n\r");
	}
	m_pParent->PostMessage(uIDC_RECEIVE,MCACONFIG3_RECORD,CheckSum);
}

//record 13
void CMCAPropertySheet::Config4Rec(char *Record)
{
	int CheckSum;
	int To;
	if (LengthOk(MCACONFIG4_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		To = TO_PFM;
		char temp[256];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		CMCACDData*   configpage  = m_pParent->m_pCDData;
		configpage->m_StaticConfig13.SetWindowText(Record);
		configpage->m_RadioConfig13.SetCheck(0);

		strncpy(temp,&Record[12],MCACONFIG4_RECORD_LENGTH-12-4);
		temp[MCACONFIG4_RECORD_LENGTH-12-4] = NULL;
		if (!m_LastConfig13.IsEmpty())
		{
			if (m_LastConfig13 != temp)
			{
				To = (TO_CEV | TO_PFM);
				WriteToFile(To, TYPE_INST, &Julian, "35110 CONFIGURATION CHANGE - CONFIG 13 (13)\n\r");
				configpage->SetColor(IDC_MCACD_CONFIG13,COLOR_RED);
				SetTabColor(IDC_CDDATA_TAB,COLOR_RED);
			}
		}
		m_LastConfig13 = temp;

		char cBuff[1024];
		strcpy(cBuff,"35120 ");
		strcat(cBuff,Record);
		WriteToFile(To, TYPE_INST, &Julian, cBuff);//InformRec

		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35130 CONFIG 13 RECORD\n\r");
	}
	m_pParent->PostMessage(uIDC_RECEIVE,MCACONFIG4_RECORD,CheckSum);
}

//record 14
void CMCAPropertySheet::Config5Rec(char *Record)
{
	int CheckSum;
	int To;
	if (LengthOk(MCACONFIG5_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		To = TO_PFM;
		char temp[256];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		CMCACDData*   configpage  = m_pParent->m_pCDData;
		configpage->m_StaticConfig14.SetWindowText(Record);
		configpage->m_RadioConfig14.SetCheck(0);

		strncpy(temp,&Record[12],MCACONFIG5_RECORD_LENGTH-12-4);
		temp[MCACONFIG5_RECORD_LENGTH-12-4] = NULL;
		if (!m_LastConfig14.IsEmpty())
		{
			if (m_LastConfig14 != temp)
			{
				To = (TO_CEV | TO_PFM);
				WriteToFile(To, TYPE_INST, &Julian, "35140 CONFIGURATION CHANGE - CONFIG 14 (14)\n\r");
				configpage->SetColor(IDC_MCACD_CONFIG14,COLOR_RED);
				SetTabColor(IDC_CDDATA_TAB,COLOR_RED);
			}
		}
		m_LastConfig14 = temp;

		char cBuff[1024];
		strcpy(cBuff,"35150 ");
		strcat(cBuff,Record);
		WriteToFile(To, TYPE_INST, &Julian, cBuff);//InformRec

		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35160 CONFIG 14 RECORD\n\r");
	}
	m_pParent->PostMessage(uIDC_RECEIVE,MCACONFIG5_RECORD,CheckSum);
}

int 
CMCAPropertySheet::Checksum(char* Record)
{
    unsigned short nbytes;	/* # bytes to include in checksum calculation */
    unsigned short sum;		/* sum of all bytes in checksum */
	unsigned short rec_sum;
    /* checksum is a one byte sum of all bytes up to the checksum byte */
	sum = 0;
    nbytes = (unsigned short)(strlen(Record)-4);//don't count lf/cr nor checksum
	for (int i=0; i<nbytes; i++)
		sum = (unsigned short)(sum + Record[i]);
	sum &= 0x00ff;	/* keep only relevant part of checksum */
//try{
	sscanf(&Record[nbytes],"%2hx",&rec_sum);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 132",MB_OK);
//	AfxMessageBox(&Record[nbytes],MB_OK);
//}
	if (rec_sum == sum)
		/* calculated checksum matches checksum in record */
		return CHECKSUM_GOOD;
//	else checksum is bad
//	CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
	CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//	CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CMCAModParam* modparpage  = m_pParent->m_pModParams;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("Checksum: %d/%H:%M:%S\n");
	DateTime += Record;
	DateTime = DateTime.Left(DateTime.GetLength()-2);
	generalpage->SetDlgItemText(IDC_MCAGENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
	generalpage->SetColor(IDC_MCAGENERAL_CHECKSUM,COLOR_RED);
//	m_pParent->FlagComm(1,false);			

	SetTabColor(IDC_MCAGENERAL_TAB,COLOR_RED);

	WriteToFile(TO_PFM, TYPE_COMP, NULL, "35170 CHECKSUM ERROR\n\r");
	
	char cBuff[1024];
	strcpy(cBuff,"35180 ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

	if (m_pParent->m_bDoDump)
	{
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35170 CHECKSUM ERROR\n\r");
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
	}

    return CHECKSUM_BAD;	       /* checksums do not match */
}

bool 
CMCAPropertySheet::LengthOk(int length, char* Record)
{
	if (strlen(Record) == (unsigned)length)
		return true;

//	CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
//	CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//	CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CMCAModParam* modparpage  = m_pParent->m_pModParams;
	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("Length: %d/%H:%M:%S\n");
	DateTime += Record;
	DateTime = DateTime.Left(DateTime.GetLength()-2);
/*
	generalpage->SetDlgItemText(IDC_MCAGENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
	generalpage->SetColor(IDC_MCAGENERAL_CHECKSUM,COLOR_RED);
	generalpage->GetDlgItem(IDC_MCAGENERAL_CHECKSUM)->Invalidate();
//	m_pParent->FlagComm(1,false);			
	SetTabColor(IDC_MCAGENERAL_TAB,COLOR_RED);
*/
	WriteToFile(TO_PFM, TYPE_COMP, NULL, "35190 LENGTH ERROR\n\r");
	char cBuff[1024];
	strcpy(cBuff,"35200 ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

	if (m_pParent->m_bDoDump)
	{
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "35190 LENGTH ERROR\n\r");
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
	}

    return false;
}

//this function should go into the parent class
void 
CMCAPropertySheet::BuildFileName(CString &cResult,
	CString cFormat,CString cPath,CString cType,
	CString cID,
	int iYear,	int iMonth,	int iDay,
	int iHour,	int iMinute,int iSecond,
	CString cSuffix
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
	if (cSuffix.GetLength())
	{
		if ((cResult.Right(1) != '.') && (cSuffix.Left(1) != '.'))
			cResult += '.';
		cResult += cSuffix;
	}
}



bool CMCAPropertySheet::WriteToMCAFile(CJulianTime* Time, MCAAcquireRecord* MCAAcquireRec)
{
	FILE *File;
	fpos_t position;
	struct MCAAcquireHeader MCAAcquireHead;

	if (m_pParent->m_bBlockWrite) 
		return false;
	
	//build filename
	if (m_pParent->m_bUseShortFilename)
	{
		sprintf(m_szCurrentMCAFileName,"%s\\%s%c%c%c%c%02d.%3s",
//		sprintf(m_szCurrentMCAFileName,"%s\\%s%c%c%c%02d%02d.%3s",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((Time->Year()-1990)<10)?
					((Time->Year()-1990<0)?'#':'0'+(Time->Year()-1990)):
					'A'+(Time->Year()-2000),
	//			'0' + (Time->Year() % 10),
				((Time->Month()<10)?
					'0'+(Time->Month()):
					'A'+(Time->Month()-10)),
				((Time->Day()  <10)?
					'0'+(Time->Day()):
					'A'+(Time->Day()  -10)),
				'A',0,MCA_SUFFIX);
//				('A'+Time->Hour()),  //%c #4
//				Time->Minute(),    //%02d #1
//				Time->Second(),    //%02d #2
//				MCA_SUFFIX);
	}
	else
	{
		CString cResult;
		BuildFileName(cResult,
			m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,"AD",
			m_pParent->m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,MCA_SUFFIX);
		strcpy(m_szCurrentMCAFileName,cResult);
	}

	//is the file there
	//'0' checks for existance only
	if (access(m_szCurrentMCAFileName,0)!=0)
	{
		//make sure path exists
		m_pParent->MakeMyPath(m_szCurrentMCAFileName);

		//build archive file name
		CString ArchiveName;
		if (m_pParent->m_bUseShortFilename)
		{
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((Time->Year()-1990)<10)?
					((Time->Year()-1990<0)?'#':'0'+(Time->Year()-1990)):
					'A'+(Time->Year()-2000),
	//			'0' + (Time->Year() % 10),
				((Time->Month()<10)?
					'0'+(Time->Month()):
					'A'+(Time->Month()-10)),
				((Time->Day()  <10)?
					'0'+(Time->Day()):
					'A'+(Time->Day()  -10)),
				'A',0,MCA_SUFFIX);
		}
		else
		{
			CString cTemp;
			cTemp = m_pParent->m_szSaveLoc;
			cTemp += "\\archive\\";
			BuildFileName(ArchiveName,
				m_pParent->m_csLongNameFormatString,cTemp,"AD",
				m_pParent->m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,MCA_SUFFIX);
		}
		//if the file is in the archive directory
		if (access(ArchiveName,0)==0)
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentMCAFileName,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	}

	int Attempts = 5;
	File = NULL;
	while ((Attempts >= 0) && (File == NULL))
	{
		File = fopen(m_szCurrentMCAFileName,"ab+");
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
				memset( &MCAAcquireHead, ' ', sizeof(MCAAcquireHeader));

				//fill header
				//size of header beyond first four bytes
				sprintf(temp,"%4d",sizeof(MCAAcquireHeader)-4);
				memcpy(MCAAcquireHead.SizeOfHeader,temp,4);

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
//	AfxMessageBox("Error at 133",MB_OK);
//	AfxMessageBox(m_pParent->m_csVersion,MB_OK);
//}

				sprintf(temp,"%2d%1d%1d%1d",sA,sB,sC,sD);  //no decimal point
				//End fix of 8/5/2005

				memcpy(MCAAcquireHead.Version,temp,5);
				sprintf(temp,"%3s",m_szCurrentID);
				memcpy(MCAAcquireHead.StationId,temp,3);
				sprintf(temp,"%3d",Time->Year()%100);
				memcpy(MCAAcquireHead.Year,temp,3);
				sprintf(temp,"%3d",Time->Month());
				memcpy(MCAAcquireHead.Month,temp,3);
				sprintf(temp,"%3d",Time->Day());
				memcpy(MCAAcquireHead.Day,temp,3);
				sprintf(temp,"%4d",Time->Year());
				memcpy(MCAAcquireHead.unused2,temp,4);
				//write header
				fwrite(&MCAAcquireHead,sizeof(MCAAcquireHeader),1,File);
			}
		}

		//send record to file
		int iWriteResult;
		if ((iWriteResult = fwrite(MCAAcquireRec,sizeof(MCAAcquireRecord),1,File))!=1)
		{
			//if failed it could be because MicXfer is trying to
			//clear the archive bit, so wait a short time and try
			//again.
			Sleep(100);
			if ((iWriteResult = fwrite(MCAAcquireRec,sizeof(MCAAcquireRecord),1,File))!=1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(200);
				if ((iWriteResult = fwrite(MCAAcquireRec,sizeof(MCAAcquireRecord),1,File))!=1)
				{
					//if failed on write then show a file write problem
					//on the instrument's button
					m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
					m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
					WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "35210 Could not write to MCA file\n\r");
					//close the file anyway
					fclose(File);
					//return write failure
					return false;
				}
			}
		}
		if ((iWriteResult == 1) && m_bCouldNotOpenBinaryFile)
		{
			m_bCouldNotOpenBinaryFile = false;
			WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "35220 Successful write to MCA file\n\r");
		}
		//close file
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
		WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "35230 Could not open MCA file\n\r");
	}
	return false;
}


void CMCAPropertySheet::WriteToFile(int ToWhom, int Type, CJulianTime* Time, char* Msg)
{
	FILE *File;
	char Message[256];
	fpos_t position;
	Message[0] = NULL;
	COleDateTime TC = COleDateTime::GetCurrentTime(); 		//computer time
	COleDateTime TG;
	COleDateTimeSpan TS;
	CString ArchiveFileName;

	if (m_pParent->m_bBlockWrite) 
		return;

	//default to the provided MCA time
	if (Time)
	{
		int Year,Month,Day,Hour,Minute,Second;
		Year = Time->Year();
		Month = Time->Month();
		Day = Time->Day();
		Hour = Time->Hour();
		Minute = Time->Minute();
		Second = Time->Second();
		if (Year	< 100 ) Year	= 1900;//COleDateTime limits year to 100-9999
		if (Year    > 9999) Year    = 9999;//COleDateTime limits year to 100-9999
		if (Month	< 1)	Month	= 1;
		if (Month   > 12)   Month   = 12;
		if (Day		< 1)	Day		= 1;
		if (Day     > 31)   Day     = 31;
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

	//make sure directory exists
//	CreateDirectory(m_pParent->m_szSaveLoc,NULL);

	if (ToWhom == TO_DMP)
	{
		
		if (m_pParent->m_bUseShortFilename)
		{
			//	build new file name and save it
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
	//			'0' + (TC.GetYear() % 10),
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
				m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,"AD",
				m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,".");
			strcpy(m_szCurrentFileName,cResult);
		}
	}
	else
	{
		//	build new file name and save it
		if (m_pParent->m_bUseShortFilename)
		{
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((TG.GetYear()-1990)<10)?
					((TG.GetYear()-1990<0)?'#':'0'+(TG.GetYear()-1990)):
					'A'+(TG.GetYear()-2000),
	//			'0' + (TG.GetYear() % 10),
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
				m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,"AD",
				m_pParent->m_pID,TG.GetYear(),TG.GetMonth(),TG.GetDay(),0,0,0,".");
			strcpy(m_szCurrentFileName,cResult);
		}
	}	

	//format string to send
	switch (Type){

	case TYPE_DUMP:
		sprintf(Message,"%4d.%02d.%02d %02d:%02d:%02d %s",
			TC.GetYear(),TC.GetMonth(),TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),
			Msg);
//		strcpy(Message,Msg);
		break;

	case TYPE_INST:
		//	message = Time::YY.MM.DD HH:MM:SS G (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d M %s",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),		TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"\n\r":Msg);
		break;

	case TYPE_COMP:
		//	message = MICTIME::YY.MM.DD HH:MM:SS C (MESSAGE==NULL)?\r:MESSAGE
		
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C %s",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),Msg[0]==NULL?"\n\r":Msg);
		break;

	case TYPE_GID2:
	case TYPE_TIME:
		//	message = Time::YY.MM.DD HH:MM:SS MCA Time   "+
		//					"YY.MM.DD HH:MM:SS Computer Time   C - M = xxxx\r"
		//computer time
		
		//MCA time
		TS = TC - TG;  // Subtract 2 COleDateTimes
		TS += HALF_SECOND;

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d M 35250 MCA Time   %04d.%02d.%02d %02d:%02d:%02d Computer Time   C - M = %.0f seconds\n\r",
			TG.GetYear(),	
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),		TG.GetMinute(),	TG.GetSecond(),
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),		TC.GetMinute(),	TC.GetSecond(),
			TS.GetTotalSeconds());
		break;

	case TYPE_INVTIME:
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 35260 INVALID TIME %04d.%02d.%02d %02d:%02d:%02d\n\r",
			((CJulianTime*)Msg)->Year(),	((CJulianTime*)Msg)->Month(),	((CJulianTime*)Msg)->Day(),
			((CJulianTime*)Msg)->Hour(),		((CJulianTime*)Msg)->Minute(),	((CJulianTime*)Msg)->Second(),
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),		TG.GetMinute(),	TG.GetSecond());
		break;

	case TYPE_START:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C MCA COLLECT Version %s<VERSION> started\r"
		
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 35270 MCA COLLECT Version %s started\n\r",
			TC.GetYear(),		
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion);
		break;

	case TYPE_ABNORMAL:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C MCA COLLECT Version %s<VERSION> started\r"
		
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 35280 MCA COLLECT Version %s started from abnormal shutdown\n\r",
			TC.GetYear(),		
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion);
		break;

	case TYPE_DELETE:
	//	message = MICNOW::YY.MM.DD HH:MM:SS C file %s<MESSAGE> deleted\r"
		
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 35290 File %s deleted\n\r",
		TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
		break;

	//just in case
	default:
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 35300 Unknown TYPE %s\n\r",
			TC.GetYear(),	
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),		TC.GetMinute(),	TC.GetSecond(),
			Msg);
	}

	//if to dmp do the write to todays file and get out
	if (ToWhom == TO_DMP)
	{
		//	open filename+cev
		CString fn(m_szCurrentFileName);
		fn += DMP_SUFFIX;
//		CString ArchiveFileNameEx = ArchiveFileName + DMP_SUFFIX;
		if (access(fn,0) != 0)
		{
//			if (access(ArchiveFileNameEx,0)!=-1)
//				MoveFileEx(ArchiveFileNameEx,fn,
//					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			//make sure path exists
			m_pParent->MakeMyPath(fn);
		}

		if ((File = fopen(fn,"ab+")) != NULL)
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
			m_pParent->MakeMyPath(fn);

			if (m_pParent->m_bUseShortFilename)
			{
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_pParent->m_szSaveLoc,
				m_pParent->m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
	//			'0' + (TC.GetYear() % 10),
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
					m_pParent->m_csLongNameFormatString,cTemp,"AD",
					m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,CEV_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"ab+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos(File,&position);
			if (position==0)
			{
				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
//				m_bBatteryTolOk	= true;
				m_bP15VoltOk	= true;
				m_bM15VoltOk	= true;
				m_bACPowerOk	= true;
				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
				m_bCountRateTamperOk = true;
				m_bRMSSigmaTamperOk  = true;
				m_bSpectChange  = false;
				m_bRestartReq   = false;
				m_bInMIIU235	= false;
				m_bInMIIU238	= false;
				m_bInMIICs137	= false;
				m_bInMIIScaler	= false;
				m_bDataFiltered	= false;
				m_pParent->m_dBreakSentCount = 0;
				m_pParent->m_dTimeOutCount = 0;
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
		//	open filename+cev
		CString fn(m_szCurrentFileName);
		fn += PFM_SUFFIX;

		CString ArchiveFileNameExt = ArchiveFileName + PFM_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (access(fn,0) != 0)
		{
			//make sure path exists
			m_pParent->MakeMyPath(fn);

			//check if it is in the archive directory
			//build file name
			if (m_pParent->m_bUseShortFilename)
			{
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_pParent->m_szSaveLoc,
				m_pParent->m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
	//			'0' + (TC.GetYear() % 10),
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
				cTemp = m_pParent->m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,
					m_pParent->m_csLongNameFormatString,cTemp,"AD",
					m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,PFM_SUFFIX);
			}
			//if it exists in the subdirectory "archive" then move it and use it
			if (access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"ab+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos(File, &position );
			if (position==0)
			{
				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
//				m_bBatteryTolOk	= true;
				m_bP15VoltOk	= true;
				m_bM15VoltOk	= true;
				m_bACPowerOk	= true;
				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
				m_bCountRateTamperOk = true;
				m_bRMSSigmaTamperOk  = true;
				m_bSpectChange  = false;
				m_bRestartReq   = false;
				m_bInMIIU235	= false;
				m_bInMIIU238	= false;
				m_bInMIICs137	= false;
				m_bInMIIScaler	= false;
				m_bDataFiltered	= false;
				m_pParent->m_dBreakSentCount = 0;
				m_pParent->m_dTimeOutCount = 0;
				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}
			//	send string
			fprintf(File,Message);
			//	close pfm
			fclose(File);
		}
	}
}


int CMCAPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	//CRect rect(6,405,100,455);
	//GetWindowRect(&rect);
	//ScreenToClient(&rect);

	m_pCloseButton = new CColorButton(false, m_pParent->m_szIniFile);
	m_pCloseButton->SetBase(10);
	m_pCloseButton->SetPreNum("BBM: ");

	m_pPauseButton = new CButton();
	m_pAckFlagsButton = new CButton();
	m_pPrintButton = new CButton();
	m_pToClose = new CButton();
	m_pImageList = new CImageList();
	m_pImageList->Create( IDB_BMP_ALL, 18, 1, COLOR_GRAY);

	return 0;
}

void CMCAPropertySheet::OnCloseButton()
{
	ShowWindow(SW_HIDE);
	KillTimer(1);
}

void CMCAPropertySheet::SetTabColor(int Tab,COLORREF Color)
{
	if ((Tab != IDC_MCAGENERAL_TAB) && 
		(Tab != IDC_DATA_TAB) &&
		(Tab != IDC_CDDATA_TAB)) return;
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
	case IDC_MCAGENERAL_TAB:tab = 1; break;
	case IDC_DATA_TAB:		tab = 2; break;
	case IDC_CDDATA_TAB:	tab = 3; break;
	};
	GetTabControl()->SetItem(tab,&pTabCtrlItem);
	GetTabControl()->Invalidate();
}

void CMCAPropertySheet::OnAckButton()
{
	m_dToClose = 0;

	//acknowledge the four button flags
	m_pParent->m_pButton->ShowIcon_Bell(VARIANT_FALSE);
	m_pParent->FlagComm(0,false);
	m_pParent->FlagOthr(0,false);
	m_pParent->FlagTime(0,false);

	m_pParent->m_pButton->ShowIcon_Disk(VARIANT_FALSE);
	m_pParent->m_pButton->put_Flash(VARIANT_FALSE);

	//acknowledge all the "red" errors
	CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
	CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CMCADataStat* datapage    = m_pParent->m_pDataStatus;
	CMCACDData*   configpage  = m_pParent->m_pCDData;
//	CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CMCAModParam* modparpage  = m_pParent->m_pModParams;

//	generalpage->SetColor(IDC_MCAGENERAL_CHECKSUM,COLOR_BLACK);
	summarypage->ResetColors();
	generalpage->ResetColors();
	datapage->ResetColors();
	configpage->ResetColors();

	SetTabColor(IDC_MCAGENERAL_TAB,COLOR_GREEN);
	SetTabColor(IDC_DATA_TAB,COLOR_GREEN);
	SetTabColor(IDC_CDDATA_TAB,COLOR_GREEN);
//	SetTabColor(IDC_USERPROG_TAB,COLOR_GREEN);

	Data_Tab = 0;
	General_Tab = 0;

	Invalidate();
}

void CMCAPropertySheet::OnPauseButton()
{
	m_dToClose = 0;
	CMCAModParam* modparpage = m_pParent->m_pModParams;

	if (m_pParent->TogglePause())
	{
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
		modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(true);
	}
	else
	{
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
		modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(false);
	}
}

void CMCAPropertySheet::IssuePrint(FILE* outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	//char cTemp, cTemp1, cTemp2, cTemp3, cTemp4;

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");

	CMCASummary*  summarypage = m_pParent->m_pSummaryPage;
	CMCAGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CMCADataStat* datapage    = m_pParent->m_pDataStatus;
//	CMCAUserProg* userpage    = m_pParent->m_pUserProgStatus;
	CMCACDData*   configpage  = m_pParent->m_pCDData;
	CMCAModParam* modparpage  = m_pParent->m_pModParams;

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"SUMMARY\n");
	fprintf(outfile," MCA Setup\n");
	summarypage->GetDlgItemText(IDC_MCASUMMARY_MONSTATUS,temp);
	fprintf(outfile,"  Startup Status: %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_BATTERY1,temp1);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_BATTERY2,temp2);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_BATTERY3,temp3);
	fprintf(outfile,"  Battery Voltage (Min / Cur / Max): %s %s %s\n",
		temp1,temp2,temp3);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_P15VOLT1,temp1);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_P15VOLT2,temp2);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_P15VOLT3,temp3);
	fprintf(outfile,"  +12 Volt Supply (Min / Cur / Max): %s %s %s\n",
		temp1,temp2,temp3);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_M15VOLT1,temp1);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_M15VOLT2,temp2);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_M15VOLT3,temp3);
	fprintf(outfile,"  -12 Volt Supply (Min / Cur / Max): %s %s %s\n",
		temp1,temp2,temp3);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_P5VOLT1,temp1);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_P5VOLT2,temp2);
	summarypage->GetDlgItemText(IDC_MCASUMMARY_P5VOLT3,temp3);
	fprintf(outfile,"   +5 Volt Supply (Min / Cur / Max): %s %s %s\n",
		temp1,temp2,temp3);
	fprintf(outfile," Data Status\n");
	summarypage->GetDlgItemText(IDC_MCASUMMARY_INQUIRETIME,temp);
	fprintf(outfile,"  Last INQUIRE Status (Y.M.D H:M:S):\n  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_ACPOWER,temp);
	fprintf(outfile,"                     External Power: %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_BATTERYOK,temp);
	fprintf(outfile,"                            Battery: %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_BBMOK,temp);
	fprintf(outfile,"           Battery Backed Up Memory: %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_OVERTHRESH,temp);
	fprintf(outfile,"              Over Threshold Alarms: %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_GDCSTATUS,temp);
	fprintf(outfile,"                GDC State Of Health: %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_HVCSTATUS,temp);
	fprintf(outfile,"                HVC State Of Health: %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_MONSTAT,temp);

	fprintf(outfile,"  Recent Commands and Responses:\n");
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND6,temp);
	temp += (summarypage->StaticColors[IDC_MCASUMMARY_COMMAND6-IDC_MCASUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND5,temp);
	temp += (summarypage->StaticColors[IDC_MCASUMMARY_COMMAND5-IDC_MCASUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND4,temp);
	temp += (summarypage->StaticColors[IDC_MCASUMMARY_COMMAND4-IDC_MCASUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND3,temp);
	temp += (summarypage->StaticColors[IDC_MCASUMMARY_COMMAND3-IDC_MCASUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND2,temp);
	temp += (summarypage->StaticColors[IDC_MCASUMMARY_COMMAND2-IDC_MCASUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_MCASUMMARY_COMMAND1,temp);
	temp += (summarypage->StaticColors[IDC_MCASUMMARY_COMMAND1-IDC_MCASUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));

	fprintf(outfile,"            Monitor State Of Health: %s\n",LPCTSTR(temp));
	fprintf(outfile,"\n%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"MCA SETUP\n Most Recent MCA GENERAL STATUS\n");
	generalpage->GetDlgItemText(IDC_MCAGENERAL_TIME,temp);
	fprintf(outfile,"  Y.M.D H:M:S: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_TEMPIMM,temp);
	fprintf(outfile,"     Temperature Immediate (C): %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_TEMPEFF,temp);
	fprintf(outfile,"     Temperature Effective (C): %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_RPAREA,temp);
	fprintf(outfile,"                       RP Area: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_RPCENT,temp);
	fprintf(outfile,"                   RP Centroid: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_HIGHVACTUAL,temp);
	fprintf(outfile,"   High Voltage (Actual Volts): %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_HIGHVDESIRED,temp);
	fprintf(outfile,"  High Voltage (Desired Volts): %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_GDCADJ,temp);
	fprintf(outfile,"               GDC Adj. Factor: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_BATTERY1,temp1);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_BATTERY2,temp2);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_BATTERY3,temp3);
	fprintf(outfile,"  Battery Voltage (Min / Cur / Max): %s %s %s\n",
		temp1,temp2,temp3);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_P15VOLT1,temp1);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_P15VOLT2,temp2);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_P15VOLT3,temp3);
	fprintf(outfile,"  +12 Volt Supply (Min / Cur / Max): %s %s %s\n",
		temp1,temp2,temp3);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_M15VOLT1,temp1);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_M15VOLT2,temp2);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_M15VOLT3,temp3);
	fprintf(outfile,"  -12 Volt Supply (Min / Cur / Max): %s %s %s\n",
		temp1,temp2,temp3);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_P5VOLT1,temp1);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_P5VOLT2,temp2);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_P5VOLT3,temp3);
	fprintf(outfile,"   +5 Volt Supply (Min / Cur / Max): %s %s %s\n",
		temp1,temp2,temp3);

	generalpage->GetDlgItemText(IDC_MCAGENERAL_U235STATE,temp1);
	generalpage->GetDlgItemText(IDC_STATIC_ALARM1,temp2);				//DR356
	fprintf(outfile,"%12s Threshold Alarm (State): %8s\n",temp2,temp1);	//DR356 

	generalpage->GetDlgItemText(IDC_MCAGENERAL_CS137STATE,temp1);
	generalpage->GetDlgItemText(IDC_STATIC_ALARM2,temp2);				//DR356
	fprintf(outfile,"%12s Threshold Alarm (State): %8s\n",temp2,temp1);	//DR356

	generalpage->GetDlgItemText(IDC_MCAGENERAL_U238STATE,temp1);
	generalpage->GetDlgItemText(IDC_STATIC_ALARM3,temp2);				//DR356
	fprintf(outfile,"%12s Threshold Alarm (State): %8s\n",temp2,temp1);	//DR356

	generalpage->GetDlgItemText(IDC_MCAGENERAL_GROSSCOUNTS,temp1);
	generalpage->GetDlgItemText(IDC_STATIC_ALARM4,temp2);				//DR356
	fprintf(outfile,"%12s Threshold Alarm (State): %8s\n",temp2,temp1);	//DR356

	generalpage->GetDlgItemText(IDC_MCAGENERAL_SCALESTATE,temp1);
	generalpage->GetDlgItemText(IDC_STATIC_ALARM5,temp2);				//DR356
	fprintf(outfile,"%12s Threshold Alarm (State): %8s\n",temp2,temp1);	//DR356

	generalpage->GetDlgItemText(IDC_MCAGENERAL_RATIO1,temp1);
	generalpage->GetDlgItemText(IDC_STATIC_ALARM6,temp2);				//DR356
	fprintf(outfile,"%12s Threshold Alarm (State): %8s\n",temp2,temp1);	//DR356

	generalpage->GetDlgItemText(IDC_MCAGENERAL_RATIO2,temp1);
	generalpage->GetDlgItemText(IDC_STATIC_ALARM7,temp2);				//DR356
	fprintf(outfile,"%12s Threshold Alarm (State): %8s\n",temp2,temp1);	//DR356

	fprintf(outfile," Status Byte 1 and 2\n");
	generalpage->GetDlgItemText(IDC_MCAGENERAL_SPECTCHANG,temp);
	fprintf(outfile,"  Spectrum Changing: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_ACPOWER,temp);
	fprintf(outfile,"     External Power: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_BATSTAT,temp);
	fprintf(outfile,"            Battery: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_FILTERING,temp);
	fprintf(outfile,"          Filtering: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_BBMSTAT,temp);
	fprintf(outfile,"                BBM: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_MONITORSTAT,temp);
	fprintf(outfile,"     Startup Status: %s\n",temp);
	fprintf(outfile," GDC State Of Health\n");
	generalpage->GetDlgItemText(IDC_MCAGENERAL_GDCENABLE,temp);
	fprintf(outfile,"     System Enabled: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_GDCHOLD,temp);
	fprintf(outfile,"     System on Hold: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_GDCSTATUS,temp);
	fprintf(outfile,"             Status: %s\n",temp);
	fprintf(outfile," HVC State Of Health\n");
	generalpage->GetDlgItemText(IDC_MCAGENERAL_HVCENABLE,temp);
	fprintf(outfile,"     System Enabled: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_HVCHOLD,temp);
	fprintf(outfile,"     System on Hold: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_HVCSTATUS,temp);
	fprintf(outfile,"             Status: %s\n",temp);
	generalpage->GetDlgItemText(IDC_MCAGENERAL_CHECKSUM,temp);
	fprintf(outfile," Checksum or Length Error in Any Message:\n%s\n",temp);
	fprintf(outfile,"\n%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"Data Status\n Most Recent MCA INQUIRE STATUS\n");
	datapage->GetDlgItemText(IDC_MCADATA_INQUIRETIME,temp);
	fprintf(outfile,"  Y.M.D H:M:S: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_BYTESMEM,temp);
	fprintf(outfile,"   Bytes of Data in BBM: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_OVERTHRESH,temp);
	fprintf(outfile,"  Over Threshold Alarms: %s\n",temp);
	fprintf(outfile," Status Byte 1 and 2\n");
	datapage->GetDlgItemText(IDC_MCADATA_SPECTCHANG,temp);
	fprintf(outfile,"  Spectrum Changing: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_ACPOWER,temp);
	fprintf(outfile,"     External Power: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_BATSTAT,temp);
	fprintf(outfile,"            Battery: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_FILTERING,temp);
	fprintf(outfile,"          Filtering: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_BBMSTAT,temp);
	fprintf(outfile,"                BBM: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_MONSTAT,temp);
	fprintf(outfile,"     Startup Status: %s\n",temp);
	fprintf(outfile," GDC State Of Health\n");
	datapage->GetDlgItemText(IDC_MCADATA_GDCENABLE,temp);
	fprintf(outfile,"     System Enabled: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_GDCHOLD,temp);
	fprintf(outfile,"     System on Hold: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_GDCSTATUS,temp);
	fprintf(outfile,"             Status: %s\n",temp);
	fprintf(outfile," HVC State Of Health\n");
	datapage->GetDlgItemText(IDC_MCADATA_HVCENABLE,temp);
	fprintf(outfile,"     System Enabled: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_HVCHOLD,temp);
	fprintf(outfile,"     System on Hold: %s\n",temp);
	datapage->GetDlgItemText(IDC_MCADATA_HVCSTATUS,temp);
	fprintf(outfile,"             Status: %s\n",temp);
	fprintf(outfile," Most Recent Acquire Records\n");
	datapage->GetDlgItemText(IDC_MCADATA_DATADATE1,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_DATADATE2,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_DATADATE3,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_DATADATE4,temp4);
	fprintf(outfile,"%34s: %10s %10s %10s %10s\n",
		"Date of Data Record (Y/M/D)",temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_MCADATA_DATATIME1,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_DATATIME2,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_DATATIME3,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_DATATIME4,temp4);
	fprintf(outfile,"%34s: %10s %10s %10s %10s\n",
		"Time of Data Record (H:M:S)",temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_MCADATA_STATUSALARM1,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_STATUSALARM2,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_STATUSALARM3,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_STATUSALARM4,temp4);
	fprintf(outfile,"%34s: %10s %10s %10s %10s\n",
		"Status Alarm",temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_STATIC_CH1,temp);
	datapage->GetDlgItemText(IDC_MCADATA_U2351,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_U2352,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_U2353,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_U2354,temp4);
	fprintf(outfile,"%28s - Ch1: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_STATIC_CH2,temp);
	datapage->GetDlgItemText(IDC_MCADATA_CS1371,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_CS1372,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_CS1373,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_CS1374,temp4);
	fprintf(outfile,"%28s - Ch2: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_STATIC_CH3,temp);
	datapage->GetDlgItemText(IDC_MCADATA_U2381,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_U2382,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_U2383,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_U2384,temp4);
	fprintf(outfile,"%28s - Ch3: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_STATIC_CH4,temp);
	datapage->GetDlgItemText(IDC_MCADATA_SCALER1,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_SCALER2,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_SCALER3,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_SCALER4,temp4);
	fprintf(outfile,"%28s - Ch4: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_STATIC_CH5,temp);
	datapage->GetDlgItemText(IDC_MCADATA_U235RATIO1,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_U235RATIO2,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_U235RATIO3,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_U235RATIO4,temp4);
	fprintf(outfile,"%28s - Ch5: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_STATIC_CH6,temp);
	datapage->GetDlgItemText(IDC_MCADATA_CS137RATIO1,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_CS137RATIO2,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_CS137RATIO3,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_CS137RATIO4,temp4);
	fprintf(outfile,"%28s - Ch6: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_STATIC_CH7,temp);
	datapage->GetDlgItemText(IDC_MCADATA_UNKNOWNR1,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_UNKNOWNR2,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_UNKNOWNR3,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_UNKNOWNR4,temp4);
	fprintf(outfile,"%28s - Ch7: %10s %10s %10s %10s\n",
		temp,temp1,temp2,temp3,temp4);
	datapage->GetDlgItemText(IDC_MCADATA_ET1,temp1);
	datapage->GetDlgItemText(IDC_MCADATA_ET2,temp2);
	datapage->GetDlgItemText(IDC_MCADATA_ET3,temp3);
	datapage->GetDlgItemText(IDC_MCADATA_ET4,temp4);
	fprintf(outfile,"%34s: %10s %10s %10s %10s\n",
		"Elapsed Time (sec)",temp1,temp2,temp3,temp4);
	fprintf(outfile,"\n%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"MCA Configuration\n");
	configpage->GetDlgItemText(IDC_MCACD_CONFIG10,temp);
	temp += (configpage->StaticColors[IDC_MCACD_CONFIG10-IDC_MCACDDATA_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  Config 10 Record:\n");
	fprintf(outfile," %s\n",temp);
	configpage->GetDlgItemText(IDC_MCACD_CONFIG11,temp);
	temp += (configpage->StaticColors[IDC_MCACD_CONFIG11-IDC_MCACDDATA_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  Config 11 Record:\n");
	fprintf(outfile," %s\n",temp);
	configpage->GetDlgItemText(IDC_MCACD_CONFIG12,temp);
	temp += (configpage->StaticColors[IDC_MCACD_CONFIG12-IDC_MCACDDATA_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  Config 12 Record:\n");
	fprintf(outfile," %s\n",temp);
	configpage->GetDlgItemText(IDC_MCACD_CONFIG13,temp);
	temp += (configpage->StaticColors[IDC_MCACD_CONFIG13-IDC_MCACDDATA_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  Config 13 Record:\n");
	fprintf(outfile," %s\n",temp);
	configpage->GetDlgItemText(IDC_MCACD_CONFIG14,temp);
	temp += (configpage->StaticColors[IDC_MCACD_CONFIG14-IDC_MCACDDATA_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  Config 14 Record:\n");
	fprintf(outfile," %s\n",temp);
	fprintf(outfile,"\n%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"MODIFY PARAMETERS -- Caution: These values may not have been applied!\n");
	fprintf(outfile," Communications\n");
	modparpage->GetDlgItemText(IDC_MODPARAMS_PORT,temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_NODE,temp1);
	fprintf(outfile,"  Port: %s\n  Node: %s\n\n",temp,temp1);
	fprintf(outfile," File Output\n");
	modparpage->GetDlgItemText(IDC_MODPARAMS_SAVELOC,temp);
	fprintf(outfile,"  Location: %s\n",temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_FILEID,temp);
	fprintf(outfile,"  Station ID: %s\n",temp);
	if(((CButton*)modparpage->GetDlgItem(IDC_MODPARAMS_LOGFILTER))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"  Log Data Filtering Msgs: %s\n",temp);
	if(((CButton*)modparpage->GetDlgItem(IDC_MODPARAMS_DODUMP))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"  Do Dump File: %s\n",temp);
	if(((CButton*)modparpage->GetDlgItem(IDC_MODPARAMS_LOGMII))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"  Log Alarm Msgs: %s\n\n",temp);
	fprintf(outfile," Error Flags\n");
	fprintf(outfile,"                           High     Low\n");
	modparpage->GetDlgItemText(IDC_MODPARAMS_BATVOLTH,temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_BATVOLTL,temp1);
	fprintf(outfile,"      Battery (28 Volt): %6s  %6s\n",temp,temp1);
	modparpage->GetDlgItemText(IDC_MODPARAMS_SUP_P15H,temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_SUP_P15L,temp1);
	fprintf(outfile,"        +15 Volt Supply: %6s  %6s\n",temp,temp1);
	modparpage->GetDlgItemText(IDC_MODPARAMS_SUP_M15H,temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_SUP_M15L,temp1);
	fprintf(outfile,"        -15 Volt Supply: %6s  %6s\n",temp,temp1);
	modparpage->GetDlgItemText(IDC_MODPARAMS_SUP_P05H,temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_SUP_P05L,temp1);
	fprintf(outfile,"         +5 Volt Supply: %6s  %6s\n",temp,temp1);
	modparpage->GetDlgItemText(IDC_MODPARAMS_TIME_ERR,temp);
	fprintf(outfile,"  Time Delta (Sec.): %6s\n\n",temp);
	fprintf(outfile," General\n");
	modparpage->GetDlgItemText(IDC_MODPARAMS_MAXCYCLE,temp);
	fprintf(outfile,"     Message Cycle Time (mSec): %s\n",temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_MAXPAUSE,temp);
	fprintf(outfile,"      Maximum Pause Time (Sec): %s\n",temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_MAXBBM,temp);
	fprintf(outfile,"           Maximum BBM (Bytes): %s\n",temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_COMMFAIL,temp);
	fprintf(outfile,"  # of Retransmits Before Fail: %s\n",temp);

	if(((CButton*)modparpage->GetDlgItem(IDC_CHECK_ALLOWAUTODATE))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"    Allow Auto Date & Time Set: %s\n",temp);

	modparpage->GetDlgItemText(IDC_MODPARAMS_MAXINQUIRE,temp);
	fprintf(outfile,"          Inquire Every(units): %s\n",temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_TIMEOUT,temp);
	fprintf(outfile,"     Response Time-Out (units): %s\n",temp);
	modparpage->GetDlgItemText(IDC_MODPARAMS_MAXSTATUS,temp);
	fprintf(outfile,"     Take Status Every (units): %s\n",temp);
	fprintf(outfile,"\n%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"CURRENT CONFIGURATION (from INI file)\n");
	char buff[_MAX_PATH];
	char *inifile = m_pParent->m_szIniFile;
	GetPrivateProfileString(m_pParent->m_pName,"SLOW_RESET","",buff,sizeof(buff),inifile);
	fprintf(outfile," SLOW_RESET=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"PORT","",buff,sizeof(buff),inifile);
	fprintf(outfile," PORT=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"NODE","",buff,sizeof(buff),inifile);
	fprintf(outfile," NODE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,SAVE_LOC,"",buff,sizeof(buff),inifile);
	fprintf(outfile," SAVE_LOC=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,FILE_ID,"",buff,sizeof(buff),inifile);
	fprintf(outfile," FILE_ID=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"LOG_FILTER","",buff,sizeof(buff),inifile);
	fprintf(outfile," LOG_FILTER=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"DO_DUMP","",buff,sizeof(buff),inifile);
	fprintf(outfile," DO_DUMP=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"LOG_MII","",buff,sizeof(buff),inifile);
	fprintf(outfile," LOG_MII=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"BATVOLTH","",buff,sizeof(buff),inifile);
	fprintf(outfile," BATVOLTH=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"BATVOLTL","",buff,sizeof(buff),inifile);
	fprintf(outfile," BATVOLTL=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_P15H","",buff,sizeof(buff),inifile);
	fprintf(outfile," SUP_P15H=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_P15L","",buff,sizeof(buff),inifile);
	fprintf(outfile," SUP_P15L=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_M15H","",buff,sizeof(buff),inifile);
	fprintf(outfile," SUP_M15H=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_M15L","",buff,sizeof(buff),inifile);
	fprintf(outfile," SUP_M15L=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_P05H","",buff,sizeof(buff),inifile);
	fprintf(outfile," SUP_P05H=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_P05L","",buff,sizeof(buff),inifile);
	fprintf(outfile," SUP-P05L=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"TIME_ERR","",buff,sizeof(buff),inifile);
	fprintf(outfile," TIME_ERR=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXCYCLE","",buff,sizeof(buff),inifile);
	fprintf(outfile," MAXCYCLE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXPAUSE","",buff,sizeof(buff),inifile);
	fprintf(outfile," MAXPAUSE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXBBM","",buff,sizeof(buff),inifile);
	fprintf(outfile," MAXBBM=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXINQUIRE","",buff,sizeof(buff),inifile);
	fprintf(outfile," MAXINQUIRE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXSTATUS","",buff,sizeof(buff),inifile);
	fprintf(outfile," MAXSTATUS=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"COMMFAIL","",buff,sizeof(buff),inifile);
	fprintf(outfile," COMMFAIL=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SLOW_RESET","",buff,sizeof(buff),inifile);
	fprintf(outfile," SLOW_RESET=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"FLAGTIME","",buff,sizeof(buff),inifile);
	fprintf(outfile," FLAGTIME=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"FLAGCOMM","",buff,sizeof(buff),inifile);
	fprintf(outfile," FLAGCOMM=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"FLAGOTHR","",buff,sizeof(buff),inifile);
	fprintf(outfile," FLAGOTHR=%s\n",buff);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
}

void CMCAPropertySheet::OnPrintButton()
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

void CMCAPropertySheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 	CPropertySheet::OnShowWindow(bShow, nStatus);

	if (bShow && nStatus==0) 
	{
		SetActivePage(0);
		m_dToClose = 0;
		SetTimer(1,1000,NULL);

		CMCAModParam* modparpage = m_pParent->m_pModParams;

		if (m_pParent->IsPaused())
			modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(true);
		else
			modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(false);
	}
	else
		KillTimer(1);
}

void CMCAPropertySheet::OnToCloseButton()
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

void CMCAPropertySheet::OnExtendClose(WPARAM, LPARAM)
{
	m_dToClose = 0;
}

void CMCAPropertySheet::OnTimer(UINT nIDEvent) 
{
	
//for debug only REMOVE ME also see OnInitDialog
	/*
	if (nIDEvent == 1000)
	{
		static i;
		if (i == 0)
		{
			COleDateTime cNow = COleDateTime::GetCurrentTime();
			COleDateTime cStart = COleDateTime(1952,1,1,0,0,0);
			COleDateTimeSpan cSpan = cNow - cStart;

			m_cSpectJulian.Init((unsigned int)cSpan.GetTotalSeconds());
			memset(m_uiSpectrum,NULL,sizeof(m_uiSpectrum));
		}

		for (int j=i; j<(i+64); j++)
			m_uiSpectrum[j]=j;

		i=i+64;
		m_iSpectNumChan = 1024;
		CMCASpecDataStat* specdatapage    = m_pParent->m_pSpecDataStatus;
		specdatapage->ProcessSpectrumBufferForRealTimeGraphic(m_uiSpectrum, m_iSpectNumChan);
		if (i >=1024)
		{
			//record 22
			m_bReceivingSpectra = true;
			SpectTail("2264\r\n");
			i = 0;
		}
	}
	*/
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

void CMCAPropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//watch for alt-f4 to try to close us -- don't let it
	m_dToClose = 0;
	if (nID == SC_CLOSE)
		ShowWindow(SW_HIDE);
	else
  		CPropertySheet::OnSysCommand(nID, lParam);
}

BOOL CMCAPropertySheet::OnInitDialog() 
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
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(1,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(2,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(3,&pTabCtrlItem);

	//for testing only REMOVE ME, also see OnTimer
	//SetTimer(1000,1000,NULL);

	return bResult;
}


BOOL CMCAPropertySheet::DestroyWindow() 
{
	GetTabControl( )->SetImageList(NULL);
	return CPropertySheet::DestroyWindow();
}


void CMCAPropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)
		m_dToClose = set;
}

void CMCAPropertySheet::ForceDateTime(void)
{
	m_dToClose = 0;
	m_pParent->PostMessage(uIDC_RECEIVE,FORCEDATETIME,CHECKSUM_GOOD);
}

CString *CMCAPropertySheet::GetVersionString()
{
	return &m_pParent->m_csVersion;
}