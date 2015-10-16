#ifndef MCAUSERPROG_H
#define MCAUSERPROG_H

// MCAUserProg.h : header file
//
#include "MCASafePS.h"
#include "ISO.h"

/////////////////////////////////////////////////////////////////////////////
// CMCAUserProg dialog
class CMCAUserProg : public CPropertyPage
{
	DECLARE_DYNCREATE(CMCAUserProg)

// Construction
public:
	CMCAUserProg(UINT id, CISO *parent);
	CMCAUserProg();
	~CMCAUserProg();

	void SetText(int, char *);
	void SetColor(int, COLORREF);
	void SetPropertySheet(CMCAPropertySheet*);

	COLORREF StaticColors[IDC_USERPROG_LAST-IDC_USERPROG_FIRST+1];
	CISO *m_pParent;

// Dialog Data
	//{{AFX_DATA(CMCAUserProg)
	enum { IDD = IDD_MCAUSERPROGSTAT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMCAUserProg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMCAPropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CMCAUserProg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // MCAUSERPROG_H
