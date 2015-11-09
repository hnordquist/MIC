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
// CMCADData.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "MCACDData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMCACDData property page

IMPLEMENT_DYNCREATE(CMCACDData, CPropertyPage)

CMCACDData::CMCACDData() : CPropertyPage(CMCACDData::IDD)
{
}


CMCACDData::CMCACDData(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_MCACDDATA_FIRST; i <= IDC_MCACDDATA_LAST; i++)
	{
		StaticColors[i-IDC_MCACDDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CMCACDData)
	//}}AFX_DATA_INIT
}

void
CMCACDData::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_MCACDDATA_FIRST) &&
		(index <= IDC_MCACDDATA_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}
void
CMCACDData::ResetColors()
{
	for (int i= IDC_MCACDDATA_FIRST; i <= IDC_MCACDDATA_LAST; i++)
	{
		StaticColors[i-IDC_MCACDDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void
CMCACDData::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_MCACDDATA_FIRST) &&
		(index <= IDC_MCACDDATA_LAST)) {
	  StaticColors[index-IDC_MCACDDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

CMCACDData::~CMCACDData()
{
}

void CMCACDData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCACDData)
	DDX_Control(pDX, IDC_RADIO_CONFIG10, m_RadioConfig10);
	DDX_Control(pDX, IDC_RADIO_CONFIG11, m_RadioConfig11);
	DDX_Control(pDX, IDC_RADIO_CONFIG12, m_RadioConfig12);
	DDX_Control(pDX, IDC_RADIO_CONFIG13, m_RadioConfig13);
	DDX_Control(pDX, IDC_RADIO_CONFIG14, m_RadioConfig14);
	DDX_Control(pDX, IDC_MCACD_CONFIG10, m_StaticConfig10);
	DDX_Control(pDX, IDC_MCACD_CONFIG14, m_StaticConfig14);
	DDX_Control(pDX, IDC_MCACD_CONFIG13, m_StaticConfig13);
	DDX_Control(pDX, IDC_MCACD_CONFIG12, m_StaticConfig12);
	DDX_Control(pDX, IDC_MCACD_CONFIG11, m_StaticConfig11);
	DDX_Control(pDX, IDC_EDIT_CONTENTS, m_pEditContents);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCACDData, CPropertyPage)
	//{{AFX_MSG_MAP(CMCACDData)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_CONFIG10, OnRadioConfig10)
	ON_BN_CLICKED(IDC_RADIO_CONFIG11, OnRadioConfig11)
	ON_BN_CLICKED(IDC_RADIO_CONFIG12, OnRadioConfig12)
	ON_BN_CLICKED(IDC_RADIO_CONFIG13, OnRadioConfig13)
	ON_BN_CLICKED(IDC_RADIO_CONFIG14, OnRadioConfig14)
	ON_WM_LBUTTONDOWN()
	ON_EN_SETFOCUS(IDC_EDIT_CONTENTS, OnSetfocusEditContents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMCACDData message handlers

HBRUSH CMCACDData::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_MCACDDATA_FIRST) && (index <= IDC_MCACDDATA_LAST))
	{
		int iFirstOfList = IDC_MCACDDATA_FIRST; 
		if (StaticColors[index-iFirstOfList]==COLOR_RED)
		{
			pDC->SetBkColor(COLOR_RED);
			pDC->SetTextColor(RGB(255,255,255));
		}
		else if (StaticColors[(index-iFirstOfList)]==COLOR_YELLOW)
		{
			pDC->SetBkColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_RED);
		}
		else
		{
			pDC->SetTextColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(RGB(0,0,0));
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}
	return hbr;
}

void CMCACDData::Concat(char* text, char* Source, int Start, int Length)
{
	char temp[32];
	Text += text;
	strncpy(temp,&Source[Start],Length);
	temp[Length] = NULL;
	Text += temp;
	Text += "\r\n";
}


