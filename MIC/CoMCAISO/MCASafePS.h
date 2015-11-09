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
#ifndef MCASAFEPS_H
#define MCASAFEPS_H

// MCASafePS.h : header file

#include "CCButton.h"
#include "JulianT.h"

#define MCAACQUIRERECCOUNT		32
#define MCAACCUMULATOR_SIZE		2048
#define EOL1					0x0a
#define EOL2					0x0d

#define MCACONFIG1_RECORD		(0x10)
#define MCACONFIG2_RECORD		(0x11)
#define MCACONFIG3_RECORD		(0x12)
#define MCACONFIG4_RECORD		(0x13)
#define MCACONFIG5_RECORD		(0x14)
#define ACQUIRE_RECORD			(0x15)
#define END_DUMP_RECORD			(0x16)
#define END_DUMP2_RECORD		(0x61)
#define DUMP_OK_RECORD			(0x17)
#define BATTERY_RECORD			(0x18)
#define GEN_STATUS_RECORD		(0x1b)
#define MONITOR_ID2_RECORD		(0x1e)
#define ANALYZE_OK_RECORD		(0x1f)

#define SPECT_HEAD_RECORD		(0x20)
#define SPECT_DATA_RECORD		(0x21)
#define SPECT_TAIL_RECORD		(0x22)

#define INFORMATION_RECORD		(0x30)
//these don't actually correspond to the first chars of a message
#define UNKNOWN_REC				(0xf8)
#define NINETY					(0x90)
#define MINIADC_REC				(0xf9)
#define FORCEDATETIME			(0xfa)
#define MONITORFAIL_RECORD		(0xfb)
#define BUSY_RECORD				(0xfc)
#define ECHO_RECORD				(0xfd)
#define ECHO_2_REC				(0xfe)
#define ECHO_3_REC				(0xff)

#define ECHO_4_REC				(0xfc)  //<<QA>> same as BUSY_RECORD

#define MCAACQUIRE_RECORD_LENGTH		97
#define MCAEND_DUMP_RECORD_LENGTH		6
#define MCAEND_DUMP2_RECORD_LENGTH		7
#define MCADUMP_OK_RECORD_LENGTH		12
#define MCABATTERY_RECORD_LENGTH		20
#define MCAGEN_STATUS_RECORD_LENGTH		69	
#define MCAMONITOR_ID2_RECORD_LENGTH	32	
#define MCAANALYZE_OK_RECORD_LENGTH		12
#define MCAINFORMATION_RECORD_LENGTH	34
#define MCACONFIG1_RECORD_LENGTH		91
#define MCACONFIG2_RECORD_LENGTH		88
#define MCACONFIG3_RECORD_LENGTH		39
#define MCACONFIG4_RECORD_LENGTH		56
#define MCACONFIG5_RECORD_LENGTH		163

#define MCASPECTHEAD_RECORD_LENGTH		30
#define MCASPECTTAIL_RECORD_LENGTH		6

#define MCASPECT_MAX	1024

//Status byte 1 
#define MCARESTARTREQ   0x0020
#define MCASPECTRUMCH	0x0010
#define MCAACPOWERLOSS	0x0008
#define MCABATTERYLOW	0x0004
#define MCABBMERROR		0x0002
#define MCAFILTEROFF	0x0001

//Status byte 2 BBM error codes
#define MCABBMERRMASK	0x0007
#define MCABBMBADREAD	0x0001
#define MCABBMBADWRITE	0x0002
#define MCABBMEMPTY		0x0003
#define MCABBMFULL		0x0004
#define MCABBMBADBUFSZ	0x0005
#define MCABBMBADOPCOD	0x0006
#define MCABBMBADREAD_TEXT	"Bad Read"
#define MCABBMBADWRITE_TEXT	"Bad Write"
#define MCABBMEMPTY_TEXT	"Empty"
#define MCABBMFULL_TEXT		"Full"
#define MCABBMBADBUFSZ_TEXT	"Bad Buffer Size"
#define MCABBMBADOPCOD_TEXT	"Bad Opcode"

