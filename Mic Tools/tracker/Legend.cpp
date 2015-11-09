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
// Legend.cpp : implementation file
//

#include "stdafx.h"
#include "Tracker.h"
#include "Legend.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLegend dialog


CLegend::CLegend(CWnd* pParent /*=NULL*/)
	: CDialog(CLegend::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLegend)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	AfxInitRichEdit();
}


void CLegend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLegend)
	DDX_Control(pDX, IDC_RICHEDIT1, m_RichEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLegend, CDialog)
	//{{AFX_MSG_MAP(CLegend)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLegend message handlers

HBRUSH CLegend::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
/*	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	COLORREF textcolor; 
	if ((index >= IDC_LEGEND_GREEN) && (index <= IDC_LEGEND_WHITE))
	{
		switch(index)
		{
		case IDC_LEGEND_GREEN: 
			textcolor = COLOR_GREEN;	
			break;
		case IDC_LEGEND_YELLOW: 
			textcolor = COLOR_YELLOW;	
			break;
		case IDC_LEGEND_RED: 
			textcolor = COLOR_RED;		
			break;
		case IDC_LEGEND_CYAN: 
			textcolor = COLOR_CYAN;	
			break;
		case IDC_LEGEND_GRAY: 
			textcolor = COLOR_GRAY;	
			break;
		case IDC_LEGEND_WHITE: 
			textcolor = COLOR_WHITE;	
			break;
		default:
			textcolor = COLOR_DEFAULT_TEXT;
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
		pDC->SetBkColor(textcolor);
	}
	// TODO: Return a different brush if the default is not desired
*/
	return hbr;
}
DWORD CALLBACK ReadEditData(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	CFile* pFile = (CFile*) dwCookie;
	LONG lBytesRead;
	try
	{
		lBytesRead = pFile->Read(pbBuff, cb);
	}
	catch (CFileException* pEx)
	{
		pEx;
		*pcb = 0;
		return 0;
	};
	*pcb = lBytesRead;
	return 0;
}

