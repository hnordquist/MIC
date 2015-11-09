/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
// Configure.cpp : implementation file
//

#include "stdafx.h"
#include "Sync.h"
#include "Configure.h"
#include "AddEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigure dialog


CConfigure::CConfigure(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigure::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigure)
	m_dCycDays = 0;
	m_dCycHours = 0;
	m_dCycMinutes = 0;
	m_dCycSeconds = 0;
	m_dPause = 0;
	//}}AFX_DATA_INIT
}


void CConfigure::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigure)
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_pDeleteButton);
	DDX_Control(pDX, IDC_BUTTON_EDIT, m_pEditButton);
	DDX_Control(pDX, IDC_LIST1, m_pList);
	DDX_Control(pDX, IDC_CHECK_HIDE, m_pCheckHide);
	DDX_Control(pDX, IDC_SPIN_PAUSE, m_pSpinPause);
	DDX_Control(pDX, IDC_SPIN_CYCSECONDS, m_pSpinSeconds);
	DDX_Control(pDX, IDC_SPIN_CYCMINUTES, m_pSpinMinutes);
	DDX_Control(pDX, IDC_SPIN_CYCHOURS, m_pSpinHours);
	DDX_Control(pDX, IDC_SPIN_CYCDAYS, m_pSpinDays);
	DDX_Text(pDX, IDC_EDIT_CYCDAYS, m_dCycDays);
	DDV_MinMaxUInt(pDX, m_dCycDays, 0, 999);
	DDX_Text(pDX, IDC_EDIT_CYCHOURS, m_dCycHours);
	DDV_MinMaxUInt(pDX, m_dCycHours, 0, 23);
	DDX_Text(pDX, IDC_EDIT_CYCMINUTES, m_dCycMinutes);
	DDV_MinMaxUInt(pDX, m_dCycMinutes, 0, 59);
	DDX_Text(pDX, IDC_EDIT_CYCSECONDS, m_dCycSeconds);
	DDV_MinMaxUInt(pDX, m_dCycSeconds, 0, 59);
	DDX_Text(pDX, IDC_EDIT_PAUSE, m_dPause);
	DDV_MinMaxUInt(pDX, m_dPause, 0, 9999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigure, CDialog)
	//{{AFX_MSG_MAP(CConfigure)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigure message handlers

	// TODO: Add your control notification handler code here
void CConfigure::OnButtonAdd() 
{
	
	// TODO: Add your control notification handler code here
	CAddEdit AddEdit;
	AddEdit.SetTitle("Add New Entry");
	if (AddEdit.DoModal() == IDOK)
	{
		LV_ITEM lvitem;
		memchr(&lvitem,NULL,sizeof(lvitem));

		CString cTemp;

		//add to display 
		int i = m_pList.GetItemCount();
		//so create it
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 0;
		lvitem.pszText = (AddEdit.m_bDoSub)?"Yes":"No"; 
		lvitem.iImage = 0;
		m_pList.InsertItem(&lvitem);

		//create the 2nd column entry
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 1;
		lvitem.iImage = 0;
		lvitem.pszText = (AddEdit.m_bConsiderArchive)?"Yes":"No";
		m_pList.SetItem(&lvitem);

		//create the 3rd column entry
		cTemp.Format("%d:%d:%d:%d",
			AddEdit.m_dCycDays,
			AddEdit.m_dCycHours,
			AddEdit.m_dCycMinutes,
			AddEdit.m_dCycSeconds);
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 2;
		lvitem.iImage = 0;
		lvitem.pszText = (char*)(LPCSTR)cTemp;
		m_pList.SetItem(&lvitem);

		//create the 4th column entry
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 3;
		lvitem.iImage = 0;
		lvitem.pszText = (char*)(LPCSTR)AddEdit.m_cPath;
		m_pList.SetItem(&lvitem);
	}
}

void CConfigure::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	m_pList.DeleteItem(m_dSelectedItem);
}

