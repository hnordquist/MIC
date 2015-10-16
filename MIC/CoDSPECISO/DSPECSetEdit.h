// DSPECSetEdit.h : header file

#ifndef DSPECSETEDIT_H
#define DSPECSETEDIT_H

#include "resource.h"

class CDSPECSetEdit : public CDialog
{
// Construction
public:
	CDSPECSetEdit(CWnd* pParent = NULL);   // standard constructor
	unsigned int m_uLength;
// Dialog Data
	//{{AFX_DATA(CDSPECSetEdit)
	enum { IDD = IDD_DSPEC_DIALOG_EDIT };
	CEdit           m_cEdit;
	CSpinButtonCtrl m_cSpinButton;
	CComboBox       m_cComboBox;
	CString	m_cDesired;
	CString	m_cDiscription;
	CString	m_cReported;
	CString	m_cIniName;
	CString	m_cCommand;
	CString m_cAllowedValues;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPECSetEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void PopulateComboBox();
	void InitializeEditSpinBuddyPair();

	void CleanUpString(
		char *destination,
		char *source, 
		int destination_length,
		int digits_after_decimal = -1);

	bool m_bUsingComboBox;
	bool m_bIntegerOnly;

	double m_dSpinMax;
	double m_dSpinMin;
	double m_dSpinStepSize;
	int    m_iRightOfDecimal;

	// Generated message map functions
	//{{AFX_MSG(CDSPECSetEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void OnDeltaposDSPECBuddySpin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // DSEPCSETEDIT_H
