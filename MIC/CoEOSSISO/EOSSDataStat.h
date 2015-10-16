//EOSSDataStat.h
#ifndef EOSSDATASTAT_H
#define EOSSDATASTAT_H

#include "EOSSSafePS.h"
#include "eossdefs.h"
#include "ISO.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CEOSSDataStat dialog
class CEOSSDataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CEOSSDataStat)

// Construction
public:
	CEOSSDataStat(UINT id, CISO *parent);
	CEOSSDataStat();
	~CEOSSDataStat();
	void SetText(int, char *);
	char *GetText(int, char *,int);
	void IssuePrint(FILE *outfile);
	void SetColor(int, COLORREF);
	void ResetColors();
	CISO *m_pParent;
	void SetPropertySheet(CEOSSPropertySheet*);
	COLORREF StaticColors[IDC_EOSSDATA_LAST-IDC_EOSSDATA_FIRST+1];
	void PopulateTree();
	int  ColorFlagToInt(unsigned char ucFlag);
	void SetUpdateDataDisplay(bool value = true);

// Dialog Data
	//{{AFX_DATA(CEOSSDataStat)
	enum { IDD = IDD_EOSSDATASTATUS_PAGE };
	CTreeCtrl	m_cTreeCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEOSSDataStat)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CImageList		m_cImages;
	unsigned int	EOSSes[32];

protected:
	CEOSSPropertySheet *m_pPropertySheet;
	void BuildPath(HTREEITEM hTreeItem, char* szFinal, char*szTemp, int iLevel = 0);

	// Generated message map functions
	//{{AFX_MSG(CEOSSDataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangingTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRemovedata();
	afx_msg void OnButtonRunnow();
	afx_msg void OnButtonRunnow2();
	afx_msg void OnButtonUpdateDataDisplay();
	afx_msg void OnItemexpandingTree1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // EOSSDATASTAT_H
