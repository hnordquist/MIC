
#ifndef NEWCOMM_H
#define NEWCOMM_H

// NewComm.h

#include "resource.h"
#include "COM_Utilities.h"
#include "../Interfaces/ICSO.h"
#include <comcat.h>
#include "ccbutton.h"

#define MAX_COMM_NAME_LENGTH  31
#define MAX_COMM_TYPE_LENGTH  31

class CNewComm : public CDialog
{
	DECLARE_DYNCREATE(CNewComm)

// Construction
	CNewComm(); // protected constructor used by dynamic creation
	virtual ~CNewComm();
public:
	CNewComm(CWnd* pParent);

// Dialog Data

	char m_pName[MAX_COMM_NAME_LENGTH + 1];
	char m_pType[MAX_COMM_TYPE_LENGTH + 1];
	bool m_bHaveName;
	bool m_bHaveType;
	int  m_iSelectionIndex;
	ICSO* m_pTemporaryICSO;
	CWnd* m_pParent;
	CString m_csTitle;
	CYellowButton *m_pCreateButton;

	//Dynamic Discovery variables and functions
#define MAX_CLSIDS 64

	ICatInformation* m_pICatInformation;
	ICatRegister*    m_pICatRegister;
	CLSID          m_CLSID[MAX_CLSIDS];
	IClassFactory *m_pClassFactory[MAX_CLSIDS];

	bool TestRegistrationOfComponentCategory();
	void CleanUpTemporaryICSO();
	void GetFriendlyName(const CLSID& clsid, char namestring[], int numchars);
	bool DoDynamicDiscovery(bool bDoGUI = true, CString *pcsOut = NULL);

	//{{AFX_DATA(CNewComm)
	enum { IDD = IDD_NEWCOMM_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewComm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewComm)
	afx_msg void OnChangeCommnameEdit();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeCommtypeList();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCreateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // NEWCOMM_H