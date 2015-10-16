#pragma once


// CBioSettings dialog

class CBioSettings : public CDialog
{
	DECLARE_DYNAMIC(CBioSettings)

public:
	CBioSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBioSettings();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSkipcount();
	afx_msg void OnBnClickedBidgen();
	virtual BOOL OnInitDialog();
};
