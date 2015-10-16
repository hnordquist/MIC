#ifndef EOSSINSTRUMENT_H
#define EOSSINSTRUMENT_H

//CEOSSInstrument Class Header File
#include "depth.h"
#include "EOSSSafePS.h"
#include "EOSSDataStat.h"
#include "EOSSModParam.h"
#include "EOSSLegend.h"
#include "EOSSSummary.h"
#include "ISO.h"				//base class
#include "../Interfaces/ICSO.h"	//the communications component interface

#define EOSS_DISCOVERY_TYPE "EOSS"
#define MAXEOSSES 64
// jfl the above value conflicts with UI limitation of '40', hard-coded by dp several places (now CEditEOSSList::maxEOSSCount)

/////////////////////////////////////////////////////////////////////////////
// CEOSSInstrument window

class CEOSSInstrument : public CISO
{
friend class CEOSSModParam;
friend class CEOSSPropertySheet;

// Construction
public:
	CEOSSInstrument();
	CEOSSInstrument(CWnd* Parent, char *inifile);
	virtual ~CEOSSInstrument();
	void FinishConstruction();

// Attributes
public:
	bool CollectParametersForNewISO(CString& csNameOut);

	bool CreateInstrument(void);
	bool UnRegInstrument();
	void ShowInstrument(bool);
	void PauseInstrument(bool);
	bool UpdateParams(bool);
	void DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	void IssuePrint(FILE* outfile);
	void Reset();
	void SetLocalRegisteredMessageValues();
	void UpdateStrings();
	void PostToWatch(char *pMessage);
	bool ReadRegistry();
	int  ValidateSeal(char* szItem,char*szFinal,unsigned long ulEOSSIndex);
	void PopulateArrays();
	void InitArrays();
	int  GetString(char *szItem, unsigned char *pucColorFlag, CString *pcString, HKEY hKey);
	void EossToSystemTime(SYSTEMTIME * pSystemTime, EOSSTIME * pEossTime);
	bool Unattended(unsigned short usCode);
	void UnMakeID(unsigned short Id, char * szOperator);
	CString GetEossMessage(unsigned short usCode);
	bool RegChanged();
	void KillEOSS();
	void CleanRegistry();
	CString CleanString(CString cString);
	bool EditEOSSList();
	void OnAck();
	void ResetReaderCount();
	void SetDepthForManualOveride();
	QueryLevel GetLatestDepth();

	unsigned long m_ulBatteryCrit;
	unsigned long m_ulBatteryWarn;
	unsigned long m_ulEBatteryCrit;
	unsigned long m_ulEBatteryWarn;

	bool m_bReSchedule;
	bool m_bRunNow;
	bool m_bRemoveOld;
	bool m_bUpdateDataDisplay;
	bool m_bPrimed;
	bool m_bEditActive;
	COleDateTime m_cNextScheduled;
	QueryLevel m_QueryDepth; // used only with manual read step

	int m_iColorFlagS;
	int m_iColorFlagG;

	FILE *m_file; 
	char m_szFileName1[MAX_PATH];
	char m_szFileName2[MAX_PATH];
	char m_szShortFileName1[MAX_PATH];
	char m_szShortFileName2[MAX_PATH];
	char m_szFormat[MAX_PATH];


	unsigned int EOSSes[MAXEOSSES];
	struct tagGENERALB {
		unsigned char ucAll;
		CString cAll;
		unsigned char ucCOMPORT;
		CString cCOMPORT;
		unsigned char ucPATH;
		CString cPath;
		unsigned char ucFORMAT;
		CString cFORMAT;
		unsigned char ucINSTRUMENT_ID;
		CString cINSTRUMENT_ID;
		unsigned char ucLATEST_READ;
		CString cLATEST_READ;
		unsigned char ucSPECIFIED_EOSSes;
		CString cSPECIFIED_EOSSes;
		unsigned char ucLATEST_RESULTS;
		CString cLATEST_RESULTS;
		unsigned char ucREADER_VERSION;
		CString cREADER_VERSION;
		unsigned char bTimeOffset;
		CString cTimeOffset;
		//unsigned char ucQueryDepth;
		//CString cQUERY_DEPTH;
	} sGeneral;

	struct tagSTATUSB {
		unsigned int uiEOSSSN;
		unsigned char ucInstID;
		CString cInstID;
		unsigned char ucRedCount;
		unsigned char ucYellowCount;
		unsigned char ucBlueCount;
		CString cAll;
		unsigned char ucLatestRead;
		CString cLatestRead;
		unsigned char ucLatestResults;
		CString cLatestResults;
		unsigned char ucKeyInfo;
		CString cKeyInfo;
		unsigned char ucVersion;
		CString cVersion;
		unsigned char ucStatus;
		CString cStatus;
		unsigned char ucLogInfo;
		CString cLogInfo;
		unsigned char ucSealLog;
		CString cSealLog;
		unsigned char ucSOHLog;
		CString cSOHLog;
		unsigned char ucSystemLog;
		CString cSystemLog;
	} sStatus[MAXEOSSES];

	CEOSSSummary    *m_pSummaryPage;
	CEOSSDataStat	*m_pDataStatus;
	CEOSSModParam	*m_pModParams;
	CLegend			*m_pLegend;

protected:
	typedef enum {
			eStartSchtasksCheckWrite = 1000, eSchtasksCheckWaitThenRead = 1500, 
			eSchtasksCompare = 2000, 
			eSchtasksDelete = 2500, eSchtasksDeleteWaitOnCompletion = 2600,
			eSchtasksRescheduling = 3000, eSchtasksReschedulingWaitOnCompletion = 3100,
			eGatherRegistryResults = 4000, 
			eRunDirectReaderProcess = 5000, eRunWaitOnDirectReaderProcess = 5500, 
			eButtonAndReaderProcessStatusCheck = 6000, 
			eRunTimerIntervalReaderProcess = 6317, 
			eWaitOnKillEOSS = 6500, 
			eClearRegistry = 7000,
			ePauseState = 9000} EOSSStates; 

private:

	

	CEOSSPropertySheet	*m_pPropertySheet;

	CWnd		*m_pReceiver;
	COleDateTime m_pToday;
	bool		m_bIsPaused;
	bool		m_bWasPaused;
	int			m_dSave_State;
	EOSSStates			m_dCurrent_State;

	CWnd*		m_pParent;
	int			m_dCycleInt;
	int			m_dTick_Count;
	int			m_dTickToInquire;
	int			m_dInquireInt;
	bool		m_bdoingBBM;
	int			m_dPreviousDay;
	COleDateTime	m_pNow;
	char		m_szLatestRead[32];

	bool		m_bTokLibTrigger;

	bool		m_XPOrMilTime;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEOSSInstrument)
	//}}AFX_VIRTUAL

protected:
	void BuildStatusStrings();
	DWORD GetEOSSIndex(char* id) const;
	// Comm and HW status management
	bool QueryLatestResultsForHardFail();
	bool m_bHardFailResults, m_bHardFail, m_bHWNotified;
	UINT m_uReaderCycles, m_uReaderCheckCount, m_uReaderCompletionCount;
	UINT m_uHardFailCount, m_uHardFailCheckCycles;
	UINT m_uFullDataInterval;


	// Generated message map functions
	//{{AFX_MSG(CEOSSInstrument)
	afx_msg void OnTimer(UINT nIDEOSS);
	afx_msg LRESULT OnReceive(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EOSSINSTRUMENT_H