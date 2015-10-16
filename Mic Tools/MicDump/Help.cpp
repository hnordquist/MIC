// Help.cpp : implementation file
//

#include "stdafx.h"
#include "WinDump.h"
#include "Help.h"


// Help dialog

IMPLEMENT_DYNAMIC(HelpDlg, CDialog)

HelpDlg::HelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(HelpDlg::IDD, pParent)
{

}

HelpDlg::~HelpDlg()
{
}

void HelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, tertst);
}


BEGIN_MESSAGE_MAP(HelpDlg, CDialog)

	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



void HelpDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		tertst.SetWindowText(help.GetBuffer());
	}
}