//Status byte 2 Monitor error codes
#define MCAMONERRMASK	0x0038	//system start up errors
#define MCAMONACQFAIL	0x0008  //CRMS_PARAMS_CS_FAILURE
#define MCAMONMONFAIL	0x0010	//MON_PARAMS_CS_FAILURE
#define MCAMONGDCFAIL	0x0018	//GDC_SETUP_FAILURE
#define MCAMONHVCFAIL	0x0020	//HVC_SETUP_FAILURE
#define MCAMONACQFAIL_TEXT	"Acq. Sys. Param. CS Failure"
#define MCAMONMONFAIL_TEXT	"Mon. Param. CS Failure"
#define MCAMONHVCFAIL_TEXT	"HVC Sys. Setup Failure"
#define MCAMONGDCFAIL_TEXT	"GDC Sys. Setup Failure"
#define MCAMONUNKNOWN_TEXT  "Unknown error"

//alarm byte error codes
#define MCAALARMU235	0x0001
#define MCAALARMCS137	0x0002
#define MCAALARMU238	0x0004
#define MCAALARMGROSSC	0x0008
#define MCAALARMSCALER	0x0010
#define MCAALARMRATIO1	0x0020
#define MCAALARMRATIO2  0x0040

//HVC System Error Codes
#define MCAHVCSOHMASK	0x000f
#define MCAHVCTEFFOOR	0x0001  //Teff_OUT_OF_RANGE
#define MCAHVCTIMMTOO	0x0002	//TEMP_IS DRIFTING
#define MCAHVCTEFFTOO	0x0003  //Teff_TOO_FAR_OFF
#define MCAHVCISDRIFT	0x0004  //HV_IS_DRIFTING
#define MCAHVCTARNOTR	0x0005  //HV_TARGET_NOT_REACHED
#define MCAHVCTIMMOFF	0x0006  //Timm_OUT_OF_RANGE
#define MCAHVCPARAMCS	0x0007  //HVC_PARAMETERS_BAD_CS
#define MCAHVCTEFFOOR_TEXT "T(eff) Out Of Range"
#define MCAHVCTIMMTOO_TEXT "T(imm) is Drifting"
#define MCAHVCTEFFTOO_TEXT "T(eff) is Drifting"
#define MCAHVCISDRIFT_TEXT "HV is Drifting"
#define MCAHVCTARNOTR_TEXT "HV target Not Reached"
#define MCAHVCTIMMOFF_TEXT "T(imm) Out of Range"
#define MCAHVCPARAMCS_TEXT "HV Param Bad CS"

#define MCAHVCONHOLD	0x0020  //hv on hold

#define MCAHVCENABLE	0x00c0  //mask
#define MCAHVCAUTO		0x0040	//automatic
#define MCAHVCFIXED		0x0080	//fixed
#define MCAHVCTEMP		0x00c0	//temperature fixed
#define MCAHVCAUTO_TEXT		"Auto"
#define MCAHVCFIXED_TEXT	"Fixed"
#define MCAHVCTFIXED_TEXT	"Temp-Fixed"

//GDC State of health
#define MCAGDCSOHMASK	0x003f
#define MCAGDCRPAREA	0x0001
#define MCAGDCRPCROWD	0x0002
#define MCAGDCRPANALY	0x0003
#define MCAGDCPEAKSEA	0x0004
#define MCAGDCOORANGE	0x0005
#define MCAGDCROIOOLI	0x0006
#define MCAGDCRPOBSCU	0x0007
#define MCAGDCRPAREA_TEXT	"RP Area Too Small"
#define MCAGDCRPCROWD_TEXT	"RP Area Crowded by U238"
#define MCAGDCRPANALY_TEXT	"RP Analysis Failure"
#define MCAGDCPEAKSEA_TEXT	"Peak Search Failure"
#define MCAGDCOORANGE_TEXT	"GDC Out of Range"
#define MCAGDCROIOOLI_TEXT	"ROI Out of Limits"
#define MCAGDCRPOBSCU_TEXT	"RP Obscured by Higher Energies"
#define MCAGDCONHOLD	0x0040
#define MCAGDCENABLE	0x00c0

