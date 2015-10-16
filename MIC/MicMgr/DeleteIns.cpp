//DeleteIns.cpp 

#include "stdafx.h"
#include "resource.h"
#include "DeleteIns.h"
#include "resource.h"
#include "validate.h"
#include "globals.h"
#include "colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteIns dialog
// delete instrument

CDeleteIns::CDeleteIns(CWnd* pParent)
	: CDialog(CDeleteIns::IDD, pParent)
{
	m_dSelect = -1;
	m_dCount  =  0;
	m_pDeleteButton = NULL;
}
CDeleteIns::~CDeleteIns()
{
	delete m_pDeleteButton;
}


void CDeleteIns::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteIns)
	DDX_Control(pDX, IDC_DELETE_LIST, m_DeleteList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteIns, CDialog)
	//{{AFX_MSG_MAP(CDeleteIns)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_DELETE_LIST, OnSelchangeDeleteList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteIns message handlers

void CDeleteIns::OnOK() 
{
	CValidate Valid(this);

    if (g_bHideWindows)
		ShowWindow(SW_HIDE);

	if (Valid.DoModal() == IDOK) 
	{
		//get the selection -- parent will do the 
		//instrument delete for us 
		int temp;
		if ((temp = m_DeleteList.GetCurSel()) != LB_ERR)
			m_dSelect = m_DeleteList.GetItemData(temp);

		BSTR bstr;
		g_pIISO[m_dSelect]->get_Name(&bstr);
		UINT count = 1 + SysStringLen(bstr);

		char *name = (char *)CoTaskMemAlloc((ULONG)count);
		Convert_BSTR_to_Character_String(bstr,name,count);

		AfxGetMainWnd()->PostMessage(IDC_DELETE_INST, NULL,(WPARAM)name);

		CDialog::OnOK();
	}
	//show this window again
	ShowWindow(SW_SHOW);
}

void CDeleteIns::AddString(char* Data)  //<<QA>> delete this
{
	//add an instrument to the list
	//this is done after instantiating the dialog
	//box and before do modal is called
	char *data = (char *)malloc(strlen (Data)+1);
	strcpy(data,Data);
	m_DeleteData[m_dCount] = data;
	m_dCount++;
}

BOOL CDeleteIns::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDOK)->EnableWindow(false);
	SetTimer(1,g_iDlgCloseMillisec,NULL);

	//set the tab stops
	int tabs[2] = {60,85};
	m_DeleteList.SetTabStops(2,tabs);	
	
	char string_out[64];
	int index;
	BSTR bstr;

	for (int i = 0; i < g_iInstCount; i++)
	{
		g_pIISO[i]->get_FullName(&bstr);
		Convert_BSTR_to_Character_String(bstr,string_out,sizeof(string_out));
		index = m_DeleteList.AddString(string_out);
		m_DeleteList.SetItemData(index,i);
	}
	
	//create delete button
	m_pDeleteButton = new CYellowButton("Delete",IDOK,g_szIniFile,this);

	return TRUE;
}

void CDeleteIns::OnDestroy() 
{
	//on quit get rid of all the data
	CDialog::OnDestroy();
	for (int i = 0; i < m_dCount; i++)
		delete [] m_DeleteData[i];
}

void CDeleteIns::OnTimer(UINT nIDEvent) 
{
	//catch the timer to auto-close this dialog box
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnCancel();
	}

	CDialog::OnTimer(nIDEvent);
}

void CDeleteIns::OnSelchangeDeleteList() 
{
	//enable "ok" button if valid current selection
	if (m_DeleteList.GetCurSel() != LB_ERR)
		m_pDeleteButton->EnableWindow(TRUE);
	else
		GetDlgItem(IDOK)->EnableWindow(FALSE);
}
