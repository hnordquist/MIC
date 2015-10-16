

#ifndef DSPECDATASTAT_H
#define DSPECDATASTAT_H

// DSPECDataStat.h

#include "DSPECSafePS.h"
#include "XYCtrl.h"
#include "ORTECSpectrum.h"
#include "resource.h"

class CDSPECInst;
/////////////////////////////////////////////////////////////////////////////
// CDSPECDATASTAT_H DataStat dialog

class CDSPECDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CDSPECDataStat)

// Construction
public:
	CDSPECDataStat(UINT id, CWnd* pParent = NULL, char *inifile = NULL);
	CDSPECDataStat();
	~CDSPECDataStat();

	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CDSPECSafePropertySheet*);
	void SetGainConversion(UINT gc);
	COLORREF StaticColors[IDC_DSPECDATASTAT_LAST-IDC_DSPECDATASTAT_FIRST+1];
	void IssuePrint(FILE *outfile);

	bool ProcessSpectrumBufferForRealTimeGraphic(CORTECSpectrum *pspec);
	void SetInhibitRealtimeDataButton(bool paused);
	int m_iShowSpectra;
	UINT m_uGainConversion;
	char m_szIniFile[_MAX_PATH];
	CDSPECInst *m_pParent;

	CXYCtrl   m_XYCtrl;

// Dialog Data
	//{{AFX_DATA(CDSPECDataStat)
	enum { IDD = IDD_DSPECDATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(DSPECCDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDSPECSafePropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CDSPECDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnClearAndRestart();
	afx_msg void OnWriteCHNFile();
	afx_msg void OnInhibitRealtimeData();
	afx_msg void OnShowSpectra();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // DSPECDATASTAT_H