void CMCACDData::OnRadioConfig10() 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	if (m_StaticConfig10.GetWindowText(temp1,256))
	{
		strncpy(temp2,&temp1[2],10);
		temp2[10] = NULL;
		JulianT.Init(temp2);
		Text = "Instrument Part Numbers and Electronic Settings\r\n";
		Text += "Date\\Time: ";
		Text += temp2;
		Text += " = ";
		Text += JulianT.Date();
		Text += " - ";
		Text += JulianT.Time();
		Text += "\r\n";
		Concat("MPU ID: ",					temp1, 12, 8);
		Concat("ADC ID: ",					temp1, 20, 3);
		Concat("AMP ID: ",					temp1, 23, 3);
		Concat("PS ID: ",					temp1, 26, 3);
		Concat("Version: ",					temp1, 29, 5);
		Concat("Comment: ",					temp1, 34,20);
		Concat("Amp Pol: ",					temp1, 54, 1);
		Concat("Coarse Gain: ",				temp1, 55, 2);
		Concat("Fine Gain: ",				temp1, 57, 4);
		Concat("Lt Rt: ",					temp1, 61, 1);
		Concat("LLd: ",						temp1, 62, 4);
		Concat("ULd: ",						temp1, 66, 4);
		Concat("HVC: ",						temp1, 70, 1);
		Concat("Time Interval: ",			temp1, 71, 2);
		Concat("T(eff) Epsilon: ",			temp1, 73, 1);
		Concat("Default Temperature: ",		temp1, 74, 3);
		Concat("SOH Time Interval: ",		temp1, 77, 2);
		Concat("Lookup Table Version:",		temp1, 79, 8);
		Concat("Checksum: ",				temp1, 87, 2);//90
	}
	else
		Text.Empty();
	m_pEditContents.SetWindowText(Text);

}

void CMCACDData::OnRadioConfig11() 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	if (m_StaticConfig11.GetWindowText(temp1,256))
	{
		strncpy(temp2,&temp1[2],10);
		temp2[10] = NULL;
		JulianT.Init(temp2);
		Text = "CRMS ROIs and Thresholds\r\n";
		Text += "Date\\Time: ";
		Text += temp2;
		Text += " = ";
		Text += JulianT.Date();
		Text += " \\ ";
		Text += JulianT.Time();
		Text += "\r\n";
		Concat("ROI 1a 1b: ",			temp1, 12, 8);
//		Concat("ROI 1b: ",			temp1, 16, 4);
//		Text += "\r\n";
		Concat("ROI 2a 2b: ",			temp1, 20, 8);
//		Concat("ROI 2b: ",			temp1, 24, 4);
//		Text += "\r\n";
		Concat("ROI 3a 3b: ",			temp1, 28, 8);
//		Concat("ROI 3b: ",			temp1, 32, 4);
//		Text += "\r\n";
		Concat("ROI 4a 4b: ",			temp1, 36, 8);
//		Concat("ROI 4b: ",			temp1, 40, 4);
//		Text += "\r\n";
		Concat("Watch / Alarm / Trigger ROI 1: ",		temp1, 44, 3);
//		Concat("Alarm on ROI 1: ",	temp1, 45, 1);
//		Concat("Trigger on ROI 1: ",temp1, 46, 1);
		Concat("ROI 1 Threshold: ", temp1, 47, 5);
		Text += "\r\n";
		Concat("Watch / Alarm / Trigger ROI 2: ",		temp1, 52, 3);
//		Concat("Alarm on ROI 2: ",	temp1, 53, 1);
//		Concat("Trigger on ROI 2: ",temp1, 54, 1);
		Concat("ROI 2 Threshold: ", temp1, 55, 5);
		Text += "\r\n";
		Concat("Watch / Alarm / Trigger ROI 3: ",		temp1, 60, 3);
//		Concat("Alarm on ROI 3: ",	temp1, 61, 1);
//		Concat("Trigger on ROI 3: ",temp1, 62, 1);
		Concat("ROI 3 Threshold: ", temp1, 63, 5);
		Text += "\r\n";
		Concat("Watch / Alarm / Trigger ROI 4: ",		temp1, 68, 3);
//		Concat("Alarm on ROI 4: ",	temp1, 69, 1);
//		Concat("Trigger on ROI 4: ",temp1, 70, 1);
		Concat("ROI 4 Threshold: ", temp1, 71, 5);
		Text += "\r\n";
		Concat("Watch / Alarm / Trigger Scaler: ",	temp1, 76, 3);
//		Concat("Alarm on Scaler: ",	temp1, 77, 1);
//		Concat("Trigger on Scaler: ",temp1,78, 1);
		Concat("Scaler Threshold: ",temp1, 79, 5);
		Text += "\r\n";
		Concat("Checksum: ",		temp1, 84, 2);//87
	}
	else
	{
		Text.Empty();
	}
	m_pEditContents.SetWindowText(Text);	
}

