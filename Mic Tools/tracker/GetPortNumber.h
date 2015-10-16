#if !defined(AFX_GETPORTNUMBER_H__BAA3C62B_3C23_4D0E_B830_AF5310A2778F__INCLUDED_)
#define AFX_GETPORTNUMBER_H__BAA3C62B_3C23_4D0E_B830_AF5310A2778F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetPortNumber.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetPortNumber dialog

class CGetPortNumber : public CDialog
{
// Construction
public:
	CGetPortNumber(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetPortNumber)
	enum { IDD = IDD_DIALOG_GETPORT };
	UINT	m_uPort;
	CString	m_cStaticAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetPortNumber)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetPortNumber)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETPORTNUMBER_H__BAA3C62B_3C23_4D0E_B830_AF5310A2778F__INCLUDED_)
