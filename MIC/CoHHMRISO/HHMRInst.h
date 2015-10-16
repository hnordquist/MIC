// HHMRInst.h

#ifndef HHMRINST_H
#define HHMRINST_H

//CHHMRInstrument Class Header File

#include "COM_Utilities.h"
#include "HHMRSafePS.h"
#include "HHMRSummary.h"
#include "HHMRDataStat.h"
#include "HHMRModParam.h"
#include "HHMRConfig2.h"  //Camera Setup
#include "HHMRLegend.h"

#include "ParallelPort.h"

#include "ISO.h"                 //base class
#include "../Interfaces/ICSO.h"  //the communications component interface

#define HHMR_DISCOVERY_TYPE "HHMR"

/////////////////////////////////////////////////////////////////////////////
// CInstrument window

class CHHMRInst : public CISO
{
friend class CHHMRModParam;
friend class CHHMRPropertySheet;
friend class CHHMRConfig2;

// Construction
public:
	CHHMRInst(CWnd* Parent, char *inifile);
	CHHMRInst();
	virtual ~CHHMRInst();
	void FinishConstruction();

// Attributes
public:
	bool  CollectParametersForNewISO(CString& csNameOut);
	bool  CreateInstrument(void);
	void  ShowInstrument(bool);
	void  PauseInstrument(bool);
	bool  UpdateParams(bool);
	void  UpdateStrings();
	void  DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	int	  GetCurrentBBM(){return 0;}
	void  IssuePrint(FILE* outfile);
	void  Reset();
	void SetLocalRegisteredMessageValues();
	void FlagTime(int,bool){;}  //there is no FlagTime for the HHMR

	bool SetupParallelPort(bool reseting);

	CHHMRSummary		*m_pSummaryPage;
	CHHMRDataStat		*m_pDataStatus;
	CHHMRModParam		*m_pModParams;
	CHHMRConfig2		*m_pCamera;
	CLegend				*m_pLegend;
	CHHMRPropertySheet	*m_pPropertySheet;

	bool		m_bIsPaused;//caution on using this var
	bool		m_bLogFilter;
	bool		m_bInDumpLast;
	int			m_dSlowReset;

private:

	int			m_dCurrent_State;
	int			CalculateDiff(SYSTEMTIME *Time1, SYSTEMTIME *Time2);
	bool		m_bCameraBlocked;

	CWnd*		m_pParent;
	unsigned int   m_dPrevHHMRTime;
	int			m_dCycleInt;
	int			m_dTick_Count;
	int			m_dTickToInquire;
	int			m_dInquireInt;
	int			m_dInquireIntAdjust;
	int			m_dTransmitXCount;
	int			m_dCommFail;
	int			m_dTimeOut;
	int			m_dHighVoltH;
	int			m_dHighVoltL;

	double		m_lSigmaThresh;
	double		m_lRealThresh;
	double		m_lTotalThresh;
	double		m_lTotal1Thresh;
	double		m_lTotalLThresh;
	int			m_dMaxCompress;

	SYSTEMTIME  m_Then;

	char		m_cCameraRealorTotals[4];
	COleDateTimeSpan m_pCameraHoldOff;
	COleDateTimeSpan m_pCameraSuppress;
	COleDateTime m_pSuppressUntil;
	SYSTEMTIME	m_sStartTime;

	bool		m_bCameraHoldingOff;

	int			m_dCountTimeUnit;
//	int			m_dCountTimeExp;
	int			m_dHighV;
	int			m_dGateWidth;
	int			m_dPreDelay;

	int			m_dRetransmit;
	int			m_dLongBreak;
	bool		m_bInHardFail;
	bool		m_bDriveToZero;

	ParallelPort	m_cParallelPort;
	int				m_dParallelPortPin;
//	int			m_dInquireIntOffset;
	int			m_iCycleCount;
	COleDateTime m_cStart;
	COleDateTime m_cFinish;
	COleDateTimeSpan m_cSpan;
	DWORD		m_dSpan;
	DWORD		m_dFinish;
	DWORD		m_dStart;
// Operations
public:
	void	SetCamera(bool);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHHMRInst)
	//}}AFX_VIRTUAL

// Implementation
public:


	// Generated message map functions
protected:
	//{{AFX_MSG(CHHMRInst)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnReceive(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif HHMRINST_H
