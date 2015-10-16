

#ifndef DSPECSTATICSETUPDATA_H
#define DSPECSTATICSETUPDATA_H


// DSPECStaticSetupData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDSPECStaticSetupData window

class CDSPECStaticSetupData : public CStatic
{
// Construction
public:
	CDSPECStaticSetupData();

// Attributes
public:
	
// Operations
public:
	void ReSetItterator(){m_uItterator = 0;};
	char *GetNext();
	LPTSTR *lpszRow;
	UINT m_uItterator;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPECStaticSetupData)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDSPECStaticSetupData();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDSPECStaticSetupData)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // DSPECSTATICSETUPDATA_H
