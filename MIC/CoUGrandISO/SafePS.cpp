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
// SafePS.cpp
// 
#include "stdafx.h"
#include "ISO_definitions.h"
#include "colors.h"
#include "ccbutton.h"
#include "JulianT.h"
#include "DataWatchDlg.h"
#include "SafePS.h"
#include "Instrument.h"
#include "UGrandSetupData.h"
#include "resource.h"
#include "invalid_chars.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSafePropertySheet

IMPLEMENT_DYNAMIC(CSafePropertySheet, CPropertySheet)

int CSafePropertySheet::ms_iUGRANDPropertySheetCount = 0;

CSafePropertySheet::CSafePropertySheet()
{
}

CSafePropertySheet::CSafePropertySheet(
	UINT nIDCaption, 
	CInstrument* pParentWnd, 
	char *inifile)
	:CPropertySheet(nIDCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
//	TRACE("CSafePropertySheet::CTOR a\n");
	m_pCaption[0] = NULL;
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	FinishConstruction();
}

CSafePropertySheet::CSafePropertySheet(
	LPCTSTR pszCaption, 
	CInstrument* pParentWnd,
	char *inifile)
	:CPropertySheet(pszCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
//	TRACE("CSafePropertySheet::CTOR b\n");
	strncpy(m_pCaption,pszCaption,sizeof(m_pCaption));
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	FinishConstruction();
}

void CSafePropertySheet::FinishConstruction()
{
//	TRACE("CSafePropertySheet::FinishConstruction\n");
	ms_iUGRANDPropertySheetCount++;
	m_bUpdatingFromFile = false;
///	m_iGeneral4_Tab		= 0;
	m_iData_Tab			= 0;
	m_iDataI_Tab		= 0;
	m_iData2_Tab		= 0;
	m_iUser4Prog_Tab	= 0;
	m_iInst4Setup_Tab	= 0;
	m_iuGInfo_Tab		= 0;
	m_iPSBInfo_Tab		= 0;
	m_iESBInfo_Tab		= 0;
	m_iBMBInfo_Tab		= 0;

	m_fTNSRHigh		= 0.0;
	m_fTNSRLow		= 0.0;
	m_dTNSRFirst	= true;

	m_fDIRHigh		= 0.0;
	m_fDIRLow		= 0.0;
	m_dDIRFirst		= true;

	m_fFBHigh		= 0.0;
	m_fFBLow		= 0.0;
	m_bFBFirst		= true;

	m_bBatteryOk	= true;
	m_bBatteryTolOk	= true;
	m_bP5VoltOk		= true;
	m_bP25VoltOk	= true;
	m_bP33VoltOk	= true;
	m_bACPowerOk	= true;
	m_bACPowerLoss	= false;
	m_bBBMOk		= true;
	m_bCountRateTamperOk = true;
	m_bRMSSigmaTamperOk = true;
	m_bInMII		= false;
	m_bNewFileName	= false;
	m_bDataFiltered	= false;
	m_bTimeError	= false;
	m_pDataWatchDlg = NULL;
	m_bDataWatchDlgOpen = false;
	m_pWatchWindow	= NULL;
	m_bWatchWindowOpen = false;
	m_bFirst15Rec	= true;

	m_fLowBV = m_fCurBV = m_fHighBV = 0.0;
	m_bFirstBV = true;
	m_fLowP25 = m_fCurP25 = m_fHighP25 = 0.0;
	m_bFirstP25 = true;
	m_fLowP33 = m_fCurP33 = m_fHighP33 = 0.0;
	m_bFirstP33 = true;
	m_fLowP5 = m_fCurP5 = m_fHighP5 = 0.0;
	m_bFirstP5 = true;

	m_dToClose				= 0;

	m_dCWStart				= -1; //neither COLD START (0) nor WARM START (1)

	m_usLength				= 0;
	m_usMessageType			= 0;
	m_dRecLengthCount		= 0;
	m_dGivenChecksum		= 0;
	m_dChecksum				= 0;

	m_bFCHVError			= false;
	m_bICHVError			= false;
	m_iFCHVError			= 0;
	m_iICHVError			= 0;

}

COLORREF
CSafePropertySheet::SetButtonColor(COLORREF color)
{
	if (color == COLOR_PAUSE)
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
	else
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
	return m_pCloseButton->SetFaceColor(color);
}

void
CSafePropertySheet::SetHLVoltageDisplay(
	float batvolth, float batvoltl, bool bFirstBV,
	float sup_p25h, float sup_p25l, bool bFirstP25,
	float sup_p33h, float sup_p33l, bool bFirstP33,
	float sup_p05h, float sup_p05l, bool bFirstP5)
{
	char temp[16];

	CSummary*  summarypage = m_pParent->m_pSummaryPage;
///	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;

	if (!bFirstBV)
	{
		m_fLowBV = batvoltl;
		m_fHighBV = batvolth;
		m_bFirstBV = false;

		sprintf(temp,"%5.1f",m_fLowBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY1,temp);
///		general4page->SetDlgItemText(IDC_GENERAL4_BATTERYMIN,temp);

		sprintf(temp,"%5.1f",m_fHighBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY3,temp);
///		general4page->SetDlgItemText(IDC_GENERAL4_BATTERYMAX,temp);
	}

	if (!bFirstP5)
	{
		m_fLowP5 = sup_p05l;
		m_fHighP5 = sup_p05h;
		m_bFirstP5 = false;

		sprintf(temp,"%5.1f",m_fLowP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,temp);
///		general4page->SetDlgItemText(IDC_GENERAL4_P5VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fHighP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,temp);
///		general4page->SetDlgItemText(IDC_GENERAL4_P5VOLTMAX,temp);
	}

	if (!bFirstP25)
	{
		m_fLowP25 = sup_p25l;
		m_fHighP25 = sup_p25h;
		m_bFirstP25 = false;

		sprintf(temp,"%5.1f",m_fLowP25);
		summarypage->SetDlgItemText(IDC_SUMMARY_P25VOLT1,temp);
///		general4page->SetDlgItemText(IDC_GENERAL4_P25VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fHighP25);
		summarypage->SetDlgItemText(IDC_SUMMARY_P25VOLT3,temp);
///		general4page->SetDlgItemText(IDC_GENERAL4_P25VOLTMAX,temp);
	}

	if (!bFirstP33)
	{
		m_fLowP33 = sup_p33l;
		m_fHighP33 = sup_p33h;
		m_bFirstP33 = false;

		sprintf(temp,"%5.1f",m_fLowP33);
		summarypage->SetDlgItemText(IDC_SUMMARY_P33VOLT1,temp);
///		general4page->SetDlgItemText(IDC_GENERAL4_P33VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fHighP25);
		summarypage->SetDlgItemText(IDC_SUMMARY_P33VOLT3,temp);
///		general4page->SetDlgItemText(IDC_GENERAL4_P33VOLTMAX,temp);
	}
}

void
CSafePropertySheet::SetLimits(
	float batvolth, float batvoltl,
	float sup_p25h, float sup_p25l,
	float sup_p33h, float sup_p33l,
	float sup_p05h, float sup_p05l,
	int time_err,	char* Id)
{	
	m_fBatVoltH=batvolth;
	m_fBatVoltL=batvoltl;
	m_fSup_P25H=sup_p25h;
	m_fSup_P25L=sup_p25l;
	m_fSup_P33H=sup_p33h;
	m_fSup_P33L=sup_p33l;
	m_fSup_P05H=sup_p05h;
	m_fSup_P05L=sup_p05l;
	m_dTime_Err=(long)time_err;
	strcpy(m_szCurrentID,Id);
}

CSafePropertySheet::~CSafePropertySheet()
{
	ms_iUGRANDPropertySheetCount--;

	m_pParent->m_pPropertySheet = NULL;

	delete m_pParent->m_pSummaryPage;
///	delete m_pParent->m_pGeneral4StatPage;
	delete m_pParent->m_pDataStatus;
	delete m_pParent->m_pUser4ProgStatus;
	delete m_pParent->m_pCDData;
	delete m_pParent->m_pModParams;
	delete m_pParent->m_pSetupData;
	delete m_pParent->m_pLegend;
	delete m_pParent->m_pUGInfo;
	delete m_pParent->m_pBMBInfo;
	delete m_pParent->m_pPSBInfo;
	delete m_pParent->m_pESBInfo;
	delete m_pParent->m_pData2Status;
	delete m_pParent->m_pDataIStatus;

	m_pParent->m_pSummaryPage = NULL;
///	m_pParent->m_pGeneral4StatPage = NULL;
	m_pParent->m_pDataStatus = NULL;
	m_pParent->m_pUser4ProgStatus = NULL;
	m_pParent->m_pCDData = NULL;
	m_pParent->m_pModParams = NULL;
	m_pParent->m_pSetupData = NULL;
	m_pParent->m_pLegend = NULL;
	m_pParent->m_pUGInfo = NULL;
	m_pParent->m_pBMBInfo = NULL;
	m_pParent->m_pPSBInfo = NULL;
	m_pParent->m_pESBInfo = NULL;
	m_pParent->m_pData2Status = NULL;
	m_pParent->m_pDataIStatus = NULL;

	delete m_pCloseButton;
	delete m_pPauseButton;
	delete m_pPrintButton;
	delete m_pAckFlagsButton;
	delete m_pToClose;
	delete m_pImageList;

	delete m_pDataWatchDlg;
}

BEGIN_MESSAGE_MAP(CSafePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSafePropertySheet)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	ON_WM_NCACTIVATE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CLOSE_BUTTON,	OnCloseButton)
	ON_BN_CLICKED(ID_PAUSE_BUTTON,	OnPauseButton)
	ON_BN_CLICKED(ID_ACKFL_BUTTON,	OnAckButton)
	ON_BN_CLICKED(ID_PRINT_BUTTON,	OnPrintButton)
	ON_BN_CLICKED(ID_TOCLOSE,		OnToCloseButton)
	ON_MESSAGE(IDC_EXTENDCLOSE,		OnExtendClose)
END_MESSAGE_MAP()
//	ON_MESSAGE(IDC_RECEIVE_OOB,		OnReceiveOOB)
//	ON_MESSAGE(IDC_RECEIVE,			OnReceive)

/////////////////////////////////////////////////////////////////////////////
// CSafePropertySheet message handlers
void CSafePropertySheet::UpdateSummary(char *Message)
{
	CString Data;
	bool bChange = false;
	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
///	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
	CDataIStat* dataipage  = m_pParent->m_pDataIStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;

	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND5,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND6,Data);
	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND4,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND5,Data);
	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND3,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND4,Data);
	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND3,Data);
	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND1,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND1,Message);

	if (strstr(Message,"LONG BREAK SET"))
	{
		COleDateTime Now = COleDateTime::GetCurrentTime();
		CString DateTime;
		DateTime = Now.Format("Long Break Sent: %d/%H:%M:%S");
		m_cCheckSumMsg = DateTime;
		m_cCommStatus = "Comm Status Unknown";
		SetChecksumDisplay();
		m_pParent->m_iDataI_Tab |= 0x0001;
		dataipage->SetColor(IDC_DATAI_CHECKSUM,COLOR_RED);
		bChange = true;
	}

	if (strstr(Message,"LOCAL LONG BREAK"))
	{
		COleDateTime Now = COleDateTime::GetCurrentTime();
		CString DateTime;
		DateTime = Now.Format("Local Long Break Sent: %d/%H:%M:%S");
		m_cCheckSumMsg = DateTime;
		SetChecksumDisplay();
		m_pParent->m_iDataI_Tab |= 0x0001;
		dataipage->SetColor(IDC_DATAI_CHECKSUM,COLOR_RED);
		bChange = true;
	}

///	if (bChange)
///	{
///		if (GetPageIndex(general4page) >= 0)
///		{
///			m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
///			if (m_pParent->m_iGeneral4_Tab == 0x0003)
///				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
///			else if (m_iGeneral4_Tab == 0x0002)
///				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
///			else if (m_iGeneral4_Tab == 0x0001)
///				SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
///			else if (m_iGeneral4_Tab == 0x0000)
///				SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
///		}
///	}
	if (bChange)
	{
		m_iDataI_Tab = m_pParent->m_iDataI_Tab;
		if (m_pParent->m_iDataI_Tab == 0x0003)
			SetTabColor(IDC_DATAI_TAB,COLOR_YELLOWRED);
		else if (m_iDataI_Tab == 0x0002)
			SetTabColor(IDC_DATAI_TAB,COLOR_YELLOW);
		else if (m_iDataI_Tab == 0x0001)
			SetTabColor(IDC_DATAI_TAB,COLOR_RED);
		else if (m_iDataI_Tab == 0x0000)
			SetTabColor(IDC_DATAI_TAB,COLOR_GREEN);
	}
}

void CSafePropertySheet::SetChecksumDisplay()
{
//	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	CDataIStat* DataIStatusPage = m_pParent->m_pDataIStatus;

	CString cString;
	cString = m_cCheckSumMsg;;
	cString += " ";
	cString += m_cCommStatus;
//	if (GetPageIndex(general4page)>=0) 
//		general4page->SetDlgItemText(IDC_GENERAL4_CHECKSUM,cString);
	if (GetPageIndex(DataIStatusPage)>=0)
		DataIStatusPage->SetDlgItemText(IDC_DATAI_CHECKSUM,cString);
}

void CSafePropertySheet::Reverse(BYTE *sz, int iLength)
{
	BYTE cTemp;
	for (int i = 0; i < (iLength/2); i++)
	{
		cTemp = sz[i];
		sz[i] = sz[iLength - i - 1];
		sz[iLength - i - 1] = cTemp;
	}
}

int CSafePropertySheet::AppendCheckSum(char *pBinaryAt)
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

//remove the high order byte(s) of the exponent
//usually make the exponent 2 digits
char* CSafePropertySheet::TrunkExp(char* szString, int iDigits)
{
	if ((iDigits == 1) || (iDigits == 2))
	{
		char *szAt;
		//find the exponent flag
		if ((szAt=strpbrk(szString,"gGfFeE"))!=0)
			strcpy(&(szString[strlen(szString)-3]),&(szString[strlen(szString)-iDigits]));
	}
	return szString;
}

