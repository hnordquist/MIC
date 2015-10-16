#if !defined(AFX_SCHEDULE_H__9DE89E24_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_)
#define AFX_SCHEDULE_H__9DE89E24_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Schedule.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSchedule dialog

class CSchedule : public CDialog
{
// Construction
public:
	CSchedule(CWnd* pParent = NULL);   // standard constructor
	void Setup(int,int,int,int,int,int,int,int);
// Dialog Data
	//{{AFX_DATA(CSchedule)
	enum { IDD = IDD_DIALOG_SCHEDULE };
	CEdit	m_pEditYear;
	CEdit	m_pEditSeconds;
	CEdit	m_pEditMonth;
	CEdit	m_pEditMinutes;
	CEdit	m_pEditHours;
	CEdit	m_pEditDay;
	CListBox	m_pSchedule;
	CListBox	m_pWeekDay;
	CSpinButtonCtrl	m_pSpinYear;
	CSpinButtonCtrl	m_pSpinSeconds;
	CSpinButtonCtrl	m_pSpinMonth;
	CSpinButtonCtrl	m_pSpinMinutes;
	CSpinButtonCtrl	m_pSpinHours;
	CSpinButtonCtrl	m_pSpinDay;
	CString	m_cWeekDay;
	CString	m_cSchedule;
	int		m_iDay;
	int		m_iHours;
	int		m_iMinutes;
	int		m_iMonth;
	int		m_iSeconds;
	int		m_iYear;
	//}}AFX_DATA

	int m_dSched;
	int m_dWeekd;
	int m_dYear;
	int m_dMonth;
	int m_dDay;
	int m_dHours;
	int m_dMinutes;
	int m_dSeconds;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSchedule)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSchedule)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListSchedule();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULE_H__9DE89E24_D7E4_11D3_8FB5_006008A70A6A__INCLUDED_)
