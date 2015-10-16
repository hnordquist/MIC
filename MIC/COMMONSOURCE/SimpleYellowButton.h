// SimpleYellowButton.h

// User: Moore        Date: 11/14/05   Time: 3:24p
// SCR00114 JSR-12 Apply buttons ambiguous
 
#ifndef SIMPLEYELLOWBUTTON_H
#define SIMPLEYELLOWBUTTON_H

#include "colors.h"

/////////////////////////////////////////////////////////////////////////////
// SimpleYellowButton

class CSimpleYellowButton : public CButton
{
// Construction
public:
	CSimpleYellowButton(
		char *pcaption, 
		UINT id, 
		//char *inifile, //not needed anymore 
		CWnd *parent,
		UINT uRGBcolor = COLOR_YELLOW);

// Attributes
public:
	UINT m_uID;
	CWnd *m_pWnd;
	CString m_csCaption;
	UINT m_input_color;  //<defaults to yellow
	CFont m_Font;
	COLORREF m_face_color;


// Operations
public:
	BOOL EnableWindow( BOOL bEnable = TRUE );
private:
	BOOL Create();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleYellowButton)
	public:
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSimpleYellowButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSimpleYellowButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //SIMPLEYELLOWBUTTON_H