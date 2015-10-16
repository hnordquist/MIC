// CSO.h
 
#ifndef CSO_H
#define CSO_H

class CCSO : public CWinThread
{
// Construction
public:

	CCSO();
	virtual ~CCSO();

// Attributes
public:

	//
	// Pure virtual
	//
	virtual bool CollectParametersForNewCSO(CString& csNameOut) = 0;

	virtual void DoReset(WPARAM,LPARAM) = 0;
	virtual void DoStart(WPARAM,LPARAM) = 0;
	virtual void DoShutdown(WPARAM,LPARAM) = 0;
	virtual void DoStop() = 0;

	virtual bool Register(
		HWND receiver_handle,bool bout_of_bounds = false,int node = -1) = 0;

	virtual bool RegisterWatch(HWND watcher_handle,int node = -1) = 0;
	virtual void SendToWatch(int bytes, char*, int node = -1) = 0;

	virtual void SetLocalRegisteredMessageValues() = 0;
	virtual bool SetUpPort(char *) = 0;

	virtual bool SetUpPort(char *szPort, unsigned BaudRate, unsigned DataBits, 
							unsigned StopBits, char* szParity, bool bLongBreak) = 0;

	virtual int  Transmit(int bytes,char *,int node = -1, bool Quiet=false) = 0;
	virtual bool UnRegister(int node = -1) = 0;
	virtual bool UnRegisterWatch(int node = -1) = 0;

	//
	// Non-pure virtual - have implementations in CSO.cpp
	//
	virtual void SetWindowsRegisteredMessageStrings(
		ULONG ulRECEIVE, 
		ULONG ulRECEIVE_OOB,
		ULONG ulIDC_COM_START,
		ULONG ulIDC_COM_STOP,
		ULONG ulIDC_COM_RESET);

	virtual void SetThresholdValue(UINT u);

	virtual CString *GetLastError();
	virtual char *GetName();
	virtual char *GetType();
	virtual void PutIniFileName(char *inifile);
	virtual void WatchDogSend();
	virtual void WatchDogOk(bool set);
	virtual void PutDialogCloseTime(int milliseconds);

	//
	// CSO Base class variables
	//
	bool	m_bWatchDogOk;
	char    m_szIniFile[_MAX_PATH];
	CString m_csLastError;
	int     m_iDlgClose;
	char    m_pType[32];
	char    m_pName[32];

	HWND    m_hReceiver;
	HWND    m_hWatcher;
	HWND    m_hOOB;

	UINT    m_uIDC_RECEIVE;
	UINT    m_uIDC_RECEIVE_OOB;
	UINT    m_uIDC_COM_START;
	UINT    m_uIDC_COM_STOP;
	UINT    m_uIDC_COM_RESET;
	
	CString	m_csVersion;
	CString	m_csNameVersionDate;
};

#endif //CSO_H