//record 0f
void CSafePropertySheet::AcquirRec1(char *szRecord)
{
	char* pszAt;

	char szTemp[32];
	char szTemp2[16];

	float fSource[14];
	float fDestin[5];
	for (int i = 0; i < 5; i++) fDestin[i] = 0.0;
	for (i = 0; i < 14; i++) fSource[i] = 0.0;

	//make sure we arn't trying to read from the binary
	//file at the moment.  This can occur if dumpbbm is
	//secquence is happening at the same time the graph
	//display is being displayed.
	while(m_bUpdatingFromFile) Sleep(500);

	CString cTemp;
	CDataStat* datapage    = m_pParent->m_pDataStatus;

	//make pointer to a RAD acquire text record to crack szRecord
	RADAcquireTextRecord* pszRecord = (RADAcquireTextRecord*)szRecord;

 	//get julian time and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp[sizeof(pszRecord->cdJulianSeconds)]=NULL;

	CJulianTime cTime;
	cTime.Init(szTemp);
	
	//move some of the date displays down one
	datapage->GetDlgItemText(IDC_DATA_DATADATE3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_DATADATE4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_DATADATE2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_DATADATE3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_DATADATE1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_DATADATE2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_DATADATE1,cTime.Date());

	//move all the date displays down one
	sprintf(szTemp,"%s.%c",cTime.Time(),pszRecord->cdJulianSec10ths);
	datapage->GetDlgItemText(IDC_DATA_DATATIME3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_DATATIME4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_DATATIME2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_DATATIME3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_DATATIME1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_DATATIME2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_DATATIME1,szTemp);

	//get the status bytes and put them in the binary record
	strncpy(szTemp2,(char*)pszRecord->chStatusByte0,sizeof(pszRecord->chStatusByte0));
	szTemp2[sizeof(pszRecord->chStatusByte0)] = NULL;
	strcpy(szTemp,szTemp2);
	strcat(szTemp,":");

	strncpy(szTemp2,(char*)pszRecord->chStatusByte1,sizeof(pszRecord->chStatusByte1));
	szTemp2[sizeof(pszRecord->chStatusByte1)] = NULL;
	strcat(szTemp,szTemp2);
	strcat(szTemp,":");

	strncpy(szTemp2,(char*)pszRecord->chStatusByte2,sizeof(pszRecord->chStatusByte2));
	szTemp2[sizeof(pszRecord->chStatusByte2)] = NULL;
	strcat(szTemp,szTemp2);
	strcat(szTemp,":");

	strncpy(szTemp2,(char*)pszRecord->chStatusByte3,sizeof(pszRecord->chStatusByte3));
	szTemp2[sizeof(pszRecord->chStatusByte3)] = NULL;
	strcat(szTemp,szTemp2);
	
	//move the displayed status all down one
	datapage->GetDlgItemText(IDC_DATA_STATUS01233,cTemp);
	datapage->SetDlgItemText(IDC_DATA_STATUS01234,cTemp);
	datapage->GetDlgItemText(IDC_DATA_STATUS01232,cTemp);
	datapage->SetDlgItemText(IDC_DATA_STATUS01233,cTemp);
	datapage->GetDlgItemText(IDC_DATA_STATUS01231,cTemp);
	datapage->SetDlgItemText(IDC_DATA_STATUS01232,cTemp);
	datapage->SetDlgItemText(IDC_DATA_STATUS01231,szTemp);

	//get the pp input state and put it in the binary record
	szTemp[0] = pszRecord->chPPInputState;
	szTemp[1] = NULL;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_PPINPUT3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_PPINPUT4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_PPINPUT2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_PPINPUT3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_PPINPUT1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_PPINPUT2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_PPINPUT1,szTemp);

	//CH 0
	strncpy(szTemp,(char*)pszRecord->chCh0Status,sizeof(pszRecord->chCh0Status));
	szTemp[sizeof(pszRecord->chCh0Status)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all done one
	datapage->GetDlgItemText(IDC_DATA_CH0STATUS3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0STATUS4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH0STATUS2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0STATUS3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH0STATUS1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0STATUS2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0STATUS1,pszAt);

	//get the ch0 count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh0CountRate,sizeof(pszRecord->cfCh0CountRate));
	szTemp[sizeof(pszRecord->cfCh0CountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH0CR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0CR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH0CR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0CR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH0CR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0CR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0CR1,pszAt);
	sscanf(pszAt,"%f",&fSource[0]);

	//get the ch0 short count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh0ShortCountRate,sizeof(pszRecord->cfCh0ShortCountRate));
	szTemp[sizeof(pszRecord->cfCh0ShortCountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH0SCR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0SCR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH0SCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0SCR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH0SCR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0SCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0SCR1,pszAt);
	sscanf(pszAt,"%f",&fSource[1]);

	//get the ch0 long count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh0LongCountRate,sizeof(pszRecord->cfCh0LongCountRate));
	szTemp[sizeof(pszRecord->cfCh0LongCountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH0LCR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0LCR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH0LCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0LCR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH0LCR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0LCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH0LCR1,pszAt);
	sscanf(pszAt,"%f",&fSource[2]);

	//CH 1
	
	//get the ch1 status and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->chCh1Status,sizeof(pszRecord->chCh1Status));
	szTemp[sizeof(pszRecord->chCh1Status)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all done one
	datapage->GetDlgItemText(IDC_DATA_CH1STATUS3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1STATUS4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH1STATUS2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1STATUS3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH1STATUS1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1STATUS2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1STATUS1,pszAt);

	//get the ch1 count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh1CountRate,sizeof(pszRecord->cfCh1CountRate));
	szTemp[sizeof(pszRecord->cfCh1CountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH1CR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1CR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH1CR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1CR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH1CR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1CR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1CR1,pszAt);
	sscanf(pszAt,"%f",&fSource[3]);

	//get the ch1 short count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh1ShortCountRate,sizeof(pszRecord->cfCh1ShortCountRate));
	szTemp[sizeof(pszRecord->cfCh1ShortCountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH1SCR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1SCR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH1SCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1SCR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH1SCR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1SCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1SCR1,pszAt);
	sscanf(pszAt,"%f",&fSource[4]);

	//get the ch1 long count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh1LongCountRate,sizeof(pszRecord->cfCh1LongCountRate));
	szTemp[sizeof(pszRecord->cfCh1LongCountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH1LCR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1LCR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH1LCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1LCR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH1LCR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1LCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH1LCR1,pszAt);
	sscanf(pszAt,"%f",&fSource[5]);

	//CH 2
	
	//get the ch2 status and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->chCh2Status,sizeof(pszRecord->chCh2Status));
	szTemp[sizeof(pszRecord->chCh2Status)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all done one
	datapage->GetDlgItemText(IDC_DATA_CH2STATUS3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2STATUS4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH2STATUS2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2STATUS3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH2STATUS1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2STATUS2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2STATUS1,pszAt);

	//get the ch2 count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh2CountRate,sizeof(pszRecord->cfCh2CountRate));
	szTemp[sizeof(pszRecord->cfCh2CountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH2CR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2CR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH2CR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2CR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH2CR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2CR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2CR1,pszAt);
	sscanf(pszAt,"%f",&fSource[6]);

	//get the ch2 short count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh2ShortCountRate,sizeof(pszRecord->cfCh2ShortCountRate));
	szTemp[sizeof(pszRecord->cfCh2ShortCountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH2SCR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2SCR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH2SCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2SCR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH2SCR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2SCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2SCR1,pszAt);
	sscanf(pszAt,"%f",&fSource[7]);

	//get the ch2 long count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh2LongCountRate,sizeof(pszRecord->cfCh2LongCountRate));
	szTemp[sizeof(pszRecord->cfCh2LongCountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH2LCR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2LCR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH2LCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2LCR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH2LCR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2LCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH2LCR1,pszAt);
	sscanf(pszAt,"%f",&fSource[8]);

	//CH 3
	
	//get the ch3 status and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->chCh3Status,sizeof(pszRecord->chCh3Status));
	szTemp[sizeof(pszRecord->chCh3Status)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all done one
	datapage->GetDlgItemText(IDC_DATA_CH3STATUS3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3STATUS4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH3STATUS2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3STATUS3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH3STATUS1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3STATUS2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3STATUS1,pszAt);

	//get the ch3 count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh3CountRate,sizeof(pszRecord->cfCh3CountRate));
	szTemp[sizeof(pszRecord->cfCh3CountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH3CR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3CR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH3CR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3CR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH3CR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3CR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3CR1,pszAt);
	sscanf(pszAt,"%f",&fSource[9]);

	//get the ch3 short count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh3ShortCountRate,sizeof(pszRecord->cfCh3ShortCountRate));
	szTemp[sizeof(pszRecord->cfCh3ShortCountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH3SCR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3SCR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH3SCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3SCR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH3SCR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3SCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3SCR1,pszAt);
	sscanf(pszAt,"%f",&fSource[10]);

	//get the ch3 long count rate and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cfCh3LongCountRate,sizeof(pszRecord->cfCh3LongCountRate));
	szTemp[sizeof(pszRecord->cfCh3LongCountRate)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CH3LCR3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3LCR4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH3LCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3LCR3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CH3LCR1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3LCR2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CH3LCR1,pszAt);
	sscanf(pszAt,"%f",&fSource[11]);

	//get the count time
	strncpy(szTemp,(char*)pszRecord->cdCountTime10ths,sizeof(pszRecord->cdCountTime10ths));
	szTemp[sizeof(pszRecord->cdCountTime10ths)] = NULL;
	pszAt = szTemp;
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_LENGTHT3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_LENGTHT4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_LENGTHT2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_LENGTHT3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_LENGTHT1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_LENGTHT2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_LENGTHT1,pszAt);

	//get the cpu and board tempuratures
	strncpy(szTemp,(char*)pszRecord->cdCPUTemp,sizeof(pszRecord->cdCPUTemp));
	szTemp[sizeof(pszRecord->cdCPUTemp)] = NULL;

	strncpy(szTemp2,(char*)pszRecord->cdBOARDTemp,sizeof(pszRecord->cdBOARDTemp));
	szTemp2[sizeof(pszRecord->cdBOARDTemp)] = NULL;

	pszAt = szTemp2;
	sscanf(pszAt,"%f",&fSource[12]);
	while (*pszAt == ' ') pszAt++;
	strcat(szTemp, " ");
	strcat(szTemp, pszAt);
	pszAt = szTemp;
	sscanf(pszAt,"%f",&fSource[13]);
	while (*pszAt == ' ') pszAt++;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA_CPUBOARDTEMP3,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CPUBOARDTEMP4,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CPUBOARDTEMP2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CPUBOARDTEMP3,cTemp);
	datapage->GetDlgItemText(IDC_DATA_CPUBOARDTEMP1,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CPUBOARDTEMP2,cTemp);
	datapage->SetDlgItemText(IDC_DATA_CPUBOARDTEMP1,pszAt);

	CDataStat	*pDataStatPage = m_pParent->m_pDataStatus;

	if (m_bDataWatchDlgOpen)
	{
		int j = 0;
		int jNext = 0;
		for (i = 0; i < 5; i++)
		{
			j = jNext;
			while (j < 14)
			{
				if (pDataStatPage->m_bCheckVector[j])
				{
					fDestin[i] = fSource[j];
					jNext = j+1;
					j = 14;
				}
				j++;
			}
		}
		m_pDataWatchDlg->SetData(cTime.TotalSeconds(),
			fDestin[0],
			fDestin[1],
			fDestin[2],
			fDestin[3],
			fDestin[4],
			true);
	}
}

//record 59
void CSafePropertySheet::AcquirRec2(char *szRecord)
{
	CData2Stat* datapage = m_pParent->m_pData2Status;
//TRACE(szRecord);
//TRACE("\r\n");
	char szTemp[32];
	char szTemp2[32];
	CString cTemp;
	int iTemp;

	//get a pointer to a ESBAcquireTextRecord
	ESBAcquireTextRecord* pszRecord = (ESBAcquireTextRecord*)szRecord;

	//get julian time and put it in the binary record
	strncpy(szTemp,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp[sizeof(pszRecord->cdJulianSeconds)]=NULL;

	CJulianTime cTime;
	cTime.Init(szTemp);

	//move some of the date displays down one
	datapage->GetDlgItemText(IDC_DATA2_DATADATE3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_DATADATE4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_DATADATE2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_DATADATE3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_DATADATE1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_DATADATE2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_DATADATE1,cTime.Date());

	//move all the date displays down one
	sprintf(szTemp,"%s.%c",	cTime.Time(),pszRecord->cdJulianSec10ths);
	datapage->GetDlgItemText(IDC_DATA2_DATATIME3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_DATATIME4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_DATATIME2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_DATATIME3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_DATATIME1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_DATATIME2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_DATATIME1,szTemp);

	//get the status bytes and put them in the binary record
	strncpy(szTemp2,(char*)pszRecord->chStatusByte0,sizeof(pszRecord->chStatusByte0));
	szTemp2[sizeof(pszRecord->chStatusByte0)] = NULL;
	strcpy(szTemp,szTemp2);
	strcat(szTemp,":");

	strncpy(szTemp2,(char*)pszRecord->chStatusByte1,sizeof(pszRecord->chStatusByte1));
	szTemp2[sizeof(pszRecord->chStatusByte1)] = NULL;
	strcat(szTemp,szTemp2);
	strcat(szTemp,":");

	strncpy(szTemp2,(char*)pszRecord->chStatusByte2,sizeof(pszRecord->chStatusByte2));
	szTemp2[sizeof(pszRecord->chStatusByte2)] = NULL;
	strcat(szTemp,szTemp2);
	strcat(szTemp,":");

	strncpy(szTemp2,(char*)pszRecord->chStatusByte3,sizeof(pszRecord->chStatusByte3));
	szTemp2[sizeof(pszRecord->chStatusByte3)] = NULL;
	strcat(szTemp,szTemp2);

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_STATUS01233,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_STATUS01234,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_STATUS01232,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_STATUS01233,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_STATUS01231,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_STATUS01232,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_STATUS01231,szTemp);

	//get the accelerometer value and put it in the binary record
	memcpy(szTemp,pszRecord->chAccelerometer,sizeof(pszRecord->chAccelerometer))                      ;
	szTemp[sizeof(pszRecord->chAccelerometer)] = NULL;
	sscanf(szTemp,"%x",&iTemp);
	sprintf(szTemp,"%d:0x%03x",iTemp,iTemp);

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_ACCEL3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_ACCEL4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_ACCEL2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_ACCEL3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_ACCEL1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_ACCEL2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_ACCEL1,szTemp);

	//do the Humidity value and put it in th ebinary record
	memcpy(szTemp,pszRecord->cdHumidity,sizeof(pszRecord->cdHumidity))                      ;
	szTemp[sizeof(pszRecord->cdHumidity)] = NULL;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_HUMID3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_HUMID4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_HUMID2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_HUMID3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_HUMID1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_HUMID2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_HUMID1,szTemp);

	//do the Light Intensity
	memcpy(szTemp,pszRecord->chLightInt,sizeof(pszRecord->chLightInt))                      ;
	szTemp[sizeof(pszRecord->chLightInt)] = NULL;
	sscanf(szTemp,"%x",&iTemp);
	sprintf(szTemp,"%d:0x%03x",iTemp,iTemp);

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_LIGHT3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_LIGHT4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_LIGHT2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_LIGHT3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_LIGHT1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_LIGHT2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_LIGHT1,szTemp);

	//do the MagnetometerX
	memcpy(szTemp,pszRecord->chMagnetometerX,sizeof(pszRecord->chMagnetometerX))                      ;
	szTemp[sizeof(pszRecord->chMagnetometerX)] = NULL;
	sscanf(szTemp,"%x",&iTemp);
	sprintf(szTemp,"%d:0x%03x",iTemp,iTemp);

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_MAGX3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGX4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_MAGX2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGX3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_MAGX1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGX2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGX1,szTemp);

	//do the MagnetometerY
	memcpy(szTemp,pszRecord->chMagnetometerY,sizeof(pszRecord->chMagnetometerY))                      ;
	szTemp[sizeof(pszRecord->chMagnetometerY)] = NULL;
	sscanf(szTemp,"%x",&iTemp);
	sprintf(szTemp,"%d:0x%03x",iTemp,iTemp);

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_MAGY3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGY4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_MAGY2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGY3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_MAGY1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGY2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGY1,szTemp);

	//do the MagnetometerZ
	memcpy(szTemp,pszRecord->chMagnetometerZ,sizeof(pszRecord->chMagnetometerZ))                      ;
	szTemp[sizeof(pszRecord->chMagnetometerZ)] = NULL;
	sscanf(szTemp,"%x",&iTemp);
	sprintf(szTemp,"%d:0x%03x",iTemp,iTemp);

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_MAGZ3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGZ4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_MAGZ2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGZ3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_MAGZ1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGZ2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_MAGZ1,szTemp);

	//do the Surface Wetness
	memcpy(szTemp,pszRecord->chSurfWetness,sizeof(pszRecord->chSurfWetness))                      ;
	szTemp[sizeof(pszRecord->chSurfWetness)] = NULL;
	sscanf(szTemp,"%x",&iTemp);
	sprintf(szTemp,"%d:0x%03x",iTemp,iTemp);

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_SURFWET3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_SURFWET4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_SURFWET2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_SURFWET3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_SURFWET1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_SURFWET2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_SURFWET1,szTemp);

	//do the CPU Temperature
	memcpy(szTemp,pszRecord->cfCPUTemp,sizeof(pszRecord->cfCPUTemp))                      ;
	szTemp[sizeof(pszRecord->cfCPUTemp)] = NULL;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_CPUTEMP3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_CPUTEMP4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_CPUTEMP2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_CPUTEMP3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_CPUTEMP1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_CPUTEMP2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_CPUTEMP1,szTemp);

	//do the BOARD Temperature
	memcpy(szTemp,pszRecord->cfBOARDTemp,sizeof(pszRecord->cfBOARDTemp))                      ;
	szTemp[sizeof(pszRecord->cfBOARDTemp)] = NULL;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_BOARDTEMP3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_BOARDTEMP4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_BOARDTEMP2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_BOARDTEMP3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_BOARDTEMP1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_BOARDTEMP2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_BOARDTEMP1,szTemp);
	
	//do the count time
	memcpy(szTemp,&pszRecord->cfLastRecIntervTime10ths,sizeof(pszRecord->cfLastRecIntervTime10ths))                      ;
	szTemp[sizeof(pszRecord->cfLastRecIntervTime10ths)] = NULL;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_INTERVAL3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_INTERVAL4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_INTERVAL2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_INTERVAL3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_INTERVAL1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_INTERVAL2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_INTERVAL1,szTemp);

	//do the ESB Serial Number	
	memcpy(szTemp,pszRecord->chESBSerialNumber,sizeof(pszRecord->chESBSerialNumber))                      ;
	szTemp[sizeof(pszRecord->chESBSerialNumber)] = NULL;

	//move the saved versions all down one
	datapage->GetDlgItemText(IDC_DATA2_SERIALNUM3,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_SERIALNUM4,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_SERIALNUM2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_SERIALNUM3,cTemp);
	datapage->GetDlgItemText(IDC_DATA2_SERIALNUM1,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_SERIALNUM2,cTemp);
	datapage->SetDlgItemText(IDC_DATA2_SERIALNUM1,szTemp);
}

//record 15
void CSafePropertySheet::AcquirRec(char *Record)//, bool /*bIgnoreAuth*/)
{
//	int CheckSum;
//	if (LengthOk(Record,ACQUIRE_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//
//	if (CheckSum == CHECKSUM_GOOD)
	{
		//get pointers to page 2 and summary page
//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;
		
		char temp[32];

 		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime jTime(temp);
		
		//get status byte 
		short Status;
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status);

		//save status in binary to acquire record
		//move all the date displays down one
		datapage->SetDlgItemText(IDC_DATA_DATADATE4,
			datapage->GetText(IDC_DATA_DATADATE3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_DATADATE3,
			datapage->GetText(IDC_DATA_DATADATE2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_DATADATE2,
			datapage->GetText(IDC_DATA_DATADATE1,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_DATADATE1,jTime.Date());
	
		//move all the time displays down one
		datapage->SetDlgItemText(IDC_DATA_DATATIME4,
			datapage->GetText(IDC_DATA_DATATIME3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_DATATIME3,
			datapage->GetText(IDC_DATA_DATATIME2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_DATATIME2,
			datapage->GetText(IDC_DATA_DATATIME1,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_DATATIME1,jTime.Time());
/*	
		//move all the Neutron A count rate displays down one
		datapage->SetDlgItemText(IDC_DATA_PULSEACNT4,
			datapage->GetText(IDC_DATA_PULSEACNT3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_PULSEACNT3,
			datapage->GetText(IDC_DATA_PULSEACNT2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_PULSEACNT2,
			datapage->GetText(IDC_DATA_PULSEACNT1,temp,sizeof(temp)));
		strncpy(temp,&Record[14],8);
		temp[8]=NULL;
		datapage->SetDlgItemText(IDC_DATA_PULSEACNT1,temp);
		//set the acquire record value
	
		//move all the Neutron B count rate displays down one
		datapage->SetDlgItemText(IDC_DATA_PULSEBCNT4,
			datapage->GetText(IDC_DATA_PULSEBCNT3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_PULSEBCNT3,
			datapage->GetText(IDC_DATA_PULSEBCNT2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_PULSEBCNT2,
			datapage->GetText(IDC_DATA_PULSEBCNT1,temp,sizeof(temp)));
		strncpy(temp,&Record[22],8);
		temp[8]=NULL;
		datapage->SetDlgItemText(IDC_DATA_PULSEBCNT1,temp);
		//set the acquire record value
	
		//move all the Neutron C count rate displays down one
		datapage->SetDlgItemText(IDC_DATA_PULSECCNT4,
			datapage->GetText(IDC_DATA_PULSECCNT3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_PULSECCNT3,
			datapage->GetText(IDC_DATA_PULSECCNT2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_PULSECCNT2,
			datapage->GetText(IDC_DATA_PULSECCNT1,temp,sizeof(temp)));
		strncpy(temp,&Record[30],8);
		temp[8]=NULL;
		datapage->SetDlgItemText(IDC_DATA_PULSECCNT1,temp);
		//set the acquire record value
	
		//move all the gross gammas 1 displays down one
		datapage->SetDlgItemText(IDC_DATA_GAMMA1GR4,
			datapage->GetText(IDC_DATA_GAMMA1GR3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_GAMMA1GR3,
			datapage->GetText(IDC_DATA_GAMMA1GR2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_GAMMA1GR2,
			datapage->GetText(IDC_DATA_GAMMA1GR1,temp,sizeof(temp)));
		strncpy(temp,&Record[38],8);
		temp[8]=NULL;
		datapage->SetDlgItemText(IDC_DATA_GAMMA1GR1,temp);
		//set the acquire record value
	
		//move all the gamma 1 sigma displays down one
		datapage->SetDlgItemText(IDC_DATA_GAMMA1S4,
			datapage->GetText(IDC_DATA_GAMMA1S3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_GAMMA1S3,
			datapage->GetText(IDC_DATA_GAMMA1S2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_GAMMA1S2,
			datapage->GetText(IDC_DATA_GAMMA1S1,temp,sizeof(temp)));
		strncpy(temp,&Record[46],7);
		temp[7]=NULL;
		datapage->SetDlgItemText(IDC_DATA_GAMMA1S1,temp);
		//set the acquire record value
	
		//move all the gross gamma 2 displays down one
		datapage->SetDlgItemText(IDC_DATA_GAMMA2GR4,
			datapage->GetText(IDC_DATA_GAMMA2GR3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_GAMMA2GR3,
			datapage->GetText(IDC_DATA_GAMMA2GR2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_GAMMA2GR2,
			datapage->GetText(IDC_DATA_GAMMA2GR1,temp,sizeof(temp)));
		strncpy(temp,&Record[53],8);
		temp[8]=NULL;
		datapage->SetDlgItemText(IDC_DATA_GAMMA2GR1,temp);
		//set the acquire record value
	
		//move all the sigma 2 displays down one
		datapage->SetDlgItemText(IDC_DATA_GAMMA2S4,
			datapage->GetText(IDC_DATA_GAMMA2S3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_GAMMA2S3,
			datapage->GetText(IDC_DATA_GAMMA2S2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_GAMMA2S2,
			datapage->GetText(IDC_DATA_GAMMA2S1,temp,sizeof(temp)));
		strncpy(temp,&Record[61],7);
		temp[7]=NULL;
		datapage->SetDlgItemText(IDC_DATA_GAMMA2S1,temp);
		//set the acquire record value
*/	
		//move all the time data acquired displays down one
		datapage->SetDlgItemText(IDC_DATA_LENGTHT4,
			datapage->GetText(IDC_DATA_LENGTHT3,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_LENGTHT3,
			datapage->GetText(IDC_DATA_LENGTHT2,temp,sizeof(temp)));
		datapage->SetDlgItemText(IDC_DATA_LENGTHT2,
			datapage->GetText(IDC_DATA_LENGTHT1,temp,sizeof(temp)));
		strncpy(temp,&Record[68],5);
		temp[5]=NULL;
		datapage->SetDlgItemText(IDC_DATA_LENGTHT1,temp);
		//set the acquire record value

	}
}

//record 16
void CSafePropertySheet::DumpEnd(char *Record)
{
}

//record 61
void CSafePropertySheet::DumpEnd2(char *Record)
{
}

//record 17
void CSafePropertySheet::DumpOk(char *Record)
{
//	int CheckSum;
//	if (LengthOk(Record,DUMP_OK_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//
//	if (CheckSum == CHECKSUM_GOOD)
	{

		char temp[8];
//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CDataIStat* dataIpage    = m_pParent->m_pDataIStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;


		//get bbm bytes
		int bbm;
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		char * pCh = temp;
		while (*pCh == ' ')pCh++;
		bbm = atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
		dataIpage->SetDlgItemText(IDC_DATAI_BYTESMEM,pCh);
	}
}


bool CSafePropertySheet::CurrentSetParam(CString cIniName,CString cValue,bool bMatch,bool bTextOnly)
{
	return CurrentSetParam((LPSTR)LPCSTR(cIniName),(LPSTR)LPCSTR(cValue),bMatch,bTextOnly);
}

bool CSafePropertySheet::CurrentSetParam(char*szIniName,char*szValue,bool bMatch, bool bTextOnly)
{
//	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
	CModParam* modparpage  = m_pParent->m_pModParams;
	CSetupData* setdatapage = m_pParent->m_pSetupData;
//	CSetupData* setdatapage = m_pParent->m_pSetupData;

	modparpage->CurrentSetParam(szIniName,szValue,bMatch,true);
	bool bReturn = true;

	if (GetPageIndex(setdatapage)>=0)
	{
		if (setdatapage->CurrentSetParam(szIniName,szValue,bMatch,bTextOnly)==false)
		{
			if (!bTextOnly)
				m_iInst4Setup_Tab |= 0x0001;
			bReturn = false;
		}
	}
	return bReturn;
}
//record 1e
void CSafePropertySheet::ID2_Record(char *Record)
{
	{
		//all page 2 stuff;// CPropertySheet
		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
		CDataIStat* dataIpage    = m_pParent->m_pDataIStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		char temp[64];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		dataIpage->SetDlgItemText(IDC_DATAI_UNITID,temp);//11
		
		CurrentSetParam("SET_IRR_TIME",temp,true,true);

//
//		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
//			(ts.GetTotalSeconds() < -m_dTime_Err ))
//		{
//			//m_pParent->FlagTime(1,false);			
//			m_iDataI_Tab |= 0x0001;
//			m_bTimeError = true;
//		} 
//
//		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
//			(ts.GetTotalSeconds() > -m_dTime_Err ))
//			m_bTimeError = false;
//
//		if (m_bTimeError)
//			m_iDataI_Tab |= 0x0001;
//		long days = ts.GetDays();
//		if (days > 0)
//			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
//		else if (days == 0)
//			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
//		else 
//		{
//			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
//				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
//		}
	
		summarypage->SetDlgItemText(IDC_SUMMARY_INQUIRETIME,
			m_pParent->GetString(CInstrument::SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME));
		
		dataIpage->SetDlgItemText(IDC_DATAI_INQUIRETIME,
			m_pParent->GetString(CInstrument::DATAIPAGE,IDC_DATAI_INQUIRETIME));

		bool bMatch;

		//get Main Unit ID
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		char temp2[8];
		temp2[0] = NULL;
		dataIpage->SetDlgItemText(IDC_DATAI_UNITID,temp);//11
		GetPrivateProfileString(m_pParent->m_pName,
			"SET_IRR_BOARDSN","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = (strcmp(temp,temp2) == 0)?true:false;
		CurrentSetParam("SET_IRR_BOARDSN",temp,bMatch,false);
		if (!bMatch) m_iDataI_Tab |= 0x0002;
	
		//get status byte 1 and 2
		short Status1, Status2;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status1);
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status2);

		//bit 7 acpower 
		if (Status1 & ACPOWER)
		{
			dataIpage->SetDlgItemText(IDC_DATAI_ACPOWER,FAIL);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,FAIL);
			m_bACPowerOk = false;
		}
		else 
		{
			dataIpage->SetDlgItemText(IDC_DATAI_ACPOWER,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,OK);
			m_bACPowerOk = true;
		}

		//bit 6 battery
		if (Status1 & BATTERY) 
		{
			dataIpage->SetDlgItemText(IDC_DATAI_BATTERYOK,LOW);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,LOW);
			m_bBatteryOk = false;
		}
		else
		{
			dataIpage->SetDlgItemText(IDC_DATAI_BATTERYOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,OK);
			m_bBatteryOk = true;
		}
	
		//bit 5 tamper alarm
		if (Status1 & TAMPER)
			m_bCountRateTamperOk = false;
		else
			m_bCountRateTamperOk = true;

		//bit 4 RMS Sigma Tamper
		if (Status1 & RMSTAMPER)
			m_bRMSSigmaTamperOk = false;
		else
			m_bRMSSigmaTamperOk = true;

		//bit 3 bbm
		if (Status1 & BBMERROR)
		{
			char cBuff[1024];
			if (Status2 & BBMWRITEERROR_FLAG)
			{
				strcpy(temp,BBMWRITEERROR);
				strcpy(cBuff,"16870");
			}
			else if (Status2 & BBMNUMOFBYTES_FLAG)
			{
				strcpy(temp,BBMNUMOFBYTES);
				strcpy(cBuff,"16880");
			}
			else if (Status2 & BBMISFULLEDER_FLAG)
			{
				strcpy(temp,BBMISFULLEDER);
				strcpy(cBuff,"16890");
			}
			else
			{
				strcpy(temp,BBMINVALIDOPC);
				strcpy(cBuff,"16900");
			}
			strcat(cBuff," STATUS CHANGE (cont) - ");

			dataIpage->SetDlgItemText(IDC_DATAI_BBMOK,temp);
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,temp);

			m_bBBMOk = false;
			m_iDataI_Tab |= 0x0001;
		}
		else
		{
			dataIpage->SetDlgItemText(IDC_DATAI_BBMOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,OK);
			m_bBBMOk = true;
		}
	
		//bit 2 startup
		if (Status1 & CWSTART) 
		{
			dataIpage->SetDlgItemText(IDC_DATAI_OPERATION,START_COLD);
			m_dCWStart = 0;
		}
		else
		{
			dataIpage->SetDlgItemText(IDC_DATAI_OPERATION,START_NORMAL);
			m_dCWStart = 1;
		}
	
		//bit 0 mii
		if (Status1 & INMII)
		{
			dataIpage->SetDlgItemText(IDC_DATAI_OOMII,INMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,INMII_TXT);
			m_bInMII = true;
		}
		else
		{
			dataIpage->SetDlgItemText(IDC_DATAI_OOMII,NOTINMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,NOTINMII_TXT);
			m_bInMII = false;
		}
	
		//bit 7 of sw 2
		if (Status2 & DATAFILTER_FLAG)
		{
			dataIpage->SetDlgItemText(IDC_DATAI_DATAFILTER,DATAFILTER_NO);
			if (!m_bDataFiltered && m_pParent->m_bLogFilter)
				m_bDataFiltered = true;
		}
		else
		{
			dataIpage->SetDlgItemText(IDC_DATAI_DATAFILTER,DATAFILTER_YES);
			if (m_bDataFiltered && m_pParent->m_bLogFilter)
				m_bDataFiltered = false;
		}

		//get bbm
		strncpy(temp,&Record[18],6);
		temp[6]=NULL;
		char * pCh = temp;
		while (*pCh == ' ')pCh++;
		dataIpage->SetDlgItemText(IDC_DATAI_BYTESMEM,pCh);
		int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
	
		m_iDataI_Tab = m_pParent->m_iDataI_Tab;
		if (m_iDataI_Tab == 0x0003)
			SetTabColor(IDC_DATAI_TAB,COLOR_YELLOWRED);
		else if (m_iDataI_Tab == 0x0002)
			SetTabColor(IDC_DATAI_TAB,COLOR_YELLOW);
		else if (m_iDataI_Tab == 0x0001)
			SetTabColor(IDC_DATAI_TAB,COLOR_RED);
		else if (m_iDataI_Tab == 0x0000)
			SetTabColor(IDC_DATAI_TAB,COLOR_GREEN);
	}
}

//record 1f
void CSafePropertySheet::AnalyzeOk(char *Record)
{
	char temp[8];
	strncpy(temp,&Record[2],6);
	temp[6] = NULL;
	int bbm=atoi(temp);
	m_pCloseButton->SetNumeric(bbm,true);
}

//record 30
void CSafePropertySheet::InformRec(char *Record)
{
}

//record 4d
void CSafePropertySheet::AnalyzeOk3Record(char *Record)
{
	char temp[8];
	strncpy(temp,&Record[2],6);
	temp[6] = NULL;
	int bbm=atoi(temp);
	m_pCloseButton->SetNumeric(bbm,true);
}

//record 4f
void CSafePropertySheet::DumpOk3_Record(char *Record)
{
	{

		char temp[8];
//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		//get bbm bytes
		// SCR00235 Processing of 4F DUMPOKRESPONSE
		unsigned long ulbbm;
		strncpy(temp,&Record[2],10);
		temp[10] = NULL;
		char *pCh = temp;
		while (*pCh == ' ')pCh++;
		//bbm = (int)atoi(temp);
		sscanf(temp,"%d",&ulbbm);
		m_pCloseButton->SetNumeric(ulbbm,true);
		datapage->SetDlgItemText(IDC_DATAI_BYTESMEM,pCh);
	}
}

//record 4e
void CSafePropertySheet::ID3_Record(char *Record)
{
//	AdjustTabsTo(POST4);

//	int CheckSum;
//	LengthOk(Record,MONITOR_ID3_RECORD_LENGTH);
//	if (true)
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		//all page 2 stuff;// CPropertySheet
		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;
	
		char temp[64];

		//get julian time
//		strncpy(temp,&Record[2],10);
//		temp[10]=NULL;
//		CJulianTime Julian(temp);
//
//		if (Julian.Year() < 1990)
//		{
//			datapage->SetColor(IDC_DATA_INQUIRETIME,COLOR_RED);
//		}
//
//		//post invalid time to CEV if necessary
//		if (m_szPreviousGTime[0] != NULL) 
//		{
//			if (strcmp(m_szPreviousGTime,temp)>0)
//			{
//				CJulianTime JulianT(m_szPreviousGTime);
//				datapage->SetColor(IDC_DATA_INQUIRETIME,COLOR_RED);
//				summarypage->SetColor(IDC_SUMMARY_INQUIRETIME,COLOR_RED);
//			}
//		}
//
//		strcpy(m_szPreviousGTime,temp);
//
//		if ((m_bStartOfFile)||(Julian.Day()!=m_dCurrentDay))
//		{
//			m_bStartOfFile = false;
//			m_dCurrentDay = Julian.Day();
//		}
//
//		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
//			      Julian.Hour(), Julian.Minute(), Julian.Second() );
//		COleDateTime t2 = COleDateTime::GetCurrentTime();
//		COleDateTimeSpan ts = t1 - t2;  // Subtract 2 COleDateTimes
//		ts += HALF_SECOND;
//
//		strcpy(temp,Julian.Date());
//		strcat(temp," ");
//		strcat(temp,Julian.Time());
//
//		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
//			(ts.GetTotalSeconds() < -m_dTime_Err ))
//			m_bTimeError = true;
//
//		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
//			(ts.GetTotalSeconds() > -m_dTime_Err ))
//			m_bTimeError = false;
//
//		long days = ts.GetDays();
//		if (days > 0)
//			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
//		else if (days == 0)
//			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
//		else 
//		{
//			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
//				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
//		}
//		datapage->SetDlgItemText(IDC_DATA_INQUIRETIME,temp);
//		summarypage->SetDlgItemText(IDC_SUMMARY_INQUIRETIME,temp);

		summarypage->SetDlgItemText(IDC_SUMMARY_INQUIRETIME,
			m_pParent->GetString(CInstrument::SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME));
		
		datapage->SetDlgItemText(IDC_DATAI_INQUIRETIME,
			m_pParent->GetString(CInstrument::DATAIPAGE,IDC_DATAI_INQUIRETIME));

		bool bMatch;

		//get Main Unit ID
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		char temp2[8];
		temp2[0] = NULL;
		datapage->SetDlgItemText(IDC_DATAI_UNITID,temp);
		GetPrivateProfileString(m_pParent->m_pName,"SET_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_UNIT_ID",temp,bMatch);
		
		//get status byte 1 and 2
		short Status1, Status2;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status1);
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status2);

		//bit 7 acpower
		if (Status1 & ACPOWER)
		{
			datapage->SetDlgItemText(IDC_DATAI_ACPOWER,FAIL);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,FAIL);
			m_bACPowerOk = false;
		}
		else 
		{
			datapage->SetDlgItemText(IDC_DATAI_ACPOWER,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,OK);
			m_bACPowerOk = true;
		}
	
		//bit 6 battery
		if (Status1 & BATTERY) 
		{
			datapage->SetDlgItemText(IDC_DATAI_BATTERYOK,LOW);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,LOW);
			m_bBatteryOk = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATAI_BATTERYOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,OK);
			m_bBatteryOk = true;
		}
	
		//bit 5 tamper alarm
		if (Status1 & TAMPER)
			m_bCountRateTamperOk = false;
		else
			m_bCountRateTamperOk = true;

		//bit 4 RMS Sigma Tamper
		if (Status1 & RMSTAMPER)
			m_bRMSSigmaTamperOk = false;
		else
			m_bRMSSigmaTamperOk = true;

		//bit 3 bbm
		if (Status1 & BBMERROR)
		{
			char cBuff[1024];
			if (Status2 & BBMWRITEERROR_FLAG)
			{
				strcpy(temp,BBMWRITEERROR);
				strcpy(cBuff,"16870");
			}
			else if (Status2 & BBMNUMOFBYTES_FLAG)
			{
				strcpy(temp,BBMNUMOFBYTES);
				strcpy(cBuff,"16880");
			}
			else if (Status2 & BBMISFULLEDER_FLAG)
			{
				strcpy(temp,BBMISFULLEDER);
				strcpy(cBuff,"16890");
			}
			else
			{
				strcpy(temp,BBMINVALIDOPC);
				strcpy(cBuff,"16900");
			}
			strcat(cBuff," STATUS CHANGE (cont) - ");

			datapage->SetDlgItemText(IDC_DATAI_BBMOK,temp);
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,temp);

			m_bBBMOk = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATAI_BBMOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,OK);
			m_bBBMOk = true;
		}
	
		//bit 2 startup
		if (Status1 & CWSTART) 
		{
			datapage->SetDlgItemText(IDC_DATAI_OPERATION,START_COLD);
			m_dCWStart = 0;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATAI_OPERATION,START_NORMAL);
			m_dCWStart = 1;
		}
	
		//bit 1 reserved

		//bit 0 mii
		if (Status1 & INMII)
		{
			datapage->SetDlgItemText(IDC_DATAI_OOMII,INMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,INMII_TXT);
			m_bInMII = true;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATAI_OOMII,NOTINMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,NOTINMII_TXT);
			m_bInMII = false;
		}
	
		//bit 7 of sw 2
		if (Status2 & DATAFILTER_FLAG)
		{
			datapage->SetDlgItemText(IDC_DATAI_DATAFILTER,DATAFILTER_NO);
			m_bDataFiltered = true;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATAI_DATAFILTER,DATAFILTER_YES);
			m_bDataFiltered = false;
		}

		//get bbm
		strncpy(temp,&Record[18],10);
		temp[10]=NULL;
		char *pCh = temp;
		while (*pCh == ' ')pCh++;
		datapage->SetDlgItemText(IDC_DATAI_BYTESMEM,pCh);
		// SCR00235 Processing of 4F DUMPOKRESPONSE
		unsigned long ulbbm;
		sscanf(temp,"%d",&ulbbm);
		//int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(ulbbm,true);
	
		m_iData_Tab = m_pParent->m_iData_Tab;
		if (m_pParent->m_iData_Tab == 0x0003)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
		else if (m_iData_Tab == 0x0002)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
		else if (m_iData_Tab == 0x0001)
			SetTabColor(IDC_DATA_TAB,COLOR_RED);
		else if (m_iData_Tab == 0x0000)
			SetTabColor(IDC_DATA_TAB,COLOR_GREEN);
	}
}

