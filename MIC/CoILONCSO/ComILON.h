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
// ComILON.h

#ifndef COMILON_H
#define COMILON_H

#include "CSO.h"

#define ILON_DISCOVERY_TYPE "ILON" 

#define CONTROL_DATA		(0x23)
#define CONTROL_RESPONSE	(0x24)
#define PASSTHROUGH_DATA	(0x21)
#define LONG_BREAK			(0x40)
#define READ_CONFIGURATION  (0x41)
#define WRITE_CONFIGURATION (0x42)
#define GET_TARGETS			(0x43)
#define SEND_TARGET_LIST	(0x44)
#define SEND_MAX_DATA		(0x45)
#define POLL				(0x46)
#define SET_HOUR			(0x47)
#define SYNC_HOUR			(0x48)
#define SET_CLOCK			(0x49)
#define READ_CLOCK			(0x4A)
#define RESET_NODE			(0x4D)
#define MIC_OK				(0x50)
#define CLEAR_ERR_LOG		(0x51)

/*deaddead*/
#define SENTINAL			(0xDEADDEAD)
#define SENTINALSHORT		(0x00ADDEAD)
#define SENTINAL1			(0xde)
#define SENTINAL2			(0xad)
#define SENTINAL3			(0xde)
#define SENTINAL4			(0xad)
#define	SENTINALMASK		(0xFFFFFFFF)
#define SENTINALMASKSHORT	(0x00FFFFFF)
//#undef	SENTINAL_TRANS
#define SENTINAL_REC

//hash 
//#define HASHMASK			(0x0000007F)
#define MAX_NODE_ARRAY		65536
#define MAX_DATAINPACKET	1500

//number of bytes in the accumulators for each 
//node which has an instrument connected to it
#define COMILON_ACCUMULATOR_SIZE	4096

//number of 30 second intervals which
//must pass without receiving anything
//to send to an instrument before
//allowing a local long break to work
#define LOCALLONGBREAKHOLDOFFCOUNT	6

//one of these for each node in the
//node array.  as each instrument registers
//itself the structure for that instrument is
//initialized.  
struct NodeINFO
{
	HWND hReceiver;		//handle to the instrument receiver to snd msg
	HWND hWatcher;		//handle to port watcher
	HWND hOOB;			//handle to out of band support
};

/////////////////////////////////////////////////////////////////////////////
// CComILON thread

class CComILON : public CCSO
{
	DECLARE_DYNCREATE(CComILON)
protected:
	CComILON();           // protected constructor used by dynamic creation
	virtual ~CComILON();

	OVERLAPPED 	m_OverlapR;
	OVERLAPPED  m_OverlapT;

// Attributes

public:

	char		m_pPort[16];
	int			m_dBaudRate;
	int			m_dDataBits;
	int			m_dStopBits;
	bool		m_bDone;
	bool		m_bLongBreak;
	bool		m_bInLocalBreak;
	char		m_pParity[16];
	char		m_pMasterAccumulator[COMILON_ACCUMULATOR_SIZE];
	int			m_dMasterAt;
	int			m_dCurrentType;
	int			m_dSourceNode;
	int			m_dWatchDogNode;
	bool		m_bWatchDogOk;
	bool		m_bWatchDogParticipate;
	int			m_dTerminus;
	int			m_dYear;
	int			m_dMonth;
	int			m_dDay;
	int			m_dHour;
	int			m_dMinute;
	int			m_dSecond;
	int			m_dSendMaxLength;
	COleDateTime	Msg;
	COleDateTime	Now;
	COleDateTimeSpan	Span;

	int			m_dLookBack;

	HANDLE		m_hComm;
	DCB*		lpdcb;
	LPCOMMPROP 	lpCommProp;
	int			m_dCurrentState;

	bool		m_bLastDUMPBBMBinary;
	bool		m_bStopped;
	int			m_iStopPort;
	struct NodeINFO NodeInfo[MAX_NODE_ARRAY];//sparce table to hash into

	int m_dLastLongBreak;
	int m_dAllowLocalLongBreakCount;//dont allow if count is below x
	char m_szLastSend[64];

// Operations

public:

	virtual bool CollectParametersForNewCSO(CString& csNameOut);
	virtual void DoReset(WPARAM,LPARAM);
	virtual void DoStart(WPARAM,LPARAM);
	virtual void DoShutdown(WPARAM,LPARAM);
	virtual void DoStop();

	virtual bool Register(
		HWND receiver_handle,bool bout_of_bounds = false,int node = -1);

	virtual bool RegisterWatch(HWND watcher_handle,int node = -1);
	virtual void SendToWatch(int bytes, char*, int node = -1);
	virtual void SetLocalRegisteredMessageValues();

	virtual bool SetUpPort(char *);
	virtual bool SetUpPort(char *szPort, unsigned BaudRate, unsigned DataBits, 
							unsigned StopBits, char* szParity, bool bLongBreak);
	virtual int Transmit(int bytes,char *, int node = -1, bool Quiet = false);

	virtual bool UnRegister(int node = -1);
	virtual bool UnRegisterWatch(int node = -1);

	virtual void WatchDogSend();
	virtual void WatchDogOk(bool);
	virtual BOOL EscapeCommFunction(DWORD dwFunc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComILON)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

protected:

	// Generated message map functions
	//{{AFX_MSG(CComILON)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // COMILON_H
