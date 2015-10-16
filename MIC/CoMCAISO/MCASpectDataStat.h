#ifndef MCASPECDATASTAT_H
#define MCASPECDATASTAT_H

// MCASpecDataStat.h

#include "MCASafePS.h"
#include "MCAXYCtrl.h"
//#include "MICStdSpc.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CMCASPECDATASTAT_HDataStat dialog

class CMCASpecDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCASpecDataStat)

// Construction
public:
	CMCASpecDataStat(UINT id, CISO *parent);
	CMCASpecDataStat();
	~CMCASpecDataStat();

	void SetReceivingSpectraName(LPCSTR szString);
	void AddNewSpectraName(LPCSTR szString);

	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CMCAPropertySheet*);
	COLORREF StaticColors[IDC_MCASPECDATA_LAST-IDC_MCASPECDATA_FIRST+1];
	void IssuePrint(FILE *outfile);

//	bool ProcessSpectrumBufferForRealTimeGraphic(CMICStdSpc *pspec);
	bool ProcessSpectrumBufferForRealTimeGraphic(LONG *buf, UINT uiChans);
	int m_iShowSpectra;
	CISO *m_pParent;

//	void SetInhibitRealtimeDataButton(bool paused);

	CMCAXYCtrl   m_XYCtrl;

	// Dialog Data
	//{{AFX_DATA(CMCASpecDataStat)
	enum { IDD = IDD_MCASPECSTATUS_PAGE };
	CListCtrl	m_cListHistory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MCASpecCDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bBlockIncoming;

	CMCAPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CMCASpecDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowSpectra();
	afx_msg void OnButtonRefresh();
	afx_msg void OnClickListSpectra(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSpectra(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // MCASPECDATASTAT_H

