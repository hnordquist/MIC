/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-040. This software was exported from the United States in accordance with the Export
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
