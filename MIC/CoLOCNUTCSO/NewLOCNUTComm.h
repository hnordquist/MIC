//$History: NewLOCNUTComm.h $
 * 
 * *****************  Version 2  *****************
 * User: Pelowitz     Date: 4/29/08    Time: 2:14p
 * Updated in $/MIC 2004/CoLOCNUTCSO
 * final check in prior to retirement
// * 
// * *****************  Version 1  *****************
// * User: Pelowitz     Date: 3/12/08    Time: 2:52p
// * Created in $/MIC 2004/CoLOCNUTCSO
// * At or near retirement (EOSS still being tested)
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 9/13/06    Time: 12:39p
// Created in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 1/11/06    Time: 7:47a
// Created in $/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Moore        Date: 5/03/05    Time: 2:11p
// Created in $/MIC 2.0/CoLOCNUTCSO
// Added CoLOCNUTCSO to workspace.
// 

#ifndef NEWLOCNUTCOMM_H
#define NEWLOCNUTCOMM_H

// NewLOCNUTComm.h : header file

#include "resource.h"
#include "ccbutton.h"

/////////////////////////////////////////////////////////////////////////////
// CNewLOCNUTComm dialog

class CNewLOCNUTComm : public CDialog
{
	DECLARE_DYNCREATE(CNewLOCNUTComm)

// Construction
public:
	CNewLOCNUTComm(char *cIniFile, int iDlgClose, char *name);
	CNewLOCNUTComm();
	virtual ~CNewLOCNUTComm();

// Dialog Data
	//{{AFX_DATA(CNewLOCNUTComm)
	enum { IDD = IDD_LOCNUT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewLOCNUTComm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	CYellowButton *m_pCreateButton;

	int  m_dDlgCloseMilliseconds;

	char m_pName[128];
	char m_szIniFile[_MAX_PATH];

	CString m_csTitle;

	// Generated message map functions
	//{{AFX_MSG(CNewLOCNUTComm)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // NEWLOCNUTCOMM_H