void 
CSafePropertySheet::OnuGConfigRecord(char* szRecord)			
{

	//will need a few work areas
	CString cTemp;
	char szTemp1[64];
	char szTemp2[256];
	char szTemp3[16];
	char szTempX[256];
	int iTemp;
	char *pAt;
	CUGINFO		*pUGInfoPage = m_pParent->m_pUGInfo;

	//do we have a change
	bool bChange = false;

	//do we have a match
	bool bMatch;

	//get a pointer to a GeneralConfigTextRecord
	uGConfigTextRecord* pszRecord = (uGConfigTextRecord*)szRecord;

 	//get julian time and put it in the binary record
	strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;

	//set the date & time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//set date and time
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_DATADATEC,cTime.Date());
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_DATATIMEC,cTime.Time());
	sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
	CurrentSetParam("SET_UGGCR_TIME",szTemp1,true);

	//set the acquire time
	strncpy(szTemp1,(char*)pszRecord->cdAcqInterval10ths,sizeof(pszRecord->cdAcqInterval10ths));
	szTemp1[sizeof(pszRecord->cdAcqInterval10ths)]=NULL;
	sscanf(szTemp1,"%d",&iTemp);
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_COUNTTIME","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	sprintf(szTemp2,"%10.1f",(float)iTemp/10.0);
	pAt = szTemp2;
	while (*pAt == ' ') pAt++;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_ACQINTC,pAt);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_ACQINTC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_COUNTTIME",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set the baud and frame
	strncpy(szTemp1,(char*)pszRecord->cdBaudFrame,sizeof(pszRecord->cdBaudFrame));
	szTemp1[sizeof(pszRecord->cdBaudFrame)]=NULL;

	switch(szTemp1[0]) {
	case '0':strcpy(szTemp2 ,"300");break;
	case '1':strcpy(szTemp2 ,"600");break;
	case '2':strcpy(szTemp2 ,"1200");break;
	case '3':strcpy(szTemp2 ,"2400");break;
	case '4':strcpy(szTemp2 ,"4800");break;
	case '5':strcpy(szTemp2 ,"9600");break;
	case '6':strcpy(szTemp2 ,"19200");break;
	case '7':strcpy(szTemp2 ,"38400");break;
	case '8':strcpy(szTemp2 ,"57600");break;
	case '9':strcpy(szTemp2 ,"115200");break;
	default:strcpy(szTemp2 ,"ERR");
	}

	switch(szTemp1[1]) {
	case '0':strcpy(szTemp3,"7:O");break;
	case '1':strcpy(szTemp3,"7:E");break;
	case '2':strcpy(szTemp3,"7:N");break;
	case '3':strcpy(szTemp3,"8:O");break;
	case '4':strcpy(szTemp3,"8:E");break;
	case '5':strcpy(szTemp3,"8:N");break;
	default:strcpy(szTemp3,"ERR");
	}

	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_BAUDRATE","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp2,szTempX) == 0)?true:false;
	CurrentSetParam("SET_UGGCR_BAUDRATE",szTemp2,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_FRAME","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp3,szTempX) == 0)?true:false;
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_BAUDFRAMEC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_FRAME",szTemp3,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	strcat(szTemp2,":");
	strcat(szTemp2,szTemp3);
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_BAUDFRAMEC,szTemp2);

	//set PHD V. Setpoint
	strncpy(szTemp1,(char*)pszRecord->cdPHDVSetPoint,sizeof(pszRecord->cdPHDVSetPoint));
	szTemp1[sizeof(pszRecord->cdPHDVSetPoint)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_PHDV","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_PHDSETPTC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_PHDSETPTC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_PHDV",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set CPU Speed
	strncpy(szTemp1,(char*)pszRecord->chCPUSpeed,sizeof(pszRecord->chCPUSpeed));
	szTemp1[sizeof(pszRecord->chCPUSpeed)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_OSCSET","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CPUSPEEDC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_CPUSPEEDC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_OSCSET",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set Mode Flags
	strncpy(szTemp1,(char*)pszRecord->chModeFlags,sizeof(pszRecord->chModeFlags));
	szTemp1[sizeof(pszRecord->chModeFlags)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_MODEFLAGS","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_MODEFLAGC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_MODEFLAGC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_MODEFLAGS",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set Filter Control
	strncpy(szTemp1,(char*)pszRecord->chFilterControl,sizeof(pszRecord->chFilterControl));
	szTemp1[sizeof(pszRecord->chFilterControl)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_FILTERCONT","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	CurrentSetParam("SET_UGGCR_FILTERCONT",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	sscanf(szTemp1,"%x",&iTemp);
	szTemp2[0] = NULL;
	if (iTemp & 0x00000080) strcat(szTemp2,"Kal ");
	if (iTemp & 0x00000004) strcat(szTemp2,"f/a ");
	if (iTemp & 0x00000002) strcat(szTemp2,"s ");
	if (iTemp & 0x00000001) strcat(szTemp2,"e");
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_FILTERCNTC,szTemp2);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_FILTERCNTC,COLOR_YELLOW);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	
	//set immediate buffer size
	strncpy(szTemp1,(char*)pszRecord->cdImmBuffSize,sizeof(pszRecord->cdImmBuffSize));
	szTemp1[sizeof(pszRecord->cdImmBuffSize)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_IMMBUFSIZE","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_IMMBUFSIZEC,COLOR_YELLOW);
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_IMMBUFSIZEC,szTemp1);
	CurrentSetParam("SET_UGGCR_IMMBUFSIZE",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set immediate buffer save size
	strncpy(szTemp1,(char*)pszRecord->cdImmBuffSaveSize,sizeof(pszRecord->cdImmBuffSaveSize));
	szTemp1[sizeof(pszRecord->cdImmBuffSaveSize)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_IMMBUFFSAVESIZE","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_IMMBUFSAVEC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_IMMBUFSAVEC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_IMMBUFFSAVESIZE",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set local background size
	strncpy(szTemp1,(char*)pszRecord->cdLocalBGSize,sizeof(pszRecord->cdLocalBGSize));
	szTemp1[sizeof(pszRecord->cdLocalBGSize)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_LOCBACKSIZE","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_LOCBGSIZEC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_LOCBGSIZEC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_LOCBACKSIZE",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set local background end
	strncpy(szTemp1,(char*)pszRecord->cdLocalBGEnd,sizeof(pszRecord->cdLocalBGEnd));
	szTemp1[sizeof(pszRecord->cdLocalBGEnd)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_LOCBACKEND","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_LOCBGENDC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_LOCBGENDC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_LOCBACKEND",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set Status record interval
	strncpy(szTemp1,(char*)pszRecord->cdStatusRecInterv,sizeof(pszRecord->cdStatusRecInterv));
	szTemp1[sizeof(pszRecord->cdStatusRecInterv)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_STATUSINT","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_STATUSRECINTC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_STATUSRECINTC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_STATUSINT",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set time sync state
	strncpy(szTemp1,(char*)&pszRecord->chTimeSyncState,sizeof(pszRecord->chTimeSyncState));
	szTemp1[sizeof(pszRecord->chTimeSyncState)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_TIMESYNC_STATE","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	szTemp2[0]=NULL;
	if (szTemp1[0]=='1') strcpy(szTemp2,"Daily");
	if (szTemp1[0]=='2') strcpy(szTemp2,"Hourly");
	if (szTemp1[0]=='3') strcpy(szTemp2,"Daily,Hourly");
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_TIMESYNCSTC,szTemp2);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_TIMESYNCSTC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_TIMESYNC_STATE",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set hourly time sync time
	strncpy(szTemp1,(char*)pszRecord->cdHourlyTimeSync,sizeof(pszRecord->cdHourlyTimeSync));
	szTemp1[sizeof(pszRecord->cdHourlyTimeSync)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_TIMESYNC_HOUR","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	sprintf(szTemp2,"%c%c:%c%c",szTemp1[0],szTemp1[1],szTemp1[2],szTemp1[3]);
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_HOURLYTSC,szTemp2);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_HOURLYTSC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_TIMESYNC_HOUR",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set daily time sync time
	strncpy(szTemp1,(char*)pszRecord->cdDailyTimeSync,sizeof(pszRecord->cdDailyTimeSync));
	szTemp1[sizeof(pszRecord->cdDailyTimeSync)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_TIMESYNC_DAILY","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	sprintf(szTemp2,"%c%c:%c%c:%c%c",szTemp1[0],szTemp1[1],szTemp1[2],szTemp1[3],szTemp1[4],szTemp1[5]);
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_DAILYTSC,szTemp2);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_DAILYTSC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_TIMESYNC_DAILY",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set time sync lower tolerance
	strncpy(szTemp1,(char*)pszRecord->cdTimeSyncLowerTol,sizeof(pszRecord->cdTimeSyncLowerTol));
	szTemp1[sizeof(pszRecord->cdTimeSyncLowerTol)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_TIMESYNC_LOWTOL","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pAt = szTemp1;
	while (*pAt == ' ') pAt++;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_TSLOWTOLC,pAt);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_TSLOWTOLC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_TIMESYNC_LOWTOL",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set time sync upper tolerance
	strncpy(szTemp1,(char*)pszRecord->cdTimeSyncUpperTol,sizeof(pszRecord->cdTimeSyncUpperTol));
	szTemp1[sizeof(pszRecord->cdTimeSyncUpperTol)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_TIMESYNC_UPPTOL","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pAt = szTemp1;
	while (*pAt == ' ') pAt++;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_TSUPTOLC,pAt);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_TSUPTOLC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_TIMESYNC_UPPTOL",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set inspector ID
	strncpy(szTemp1,(char*)pszRecord->cdInspectorID,sizeof(pszRecord->cdInspectorID));
	szTemp1[sizeof(pszRecord->cdInspectorID)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_INSPECTOR_ID","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_INSPECTORC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_INSPECTORC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_INSPECTOR_ID",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set unit ID
	strncpy(szTemp1,(char*)&pszRecord->chUnitID,sizeof(pszRecord->chUnitID));
	szTemp1[sizeof(pszRecord->chUnitID)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_UNIT_ID","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_UNITIDC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_UNITIDC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_UNIT_ID",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set impedance settings
	strncpy(szTemp1,(char*)&pszRecord->chImpedanceSet,sizeof(pszRecord->chImpedanceSet));
	szTemp1[sizeof(pszRecord->chImpedanceSet)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_IMPEDANCE_SET","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	CurrentSetParam("SET_UGGCR_IMPEDANCE_SET",szTemp1,true,false);
	sscanf(szTemp1,"%x",&iTemp);
	szTemp2[0]=NULL;
	if (iTemp & 0x01) strcat(szTemp2,"Ch0:50 "); else strcat(szTemp2,"Ch0:10K ");
	if (iTemp & 0x02) strcat(szTemp2,"Ch1:50 "); else strcat(szTemp2,"Ch1:10K ");
	if (iTemp & 0x04) strcat(szTemp2,"Ch2:50 "); else strcat(szTemp2,"Ch2:10K ");
	if (iTemp & 0x08) strcat(szTemp2,"Ch3:50");  else strcat(szTemp2,"Ch3:10K");
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_IMPEDSETSC,COLOR_YELLOW);
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_IMPEDSETSC,szTemp2);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set ch 0 kalman r
	strncpy(szTemp1,(char*)pszRecord->cfCh0KalmanR,sizeof(pszRecord->cfCh0KalmanR));
	szTemp1[sizeof(pszRecord->cfCh0KalmanR)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_CH0KALR","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CH0KALRC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_CH0KALRC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_CH0KALR",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set ch 1 kalman r
	strncpy(szTemp1,(char*)pszRecord->cfCh1KalmanR,sizeof(pszRecord->cfCh1KalmanR));
	szTemp1[sizeof(pszRecord->cfCh1KalmanR)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_CH1KALR","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CH1KALRC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_CH1KALRC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_CH1KALR",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set ch 2 kalman r
	strncpy(szTemp1,(char*)pszRecord->cfCh2KalmanR,sizeof(pszRecord->cfCh2KalmanR));
	szTemp1[sizeof(pszRecord->cfCh2KalmanR)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_CH2KALR","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CH2KALRC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_CH2KALRC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_CH2KALR",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set ch 3 kalman r
	strncpy(szTemp1,(char*)pszRecord->cfCh3KalmanR,sizeof(pszRecord->cfCh3KalmanR));
	szTemp1[sizeof(pszRecord->cfCh3KalmanR)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_CH3KALR","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CH3KALRC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_CH3KALRC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_CH3KALR",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set ch 0 kalman Q
	strncpy(szTemp1,(char*)pszRecord->cfCh0KalmanQ,sizeof(pszRecord->cfCh0KalmanQ));
	szTemp1[sizeof(pszRecord->cfCh0KalmanQ)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_CH0KALQ","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CH0KALQC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_CH0KALQC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_CH0KALQ",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set ch 1 kalman Q
	strncpy(szTemp1,(char*)pszRecord->cfCh1KalmanQ,sizeof(pszRecord->cfCh1KalmanQ));
	szTemp1[sizeof(pszRecord->cfCh1KalmanQ)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_CH1KALQ","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CH1KALQC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_CH1KALQC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_CH1KALQ",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set ch 2 kalman Q
	strncpy(szTemp1,(char*)pszRecord->cfCh2KalmanQ,sizeof(pszRecord->cfCh2KalmanQ));
	szTemp1[sizeof(pszRecord->cfCh2KalmanQ)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_CH2KALQ","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CH2KALQC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_CH2KALQC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_CH2KALQ",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set ch 3 kalman Q
	strncpy(szTemp1,(char*)pszRecord->cfCh3KalmanQ,sizeof(pszRecord->cfCh3KalmanQ));
	szTemp1[sizeof(pszRecord->cfCh3KalmanQ)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGGCR_CH3KALQ","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CH3KALQC,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_CH3KALQC,COLOR_YELLOW);
	CurrentSetParam("SET_UGGCR_CH3KALQ",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	if (m_iuGInfo_Tab == 0x0003)
		SetTabColor(IDC_UGINFO_TAB,COLOR_YELLOWRED);
	else if (m_iuGInfo_Tab == 0x0002)
		SetTabColor(IDC_UGINFO_TAB,COLOR_YELLOW);
	else if (m_iuGInfo_Tab == 0x0001)
		SetTabColor(IDC_UGINFO_TAB,COLOR_RED);
	else if (m_iuGInfo_Tab == 0x0000)
		SetTabColor(IDC_UGINFO_TAB,COLOR_GREEN);

	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);

}

void 
CSafePropertySheet::OnuGChConfigRecord(char* szRecord)		
{
	//some temporary buffers
	char szTemp1[32];
	unsigned short usTemp;

	//we want to send stuff to the correct property page
	CUser4Prog* user4page    = m_pParent->m_pUser4ProgStatus;

	//get a pointer to a text record
	uGChConfigTextRecord* psuGChConfigTextRecord = (uGChConfigTextRecord*)szRecord;

	//get the julian seconds
	memcpy(szTemp1,psuGChConfigTextRecord->cdJulianSeconds,sizeof(psuGChConfigTextRecord->cdJulianSeconds));
	szTemp1[sizeof(psuGChConfigTextRecord->cdJulianSeconds)]=NULL;
	//set the date and time
	CJulianTime cTime;
	cTime.Init(szTemp1);
	
	//display the date and time
	sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
	CurrentSetParam("SET_CCR_TIME",szTemp1,true);
	user4page->SetDlgItemText(IDC_USER4PROG_TIME2,szTemp1);

	//get the used for analysis
	memcpy(szTemp1,&psuGChConfigTextRecord->chUsedForAnalysis,sizeof(psuGChConfigTextRecord->chUsedForAnalysis));
	szTemp1[sizeof(psuGChConfigTextRecord->chUsedForAnalysis)]=NULL;
	sscanf(szTemp1,"%x",&usTemp);
	if (!CurrentSetParam("SET_CCR_USED",szTemp1,false))
	{
		unsigned short usTemp1;
		GetPrivateProfileString(m_pParent->m_pName,"SET_CCR_USED","",szTemp1,sizeof(szTemp1),m_szIniFile);
		sscanf(szTemp1,"%x",&usTemp1);
		usTemp1 = usTemp1 ^ usTemp; 
		if (usTemp1 & 0x0001) user4page->SetColor(IDC_USER4PROG_CH0USED,COLOR_YELLOW);
		if (usTemp1 & 0x0002) user4page->SetColor(IDC_USER4PROG_CH1USED,COLOR_YELLOW);
		if (usTemp1 & 0x0004) user4page->SetColor(IDC_USER4PROG_CH2USED,COLOR_YELLOW);
		if (usTemp1 & 0x0008) user4page->SetColor(IDC_USER4PROG_CH3USED,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_CH0USED,(usTemp & 0x0001)?"Chan. 0":"Not Used");
	user4page->SetDlgItemText(IDC_USER4PROG_CH1USED,(usTemp & 0x0002)?"Chan. 1":"Not Used");
	user4page->SetDlgItemText(IDC_USER4PROG_CH2USED,(usTemp & 0x0004)?"Chan. 2":"Not Used");
	user4page->SetDlgItemText(IDC_USER4PROG_CH3USED,(usTemp & 0x0008)?"Chan. 3":"Not Used");


	//get threshold 1 type and direction
	memcpy(szTemp1,psuGChConfigTextRecord->chThresh1TypeDir,sizeof(psuGChConfigTextRecord->chThresh1TypeDir));
	szTemp1[sizeof(psuGChConfigTextRecord->chThresh1TypeDir)]=NULL;
	sscanf(szTemp1,"%x",&usTemp);
	if (!CurrentSetParam("SET_CCR_T1TYPEDIR",szTemp1,false))
	{
		unsigned short usTemp1;
		GetPrivateProfileString(m_pParent->m_pName,"SET_CCR_T1TYPEDIR","",szTemp1,sizeof(szTemp1),m_szIniFile);
		sscanf(szTemp1,"%x",&usTemp1);
		usTemp1 = usTemp1 ^ usTemp; 
		if (usTemp1 & 0x00c0) user4page->SetColor(IDC_USER4PROG_C00,COLOR_YELLOW);
		if (usTemp1 & 0x0030) user4page->SetColor(IDC_USER4PROG_C10,COLOR_YELLOW);
		if (usTemp1 & 0x000c) user4page->SetColor(IDC_USER4PROG_C20,COLOR_YELLOW);
		if (usTemp1 & 0x0003) user4page->SetColor(IDC_USER4PROG_C30,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	strcpy(szTemp1,(usTemp & 0x0080)?"Rel/":"Abs/");
	strcat(szTemp1,(usTemp & 0x0040)?"+":"-");
	user4page->SetDlgItemText(IDC_USER4PROG_C00,szTemp1);
	strcpy(szTemp1,(usTemp & 0x0020)?"Rel/":"Abs/");
	strcat(szTemp1,(usTemp & 0x0010)?"+":"-");
	user4page->SetDlgItemText(IDC_USER4PROG_C10,szTemp1);
	strcpy(szTemp1,(usTemp & 0x0008)?"Rel/":"Abs/");
	strcat(szTemp1,(usTemp & 0x0004)?"+":"-");
	user4page->SetDlgItemText(IDC_USER4PROG_C20,szTemp1);
	strcpy(szTemp1,(usTemp & 0x0002)?"Rel/":"Abs/");
	strcat(szTemp1,(usTemp & 0x0001)?"+":"-");
	user4page->SetDlgItemText(IDC_USER4PROG_C30,szTemp1);

	//get threshold 2 type and direction
	memcpy(szTemp1,psuGChConfigTextRecord->chThresh2TypeDir,sizeof(psuGChConfigTextRecord->chThresh2TypeDir));
	szTemp1[sizeof(psuGChConfigTextRecord->chThresh2TypeDir)]=NULL;
	sscanf(szTemp1,"%x",&usTemp);
	if (!CurrentSetParam("SET_CCR_T2TYPEDIR",szTemp1,false))
	{
		unsigned short usTemp1;
		GetPrivateProfileString(m_pParent->m_pName,"SET_CCR_T2TYPEDIR","",szTemp1,sizeof(szTemp1),m_szIniFile);
		sscanf(szTemp1,"%x",&usTemp1);
		usTemp1 = usTemp1 ^ usTemp; 
		if (usTemp1 & 0x00c0) user4page->SetColor(IDC_USER4PROG_C03,COLOR_YELLOW);
		if (usTemp1 & 0x0030) user4page->SetColor(IDC_USER4PROG_C13,COLOR_YELLOW);
		if (usTemp1 & 0x000c) user4page->SetColor(IDC_USER4PROG_C23,COLOR_YELLOW);
		if (usTemp1 & 0x0003) user4page->SetColor(IDC_USER4PROG_C33,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	strcpy(szTemp1,(usTemp & 0x0080)?"Rel/":"Abs/");
	strcat(szTemp1,(usTemp & 0x0040)?"+":"-");
	user4page->SetDlgItemText(IDC_USER4PROG_C03,szTemp1);
	strcpy(szTemp1,(usTemp & 0x0020)?"Rel/":"Abs/");
	strcat(szTemp1,(usTemp & 0x0010)?"+":"-");
	user4page->SetDlgItemText(IDC_USER4PROG_C13,szTemp1);
	strcpy(szTemp1,(usTemp & 0x0008)?"Rel/":"Abs/");
	strcat(szTemp1,(usTemp & 0x0004)?"+":"-");
	user4page->SetDlgItemText(IDC_USER4PROG_C23,szTemp1);
	strcpy(szTemp1,(usTemp & 0x0002)?"Rel/":"Abs/");
	strcat(szTemp1,(usTemp & 0x0001)?"+":"-");
	user4page->SetDlgItemText(IDC_USER4PROG_C33,szTemp1);

	//get ch0 threshold 1 value 
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh0Thresh1Value,sizeof(psuGChConfigTextRecord->cdfCh0Thresh1Value));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh0Thresh1Value)]=NULL;
	if (!CurrentSetParam("SET_CCR_C0T1VALUE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C01,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C01,szTemp1);

	//get ch1 threshold 1 value 
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh1Thresh1Value,sizeof(psuGChConfigTextRecord->cdfCh1Thresh1Value));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh1Thresh1Value)]=NULL;
	if (!CurrentSetParam("SET_CCR_C1T1VALUE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C11,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C11,szTemp1);

	//get ch2 threshold 1 value
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh2Thresh1Value,sizeof(psuGChConfigTextRecord->cdfCh2Thresh1Value));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh2Thresh1Value)]=NULL;
	if (!CurrentSetParam("SET_CCR_C2T1VALUE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C21,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C21,szTemp1);

	//get ch3 threshold 1 value
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh3Thresh1Value,sizeof(psuGChConfigTextRecord->cdfCh3Thresh1Value));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh3Thresh1Value)]=NULL;
	if (!CurrentSetParam("SET_CCR_C3T1VALUE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C31,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C31,szTemp1);

	//get ch0 threshold 2 value 
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh0Thresh2Value,sizeof(psuGChConfigTextRecord->cdfCh0Thresh2Value));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh0Thresh2Value)]=NULL;
	if (!CurrentSetParam("SET_CCR_C0T2VALUE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C04,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C04,szTemp1);

	//get ch1 threshold 2 value 
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh1Thresh2Value,sizeof(psuGChConfigTextRecord->cdfCh1Thresh2Value));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh1Thresh2Value)]=NULL;
	if (!CurrentSetParam("SET_CCR_C1T2VALUE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C14,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C14,szTemp1);

	//get ch2 threshold 2 value
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh2Thresh2Value,sizeof(psuGChConfigTextRecord->cdfCh2Thresh2Value));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh2Thresh2Value)]=NULL;
	if (!CurrentSetParam("SET_CCR_C2T2VALUE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C24,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C24,szTemp1);

	//get ch3 threshold 2 value
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh3Thresh2Value,sizeof(psuGChConfigTextRecord->cdfCh3Thresh2Value));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh3Thresh2Value)]=NULL;
	if (!CurrentSetParam("SET_CCR_C3T2VALUE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C34,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C34,szTemp1);

	//get ch0 threshold 1 entry/exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh0Thresh1EntryExit,sizeof(psuGChConfigTextRecord->chCh0Thresh1EntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh0Thresh1EntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C0T1EE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C02,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C02,szTemp1);

	//get ch1 threshold 1 entry/exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh1Thresh1EntryExit,sizeof(psuGChConfigTextRecord->chCh1Thresh1EntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh1Thresh1EntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C1T1EE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C12,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C12,szTemp1);

	//get ch2 threshold 1 entry/exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh2Thresh1EntryExit,sizeof(psuGChConfigTextRecord->chCh2Thresh1EntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh2Thresh1EntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C2T1EE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C22,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C22,szTemp1);

	//get ch3 threshold 1 entry/exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh3Thresh1EntryExit,sizeof(psuGChConfigTextRecord->chCh3Thresh1EntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh3Thresh1EntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C3T1EE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C32,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C32,szTemp1);

	//get ch0 threshold 2 entry/exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh0Thresh2EntryExit,sizeof(psuGChConfigTextRecord->chCh0Thresh2EntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh0Thresh2EntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C0T2EE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C05,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C05,szTemp1);

	//get ch1 threshold 2 entry/exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh1Thresh2EntryExit,sizeof(psuGChConfigTextRecord->chCh1Thresh2EntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh2Thresh2EntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C1T2EE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C15,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C15,szTemp1);

	//get ch2 threshold 2 entry/exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh2Thresh2EntryExit,sizeof(psuGChConfigTextRecord->chCh2Thresh2EntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh2Thresh2EntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C2T2EE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C25,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C25,szTemp1);

	//get ch3 threshold 2 entry/exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh3Thresh2EntryExit,sizeof(psuGChConfigTextRecord->chCh3Thresh2EntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh3Thresh2EntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C3T2EE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C35,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C35,szTemp1);
	
	//get ch0 changing signal mult
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh0ChangSigMultip,sizeof(psuGChConfigTextRecord->cdfCh0ChangSigMultip));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh0ChangSigMultip)]=NULL;
	if (!CurrentSetParam("SET_CCR_C0CCM",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C06,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C06,szTemp1);

	//get ch1 changing signal mult
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh1ChangSigMultip,sizeof(psuGChConfigTextRecord->cdfCh1ChangSigMultip));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh1ChangSigMultip)]=NULL;
	if (!CurrentSetParam("SET_CCR_C1CCM",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C16,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C16,szTemp1);

	//get ch2 changing signal mult
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh2ChangSigMultip,sizeof(psuGChConfigTextRecord->cdfCh2ChangSigMultip));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh2ChangSigMultip)]=NULL;
	if (!CurrentSetParam("SET_CCR_C2CCM",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C26,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C26,szTemp1);

	//get ch3 changing signal mult
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh3ChangSigMultip,sizeof(psuGChConfigTextRecord->cdfCh3ChangSigMultip));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh3ChangSigMultip)]=NULL;
	if (!CurrentSetParam("SET_CCR_C3CCM",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C36,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C36,szTemp1);

	//get ch0 changing signal entry exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh0ChangSigEntryExit,sizeof(psuGChConfigTextRecord->chCh0ChangSigEntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh0ChangSigEntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C0CSEE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C07,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C07,szTemp1);

	//get ch1 changing signal entry exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh1ChangSigEntryExit,sizeof(psuGChConfigTextRecord->chCh1ChangSigEntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh1ChangSigEntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C1CSEE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C17,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C17,szTemp1);

	//get ch2 changing signal entry exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh2ChangSigEntryExit,sizeof(psuGChConfigTextRecord->chCh2ChangSigEntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh2ChangSigEntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C2CSEE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C27,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%uhd / %uhd",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C27,szTemp1);

	//get ch3 changing signal entry exit
	memcpy(szTemp1,psuGChConfigTextRecord->chCh3ChangSigEntryExit,sizeof(psuGChConfigTextRecord->chCh3ChangSigEntryExit));
	szTemp1[sizeof(psuGChConfigTextRecord->chCh3ChangSigEntryExit)]=NULL;
	if (!CurrentSetParam("SET_CCR_C3CSEE",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C37,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	sscanf(szTemp1,"%x",&usTemp);
	sprintf(szTemp1,"%hu / %hu",(usTemp>>4),(usTemp & 0x000f));
	user4page->SetDlgItemText(IDC_USER4PROG_C37,szTemp1);

	//get ch0 filter limit
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh0FilterLimit,sizeof(psuGChConfigTextRecord->cdfCh0FilterLimit));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh0FilterLimit)]=NULL;
	if (!CurrentSetParam("SET_CCR_FILTERLIM0",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C08,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C08,szTemp1);

	//get ch1 filter limit
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh1FilterLimit,sizeof(psuGChConfigTextRecord->cdfCh1FilterLimit));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh1FilterLimit)]=NULL;
	if (!CurrentSetParam("SET_CCR_FILTERLIM1",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C18,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C18,szTemp1);

	//get ch2 filter limit
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh2FilterLimit,sizeof(psuGChConfigTextRecord->cdfCh2FilterLimit));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh2FilterLimit)]=NULL;
	if (!CurrentSetParam("SET_CCR_FILTERLIM2",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C28,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C28,szTemp1);

	//get ch3 filter limit
	memcpy(szTemp1,psuGChConfigTextRecord->cdfCh3FilterLimit,sizeof(psuGChConfigTextRecord->cdfCh0FilterLimit));
	szTemp1[sizeof(psuGChConfigTextRecord->cdfCh3FilterLimit)]=NULL;
	if (!CurrentSetParam("SET_CCR_FILTERLIM3",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C38,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C38,szTemp1);

	//get ch0 hysteresis
	memcpy(szTemp1,psuGChConfigTextRecord->cdCh0Hysteresis,sizeof(psuGChConfigTextRecord->cdCh0Hysteresis));
	szTemp1[sizeof(psuGChConfigTextRecord->cdCh0Hysteresis)]=NULL;
	if (!CurrentSetParam("SET_CCR_HYSTER0",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C09,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C09,szTemp1);

	//get ch1 hysteresis
	memcpy(szTemp1,psuGChConfigTextRecord->cdCh0Hysteresis,sizeof(psuGChConfigTextRecord->cdCh1Hysteresis));
	szTemp1[sizeof(psuGChConfigTextRecord->cdCh1Hysteresis)]=NULL;
	if (!CurrentSetParam("SET_CCR_HYSTER1",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C19,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C19,szTemp1);

	//get ch2 hysteresis
	memcpy(szTemp1,psuGChConfigTextRecord->cdCh2Hysteresis,sizeof(psuGChConfigTextRecord->cdCh2Hysteresis));
	szTemp1[sizeof(psuGChConfigTextRecord->cdCh2Hysteresis)]=NULL;
	if (!CurrentSetParam("SET_CCR_HYSTER2",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C29,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C29,szTemp1);

	//get ch3 hysteresis
	memcpy(szTemp1,psuGChConfigTextRecord->cdCh3Hysteresis,sizeof(psuGChConfigTextRecord->cdCh0Hysteresis));
	szTemp1[sizeof(psuGChConfigTextRecord->cdCh3Hysteresis)]=NULL;
	if (!CurrentSetParam("SET_CCR_HYSTER3",szTemp1,false))
	{
		user4page->SetColor(IDC_USER4PROG_C39,COLOR_YELLOW);
		m_iUser4Prog_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}
	user4page->SetDlgItemText(IDC_USER4PROG_C39,szTemp1);

	m_pParent->m_iUser4Prog_Tab = m_iUser4Prog_Tab;
	if (m_iUser4Prog_Tab == 0x0003)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOWRED);
	else if (m_iUser4Prog_Tab == 0x0002)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOW);
	else if (m_iUser4Prog_Tab == 0x0001)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_RED);
	else if (m_iUser4Prog_Tab == 0x0000)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_GREEN);

	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);

}