void CConfigure::OnButtonEdit() 
{
	// TODO: Add your control notification handler code here
	CAddEdit AddEdit;
	AddEdit.SetTitle("Edit Selected");
	BOOL bConsiderArchive;
	BOOL bDoSub;
	UINT Days;
	UINT Hours;
	UINT Minutes;
	UINT Seconds;
	char szPath[_MAX_PATH];
	m_pList.GetItemText(m_dSelectedItem,0,szPath,sizeof(szPath));
	if (toupper(szPath[0])=='Y')
		bDoSub = TRUE;
	else
		bDoSub = FALSE;

	m_pList.GetItemText(m_dSelectedItem,1,szPath,sizeof(szPath));
	if (toupper(szPath[0])=='Y')
		bConsiderArchive = TRUE;
	else
		bConsiderArchive = FALSE;

	m_pList.GetItemText(m_dSelectedItem,2,szPath,sizeof(szPath));
	sscanf(szPath,"%d:%d:%d:%d",&Days,&Hours,&Minutes,&Seconds);

	m_pList.GetItemText(m_dSelectedItem,3,szPath,sizeof(szPath));

	//place the data into the dialog box
	AddEdit.SetUp(Days,Hours,Minutes,Seconds,
		bDoSub,bConsiderArchive,szPath);
	if (AddEdit.DoModal() == IDOK)
	{
		CString cTemp;

		//set the currently selected item
		m_pList.SetItemText(m_dSelectedItem,0,(AddEdit.m_bDoSub)?"Yes":"No");

		//create the 2nd column entry
		m_pList.SetItemText(m_dSelectedItem,1,(AddEdit.m_bConsiderArchive)?"Yes":"No");

		//create the 3rd column entry
		cTemp.Format("%d:%d:%d:%d",
			AddEdit.m_dCycDays,
			AddEdit.m_dCycHours,
			AddEdit.m_dCycMinutes,
			AddEdit.m_dCycSeconds);
		m_pList.SetItemText(m_dSelectedItem,2,(char*)(LPCSTR)cTemp);

		//create the 4th column entry
		m_pList.SetItemText(m_dSelectedItem,3,(char*)(LPCSTR)AddEdit.m_cPath);

	}
}