#define UNKNOWN_TEXT "Unknown"

//pack structure at the byte level
#pragma pack(1)
struct MCAAcquireRecord
{
	unsigned int JulianSeconds;	//4 bytes
	BYTE	StatusByte1;	//1 byte
	BYTE	StatusByte2;	//1 byte
	BYTE	Alarm;			//1 byte
	float	U235;			//4 bytes
	float	CS137;			//4 bytes
	float	U238;			//4 bytes
	float   Unknown;		//4 bytes
	float	Scaler;			//4 bytes
	float	U235Ratio;		//4 bytes
	float	CS137Ratio;		//4 bytes
	int		ElapsedTime;	//4 bytes
};
// TRUE DATA ORDER
//	u235
//	cs137
//	u238
//	gross counts (unknown)
//  scaler
//	ratio 1
//	ratio 2

//pack structure at the byte level
#pragma pack(1)
struct MCAAcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	unused2[47];
// or replace unused2 with...
//	char	Year4[4];
//	char	unused2[43];
};
#pragma pack()

class CMCAInst;
/////////////////////////////////////////////////////////////////////////////
// CMCAPropertySheet

class CMCAPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMCAPropertySheet)

// Construction
public:

	CMCAPropertySheet(UINT nIDCaption, CMCAInst* pParentWnd);
	CMCAPropertySheet(LPCTSTR pszCaption, CMCAInst* pParentWnd);
	void FinishConstruction();
	virtual ~CMCAPropertySheet();

	COLORREF SetButtonColor(COLORREF Color);
	void WriteToFile(int ToWhom, int Type, CJulianTime* Time = NULL, char* Msg = NULL);
	bool WriteToMCAFile(CJulianTime* Time, MCAAcquireRecord* AcquireRec);
	void SetTabColor(int Tab, COLORREF Color);
	void SetLimits(
				float batvolth, float batvoltl,
				float sup_p15h, float sup_p15l,
				float sup_m15h, float sup_m15l,
				float sup_p05h, float sup_p05l,
				int   time,		char * pId);

	void IssuePrint(FILE* outfile);
	CString *GetVersionString();

	virtual void	SetToClose(int set);
	virtual void	ForceDateTime(void);
	virtual void	UpdateSummary(char *);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMCAPropertySheet)
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
	CMCAInst		*m_pParent;
	CImageList		*m_pImageList;

	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentMCAFileName[_MAX_PATH];

	int				m_dAt;
	int				m_dAcquireRecordCount;
	struct			MCAAcquireRecord MCAAcquireRec[MCAACQUIRERECCOUNT];
	CJulianTime		AcquireTime[MCAACQUIRERECCOUNT];


	// Generated message map functions
