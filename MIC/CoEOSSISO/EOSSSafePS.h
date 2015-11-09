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
#ifndef EOSSSAFEPS_H
#define EOSSSAFEPS_H

#include "ISO_definitions.h"

// EOSSSafePS.h : header file
#include "ccbutton.h"

//#define NUMBEROFACQUIRERECS		64
//#define ACCUMULATOR_SIZE		2048
//#define BINARYACCUMULATOR_SIZE	2048

//these don't actually correspond to the first chars of a message
//#define UNKNOWN_REC				(0xf8)
#define FORCEDATETIME			(0xfa)
//#define ECHO_RECORD				(0xfd)

//#define ECHO_REC				0x00
//#define ECHO_REC_LENGTH			6

//#define DUMPOK_REC				0x3137
//#define DUMPOK_REC_LENGTH		10

//#define INQUIRE2_REC			0x3145
//#define INQUIRE2_REC_LENGTH		26

//#define STATUS_REC				0x3334
//#define STATUS_REC_LENGTH		15

//#define BINARYEOSS_REC			0x3332
//#define VACOSSINFO_REC			0x3335
//#define VACOSSEOSS_REC			0x3336
//#define VACOSSERROR_REC			0x3339
//#define GPSDATA_REC				0x3341
//#define ENDDUMP_REC				0x3632

//#ifdef RFIDDEMO
//#define RADDATA_REC				0x3445
//#define LGPSDATA_REC			0x3446
//#define RFIDDATA_REC			0x3447
//#endif

//This group from MIC 1.9.0.7 SafePS.h
//#define STATUS_RECORD			(0x12)
//#define ACQUIRE_RECORD			(0x15)
//#define END_DUMP_RECORD			(0x16)
//#define END_DUMP2_RECORD		(0x61)
//#define DUMP_OK_RECORD			(0x17)
//#define MONITOR_ID2_RECORD		(0x1e)
//#define ECHO_3_REC				(0xff)
//End group from MIC 1.9.0.7 SafePS.h

//status byte 1
//#define BBMERROR			0x0008
//status byte 2
//#define BBMFULL				0x0008
//#define BBMBADRECSIZE		0x0004
//#define BBMBADWRITE			0x0002
//#define BBMBADOPCODE		0x0001

//#define UNUSED			0x0002
//#define INMII				0x0001

//#define FAIL				"Fail"
//#define OK					"Ok"
//#define LOW					"Low"
#define YES					"Yes"
#define NO					"No"
//#define INMII_TXT			"In MII"
//#define NOTINMII_TXT		"Not in MII"
//#define START_COLD			"Cold Start"
//#define START_NORMAL		"Normal"

//status byte 2
//#define DATAFILTER_FLAG		0x0080
//#define BBMISFULLEDER_FLAG	0x0040
//#define BBMNUMOFBYTES_FLAG	0x0020
//#define BBMWRITEERROR_FLAG	0x0010

//#define DATAFILTER_YES		"Filtered"
//#define DATAFILTER_NO		"No Filter"
//#define	BBMWRITEERROR		"Could not write value to BBM"
//#define BBMNUMOFBYTES		"Number of bytes for opcode wrong"
//#define BBMISFULLEDER		"BBM is filled"
//#define BBMINVALIDOPC		"Invalid opcode read from BBM"

#define REDFLAG				"*"

#include "JulianT.h"

//pack structures at the byte level
#pragma pack(1)

#pragma pack()

class CEOSSInstrument;
/////////////////////////////////////////////////////////////////////////////
// CEOSSPropertySheet
// property sheet for the EOSS instruments
class CEOSSPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CEOSSPropertySheet)

// Construction
public:
	CEOSSPropertySheet(
		UINT nIDCaption, 
		CEOSSInstrument* pParentWnd);

	CEOSSPropertySheet(
		LPCTSTR pszCaption, 
		CEOSSInstrument* pParentWnd);

	void FinishConstruction();

	virtual ~CEOSSPropertySheet();

	COLORREF SetButtonColor(COLORREF Color);
	void WriteToFile(int ToWhom, int Type, CJulianTime* Time = NULL, char* Message = NULL);
	bool WriteToESSFile(int iIndex);
	void SetTabColor(int Tab, COLORREF Color);
	void SetLimits(int time, char * pId);
	void IssuePrint(FILE* outfile);

	CString *GetVersionString();

	virtual void SetToClose(int set);
	virtual void ForceDateTime(void);

	virtual void UpdateSummary(char *);
	void    RestartAutoCloseTimer(){m_dToClose = 0;}
	bool	EditEOSSList();
	void	RefreshDataDisplay();
	void	ResetReaderCount();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEOSSPropertySheet)
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
	CEOSSInstrument *m_pParent;
	CImageList		*m_pImageList;

	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentEVTFileName[_MAX_PATH];

protected:
	int Data_Tab;

	void Reverse(BYTE*,int);
	int AppendCheckSum(char*);

	void MakeMyPath(const char *);
	void BuildFileName(CString &cResult,CString cFormat,CString cPath,
		CString cType,CString cID,int iYear,int iMonth,int iDay,
		int iHour,int iMinute,int iSecond,
		CString cRFID,CString cSuffix);

	long  m_dTime_Err;			//Difference between MIC and Inst 

	char  m_szCurrentID[3];
	int	  m_dCurrentYear;
	int   m_dCurrentMonth;
	int   m_dCurrentDay;
//	int	  m_dCurrentHour;
//	int   m_dCurrentMinute;

	bool  m_bStartOfFile;

//	bool  m_bBatteryOk;
//	bool  m_bBatteryTolOk;
//	bool  m_bP5VoltOk;
//	bool  m_bP15VoltOk;
//	bool  m_bM15VoltOk;
//	bool  m_bACPowerOk;
//	bool  m_bACPowerLoss;
	bool  m_bTimeError;
	bool  m_bBBMOk;
//	bool  m_bCountRateTamperOk;
//	bool  m_bRMSSigmaTamperOk;
//	int   m_dCWStart;
	bool  m_bNewFileName;
//	bool  m_bInMII;
//	bool  m_bDataFiltered;
	char  m_szPreviousGTime[16];
//	char  m_pAccumulator[ACCUMULATOR_SIZE];
//	BYTE  m_pBinaryAccumulator[BINARYACCUMULATOR_SIZE];
	char  m_pCaption[256];
//	CString m_cCommStatus;
//	CString m_cCheckSumMsg;
//	bool  m_bCouldNotOpenBinaryFile;

	//{{AFX_MSG(CEOSSPropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCloseButton();
	afx_msg void OnTimer(UINT nIDEOSS);
	afx_msg void OnPauseButton();
	afx_msg void OnAckButton();
	afx_msg void OnPrintButton();
	afx_msg void OnToCloseButton();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg LRESULT OnReceive(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveOOB(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnExtendClose(WPARAM WParam, LPARAM LParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EOSSSAFEPS_H