void 
CSafePropertySheet::OnESBConfigRecord(char* szRecord)			
{
	//some temporary buffers
	char szTemp1[32];
	char szTemp2[32];
	char szTemp3[32];
	char szTemp4[32];
	char szTempModeFlags[8];
	char szTempNomAcq[8];
	char szTempAbNomAcq[8];
	char szTempLowPwrAcq[8];
	char szTempSN[16];
	char szTempX[64];

	//we need to send stuff to the correct property page
	CSetupData* setdatapage = m_pParent->m_pSetupData;
	CESBInfo	*pESBInfoPage = m_pParent->m_pESBInfo;

	//get a pointer to the text record
	ESBConfigTextRecord* psESBConfigTextRecord = (ESBConfigTextRecord*)szRecord;

	//get the julian seconds
	memcpy(szTemp1,psESBConfigTextRecord->cdJulianSeconds,sizeof(psESBConfigTextRecord->cdJulianSeconds));
	szTemp1[sizeof(psESBConfigTextRecord->cdJulianSeconds)]=NULL;
	//set the date and time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//get nominal acq interval
	memcpy(szTempNomAcq,psESBConfigTextRecord->cdNominalAcqInterval,sizeof(psESBConfigTextRecord->cdNominalAcqInterval));
	szTempNomAcq[sizeof(psESBConfigTextRecord->cdNominalAcqInterval)]=NULL;

	//get anomalous acq interval
	memcpy(szTempAbNomAcq,psESBConfigTextRecord->cdAnomalousAcqInterval,sizeof(psESBConfigTextRecord->cdAnomalousAcqInterval));
	szTempAbNomAcq[sizeof(psESBConfigTextRecord->cdAnomalousAcqInterval)]=NULL;

	//get low power acq
	memcpy(szTempLowPwrAcq,psESBConfigTextRecord->cdLowPowerAcqInterval,sizeof(psESBConfigTextRecord->cdLowPowerAcqInterval));
	szTempLowPwrAcq[sizeof(psESBConfigTextRecord->cdLowPowerAcqInterval)];
	
	//get mode flags 
	memcpy(szTempModeFlags,psESBConfigTextRecord->chModeFlags,sizeof(psESBConfigTextRecord->chModeFlags));
	szTempModeFlags[sizeof(psESBConfigTextRecord->chModeFlags)]=NULL;

	//get the serial number
	memcpy(szTempSN,psESBConfigTextRecord->chSerialNumber,sizeof(psESBConfigTextRecord->chSerialNumber));
	szTempSN[sizeof(psESBConfigTextRecord->chSerialNumber)]=NULL;

	//get what is in which columns now
	pESBInfoPage->GetDlgItemText(IDC_ESBINFO_SERIAL1,szTemp1,sizeof(szTemp1));
	pESBInfoPage->GetDlgItemText(IDC_ESBINFO_SERIAL2,szTemp2,sizeof(szTemp2));
	pESBInfoPage->GetDlgItemText(IDC_ESBINFO_SERIAL3,szTemp3,sizeof(szTemp3));
	pESBInfoPage->GetDlgItemText(IDC_ESBINFO_SERIAL4,szTemp4,sizeof(szTemp4));

	int iColumn = 0;
	//first see current matches any of the columns
	if      (strncmp((char*)&psESBConfigTextRecord->chSerialNumber,szTemp1,sizeof(psESBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 1;
	else if (strncmp((char*)&psESBConfigTextRecord->chSerialNumber,szTemp2,sizeof(psESBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 2;
	else if (strncmp((char*)&psESBConfigTextRecord->chSerialNumber,szTemp3,sizeof(psESBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 3;
	else if (strncmp((char*)&psESBConfigTextRecord->chSerialNumber,szTemp4,sizeof(psESBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 4;
	//if not then find the first unused column
	else if (strlen(szTemp1)==0) iColumn = 5;
	else if (strlen(szTemp2)==0) iColumn = 6;
	else if (strlen(szTemp3)==0) iColumn = 7;
	else if (strlen(szTemp4)==0) iColumn = 8;

	bool bMatch;

	switch (iColumn) {
	case 5:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBSR_SERIALNUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_SERIAL1,szTempSN);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_SERIAL1,COLOR_YELLOW);
		CurrentSetParam("SET_ESBSR_SERIALNUM1",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 1:
		//set date and time
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATADATEC1,cTime.Date());
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATATIMEC1,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_ESBCR_TIME1",szTemp1,false,true);

		//set nom acq interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_NORMACQINTERV1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempNomAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_NOMACQINT1, szTempNomAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_NOMACQINT1,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_NORMACQINTERV1",szTempNomAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		
		//set anomalous acq interval  IDC_ESBINFO_ANOMACQINT2
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_ANOMACQINTERV1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempAbNomAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_ANOMACQINT1, szTempAbNomAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_ANOMACQINT1,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_ANOMACQINTERV1",szTempAbNomAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		
		//set low power acq interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_LOPWACQINTERV1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempLowPwrAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_LPACQINT1, szTempLowPwrAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_LPACQINT1,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_LOPWACQINTERV1",szTempLowPwrAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_MODEFLAGS1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_MODEFLAGS1, szTempModeFlags);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_MODEFLAGS1,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_MODEFLAGS1",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 6:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBSR_SERIALNUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_SERIAL2,szTempSN);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_SERIAL2,COLOR_YELLOW);
		CurrentSetParam("SET_ESBSR_SERIALNUM2",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 2:
		//set date and time
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATADATEC2,cTime.Date());
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATATIMEC2,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_ESBCR_TIME2",szTemp1,false,true);

		//set nom acq interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_NORMACQINTERV2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempNomAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_NOMACQINT2, szTempNomAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_NOMACQINT2,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_NORMACQINTERV2",szTempNomAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		
		//set anomalous acq interval  IDC_ESBINFO_ANOMACQINT2
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_ANOMACQINTERV2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempAbNomAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_ANOMACQINT2, szTempAbNomAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_ANOMACQINT2,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_ANOMACQINTERV2",szTempAbNomAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		
		//set low power acq interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_LOPWACQINTERV2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempLowPwrAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_LPACQINT2, szTempLowPwrAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_LPACQINT2,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_LOPWACQINTERV2",szTempLowPwrAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_MODEFLAGS2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_MODEFLAGS2, szTempModeFlags);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_MODEFLAGS2,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_MODEFLAGS2",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 7:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBSR_SERIALNUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_SERIAL3,szTempSN);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_SERIAL3,COLOR_YELLOW);
		CurrentSetParam("SET_ESBSR_SERIALNUM3",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 3:
		//set date and time
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATADATEC3,cTime.Date());
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATATIMEC3,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_ESBCR_TIME3",szTemp1,false,true);

		//set nom acq interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_NORMACQINTERV3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempNomAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_NOMACQINT3, szTempNomAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_NOMACQINT3,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_NORMACQINTERV3",szTempNomAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		
		//set anomalous acq interval  IDC_ESBINFO_ANOMACQINT2
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_ANOMACQINTERV3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempAbNomAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_ANOMACQINT3, szTempAbNomAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_ANOMACQINT3,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_ANOMACQINTERV3",szTempAbNomAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		
		//set low power acq interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_LOPWACQINTERV3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempLowPwrAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_LPACQINT3, szTempLowPwrAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_LPACQINT3,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_LOPWACQINTERV3",szTempLowPwrAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_MODEFLAGS3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_MODEFLAGS3, szTempModeFlags);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_MODEFLAGS3,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_MODEFLAGS3",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 8:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBSR_SERIALNUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_SERIAL4,szTempSN);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_SERIAL4,COLOR_YELLOW);
		CurrentSetParam("SET_ESBSR_SERIALNUM4",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 4:
		//set date and time
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATADATEC4,cTime.Date());
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATATIMEC4,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_ESBCR_TIME4",szTemp1,false,true);

		//set nom acq interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_NORMACQINTERV4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempNomAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_NOMACQINT4, szTempNomAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_NOMACQINT4,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_NORMACQINTERV4",szTempNomAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		
		//set anomalous acq interval  IDC_ESBINFO_ANOMACQINT2
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_ANOMACQINTERV4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempAbNomAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_ANOMACQINT4, szTempAbNomAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_ANOMACQINT4,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_ANOMACQINTERV4",szTempAbNomAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		
		//set low power acq interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_LOPWACQINTERV4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempLowPwrAcq,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_LPACQINT4, szTempLowPwrAcq);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_LPACQINT4,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_LOPWACQINTERV4",szTempLowPwrAcq,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBCR_MODEFLAGS4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_MODEFLAGS4, szTempModeFlags);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_MODEFLAGS4,COLOR_YELLOW);
		CurrentSetParam("SET_ESBCR_MODEFLAGS4",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	}

	m_pParent->m_iESBInfo_Tab = m_iESBInfo_Tab;
	if (m_iESBInfo_Tab == 0x0003)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iESBInfo_Tab == 0x0002)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_YELLOW);
	else if (m_iESBInfo_Tab == 0x0001)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_RED);
	else if (m_iESBInfo_Tab == 0x0000)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_GREEN);

	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);

}

void 
CSafePropertySheet::OnPSBConfigRecord(char* szRecord)			
{
	char szTemp1[32];
	char szTemp2[32];
	char szTemp3[32];
	char szTemp4[32];
	char szTempModeFlags[8];
	char szTempHVSetPoint[8];
	char szTempHVConfigState[2];
	char szTempSN[16];
	char szTempX[64];

	//we need to send stuff to the property page
	CSetupData* setdatapage = m_pParent->m_pSetupData;
	CPSBInfo	*pPSBInfoPage = m_pParent->m_pPSBInfo;

	//get a pointer to the text record
	PSBConfigTextRecord* psPSBConfigTextRecord = (PSBConfigTextRecord*)szRecord;

	//get the julian seconds
	memcpy(szTemp1,psPSBConfigTextRecord->cdJulianSeconds,sizeof(psPSBConfigTextRecord->cdJulianSeconds));
	szTemp1[sizeof(psPSBConfigTextRecord->cdJulianSeconds)]=NULL;
	//set the date and time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//get mode flags 
	memcpy(szTempModeFlags,psPSBConfigTextRecord->chModeFlags,sizeof(psPSBConfigTextRecord->chModeFlags));
	szTempModeFlags[sizeof(psPSBConfigTextRecord->chModeFlags)]=NULL;

	//get the HV set point
	memcpy(szTempHVSetPoint,psPSBConfigTextRecord->cdfHVSetPoint,sizeof(psPSBConfigTextRecord->cdfHVSetPoint));
	szTempHVSetPoint[sizeof(psPSBConfigTextRecord->cdfHVSetPoint)]=NULL;

	//get the HV Configured State
	memcpy(szTempHVConfigState,&psPSBConfigTextRecord->chHVConfigState,sizeof(psPSBConfigTextRecord->chHVConfigState));
	szTempHVConfigState[sizeof(psPSBConfigTextRecord->chHVConfigState)]=NULL;

	//get the serial number
	memcpy(szTempSN,psPSBConfigTextRecord->chSerialNumber,sizeof(psPSBConfigTextRecord->chSerialNumber));
	szTempSN[sizeof(psPSBConfigTextRecord->chSerialNumber)]=NULL;

	//get what is in which columns now
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL1,szTemp1,sizeof(szTemp1));
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL2,szTemp2,sizeof(szTemp2));
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL3,szTemp3,sizeof(szTemp3));
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL4,szTemp4,sizeof(szTemp4));

	int iColumn = 0;
	//first see current matches any of the columns
	if      (strncmp((char*)&psPSBConfigTextRecord->chSerialNumber,szTemp1,sizeof(psPSBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 1;
	else if (strncmp((char*)&psPSBConfigTextRecord->chSerialNumber,szTemp2,sizeof(psPSBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 2;
	else if (strncmp((char*)&psPSBConfigTextRecord->chSerialNumber,szTemp3,sizeof(psPSBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 3;
	else if (strncmp((char*)&psPSBConfigTextRecord->chSerialNumber,szTemp4,sizeof(psPSBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 4;
	//if not then find the first unused column
	else if (strlen(szTemp1)==0) iColumn = 5;
	else if (strlen(szTemp2)==0) iColumn = 6;
	else if (strlen(szTemp3)==0) iColumn = 7;
	else if (strlen(szTemp4)==0) iColumn = 8;

	bool bMatch;

	switch (iColumn) {
	case 5:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBSR_SERIALNUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL1,szTempSN);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL1,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM1",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 1:
		//set date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATEC1,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMEC1,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_PSBCR_TIME1",szTemp1,false,true);

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_MODEFLAGS1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_MODEFLAGS1, szTempModeFlags);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_MODEFLAGS1,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_MODEFLAGS1",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set hv set point
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_HVSETPOINT1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempHVSetPoint,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVSETPT1,szTempHVSetPoint);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_HVSETPT1,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_HVSETPOINT1",szTempHVSetPoint,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set hv configured state
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_CONFIGSTATE1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempHVConfigState,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVCONFSTATE1,szTempHVConfigState);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_HVCONFSTATE1,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_CONFIGSTATE1",szTempHVConfigState,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 6:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBSR_SERIALNUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL2,szTempSN);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL2,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM2",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 2:
		//set date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATEC2,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMEC2,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_PSBCR_TIME2",szTemp1,false,true);

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_MODEFLAGS2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_MODEFLAGS2, szTempModeFlags);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_MODEFLAGS2,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_MODEFLAGS2",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set hv set point
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_HVSETPOINT2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempHVSetPoint,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVSETPT2,szTempHVSetPoint);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_HVSETPT2,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_HVSETPOINT2",szTempHVSetPoint,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set hv configured state
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_CONFIGSTATE2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempHVConfigState,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVCONFSTATE2,szTempHVConfigState);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_HVCONFSTATE2,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_CONFIGSTATE2",szTempHVConfigState,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 7:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBSR_SERIALNUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL3,szTempSN);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL3,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM3",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 3:
		//set date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATEC3,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMEC3,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_PSBCR_TIME1",szTemp1,false,true);

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_MODEFLAGS3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_MODEFLAGS3, szTempModeFlags);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_MODEFLAGS3,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_MODEFLAGS3",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set hv set point
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_HVSETPOINT3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempHVSetPoint,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVSETPT3,szTempHVSetPoint);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_HVSETPT3,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_HVSETPOINT3",szTempHVSetPoint,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set hv configured state
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_CONFIGSTATE3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempHVConfigState,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVCONFSTATE3,szTempHVConfigState);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_HVCONFSTATE3,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_CONFIGSTATE3",szTempHVConfigState,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 8:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBSR_SERIALNUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL4,szTempSN);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL4,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM4",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 4:
		//set date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATEC4,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMEC4,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_PSBCR_TIME4",szTemp1,false,true);

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_MODEFLAGS4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_MODEFLAGS4, szTempModeFlags);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_MODEFLAGS4,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_MODEFLAGS4",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set hv set point
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_HVSETPOINT4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempHVSetPoint,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVSETPT4,szTempHVSetPoint);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_HVSETPT4,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_HVSETPOINT4",szTempHVSetPoint,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set hv configured state
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBCR_CONFIGSTATE4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempHVConfigState,szTempX)==0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVCONFSTATE4,szTempHVConfigState);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_HVCONFSTATE4,COLOR_YELLOW);
		CurrentSetParam("SET_PSBCR_CONFIGSTATE4",szTempHVConfigState,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	}

	m_pParent->m_iPSBInfo_Tab = m_iPSBInfo_Tab;
	if (m_iPSBInfo_Tab == 0x0003)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iPSBInfo_Tab == 0x0002)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_YELLOW);
	else if (m_iPSBInfo_Tab == 0x0001)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_RED);
	else if (m_iPSBInfo_Tab == 0x0000)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_GREEN);

	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);

}

void 
CSafePropertySheet::OnBMBConfigRecord(char* szRecord)			
{

	//some temp buffers
	char szTemp1[32];
	char szTemp2[32];
	char szTemp3[32];
	char szTemp4[32];
	char szTempSN[16];
	char szTempPwrOn[8];
	char szTempPwrOff[8];
	char szTempModeFlags[8];
	char szTempX[64];

	//we need to send stuff to the property page
	CSetupData* setdatapage = m_pParent->m_pSetupData;
	CBMBInfo	*pBMBInfoPage = m_pParent->m_pBMBInfo;

	//get a pointer to the text record
	BMBConfigTextRecord* psBMBConfigTextRecord = (BMBConfigTextRecord*)szRecord;

	//get the julian seconds
	memcpy(szTemp1,psBMBConfigTextRecord->cdJulianSeconds,sizeof(psBMBConfigTextRecord->cdJulianSeconds));
	szTemp1[sizeof(psBMBConfigTextRecord->cdJulianSeconds)]=NULL;
	//set the date and time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//get mode flags 
	memcpy(szTempModeFlags,psBMBConfigTextRecord->chModeFlags,sizeof(psBMBConfigTextRecord->chModeFlags));
	szTempModeFlags[sizeof(psBMBConfigTextRecord->chModeFlags)]=NULL;

	//get the power on status-reporting interval
	memcpy(szTempPwrOn,psBMBConfigTextRecord->cdPowerOnReportInter,sizeof(psBMBConfigTextRecord->cdPowerOnReportInter));
	szTempPwrOn[sizeof(psBMBConfigTextRecord->cdPowerOnReportInter)]=NULL;

	//get the power off status-reporting interval
	memcpy(szTempPwrOff,&psBMBConfigTextRecord->cdPowerOffReportInterv,sizeof(psBMBConfigTextRecord->cdPowerOffReportInterv));
	szTempPwrOff[sizeof(psBMBConfigTextRecord->cdPowerOffReportInterv)]=NULL;

	//get the serial number
	memcpy(szTempSN,psBMBConfigTextRecord->chSerialNumber,sizeof(psBMBConfigTextRecord->chSerialNumber));
	szTempSN[sizeof(psBMBConfigTextRecord->chSerialNumber)]=NULL;

	//get what is in which columns now
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL1,szTemp1,sizeof(szTemp1));
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL2,szTemp2,sizeof(szTemp2));
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL3,szTemp3,sizeof(szTemp3));
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL4,szTemp4,sizeof(szTemp4));

	int iColumn = 0;
	//first see current matches any of the columns
	if      (strncmp((char*)&psBMBConfigTextRecord->chSerialNumber,szTemp1,sizeof(psBMBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 1;
	else if (strncmp((char*)&psBMBConfigTextRecord->chSerialNumber,szTemp2,sizeof(psBMBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 2;
	else if (strncmp((char*)&psBMBConfigTextRecord->chSerialNumber,szTemp3,sizeof(psBMBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 3;
	else if (strncmp((char*)&psBMBConfigTextRecord->chSerialNumber,szTemp4,sizeof(psBMBConfigTextRecord->chSerialNumber)) == 0)	iColumn = 4;
	//if not then find the first unused column
	else if (strlen(szTemp1)==0) iColumn = 5;
	else if (strlen(szTemp2)==0) iColumn = 6;
	else if (strlen(szTemp3)==0) iColumn = 7;
	else if (strlen(szTemp4)==0) iColumn = 8;

	bool bMatch;

	switch (iColumn) {
	case 5:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBSR_SERIALNUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL1,szTempSN);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
		CurrentSetParam("SET_BMBSR_SERIALNUM1",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 1:
		//set date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATEC1,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMEC1,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_BMBCR_TIME1",szTemp1,false,true);

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_MODEFLAGS1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_MODEFLAGS1, szTempModeFlags);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_MODEFLAGS1,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_MODEFLAGS1",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set power on status interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_PONSTATINT1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempPwrOn,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_PONRPT1,szTempPwrOn);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_PONRPT1,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_PONSTATINT1",szTempPwrOn,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set power off status interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_POFFSTATINT1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempPwrOff,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_POFFRPT1,szTempPwrOff);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_POFFRPT1,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_POFFSTATINT1",szTempPwrOff,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_SERIALNUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL1,szTempSN);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 6:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBSR_SERIALNUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL2,szTempSN);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
		CurrentSetParam("SET_BMBSR_SERIALNUM2",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 2:
		//get date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATEC2,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMEC2,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_BMBCR_TIME2",szTemp1,false,true);

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_MODEFLAGS2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_MODEFLAGS2, szTempModeFlags);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_MODEFLAGS2,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_MODEFLAGS2",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set power on status interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_PONSTATINT2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempPwrOn,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_PONRPT2,szTempPwrOn);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_PONRPT2,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_PONSTATINT2",szTempPwrOn,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set power off status interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_POFFSTATINT2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempPwrOff,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_POFFRPT2,szTempPwrOff);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_POFFRPT2,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_POFFSTATINT2",szTempPwrOff,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_SERIALNUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL2,szTempSN);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 7:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBSR_SERIALNUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL3,szTempSN);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
		CurrentSetParam("SET_BMBSR_SERIALNUM3",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 3:
		//get date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATEC3,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMEC3,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_BMBCR_TIME3",szTemp1,false,true);

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_MODEFLAGS3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_MODEFLAGS3, szTempModeFlags);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_MODEFLAGS3,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_MODEFLAGS3",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set power on status interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_PONSTATINT3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempPwrOn,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_PONRPT3,szTempPwrOn);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_PONRPT3,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_PONSTATINT3",szTempPwrOn,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set power off status interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_POFFSTATINT3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempPwrOff,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_POFFRPT3,szTempPwrOff);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_POFFRPT3,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_POFFSTATINT3",szTempPwrOff,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_SERIALNUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL3,szTempSN);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 8:
		//set set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBSR_SERIALNUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL4,szTempSN);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
		CurrentSetParam("SET_BMBSR_SERIALNUM4",szTempSN,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process rest
	case 4:
		//get date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATEC4,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMEC4,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_BMBCR_TIME4",szTemp1,false,true);

		//set mode flags
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_MODEFLAGS4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempModeFlags,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_MODEFLAGS4, szTempModeFlags);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_MODEFLAGS4,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_MODEFLAGS4",szTempModeFlags,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set power on status interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_PONSTATINT4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempPwrOn,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_PONRPT4,szTempPwrOn);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_PONRPT4,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_PONSTATINT4",szTempPwrOn,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set power off status interval
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_POFFSTATINT4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempPwrOff,szTempX)==0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_POFFRPT4,szTempPwrOff);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_POFFRPT4,COLOR_YELLOW);
		CurrentSetParam("SET_BMBCR_POFFSTATINT4",szTempPwrOff,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set serial number
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBCR_SERIALNUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTempSN,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL4,szTempSN);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	}

	m_pParent->m_iBMBInfo_Tab = m_iBMBInfo_Tab;
	if (m_iBMBInfo_Tab == 0x0003)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iBMBInfo_Tab == 0x0002)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOW);
	else if (m_iBMBInfo_Tab == 0x0001)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_RED);
	else if (m_iBMBInfo_Tab == 0x0000)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_GREEN);

	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);

}