void CConfigure::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CConfigure::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_IniFile.GetLength() > 0)
	{
		char szTemp[_MAX_PATH];

		m_pList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,
			(LPARAM)LVS_EX_FULLROWSELECT);

		//get cycle time
		int Days, Hours, Minutes, Seconds;
		GetPrivateProfileString("CONFIGURATION","CYCLETIME","999:23:59:59",szTemp,sizeof(szTemp),LPCSTR(m_IniFile));
		if (strchr(szTemp,':'))
		{
			if (sscanf(szTemp,"%d:%d:%d:%d",&Days,&Hours,&Minutes,&Seconds) != 4)
			{
				Days = 999; Hours = 23; Minutes = 59; Seconds = 59;
			}
		}
		else
		{
			Days = Hours = Minutes = 0;
			Seconds = atoi(szTemp);
		}

		m_pSpinDays.SetRange(0,999);
		m_pSpinDays.SetPos(Days);
		m_pSpinHours.SetRange(0,23);
		m_pSpinHours.SetPos(Hours);
		m_pSpinMinutes.SetRange(0,59);
		m_pSpinMinutes.SetPos(Minutes);
		m_pSpinSeconds.SetRange(0,59);
		m_pSpinSeconds.SetPos(Seconds);

		//get max pause
		m_pSpinPause.SetRange(0,9999);
		m_pSpinPause.SetPos(GetPrivateProfileInt("CONFIGURATION","MAXPAUSE",60,LPCSTR(m_IniFile)));
		
		//get hide dialog flag
		GetPrivateProfileString("CONFIGURATION","SHOW","1",szTemp,sizeof(szTemp),LPCSTR(m_IniFile));
		if (toupper(szTemp[0] == 'Y') || (szTemp[0] == '1'))
			m_pCheckHide.SetCheck(0);
		else
			m_pCheckHide.SetCheck(1);


		//set up the list box
		//set columns in listcontrol
		m_pList.InsertColumn(1, "SubDir",	LVCFMT_LEFT,	50,		0);
		m_pList.InsertColumn(2, "Arch. Bit",LVCFMT_LEFT,    55,     1);
		m_pList.InsertColumn(3, "Days:H:M:S",LVCFMT_LEFT,	75,		2);
		m_pList.InsertColumn(4, "Directory",LVCFMT_LEFT,	240,	3);


		int DirIndex = 1;
		char INIIndex[8];
		char InDirectory[_MAX_PATH];
		char szTemp1[_MAX_PATH];
		char szTemp2[_MAX_PATH];

		bool bConsiderArchiveBit;
		bool bDoSubDirectories;

		//build first INI entry name
		sprintf(INIIndex,"%d",DirIndex);
		//get the first name from the INI file
		COleDateTimeSpan Age;

		GetPrivateProfileString("WATCHES",INIIndex,"",InDirectory,sizeof(InDirectory),LPCSTR(m_IniFile));
		//while we have a name do
		
		while (strlen(InDirectory) > 0)
		{
			//get use archive flag
			sprintf(szTemp1,"%dUSEARCHIVEFLAG",DirIndex);
			GetPrivateProfileString("WATCHES",szTemp1,"",szTemp2,sizeof(szTemp2),LPCSTR(m_IniFile));
			if (strlen(szTemp2)==0)
			{
				WritePrivateProfileString("WATCHES",szTemp1,"N",LPCSTR(m_IniFile));
				strcpy(szTemp2,"N");
			}

			if (toupper(szTemp2[0])=='Y')
				bConsiderArchiveBit = true;
			else
				bConsiderArchiveBit = false;

			sprintf(szTemp1, "%dDOSUBDIRECTORY",DirIndex);
			GetPrivateProfileString("WATCHES",szTemp1,"",szTemp2,sizeof(szTemp2),LPCSTR(m_IniFile));
			if (strlen(szTemp2)==0)
			{
				WritePrivateProfileString("WATCHES",szTemp1,"N",LPCSTR(m_IniFile));
				strcpy(szTemp2,"N");
			}

			if (toupper(szTemp2[0])=='Y')
				bDoSubDirectories = true;
			else
				bDoSubDirectories = false;

			sprintf(szTemp1, "%dDELETE_AT_AGE",DirIndex);
			GetPrivateProfileString("WATCHES",szTemp1,"",szTemp2,sizeof(szTemp2),LPCSTR(m_IniFile));
			if (strlen(szTemp2)==0)
			{
				GetPrivateProfileString("CONFIGURATION","AGE","",szTemp2,sizeof(szTemp2),LPCSTR(m_IniFile));
				if (strlen(szTemp2) == 0)
					strcpy(szTemp2,"180:0:0:0");
				WritePrivateProfileString("WATCHES",szTemp1,szTemp2,LPCSTR(m_IniFile));
			}

			if (strchr(szTemp2,':'))
			{
				int Days, Hours, Minutes, Seconds;
				if (sscanf(szTemp2,"%d:%d:%d:%d",&Days,&Hours,&Minutes,&Seconds)!= 4)
				{
					Days = 180; Hours = 0; Minutes = 0; Seconds = 0;
				}
				Age = COleDateTimeSpan(Days,Hours,Minutes,Seconds);
			}
			else	
				Age = COleDateTimeSpan(0,0,0,atoi(szTemp2));
			Age += 5.7870370370370370e-6;  //# do a #define on this travesty

			LV_ITEM lvitem;
			memchr(&lvitem,NULL,sizeof(lvitem));
			int NewItem;
			//add to display 
			//so create it
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = DirIndex-1;
			lvitem.iSubItem = 0;
			lvitem.pszText = (bDoSubDirectories)?"Yes":"No"; 
			lvitem.iImage = 0;
			NewItem = m_pList.InsertItem(&lvitem);

			//create the 2nd column entry
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = NewItem;
			lvitem.iSubItem = 1;
			lvitem.iImage = 0;
			lvitem.pszText = (bConsiderArchiveBit)?"Yes":"No";
			m_pList.SetItem(&lvitem);

			//create the 3rd column entry
			sprintf(szTemp,"%d:%d:%d:%d",
				Age.GetDays(),Age.GetHours(),Age.GetMinutes(),Age.GetSeconds());
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = NewItem;
			lvitem.iSubItem = 2;
			lvitem.iImage = 0;
			lvitem.pszText = szTemp;
			m_pList.SetItem(&lvitem);

			//create the 4th column entry
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = NewItem;
			lvitem.iSubItem = 3;
			lvitem.iImage = 0;
			lvitem.pszText = InDirectory;
			m_pList.SetItem(&lvitem);

			//	build next directory name
			DirIndex++;
			sprintf(INIIndex,"%d",DirIndex);
			//	get the next dir name from the INI file
			GetPrivateProfileString("WATCHES",INIIndex,"",InDirectory,sizeof(InDirectory),LPCSTR(m_IniFile));
		//end while
		}

	}
	//nothing is selected to start with
	m_dSelectedItem = -1;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigure::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (m_pList.GetSelectedCount())
	{
		m_dSelectedItem = pNMListView->iItem;
		m_pEditButton.EnableWindow(true);
		m_pDeleteButton.EnableWindow(true);
	}
	else
	{
		m_dSelectedItem = -1;
		m_pEditButton.EnableWindow(false);
		m_pDeleteButton.EnableWindow(false);
	}

	*pResult = 0;
}