BOOL CLegend::OnInitDialog() 
{
	CDialog::OnInitDialog();

	bool bDoDefault = true;
	if (m_cName.GetLength()>0)
	{
		CString cName = m_cName;
		cName += ".rtf";
		if (_access(cName,4)==0)
		{
			CFile fIn(cName, CFile::typeBinary | CFile::modeRead);
			EDITSTREAM es;
			es.dwCookie = (DWORD) &fIn;
			es.dwError = NULL;
			es.pfnCallback = ReadEditData;
			while(m_RichEdit.StreamIn(SF_RTF | SFF_SELECTION,es)!=0);
			fIn.Close();
			bDoDefault = false;
		}
		else if (_access("default.rtf",4) == 0)
		{
			CFile fIn("default.rtf", CFile::typeBinary | CFile::modeRead);
			EDITSTREAM es;
			es.dwCookie = (DWORD) &fIn;
			es.dwError = NULL;
			es.pfnCallback = ReadEditData;
			while(m_RichEdit.StreamIn(SF_RTF | SFF_SELECTION,es)!=0);
			fIn.Close();
			bDoDefault = false;
		}
	}
	
	if (bDoDefault)
	{
		// TODO: Add extra initialization here
		m_RichEdit.SetWindowText(
		"Instrument Button Colors...\n"
		"GREEN_ICON Normal state.\n"
		"YELLOW_ICON A command has been sent from MIC to the instrument and MIC is waiting for a response.\n"
		"RED_ICON Communications Failure! A command has been sent and resent but no acceptable response has been received back from the instrument.\n"
		"WHITE_ICON FLASHING to previous color. The disk is full or the instrument has a failure which is causing data loss\n"
		"CYAN_ICON Some instruments cannot respond to MIC during initialization. This is a wait state that MIC may enter for a short period to allow the instrument to finish initializing.\n"
		"GRAY_ICON MIC has been instructed not to talk to the instrument.  In all cases this state will eventuall revert to a normal operational state.\n"
		"\n"
		"Instrument Button ICONs...\n"
		"SQUARE_ICON Square: One or more of the tabbed pages have a data item out of tolerance or a configuration mismatch\n"
		"CLOCK_ICON Clock: The difference between MIC's time and the instrument's time is greater than the user set tolerance\n"
		"FLAG_ICON Flag:  MIC has sent and resent a command with no acceptable response and consequently commanded a long break reset in an attempt to reinitialize the instrument or a Hard Checksum Error occurred.\n"
		"BELL_ICON Bell: The instrument is currently reporting a \"Measurement Interval of Interest\" or equivalent.\n"	
		"FACE_ICON Face: The instrument has reported either a power problem or a battery backed up memory problem.\n"	
		"FLOPPY_ICON Floppy Disk & Flashing: MIC has stopped writing to disk and talking to the instrument.  This can be caused by the hard drive being too full, the maximum use of the hard drive set too low, or disk write failure.\n"
		"SKULL_ICON Skull and Cross Bones: The instrument has repeatedly failed to respond to commands and long break resets.  MIC will attempt to reset it every 15 minutes or instrument is in a state which is causing DATA LOSS.\n"
		"BOMB_ICON Bomb: Configuration Error.  Instrument support object could not connect to communications object.  Edit the MIC.INI file or delete the instrument object and recreate it to correct this problem.\n");

		Substitute("GREEN_ICON", "GREEN",COLOR_GREEN,false);
		Substitute("YELLOW_ICON","YELLOW",COLOR_YELLOW,false);
		Substitute("RED_ICON",   "RED",COLOR_RED,false);
		Substitute("WHITE_ICON","WHITE",COLOR_BLACK,false);
		Substitute("CYAN_ICON", "CYAN",COLOR_CYAN,false);
		Substitute("GRAY_ICON", "GRAY",COLOR_GRAY,false);

		Substitute("RED_ICON","RED",COLOR_RED,false);
		Substitute("SQUARE_ICON","r",COLOR_RED,true);
		Substitute("CLOCK_ICON", "»",COLOR_RED,true);
		Substitute("FLAG_ICON",  "O",COLOR_RED,true);
		Substitute("BELL_ICON",  "%",COLOR_RED,true);
		Substitute("FACE_ICON",  "L",COLOR_RED,true);
		Substitute("FLOPPY_ICON","<",COLOR_RED,true);
		Substitute("SKULL_ICON", "N",COLOR_RED,true);
		Substitute("BOMB_ICON",  "M",COLOR_RED,true);
	//	m_RichEdit.SetSel(0,10);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLegend::Substitute(char* szString, char* szSubStr, COLORREF cTXTColor, bool bWingdings)
{
	FINDTEXTEX ft;
	ft.chrg.cpMin = 0;
	ft.chrg.cpMax = -1;
	ft.lpstrText = szString;
	long n = m_RichEdit.FindText(FR_MATCHCASE|FR_WHOLEWORD, &ft);
	while (n != -1)
	{
		m_RichEdit.SetSel(ft.chrgText);

		// Modify the selection format so that the selected text is 
		// displayed in bold and not striked out.
		CHARFORMAT cf;
		if (bWingdings)
			cf.dwMask = CFM_BOLD|CFM_COLOR|CFM_CHARSET|CFM_FACE|CFM_SIZE;
		else
			cf.dwMask = CFM_BOLD|CFM_COLOR|CFM_SIZE;
		cf.dwEffects = CFE_BOLD;
		cf.crTextColor = cTXTColor;
		cf.bCharSet = SYMBOL_CHARSET;
		cf.yHeight=500;
		strcpy_s(cf.szFaceName,"wingdings");

		m_RichEdit.SetSelectionCharFormat(cf);
		m_RichEdit.ReplaceSel(szSubStr);

		n = m_RichEdit.FindText(FR_MATCHCASE|FR_WHOLEWORD, &ft);
	}
}

void CLegend::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	m_RichEdit.SetSel(0,0);
	
}
