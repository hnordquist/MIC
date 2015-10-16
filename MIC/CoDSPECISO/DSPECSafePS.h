// DSPECSafePS.h

#ifndef DSPECSAFEPS_H
#define DSPECSAFEPS_H

#include "CCButton.h"
#include "ORTECSpectrum.h"
#include "DSPECParameters.h"
#include "JulianT.h"

class CDSPECInst;
/////////////////////////////////////////////////////////////////////////////
// CDSPECSafePropertySheet
class CDSPECSafePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDSPECSafePropertySheet)

// Construction
public:
	CDSPECSafePropertySheet(
		UINT nIDCaption, 
		CDSPECInst* pParentWnd, 
		char *inifile);

	CDSPECSafePropertySheet(
		LPCTSTR pszCaption, 
		CDSPECInst* pParentWnd, 
		char *inifile);

	void FinishInstance();

	COLORREF SetButtonColor(COLORREF Color);

	void WriteToFile(
		int ToWhom, 
		int Type, 
		CJulianTime* Time = NULL, 
		const char* Message = NULL);

	void WriteSetupFailureToFile(int number, char *msg);

	bool WriteToCHNFile(CORTECSpectrum *pspec, char tag);
	void MakeMyPath(const char *);

	void SetLimits(int hvvolth, int hvvoltl);

	void CleanUpString(
		char *destination, 
		char *source, 
		int destination_length,
		int digits_after_decimal = -1);

	void SetTabColor(int Tab, COLORREF Color);

	void IssuePrint(FILE* outfile);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPECSafePropertySheet)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDSPECSafePropertySheet();
	CColorButton	*m_pCloseButton;
	CButton			*m_pPauseButton;
	CButton			*m_pPrintButton;
	CButton			*m_pAckFlagsButton;
	CButton			*m_pToClose;
	int				m_dToClose;
	CDSPECInst		*m_pParent;
	CImageList		*m_pImageList;

	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentCHNFileName[_MAX_PATH];
	char			m_szIniFile[_MAX_PATH];

	virtual void	SetToClose(int set);
	virtual void	UpdateSummary(char *);
	void			UpdateDataStatus();

	void BuildFileName(CString &cResult,CString cFormat,CString cPath,
		CString cType,CString cID,int iYear,int iMonth,int iDay,
		int iHour,int iMinute,int iSecond,
		CString cSuffix);

	DSPEC_PARAMETERS m_ReportedParameters;

protected:
	int Data_Tab;
	int InstSetup_Tab;

	UINT    m_uLocalNonzero_channels;
	UINT    m_uLocalChannels_above_threshold;

	void CurrentSetParam(char*szIniName,char*szValue,bool bMatch);

	// Limits variables
	int m_dHVVoltH;
	int m_dHVVoltL;

	char  m_pCaption[256];

	//{{AFX_MSG(CDSPECSafePropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCloseButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPauseButton();
	afx_msg void OnAckButton();
	afx_msg void OnPrintButton();
	afx_msg void OnToCloseButton();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnReceive(WPARAM WParam, LPARAM LParam);
	afx_msg void OnReceiveCommandResponsePair(WPARAM command, LPARAM response);
	afx_msg void OnIPXMCBError(WPARAM WParam, LPARAM LParam);
	afx_msg void OnExtendClose(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



};

#endif // DSPECSAFEPS_H