void CConfigure::OnOK() 
{
	// TODO: Add extra validation here
	char szPath[_MAX_PATH];
	char szDoSub[8];
	char szConsiderArchive[8];
	char szAge[16];
	char szItem[32];
	LV_ITEM lvitem;
	memchr(&lvitem,NULL,sizeof(lvitem));

	sprintf(szAge,"%d:%d:%d:%d",
		GetDlgItemInt(IDC_EDIT_CYCDAYS),
		GetDlgItemInt(IDC_EDIT_CYCHOURS),
		GetDlgItemInt(IDC_EDIT_CYCMINUTES),
		GetDlgItemInt(IDC_EDIT_CYCSECONDS));
	WritePrivateProfileString("CONFIGURATION","CYCLETIME",szAge,LPCSTR(m_IniFile));
	GetDlgItemText(IDC_EDIT_PAUSE,szPath,sizeof(szPath));
	WritePrivateProfileString("CONFIGURATION","MAXPAUSE",szPath,LPCSTR(m_IniFile));
	WritePrivateProfileString("CONFIGURATION","SHOW",
		(m_pCheckHide.GetCheck()==1)?"0":"1",LPCSTR(m_IniFile));
	WritePrivateProfileString("WATCHES",NULL,NULL,LPCSTR(m_IniFile));
	for (int i = 0; i < (int)m_pList.GetItemCount(); i++)
	{
		m_pList.GetItemText(i,0,szDoSub,sizeof(szDoSub));
		m_pList.GetItemText(i,1,szConsiderArchive,sizeof(szConsiderArchive));
		m_pList.GetItemText(i,2,szAge,sizeof(szAge));
		m_pList.GetItemText(i,3,szPath,sizeof(szPath));
		

		sprintf(szItem,"%d",i+1);
		WritePrivateProfileString("WATCHES",szItem,szPath,LPCSTR(m_IniFile));
		sprintf(szItem,"%dDOSUBDIRECTORY",i+1);
		WritePrivateProfileString("WATCHES",szItem,szDoSub,LPCSTR(m_IniFile));
		sprintf(szItem,"%dUSEARCHIVEFLAG",i+1);
		WritePrivateProfileString("WATCHES",szItem,szConsiderArchive,LPCSTR(m_IniFile));
		sprintf(szItem,"%dDELETE_AT_AGE",i+1);
		WritePrivateProfileString("WATCHES",szItem,szAge,LPCSTR(m_IniFile));
	}
	CDialog::OnOK();
}
