//$History: ComLOCNUT.h $
 * 
 * *****************  Version 2  *****************
 * User: Pelowitz     Date: 4/29/08    Time: 2:14p
 * Updated in $/MIC 2004/CoLOCNUTCSO
 * final check in prior to retirement
// * 
// * *****************  Version 1  *****************
// * User: Pelowitz     Date: 3/12/08    Time: 2:52p
// * Created in $/MIC 2004/CoLOCNUTCSO
// * At or near retirement (EOSS still being tested)
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 9/13/06    Time: 12:39p
// Created in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 2  *****************
// User: Pelowitz     Date: 3/02/06    Time: 2:10p
// Updated in $/CoLOCNUTCSO
// Cleaned up problem with socket blocking on call to rec and added code
// to issue TIMESET messages to node 0 watcher (if one exists) and to
// watcher window (if one exists).
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 1/11/06    Time: 7:47a
// Created in $/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Moore        Date: 8/09/05    Time: 4:12p
// Created in $/MIC 2.000/CoLOCNUTCSO
// 
// *****************  Version 2  *****************
// User: Moore        Date: 5/04/05    Time: 10:07a
// Updated in $/MIC 2.0/CoLOCNUTCSO
// Fixed version static on GRAND component.
// 
// *****************  Version 1  *****************
// User: Moore        Date: 5/03/05    Time: 2:11p
// Created in $/MIC 2.0/CoLOCNUTCSO
// Added CoLOCNUTCSO to workspace.
//
#ifndef COMLOCNUT_H
#define COMLOCNUT_H

// ComLOCNUT.h : header file
//

//#include "Winsock2.h"
#include "Afxsock.h"

#include "CSO.h"

#define LOCNUT_DISCOVERY_TYPE "LOCNUT" 

//hash 
#define MAX_UDPNODE_ARRAY		256

//one of these for each node in the
//node array.  as each instrument registers
//itself the structure for that instrument is
//initialized.  

/////////////////////////////////////////////////////////////////////////////
// CComLOCNUT thread

class CComLOCNUT : public CCSO
{
	DECLARE_DYNCREATE(CComLOCNUT)
protected:
	CComLOCNUT();           // protected constructor used by dynamic creation
	virtual ~CComLOCNUT();


	bool		m_bWorking;
	bool		m_bLastDUMPBBMBinary;
	bool		m_bTimeSyncPC;
	int			m_iPortNumber;
	char		m_szBaseIP[32];
	char		m_szPassphrase[32];
	int			m_iBaseIP[4];
	CWnd*   	m_pReceiver;
	CWnd*		m_pWatcher;
	CWnd*		m_pOOB;
	SOCKET		m_ReceivingSocket;
	SOCKET		m_TimeSyncSocket;
	struct UDPNodeINFO 
	{
		HWND hReceiver;		//pointer to the instrument receiver to snd msg
		HWND hWatcher;		//pointer to port watcher
		HWND hOOB;			//pointer to out of band support
	}	NodeInfo[MAX_UDPNODE_ARRAY];
	afx_msg void	InitSocket(WPARAM,LPARAM);
	afx_msg void	OnReceive(WPARAM,LPARAM);
	afx_msg void    OnReceiveTS(WPARAM,LPARAM);
	afx_msg void	SendSocket(WPARAM,LPARAM);
	int			CheckAndSetTime(unsigned int ulTimeRem, BYTE bFrom);
	COleDateTime m_cLastTimeSync;
	int			m_iTimeSyncCount;


// Attributes

public:
//	char		m_pName[32];
	bool		m_bStopped;
/*
	char		m_pPort[16];
	int			m_dBaudRate;
	int			m_dDataBits;
	int			m_dStopBits;
	bool		m_bDone;
	bool		m_bLongBreak;
	bool		m_bInLocalBreak;
	char		m_pParity[16];
	char		m_pMasterAccumulator[COMLOCNUT_ACCUMULATOR_SIZE];
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
*/
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

	virtual char *GetName();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComLOCNUT)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

protected:

	// Generated message map functions
	//{{AFX_MSG(CComLOCNUT)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // COMLOCNUT_H