void 
CSafePropertySheet::OnuGStatusRecord(char* szRecord)			
{
	//some temporary buffers
	char szTemp1[32];
	float fTemp;
	CString cTemp;

	//we need to send stuff to the property page
	CUGINFO*	pUGInfoPage = m_pParent->m_pUGInfo;
	CSummary*	pSummaryPage = m_pParent->m_pSummaryPage;
	CSetupData* setdatapage = m_pParent->m_pSetupData;

	//get a pointer to a GeneralConfigTextRecord
	uGStatusTextRecord* pszRecord = (uGStatusTextRecord*)szRecord;

 	//get julian time and put it in the binary record
	strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;

	//set the date & time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//do we have a match
//	bool bMatch;

	//get the 10th of a second
	//strncpy(szTemp1,(char*)pszRecord->cdJulianSec10ths,sizeof(pszRecord->cdJulianSec10ths));
	//szTemp1[sizeof(pszRecord->cdJulianSec10ths)]=NULL;
	szTemp1[0] = pszRecord->cdJulianSec10ths;
	szTemp1[1]=NULL;

	//set date and time
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_DATADATES,cTime.Date());
	cTemp = cTime.Time();
	cTemp += ".";
	cTemp += szTemp1;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_DATATIMES,cTemp);
	cTemp = cTime.Date();
	cTemp += " ";
	cTemp += cTime.Time();
	cTemp += ".";
	cTemp += szTemp1;
	CurrentSetParam("SET_UGSR_TIME",cTemp,false);
	
	//set the 5.0 volt supply
	strncpy(szTemp1,(char*)pszRecord->cdf50VSupply,sizeof(pszRecord->cdf50VSupply));
	szTemp1[sizeof(pszRecord->cdf50VSupply)]=NULL;
	sscanf(szTemp1,"%f",&fTemp);

	bool bCurrentRed = false;
	bool bCurrentLow = false;
	bool bCurrentHigh= false;

	m_fCurP5 = fTemp;

	if (m_bFirstP5)
	{
		m_bFirstP5 = false;
		m_fLowP5 = m_fHighP5 = m_fCurP5;
	}
	if (m_fCurP5 < m_fLowP5)
	{
		bCurrentLow = true;
		m_fLowP5 = m_fCurP5;
	}
	else
		bCurrentLow = false;
	if (m_fCurP5 > m_fHighP5)
	{
		bCurrentHigh = true;
		m_fHighP5= m_fCurP5;
	}
	else
		bCurrentHigh = false;

	if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
	{
//		bMatch = false;
		bCurrentRed = true;			
		pUGInfoPage->SetColor(IDC_UGINFO_50VSUPPLYS,COLOR_RED);
		pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT2,COLOR_RED);
		m_bP5VoltOk = false;
		m_iuGInfo_Tab |= 0x0001;
		m_iInst4Setup_Tab |= 0x0001;
	}
	else
	{
//		bMatch = true;
		bCurrentRed = false;
		m_bP5VoltOk = true;
	}

	sprintf(szTemp1,"%4.1f",m_fCurP5);
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_50VSUPPLYS,szTemp1);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,szTemp1);
	//already set the color so match isn't needed
	//CurrentSetParam("SET_UGSR_50V",szTemp1,false,false);
	setdatapage->CurrentSetParamColor("SET_UGSR_50V",szTemp1,bCurrentRed);

	if (bCurrentRed && bCurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
		pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT1,COLOR_RED);

	if (bCurrentRed && bCurrentHigh && (m_fHighP5 >= m_fSup_P05H))// || (m_fHighP5 >= m_fSup_P05H))
		pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT3,COLOR_RED);

	sprintf(szTemp1,"%4.1f",m_fLowP5);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,szTemp1);

	sprintf(szTemp1,"%4.1f",m_fHighP5);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,szTemp1);

	//set the 3.3 volt supply (uses the -15 from microGRAND)
	strncpy(szTemp1,(char*)pszRecord->cdf33VSupply,sizeof(pszRecord->cdf33VSupply));
	szTemp1[sizeof(pszRecord->cdf33VSupply)]=NULL;
	sscanf(szTemp1,"%f",&fTemp);

	bCurrentRed = false;
	bCurrentLow = false;
	bCurrentHigh= false;

	m_fCurP33 = fTemp;

	if (m_bFirstP33)
	{
		m_bFirstP33 = false;
		m_fLowP33 = m_fHighP33 = m_fCurP33;
	}
	if (m_fCurP33 < m_fLowP33)
	{
		bCurrentLow = true;
		m_fLowP33 = m_fCurP33;
	}
	else
		bCurrentLow = false;
	if (m_fCurP33 > m_fHighP33)
	{
		bCurrentHigh = true;
		m_fHighP33 = m_fCurP33;
	}
	else
		bCurrentHigh = false;

	if ((m_fCurP33 <= m_fSup_P33L) || (m_fCurP33 >= m_fSup_P33H))
	{
//		bMatch = false;
		bCurrentRed = true;			
		pUGInfoPage->SetColor(IDC_UGINFO_33VSUPPLYS,COLOR_RED);
		pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT2,COLOR_RED);
		m_bP33VoltOk = false;
		m_iuGInfo_Tab |= 0x0001;
		m_iInst4Setup_Tab |= 0x0001;
	}
	else
	{
//		bMatch = true;
		bCurrentRed = false;
		m_bP33VoltOk = true;
	}

	sprintf(szTemp1,"%4.1f",m_fCurP33);
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_33VSUPPLYS,szTemp1);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT2,szTemp1);
//	CurrentSetParam("SET_UGSR_33V",szTemp1,false,false);
	setdatapage->CurrentSetParamColor("SET_UGSR_33V",szTemp1,bCurrentRed);

	if (bCurrentRed && bCurrentLow && (m_fLowP33 <= m_fSup_P33L))// || (m_fLowP5 >= m_fSup_P05H))
		pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT1,COLOR_RED);

	if (bCurrentRed && bCurrentHigh && (m_fHighP33 >= m_fSup_P33H))// || (m_fHighP5 >= m_fSup_P05H))
		pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT3,COLOR_RED);

	pUGInfoPage->SetDlgItemText(IDC_UGINFO_33VSUPPLYS,szTemp1);

	sprintf(szTemp1,"%4.1f",m_fLowP33);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT1,szTemp1);

	sprintf(szTemp1,"%4.1f",m_fHighP33);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT3,szTemp1);

	//set the 2.5 volt supply (uses the +15 from microGRAND)
	strncpy(szTemp1,(char*)pszRecord->cdf25VSupply,sizeof(pszRecord->cdf25VSupply));
	szTemp1[sizeof(pszRecord->cdf25VSupply)]=NULL;
	sscanf(szTemp1,"%f",&fTemp);

	bCurrentRed = false;
	bCurrentLow = false;
	bCurrentHigh= false;

	m_fCurP25 = fTemp;

	if (m_bFirstP25)
	{
		m_bFirstP25 = false;
		m_fLowP25 = m_fHighP25 = m_fCurP25;
	}
	if (m_fCurP25 < m_fLowP25)
	{
		bCurrentLow = true;
		m_fLowP25 = m_fCurP25;
	}
	else
		bCurrentLow = false;
	if (m_fCurP25 > m_fHighP25)
	{
		bCurrentHigh = true;
		m_fHighP25 = m_fCurP25;
	}
	else
		bCurrentHigh = false;

	if ((m_fCurP25 <= m_fSup_P25L) || (m_fCurP25 >= m_fSup_P25H))
	{
//		bMatch = false;
		bCurrentRed = true;			
		pUGInfoPage->SetColor(IDC_UGINFO_25VSUPPLYS,COLOR_RED);
		pSummaryPage->SetColor(IDC_SUMMARY_P25VOLT2,COLOR_RED);
		m_bP25VoltOk = false;
		m_iuGInfo_Tab |= 0x0001;
		m_iInst4Setup_Tab |= 0x0001;
	}
	else
	{
//		bMatch = true;
		bCurrentRed = false;
		m_bP25VoltOk = true;
	}

	sprintf(szTemp1,"%4.1f",m_fCurP25);
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_25VSUPPLYS,szTemp1);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_P25VOLT2,szTemp1);
//	CurrentSetParam("SET_UGSR_25V",szTemp1,false,false);
	setdatapage->CurrentSetParamColor("SET_UGSR_25V",szTemp1,bCurrentRed);

	if (bCurrentRed && bCurrentLow && (m_fLowP25 <= m_fSup_P25L))// || (m_fLowP5 >= m_fSup_P05H))
		pSummaryPage->SetColor(IDC_SUMMARY_P25VOLT1,COLOR_RED);

	if (bCurrentRed && bCurrentHigh && (m_fHighP25 >= m_fSup_P25H))// || (m_fHighP5 >= m_fSup_P05H))
		pSummaryPage->SetColor(IDC_SUMMARY_P25VOLT3,COLOR_RED);

	sprintf(szTemp1,"%4.1f",m_fLowP25);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_P25VOLT1,szTemp1);

	sprintf(szTemp1,"%4.1f",m_fHighP25);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_P25VOLT3,szTemp1);

	//set the PHD volt supply (uses the BAT from microGRAND)
	strncpy(szTemp1,(char*)pszRecord->cdfPHDVoltage,sizeof(pszRecord->cdfPHDVoltage));
	szTemp1[sizeof(pszRecord->cdfPHDVoltage)]=NULL;
	sscanf(szTemp1,"%f",&fTemp);

	bCurrentRed = false;
	bCurrentLow = false;
	bCurrentHigh= false;

	m_fCurBV = fTemp;

	if (m_bFirstBV)
	{
		m_bFirstBV = false;
		m_fLowBV = m_fHighBV = m_fCurBV;
	}
	if (m_fCurBV < m_fLowBV)
	{
		bCurrentLow = true;
		m_fLowBV = m_fCurBV;
	}
	else
		bCurrentLow = false;
	if (m_fCurBV > m_fHighBV)
	{
		bCurrentHigh = true;
		m_fHighBV = m_fCurBV;
	}
	else
		bCurrentHigh = false;

	if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
	{
//		bMatch = false;
		bCurrentRed = true;			
		pUGInfoPage->SetColor(IDC_UGINFO_PHDVS,COLOR_RED);
		pSummaryPage->SetColor(IDC_SUMMARY_BATTERY2,COLOR_RED);

		m_bBatteryOk = false;
		m_iuGInfo_Tab |= 0x0001;
		m_iInst4Setup_Tab |= 0x0001;
	}
	else
	{
//		bMatch = true;
		bCurrentRed = false;
		m_bBatteryOk = true;
	}

	sprintf(szTemp1,"%4.1f",m_fCurBV);
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_PHDVS,szTemp1);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_BATTERY2,szTemp1);
//	CurrentSetParam("SET_UGSR_PHDVOLT",szTemp1,false,false);
	setdatapage->CurrentSetParamColor("SET_UGSR_PHDVOLT",szTemp1,bCurrentRed);

	if (bCurrentRed && bCurrentLow && (m_fLowBV <= m_fBatVoltL))// || (m_fLowP5 >= m_fSup_P05H))
		pSummaryPage->SetColor(IDC_SUMMARY_BATTERY1,COLOR_RED);

	if (bCurrentRed && bCurrentHigh && (m_fHighBV >= m_fBatVoltH))// || (m_fHighP5 >= m_fSup_P05H))
		pSummaryPage->SetColor(IDC_SUMMARY_BATTERY3,COLOR_RED);

	sprintf(szTemp1,"%4.1f",m_fLowBV);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_BATTERY1,szTemp1);

	sprintf(szTemp1,"%4.1f",m_fHighBV);
	pSummaryPage->SetDlgItemText(IDC_SUMMARY_BATTERY3,szTemp1);

	//set status byte 1
	strncpy(szTemp1,(char*)pszRecord->chStatus1Byte,sizeof(pszRecord->chStatus1Byte));
	szTemp1[sizeof(pszRecord->chStatus1Byte)]=NULL;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_STATUS1S,szTemp1);
	CurrentSetParam("SET_UGSR_STATUSBYTE1",szTemp1,false);

	//set status byte 2
	strncpy(szTemp1,(char*)pszRecord->chStatus2Byte,sizeof(pszRecord->chStatus2Byte));
	szTemp1[sizeof(pszRecord->chStatus2Byte)]=NULL;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_STATUS2S,szTemp1);
	CurrentSetParam("SET_UGSR_STATUSBYTE2",szTemp1,false);

	//set status byte 3
	strncpy(szTemp1,(char*)pszRecord->chStatus3Byte,sizeof(pszRecord->chStatus3Byte));
	szTemp1[sizeof(pszRecord->chStatus3Byte)]=NULL;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_STATUS3S,szTemp1);
	CurrentSetParam("SET_UGSR_STATUSBYTE3",szTemp1,false);

	//set status byte 4
	strncpy(szTemp1,(char*)pszRecord->chStatus4Byte,sizeof(pszRecord->chStatus4Byte));
	szTemp1[sizeof(pszRecord->chStatus1Byte)]=NULL;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_STATUS4S,szTemp1);
	CurrentSetParam("SET_UGSR_STATUSBYTE4",szTemp1,false);

	//set cpu up time
	strncpy(szTemp1,(char*)pszRecord->chCPUUpTime,sizeof(pszRecord->chCPUUpTime));
	szTemp1[sizeof(pszRecord->chCPUUpTime)]=NULL;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CPUUPS,szTemp1);
	CurrentSetParam("SET_UGSR_CPUUPTIME",szTemp1,false);

	//set cpu temperature
	strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
	szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_CPUTEMPS,szTemp1);
	CurrentSetParam("SET_UGSR_CPUTEMP",szTemp1,false);

	//set board temperature
	strncpy(szTemp1,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
	szTemp1[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_BOARDTEMPS,szTemp1);
	CurrentSetParam("SET_UGSR_BOARDTEMP",szTemp1,false);

	//set reserved
	strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
	szTemp1[sizeof(pszRecord->chReserved)]=NULL;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_RESERVEDS,szTemp1);
	CurrentSetParam("SET_UGSR_RESERVED",szTemp1,false);

	m_pParent->m_iUGInfo_Tab = m_iuGInfo_Tab;
	if (m_iuGInfo_Tab == 0x0003)
		SetTabColor(IDC_UGINFO_TAB,COLOR_YELLOWRED);
	else if (m_iuGInfo_Tab == 0x0002)
		SetTabColor(IDC_UGINFO_TAB,COLOR_YELLOW);
	else if (m_iuGInfo_Tab == 0x0001)
		SetTabColor(IDC_UGINFO_TAB,COLOR_RED);
	else if (m_iuGInfo_Tab == 0x0000)
		SetTabColor(IDC_UGINFO_TAB,COLOR_GREEN);

	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);
}

void 
CSafePropertySheet::OnPSBStatusRecord(char* szRecord)			
{
	//some temporary buffers
	char szTemp1[32];
	char szTemp2[32];
	char szTemp3[32];
	char szTemp4[32];
	char szTempX[32];
	bool bCurrentRed = false;
	bool bCurrentLow = false;
	bool bCurrentHigh= false;
	int iTemp;
	float fTemp;

	CString csTime;
	char szJulian10ths;

	CString csDateTime;

	bool bMatch;

	//we need to send stuff to the property page
	CSetupData* setdatapage = m_pParent->m_pSetupData;
	CSummary*	pSummaryPage = m_pParent->m_pSummaryPage;
	CPSBInfo	*pPSBInfoPage = m_pParent->m_pPSBInfo;

	//initialize to zero so as to not use if
	//more then four boards are being used
	int iColumn = 0;

	//get a pointer to a GeneralConfigTextRecord
	PSBStatusTextRecord* pszRecord = (PSBStatusTextRecord*)szRecord;

 	//get julian time 
	strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;

	//set the date & time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//get the 10th of a second
	szJulian10ths = pszRecord->cdJulianSec10ths;

	//set date and time
	csTime = cTime.Time();
	csTime += ".";
	csTime += pszRecord->cdJulianSec10ths;
	csDateTime = cTime.Date();
	csDateTime += " ";
	csDateTime += csTime;

	//get serial number for column placement
	strncpy(szTempX,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
	szTempX[sizeof(pszRecord->chSerialNumber)]=NULL;

	//get what is in which columns now
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL1,szTemp1,sizeof(szTemp1));
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL2,szTemp2,sizeof(szTemp2));
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL3,szTemp3,sizeof(szTemp3));
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL4,szTemp4,sizeof(szTemp4));

	//first see current matches any of the columns
	if      (strncmp((char*)&pszRecord->chSerialNumber,szTemp1,sizeof(pszRecord->chSerialNumber)) == 0)	iColumn = 1;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp2,sizeof(pszRecord->chSerialNumber)) == 0)	iColumn = 2;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp3,sizeof(pszRecord->chSerialNumber)) == 0)	iColumn = 3;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp4,sizeof(pszRecord->chSerialNumber)) == 0)	iColumn = 4;
	//if not then find the first unused column
	else if (strlen(szTemp1)==0) iColumn = 5;
	else if (strlen(szTemp2)==0) iColumn = 6;
	else if (strlen(szTemp3)==0) iColumn = 7;
	else if (strlen(szTemp4)==0) iColumn = 8;

	switch (iColumn) {
	case 5:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBSR_SERIALNUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL1,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL1,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to the process the rest
	case 1:
		//get date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATES1,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMES1,csTime);
		CurrentSetParam("SET_PSBSR_TIME1",csDateTime,false,true);

		//get external pwr supply voltage
		strncpy(szTemp1,(char*)pszRecord->cdfExternPWRSupply,sizeof(pszRecord->cdfExternPWRSupply));
		szTemp1[sizeof(pszRecord->cdfExternPWRSupply)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_EXTPSV1,szTemp1);
		CurrentSetParam("SET_PSBSR_EXTERNPSVOLTS1",szTemp1,true,false);

		//get status byte
		strncpy(szTemp1,(char*)pszRecord->chStatusByte,sizeof(pszRecord->chStatusByte));
		szTemp1[sizeof(pszRecord->chStatusByte)]=NULL;
		CurrentSetParam("SET_PSBSR_STATUS1",szTemp1,true,false);
		sscanf(szTemp1,"%h2x",&iTemp);
		if (iTemp && 0x0080) strcat(szTemp1," Bat");
		if (iTemp && 0x0040) strcat(szTemp1," HV");
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_STATUS1,szTemp1);
		
		//get 5.0 volt output
		strncpy(szTemp1,(char*)pszRecord->cdf50VOutput,sizeof(pszRecord->cdf50VOutput));
		szTemp1[sizeof(pszRecord->cdf50VOutput)]=NULL;
		CurrentSetParam("SET_PSBSR_50VOLTOUT1",szTemp1,true,false);

		sscanf(szTemp1,"%f",&fTemp);

		bCurrentRed = false;
		bCurrentLow = false;
		bCurrentHigh= false;

		m_fCurP5 = fTemp;

		if (m_bFirstP5)
		{
			m_bFirstP5 = false;
			m_fLowP5 = m_fHighP5 = m_fCurP5;
		}
		if (m_fCurP5 < m_fLowP5)
		{
			bCurrentLow = true;
			m_fLowP5 = m_fCurP5;
		}
		else
			bCurrentLow = false;
		if (m_fCurP5 > m_fHighP5)
		{
			bCurrentHigh = true;
			m_fHighP5= m_fCurP5;
		}
		else
			bCurrentHigh = false;

		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		{
			bCurrentRed = true;			
			pPSBInfoPage->SetColor(IDC_PSBINFO_50OUT1,COLOR_RED);
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT2,COLOR_RED);
			m_bP5VoltOk = false;
			m_iPSBInfo_Tab |= 0x0001;
			m_iInst4Setup_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			m_bP5VoltOk = true;
		}

		sprintf(szTemp1,"%4.1f",m_fCurP5);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_50OUT1,szTemp1);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,szTemp1);
		//already set the color so match isn't needed
		//CurrentSetParam("SET_UGSR_50V",szTemp1,false,false);
		if (bCurrentRed)
			setdatapage->CurrentSetParamColor("SET_PSBSR_50VOLTOUT1",szTemp1,bCurrentRed);

		if (bCurrentRed && bCurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP5 >= m_fSup_P05H))// || (m_fHighP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP5);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fHighP5);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,szTemp1);

		//get 3.3 volt output
		strncpy(szTemp1,(char*)pszRecord->cdf33VOutput,sizeof(pszRecord->cdf33VOutput));
		szTemp1[sizeof(pszRecord->cdf33VOutput)]=NULL;
		CurrentSetParam("SET_PSBSR_33VOLTOUT1",szTemp1,true,false);

		sscanf(szTemp1,"%f",&fTemp);

		bCurrentRed = false;
		bCurrentLow = false;
		bCurrentHigh= false;

		m_fCurP33 = fTemp;

		if (m_bFirstP33)
		{
			m_bFirstP33 = false;
			m_fLowP33 = m_fHighP33 = m_fCurP33;
		}
		if (m_fCurP33 < m_fLowP33)
		{
			bCurrentLow = true;
			m_fLowP33 = m_fCurP33;
		}
		else
			bCurrentLow = false;
		if (m_fCurP33 > m_fHighP33)
		{
			bCurrentHigh = true;
			m_fHighP33= m_fCurP33;
		}
		else
			bCurrentHigh = false;

		if ((m_fCurP33 <= m_fSup_P33L) || (m_fCurP33 >= m_fSup_P33H))
		{
			bCurrentRed = true;			
			pPSBInfoPage->SetColor(IDC_PSBINFO_33OUT1,COLOR_RED);
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT2,COLOR_RED);
			m_bP33VoltOk = false;
			m_iPSBInfo_Tab |= 0x0001;
			m_iInst4Setup_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			m_bP33VoltOk = true;
		}

		sprintf(szTemp1,"%4.1f",m_fCurP5);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_33OUT1,szTemp1);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT2,szTemp1);
		//already set the color so match isn't needed
		//CurrentSetParam("SET_UGSR_33V",szTemp1,false,false);
		if (bCurrentRed)
			setdatapage->CurrentSetParamColor("SET_PSBSR_33VOLTOUT1",szTemp1,bCurrentRed);

		if (bCurrentRed && bCurrentLow && (m_fLowP33 <= m_fSup_P33L))// || (m_fLowP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP33 >= m_fSup_P33H))// || (m_fHighP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP33);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fHighP33);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT3,szTemp1);

		//set HV output
		strncpy(szTemp1,(char*)pszRecord->cdfHVOutputV,sizeof(pszRecord->cdfHVOutputV));
		szTemp1[sizeof(pszRecord->cdfHVOutputV)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVOUT1,szTemp1);
		CurrentSetParam("SET_PSBSR_HVVOLTOUT1",szTemp1,true,false);

		//set HV current
		strncpy(szTemp1,(char*)pszRecord->cdfHVOutputC,sizeof(pszRecord->cdfHVOutputC));
		szTemp1[sizeof(pszRecord->cdfHVOutputC)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_CPUTEMP1,szTemp1);
		CurrentSetParam("SET_PSBSR_HVCURROUT1",szTemp1,true,false);

		//set CPU Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
		CurrentSetParam("SET_PSBSR_CPUTEMPERATURE1",szTemp1,true,false);

		//set BOARD Temperature
		strncpy(szTemp2,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTemp2[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
		CurrentSetParam("SET_PSBSR_BOARDTEMPERATURE1",szTemp2,true,false);
		strcat(szTemp1," / ");
		strcat(szTemp1,szTemp2);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_BOARDTEMP1,szTemp1);

		//set Reserved
		strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTemp1[sizeof(pszRecord->chReserved)]=NULL;
		CurrentSetParam("SET_PSBSR_RESERVED1",szTemp1,true,false);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_RESERVE1,szTemp1);
		
		break;

	case 6:
		//set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBSR_SERIALNUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL2,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL2,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to the process the rest
	case 2:
		//get date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATES2,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMES2,csTime);
//		sprintf(szTemp1,"%s",csDateTime);
		CurrentSetParam("SET_PSBSR_TIME2",csDateTime,false,true);

		//get external pwr supply voltage
		strncpy(szTemp1,(char*)pszRecord->cdfExternPWRSupply,sizeof(pszRecord->cdfExternPWRSupply));
		szTemp1[sizeof(pszRecord->cdfExternPWRSupply)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_EXTPSV2,szTemp1);
		CurrentSetParam("SET_PSBSR_EXTERNPSVOLTS2",szTemp1,true,false);

		//get status byte
		strncpy(szTemp1,(char*)pszRecord->chStatusByte,sizeof(pszRecord->chStatusByte));
		szTemp1[sizeof(pszRecord->chStatusByte)]=NULL;
		CurrentSetParam("SET_PSBSR_STATUS2",szTemp1,true,false);
		sscanf(szTemp1,"%h2x",&iTemp);
		if (iTemp && 0x0080) strcat(szTemp1," Bat");
		if (iTemp && 0x0040) strcat(szTemp1," HV");
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_STATUS2,szTemp1);
		
		//get 5.0 volt output
		strncpy(szTemp1,(char*)pszRecord->cdf50VOutput,sizeof(pszRecord->cdf50VOutput));
		szTemp1[sizeof(pszRecord->cdf50VOutput)]=NULL;
		CurrentSetParam("SET_PSBSR_50VOLTOUT2",szTemp1,true,false);

		sscanf(szTemp1,"%f",&fTemp);

		bCurrentRed = false;
		bCurrentLow = false;
		bCurrentHigh= false;

		m_fCurP5 = fTemp;

		if (m_bFirstP5)
		{
			m_bFirstP5 = false;
			m_fLowP5 = m_fHighP5 = m_fCurP5;
		}
		if (m_fCurP5 < m_fLowP5)
		{
			bCurrentLow = true;
			m_fLowP5 = m_fCurP5;
		}
		else
			bCurrentLow = false;
		if (m_fCurP5 > m_fHighP5)
		{
			bCurrentHigh = true;
			m_fHighP5= m_fCurP5;
		}
		else
			bCurrentHigh = false;

		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		{
			bCurrentRed = true;			
			pPSBInfoPage->SetColor(IDC_PSBINFO_50OUT2,COLOR_RED);
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT2,COLOR_RED);
			m_bP5VoltOk = false;
			m_iPSBInfo_Tab |= 0x0001;
			m_iInst4Setup_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			m_bP5VoltOk = true;
		}

		sprintf(szTemp1,"%4.1f",m_fCurP5);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_50OUT2,szTemp1);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,szTemp1);
		//already set the color so match isn't needed
		//CurrentSetParam("SET_UGSR_50V",szTemp1,false,false);
		if (bCurrentRed)
			setdatapage->CurrentSetParamColor("SET_PSBSR_50VOLTOUT2",szTemp1,bCurrentRed);

		if (bCurrentRed && bCurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP5 >= m_fSup_P05H))// || (m_fHighP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP5);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fHighP5);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,szTemp1);

		//get 3.3 volt output
		strncpy(szTemp1,(char*)pszRecord->cdf33VOutput,sizeof(pszRecord->cdf33VOutput));
		szTemp1[sizeof(pszRecord->cdf33VOutput)]=NULL;
		CurrentSetParam("SET_PSBSR_33VOLTOUT2",szTemp1,true,false);

		sscanf(szTemp1,"%f",&fTemp);

		bCurrentRed = false;
		bCurrentLow = false;
		bCurrentHigh= false;

		m_fCurP33 = fTemp;

		if (m_bFirstP33)
		{
			m_bFirstP33 = false;
			m_fLowP33 = m_fHighP33 = m_fCurP33;
		}
		if (m_fCurP33 < m_fLowP33)
		{
			bCurrentLow = true;
			m_fLowP33 = m_fCurP33;
		}
		else
			bCurrentLow = false;
		if (m_fCurP33 > m_fHighP33)
		{
			bCurrentHigh = true;
			m_fHighP33= m_fCurP33;
		}
		else
			bCurrentHigh = false;

		if ((m_fCurP33 <= m_fSup_P33L) || (m_fCurP33 >= m_fSup_P33H))
		{
			bCurrentRed = true;			
			pPSBInfoPage->SetColor(IDC_PSBINFO_33OUT2,COLOR_RED);
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT2,COLOR_RED);
			m_bP33VoltOk = false;
			m_iPSBInfo_Tab |= 0x0001;
			m_iInst4Setup_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			m_bP33VoltOk = true;
		}

		sprintf(szTemp1,"%4.1f",m_fCurP5);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_33OUT2,szTemp1);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT2,szTemp1);
		//already set the color so match isn't needed
		//CurrentSetParam("SET_UGSR_33V",szTemp1,false,false);
		if (bCurrentRed)
			setdatapage->CurrentSetParamColor("SET_PSBSR_33VOLTOUT2",szTemp1,bCurrentRed);

		if (bCurrentRed && bCurrentLow && (m_fLowP33 <= m_fSup_P33L))// || (m_fLowP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP33 >= m_fSup_P33H))// || (m_fHighP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP33);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fHighP33);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT3,szTemp1);

		//set HV output
		strncpy(szTemp1,(char*)pszRecord->cdfHVOutputV,sizeof(pszRecord->cdfHVOutputV));
		szTemp1[sizeof(pszRecord->cdfHVOutputV)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVOUT2,szTemp1);
		CurrentSetParam("SET_PSBSR_HVVOLTOUT2",szTemp1,true,false);

		//set HV current
		strncpy(szTemp1,(char*)pszRecord->cdfHVOutputC,sizeof(pszRecord->cdfHVOutputC));
		szTemp1[sizeof(pszRecord->cdfHVOutputC)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_CPUTEMP2,szTemp1);
		CurrentSetParam("SET_PSBSR_HVCURROUT2",szTemp1,true,false);

		//set CPU Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
		CurrentSetParam("SET_PSBSR_CPUTEMPERATURE2",szTemp1,true,false);

		//set BOARD Temperature
		strncpy(szTemp2,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTemp2[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
		CurrentSetParam("SET_PSBSR_BOARDTEMPERATURE2",szTemp2,true,false);
		strcat(szTemp1," / ");
		strcat(szTemp1,szTemp2);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_BOARDTEMP2,szTemp1);

		//set Reserved
		strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTemp1[sizeof(pszRecord->chReserved)]=NULL;
		CurrentSetParam("SET_PSBSR_RESERVED2",szTemp1,true,false);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_RESERVE2,szTemp1);
		
		break;

	case 7:
		//set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBSR_SERIALNUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL3,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL3,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to the process the rest
	case 3:
		//get date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATES3,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMES3,csTime);