void CMCACDData::OnRadioConfig12() 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	if (m_StaticConfig12.GetWindowText(temp1,256))
	{
		Text = "Monitor Times & Time-sync Parameters\r\n";
		strncpy(temp2,&temp1[2],10);
		temp2[10] = NULL;
		JulianT.Init(temp2);
		Text += "Date\\Time: ";
		Text += temp2;
		Text += " = ";
		Text += JulianT.Date();
		Text += " \\ ";
		Text += JulianT.Time();
		Text += "\r\n";

		Concat("Process Point Enabled: ",		temp1, 12, 1);
		Concat("Monitor Time Interval: ",		temp1, 13, 2);
		Concat("Daily Status Time Interval: ",	temp1, 15, 2);
		Concat("Battery Status Time Interval: ",temp1, 17, 2);
		Concat("Daily Hour: ",					temp1, 19, 2);
		Concat("Daily Minute: ",				temp1, 21, 2);
		Concat("Daily Second: ",				temp1, 23, 2);
		Concat("Hourly Minute: ",				temp1, 25, 2);
		Concat("Hourly Second: ",				temp1, 27, 2);
		Concat("Tolerance Lower: ",				temp1, 29, 2);
		Concat("Tolerance Upper: ",				temp1, 31, 2);
		Concat("Daily Enabled: ",				temp1, 33, 1);
		Concat("Hourly Enabled: ",				temp1, 34, 1);
		Concat("Checksum: ",					temp1, 35, 2);//38
	}
	else
	{
		Text.Empty();
	}
	m_pEditContents.SetWindowText(Text);	
	
}

void CMCACDData::OnRadioConfig13() 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	if (m_StaticConfig13.GetWindowText(temp1,256))
	{
		Text = "Analysis Parameters\r\n";
		strncpy(temp2,&temp1[2],10);
		temp2[10] = NULL;
		JulianT.Init(temp2);
		Text += "Date\\Time: ";
		Text += temp2;
		Text += " = ";
		Text += JulianT.Date();
		Text += " \\ ";
		Text += JulianT.Time();
		Text += "\r\n";

		Concat("Threshold Entry Count: ",	temp1, 12, 3);
		Concat("Threshold Exit Count: ",	temp1, 15, 3);
		Concat("Trigger Type: ",			temp1, 18, 2);
		Concat("Immediate Size: ",			temp1, 20, 3);
		Concat("Immediate Save: ",			temp1, 23, 3);
		Concat("Save Boundary: ",			temp1, 26, 3);
		Concat("Changing Entry Count: ",	temp1, 29, 3);
		Concat("Sigma Multiplier: ",		temp1, 32, 6);
		Concat("Background Start: ",		temp1, 38, 3);
		Concat("Background End: ",			temp1, 41, 3);
		Concat("Background Size: ",			temp1, 44, 3);
		Concat("No Filter: ",				temp1, 47, 1);
		Concat("Use Average Points: ",		temp1, 48, 1);
		Concat("Maximum Buffers Saved: ",	temp1, 49, 3);
		Concat("Checksum: ",				temp1, 52, 2);//55	
	}
	else
	{
		Text.Empty();
	}
	m_pEditContents.SetWindowText(Text);	
	
}

