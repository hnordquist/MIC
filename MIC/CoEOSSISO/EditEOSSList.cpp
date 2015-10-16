// EditEOSSList.cpp : implementation file
//

#include "stdafx.h"
#include "coeossiso.h"
#include "EditEOSSList.h"
#include "AddEOSS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEOSSList dialog


CEditEOSSList::CEditEOSSList(CWnd* pParent /*=NULL*/)
	: CDialog(CEditEOSSList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditEOSSList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEditEOSSList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditEOSSList)
	DDX_Control(pDX, IDC_BUTTON_MOVEUP, m_cButtonMoveUp);
	DDX_Control(pDX, IDC_BUTTON_MOVEDOWN, m_cButtonMoveDown);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_cButtonDelete);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_cButtonAdd);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditEOSSList, CDialog)
	//{{AFX_MSG_MAP(CEditEOSSList)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_NOTIFY(HDN_ITEMCLICK, IDC_LIST1, OnItemclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON_MOVEUP, OnButtonMoveup)
	ON_BN_CLICKED(IDC_BUTTON_MOVEDOWN, OnButtonMovedown)
	ON_NOTIFY(HDN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEOSSList message handlers

void CEditEOSSList::OnButtonAdd() 
{
	CAddEOSS cAddEOSS;
	cAddEOSS.m_parentsListCtrl = &m_ListCtrl;

	if (cAddEOSS.DoModal() == IDOK)
	{
		if (cAddEOSS.m_csEOSSSN.GetLength() == 5)
		{
			char szTemp[32];
			int iSelected = m_ListCtrl.GetSelectionMark();
			//if (iSelected >= 0) m_ListCtrl.SetItemState(iSelected,0,LVIS_SELECTED);
			iSelected = max(0,iSelected+1);
			int iItem = m_ListCtrl.InsertItem(iSelected,"ONE",NULL);
	
			sprintf(szTemp,"%5d",iItem+1);
			m_ListCtrl.SetItem(iItem,0,LVIF_TEXT , szTemp, NULL,NULL,NULL,NULL);
			m_ListCtrl.SetItem(iItem,1,LVIF_TEXT , cAddEOSS.m_csEOSSSN, NULL,NULL,NULL,NULL);
			m_ListCtrl.EnsureVisible(iItem,FALSE);
			if (m_ListCtrl.GetItemCount() >= 32)
				m_cButtonAdd.EnableWindow(FALSE);
			m_ListCtrl.SetItemState(iItem, LVIS_SELECTED,LVIS_SELECTED);
			m_ListCtrl.SetSelectionMark(iItem);
			m_ListCtrl.SetFocus();
			int iCount = m_ListCtrl.GetItemCount();
			for (int i = 0; i < iCount; i++)
			{
				sprintf(szTemp,"%6d",i+1);
				m_ListCtrl.SetItemText(i,0,szTemp);
			}
			AdjustIndex();
			AdjustButtons();
		}
	}
}

void CEditEOSSList::AdjustIndex()
{
	int iCount = m_ListCtrl.GetItemCount();
	char szTemp[16];
	for (int i = 0; i < iCount; i++)
	{
		sprintf(szTemp,"%6d",i+1);
		m_ListCtrl.SetItemText(i,0,szTemp);
	}
}

void CEditEOSSList::OnButtonDelete() 
{
	int iItem = m_ListCtrl.GetItemCount();
	if (iItem > 0)
	{
		int iSelected = m_ListCtrl.GetSelectionMark();
		m_ListCtrl.DeleteItem(iSelected);
		iItem--;
		if (iSelected < iItem) 
		{
			m_ListCtrl.SetItemState(iSelected, LVIS_SELECTED,LVIS_SELECTED);
			m_ListCtrl.SetSelectionMark(iSelected);
			m_ListCtrl.SetFocus();
		}
		else
		{
			m_ListCtrl.SetItemState(iItem-1, LVIS_SELECTED,LVIS_SELECTED);
			m_ListCtrl.SetSelectionMark(iItem-1);
			m_ListCtrl.SetFocus();
		}
	}
	AdjustIndex();
	AdjustButtons();
}

void CEditEOSSList::AdjustButtons()
{
	int iSelected = m_ListCtrl.GetSelectionMark();
	int iCount = m_ListCtrl.GetItemCount();

	TRACE("Selected: %d Count: %d\r\n",iSelected,iCount);
	if (iCount == 0)
	{
		m_cButtonAdd.EnableWindow(TRUE);
		m_cButtonDelete.EnableWindow(FALSE);
		m_cButtonMoveUp.EnableWindow(FALSE);
		m_cButtonMoveDown.EnableWindow(FALSE);
	}
	else if (iCount == 32)
	{
		m_cButtonAdd.EnableWindow(FALSE);
		m_cButtonDelete.EnableWindow((iSelected >= 0)?TRUE:FALSE);
		m_cButtonMoveUp.EnableWindow((iSelected >  0)?TRUE:FALSE);
		m_cButtonMoveDown.EnableWindow((iSelected >= 0 && iSelected < 32)?TRUE:FALSE);
	}
	else
	{
		m_cButtonAdd.EnableWindow(TRUE);
		m_cButtonDelete.EnableWindow((iSelected>=0)?TRUE:FALSE);
		m_cButtonMoveDown.EnableWindow((iSelected+1 == iCount)?FALSE:TRUE);
		m_cButtonMoveUp.EnableWindow((iSelected == 0)?FALSE:TRUE);
	}
}


void CEditEOSSList::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int iSelected = m_ListCtrl.GetSelectionMark();
	m_ListCtrl.SetItemState(iSelected, LVIS_SELECTED,LVIS_SELECTED);
	m_ListCtrl.SetSelectionMark(iSelected);
	m_ListCtrl.SetFocus();
	AdjustButtons();	
	*pResult = 0;
}

