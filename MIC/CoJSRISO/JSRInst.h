// JSRInst.h

#ifndef JSRINST_H
#define JSRINST_H

//CJSRInstrument Class Header File

#include "COM_Utilities.h"
#include "JSRSafePS.h"
#include "JSRSummary.h"
#include "JSRDataStat.h"
#include "JSRModParam.h"
#include "JSRConfig2.h"  //Camera Setup
#include "JSRLegend.h"

#include "ParallelPort.h"

#include "ISO.h"                 //base class
#include "../Interfaces/ICSO.h"  //the communications component interface

#define JSR_DISCOVERY_TYPE "JSR"

/////////////////////////////////////////////////////////////////////////////
// CInstrument window

class CJSRInst : public CISO
{
friend class CJSRModParam;
friend class CJSRPropertySheet;
friend class CJSRConfig2;

// Construction
public:
	CJSRInst(CWnd* Parent, char *inifile);
	CJSRInst();
	virtual ~CJSRInst();
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
	void FlagTime(int,bool){;}  //there is no FlagTime for the JSR

	bool SetupParallelPort(bool reseting);

	CJSRSummary			*m_pSummaryPage;
	CJSRDataStat		*m_pDataStatus;
	CJSRModParam		*m_pModParams;
	CJSRConfig2			*m_pCamera;
	CLegend				*m_pLegend;
	CJSRPropertySheet	*m_pPropertySheet;

	bool		m_bIsPaused;//caution on using this var
	bool		m_bLogFilter;
	bool		m_bInDumpLast;
	int			m_dSlowReset;

private:

	int			m_dCurrent_State;
	int			CalculateDiff(SYSTEMTIME *Time1, SYSTEMTIME *Time2);
	bool		m_bCameraBlocked;

	CWnd*		m_pParent;
	unsigned int   m_dPrevJSRTime;
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
	int			m_dCountTimeExp;
	int			m_dHighV;
	int			m_dGateWidth;
	int			m_dPreDelay;

	int			m_dRetransmit;
	int			m_dLongBreak;
	bool		m_bInHardFail;
	bool		m_bDriveToZero;

	ParallelPort	m_cParallelPort;
	int				m_dParallelPortPin;

// Operations
public:
	void	SetCamera(bool);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJSRInst)
	//}}AFX_VIRTUAL

// Implementation
public:


	// Generated message map functions
protected:
	//{{AFX_MSG(CJSRInst)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnReceive(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif JSRINST_H
