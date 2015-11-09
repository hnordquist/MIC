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
#ifndef EVENTINSTRUMENT_H
#define EVENTINSTRUMENT_H

//CEventInstrument Class Header File

#include "EventSafePS.h"
#include "EventDataStat.h"
#include "BinaryDataStat.h"
#include "GPSDataStat.h"
#include "VACOSSDataStat.h"
#include "EventModParam.h"
#include "EventLegend.h"
#include "EventSummary.h"
#include "ISO.h"				//base class
#include "../Interfaces/ICSO.h"	//the communications component interface

#define EVENT_DISCOVERY_TYPE "EVENT"

/////////////////////////////////////////////////////////////////////////////
// CEventInstrument window

class CEventInstrument : public CISO
{
friend class CEventModParam;
friend class CEventPropertySheet;

// Construction
public:
	CEventInstrument();
	CEventInstrument(CWnd* Parent, char *inifile);
	virtual ~CEventInstrument();
	void FinishConstruction();

// Attributes
public:
	bool CollectParametersForNewISO(CString& csNameOut);

	bool CreateInstrument(void);
	bool UnRegInstrument();
	void ShowInstrument(bool);
	void PauseInstrument(bool);
	bool UpdateParams(bool);
	void DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	void IssuePrint(FILE* outfile);
	void Reset();
	void SetLocalRegisteredMessageValues();
	void UpdateStrings();
	void PostToWatch(char *pMessage);

private:

	CEventSummary   *m_pSummaryPage;
	CEventDataStat	*m_pDataStatus;
	CBinaryDataStat *m_pBinaryDataStatus;
	CGPSDataStat	*m_pGPSDataStatus;
	CVACOSSDataStat *m_pVACOSSDataStatus;
	CEventModParam	*m_pModParams;
	CLegend			*m_pLegend;

	CEventPropertySheet	*m_pPropertySheet;

	CWnd		*m_pReceiver;
	COleDateTime m_pToday;
	bool		m_bIsPaused;
	bool		m_bWasPaused;
	bool		m_bDoDump;
	bool		m_bLogMII;
	bool		m_bLogFilter;
	bool		m_bInDumpBBM;
	int			m_dSave_State;
	int			m_dSlowReset;
	int			m_dNumberOfAcquires;
	int			m_dBreakSentCount;
	int			m_dTimeOutCount;
	int			m_dInvalidTimeCount;

	int			m_dCurrent_State;

	CWnd*		m_pParent;
	unsigned int m_dPrevBIDTime;
//	int			m_dPauseMax;		//dgp 18 Oct 2006
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
	float		m_fsup_p15h;
	float		m_fsup_p15l;
	float		m_fsup_m15h;
	float		m_fsup_m15l;
	float		m_fsup_p05h;
	float		m_fsup_p05l;
	int			m_dtime_err;
	int			m_dRetransmit;
	int			m_dLongBreak;
	bool		m_bdoingBBM;
	bool		m_bdumpOK;
	bool		m_bChecksumFail;
//	bool		m_bGoodRegister;	//dgp 18 Oct 2006
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

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventInstrument)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CEventInstrument)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnReceive(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EVENTINSTRUMENT_H
