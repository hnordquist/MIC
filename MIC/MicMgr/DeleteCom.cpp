// 
// DeleteCom.cpp : implementation file
//

#include "stdafx.h"
#include "INI_definitions.h"
#include "DeleteCom.h"
#include "validate.h"
#include "TimedMessageDialog.h"
#include "globals.h"
#include "mic_definitions.h"
#include "colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteCom dialog


CDeleteCom::CDeleteCom(CWnd* pParent)
	: CDialog(CDeleteCom::IDD, pParent)
{
	m_csTitle = CString(MIC_NAMELONG)+CString(": Delete Comm");
	m_pDeleteButton = NULL;
}
CDeleteCom::~CDeleteCom()
{
	delete m_pDeleteButton;
}


void CDeleteCom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteCom)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteCom, CDialog)
	//{{AFX_MSG_MAP(CDeleteCom)
	ON_LBN_SELCHANGE(IDC_DELETE_LIST, OnSelchangeDeleteList)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteCom message handlers
//catch the selection change in the delete list
void CDeleteCom::OnSelchangeDeleteList() 
{
	//if the selection is still valid then enable the ok button
	if (((CListBox*)GetDlgItem(IDC_DELETE_LIST))->GetCurSel() != LB_ERR)
		m_pDeleteButton->EnableWindow(TRUE);
	else
		GetDlgItem(IDOK)->EnableWindow(FALSE);
}

//catch the timer messages to close the box after a while
void CDeleteCom::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CDeleteCom::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDOK)->EnableWindow(false);
	SetTimer(1,g_iDlgCloseMillisec,NULL);

	//get all of the comm items from the ini file
	//and place them in the clistbox

	//<<QA>> these should all be done with CString's eventually.
	//Then we don't have to worry about overruns.
	char buff[16384];  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
	char buff2[64];
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,sizeof(buff),g_szIniFile)>0)
	{
		char *at = buff;
		char tempat[64];
		while (strlen(at) > 0)
		{
			strcpy(tempat,at);
			strtok(tempat,"=");
			if (GetPrivateProfileString(tempat,PORT,"",buff2,sizeof(buff2),g_szIniFile)>0)
			{
				strcat(tempat,"\t");
				strcat(tempat,buff2);
			}
			((CListBox*)GetDlgItem(IDC_DELETE_LIST))->AddString(tempat);
			at = at + strlen(at)+1;
		}
	}

	m_pDeleteButton = new CYellowButton("Delete",IDOK,g_szIniFile,this);

	return TRUE;
}

void CDeleteCom::OnOK() 
{
	char buff[16384];  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
	CValidate Valid(this);
	//if ok then shutdown
	bool fail = false;
	//if user name/pw is valid
	if (Valid.DoModal() == IDOK) 
	{
		int selection;
		char Select[64];
		//get the selected com object
		if ((selection=((CListBox*)GetDlgItem(IDC_DELETE_LIST))->GetCurSel())!=LB_ERR)
		{
			//get the text of the selected item
			((CListBox*)GetDlgItem(IDC_DELETE_LIST))->GetText(selection,Select);
			//strip off all but the name
			strtok(Select,"\t");
			//get the list of instruments from the ini file
			if (GetPrivateProfileSection(INSTRUMENTS,buff,sizeof(buff),g_szIniFile)>0)
			{
				char *at = buff;
				char tempat[128];
				char tempat2[128];
				//for each instrument find the "port" and see if
				//it is the same as the port we are deleting
				while ((strlen(at) > 0) && !fail)
				{
					strcpy(tempat,at);
					strtok(tempat,"=");
					GetPrivateProfileString(tempat,PORT,"ERROR",tempat2,sizeof(tempat2)-1,g_szIniFile);
					if (strcmp(tempat2,Select)==0)
						fail = true;
					at = at + strlen(at)+1;
				}//while
				//if we were successfull -- e.g. some instrument
				//is using this port then fail
				if (fail)
				{
					KillTimer(1);
					CString csText("You cannot delete a Communications object while instruments are connected to it! "\
						"  Object NOT deleted!");
					new CTimedMessageDialog(m_csTitle,csText, g_iDlgCloseMillisec/1000, this);
					return;
				} 
			}//if (getprivate
			if (!fail) //we can go ahead and delete the comm object
			{
				char *temp = (char*)CoTaskMemAlloc((ULONG)(strlen(Select)+1));
				strcpy(temp,Select);
				AfxGetMainWnd()->PostMessage(IDC_DELETE_COMM, NULL,(WPARAM)temp);
				CDialog::OnOK();
				return;
			}
		}//if selection
	}//if valid

	CDialog::OnCancel();
}
