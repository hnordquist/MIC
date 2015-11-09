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
// EOSSDataStat.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "EOSSDataStat.h"
#include "EOSSInstrument.h"
#include "eossdefs.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IDC_TREELIST 123
#define USER_DATA_SIZE 256
/////////////////////////////////////////////////////////////////////////////
// CEOSSDataStat property page

IMPLEMENT_DYNCREATE(CEOSSDataStat, CPropertyPage)

CEOSSDataStat::CEOSSDataStat() : CPropertyPage(CEOSSDataStat::IDD)
{
}

CEOSSDataStat::CEOSSDataStat(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;
	//initialize all displayed (cstatic) text to the
	//default color (black)
	for (int i= IDC_EOSSDATA_FIRST; i <= IDC_EOSSDATA_LAST; i++)
	{
		StaticColors[i-IDC_EOSSDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CGenlStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void
CEOSSDataStat::SetPropertySheet(CEOSSPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}


void
CEOSSDataStat::SetText(int index, char * text)
{
	//set the cstatic to the provided text
	if ((index >= IDC_EOSSDATA_FIRST) &&
		(index <= IDC_EOSSDATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *
CEOSSDataStat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_EOSSDATA_FIRST) &&
		(index <= IDC_EOSSDATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void
CEOSSDataStat::SetColor(int index, COLORREF color)
{
	//set the current color of a cstatic
	//it will get updated via onctlcolor()
	if ((index >= IDC_EOSSDATA_FIRST) &&
		(index <= IDC_EOSSDATA_LAST)) {
	  StaticColors[index-IDC_EOSSDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CEOSSDataStat::ResetColors()
{
	for (int i= IDC_EOSSDATA_FIRST; i <= IDC_EOSSDATA_LAST; i++)
	{
		StaticColors[i-IDC_EOSSDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CEOSSDataStat::~CEOSSDataStat()
{
}

void CEOSSDataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEOSSDataStat)
	DDX_Control(pDX, IDC_TREE1, m_cTreeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEOSSDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CEOSSDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_DRAWITEM()
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE1, OnSelchangingTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEDATA, OnButtonRemovedata)
	ON_BN_CLICKED(IDC_BUTTON_RUNNOW, OnButtonRunnow)
	ON_BN_CLICKED(IDC_BUTTON_RUNNOW2, OnButtonRunnow2)
	ON_BN_CLICKED(IDC_BUTTON_UPDATEDISPLAY, OnButtonUpdateDataDisplay)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, OnItemexpandingTree1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEOSSDataStat message handlers
HBRUSH CEOSSDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//get the id of the control 
	UINT index = pWnd->GetDlgCtrlID();
	//set the font and color of that control
	if ((index >= IDC_EOSSDATA_FIRST) && (index <= IDC_EOSSDATA_LAST))
	{
		int iFirstOfList = IDC_EOSSDATA_FIRST; 
		if (StaticColors[index-iFirstOfList]==COLOR_RED)
		{
			pDC->SetBkColor(COLOR_RED);
			pDC->SetTextColor(RGB(255,255,255));
		}
		else if (StaticColors[(index-iFirstOfList)]==COLOR_YELLOW)
		{
			pDC->SetBkColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_RED);
		}
		else
		{
			pDC->SetTextColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(RGB(0,0,0));
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}
	return hbr;
}
void CEOSSDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

//*****************************************************************************
//*
//*		OnDrawItem
//*
//*****************************************************************************
void CEOSSDataStat::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CEOSSDataStat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	if (bShow)
		PopulateTree();
	
}

BOOL CEOSSDataStat::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CEOSSDataStat::OnSetActive() 
{
	BOOL bResult = CPropertyPage::OnSetActive();
	return bResult;
}

int CEOSSDataStat::ColorFlagToInt(unsigned char ucFlag)
{
	int iResult;
	if (ucFlag == 0x03) iResult = 10; //red and yellow
	else if (ucFlag == 0x02) iResult = 0; // red square
	else if (ucFlag == 0x01) iResult = 3; // yellow triangle
	else iResult = 1;//green
	return iResult;
}

void CEOSSDataStat::PopulateTree()
{

	CEOSSInstrument* pInst = (CEOSSInstrument*)m_pParent;

	DWORD dwIndex = 0;
	bool bDone = false;

	m_cTreeCtrl.DeleteAllItems();
	SetDlgItemText(IDC_STATUS_TXT,"");

	m_cTreeCtrl.SetImageList(m_pPropertySheet->m_pImageList,TVSIL_NORMAL);
	
	HTREEITEM hTreeItem = m_cTreeCtrl.InsertItem("GENERAL",pInst->sGeneral.ucAll,pInst->sGeneral.ucAll);

	//go general stuff
	int iColor;
	iColor = ColorFlagToInt(pInst->sGeneral.ucCOMPORT);
	m_cTreeCtrl.InsertItem("COMPORT", iColor, iColor, hTreeItem);

	iColor = ColorFlagToInt(pInst->sGeneral.ucPATH);
	m_cTreeCtrl.InsertItem("PATH", iColor, iColor, hTreeItem);

	iColor = ColorFlagToInt(pInst->sGeneral.ucFORMAT);
	m_cTreeCtrl.InsertItem("FORMAT", iColor, iColor, hTreeItem);

	iColor = ColorFlagToInt(pInst->sGeneral.ucINSTRUMENT_ID);
	m_cTreeCtrl.InsertItem("INSTRUMENT ID", iColor, iColor, hTreeItem);

	iColor = ColorFlagToInt(pInst->sGeneral.bTimeOffset);
	m_cTreeCtrl.InsertItem("TIME OFFSET", iColor, iColor, hTreeItem);

	iColor = ColorFlagToInt(pInst->sGeneral.ucLATEST_READ);
	m_cTreeCtrl.InsertItem("LATEST READ", iColor, iColor, hTreeItem);

	iColor = ColorFlagToInt(pInst->sGeneral.ucSPECIFIED_EOSSes);
	m_cTreeCtrl.InsertItem("SPECIFIED EOSSes", iColor, iColor,hTreeItem);

	iColor = ColorFlagToInt(pInst->sGeneral.ucLATEST_RESULTS);
	m_cTreeCtrl.InsertItem("LATEST RESULTS", iColor, iColor, hTreeItem);

	iColor = ColorFlagToInt(pInst->sGeneral.ucREADER_VERSION);
	m_cTreeCtrl.InsertItem("READER VERSION", iColor, iColor, hTreeItem);

	//iColor = ColorFlagToInt(pInst->sGeneral.ucQueryDepth);
	//m_cTreeCtrl.InsertItem(QUERY_DEPTH, iColor, iColor, hTreeItem);

	//do each eoss
	char szTemp[16];
	while (!bDone)
	{
		HTREEITEM hTreeSubItem;
		HTREEITEM hTreeItem;
		if (pInst->EOSSes[dwIndex])
		{

			if ((pInst->m_iColorFlagG == 4) || (pInst->m_iColorFlagS == 4))
				iColor = 6; // white
			else 
			if (pInst->sStatus[dwIndex].ucYellowCount && pInst->sStatus[dwIndex].ucRedCount) // jfl "seal open" not driving this first selection
				iColor = 10;//red and yellow
			else if (pInst->sStatus[dwIndex].ucYellowCount)
				iColor = 3;// first yellow
			else if (pInst->sStatus[dwIndex].ucRedCount)
				iColor = 0;//red
			else if (pInst->sStatus[dwIndex].ucInstID == 6) // not yet read at all
				iColor = 6; // white
			else
				iColor = 1;//green

			hTreeItem = m_cTreeCtrl.InsertItem(_itoa(pInst->sStatus[dwIndex].uiEOSSSN,szTemp,10),iColor,iColor);

			iColor = pInst->sStatus[dwIndex].ucLatestRead;
			hTreeSubItem = m_cTreeCtrl.InsertItem("LATEST READ",iColor,iColor,hTreeItem);

			iColor = pInst->sStatus[dwIndex].ucLatestResults;
			hTreeSubItem = m_cTreeCtrl.InsertItem("LATEST RESULTS",iColor,iColor,hTreeItem);

			iColor = pInst->sStatus[dwIndex].ucKeyInfo;
			hTreeSubItem = m_cTreeCtrl.InsertItem("KEY INFO",iColor,iColor,hTreeItem);

			iColor = pInst->sStatus[dwIndex].ucVersion;
			hTreeSubItem = m_cTreeCtrl.InsertItem("VERSION",iColor,iColor,hTreeItem);

			iColor = pInst->sStatus[dwIndex].ucStatus;
			hTreeSubItem = m_cTreeCtrl.InsertItem("STATUS",iColor,iColor,hTreeItem);

			iColor = pInst->sStatus[dwIndex].ucLogInfo;
			hTreeSubItem = m_cTreeCtrl.InsertItem("LOGINFO",iColor,iColor,hTreeItem);

			iColor = pInst->sStatus[dwIndex].ucSealLog;
			hTreeSubItem = m_cTreeCtrl.InsertItem("SEALLOG",iColor,iColor,hTreeItem);

			iColor = pInst->sStatus[dwIndex].ucSOHLog;
			hTreeSubItem = m_cTreeCtrl.InsertItem("SOHLOG",iColor,iColor,hTreeItem);

			iColor = pInst->sStatus[dwIndex].ucSystemLog;
			hTreeSubItem = m_cTreeCtrl.InsertItem("SYSTEMLOG",iColor,iColor,hTreeItem);

			dwIndex++;
			if (dwIndex >= 64) bDone = true;
		}
		else
			bDone = true;
	}
}


void CEOSSDataStat::OnSelchangingTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	*pResult = 0;
}

void CEOSSDataStat::BuildPath(HTREEITEM hTreeItem, char* szFinal, char*szItem,int iLevel)
{
	char szTemp[64];
	HTREEITEM hTreeItemParent1;
	TVITEM sItem1; 

	sItem1.mask = TVIF_HANDLE | TVIF_TEXT;
	sItem1.hItem = hTreeItem;
	sItem1.state = 0;
	sItem1.stateMask = 0;
	sItem1.pszText = szTemp;
	sItem1.cchTextMax = sizeof(szTemp);
	m_cTreeCtrl.GetItem(&sItem1);

	if ((hTreeItemParent1 = m_cTreeCtrl.GetParentItem(hTreeItem)) == 0)
	{
		strcpy(szFinal,szTemp);
	}
	else
	{
		BuildPath(hTreeItemParent1,szFinal,szItem,iLevel+1); 
		if (iLevel != 0)
		{
			strcat(szFinal,"\\");
			strcat(szFinal,szTemp);
		}
	}
	if (iLevel == 0)
		strcpy(szItem,szTemp);
}

void CEOSSDataStat::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString cOut;
	CEOSSInstrument* pInst = (CEOSSInstrument*)m_pParent;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hTreeItem = m_cTreeCtrl.GetSelectedItem();

	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);

	SetDlgItemText(IDC_STATUS_TXT,"");

	if (hTreeItem != NULL)
	{

		char szFinal[MAX_PATH];
		char szItem[MAX_PATH];

		BuildPath(hTreeItem, szFinal, szItem);

		if (strcmp(szFinal,"GENERAL")==0)
		{
			
			if (strcmp(szItem,"GENERAL")==0)
				cOut += pInst->sGeneral.cAll;
			else if (strcmp(szItem,"COMPORT")==0)
				cOut += pInst->sGeneral.cCOMPORT;
			else if (strcmp(szItem,"PATH")==0)
				cOut += pInst->sGeneral.cPath;
			else if (strcmp(szItem,"FORMAT")==0)
				cOut += pInst->sGeneral.cFORMAT;
			else if (strcmp(szItem,"INSTRUMENT ID")==0)
				cOut += pInst->sGeneral.cINSTRUMENT_ID;
			else if (strcmp(szItem,"TIME OFFSET")==0)
				cOut += pInst->sGeneral.cTimeOffset;
//			else if (strcmp(szItem,"LATEST ATTEMPT")==0)
//				cOut += pInst->sGeneral.cLATEST_ATTEMPT;
			else if (strcmp(szItem,"SPECIFIED EOSSes")==0)
				cOut += pInst->sGeneral.cSPECIFIED_EOSSes;
			else if (strcmp(szItem,"LATEST READ")==0)
				cOut += pInst->sGeneral.cLATEST_READ;
			else if (strcmp(szItem,"READER VERSION")==0)
				cOut += pInst->sGeneral.cREADER_VERSION;
			else if (strcmp(szItem,"LATEST RESULTS")==0)
				cOut += pInst->sGeneral.cLATEST_RESULTS;
			//else if (strcmp(szItem,QUERY_DEPTH)==0)
			//	cOut += pInst->sGeneral.cQUERY_DEPTH;
		}
		else //must be a seal id
		{
			int iIndex;
			int iSN = atoi(szFinal);
			if (iSN != 0)
			{
				for (iIndex = 0; iIndex < 64; iIndex++)
				{
					if (iSN == (signed)pInst->EOSSes[iIndex])
						break;
				}
				if (iIndex < 64)
				{
					if (strcmp(szItem,szFinal)==0)
						cOut += pInst->sStatus[iIndex].cAll;
					else if (strcmp(szItem,"LATEST READ")==0)
						cOut += pInst->sStatus[iIndex].cLatestRead;
					else if (strcmp(szItem,"LATEST RESULTS")==0)
						cOut += pInst->sStatus[iIndex].cLatestResults;
					else if (strcmp(szItem,"KEY INFO")==0)
						cOut += pInst->sStatus[iIndex].cKeyInfo;
					else if (strcmp(szItem,"VERSION")==0)
						cOut += pInst->sStatus[iIndex].cVersion;
					else if (strcmp(szItem,"STATUS")==0)
						cOut += pInst->sStatus[iIndex].cStatus;
					else if (strcmp(szItem,"LOGINFO")==0)
						cOut += pInst->sStatus[iIndex].cLogInfo;
					else if (strcmp(szItem,"SEALLOG")==0)
						cOut += pInst->sStatus[iIndex].cSealLog;
					else if (strcmp(szItem,"SOHLOG")==0)
						cOut += pInst->sStatus[iIndex].cSOHLog;
					else if (strcmp(szItem,"SYSTEMLOG")==0)
						cOut += pInst->sStatus[iIndex].cSystemLog;
				}
			}
		}
		SetDlgItemText(IDC_STATUS_TXT,cOut);
	}
	*pResult = 0;
}

void CEOSSDataStat::OnButtonRemovedata() 
{
	CEOSSInstrument* pInst = (CEOSSInstrument*)m_pParent;
	GetDlgItem(IDC_BUTTON_REMOVEDATA)->EnableWindow(FALSE);
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	pInst->m_bRemoveOld = true;	
}

void CEOSSDataStat::OnButtonRunnow() 
{
	CEOSSInstrument* pInst = (CEOSSInstrument*)m_pParent;
	GetDlgItem(IDC_BUTTON_RUNNOW)->EnableWindow(FALSE);
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	pInst->m_QueryDepth = eReadAllData;
	pInst->m_bRunNow = true;	
}
void CEOSSDataStat::OnButtonRunnow2() 
{
	CEOSSInstrument* pInst = (CEOSSInstrument*)m_pParent;
	GetDlgItem(IDC_BUTTON_RUNNOW2)->EnableWindow(FALSE);
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	pInst->m_QueryDepth = eStatusAndLog;
	pInst->m_bRunNow = true;	
}
void CEOSSDataStat::OnButtonUpdateDataDisplay() 
{
	CEOSSInstrument* pInst = (CEOSSInstrument*)m_pParent;
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	pInst->m_bUpdateDataDisplay = true;	
}

void CEOSSDataStat::SetUpdateDataDisplay(bool value) 
{
	CEOSSInstrument* pInst = (CEOSSInstrument*)m_pParent;
	pInst->m_bUpdateDataDisplay = value;	
}

void CEOSSDataStat::OnItemexpandingTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	*pResult = 0;
}
