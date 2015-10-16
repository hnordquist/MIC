#if !defined(AFX_Serial_H__9A076211_A17B_11D1_B39F_006008A70A6A__INCLUDED_)
#define AFX_Serial_H__9A076211_A17B_11D1_B39F_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Serial.h : header file
//

#define IDC_COM_START                   10000
#define IDC_RECEIVE                     10100
#define IDC_RECEIVEA					10101
#define IDC_RECEIVEB					10102
#define IDC_RECEIVEC					10103
#define IDC_RECEIVED					10104
#define IDC_CREATE_COMM                 10120
#define IDC_DELETE_COMM                 10130
#define IDC_COM_STOP                    11111
#define ISZSTRING						-1
/////////////////////////////////////////////////////////////////////////////
// CSerial thread
class CSerial : public CWinThread
{
	DECLARE_DYNCREATE(CSerial)
protected:
	CSerial();           // protected constructor used by dynamic creation
	OVERLAPPED m_Overlap;
	char	m_pPort[16];
	int	m_dBaudRate;
	int	m_dDataBits;
	int	m_dStopBits;
	bool	m_bDone;
	char	m_pParity[16];
	CWnd*   m_pReceiver;
	CWnd*	m_pWatch;
	HANDLE	m_hComm;
	DCB *lpdcb;
	LPCOMMPROP lpCommProp;
// Attributes
public:
	virtual void DoStart(WPARAM,LPARAM);
	virtual void DoShutdown(WPARAM,LPARAM);
	virtual void DoStop();
	virtual unsigned long Transmit(int bytes,char *);
	virtual void DoSetMessage(int index);
private:
	int m_dIndex;

// Operations
public:
	virtual bool SetUpPort(char * pPort,int dBaudRate, int dDataBits,
		int  dStopBits, char* pParity);
	virtual bool Register(CWnd *);
	virtual bool UnRegister(int);
	virtual BOOL EscapeCommFunction(DWORD dwFunction);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerial)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CSerial();
protected:

	// Generated message map functions
	//{{AFX_MSG(CSerial)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Serial_H__9A076211_A17B_11D1_B39F_006008A70A6A__INCLUDED_)
