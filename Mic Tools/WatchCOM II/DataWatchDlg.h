#if !defined(AFX_DATAWATCHDLG_H__E0C56560_F034_4831_925D_37911F828151__INCLUDED_)
#define AFX_DATAWATCHDLG_H__E0C56560_F034_4831_925D_37911F828151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataWatchDlg.h : header file
//
#include "chart.h"
//#include "JulianT.h"
#include "resource.h"
//#include "WatchComDlg.h"
#include <afxdisp.h>
/////////////////////////////////////////////////////////////////////////////
// CDataWatchDlg dialog

class CDataWatchDlg : public CDialog
{
// Construction

public:
	CDataWatchDlg(CWnd* pParent = NULL);   // standard constructor
	~CDataWatchDlg();
	void SetDlgTitle(CString cTitle, CWnd* pParent, int iIndex);
	void SetGraphTitles(CString cs1, CString cs2, CString cs3, CString cs4, CString cs5, CString cs6, CString cs7, CString cs8);
	void SetData(unsigned int uiTime, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, bool bDraw);

// Dialog Data
	//{{AFX_DATA(CDataWatchDlg)
	enum { IDD = IDD_DATAWATCH_DIALOG };
	CButton	m_cButtonExpand;
	CButton	m_cButtonContract;
	CButton	m_cLocked;
	CStatic	m_cStaticStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataWatchDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_cTitle;
	CString m_cs1, m_cs2, m_cs3, m_cs4, m_cs5, m_cs6, m_cs7, m_cs8;
	HICON m_hIcon;
	CWnd* m_pParent;
	void ResizeChildren(int iOversize);
	bool m_bConfig;
	int  m_iMyIndex;
	UINT m_uiLastTime;
	UINT m_uiLineIndex;
	bool m_bNewLine;
	CRect m_WindowRect;


	UINT m_uBaseTime;
	UINT m_uiCurrentHour;
	CFont m_VertFont;
//	ITEM_COLOR m_cRGBbk;
//	ITEM_COLOR m_cRGBfg;
	UINT m_uiMultiplier;
	COleDateTime m_cLastReadTime;

	CString cLabel1;
	CString cLabel2;
	CString cLabel3;
	CString cLabel4;
	CString cLabel5;
	CString cLabel6;
	CString cLabel7;
	CString cLabel8;

	CScrollChart< float > m_wnd1Chart;
	CScrollChart< float > m_wnd2Chart;
	CScrollChart< float > m_wnd3Chart;
	CScrollChart< float > m_wnd4Chart;
	CScrollChart< float > m_wnd5Chart;
	CScrollChart< float > m_wnd6Chart;
	CScrollChart< float > m_wnd7Chart;
	CScrollChart< float > m_wnd8Chart;

	int m_iChart1x;
	int m_iChart2x;
	int m_iChart3x;
	int m_iChart4x;
	int m_iChart5x;
	int m_iChart6x;
	int m_iChart7x;
	int m_iChart8x;

	int m_iOversize;

//	void SetStatusText();
	LRESULT OnNewV1Limits(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewH1Limits(WPARAM wParam, LPARAM lParam);

	LRESULT OnNewV2Limits(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewH2Limits(WPARAM wParam, LPARAM lParam);

	LRESULT OnNewV3Limits(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewH3Limits(WPARAM wParam, LPARAM lParam);

	LRESULT OnNewV4Limits(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewH4Limits(WPARAM wParam, LPARAM lParam);

	LRESULT OnNewV5Limits(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewH5Limits(WPARAM wParam, LPARAM lParam);

	LRESULT OnNewV6Limits(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewH6Limits(WPARAM wParam, LPARAM lParam);

	LRESULT OnNewV7Limits(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewH7Limits(WPARAM wParam, LPARAM lParam);

	LRESULT OnNewV8Limits(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewH8Limits(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CDataWatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg LRESULT OnScrollH1Msg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScrollH2Msg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScrollH3Msg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScrollH4Msg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScrollH5Msg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScrollH6Msg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScrollH7Msg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScrollH8Msg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData1FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData2FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData3FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData4FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData5FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData6FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData7FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData8FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData1LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData2LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData3LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData4LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData5LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData6LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData7LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnData8LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnCheckLocked();
	afx_msg void OnButtonExpand();
	afx_msg void OnButtonContract();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAWATCHDLG_H__E0C56560_F034_4831_925D_37911F828151__INCLUDED_)