//		sprintf(szTemp1,"%s",csDateTime);
		CurrentSetParam("SET_PSBSR_TIME3",csDateTime,false,true);

		//get external pwr supply voltage
		strncpy(szTemp1,(char*)pszRecord->cdfExternPWRSupply,sizeof(pszRecord->cdfExternPWRSupply));
		szTemp1[sizeof(pszRecord->cdfExternPWRSupply)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_EXTPSV3,szTemp1);
		CurrentSetParam("SET_PSBSR_EXTERNPSVOLTS3",szTemp1,true,false);

		//get status byte
		strncpy(szTemp1,(char*)pszRecord->chStatusByte,sizeof(pszRecord->chStatusByte));
		szTemp1[sizeof(pszRecord->chStatusByte)]=NULL;
		CurrentSetParam("SET_PSBSR_STATUS3",szTemp1,true,false);
		sscanf(szTemp1,"%h2x",&iTemp);
		if (iTemp && 0x0080) strcat(szTemp1," Bat");
		if (iTemp && 0x0040) strcat(szTemp1," HV");
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_STATUS3,szTemp1);
		
		//get 5.0 volt output
		strncpy(szTemp1,(char*)pszRecord->cdf50VOutput,sizeof(pszRecord->cdf50VOutput));
		szTemp1[sizeof(pszRecord->cdf50VOutput)]=NULL;
		CurrentSetParam("SET_PSBSR_50VOLTOUT3",szTemp1,true,false);

		sscanf(szTemp1,"%f",&fTemp);

		bCurrentRed = false;
		bCurrentLow = false;
		bCurrentHigh= false;

		m_fCurP5 = fTemp;

		if (m_bFirstP5)
		{
			m_bFirstP5 = false;
			m_fLowP5 = m_fHighP5 = m_fCurP5;
		}
		if (m_fCurP5 < m_fLowP5)
		{
			bCurrentLow = true;
			m_fLowP5 = m_fCurP5;
		}
		else
			bCurrentLow = false;
		if (m_fCurP5 > m_fHighP5)
		{
			bCurrentHigh = true;
			m_fHighP5= m_fCurP5;
		}
		else
			bCurrentHigh = false;

		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		{
			bCurrentRed = true;			
			pPSBInfoPage->SetColor(IDC_PSBINFO_50OUT3,COLOR_RED);
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT2,COLOR_RED);
			m_bP5VoltOk = false;
			m_iPSBInfo_Tab |= 0x0001;
			m_iInst4Setup_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			m_bP5VoltOk = true;
		}

		sprintf(szTemp1,"%4.1f",m_fCurP5);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_50OUT3,szTemp1);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,szTemp1);
		//already set the color so match isn't needed
		//CurrentSetParam("SET_UGSR_50V",szTemp1,false,false);
		if (bCurrentRed)
			setdatapage->CurrentSetParamColor("SET_PSBSR_50VOLTOUT3",szTemp1,bCurrentRed);

		if (bCurrentRed && bCurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP5 >= m_fSup_P05H))// || (m_fHighP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP5);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fHighP5);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,szTemp1);

		//get 3.3 volt output
		strncpy(szTemp1,(char*)pszRecord->cdf33VOutput,sizeof(pszRecord->cdf33VOutput));
		szTemp1[sizeof(pszRecord->cdf33VOutput)]=NULL;
		CurrentSetParam("SET_PSBSR_33VOLTOUT3",szTemp1,true,false);

		sscanf(szTemp1,"%f",&fTemp);

		bCurrentRed = false;
		bCurrentLow = false;
		bCurrentHigh= false;

		m_fCurP33 = fTemp;

		if (m_bFirstP33)
		{
			m_bFirstP33 = false;
			m_fLowP33 = m_fHighP33 = m_fCurP33;
		}
		if (m_fCurP33 < m_fLowP33)
		{
			bCurrentLow = true;
			m_fLowP33 = m_fCurP33;
		}
		else
			bCurrentLow = false;
		if (m_fCurP33 > m_fHighP33)
		{
			bCurrentHigh = true;
			m_fHighP33= m_fCurP33;
		}
		else
			bCurrentHigh = false;

		if ((m_fCurP33 <= m_fSup_P33L) || (m_fCurP33 >= m_fSup_P33H))
		{
			bCurrentRed = true;			
			pPSBInfoPage->SetColor(IDC_PSBINFO_33OUT3,COLOR_RED);
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT2,COLOR_RED);
			m_bP33VoltOk = false;
			m_iPSBInfo_Tab |= 0x0001;
			m_iInst4Setup_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			m_bP33VoltOk = true;
		}

		sprintf(szTemp1,"%4.1f",m_fCurP5);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_33OUT3,szTemp1);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT2,szTemp1);
		//already set the color so match isn't needed
		//CurrentSetParam("SET_UGSR_33V",szTemp1,false,false);
		if (bCurrentRed)
			setdatapage->CurrentSetParamColor("SET_PSBSR_33VOLTOUT3",szTemp1,bCurrentRed);

		if (bCurrentRed && bCurrentLow && (m_fLowP33 <= m_fSup_P33L))// || (m_fLowP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP33 >= m_fSup_P33H))// || (m_fHighP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP33);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fHighP33);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT3,szTemp1);

		//set HV output
		strncpy(szTemp1,(char*)pszRecord->cdfHVOutputV,sizeof(pszRecord->cdfHVOutputV));
		szTemp1[sizeof(pszRecord->cdfHVOutputV)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVOUT3,szTemp1);
		CurrentSetParam("SET_PSBSR_HVVOLTOUT3",szTemp1,true,false);

		//set HV current
		strncpy(szTemp1,(char*)pszRecord->cdfHVOutputC,sizeof(pszRecord->cdfHVOutputC));
		szTemp1[sizeof(pszRecord->cdfHVOutputC)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_CPUTEMP3,szTemp1);
		CurrentSetParam("SET_PSBSR_HVCURROUT3",szTemp1,true,false);

		//set CPU Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
		CurrentSetParam("SET_PSBSR_CPUTEMPERATURE3",szTemp1,true,false);

		//set BOARD Temperature
		strncpy(szTemp2,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTemp2[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
		CurrentSetParam("SET_PSBSR_BOARDTEMPERATURE3",szTemp2,true,false);
		strcat(szTemp1," / ");
		strcat(szTemp1,szTemp2);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_BOARDTEMP3,szTemp1);

		//set Reserved
		strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTemp1[sizeof(pszRecord->chReserved)]=NULL;
		CurrentSetParam("SET_PSBSR_RESERVED3",szTemp1,true,false);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_RESERVE3,szTemp1);
		
		break;

	case 8:
		//set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBSR_SERIALNUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL4,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL4,COLOR_YELLOW);
		CurrentSetParam("SET_PSBSR_SERIALNUM4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to the process the rest
	case 4:
		//get date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATES4,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMES4,csTime);
//		sprintf(szTemp1,"%s",csDateTime);
		CurrentSetParam("SET_PSBSR_TIME4",csDateTime,false,true);

		//get external pwr supply voltage
		strncpy(szTemp1,(char*)pszRecord->cdfExternPWRSupply,sizeof(pszRecord->cdfExternPWRSupply));
		szTemp1[sizeof(pszRecord->cdfExternPWRSupply)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_EXTPSV4,szTemp1);
		CurrentSetParam("SET_PSBSR_EXTERNPSVOLTS4",szTemp1,true,false);

		//get status byte
		strncpy(szTemp1,(char*)pszRecord->chStatusByte,sizeof(pszRecord->chStatusByte));
		szTemp1[sizeof(pszRecord->chStatusByte)]=NULL;
		CurrentSetParam("SET_PSBSR_STATUS4",szTemp1,true,false);
		sscanf(szTemp1,"%h2x",&iTemp);
		if (iTemp && 0x0080) strcat(szTemp1," Bat");
		if (iTemp && 0x0040) strcat(szTemp1," HV");
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_STATUS4,szTemp1);
		
		//get 5.0 volt output
		strncpy(szTemp1,(char*)pszRecord->cdf50VOutput,sizeof(pszRecord->cdf50VOutput));
		szTemp1[sizeof(pszRecord->cdf50VOutput)]=NULL;
		CurrentSetParam("SET_PSBSR_50VOLTOUT4",szTemp1,true,false);

		sscanf(szTemp1,"%f",&fTemp);

		bCurrentRed = false;
		bCurrentLow = false;
		bCurrentHigh= false;

		m_fCurP5 = fTemp;

		if (m_bFirstP5)
		{
			m_bFirstP5 = false;
			m_fLowP5 = m_fHighP5 = m_fCurP5;
		}
		if (m_fCurP5 < m_fLowP5)
		{
			bCurrentLow = true;
			m_fLowP5 = m_fCurP5;
		}
		else
			bCurrentLow = false;
		if (m_fCurP5 > m_fHighP5)
		{
			bCurrentHigh = true;
			m_fHighP5= m_fCurP5;
		}
		else
			bCurrentHigh = false;

		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		{
			bCurrentRed = true;			
			pPSBInfoPage->SetColor(IDC_PSBINFO_50OUT4,COLOR_RED);
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT2,COLOR_RED);
			m_bP5VoltOk = false;
			m_iPSBInfo_Tab |= 0x0001;
			m_iInst4Setup_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			m_bP5VoltOk = true;
		}

		sprintf(szTemp1,"%4.1f",m_fCurP5);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_50OUT4,szTemp1);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,szTemp1);
		//already set the color so match isn't needed
		//CurrentSetParam("SET_UGSR_50V",szTemp1,false,false);
		if (bCurrentRed)
			setdatapage->CurrentSetParamColor("SET_PSBSR_50VOLTOUT4",szTemp1,bCurrentRed);

		if (bCurrentRed && bCurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP5 >= m_fSup_P05H))// || (m_fHighP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P5VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP5);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fHighP5);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,szTemp1);

		//get 3.3 volt output
		strncpy(szTemp1,(char*)pszRecord->cdf33VOutput,sizeof(pszRecord->cdf33VOutput));
		szTemp1[sizeof(pszRecord->cdf33VOutput)]=NULL;
		CurrentSetParam("SET_PSBSR_33VOLTOUT4",szTemp1,true,false);

		sscanf(szTemp1,"%f",&fTemp);

		bCurrentRed = false;
		bCurrentLow = false;
		bCurrentHigh= false;

		m_fCurP33 = fTemp;

		if (m_bFirstP33)
		{
			m_bFirstP33 = false;
			m_fLowP33 = m_fHighP33 = m_fCurP33;
		}
		if (m_fCurP33 < m_fLowP33)
		{
			bCurrentLow = true;
			m_fLowP33 = m_fCurP33;
		}
		else
			bCurrentLow = false;
		if (m_fCurP33 > m_fHighP33)
		{
			bCurrentHigh = true;
			m_fHighP33= m_fCurP33;
		}
		else
			bCurrentHigh = false;

		if ((m_fCurP33 <= m_fSup_P33L) || (m_fCurP33 >= m_fSup_P33H))
		{
			bCurrentRed = true;			
			pPSBInfoPage->SetColor(IDC_PSBINFO_33OUT4,COLOR_RED);
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT2,COLOR_RED);
			m_bP33VoltOk = false;
			m_iPSBInfo_Tab |= 0x0001;
			m_iInst4Setup_Tab |= 0x0001;
		}
		else
		{
			bCurrentRed = false;
			m_bP33VoltOk = true;
		}

		sprintf(szTemp1,"%4.1f",m_fCurP5);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_33OUT4,szTemp1);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT2,szTemp1);
		//already set the color so match isn't needed
		//CurrentSetParam("SET_UGSR_33V",szTemp1,false,false);
		if (bCurrentRed)
			setdatapage->CurrentSetParamColor("SET_PSBSR_33VOLTOUT4",szTemp1,bCurrentRed);

		if (bCurrentRed && bCurrentLow && (m_fLowP33 <= m_fSup_P33L))// || (m_fLowP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT1,COLOR_RED);

		if (bCurrentRed && bCurrentHigh && (m_fHighP33 >= m_fSup_P33H))// || (m_fHighP5 >= m_fSup_P05H))
			pSummaryPage->SetColor(IDC_SUMMARY_P33VOLT3,COLOR_RED);

		sprintf(szTemp1,"%4.1f",m_fLowP33);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT1,szTemp1);

		sprintf(szTemp1,"%4.1f",m_fHighP33);
		pSummaryPage->SetDlgItemText(IDC_SUMMARY_P33VOLT3,szTemp1);

		//set HV output
		strncpy(szTemp1,(char*)pszRecord->cdfHVOutputV,sizeof(pszRecord->cdfHVOutputV));
		szTemp1[sizeof(pszRecord->cdfHVOutputV)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_HVOUT4,szTemp1);
		CurrentSetParam("SET_PSBSR_HVVOLTOUT4",szTemp1,true,false);

		//set HV current
		strncpy(szTemp1,(char*)pszRecord->cdfHVOutputC,sizeof(pszRecord->cdfHVOutputC));
		szTemp1[sizeof(pszRecord->cdfHVOutputC)]=NULL;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_CPUTEMP4,szTemp1);
		CurrentSetParam("SET_PSBSR_HVCURROUT4",szTemp1,true,false);

		//set CPU Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
		CurrentSetParam("SET_PSBSR_CPUTEMPERATURE4",szTemp1,true,false);

		//set BOARD Temperature
		strncpy(szTemp2,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTemp2[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
		CurrentSetParam("SET_PSBSR_BOARDTEMPERATURE4",szTemp2,true,false);
		strcat(szTemp1," / ");
		strcat(szTemp1,szTemp2);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_BOARDTEMP4,szTemp1);

		//set Reserved
		strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTemp1[sizeof(pszRecord->chReserved)]=NULL;
		CurrentSetParam("SET_PSBSR_RESERVED4",szTemp1,true,false);
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_RESERVE4,szTemp1);
		
		break;
	}

	m_pParent->m_iBMBInfo_Tab = m_iBMBInfo_Tab;
	if (m_iBMBInfo_Tab == 0x0003)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iBMBInfo_Tab == 0x0002)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOW);
	else if (m_iBMBInfo_Tab == 0x0001)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_RED);
	else if (m_iBMBInfo_Tab == 0x0000)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_GREEN);

	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);

}

void 
CSafePropertySheet::OnBMBStatusRecord(char* szRecord)			
{
	//some temporary buffers
	char szTemp1[32];
	char szTemp2[32];
	char szTemp3[32];
	char szTemp4[32];
	char szTempX[32];
	CString csTime;
	char szJulian10ths;

	CString csDateTime;

	bool bMatch;

	//we need to send stuff to the property page
	CSetupData* setdatapage = m_pParent->m_pSetupData;
	CBMBInfo	*pBMBInfoPage = m_pParent->m_pBMBInfo;

	//initialize to zero so as to not use if
	//more then four boards are being used
	int iColumn = 0;

	//get a pointer to a GeneralConfigTextRecord
	BMBStatusTextRecord* pszRecord = (BMBStatusTextRecord*)szRecord;

 	//get julian time 
	strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;

	//set the date & time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//get the 10th of a second
	szJulian10ths = pszRecord->cdJulianSec10ths;

	//set date and time
	csTime = cTime.Time();
	csTime += ".";
	csTime += pszRecord->cdJulianSec10ths;
	csDateTime = cTime.Date();
	csDateTime += " ";
	csDateTime += csTime;

	//get serial number for column placement
	strncpy(szTempX,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
	szTempX[sizeof(pszRecord->chSerialNumber)]=NULL;

	//get what is in which columns now
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL1,szTemp1,sizeof(szTemp1));
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL2,szTemp2,sizeof(szTemp2));
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL3,szTemp3,sizeof(szTemp3));
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL4,szTemp4,sizeof(szTemp4));

	//first see current matches any of the columns
	if      (strncmp((char*)&pszRecord->chSerialNumber,szTemp1,sizeof(pszRecord->chSerialNumber)) == 0)	iColumn = 1;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp2,sizeof(pszRecord->chSerialNumber)) == 0)	iColumn = 2;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp3,sizeof(pszRecord->chSerialNumber)) == 0)	iColumn = 3;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp4,sizeof(pszRecord->chSerialNumber)) == 0)	iColumn = 4;
	//if not then find the first unused column
	else if (strlen(szTemp1)==0) iColumn = 5;
	else if (strlen(szTemp2)==0) iColumn = 6;
	else if (strlen(szTemp3)==0) iColumn = 7;
	else if (strlen(szTemp4)==0) iColumn = 8;

	switch (iColumn) {
	case 5:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBSR_SERIALNUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL1,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
		CurrentSetParam("SET_BMBSR_SERIALNUM1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to the process the rest
	case 1:
		//set date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATES1,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMES1,csTime);
//		sprintf(szTemp1,"%s",csDateTime);
		CurrentSetParam("SET_BMBSR_TIME1",csDateTime,false,true);

		//set charge current
		strncpy(szTemp1,(char*)pszRecord->cdfChargeCurrent,sizeof(pszRecord->cdfChargeCurrent));
		szTemp1[sizeof(pszRecord->cdfChargeCurrent)]=NULL;
//		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBSR_CHARGECURNT1","",szTempX,sizeof(szTempX),m_szIniFile);
//		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CHARGECNT1,szTemp1);
//		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_CHARGECNT1,COLOR_YELLOW);
		CurrentSetParam("SET_BMBSR_CHARGECURNT1",szTemp1,true,false);
//		if (!bMatch)
//		{
//			m_iESBInfo_Tab |= 0x0002;
//			m_iInst4Setup_Tab |= 0x0001;
//		}

		//set discharge current
		strncpy(szTemp1,(char*)pszRecord->cdfDischargeCurr,sizeof(pszRecord->cdfDischargeCurr));
		szTemp1[sizeof(pszRecord->cdfDischargeCurr)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DISCHARGECNT1,szTemp1);
		CurrentSetParam("SET_BMBSR_DISCHGCURNT1",szTemp1,true,false);
		
		//set charge / discharge cycles
		strncpy(szTemp1,(char*)pszRecord->cdChDisCycles,sizeof(pszRecord->cdChDisCycles));
		szTemp1[sizeof(pszRecord->cdChDisCycles)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CDCYCLES1,szTemp1);
		CurrentSetParam("SET_BMBSR_CDCYCLES1",szTemp1,true,false);
		
		//set CPU Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_CPUTEMP1",szTemp1,true,false);

		//set BOARD Temperature
		strncpy(szTemp2,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTemp2[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_BOARDTEMP1",szTemp2,true,false);
		strcat(szTemp1," / ");
		strcat(szTemp1,szTemp2);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CPUBOARDT1,szTemp1);

		//set Time on Battery
		strncpy(szTemp1,(char*)pszRecord->cdTimeOnBatt,sizeof(pszRecord->cdTimeOnBatt));
		szTemp1[sizeof(pszRecord->cdTimeOnBatt)]=NULL;
		CurrentSetParam("SET_BMBSR_TIMEONBAT1",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_TIMEONBAT1,szTemp1);

		//set Battery Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfBattTemp,sizeof(pszRecord->cdfBattTemp));
		szTemp1[sizeof(pszRecord->cdfBattTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_BATTTEMP1",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_BATTEMP1,szTemp1);

		//set Battery Status
		strncpy(szTemp1,(char*)pszRecord->chBattStatus,sizeof(pszRecord->chBattStatus));
		szTemp1[sizeof(pszRecord->chBattStatus)]=NULL;
		CurrentSetParam("SET_BMBSR_BATTSTATUS1",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_BATSTAT1,szTemp1);

		//set Reserved
		strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTemp1[sizeof(pszRecord->chReserved)]=NULL;
		CurrentSetParam("SET_BMBSR_RESERVED1",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_RESERVE1,szTemp1);
		
		break;

	case 6:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBSR_SERIALNUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL2,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
		CurrentSetParam("SET_BMBSR_SERIALNUM2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to the process the rest
	case 2:
		//set date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATES2,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMES2,csTime);
//		sprintf(szTemp1,"%s",csDateTime);
		CurrentSetParam("SET_BMBSR_TIME2",csDateTime,false,true);

		//set charge current
		strncpy(szTemp1,(char*)pszRecord->cdfChargeCurrent,sizeof(pszRecord->cdfChargeCurrent));
		szTemp1[sizeof(pszRecord->cdfChargeCurrent)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CHARGECNT2,szTemp1);
		CurrentSetParam("SET_BMBSR_CHARGECURNT2",szTemp1,true,false);

		//set discharge current
		strncpy(szTemp1,(char*)pszRecord->cdfDischargeCurr,sizeof(pszRecord->cdfDischargeCurr));
		szTemp1[sizeof(pszRecord->cdfDischargeCurr)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DISCHARGECNT2,szTemp1);
		CurrentSetParam("SET_BMBSR_DISCHGCURNT2",szTemp1,true,false);
		
		//set charge / discharge cycles
		strncpy(szTemp1,(char*)pszRecord->cdChDisCycles,sizeof(pszRecord->cdChDisCycles));
		szTemp1[sizeof(pszRecord->cdChDisCycles)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CDCYCLES2,szTemp1);
		CurrentSetParam("SET_BMBSR_CDCYCLES2",szTemp1,true,false);
		
		//set CPU Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_CPUTEMP2",szTemp1,true,false);

		//set BOARD Temperature
		strncpy(szTemp2,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTemp2[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_BOARDTEMP2",szTemp2,true,false);
		strcat(szTemp1," / ");
		strcat(szTemp1,szTemp2);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CPUBOARDT2,szTemp1);

		//set Time on Battery
		strncpy(szTemp1,(char*)pszRecord->cdTimeOnBatt,sizeof(pszRecord->cdTimeOnBatt));
		szTemp1[sizeof(pszRecord->cdTimeOnBatt)]=NULL;
		CurrentSetParam("SET_BMBSR_TIMEONBAT2",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_TIMEONBAT2,szTemp1);

		//set Battery Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfBattTemp,sizeof(pszRecord->cdfBattTemp));
		szTemp1[sizeof(pszRecord->cdfBattTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_BATTTEMP2",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_BATTEMP2,szTemp1);

		//set Battery Status
		strncpy(szTemp1,(char*)pszRecord->chBattStatus,sizeof(pszRecord->chBattStatus));
		szTemp1[sizeof(pszRecord->chBattStatus)]=NULL;
		CurrentSetParam("SET_BMBSR_BATTSTATUS2",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_BATSTAT2,szTemp1);

		//set Reserved
		strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTemp1[sizeof(pszRecord->chReserved)]=NULL;
		CurrentSetParam("SET_BMBSR_RESERVED2",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_RESERVE2,szTemp1);
		
		break;

	case 7:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBSR_SERIALNUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL3,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
		CurrentSetParam("SET_BMBSR_SERIALNUM3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to the process the rest
	case 3:
		//set date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATES3,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMES3,csTime);
//		sprintf(szTemp1,"%s",csDateTime);
		CurrentSetParam("SET_BMBSR_TIME3",csDateTime,false,true);

		//set charge current
		strncpy(szTemp1,(char*)pszRecord->cdfChargeCurrent,sizeof(pszRecord->cdfChargeCurrent));
		szTemp1[sizeof(pszRecord->cdfChargeCurrent)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CHARGECNT3,szTemp1);
		CurrentSetParam("SET_BMBSR_CHARGECURNT3",szTemp1,true,false);

		//set discharge current
		strncpy(szTemp1,(char*)pszRecord->cdfDischargeCurr,sizeof(pszRecord->cdfDischargeCurr));
		szTemp1[sizeof(pszRecord->cdfDischargeCurr)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DISCHARGECNT3,szTemp1);
		CurrentSetParam("SET_BMBSR_DISCHGCURNT3",szTemp1,true,false);
		
		//set charge / discharge cycles
		strncpy(szTemp1,(char*)pszRecord->cdChDisCycles,sizeof(pszRecord->cdChDisCycles));
		szTemp1[sizeof(pszRecord->cdChDisCycles)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CDCYCLES3,szTemp1);
		CurrentSetParam("SET_BMBSR_CDCYCLES3",szTemp1,true,false);
		
		//set CPU Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_CPUTEMP3",szTemp1,true,false);

		//set BOARD Temperature
		strncpy(szTemp2,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTemp2[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_BOARDTEMP3",szTemp2,true,false);
		strcat(szTemp1," / ");
		strcat(szTemp1,szTemp2);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CPUBOARDT3,szTemp1);

		//set Time on Battery
		strncpy(szTemp1,(char*)pszRecord->cdTimeOnBatt,sizeof(pszRecord->cdTimeOnBatt));
		szTemp1[sizeof(pszRecord->cdTimeOnBatt)]=NULL;
		CurrentSetParam("SET_BMBSR_TIMEONBAT3",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_TIMEONBAT3,szTemp1);

		//set Battery Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfBattTemp,sizeof(pszRecord->cdfBattTemp));
		szTemp1[sizeof(pszRecord->cdfBattTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_BATTTEMP3",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_BATTEMP3,szTemp1);

		//set Battery Status
		strncpy(szTemp1,(char*)pszRecord->chBattStatus,sizeof(pszRecord->chBattStatus));
		szTemp1[sizeof(pszRecord->chBattStatus)]=NULL;
		CurrentSetParam("SET_BMBSR_BATTSTATUS3",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_BATSTAT3,szTemp1);

		//set Reserved
		strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTemp1[sizeof(pszRecord->chReserved)]=NULL;
		CurrentSetParam("SET_BMBSR_RESERVED3",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_RESERVE3,szTemp1);
		break;

	case 8:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBSR_SERIALNUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL4,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
		CurrentSetParam("SET_BMBSR_SERIALNUM4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to the process the rest
	case 4:
		//set date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATES4,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMES4,csTime);
//		sprintf(szTemp1,"%s",csDateTime);
		CurrentSetParam("SET_BMBSR_TIME4",csDateTime,false,true);

		//set charge current
		strncpy(szTemp1,(char*)pszRecord->cdfChargeCurrent,sizeof(pszRecord->cdfChargeCurrent));
		szTemp1[sizeof(pszRecord->cdfChargeCurrent)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CHARGECNT4,szTemp1);
		CurrentSetParam("SET_BMBSR_CHARGECURNT4",szTemp1,true,false);

		//set discharge current
		strncpy(szTemp1,(char*)pszRecord->cdfDischargeCurr,sizeof(pszRecord->cdfDischargeCurr));
		szTemp1[sizeof(pszRecord->cdfDischargeCurr)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DISCHARGECNT4,szTemp1);
		CurrentSetParam("SET_BMBSR_DISCHGCURNT4",szTemp1,true,false);
		
		//set charge / discharge cycles
		strncpy(szTemp1,(char*)pszRecord->cdChDisCycles,sizeof(pszRecord->cdChDisCycles));
		szTemp1[sizeof(pszRecord->cdChDisCycles)]=NULL;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CDCYCLES4,szTemp1);
		CurrentSetParam("SET_BMBSR_CDCYCLES4",szTemp1,true,false);
		
		//set CPU Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfCPUTemp,sizeof(pszRecord->cdfCPUTemp));
		szTemp1[sizeof(pszRecord->cdfCPUTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_CPUTEMP4",szTemp1,true,false);

		//set BOARD Temperature
		strncpy(szTemp2,(char*)pszRecord->cdfBOARDTemp,sizeof(pszRecord->cdfBOARDTemp));
		szTemp2[sizeof(pszRecord->cdfBOARDTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_BOARDTEMP4",szTemp2,true,false);
		strcat(szTemp1," / ");
		strcat(szTemp1,szTemp2);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_CPUBOARDT4,szTemp1);

		//set Time on Battery
		strncpy(szTemp1,(char*)pszRecord->cdTimeOnBatt,sizeof(pszRecord->cdTimeOnBatt));
		szTemp1[sizeof(pszRecord->cdTimeOnBatt)]=NULL;
		CurrentSetParam("SET_BMBSR_TIMEONBAT4",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_TIMEONBAT4,szTemp1);

		//set Battery Temperature
		strncpy(szTemp1,(char*)pszRecord->cdfBattTemp,sizeof(pszRecord->cdfBattTemp));
		szTemp1[sizeof(pszRecord->cdfBattTemp)]=NULL;
		CurrentSetParam("SET_BMBSR_BATTTEMP4",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_BATTEMP4,szTemp1);

		//set Battery Status
		strncpy(szTemp1,(char*)pszRecord->chBattStatus,sizeof(pszRecord->chBattStatus));
		szTemp1[sizeof(pszRecord->chBattStatus)]=NULL;
		CurrentSetParam("SET_BMBSR_BATTSTATUS4",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_BATSTAT4,szTemp1);

		//set Reserved
		strncpy(szTemp1,(char*)pszRecord->chReserved,sizeof(pszRecord->chReserved));
		szTemp1[sizeof(pszRecord->chReserved)]=NULL;
		CurrentSetParam("SET_BMBSR_RESERVED4",szTemp1,true,false);
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_RESERVE4,szTemp1);
		break;
	}

	m_pParent->m_iBMBInfo_Tab = m_iBMBInfo_Tab;
	if (m_iBMBInfo_Tab == 0x0003)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iBMBInfo_Tab == 0x0002)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOW);
	else if (m_iBMBInfo_Tab == 0x0001)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_RED);
	else if (m_iBMBInfo_Tab == 0x0000)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_GREEN);

	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);

}

void 
CSafePropertySheet::OnuGInfoRecord(char* szRecord)				
{
	//will need a few work areas
	char szTemp1[64];
	char szTempX[256];
	CUGINFO		*pUGInfoPage = m_pParent->m_pUGInfo;

	//do we have a match
	bool bMatch;

	//get a pointer to a GeneralConfigTextRecord
	uGInfoTextRecord* pszRecord = (uGInfoTextRecord*)szRecord;

 	//get julian time and put it in the binary record
	strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;

	//set the date & time
	CJulianTime cTime;
	cTime.Init(szTemp1);
	//set date and time
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_DATADATEI,cTime.Date());
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_DATATIMEI,cTime.Time());
	sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
	CurrentSetParam("SET_UGIR_TIME",szTemp1,false,true);

	//set Actel version
	strncpy(szTemp1,(char*)pszRecord->chActelVersion,sizeof(pszRecord->chActelVersion));
	szTemp1[sizeof(pszRecord->chActelVersion)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGIR_ACTELVER","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_ACTELVERI,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_ACTELVERI,COLOR_YELLOW);
	CurrentSetParam("SET_UGIR_ACTELVER",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set firmware version
	strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
	szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGIR_FIRMWAREVER","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_FIRMWVI,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_FIRMWVI,COLOR_YELLOW);
	CurrentSetParam("SET_UGIR_FIRMWAREVER",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set firmware checksum
	strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
	szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGIR_FIRMWARECKSUM","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_FIRMWCHKI,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_FIRMWCHKI,COLOR_YELLOW);
	CurrentSetParam("SET_UGIR_FIRMWARECKSUM",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	//set set serial number
	strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
	szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
	GetPrivateProfileString(m_pParent->m_pName,"SET_UGIR_SERIALNUM","",szTempX,sizeof(szTempX),m_szIniFile);
	bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
	pUGInfoPage->SetDlgItemText(IDC_UGINFO_UGRANDSNI,szTemp1);
	if (!bMatch) pUGInfoPage->SetColor(IDC_UGINFO_UGRANDSNI,COLOR_YELLOW);
	CurrentSetParam("SET_UGIR_SERIALNUM",szTemp1,bMatch,false);
	if (!bMatch)
	{
		m_iuGInfo_Tab |= 0x0002;
		m_iInst4Setup_Tab |= 0x0001;
	}

	if (m_iuGInfo_Tab == 0x0003)
		SetTabColor(IDC_UGINFO_TAB,COLOR_YELLOWRED);
	else if (m_iuGInfo_Tab == 0x0002)
		SetTabColor(IDC_UGINFO_TAB,COLOR_YELLOW);
	else if (m_iuGInfo_Tab == 0x0001)
		SetTabColor(IDC_UGINFO_TAB,COLOR_RED);
	else if (m_iuGInfo_Tab == 0x0000)
		SetTabColor(IDC_UGINFO_TAB,COLOR_GREEN);

	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);
}

void 
CSafePropertySheet::OnESBInfoRecord(char* szRecord)			
{
	//will need a few work areas
	char szTempX[256];
	char szTemp1[64];
	char szTemp2[64];
	char szTemp3[64];
	char szTemp4[64];

	//initialize to zero so as to not use if
	//more then four boards are being used
	int iColumn = 0;

	CESBInfo	*pESBInfoPage = m_pParent->m_pESBInfo;

	//do we have a match
	bool bMatch;

	//get a pointer to a GeneralConfigTextRecord
	ESBInfoTextRecord* pszRecord = (ESBInfoTextRecord*)szRecord;

 	//get julian time and put it in the binary record
	strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;
	//set the date & time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//get serial number for column placement
	strncpy(szTempX,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
	szTempX[sizeof(pszRecord->chSerialNumber)]=NULL;

	//get what is in which columns now
	pESBInfoPage->GetDlgItemText(IDC_ESBINFO_SERIAL1,szTemp1,sizeof(szTemp1));
	pESBInfoPage->GetDlgItemText(IDC_ESBINFO_SERIAL2,szTemp2,sizeof(szTemp2));
	pESBInfoPage->GetDlgItemText(IDC_ESBINFO_SERIAL3,szTemp3,sizeof(szTemp3));
	pESBInfoPage->GetDlgItemText(IDC_ESBINFO_SERIAL4,szTemp4,sizeof(szTemp4));

	//first see current matches any of the columns
	if      (strncmp((char*)&pszRecord->chSerialNumber,szTemp1,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 1;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp2,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 2;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp3,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 3;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp4,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 4;
	//if not then find the first unused column
	else if (strlen(szTemp1)==0) iColumn = 5;
	else if (strlen(szTemp2)==0) iColumn = 6;
	else if (strlen(szTemp3)==0) iColumn = 7;
	else if (strlen(szTemp4)==0) iColumn = 8;

	switch (iColumn) {
	case 5:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_SERIALNUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_SERIAL1,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_SERIAL1,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_SERIALNUM1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 1:
		//set date and time
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATADATEI1,cTime.Date());
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATATIMEI1,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_ESBIR_TIME1",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_FIRMWAREVER1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_FIRMWAREV1,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_FIRMWAREV1,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_FIRMWAREVER1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_FIRMWARECKSUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_FIRMWAREC1,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_FIRMWAREC1,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_FIRMWARECKSUM1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		break;
	case 6:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_SERIALNUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_SERIAL2,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_SERIAL2,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_SERIALNUM2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 2:
		//set date and time
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATADATEI2,cTime.Date());
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATATIMEI2,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_ESBIR_TIME2",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_FIRMWAREVER2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_FIRMWAREV2,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_FIRMWAREV2,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_FIRMWAREVER2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_FIRMWARECKSUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_FIRMWAREC2,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_FIRMWAREC2,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_FIRMWARECKSUM2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	case 7:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_SERIALNUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_SERIAL3,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_SERIAL3,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_SERIALNUM3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 3:
		//set date and time
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATADATEI3,cTime.Date());
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATATIMEI3,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_ESBIR_TIME3",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_FIRMWAREVER3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_FIRMWAREV3,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_FIRMWAREV3,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_FIRMWAREVER3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_FIRMWARECKSUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_FIRMWAREC3,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_FIRMWAREC3,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_FIRMWARECKSUM3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	case 8:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_SERIALNUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_SERIAL4,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_SERIAL4,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_SERIALNUM4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 4:
		//set date and time
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATADATEI4,cTime.Date());
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_DATATIMEI4,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_ESBIR_TIME4",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_FIRMWAREVER4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_FIRMWAREV4,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_FIRMWAREV4,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_FIRMWAREVER4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ESBIR_FIRMWARECKSUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pESBInfoPage->SetDlgItemText(IDC_ESBINFO_FIRMWAREC4,szTemp1);
		if (!bMatch) pESBInfoPage->SetColor(IDC_ESBINFO_FIRMWAREC4,COLOR_YELLOW);
		CurrentSetParam("SET_ESBIR_FIRMWARECKSUM4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iESBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	}

	//set the tabs on the ESB Info page
	if (m_iESBInfo_Tab == 0x0003)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iESBInfo_Tab == 0x0002)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_YELLOW);
	else if (m_iESBInfo_Tab == 0x0001)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_RED);
	else if (m_iESBInfo_Tab == 0x0000)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_GREEN);

	//set the tab on the greenbar page
	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);
}

void 
CSafePropertySheet::OnPSBInfoRecord(char* szRecord)			
{
	//will need a few work areas
	char szTempX[256];
	char szTemp1[64];
	char szTemp2[64];
	char szTemp3[64];
	char szTemp4[64];

	//initialize to zero so as to not use if
	//more then four boards are being used
	int iColumn = 0;

	CPSBInfo	*pPSBInfoPage = m_pParent->m_pPSBInfo;

	//do we have a match
	bool bMatch;

	//get a pointer to a GeneralConfigTextRecord
	PSBInfoTextRecord* pszRecord = (PSBInfoTextRecord*)szRecord;

 	//get julian time and put it in the binary record
	strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;
	//set the date & time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//get serial number for column placement
	strncpy(szTempX,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
	szTempX[sizeof(pszRecord->chSerialNumber)]=NULL;

	//get what is in which columns now
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL1,szTemp1,sizeof(szTemp1));
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL2,szTemp2,sizeof(szTemp2));
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL3,szTemp3,sizeof(szTemp3));
	pPSBInfoPage->GetDlgItemText(IDC_PSBINFO_SERIAL4,szTemp4,sizeof(szTemp4));

	//first see current matches any of the columns
	if      (strncmp((char*)&pszRecord->chSerialNumber,szTemp1,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 1;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp2,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 2;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp3,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 3;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp4,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 4;
	//if not then find the first unused column
	else if (strlen(szTemp1)==0) iColumn = 5;
	else if (strlen(szTemp2)==0) iColumn = 6;
	else if (strlen(szTemp3)==0) iColumn = 7;
	else if (strlen(szTemp4)==0) iColumn = 8;

	switch (iColumn) {
	case 5:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_SERIALNUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL1,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL1,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_SERIALNUM1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 1:
		//set date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATEI1,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMEI1,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_PSBIR_TIME1",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_FIRMWAREVER1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_FIRMWAREV1,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_FIRMWAREV1,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_FIRMWAREVER1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_FIRMWARECKSUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_FIRMWAREC1,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_FIRMWAREC1,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_FIRMWARECKSUM1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		break;
	case 6:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_SERIALNUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL2,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL2,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_SERIALNUM2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 2:
		//set date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATEI2,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMEI2,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_PSBIR_TIME2",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_FIRMWAREVER2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_FIRMWAREV2,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_FIRMWAREV2,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_FIRMWAREVER2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_FIRMWARECKSUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_FIRMWAREC2,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_FIRMWAREC2,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_FIRMWARECKSUM2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	case 7:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_SERIALNUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL3,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL3,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_SERIALNUM3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 3:
		//set date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATEI3,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMEI3,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_PSBIR_TIME3",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_FIRMWAREVER3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_FIRMWAREV3,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_FIRMWAREV3,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_FIRMWAREVER3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_FIRMWARECKSUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_FIRMWAREC3,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_FIRMWAREC3,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_FIRMWARECKSUM3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	case 8:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_SERIALNUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_SERIAL4,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_SERIAL4,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_SERIALNUM4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 4:
		//set date and time
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATADATEI4,cTime.Date());
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_DATATIMEI4,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_PSBIR_TIME4",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_FIRMWAREVER4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_FIRMWAREV4,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_FIRMWAREV4,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_FIRMWAREVER4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PSBIR_FIRMWARECKSUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pPSBInfoPage->SetDlgItemText(IDC_PSBINFO_FIRMWAREC4,szTemp1);
		if (!bMatch) pPSBInfoPage->SetColor(IDC_PSBINFO_FIRMWAREC4,COLOR_YELLOW);
		CurrentSetParam("SET_PSBIR_FIRMWARECKSUM4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iPSBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	}

	//set the tabs on the PSB Info page
	if (m_iPSBInfo_Tab == 0x0003)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iPSBInfo_Tab == 0x0002)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_YELLOW);
	else if (m_iPSBInfo_Tab == 0x0001)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_RED);
	else if (m_iPSBInfo_Tab == 0x0000)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_GREEN);

	//set the tab on the greenbar page
	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);
}

