//DSPECInst.h

#ifndef DSPECINST_H
#define DSPECINST_H

#include "COM_Utilities.h"
#include "DSPECDataStat.h"
#include "DSPECModParam.h"
#include "DSPECSafePS.h"
#include "DSPECLegend.h"
#include "DSPECSummary.h"
#include "DSPECProppageSetupData.h"
#include "ORTECSpectrum.h"
#include "DSPECStrings.h"

#include "ISO.h"                 //base class
#include "../Interfaces/ICSO.h"  //the communications component interface

#include <commctrl.h>

#define DSPEC_DISCOVERY_TYPE "DSPEC"

/////////////////////////////////////////////////////////////////////////////
// CDSPECInst window

class CDSPECInst : public CISO
{
	//MIC_CB_028 to MIC 1.907 PJM 1/19/05 - added these friend classes
	friend class CDSPECSafePropertySheet;
	friend class CDSPECDataStat;
	friend class CDSPECModparam;

// Construction
public:
	CDSPECInst();
	CDSPECInst(CWnd* Parent, char *inifile);

	void FinishConstruction();

// Attributes
public:

	bool CollectParametersForNewISO(CString& csNameOut);
	bool CreateInstrument(void);
	void ShowInstrument(bool);
	void PauseInstrument(bool);
	bool UpdateParams(bool);
	void UpdateStrings();
	void DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	void IssuePrint(FILE* outfile);
	void Reset();
	void SetLocalRegisteredMessageValues();
	bool UnRegInstrument();
private:
	void AdjustShutdownAndStartupBooleans();
	bool DwellSettingIsShort();
	bool DwellSettingIsLong();
	void GetListsFromInstrument();
	void StartNewShortDwell(bool resetmemory);
	void TransmitAndUpdateSummary(char *command);	
	void AcknowledgeInputFromCSO();
	void Query_CSO();
	void RecoverFromFailure();
	void SendSetupCommandsToDSPEC();  //MIC 1.907a
	void KillAllTimers();

	// Grouped commands for UpdateParams() processing
	void SendShapeCommandsToDSPEC(bool resetting);
	void SendStandardGainCommandsToDSPEC(bool resetting);
	void SendStabilizationCommandsToDSPEC(bool resetting);
	void SendHVCommandsToDSPEC(bool resetting);
	void SendPoleZeroCommandsToDSPEC(bool resetting);
	void SendOtherCommandsToDSPEC(bool resetting);

	bool m_bIsPaused;
	bool m_bRealtimeWatcherDataQuiet;
	bool m_bCommsFailure;
	bool m_bHVVoltageLevelFailure;
	bool m_bHVShutdownFailure;

	COleDateTime m_cLatestReportedColumnUpdate;

	CDSPECSummary			*m_pSummaryPage;
	CDSPECDataStat			*m_pDataStatus;
	CDSPECModParam			*m_pModParams;
	CDSPECProppageSetupData	*m_pSetupData;
	CLegend					*m_pLegend;
	CDSPECSafePropertySheet	*m_pPropertySheet;

	COleDateTime			m_pNow;
	DSPEC_PARAMETERS		m_SentParameters;
	char					m_cCurrent_Tag;
	CWnd					*m_pParent;

	DWELL_TYPE				m_eDwellType;

	int	 m_iCycleInt_ms;			//for TIMER_TICK
	int	 m_iHVFail;
	int	 m_iLongdwell_sec;
//	int	 m_iPauseMax_ms;			//for TIMER_PAUSE	//dgp 18 Oct 2006
	int	 m_iPreviousDay;
	int	 m_iRealtimedwell_sec;
	int	 m_iSecToGo;             //Until End Of Dwell
	int	 m_iShortdwell_sec;

	int	 m_iSlowReset_sec;
	int	 m_iSlowResetCount_sec;

	int	 m_iShortTermTimeouts;
	int	 m_iShortTermTimeoutRunningCounter;

	int	 m_iStatusInt_sec;
	int	 m_iTickToStatus;

	ULONG m_ulThreshold;

	int	 m_iCommFail;  //timeouts allowed before hard fail
	int	 m_iTimeOut;   //length of a timeout (seconds)

	int m_iSecondsElapsedSinceReceivingData;

	int	 m_ihvvolth;
	int	 m_ihvvoltl;

///////////////////////////////////////////////////
//
//              STATE DEFINITIONS
//
///////////////////////////////////////////////////
enum DSPEC_STATE
{
	INITIALIZING_1000                        = 1000,
	FIRST_NON_STARTUP_STATE_1100             = 1100,

	WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000 = 2000,
	EXPECTING_SHORT_DWELL_SPECTRUM_DATA_2010 = 2010,

	WAITING_WITHIN_LONG_DWELL_INTERVAL_3000  = 3000,
	EXPECTING_LONG_DWELL_SPECTRUM_DATA_3010  = 3010,

	EXPECTING_MANUAL_WRITE_SPECTRUM_DATA_4000 = 4000,

	SHORT_TERM_HARD_FAIL_9000          = 9000,	//9000 and above are "in shutdown"
	LONG_TERM_HARD_FAIL_9500           = 9500,	// Doing Slow reset
	LOCKED_UP_9900                     = 9900
};

	DSPEC_STATE	m_dCurrent_State;
	DSPEC_STATE m_dSave_State;
	void SetStateAndButtonColor(DSPEC_STATE ds);
/////////////////////////////////////////
/////////////////////////////////////////

#ifdef ALLOWTRIGGERHOLDOFF
//	void ReadExclusion();
//	void WriteExclusion();
	typedef struct EXCLUSIONRECTAG{
		unsigned int uiStartTime;
		unsigned int uiEndTime;
	} EXCLUSIONREC;
	CList< EXCLUSIONREC, EXCLUSIONREC > m_ExclusionList;
#endif


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPECInst)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDSPECInst();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDSPECInst)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRequestSettingsUpdates(WPARAM, LPARAM);
	afx_msg void OnUpdateParams(WPARAM, LPARAM);
	afx_msg void OnDesiredViceCurrentMismatch(WPARAM,LPARAM);
	afx_msg void OnClearAndRestartSpectrum(WPARAM,LPARAM);
	afx_msg void OnManualWriteToCHNFile(WPARAM,LPARAM);
	afx_msg void OnHVVoltageLevelFail(WPARAM,LPARAM);
	afx_msg void OnHVVoltageLevelRecover(WPARAM,LPARAM);
	afx_msg void OnHVShutdown(WPARAM,LPARAM);
	afx_msg void OnHVShutdownRecover(WPARAM,LPARAM);
	afx_msg void OnReceive(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // DSPECINST_H
