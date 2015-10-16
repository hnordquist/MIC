// NewWatchWindow.cpp

#include "stdafx.h"
#include "NewWatchWindow.h"
#include "INI_definitions.h"
#include "testinst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewWatchWindow dialog

CNewWatchWindow::CNewWatchWindow(						 
		char * szIniFile,
		int iDlgClose,
		char *name,
		CWnd* pParent /*=NULL*/)
	: CDialog(CNewWatchWindow::IDD, pParent)
{
//	TRACE("CNewWatchWindow::COTR\n");
	strncpy(m_szIniFile, szIniFile, sizeof(m_szIniFile));
	m_iDlgCloseInMilliseconds = iDlgClose;
	strncpy(m_pName, name, sizeof(m_pName));

	//{{AFX_DATA_INIT(CNewWatchWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CNewWatchWindow::~CNewWatchWindow()
{
//	TRACE("CNewWatchWindow::DTOR - NOOP\n");
}


void CNewWatchWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewWatchWindow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewWatchWindow, CDialog)
	//{{AFX_MSG_MAP(CNewWatchWindow)
	ON_LBN_SELCHANGE(IDC_COMMNAMES_LIST, OnSelchangeCommnamesList)
	ON_EN_CHANGE(IDC_NODE_EDIT, OnChangeNodeEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NODE_SPIN, OnDeltaposNodeSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewWatchWindow message handlers

void CNewWatchWindow::OnSelchangeCommnamesList() 
{
	int index;
	if((index=((CListBox*)GetDlgItem(IDC_COMMNAMES_LIST))->GetCurSel())==LB_ERR)
	{
		m_pCommName[0] = NULL;
	}
	else
	{
		((CListBox*)GetDlgItem(IDC_COMMNAMES_LIST))->GetText(index,m_pCommName);
		//((CButton *)GetDlgItem(IDOK))->EnableWindow(true);
		m_pCreateButton->EnableWindow(TRUE);
	}
}

void CNewWatchWindow::OnChangeNodeEdit() 
{
	//don't let the user place a number larger than 32767 in the node
	//as above--we'll have to revisit this when we add support for 
	//ethernet
	CString Value;
	GetDlgItemText(IDC_NODE_EDIT,Value);
	if (atoi(Value) > 32767)
		SetDlgItemText(IDC_NODE_EDIT,"32767");
	else if (atoi(Value) == 0)
		SetDlgItemText(IDC_NODE_EDIT,"0");
	else if (atoi(Value) <= -1)
		SetDlgItemText(IDC_NODE_EDIT, "-1");
}

void CNewWatchWindow::OnDeltaposNodeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CNewWatchWindow::OnCancel() 
{
//	TRACE("CNewWatchWindow::OnCancel()\n");
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CNewWatchWindow::OnOK() 
{
//	TRACE("CNewWatchWindow::OnOk()\n");
	//find the equals 
	char* key = strtok(m_pCommName,"=");

	//get the node
	CString Node;
	GetDlgItemText(IDC_NODE_EDIT,Node);

	WritePrivateProfileString(INSTRUMENTS,m_pName,WATCH_DISCOVERY_TYPE,m_szIniFile);

	//create this instrument's section and add the port and node entries
	WritePrivateProfileString(m_pName,PORT,key,m_szIniFile);
	WritePrivateProfileString(m_pName,NODE,Node,m_szIniFile);
	
	CDialog::OnOK();
}

BOOL CNewWatchWindow::OnInitDialog() 
{
//	TRACE("CNewWatchWindow::OnInitDialog()\n");

	CDialog::OnInitDialog();
	
	char buff[512];
	//get all of the communications objects and place
	//their names in the list box
	//m_bAllowFinish = false;
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,
		sizeof(buff), m_szIniFile)>0)
	{
		char *at = buff;
		while (strlen(at) > 0)
		{
			((CListBox*)GetDlgItem(IDC_COMMNAMES_LIST))->AddString(at);
			//m_bAllowFinish = true;
			at = at + strlen(at)+1;
		}
	}

	SetWindowText(m_pName);

	//limit the user input to 5 digits
	((CEdit*)GetDlgItem(IDC_NODE_EDIT))->LimitText(5);

	//limit the spinner to what the ILON needs
	//we will have to revisit this when we add
	//support for ethernet
	((CSpinButtonCtrl*)GetDlgItem(IDC_NODE_SPIN))->SetRange(-1,32767);

	SetDlgItemText(IDC_NODE_EDIT, "-1");

	//GetDlgItem(IDOK)->EnableWindow(FALSE);

	//create the create button
	m_pCreateButton = new CYellowButton(
		"Create...",
		IDOK,
		m_szIniFile,
		this);


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
