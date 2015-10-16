//ComGrand.h

#ifndef COMGRAND_H
#define COMGRAND_H

#include "CSO.h"

#define DIRECT_SERIAL_DISCOVERY_TYPE "SERIAL"

/////////////////////////////////////////////////////////////////////////////
// CComGrand thread

class CComGrand : public CCSO
{
	DECLARE_DYNCREATE(CComGrand)
protected:
	CComGrand();           // protected constructor used by dynamic creation
	virtual ~CComGrand();

	OVERLAPPED m_OverlapR;
	OVERLAPPED m_OverlapT;

// Attributes

public:

	char	m_pPort[16];
	int		m_dBaudRate;
	int		m_dDataBits;
	int		m_dStopBits;
	bool	m_bDone;
	bool	m_bLongBreak;
	bool	m_bInLocalBreak;
	char	m_pParity[16];
	int		m_dWatchDogNode;
	HANDLE	   m_hComm;
	DCB        *lpdcb;
	LPCOMMPROP lpCommProp;

	bool	m_bLastDUMPBBMBinary;
	bool	m_bStopped;
	char	m_szLastSend[64];


// Operations
public:

	virtual bool CollectParametersForNewCSO(CString& csNameOut);
	virtual void DoReset(WPARAM,LPARAM);
	virtual void DoStart(WPARAM,LPARAM);
	virtual void DoShutdown(WPARAM,LPARAM);
	virtual void DoStop();

	virtual bool Register(
		HWND receiver_handle,bool bout_of_bounds = false,int node = -1);

	virtual bool RegisterWatch(HWND watcher_handle, int node = -1);
	virtual void SendToWatch(int bytes, char*, int node = -1);
	virtual void SetLocalRegisteredMessageValues();
	virtual bool SetUpPort(char *);

	virtual bool SetUpPort(char *szPort, unsigned BaudRate, unsigned DataBits, 
							unsigned StopBits, char* szParity, bool bLongBreak);

	virtual int Transmit(int bytes,char *,int node,bool Quiet=false);
	virtual bool UnRegister(int node = -1);
	virtual bool UnRegisterWatch(int node = -1);
	virtual void WatchDogSend();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComGrand)
	public:
	virtual int ExitInstance();
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

protected:

	// Generated message map functions
	//{{AFX_MSG(CComGrand)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // COMGRAND_H