void 
CSafePropertySheet::OnBMBInfoRecord(char* szRecord)			
{
	//will need a few work areas
	char szTempX[256];
	char szTemp1[64];
	char szTemp2[64];
	char szTemp3[64];
	char szTemp4[64];

	//initialize to zero so as to not use if
	//more then four boards are being used
	int iColumn = 0;

	CBMBInfo	*pBMBInfoPage = m_pParent->m_pBMBInfo;

	//do we have a match
	bool bMatch;

	//get a pointer to a GeneralConfigTextRecord
	BMBInfoTextRecord* pszRecord = (BMBInfoTextRecord*)szRecord;

 	//get julian time and put it in the binary record
	strncpy(szTemp1,(char*)pszRecord->cdJulianSeconds,sizeof(pszRecord->cdJulianSeconds));
	szTemp1[sizeof(pszRecord->cdJulianSeconds)]=NULL;
	//set the date & time
	CJulianTime cTime;
	cTime.Init(szTemp1);

	//get serial number for column placement
	strncpy(szTempX,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
	szTempX[sizeof(pszRecord->chSerialNumber)]=NULL;

	//get what is in which columns now
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL1,szTemp1,sizeof(szTemp1));
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL2,szTemp2,sizeof(szTemp2));
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL3,szTemp3,sizeof(szTemp3));
	pBMBInfoPage->GetDlgItemText(IDC_BMBINFO_SERIAL4,szTemp4,sizeof(szTemp4));

	//first see current matches any of the columns
	if      (strncmp((char*)&pszRecord->chSerialNumber,szTemp1,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 1;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp2,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 2;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp3,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 3;
	else if (strncmp((char*)&pszRecord->chSerialNumber,szTemp4,sizeof(pszRecord->chSerialNumber)) == 0)
		iColumn = 4;
	//if not then find the first unused column
	else if (strlen(szTemp1)==0) iColumn = 5;
	else if (strlen(szTemp2)==0) iColumn = 6;
	else if (strlen(szTemp3)==0) iColumn = 7;
	else if (strlen(szTemp4)==0) iColumn = 8;

	switch (iColumn) {
	case 5:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_SERIALNUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL1,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_SERIALNUM1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 1:
		//set date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATEI1,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMEI1,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_BMBIR_TIME1",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_FIRMWAREVER1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_FIRMWAREV1,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_FIRMWAREV1,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_FIRMWAREVER1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_FIRMWARECKSUM1","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_FIRMWAREC1,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_FIRMWAREC1,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_FIRMWARECKSUM1",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		break;

	case 6:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_SERIALNUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL2,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_SERIALNUM2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 2:
		//set date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATEI2,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMEI2,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_BMBIR_TIME2",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_FIRMWAREVER2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_FIRMWAREV2,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_FIRMWAREV2,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_FIRMWAREVER2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_FIRMWARECKSUM2","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_FIRMWAREC2,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_FIRMWAREC2,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_FIRMWARECKSUM2",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 7:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_SERIALNUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL3,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_SERIALNUM3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 3:
		//set date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATEI3,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMEI3,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_BMBIR_TIME3",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_FIRMWAREVER3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_FIRMWAREV3,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_FIRMWAREV3,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_FIRMWAREVER3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_FIRMWARECKSUM3","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_FIRMWAREC3,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_FIRMWAREC3,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_FIRMWARECKSUM3",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;

	case 8:
		//set set serial number
		strncpy(szTemp1,(char*)pszRecord->chSerialNumber,sizeof(pszRecord->chSerialNumber));
		szTemp1[sizeof(pszRecord->chSerialNumber)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_SERIALNUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_SERIAL4,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_SERIALNUM4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		//fall through to process the rest of the items
	case 4:
		//set date and time
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATADATEI4,cTime.Date());
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_DATATIMEI4,cTime.Time());
		sprintf(szTemp1,"%s %s",cTime.Date(),cTime.Time());
		CurrentSetParam("SET_BMBIR_TIME4",szTemp1,false,true);

		//set firmware version
		strncpy(szTemp1,(char*)pszRecord->c3dFirmwareVersion,sizeof(pszRecord->c3dFirmwareVersion));
		szTemp1[sizeof(pszRecord->c3dFirmwareVersion)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_FIRMWAREVER4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_FIRMWAREV4,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_FIRMWAREV4,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_FIRMWAREVER4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}

		//set firmware checksum
		strncpy(szTemp1,(char*)pszRecord->cdFirmwareChkSum,sizeof(pszRecord->cdFirmwareChkSum));
		szTemp1[sizeof(pszRecord->cdFirmwareChkSum)]=NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BMBIR_FIRMWARECKSUM4","",szTempX,sizeof(szTempX),m_szIniFile);
		bMatch = (strcmp(szTemp1,szTempX) == 0)?true:false;
		pBMBInfoPage->SetDlgItemText(IDC_BMBINFO_FIRMWAREC4,szTemp1);
		if (!bMatch) pBMBInfoPage->SetColor(IDC_BMBINFO_FIRMWAREC4,COLOR_YELLOW);
		CurrentSetParam("SET_BMBIR_FIRMWARECKSUM4",szTemp1,bMatch,false);
		if (!bMatch)
		{
			m_iBMBInfo_Tab |= 0x0002;
			m_iInst4Setup_Tab |= 0x0001;
		}
		break;
	}

	//set the tabs on the BMB Info page
	if (m_iBMBInfo_Tab == 0x0003)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iBMBInfo_Tab == 0x0002)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOW);
	else if (m_iBMBInfo_Tab == 0x0001)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_RED);
	else if (m_iBMBInfo_Tab == 0x0000)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_GREEN);

	//set the tab on the greenbar page
	m_pParent->m_iInst4Setup_Tab = m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);
}

void 
CSafePropertySheet::OnInformationRecord(char* szRecord)		
{
}

void 
CSafePropertySheet::OnTriggerConfigureRecord(char* szRecord)
{
}



//record 45 (post monitor version 4 only)
void
CSafePropertySheet::TriggerConfigRecord(char *Record)
{
//	AdjustTabsTo(POST4);
	
//	int CheckSum;
//
//	if (LengthOk(Record,TRIGGER_CONFIG_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{

//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
		CUser4Prog* user4page    = m_pParent->m_pUser4ProgStatus;
		CSetupData* setdatapage = m_pParent->m_pSetupData;
//		CModParam* modparpage  = m_pParent->m_pModParams;
		bool bChange = false;
		bool bDoYellow = false;
	
		CString cFull(Record);
		CString cPart;
		CString cPart2;
		CString cSet;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);
		cPart = cjTime.Date();
		cPart += " ";
		cPart += cjTime.Time();
		CurrentSetParam("SET4_TCR_TIME",cPart,true);
		user4page->SetDlgItemText(IDC_USER4PROG_TIME,cPart);

		cPart = cFull.Mid(12,1);
		if (!CurrentSetParam("SET4_TRIGGER4_LOGIC",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
		}
		if (cPart == "1")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC1,"OR");
		else if (cPart == "0")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC1,"AND");
		else
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC1,cPart);
		
		cPart = cFull.Mid(13,16);
		if (!CurrentSetParam("SET4_TRIGGER4_CHTYPE",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			cSet = setdatapage->GetMatchString();
			while (cSet.GetLength() < 16) cSet += ' ';
		}
		else
			bDoYellow = false;

		unsigned short usValue;
		cPart2 = cPart[0];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[1];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE1,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE1,"");

		cPart2 = cPart[2];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[3];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE2,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE2,"");

		cPart2 = cPart[4];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[5];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE3,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE3,"");

		cPart2 = cPart[6];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[7];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE4,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE4,"");

		cPart2 = cPart[8];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[9];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE5,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE5,"");

		cPart2 = cPart[10];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[11];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE6,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE6,"");

		cPart2 = cPart[12];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[13];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE7,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE7,"");

		cPart2 = cPart[14];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[15];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE8,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE8,"");
		
		cPart = cFull.Mid(29,1);
		if (!CurrentSetParam("SET4_TRIGGER5_LOGIC",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
		}
		else
			bDoYellow = false;
		if (cPart == "1")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC2,"OR");
		else if (cPart == "0")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC2,"AND");
		else
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC2,cPart);
		
		cPart = cFull.Mid(30,16);
		if (!CurrentSetParam("SET4_TRIGGER5_CHTYPE",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			cSet = setdatapage->GetMatchString();
			while (cSet.GetLength() < 16) cSet += ' ';
		}
		else
			bDoYellow = false;

		cPart2 = cPart[0];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[1];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE1,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE1,"");

		cPart2 = cPart[2];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[3];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE2,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE2,"");

		cPart2 = cPart[4];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[5];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE3,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE3,"");

		cPart2 = cPart[6];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[7];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE4,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE4,"");

		cPart2 = cPart[8];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[9];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE5,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE5,"");

		cPart2 = cPart[10];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[11];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE6,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE6,"");

		cPart2 = cPart[12];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[13];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE7,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE7,"");

		cPart2 = cPart[14];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[15];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE8,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE8,"");
		
		cPart = cFull.Mid(46,1);
		if (!CurrentSetParam("SET4_TRIGGER6_LOGIC",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
		}
		else
			bDoYellow = false;
		if (cPart == "1")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC3,"OR");
		else if (cPart == "0")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC3,"AND");
		else
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC3,cPart);
		
		cPart = cFull.Mid(47,16);
		if (!CurrentSetParam("SET4_TRIGGER6_CHTYPE",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			cSet = setdatapage->GetMatchString();
			while (cSet.GetLength() < 16) cSet += ' ';
		}
		else
			bDoYellow = false;

		cPart2 = cPart[0];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[1];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE1,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE1,"");

		cPart2 = cPart[2];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[3];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE2,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE2,"");

		cPart2 = cPart[4];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[5];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE3,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE3,"");

		cPart2 = cPart[6];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[7];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE4,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE4,"");

		cPart2 = cPart[8];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[9];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE5,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE5,"");

		cPart2 = cPart[10];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[11];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE6,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE6,"");
		
		cPart2 = cPart[12];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[13];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE7,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE7,"");

		cPart2 = cPart[14];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[15];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE8,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE8,"");
		
		cPart = cFull.Mid(63,1);
		if (!CurrentSetParam("SET4_TRIGGER7_LOGIC",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
		}
		else
			bDoYellow = false;
		if (cPart == "1")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC4,"OR");
		else if (cPart == "0")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC4,"AND");
		else
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC4,cPart);
		
		cPart = cFull.Mid(64,16);
		if (!CurrentSetParam("SET4_TRIGGER7_CHTYPE",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			cSet = setdatapage->GetMatchString();
			while (cSet.GetLength() < 16) cSet += ' ';
		}
		else
			bDoYellow = false;

		cPart2 = cPart[0];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[1];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE1,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE1,"");

		cPart2 = cPart[2];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[3];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE2,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE2,"");

		cPart2 = cPart[4];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[5];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE3,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE3,"");

		cPart2 = cPart[6];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[7];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE4,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE4,"");

		cPart2 = cPart[8];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[9];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE5,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE5,"");

		cPart2 = cPart[10];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[11];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE6,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE6,"");

		cPart2 = cPart[12];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[13];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE7,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE7,"");

		cPart2 = cPart[14];
		sscanf(cPart2,"%02x",&usValue);
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[15];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE8,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE8,"");

		if (bChange)
		{
			m_iUser4Prog_Tab = m_pParent->m_iUser4Prog_Tab;
			if (m_iUser4Prog_Tab == 0x0003)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOWRED);
			else if (m_iUser4Prog_Tab == 0x0002)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOW);
			else if (m_iUser4Prog_Tab == 0x0001)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_RED);
			else if (m_iUser4Prog_Tab == 0x0000)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_GREEN);
		}
	}
}

int 
CSafePropertySheet::Checksum(char* Record)
{
    unsigned short nbytes;	/* # bytes to include in checksum calculation */
    unsigned short sum;		/* sum of all bytes in checksum */
	unsigned short rec_sum;
    /* checksum is a one byte sum of all bytes up to the checksum byte */
	sum = 0;
    nbytes = (unsigned short)(strlen(Record)-4);//don't count lf/cr nor checksum
	//adjust for authentication in acquire record
	if (strncmp(ACQUIRE_RECORD_CHAR,Record,2)==0) 
	{
		//and cause a checksum error if wrong length received
		nbytes -= 2;//adjust for acquire record's auth byte
	}

	//don't bother with checking checksum if length is wrong
	for (int i=0; i<nbytes; i++)
		sum = (unsigned short)(sum + Record[i]);
	sum &= 0x00ff;	/* keep only relevant part of checksum */
	sscanf(&Record[nbytes],"%2hx",&rec_sum);
	if (rec_sum == sum)
		/* calculated checksum matches checksum in record */
		return CHECKSUM_GOOD;

//	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
///	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	CDataIStat* dataipage = m_pParent->m_pDataIStatus;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("CheckSum: %d/%H:%M:%S ");
	DateTime += Record;

///	if (GetPageIndex(general4page) >= 0)
///	{
///		general4page->SetDlgItemText(IDC_GENERAL4_CHECKSUM,(char*)LPCTSTR(DateTime));
///		m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
///		if (m_iGeneral4_Tab == 0x0003)
///			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
///		else if (m_iGeneral4_Tab == 0x0002)
///			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
///		else if (m_iGeneral4_Tab == 0x0001)
///			SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
///		else if (m_iGeneral4_Tab == 0x0000)
///			SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
///	}

	if (GetPageIndex(dataipage) >= 0)
	{
		dataipage->SetDlgItemText(IDC_DATAI_CHECKSUM,(char*)LPCTSTR(DateTime));
		m_iDataI_Tab = m_pParent->m_iDataI_Tab;
		if (m_iDataI_Tab == 0x0003)
			SetTabColor(IDC_DATAI_TAB,COLOR_YELLOWRED);
		else if (m_iDataI_Tab == 0x0002)
			SetTabColor(IDC_DATAI_TAB,COLOR_YELLOW);
		else if (m_iDataI_Tab == 0x0001)
			SetTabColor(IDC_DATAI_TAB,COLOR_RED);
		else if (m_iDataI_Tab == 0x0000)
			SetTabColor(IDC_DATAI_TAB,COLOR_GREEN);
	}
    return CHECKSUM_BAD;	       /* checksums do not match */
}

bool 
CSafePropertySheet::LengthOk(char* Record, int length)
{
	//Check the length of the record	
	if (strlen(Record) == (unsigned)length)
		return true;

    return false;
}

int CSafePropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pCloseButton = new CColorButton();
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

void CSafePropertySheet::OnCloseButton()
{
	KillTimer(1);
	::PostMessage(m_hWnd,WM_SYSCOMMAND,SC_CLOSE,NULL);
}

void CSafePropertySheet::UpdateTabs()
{
///	m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
///	if (m_iGeneral4_Tab == 0x0003)
///		SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
///	else if (m_iGeneral4_Tab == 0x0002)
///		SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
///	else if (m_iGeneral4_Tab == 0x0001)
///		SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
///	else if (m_iGeneral4_Tab == 0x0000)
///		SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);

	m_iDataI_Tab = m_pParent->m_iDataI_Tab;
	if (m_iDataI_Tab == 0x0003)
		SetTabColor(IDC_DATAI_TAB,COLOR_YELLOWRED);
	else if (m_iDataI_Tab == 0x0002)
		SetTabColor(IDC_DATAI_TAB,COLOR_YELLOW);
	else if (m_iDataI_Tab == 0x0001)
		SetTabColor(IDC_DATAI_TAB,COLOR_RED);
	else if (m_iDataI_Tab == 0x0000)
		SetTabColor(IDC_DATAI_TAB,COLOR_GREEN);

	m_iData_Tab = m_pParent->m_iData_Tab;
	if (m_iData_Tab == 0x0003)
		SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
	else if (m_iData_Tab == 0x0002)
		SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
	else if (m_iData_Tab == 0x0001)
		SetTabColor(IDC_DATA_TAB,COLOR_RED);
	else if (m_iData_Tab == 0x0000)
		SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

	m_iUser4Prog_Tab = m_pParent->m_iUser4Prog_Tab;
	if (m_iUser4Prog_Tab == 0x0003)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOWRED);
	else if (m_iUser4Prog_Tab == 0x0002)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOW);
	else if (m_iUser4Prog_Tab == 0x0001)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_RED);
	else if (m_iUser4Prog_Tab == 0x0000)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_GREEN);

	m_iInst4Setup_Tab = m_pParent->m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);

	m_iBMBInfo_Tab = m_pParent->m_iBMBInfo_Tab;
	if (m_iBMBInfo_Tab == 0x0003)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iBMBInfo_Tab == 0x0002)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_YELLOW);
	else if (m_iBMBInfo_Tab == 0x0001)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_RED);
	else if (m_iBMBInfo_Tab == 0x0000)
		SetTabColor(IDC_BMBINFO_TAB,COLOR_GREEN);

	m_iuGInfo_Tab = m_pParent->m_iUGInfo_Tab;
	if (m_iuGInfo_Tab == 0x0003)
		SetTabColor(IDC_UGINFO_TAB,COLOR_YELLOWRED);
	else if (m_iuGInfo_Tab == 0x0002)
		SetTabColor(IDC_UGINFO_TAB,COLOR_YELLOW);
	else if (m_iuGInfo_Tab == 0x0001)
		SetTabColor(IDC_UGINFO_TAB,COLOR_RED);
	else if (m_iuGInfo_Tab == 0x0000)
		SetTabColor(IDC_UGINFO_TAB,COLOR_GREEN);

	m_iPSBInfo_Tab = m_pParent->m_iPSBInfo_Tab;
	if (m_iPSBInfo_Tab == 0x0003)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iPSBInfo_Tab == 0x0002)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_YELLOW);
	else if (m_iPSBInfo_Tab == 0x0001)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_RED);
	else if (m_iPSBInfo_Tab == 0x0000)
		SetTabColor(IDC_PSBINFO_TAB,COLOR_GREEN);

	m_iESBInfo_Tab = m_pParent->m_iESBInfo_Tab;
	if (m_iESBInfo_Tab == 0x0003)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_YELLOWRED);
	else if (m_iESBInfo_Tab == 0x0002)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_YELLOW);
	else if (m_iESBInfo_Tab == 0x0001)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_RED);
	else if (m_iESBInfo_Tab == 0x0000)
		SetTabColor(IDC_ESBINFO_TAB,COLOR_GREEN);
}

