// BioSettings.cpp : implementation file
//

#include "stdafx.h"
#include "WinDump.h"
#include "BioSettings.h"
#include "BI0.h"

// CBioSettings dialog

IMPLEMENT_DYNAMIC(CBioSettings, CDialog)

CBioSettings::CBioSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CBioSettings::IDD, pParent)
{

}

CBioSettings::~CBioSettings()
{
}

void CBioSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBioSettings, CDialog)
	ON_BN_CLICKED(IDC_SKIPCOUNT, &CBioSettings::OnBnClickedSkipcount)
	ON_BN_CLICKED(IDC_BIDGEN, &CBioSettings::OnBnClickedBidgen)
END_MESSAGE_MAP()


// CBioSettings message handlers

void CBioSettings::OnBnClickedSkipcount()
{
	BI0Processor::mSkipSubSecondStart = (IsDlgButtonChecked(IDC_SKIPCOUNT) == BST_CHECKED ? true: false);
}

void CBioSettings::OnBnClickedBidgen()
{
	BI0Processor::mWriteBIDfromBI0 = (IsDlgButtonChecked(IDC_BIDGEN) == BST_CHECKED ? true: false);
	GetDlgItem(IDC_SKIPCOUNT)->EnableWindow(BI0Processor::mWriteBIDfromBI0);
}

BOOL CBioSettings::OnInitDialog()
{
	CDialog::OnInitDialog();
	CheckDlgButton(IDC_BIDGEN, BI0Processor::mWriteBIDfromBI0 ? 1 : 0);
	CheckDlgButton(IDC_SKIPCOUNT, BI0Processor::mSkipSubSecondStart ? 1 : 0);
	GetDlgItem(IDC_SKIPCOUNT)->EnableWindow(BI0Processor::mWriteBIDfromBI0);
	return TRUE;
}