protected:
	int General_Tab;
	int Data_Tab;
	int UserProg_Tab;

	void AcquirRec(char *);
	void Config1Rec(char *);
	void Config2Rec(char *);
	void Config3Rec(char *);
	void Config4Rec(char *);
	void Config5Rec(char *);
	void DumpEnd(char *);
	void DumpEnd2(char *);
	void DumpOk(char *);
	void BatteryRec(char *);
	void GenStatRec(char *);
	void ID2_Record(char *);
	void AnalyzeOk(char *);
	void Busy_Rec(char *);
	void InformRec(char *);
	void Echo_Rec(char *);
	void Echo_2_Rec(char *);
	void Echo_3_Rec(char *);
	void Echo_4_Rec(char *);
	void Echo_5_Rec(char *);

	void SpectHead(char *);
	void SpectData(char *);
	void SpectTail(char *);

	int Checksum(char *);
	bool LengthOk(int length, char* Record);
	void SetChecksumDisplay();

	float m_fBatVoltH; 
	float m_fBatVoltL;
	float m_fSup_P15H; 
	float m_fSup_P15L;
	float m_fSup_M15H; 
	float m_fSup_M15L;
	float m_fSup_P05H; 
	float m_fSup_P05L;
	long  m_dTime_Err;

	float m_fLowBV;
	float m_fCurBV;
	float m_fHighBV;
	float m_fLowP15;
	float m_fCurP15;
	float m_fHighP15;
	float m_fLowM15;
	float m_fCurM15;
	float m_fHighM15;
	float m_fLowP5;
	float m_fCurP5;
	float m_fHighP5;
	bool  m_bFirstBV;
	bool  m_bFirstP15;
	bool  m_bFirstM15;
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
//	int	  m_dCurrentHour;
//	int   m_dCurrentMinute;

	bool  m_bStartOfFile;

	bool  m_bBatteryOk;
//	bool  m_bBatteryTolOk;
	bool  m_bP5VoltOk;
	bool  m_bP15VoltOk;
	bool  m_bM15VoltOk;
	bool  m_bACPowerOk;
	bool  m_bACPowerLoss;

	bool  m_bFiltering;
	bool  m_bSpectChange;
	bool  m_bRestartReq;
	bool  m_bBBMError;
	bool  m_bMonError;
	bool  m_bAlarmU235;
	bool  m_bAlarmU238;
	bool  m_bAlarmCs137;
	bool  m_bAlarmScaler;
	bool  m_bAlarmGrossCnt;
	bool  m_bAlarmRatio1;
	bool  m_bAlarmRatio2;
	bool  m_bGDCSOHError;
	bool  m_bGDCSOHEnable;
	bool  m_bGDCSOHOnHold;
	CJulianTime  m_dGDCOnHold;
	bool  m_bHVCSOHError;
	bool  m_bHVCSOHEnable;
	bool  m_bHVCSOHOnHold;
	CJulianTime  m_dHVCOnHold;

	bool  m_bTimeError;
	bool  m_bBBMOk;
	bool  m_bCountRateTamperOk;
	bool  m_bRMSSigmaTamperOk;
	int   m_dCWStart;
	bool  m_bNewFileName;

	bool  m_bInMIIU235;	
	bool  m_bInMIIU238;	
	bool  m_bInMIICs137;	
	bool  m_bInMIIScaler;	
	bool  m_bInMIIGrossCounts;
	bool  m_bInMIIRatio1;
	bool  m_bInMIIRatio2;

	bool  m_bDataFiltered;
	char  m_szPreviousGTime[16]; 

	char  m_pAccumulator[MCAACCUMULATOR_SIZE];
	char  m_pCaption[256];
	CString m_LastConfig10;
	CString m_LastConfig11;
	CString m_LastConfig12;
	CString m_LastConfig13;
	CString m_LastConfig14;
	CString m_cCommStatus;
	CString m_cCheckSumMsg;
	bool  m_bCouldNotOpenBinaryFile;

	long	m_uiSpectrum[MCASPECT_MAX];
	bool	m_bReceivingSpectra;
	int 	m_iSpectNumChan;
	CJulianTime m_cSpectJulian;

	void BuildFileName(CString &cResult,CString cFormat,CString cPath,
		CString cType,CString cID,int iYear,int iMonth,int iDay,
		int iHour,int iMinute,int iSecond,
		CString cSuffix);

	//{{AFX_MSG(CMCAPropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCloseButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPauseButton();
	afx_msg void OnAckButton();
	afx_msg void OnPrintButton();
	afx_msg void OnToCloseButton();
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg void OnReceive(WPARAM WParam, LPARAM LParam);
	afx_msg void OnReceiveOOB(WPARAM WParam, LPARAM LParam);
	afx_msg void OnExtendClose(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // MCASAFEPS_H