void CSafePropertySheet::SetTabColor(int Tab,COLORREF Color)
{

	CSummary*  summarypage =	m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage =	m_pParent->m_pGeneralStatPage;
///	CGenl4Stat* general4page =	m_pParent->m_pGeneral4StatPage;
	CDataStat* datapage    =	m_pParent->m_pDataStatus;
	CDataIStat* dataIpage    =	m_pParent->m_pDataIStatus;
	CData2Stat* data2page    =	m_pParent->m_pData2Status;
//	CUserProg* userpage    =	m_pParent->m_pUserProgStatus;
	CUser4Prog* user4page    =	m_pParent->m_pUser4ProgStatus;
	CModParam* modparpage  =	m_pParent->m_pModParams;
//	CSetupData* setdatapage =	m_pParent->m_pSetupData;
	CSetupData* setdatapage =	m_pParent->m_pSetupData;
	CUGINFO* uginfopage   =		m_pParent->m_pUGInfo;
	CBMBInfo* bmbinfopage =		m_pParent->m_pBMBInfo;
	CPSBInfo* psbinfopage =		m_pParent->m_pPSBInfo;
	CESBInfo* esbinfopage =		m_pParent->m_pESBInfo;
	CLegend* legend =			m_pParent->m_pLegend;

	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	switch(Color) {
	case COLOR_RED:
		pTabCtrlItem.iImage = 0;
		break;
	case COLOR_GREEN:
		pTabCtrlItem.iImage = 1;
		break;
	case COLOR_BLUE:
		pTabCtrlItem.iImage = 2;
		break;
	case COLOR_YELLOW:
		pTabCtrlItem.iImage = 3;
		break;
	case COLOR_CYAN:
		pTabCtrlItem.iImage = 4;
		break;
	case COLOR_MAGENTA:
		pTabCtrlItem.iImage = 5;
		break;
	case COLOR_WHITE:
		pTabCtrlItem.iImage = 6;
		break;
	case COLOR_BLACK:
		pTabCtrlItem.iImage = 7;
		break;
	case COLOR_YELLOWRED:
		pTabCtrlItem.iImage = 10;
		break;
	default:
		pTabCtrlItem.iImage = 6;
	};

	int iTab = -1;
	switch (Tab) {
//	case IDC_GENERAL_TAB:
//		iTab = GetPageIndex(generalpage);
//		break;
	case IDC_DATA_TAB:
		iTab = GetPageIndex(datapage);
		break;
	case IDC_DATAI_TAB:
		iTab = GetPageIndex(dataIpage);
		break;
	case IDC_DATA2_TAB:
		iTab = GetPageIndex(data2page);
		break;
//	case IDC_USERPROG_TAB:
//		iTab = GetPageIndex(userpage);
//		break;
	case IDC_USER4PROG_TAB:
		iTab = GetPageIndex(user4page);
		break;
//	case IDC_INSTSETUP_TAB:
//		iTab = GetPageIndex(setdatapage);
//		break;
	case IDC_INST4SETUP_TAB:
		iTab = GetPageIndex(setdatapage);
		break;
///	case IDC_GENERAL4_TAB:
///		iTab = GetPageIndex(general4page);
///		break;
	case IDC_UGINFO_TAB:
		iTab = GetPageIndex(uginfopage);
		break;
	case IDC_BMBINFO_TAB:
		iTab = GetPageIndex(bmbinfopage);
		break;
	case IDC_PSBINFO_TAB:
		iTab = GetPageIndex(psbinfopage);
		break;
	case IDC_ESBINFO_TAB:
		iTab = GetPageIndex(esbinfopage);
		break;
	};

	if (iTab >= 0)
	{
		GetTabControl()->SetItem(iTab,&pTabCtrlItem);
		GetTabControl()->Invalidate();
	}
}

void CSafePropertySheet::OnAckButton()
{
	m_dToClose = 0;
	//acknowledge the four button flags
	//acknowledge the checksum error

	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
///	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	CDataStat* datapage    = m_pParent->m_pDataStatus;
	CDataIStat* dataIpage    = m_pParent->m_pDataIStatus;
	CData2Stat* data2page    = m_pParent->m_pData2Status;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
	CUser4Prog* user4page    = m_pParent->m_pUser4ProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;
//	CSetupData*  setdatapage = m_pParent->m_pSetupData;
	CSetupData*  setdatapage = m_pParent->m_pSetupData;
	CUGINFO* uginfopage =		m_pParent->m_pUGInfo;
	CBMBInfo* bmbinfopage =		m_pParent->m_pBMBInfo;
	CPSBInfo* psbinfopage =		m_pParent->m_pPSBInfo;
	CESBInfo* esbinfopage =		m_pParent->m_pESBInfo;

	
	summarypage->ResetColors();
///	general4page->ResetColors();
	datapage->ResetColors();
	user4page->ResetColors();
	uginfopage->ResetColors();
	bmbinfopage->ResetColors();
	psbinfopage->ResetColors();
	esbinfopage->ResetColors();
	setdatapage->ResetColors();
			
///	SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);	//UGRAND ver 4 Setup
	SetTabColor(IDC_DATA_TAB,COLOR_GREEN);      //Data Status
	SetTabColor(IDC_DATAI_TAB,COLOR_GREEN);      //Data Status
	SetTabColor(IDC_DATA2_TAB,COLOR_GREEN);      //Data Status
	SetTabColor(IDC_USER4PROG_TAB,COLOR_GREEN);  //Monitor Parameters
	SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);//inst 4 settings
	SetTabColor(IDC_UGINFO_TAB,COLOR_GREEN);
	SetTabColor(IDC_BMBINFO_TAB,COLOR_GREEN);//inst 4 settings
	SetTabColor(IDC_PSBINFO_TAB,COLOR_GREEN);//inst 4 settings
	SetTabColor(IDC_ESBINFO_TAB,COLOR_GREEN);//inst 4 settings

	m_iData_Tab = 0x0000;
	m_iDataI_Tab = 0x0000;
	m_iData2_Tab = 0x0000;
///	m_iGeneral4_Tab = 0x0000;
	m_iUser4Prog_Tab = 0x0000;
	m_iInst4Setup_Tab = 0x0000;
	m_iBMBInfo_Tab = 0x0000;
	m_iPSBInfo_Tab = 0x0000;
	m_iuGInfo_Tab = 0x0000;
	m_iESBInfo_Tab = 0x0000;

	m_pParent->AckButton();

	Invalidate();
}

void CSafePropertySheet::OnPauseButton()
{
	m_dToClose = 0;
	CModParam* modparpage = m_pParent->m_pModParams;
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

void CSafePropertySheet::OnPrintButton()
{
/*
	char *sMsg;
	int nBytesRead;

#ifdef _DEBUG
	//test generation of General Configuration Record message
	//411661096173    1141234.095FC5A904252015 3    5 1196462A25030 72040 5 304.10474B4D
	sMsg = (char*)malloc(85);
	nBytesRead = 84;
	if (sMsg)
	{
		strcpy(sMsg,"41");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"11111");
		strcat(sMsg,"FF");
		strcat(sMsg,"2222.2");
		strcat(sMsg,"00");
		strcat(sMsg,"FF");
		strcat(sMsg,"EE");
		strcat(sMsg,"33");
		strcat(sMsg,"D");
		strcat(sMsg,"44");
		strcat(sMsg,"55");
		strcat(sMsg,"66");
		strcat(sMsg,"77");
		strcat(sMsg,"88888");
		strcat(sMsg,"9999999");
		strcat(sMsg,"CC");
		strcat(sMsg,"B");
		strcat(sMsg,"1234");
		strcat(sMsg,"HHMMSS");
		strcat(sMsg,"55");
		strcat(sMsg,"66");
		strcat(sMsg,"77.88");
		strcat(sMsg,"ABCD");
		strcat(sMsg,"dc");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test generation of Instrument status record
	//42166109617312.4 4.911.9-12.001342.0260.0 80.1240.2D0
	sMsg = (char*)malloc(56);
	nBytesRead = 55;
	if (sMsg)
	{
		strcpy(sMsg,"42");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"12.3");
		strcat(sMsg,"45.6");
		strcat(sMsg,"78.9");
		strcat(sMsg,"-12.3");
		strcat(sMsg,"1");
		strcat(sMsg,"1234.5");
		strcat(sMsg,"123.4");
		strcat(sMsg,"456.7");
		strcat(sMsg,"123.4");
		strcat(sMsg,"1d");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test generation of instrument information record
	//4316610961740000089131A921FDDF01031FFFF041FF04FF01D6
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"43");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"aaaaaaaaaaaa");
		strcat(sMsg,"11");
		strcat(sMsg,"22");
		strcat(sMsg,"33");
		strcat(sMsg,"44");
		strcat(sMsg,"55");
		strcat(sMsg,"66");
		strcat(sMsg,"77");
		strcat(sMsg,"88");
		strcat(sMsg,"99");
		strcat(sMsg,"aa");
		strcat(sMsg,"bb");
		strcat(sMsg,"cc");
		strcat(sMsg,"dd");
		strcat(sMsg,"ee");
		strcat(sMsg,"98");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test generation of Dual Current Mode Configuration Record
	//4416610961741 45128111 5111 8255.01 0.002D4
	sMsg = (char*)malloc(46);
	nBytesRead = 45;
	if (sMsg)
	{
		strcpy(sMsg,"44");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"A");
		strcat(sMsg,"123");
		strcat(sMsg,"123");
		strcat(sMsg,"4");
		strcat(sMsg,"56");
		strcat(sMsg,"78");
		strcat(sMsg,"9");
		strcat(sMsg,"12");
		strcat(sMsg,"34");
		strcat(sMsg,"567.8");
		strcat(sMsg,"9");
		strcat(sMsg,"1.2345");
		strcat(sMsg,"61");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test generation of trigger configuration record
	//451661096174187000000000000001818200000000000008090A0000000000018092A100000000009E
	sMsg = (char*)malloc(85);
	nBytesRead = 84;
	if (sMsg)
	{
		strcpy(sMsg,"45");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"A");
		strcat(sMsg,"aabbccddeeffaabb");
		strcat(sMsg,"B");
		strcat(sMsg,"1122334455667788");
		strcat(sMsg,"C");
		strcat(sMsg,"AABBCCDDEEFFGGHH");
		strcat(sMsg,"D");
		strcat(sMsg,"9988776655443322");
		strcat(sMsg,"98");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test chan 0 configuration record
	//46166109617401   1234.5122    100.03A 0.1F 0.0020120A1
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"46");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"1");
		strcat(sMsg,"B");
		strcat(sMsg,"1234567.8");
		strcat(sMsg,"CC");
		strcat(sMsg,"D");
		strcat(sMsg,"9876543.2");
		strcat(sMsg,"EE");
		strcat(sMsg,"12.3");
		strcat(sMsg,"F");
		strcat(sMsg,"65.4321");
		strcat(sMsg,"321");
		strcat(sMsg,"ed");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test chan 1 configuration record
	//47166109617412    100.03A1    555.0CD 1.53 0.2000 809E
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"47");
		strcat(sMsg,"7234567890");
		strcat(sMsg,"7");
		strcat(sMsg,"7");
		strcat(sMsg,"7234567.8");
		strcat(sMsg,"7C");
		strcat(sMsg,"7");
		strcat(sMsg,"7876543.2");
		strcat(sMsg,"7E");
		strcat(sMsg,"72.3");
		strcat(sMsg,"7");
		strcat(sMsg,"75.4321");
		strcat(sMsg,"721");
		strcat(sMsg,"c8");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
	//test chan 2 configuration record
	//48166109617412    100.03A2    100.03A 1.53 0.2000 807F
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"48");
		strcat(sMsg,"8234567890");
		strcat(sMsg,"8");
		strcat(sMsg,"8");
		strcat(sMsg,"8234567.8");
		strcat(sMsg,"8C");
		strcat(sMsg,"8");
		strcat(sMsg,"8876543.2");
		strcat(sMsg,"8E");
		strcat(sMsg,"82.3");
		strcat(sMsg,"8");
		strcat(sMsg,"85.4321");
		strcat(sMsg,"821");
		strcat(sMsg,"d5");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
	//test chan 3 configuration record
	//49166109617403   5432.1342    100.03A 9.0D 0.0200125B5
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"49");
		strcat(sMsg,"9999999999");
		strcat(sMsg,"9");
		strcat(sMsg,"9");
		strcat(sMsg,"9999999.9");
		strcat(sMsg,"99");
		strcat(sMsg,"9");
		strcat(sMsg,"9999999.9");
		strcat(sMsg,"99");
		strcat(sMsg,"99.9");
		strcat(sMsg,"9");
		strcat(sMsg,"99.9999");
		strcat(sMsg,"999");
		strcat(sMsg,"63");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
	//test chan 4 configuration record
	//4A166109617412    100.03A3    105.61175.06 0.0200 809B
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"4a");
		strcat(sMsg,"aaaaaaaaaa");
		strcat(sMsg,"a");
		strcat(sMsg,"a");
		strcat(sMsg,"aaaaaaa.a");
		strcat(sMsg,"aa");
		strcat(sMsg,"a");
		strcat(sMsg,"aaaaaaa.a");
		strcat(sMsg,"aa");
		strcat(sMsg,"aa.a");
		strcat(sMsg,"a");
		strcat(sMsg,"aa.aaaa");
		strcat(sMsg,"aaa");
		strcat(sMsg,"bb");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
	//test chan 5 configuration record
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"4b");
		strcat(sMsg,"bbbbbbbbbb");
		strcat(sMsg,"b");
		strcat(sMsg,"b");
		strcat(sMsg,"bbbbbbb.b");
		strcat(sMsg,"bb");
		strcat(sMsg,"b");
		strcat(sMsg,"bbbbbbb.b");
		strcat(sMsg,"bb");
		strcat(sMsg,"bb.b");
		strcat(sMsg,"b");
		strcat(sMsg,"bb.bbbb");
		strcat(sMsg,"bbb");
		strcat(sMsg,"ea");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test battery record
	sMsg = (char*)malloc(21);
	nBytesRead = 20;
	if (sMsg)
	{
		strcpy(sMsg,"18");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"98.7");
		strcat(sMsg,"4c");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
#endif
*/
	FILE* outfile;
	int iAt;
	CString cCaption(m_pParent->m_pName);
	while ((iAt = cCaption.FindOneOf(INVALIDCHARS)) >= 0) 
		cCaption.SetAt(iAt,'_');
	
	m_dToClose = 0;

	CFileDialog FileDialog(FALSE,"txt",cCaption,
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

void CSafePropertySheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 	CPropertySheet::OnShowWindow(bShow, nStatus);

	m_dToClose = 0;
	if (bShow && nStatus==0) 
	{
		SetActivePage(0);
		m_dToClose = 0;
		SetTimer(1,1000,NULL);
		CModParam* modparpage = m_pParent->m_pModParams;
		if (m_pParent->IsPaused())
			modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(true);
		else
			modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(false);
	}
	else
		KillTimer(1);
}

void CSafePropertySheet::OnExtendClose(WPARAM,LPARAM)
{
	m_dToClose = 0;
}

void CSafePropertySheet::OnToCloseButton()
{
	char temp[32];
	m_dToClose -= BUMP_EXTEND;  //5 more minutes

    int seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;

	if (seconds_to_go > MAX_EXTEND) //120 minutes
	{
		m_dToClose   = (m_pParent->m_iDlgCloseMilliseconds/1000) - MAX_EXTEND;
		seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;
	}

	int minutes = seconds_to_go / 60;
	int seconds = seconds_to_go % 60;

	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	m_pToClose->SetWindowText(temp);
}

void CSafePropertySheet::DisplayWatchWindow()
{
	//later add watch window support but for now do nothing 
}

void CSafePropertySheet::DisplayTodayData()
{
	CDataStat	*pDataStatPage = m_pParent->m_pDataStatus;
	m_bUpdatingFromFile = true;
	if (m_pDataWatchDlg)
	{
		if (m_bDataWatchDlgOpen)
		{
			char cSourceTitles[14][10] = {"Ch 0 CR","Ch 0 SCR","Ch 0 LCR",
	  						  "Ch 1 CR","Ch 1 SCR","Ch 1 LCR",
	  						  "Ch 2 CR","Ch 2 SCR","Ch 2 LCR",
	  						  "Ch 3 CR","Ch 3 SCR","Ch 3 LCR",
							  "CPU Temp","BRD Temp"};

			char cDestinTitles[5][10];
			for (int i = 0; i < 5; i++) 
				strcpy(cDestinTitles[i],"NOT Used") ;
			
			int j = 0;
			int jNext = 0;
			for (i = 0; i < 5; i++)
			{
				j = jNext;
				while (j < 14)
				{
					if (pDataStatPage->m_bCheckVector[j])
					{
						strcpy(cDestinTitles[i],cSourceTitles[j]);
						jNext = j+1;
						j = 14;
					}
					j++;
				}
			}

			m_pDataWatchDlg->SetGraphTitles(
				cDestinTitles[0],
				cDestinTitles[1],
				cDestinTitles[2],
				cDestinTitles[3],
				cDestinTitles[4]);

			m_pDataWatchDlg->Invalidate();
//			m_pDataWatchDlg->ShowWindow(SW_SHOW);
			return;
		}
		else
		{
//			m_pDataWatchDlg->DestroyWindow();
			m_pDataWatchDlg->EndDialog(0);
			delete m_pDataWatchDlg;
			m_pDataWatchDlg = NULL;
		}
	}
	
	m_pDataWatchDlg = new CDataWatchDlg();
	m_pDataWatchDlg->SetDlgTitle("µGRAND Data", this);
	

	char cSourceTitles[14][10] = {"Ch 0 CR","Ch 0 SCR","Ch 0 LCR",
	  	              "Ch 1 CR","Ch 1 SCR","Ch 1 LCR",
	  	              "Ch 2 CR","Ch 2 SCR","Ch 2 LCR",
	  	              "Ch 3 CR","Ch 3 SCR","Ch 3 LCR",
					  "CPU Temp","BRD Temp"};

	char cDestinTitles[5][10];
	for (int i = 0; i < 5; i++) 
		strcpy(cDestinTitles[i],"NOT Used") ;
	
	int j = 0;
	int jNext = 0;
	for (i = 0; i < 5; i++)
	{
		j = jNext;
		while (j < 14)
		{
			if (pDataStatPage->m_bCheckVector[j])
			{
				strcpy(cDestinTitles[i],cSourceTitles[j]);
				jNext = j+1;
				j = 14;
			}
			j++;
		}
	}

	m_pDataWatchDlg->SetGraphTitles(
		cDestinTitles[0],
		cDestinTitles[1],
		cDestinTitles[2],
		cDestinTitles[3],
		cDestinTitles[4]);


	m_pDataWatchDlg->Create(IDD_DATAWATCH_DIALOG,NULL);
	m_bDataWatchDlgOpen = true;
	m_pDataWatchDlg->m_pParentWnd = NULL;

	//build today file name
	//open file and read header
		//read and add data to end of file
		//close file

	COleDateTime cNow = COleDateTime::GetCurrentTime();
	COleDateTime cBase(1952,1,1,0,0,0);
	COleDateTimeSpan cSpan = cNow - cBase;
	CJulianTime Time(cSpan.GetTotalSeconds());

	//build filename
	if (m_pParent->m_bUseShortFilename)
	{
		sprintf(m_szCurrentBIDFileName,"%s\\%s%c%c%c%c%02d.%3s",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((Time.Year()-1990)<10)?
					((Time.Year()-1990<0)?'#':'0'+(Time.Year()-1990)):
					'A'+(Time.Year()-2000),
	////			'0' + (Time->Year() % 10),
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,MGD_SUFFIX);
	}
	else
	{
		CString cResult;
		m_pParent->BuildFileName(cResult,m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,"uG",
			m_pParent->m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,MGD_SUFFIX);
		strcpy(m_szCurrentBIDFileName,cResult);
	}

	//is the file there
	// '0' checks for existance only
	if (access(m_szCurrentBIDFileName,0) != 0)
	{
		//make sure path exists
		m_pParent->MakeMyPath(m_szCurrentBIDFileName);

		//build archive file name
		CString ArchiveName;
		if (m_pParent->m_bUseShortFilename)
		{
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((Time.Year()-1990)<10)?
					((Time.Year()-1990<0)?'#':'0'+(Time.Year()-1990)):
					'A'+(Time.Year()-2000),
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,MGD_SUFFIX);
		}
		else
		{
			CString cTemp;
			cTemp = m_pParent->m_szSaveLoc;
			cTemp += "\\archive\\";
			m_pParent->BuildFileName(ArchiveName,m_pParent->m_csLongNameFormatString,cTemp,"uG",
				m_pParent->m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,MGD_SUFFIX);
		}
		//if the file is in the archive directory
		if (access(ArchiveName,0)==0)
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentBIDFileName,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	}

	CFile cFile;
	CFileException ex;
	if (cFile.Open(m_szCurrentBIDFileName,CFile::modeRead|CFile::shareDenyNone,&ex))
	{
		DWORD dwLength = cFile.GetLength();
		UINT uiEntryLength = sizeof(AcquireRecordX) + sizeof(unsigned short) + sizeof(unsigned short);
		bool bFail = false;
		if (dwLength > (sizeof(AcquireHeader) + 3600*uiEntryLength))
			cFile.Seek(sizeof(AcquireHeader) + 3600*uiEntryLength, CFile::begin);
		else if (dwLength > sizeof(AcquireHeader))
			cFile.Seek(sizeof(AcquireHeader), CFile::begin);
		else 
			bFail = true;

		if (!bFail)
		{
			//while not eof read and display data
			AcquireRecordX sAcquireRecordX;
			unsigned short usCheckSum;
			unsigned short usBeef;

			UINT nBytesRead;
			//get the length if we can
			while ((nBytesRead = cFile.Read(&sAcquireRecordX.bLength,sizeof(sAcquireRecordX.bLength))) > 0)
			{
				cFile.Read(&sAcquireRecordX.bOpcode,sAcquireRecordX.bLength-sizeof(sAcquireRecordX.bLength));
				cFile.Read(&usCheckSum,sizeof(unsigned short));
				cFile.Read(&usBeef,sizeof(unsigned short));

				float fData[12];
				for (int i = 0; i < 12; i++) fData[i] = 0.0;
				int iDest = 0;

				if (pDataStatPage->m_bCheckVector[0])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh0CountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[1])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh0ShortCountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[2])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh0LongCountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[3])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh1CountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[4])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh1ShortCountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[5])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh1LongCountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[6])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh2CountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[7])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh2ShortCountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[8])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh2LongCountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[9])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh3CountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[10])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh3ShortCountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[11])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCh3LongCountRate;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[12])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fCPUTemp;
					iDest++;
				}
				if (pDataStatPage->m_bCheckVector[13])
				{
					fData[iDest] = sAcquireRecordX.RADAcquireRec.fBOARDTemp;
					iDest++;
				}

				m_pDataWatchDlg->SetData(sAcquireRecordX.uiJulianSeconds,
					fData[0],fData[1],fData[2],fData[3],fData[4],
					cFile.GetPosition()==dwLength?true:false);
			}
		}
		cFile.Close();
	}


	m_bUpdatingFromFile = false;
}

void CSafePropertySheet::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		if (m_dToClose >= m_pParent->m_iDlgCloseMilliseconds/1000)
		{
			KillTimer(1);
			m_dToClose = 0;
			PostMessage(WM_SYSCOMMAND,SC_CLOSE,NULL);
		} 
		else
		{
			char temp[32];
			int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
			int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;;
			sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
			m_pToClose->SetWindowText(temp);
			m_dToClose++;
		}
	}
	CPropertySheet::OnTimer(nIDEvent);
}

HBRUSH CSafePropertySheet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertySheet::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CSafePropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//watch for alt-f4 to try to close us -- don't let it
	if (nID == SC_CLOSE)
	{
		DestroyWindow();
		delete (this);
	}
	else
  		CPropertySheet::OnSysCommand(nID, lParam);
}

BOOL CSafePropertySheet::OnInitDialog() 
{
	EnableStackedTabs(FALSE);
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

	m_pCloseButton->Create( "Close",  
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE |
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER ,
		  CRect(Interspace,  Y1,
				Interspace + ButtonWidth,Y2),
		  this,
		  ID_CLOSE_BUTTON);

	m_pPauseButton->Create(COLLECTINGBEGIN,
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE |
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
		  CRect(2*Interspace + ButtonWidth,Y1,
				2*Interspace + 2*ButtonWidth,Y2),
		  this,
		  ID_PAUSE_BUTTON);

	m_pAckFlagsButton->Create("Acknowledge Flags",
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE | 
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
		  CRect(3*Interspace + 2*ButtonWidth,Y1,
				3*Interspace + 3*ButtonWidth,Y2),
		  this,
		  ID_ACKFL_BUTTON);

	m_pPrintButton->Create("Snapshot Status To File",
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE | 
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
		  CRect(4*Interspace + 3*ButtonWidth,Y1,
				4*Interspace + 4*ButtonWidth,Y2),
		  this,
		  ID_PRINT_BUTTON);

	char temp[32];

	int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
	int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;
	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	m_pToClose->Create(temp,
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE | 
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
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
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(4,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(5,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(6,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(7,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(8,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(9,&pTabCtrlItem);

	SetTimer(1,1000,NULL);


/*
//sets up pointer to window so the tab bar can automatically shift to the right
//when a new page is selected

	m_pcWndSpin = NULL;
	TCHAR classname[100];
	CTabCtrl* cTabCtrl = GetTabControl();
	CWnd* pcWndFind = cTabCtrl->GetWindow(GW_HWNDFIRST);
	while(pcWndFind)
	{
		GetClassName(pcWndFind->m_hWnd,classname,100);

		if(lstrcmp(_T("SysTabControl32"),classname) == 0)
			pcWndFind = pcWndFind->GetWindow(GW_CHILD);
		else if(lstrcmp(_T("msctls_updown32"),classname) == 0)
		{
			m_pcWndSpin = pcWndFind;
			break;
		}
		else
			pcWndFind = pcWndFind->GetWindow(GW_HWNDNEXT);
	};
*/
	return bResult;
}

BOOL CSafePropertySheet::DestroyWindow() 
{
	GetTabControl( )->SetImageList(NULL);
	return CPropertySheet::DestroyWindow();
}

void CSafePropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)
		m_dToClose = set;
}

void CSafePropertySheet::ForceDateTime(void)
{
	m_dToClose = 0;
	m_pParent->PostMessage(IDC_RECEIVE_RESULT,FORCEDATETIME,CHECKSUM_GOOD);
}

CString *CSafePropertySheet::GetVersionString()
{
	return &m_pParent->m_csVersion;
}

BOOL CSafePropertySheet::OnNcActivate(BOOL bActive) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_pDataWatchDlg)
	{
		if (m_bDataWatchDlgOpen)
		{
//			if (bActive) m_pDataWatchDlg->ShowWindow(SW_HIDE);
////			m_pDataWatchDlg->SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
////			SetWindowPos(&wndTop   ,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
	}

	return CPropertySheet::OnNcActivate(bActive);
}

