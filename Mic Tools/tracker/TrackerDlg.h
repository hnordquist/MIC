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
// TrackerDlg.h : header file
//

#if !defined(AFX_TRACKERDLG_H__156499A6_A4BC_11D2_B439_006008A70A6A__INCLUDED_)
#define AFX_TRACKERDLG_H__156499A6_A4BC_11D2_B439_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxdisp.h>
#include "CCButton.h"

#define RED		RGB(255,0,0)
#define PUSH_XML		"PUSH XML"

/////////////////////////////////////////////////////////////////////////////
// CTrackerDlg dialog
#define WM_WSARECV	(WM_USER+5)
#define MAX_BUTTONS 50
#define MAX_FORWARD 20
#define BUTTONWIDTH 175
#define BUTTONFLAGSIZE 36
#define BUTTONHEIGHT 70
#define BUTTONCOLUMNS 1
#define IPORT 1028
#define EXCLUDE_SIZE 10000
#define LOCATION_BASE 10000

#define BUTTONBASE 9000

typedef struct senddata {
	int BBM;
	int Index;
	int NumButtons;
	int	Base;
	COLORREF FaceColor;
	bool Flash;
	bool Flag;
	bool Bell;
	bool Time;
	bool Other;
	bool Disk;
	char FlagC;
	char BellC;
	char TimeC;
	char OtherC;
	char DiskC;
	char Title[32];
	char PreNum[30];
	char QuestC;
	bool Quest;
} SendDataType;

struct SREQUEST {
	char* pszFTPServer;
	char* pszFTPUserName;
	char* pszFTPPassword;
	char* pszFTPDir;
	int   iContext;
	char* pszPathNameToSend;
	char* pszNameOut;
	bool  bGotData;
	DWORD dwAttrs;
};

class CTrackerDlg : public CDialog
{
// Construction
public:
	CTrackerDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CTrackerDlg();
	char	m_cCommandLine[_MAX_PATH];
// Dialog Data
	//{{AFX_DATA(CTrackerDlg)
	enum { IDD = IDD_TRACKER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrackerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnInstrumentButton(UINT);
	HICON m_hIcon;
	CColorButton* Buttons[MAX_BUTTONS];
	void	Log(bool bFlash, char* szText);
//	int		FormatXML(char *szXML, char*szSource, int iFirst, int iLast, bool bAlarm=false);
	int     FormatXML(char * szXML, char* szSource, char* szMediaName, int iType, int iButtonIndex);

	void	Reorg();
	void	Reconfigure();
	afx_msg LRESULT OnTrigger(WPARAM,LPARAM);
	afx_msg LRESULT OnTriggerClr(WPARAM,LPARAM);
	WORD	m_wVer;
	WSADATA m_WSAData;
	SOCKET	 m_sd_dg;
	SOCKET	 m_sd_dg2;
	struct	sockaddr_in m_sAddr_Loc;
	struct	sockaddr_in m_sAddr_Rem;
	struct	sockaddr_in m_sAddr_Tmp;
	struct	sockaddr_in m_sAddr_Tmp2;
	CString	m_sForwardAddress[MAX_FORWARD];
	COleDateTime m_sForwardTime[MAX_FORWARD];
	COleDateTime m_sLastGoodMessageTime;
	int		m_dTop;
	int		m_dButtonWidth;
	int		m_dButtonHeight;
	int		m_dButtonFlagSize;
	int		m_dButtonsWide;
	int		m_dNumButtons;
	CRect	m_sInitialRect;
	CRect	m_sStartRect;
	bool	m_bMoved;
	bool	m_bPushXML;
	char	m_szIPAddress[32];
	bool	m_bAcked;
	CString m_MSName;
	char	m_szPathName[MAX_PATH];
	char	m_szPathNameOut[MAX_PATH];
	char	m_szDir[MAX_PATH];
	char	m_szConvertPath[MAX_PATH];
	char	m_szCameraLocation[64];
	bool	m_bTriggerExclude[EXCLUDE_SIZE];
	bool	m_bInSendImages;

public:
	char	m_cIniFile[_MAX_PATH];
	char	m_szHTMLName[_MAX_PATH];
	HANDLE	m_hMailSlot;
	int		m_dDefPort;
	char	m_szSlotName[64];
	char	m_cName[64];
	bool	m_bDoingMailSlot;
	bool	m_bDoHTML;
	bool	m_bAllowForward;
	bool	m_bToMIC;
	bool	m_bAllFlashing;
	int		m_dHTMLTime;
	int		m_dMSTime;
	char	m_cLogFileName[_MAX_PATH];
	BYTE	m_b1;
	BYTE	m_b2;
	BYTE	m_b3;
	BYTE	m_b4;
	bool	m_bDoAlarm;
	CString	m_cVERSION;


	// Generated message map functions
	//{{AFX_MSG(CTrackerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnReceive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTriggerExclude(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACKERDLG_H__156499A6_A4BC_11D2_B439_006008A70A6A__INCLUDED_)