BOOL CEditEOSSList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ListCtrl.InsertColumn(0,"Index",LVCFMT_CENTER,65,NULL);
	m_ListCtrl.InsertColumn(1,"Name",LVCFMT_CENTER,70,NULL);
	DWORD dwStyle = m_ListCtrl.GetExtendedStyle ();
	dwStyle = dwStyle & ~LVS_EX_TRACKSELECT;
	dwStyle |= LVS_EX_FULLROWSELECT; // highlight the selected full row
	dwStyle |= LVS_EX_GRIDLINES; // grid lines (with the style of the report)
	dwStyle |= LVS_SHOWSELALWAYS; // always show selection
	//dwStyle |= LVS_EX_CHECKBOXES; // checkbox controls
	//m_ListCtrl.SetExtendedStyle (dwStyle); // set expand style	
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);

//add what is currently in the registry/ini
	int iItem = 0;
	if (m_czEOSSes.GetLength())
	{
		char* szAt;
		szAt = strtok((char*)LPCSTR(m_czEOSSes),",");
		while (szAt)
		{
			char szTemp[32];
			iItem = m_ListCtrl.InsertItem(iItem,"ONE",NULL);

			sprintf(szTemp,"%5d",iItem+1);
			m_ListCtrl.SetItem(iItem,0,LVIF_TEXT , szTemp, NULL,NULL,NULL,NULL);
			m_ListCtrl.SetItem(iItem,1,LVIF_TEXT , szAt  , NULL,NULL,NULL,NULL);
			iItem++;
			szAt = strtok(NULL,",");
		}
	}

	int iCnt = m_ListCtrl.GetItemCount();
	if (iCnt)
	{
		m_ListCtrl.SetItemState(0, LVIS_SELECTED,LVIS_SELECTED);
		m_ListCtrl.SetSelectionMark(0);
		m_ListCtrl.SetFocus();
	}
	AdjustIndex();
	AdjustButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditEOSSList::OnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
//	AdjustButtons();
	*pResult = 0;
}

void CEditEOSSList::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	AdjustButtons();
	*pResult = 0;
}

void CEditEOSSList::OnButtonMoveup() 
{
	// TODO: Add your control notification handler code here
	int iSelected = m_ListCtrl.GetSelectionMark();
	int iCnt = m_ListCtrl.GetItemCount();
	if (iSelected > 0)
	{
		m_ListCtrl.SetItemState(iSelected,0,LVIS_SELECTED);

		char szSource[16];
		char szDestin[16];
		m_ListCtrl.GetItemText(iSelected,1,szSource,sizeof(szSource));
		m_ListCtrl.GetItemText(iSelected-1,1,szDestin,sizeof(szDestin));
		m_ListCtrl.SetItemText(iSelected-1,1,szSource);
		m_ListCtrl.SetItemText(iSelected,1,szDestin);
		m_ListCtrl.SetItemState(iSelected-1, LVIS_SELECTED,LVIS_SELECTED);
		m_ListCtrl.SetSelectionMark(iSelected-1);
		m_ListCtrl.SetFocus();
	}
	AdjustButtons();
}

void CEditEOSSList::OnButtonMovedown() 
{
	// TODO: Add your control notification handler code here
	int iSelected = m_ListCtrl.GetSelectionMark();
	int iCnt = m_ListCtrl.GetItemCount();
	if (iSelected >= 0 && iSelected <= iCnt)
	{
		m_ListCtrl.SetItemState(iSelected,0,LVIS_SELECTED);

		char szSource[16];
		char szDestin[16];
		m_ListCtrl.GetItemText(iSelected,1,szDestin,sizeof(szDestin));
		m_ListCtrl.GetItemText(iSelected+1,1,szSource,sizeof(szSource));
		m_ListCtrl.SetItemText(iSelected,1,szSource);
		m_ListCtrl.SetItemText(iSelected+1,1,szDestin);
		m_ListCtrl.SetItemState(iSelected+1, LVIS_SELECTED,LVIS_SELECTED);
		m_ListCtrl.SetSelectionMark(iSelected+1);
		m_ListCtrl.SetFocus();
	}
	AdjustButtons();
}

void CEditEOSSList::OnOK() 
{
	// TODO: Add extra validation here
	m_czEOSSes.Empty();
	int iCnt = m_ListCtrl.GetItemCount();
	for (int i = 0; i < iCnt; i++)
	{
		if (i > 0) m_czEOSSes += ",";
		m_czEOSSes += m_ListCtrl.GetItemText(i,1);
	}
	
	CDialog::OnOK();
}


const int CEditEOSSList::maxEOSSCount = 40;