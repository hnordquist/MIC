// NewInst.h
 
#ifndef NEWINST_H
#define NEWINST_H

#include "resource.h"
#include "COM_Utilities.h"
#include "../Interfaces/IISO.h"
#include <comcat.h>
#include "ccbutton.h"

#define MAX_INST_NAME_LENGTH	31
#define MAX_INST_TYPE_LENGTH	31

class CNewInst : public CDialog
{
	DECLARE_DYNCREATE(CNewInst)

// Construction
	CNewInst();        // protected constructor used by dynamic creation
	virtual ~CNewInst();

public:
	CNewInst(CWnd* pParent);

// Dialog Data
	char m_pName[MAX_INST_NAME_LENGTH + 1];
	char m_pType[MAX_INST_TYPE_LENGTH + 1];
	bool m_bHaveName;
	bool m_bHaveType;
	int  m_iSelectionIndex;
	IISO* m_pTemporaryIISO;
	CWnd* m_pParent;
	CString m_csTitle;
	CYellowButton *m_pCreateButton;

	//Dynamic Discovery variables and functions
#define MAX_CLSIDS 64
	ICatInformation* m_pICatInformation;
	ICatRegister*    m_pICatRegister;
	CLSID            m_CLSID[MAX_CLSIDS];
	IClassFactory*   m_pClassFactory[MAX_CLSIDS];

	bool TestRegistrationOfComponentCategory();
	void CleanUpTemporaryIISO();
	void GetFriendlyName(const CLSID& clsid, char namestring[], int numchars);	//dgp 24 Oct 2006
	bool DoDynamicDiscovery(bool bDoGUI = true, CString *pcsOut = NULL);

	//{{AFX_DATA(CNewInst)
	enum { IDD = IDD_NEWINST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewInst)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewInst)
	afx_msg void OnChangeInstnameEdit();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeInsttypeList();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCreateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // NEWINST_H
