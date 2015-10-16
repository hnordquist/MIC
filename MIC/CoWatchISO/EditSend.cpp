// EditSend.cpp

#include "stdafx.h"
#include "EditSend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditSend

CEditSend::CEditSend() {;}
CEditSend::~CEditSend() {;}

BEGIN_MESSAGE_MAP(CEditSend, CEdit)
	//{{AFX_MSG_MAP(CEditSend)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditSend::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if( nChar == VK_RETURN )
		GetParent()->PostMessage(WM_COMMAND,IDOK,NULL);	
	else
		CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CEditSend::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if ((nChar == VK_UP)||(nChar == VK_DOWN)||(nChar == VK_ESCAPE))
		GetParent()->PostMessage(IDC_SPECIALKEY,nChar,NULL);
	else
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