void CMCACDData::OnRadioConfig14() 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	if (m_StaticConfig14.GetWindowText(temp1,256))
	{
		Text = "Monitor Thresholds\r\n";
		strncpy(temp2,&temp1[2],10);
		temp2[10] = NULL;
		JulianT.Init(temp2);
		Text += "Date\\Time: ";
		Text += temp2;
		Text += " = ";
		Text += JulianT.Date();
		Text += " \\ ";
		Text += JulianT.Time();
		Text += "\r\n";
		Text += "Channel 1...\r\n";
		Concat("Used: ",				temp1, 12, 1);
		Concat("Type: ",				temp1, 13, 2);
		Concat("Trigger Threshold: ",	temp1, 15, 6);
		Concat("Alarm Threshold: ",		temp1, 21, 6);
		Concat("Filter Limit: ",		temp1, 27, 6);
		Text += "\r\nChannel 2...\r\n";
		Concat("Used: ",				temp1, 33, 1);
		Concat("Type: ",				temp1, 34, 2);
		Concat("Trigger Threshold: ",	temp1, 36, 6);
		Concat("Alarm Threshold: ",		temp1, 42, 6);
		Concat("Filter Limit: ",		temp1, 48, 6);
		Text += "\r\nChannel 3...\r\n";
		Concat("Used: ",				temp1, 54, 1);
		Concat("Type: ",				temp1, 55, 2);
		Concat("Trigger Threshold: ",	temp1, 57, 6);
		Concat("Alarm Threshold: ",		temp1, 63, 6);
		Concat("Filter Limit: ",		temp1, 69, 6);
		Text += "\r\nChannel 4...\r\n";
		Concat("Used: ",				temp1, 75, 1);
		Concat("Type: ",				temp1, 76, 2);
		Concat("Trigger Threshold: ",	temp1, 78, 6);
		Concat("Alarm Threshold: ",		temp1, 84, 6);
		Concat("Filter Limit: ",		temp1, 90, 6);
		Text += "\r\nChannel 5...\r\n";
		Concat("Used: ",				temp1, 96, 1);
		Concat("Type: ",				temp1, 97, 2);
		Concat("Trigger Threshold: ",	temp1, 99, 6);
		Concat("Alarm Threshold: ",		temp1,105, 6);
		Concat("Filter Limit: ",		temp1,111, 6);
		Text += "\r\nChannel 6...\r\n";
		Concat("Used: ",				temp1,117, 1);
		Concat("Type: ",				temp1,118, 2);
		Concat("Trigger Threshold: ",	temp1,120, 6);
		Concat("Alarm Threshold: ",		temp1,126, 6);
		Concat("Filter Limit: ",		temp1,132, 6);
		Text += "\r\nChannel 7...\r\n";
		Concat("Used: ",				temp1,138, 1);
		Concat("Type: ",				temp1,139, 2);
		Concat("Trigger Threshold: ",	temp1,141, 6);
		Concat("Alarm Threshold: ",		temp1,147, 6);
		Concat("Filter Limit: ",		temp1,153, 6);
		Concat("\r\nChecksum: ",		temp1,159, 2);//162
	}
	else
	{
		Text.Empty();
	}
	m_pEditContents.SetWindowText(Text);	
	
}

void CMCACDData::SetPropertySheet(CMCAPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CMCACDData::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_MCACDDATA_FIRST) && (index <= IDC_MCACDDATA_LAST))
		{
				m_RadioConfig10.SetCheck(0);
				m_RadioConfig11.SetCheck(0);
				m_RadioConfig12.SetCheck(0);
				m_RadioConfig13.SetCheck(0);
				m_RadioConfig14.SetCheck(0);
			switch (index)
			{
			case IDC_MCACD_CONFIG10:
				m_RadioConfig10.SetCheck(1);
				OnRadioConfig10();
				m_RadioConfig10.SetFocus();
				break;
			case IDC_MCACD_CONFIG11:
				m_RadioConfig11.SetCheck(1);
				OnRadioConfig11();
				m_RadioConfig11.SetFocus();
				break;
			case IDC_MCACD_CONFIG12:
				m_RadioConfig12.SetCheck(1);
				OnRadioConfig12();
				m_RadioConfig12.SetFocus();
				break;
			case IDC_MCACD_CONFIG13:
				m_RadioConfig13.SetCheck(1);
				OnRadioConfig13();
				m_RadioConfig13.SetFocus();
				break;
			case IDC_MCACD_CONFIG14:
				m_RadioConfig14.SetCheck(1);
				OnRadioConfig14();
				m_RadioConfig14.SetFocus();
				break;
			}
//			if ((index == IDC_SUMMARY_COMMAND6) ||
//				(index == IDC_SUMMARY_COMMAND5) ||
//				(index == IDC_SUMMARY_COMMAND4) ||
//				(index == IDC_SUMMARY_COMMAND3) ||
//				(index == IDC_SUMMARY_COMMAND2) ||
//				(index == IDC_SUMMARY_COMMAND1))
//			{
//				char Text[256];
//				Text[0] = NULL;
//				GetDlgItemText(index,Text,sizeof(Text));
//				if (strlen(Text))
//				{
//					strcat(Text,"\"");
//					memmove(&Text[2],Text,strlen(Text)+1);
//					Text[0] = '"';
//					Text[1] = 'G';
//					_spawnl(_P_NOWAIT,g_cUtilPath,"MsgUtil.exe",Text,NULL);
//				}
//			}
		}
	}

	
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CMCACDData::OnSetfocusEditContents() 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
}

BOOL CMCACDData::OnSetActive() 
{
	return CPropertyPage::OnSetActive();
}

BOOL CMCACDData::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (EditFont.CreateFont(-12,0,0,0,FW_SEMIBOLD,0,0,0,0,0,0,0,0,"SYSTEM"))
//	if (EditFont.CreateFont(-12,0,0,0,FW_SEMIBOLD,0,0,0,0,0,0,0,0,"Lucida Console (Western)"))
		m_pEditContents.SetFont(&EditFont,false);
	return TRUE;
}
