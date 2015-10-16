// EditSend.h

#ifndef EDITSEND_H
#define EDITSEND_H

#define IDC_SPECIALKEY	(WM_USER+1500)

class CEditSend : public CEdit
{
public:
	CEditSend();
	virtual ~CEditSend();
protected:
	//{{AFX_MSG(CEditSend)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //EDITSEND_H
