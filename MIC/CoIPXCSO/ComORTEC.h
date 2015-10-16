#ifndef COMORTEC_H
#define COMORTEC_H

// ComORTEC.h

#include "mcbcio32/mcbcio32.h"
#include "ORTECMcb.h"
#include "ORTECSpectrum.h"
#include "CSO.h"
#include <Afxmt.h>

#define IPX_DISCOVERY_TYPE "ORTECIPX"

// CComORTEC thread

class CComORTEC : public CCSO
{
	DECLARE_DYNCREATE(CComORTEC)

protected:
	CComORTEC();           // protected constructor used by dynamic creation

public:
	CORTECSpectrum	*m_pspec;
	CORTECMcb		*m_pmcb;
	int				m_dNode;  //Not used for ORTEC IPX.  Is set to -1 in the constructor.

	UINT m_uRawDataThresholdValue;

	virtual bool CollectParametersForNewCSO(CString& csNameOut);
	virtual void DoReset(WPARAM,LPARAM);
	virtual void DoStart(WPARAM,LPARAM);
	virtual void DoShutdown(WPARAM,LPARAM);
	virtual void DoStop();

	//<<QA>> These weren't in the orginal MIC_GM version.
	virtual bool Register(
		HWND receiver_handle,bool bout_of_bounds = false,int node = -1);
	virtual bool RegisterWatch(HWND watcher_handle,int node = -1);
	virtual bool UnRegister(int node = -1);
	virtual bool UnRegisterWatch(int node = -1);
	virtual void SendToWatch(int Length, char* Output, int /*node*/);
	//end <<QA>>

	virtual void SetLocalRegisteredMessageValues();
	virtual int Transmit(int bytes,char *,int node,bool Quiet=false);
	virtual bool SetUpPort(char *);
	virtual bool SetUpPort(char*,unsigned,unsigned,unsigned,char*,bool); //not used

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComORTEC)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CComORTEC();

	void	DoGetSpectrumDataForReceiver(WPARAM, LPARAM);
	void	DoSendRawCommandDataToMCA(WPARAM, LPARAM);
	void	CommandAndResponse(char *cmd, char *response); //for the CSO to forward to the cracker
	void	SetThresholdValue(UINT u);
	void	InitMCB(WPARAM, LPARAM);

protected:
	char m_szLastSend[64];
	int  m_iCMcbSuccess;

	CEvent *m_pEvent;

	// Generated message map functions
	//{{AFX_MSG(CComORTEC)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //COMORTEC_H