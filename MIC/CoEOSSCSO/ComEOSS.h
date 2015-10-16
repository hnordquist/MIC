// ComEOSS.h

#ifndef COMEOSS_H
#define COMEOSS_H

#include "CSO.h"

#define EOSS_DISCOVERY_TYPE "EOSS" 
/*
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
*/
//#define MAX_DATAINPACKET	1500


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
// CComEOSS thread

class CComEOSS : public CCSO
{
	DECLARE_DYNCREATE(CComEOSS)
protected:
	CComEOSS();           // protected constructor used by dynamic creation
	virtual ~CComEOSS();

// Attributes

public:

	char		m_pPort[16];
	bool		m_bDone;
	int			m_dSourceNode;
	COleDateTime	Msg;
	COleDateTime	Now;
	COleDateTimeSpan	Span;

	struct NodeINFO NodeInfo;

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

	virtual BOOL EscapeCommFunction(DWORD dwFunc);
	bool		m_bStopped;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComEOSS)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

protected:

	// Generated message map functions
	//{{AFX_MSG(CComEOSS)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // COMEOSS_H
