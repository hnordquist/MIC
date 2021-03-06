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
// WatchComDlg.h : header file
//
#include "serial.h"

#if !defined(AFX_WATCHCOMDLG_H__335375D6_DAF8_11D2_B4B7_006008A70A6A__INCLUDED_)
#define AFX_WATCHCOMDLG_H__335375D6_DAF8_11D2_B4B7_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DataWatchDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CWatchComDlg dialog
#define RESIZETIMER 1000

class CWatchComDlg : public CDialog
{
// Construction
public:
	CWatchComDlg(CWnd* pParent = NULL);	// standard constructor
	~CWatchComDlg();
// Dialog Data
	//{{AFX_DATA(CWatchComDlg)
	enum { IDD = IDD_WATCHCOM_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchComDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void GraphIsRunning(int i);
	void GraphIsDestroyed(int i);

protected:
	void ResizeChildren();
	CRect m_WindowRect;
	HICON m_hIcon;
	HICON m_hButtonIcon;
	CSerial *ComA;
	CSerial *ComB;
	CSerial *ComC;
	CSerial *ComD;

	CString m_cPrevious;

	WORD	m_wVer;
	WSADATA m_WSAData;
	int		m_iRc;
	int		m_iSeq;

	struct SACQREC {
		float NeutronChA;
		float NeutronChB;
		float NeutronChC;
		float NeutronChD;
		float GammaCh1;
		float GammaCh2;
		float GammaCh3;
		float GammaCh4;
	} sAcquireRecordA,sAcquireRecordB,sAcquireRecordC,sAcquireRecordD;

	SOCKET m_SocketA;
	SOCKET m_SocketB;
	SOCKET m_SocketC;
	SOCKET m_SocketD;

	struct	sockaddr_in m_sAddrA;
	struct	sockaddr_in m_sAddrB;
	struct	sockaddr_in m_sAddrC;
	struct	sockaddr_in m_sAddrD;

	char m_szIniFile[_MAX_PATH];

	bool m_bPauseLog;

	char m_szANet[32];
	char m_szBNet[32];
	char m_szCNet[32];
	char m_szDNet[32];

	char m_szAPort[16];
	char m_szBPort[16];
	char m_szCPort[16];
	char m_szDPort[16];

	unsigned int m_uiANetPort;
	unsigned int m_uiBNetPort;
	unsigned int m_uiCNetPort;
	unsigned int m_uiDNetPort;

	char m_szAParity[16];
	char m_szBParity[16];
	char m_szCParity[16];
	char m_szDParity[16];

	int m_dABaudRate;
	int m_dBBaudRate;
	int m_dCBaudRate;
	int m_dDBaudRate;

	int m_dADataBits;
	int m_dBDataBits;
	int m_dCDataBits;
	int m_dDDataBits;

	int m_dAStopBits;
	int m_dBStopBits;
	int m_dCStopBits;
	int m_dDStopBits;

	char m_szADestination[_MAX_PATH];
	char m_szBDestination[_MAX_PATH];
	char m_szCDestination[_MAX_PATH];
	char m_szDDestination[_MAX_PATH];

	char m_szAFileName[_MAX_PATH];
	char m_szBFileName[_MAX_PATH];
	char m_szCFileName[_MAX_PATH];
	char m_szDFileName[_MAX_PATH];

	bool m_bANewLine;
	bool m_bBNewLine;
	bool m_bCNewLine;
	bool m_bDNewLine;

	int m_iPreviousDay;

	char m_cAPreviousChar;
	char m_cBPreviousChar;
	char m_cCPreviousChar;
	char m_cDPreviousChar;

	COLORREF m_iAColor;
	COLORREF m_iBColor;
	COLORREF m_iCColor;
	COLORREF m_iDColor;

	CDataWatchDlg* m_pDataWatchDlg1;
	bool m_bDataWatchDlg1Open;

	CDataWatchDlg* m_pDataWatchDlg2;
	bool m_bDataWatchDlg2Open;

	CDataWatchDlg* m_pDataWatchDlg3;
	bool m_bDataWatchDlg3Open;

	CDataWatchDlg* m_pDataWatchDlg4;
	bool m_bDataWatchDlg4Open;

	// Generated message map functions
	//{{AFX_MSG(CWatchComDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReceive(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveA(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveB(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveC(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveD(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveNA(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveNB(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveNC(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveND(WPARAM WParam, LPARAM LParam);
	afx_msg void OnButtonClear();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonA();
	afx_msg void OnButtonB();
	afx_msg void OnButtonC();
	afx_msg void OnButtonD();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnStaticComa();
	afx_msg void OnStaticComc();
	afx_msg void OnStaticComb();
	afx_msg void OnStaticComd();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnCheck1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHCOMDLG_H__335375D6_DAF8_11D2_B4B7_006008A70A6A__INCLUDED_)
