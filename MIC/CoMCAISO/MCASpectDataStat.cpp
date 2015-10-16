// MCASpecDataStat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "MCASpectDataStat.h"
#include "gamnew.h"
#include "mcainst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMCASpecDataStat property page

IMPLEMENT_DYNCREATE(CMCASpecDataStat, CPropertyPage)

CMCASpecDataStat::CMCASpecDataStat() : CPropertyPage(CMCASpecDataStat::IDD)
{
}

CMCASpecDataStat::CMCASpecDataStat(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_bBlockIncoming = false;
	m_pPropertySheet = NULL;
	m_pParent = parent;

	//initialize all displayed (cstatic) text to the
	//default color (black)
	for (int i= IDC_MCASPECDATA_FIRST; i <= IDC_MCASPECDATA_LAST; i++)
	{
		StaticColors[i-IDC_MCASPECDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CGenlStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void CMCASpecDataStat::SetPropertySheet(CMCAPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}


void CMCASpecDataStat::SetText(int index, char * text)
{
	//set the cstatic to the provided text
	if ((index >= IDC_MCASPECDATA_FIRST) &&
		(index <= IDC_MCASPECDATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *CMCASpecDataStat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_MCASPECDATA_FIRST) &&
		(index <= IDC_MCASPECDATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void CMCASpecDataStat::SetColor(int index, COLORREF color)
{
	//set the current color of a cstatic
	//it will get updated via onctlcolor()
	if ((index >= IDC_MCASPECDATA_FIRST) &&
		(index <= IDC_MCASPECDATA_LAST)) {
	  StaticColors[index-IDC_MCASPECDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CMCASpecDataStat::ResetColors()
{
	for (int i= IDC_MCASPECDATA_FIRST; i <= IDC_MCASPECDATA_LAST; i++)
	{
		StaticColors[i-IDC_MCASPECDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CMCASpecDataStat::~CMCASpecDataStat()
{
}

void CMCASpecDataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCASpecDataStat)
	DDX_Control(pDX, IDC_LIST_SPECTRA, m_cListHistory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCASpecDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CMCASpecDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CHECK_SHOW, OnShowSpectra)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SPECTRA, OnClickListSpectra)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SPECTRA, OnItemchangedListSpectra)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//	ON_BN_CLICKED(IDC_BUTTON_VIEWSPECTRA, OnButtonViewspectra)

/////////////////////////////////////////////////////////////////////////////
// CMCASpecDataStat message handlers

HBRUSH CMCASpecDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	//get the id of the control 
	UINT index = pWnd->GetDlgCtrlID();
	//set the font and color of that control
	if ((index >= IDC_MCASPECDATA_FIRST) && (index <= IDC_MCASPECDATA_LAST))
	{
		int iFirstOfList = IDC_MCASPECDATA_FIRST; 
		if (StaticColors[index-iFirstOfList]==COLOR_RED)
		{
			pDC->SetBkColor(COLOR_RED);
			pDC->SetTextColor(COLOR_WHITE);
		}
		else if (StaticColors[(index-iFirstOfList)]==COLOR_YELLOW)
		{
			pDC->SetBkColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_RED);
		}
		else
		{
			pDC->SetTextColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_BLACK);
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);

	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}
void CMCASpecDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
	m_XYCtrl.ClearChDisplay();
}

BOOL CMCASpecDataStat::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_XYCtrl.SubclassDlgItem(IDC_CUSTOM_XYCTRL,this);

	//m_XYCtrl.SetTitle("title");
	//m_XYCtrl.SetLeftLabel("left");
	//m_XYCtrl.SetBottomLabel("bottom");

	m_XYCtrl.SetTitle("");
	m_XYCtrl.SetLeftLabel("");
	m_XYCtrl.SetBottomLabel("TEST");

	if (m_pPropertySheet)
	{
		m_iShowSpectra = GetPrivateProfileInt(
			m_pParent->m_pName,"SHOW_SPECTRA",0,m_pParent->m_szIniFile);
	}
	if (m_iShowSpectra)
		SetDlgItemText(IDC_CHECK_SHOW,"Hide Spectrum.\n\nNeeds Supervisor\naccess to change.");
	else
		SetDlgItemText(IDC_CHECK_SHOW,"Show Spectrum.\n\nNeeds Supervisor\naccess to change.");

	((CButton*)GetDlgItem(IDC_CHECK_SHOW))->SetCheck(m_iShowSpectra);
//	((CButton*)GetDlgItem(IDC_BUTTON_VIEWSPECTRA))->EnableWindow(m_iShowSpectra);


/*
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	CString cList;
	CString cString(m_pPropertySheet->m_pParent->m_szSaveLoc);
	cString += "\\*.*";
	hFind = FindFirstFile(cString, &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		bool bMoreFiles = true;
		do
		{
			if (
				strstr(FindFileData.cFileName,".txt") ||
				strstr(FindFileData.cFileName,".csv") ||
				strstr(FindFileData.cFileName,".chn") ||
				strstr(FindFileData.cFileName,".TXT") ||
				strstr(FindFileData.cFileName,".CSV") ||
				strstr(FindFileData.cFileName,".CHN")
				)
			{
				cList += m_pPropertySheet->m_pParent->m_szSaveLoc;
				cList += "\\";
				cList += FindFileData.cFileName;
				cList += "\r\n";
			}
		} while (FindNextFile(hFind,&FindFileData)!=0);
		FindClose(hFind);
		((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->SetWindowText(cList);
	}
*/
	CRect cRect;
	m_cListHistory.GetClientRect(&cRect);
	ScreenToClient(&cRect);
	//adjust the scroll bars to 1/16th of the screen
	//get the old values
	static NONCLIENTMETRICS NonClientMetrics;
	NonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(
		SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS),
		&NonClientMetrics,NULL	);

	m_cListHistory.InsertColumn(0,"Path",LVCFMT_LEFT,
		cRect.Width()-NonClientMetrics.iScrollWidth);//-NonClientMetrics.iBorderWidth*3);
	OnButtonRefresh();
	//CListCtrl

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMCASpecDataStat::IssuePrint(FILE *outfile)
{
	fprintf(outfile,"SPECTRUM STATUS\n");
}

void CMCASpecDataStat::SetReceivingSpectraName(LPCSTR szString)
{
	if (!m_bBlockIncoming)
		SetDlgItemText(IDC_STATIC_SPECTRANAME,szString);
}

void CMCASpecDataStat::AddNewSpectraName(LPCSTR /*szString*/)
{
//	CString szText;
//	((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->GetWindowText(szText);
//	szText += "\r\n";
//	szText += szString;
//	if (szText.GetLength() > 10000)
//	{
//		int iFirstEOL = szText.Find("\r\n");
//		szText = szText.Mid(iFirstEOL+1);
//	}
//	((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->SetWindowText(szText);
	OnButtonRefresh();
}

void CMCASpecDataStat::OnShowSpectra()
{
	//if ok then allow change state
	if (m_pParent->Validate(true)) //superuser
	{
		if (m_iShowSpectra==1)
			m_iShowSpectra = 0;
		else
			m_iShowSpectra = 1;
		char szTemp[16];
		WritePrivateProfileString(m_pParent->m_pName,"SHOW_SPECTRA",
			itoa(m_iShowSpectra,szTemp,10),m_pParent->m_szIniFile);
	}
	ShowWindow(SW_SHOW);

	if (m_iShowSpectra)
		SetDlgItemText(IDC_CHECK_SHOW,"Hide Spectrum.\n\nNeeds Supervisor\naccess to change.");
	else
	{
		SetDlgItemText(IDC_CHECK_SHOW,"Show Spectrum.\n\nNeeds Supervisor\naccess to change.");
		m_XYCtrl.Reset();
	}

	((CButton*)GetDlgItem(IDC_CHECK_SHOW))->SetCheck(m_iShowSpectra);
//	((CButton*)GetDlgItem(IDC_BUTTON_VIEWSPECTRA))->EnableWindow(m_iShowSpectra);
}

bool CMCASpecDataStat::ProcessSpectrumBufferForRealTimeGraphic(LONG* buf,UINT uiSize)
{
	//TRACE("CMCASpecDataStat::ProcessSpectrumBufferForGraphic() ");
	if (m_iShowSpectra && !m_bBlockIncoming)
	{
		//unsigned long cColor = COLOR_YELLOW;
		static unsigned long cThresholdLineColor = COLOR_GREEN;

		m_XYCtrl.Reset();

		m_XYCtrl.DrawHorizontalGridLines(COLOR_BLACK);
		for (UINT i = 0; i < uiSize; i++)
			m_XYCtrl.AddPoint(i,buf[i]);
	}
	return true;
}
/*
void CMCASpecDataStat::OnButtonViewspectra() 
{
	// TODO: Add your control notification handler code here
	if (m_iShowSpectra)
	{
		int nLine;
		char szLine[MAX_PATH];
		nLine = ((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->LineFromChar(-1);
		int iCount = ((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->GetLine(nLine,szLine,sizeof(szLine));
		szLine[iCount] = NULL;
		//block incoming from being displayed
//		m_bBlockIncoming = true;
		long lSpectrum[1024];
//		m_XYCtrl.m_colorLine = COLOR_BLUE;
//		m_XYCtrl.m_colorDefault = COLOR_BLUE;
		m_XYCtrl.m_crPixelColor = COLOR_DARKBLUE;
		//m_crPixelColor
		CString cName("Displayed Spectrum: ");
		cName += szLine;
		SetDlgItemText(IDC_STATIC_SPECTRANAME,cName);
		if (strstr(szLine,".chn") || strstr(szLine,".CHN"))
		{
			CChnSpcEx   Spec1(1024,lSpectrum,0,NULL,NULL);
			if (Spec1.Read(szLine))
			{
				long nCh = Spec1.NumChans();
				//load and display .chn
				ProcessSpectrumBufferForRealTimeGraphic(lSpectrum,nCh);
			}
		}
		else if (strstr(szLine,".txt") || strstr(szLine,".TXT"))
		{
			//load and display .txt
			FILE *inFile;
			inFile = fopen(szLine,"rt");
			if (inFile)
			{
				int iSpectrumIndex = 0;
				while (!feof(inFile) && (iSpectrumIndex < 1024))
				{
					fscanf(inFile,"%d",&(lSpectrum[iSpectrumIndex++]));
				}
				fclose(inFile);
				//load and display .chn
				ProcessSpectrumBufferForRealTimeGraphic(lSpectrum,iSpectrumIndex);
			}
		}
		else if (strstr(szLine,".csv") || strstr(szLine,".CSV"))
		{
			//load and display .csv
			FILE *inFile;
			inFile = fopen(szLine,"rt");
			if (inFile)
			{
				int iSpectrumIndex = 0;
				while (!feof(inFile) && (iSpectrumIndex < 1024))
				{
					fscanf(inFile,"%d,",&(lSpectrum[iSpectrumIndex]));
					iSpectrumIndex++;
				}
				fclose(inFile);
				//load and display .chn
				ProcessSpectrumBufferForRealTimeGraphic(lSpectrum,iSpectrumIndex);
			}
		}
		//allow incoming to be displayed
//		m_bBlockIncoming = false;
	}
}
*/
void CMCASpecDataStat::OnButtonRefresh() 
{
	// TODO: Add your control notification handler code here
//	((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->SetWindowText("");
	m_cListHistory.DeleteAllItems();

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	int nItem;
	CString cList;
	CString cString(m_pPropertySheet->m_pParent->m_szSaveLoc);
	cString += "\\*.*";
	hFind = FindFirstFile(cString, &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE) 
	{
//		bool bMoreFiles = true;
		nItem = 0;
		do
		{
			if (
				strstr(FindFileData.cFileName,".txt") ||
				strstr(FindFileData.cFileName,".csv") ||
				strstr(FindFileData.cFileName,".chn") ||
				strstr(FindFileData.cFileName,".TXT") ||
				strstr(FindFileData.cFileName,".CSV") ||
				strstr(FindFileData.cFileName,".CHN")
				)
			{
//				cList += m_pPropertySheet->m_pParent->m_szSaveLoc;
//				cList += "\\";
//				cList += FindFileData.cFileName;
//				cList += "\r\n";

				cList = m_pPropertySheet->m_pParent->m_szSaveLoc;
				cList += "\\";
				cList += FindFileData.cFileName;
				m_cListHistory.InsertItem(nItem++, cList);
			}
		} while (FindNextFile(hFind,&FindFileData)!=0);
		FindClose(hFind);
		m_cListHistory.EnsureVisible(nItem-1,FALSE);
//		((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->SetWindowText(cList);
	}
}

void CMCASpecDataStat::OnClickListSpectra(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	int nItem;
	if (m_iShowSpectra && ((nItem = m_cListHistory.GetNextItem(-1,LVNI_SELECTED)) != -1))
	{
		CString cString = m_cListHistory.GetItemText(nItem,0);
//		int nLine;
//		char szLine[MAX_PATH];
//		nLine = ((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->LineFromChar(-1);
//		int iCount = ((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->GetLine(nLine,szLine,sizeof(szLine));
//		szLine[iCount] = NULL;
		long lSpectrum[1024];
		m_XYCtrl.m_crPixelColor = COLOR_DARKBLUE;
		//m_crPixelColor
		CString cName("Displayed Spectrum: ");
		cName += cString;
		SetDlgItemText(IDC_STATIC_SPECTRANAME,cName);

		if ((cString.Find(".chn") >= 0)|| (cString.Find(".CHN") >= 0))  //3/23/2005 PJM and DGP
		{
			CChnSpcEx   Spec1(1024,lSpectrum,0,NULL,NULL);
			if (Spec1.Read((LPSTR)LPCSTR(cString)))
			{
				long nCh = Spec1.NumChans();
				//load and display .chn
				ProcessSpectrumBufferForRealTimeGraphic(lSpectrum,nCh);
			}
		}

		else if ((cString.Find(".txt") >= 0) || (cString.Find(".TXT") >= 0))  //3/23/2005 PJM and DGP
		{
			//load and display .txt
			FILE *inFile;
			inFile = fopen(cString,"rt");
			if (inFile)
			{
				int iSpectrumIndex = 0;
				while (!feof(inFile) && (iSpectrumIndex < 1024))
				{
					fscanf(inFile,"%d",&(lSpectrum[iSpectrumIndex++]));
				}
				fclose(inFile);
				//load and display .chn
				ProcessSpectrumBufferForRealTimeGraphic(lSpectrum,iSpectrumIndex);
			}
		}

		else if ((cString.Find(".csv") >= 0) || (cString.Find(".CSV") >= 0))  //3/23/2005 PJM and DGP
		{
			//load and display .csv
			FILE *inFile;
			inFile = fopen(cString,"rt");
			if (inFile)
			{
				int iSpectrumIndex = 0;
				while (!feof(inFile) && (iSpectrumIndex < 1024))
				{
					fscanf(inFile,"%d,",&(lSpectrum[iSpectrumIndex]));
					iSpectrumIndex++;
				}
				fclose(inFile);
				//load and display .chn
				ProcessSpectrumBufferForRealTimeGraphic(lSpectrum,iSpectrumIndex);
			}
		}
		//allow incoming to be displayed
//		m_bBlockIncoming = false;
	}
		
	
	*pResult = 0;
}

void CMCASpecDataStat::OnItemchangedListSpectra(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	OnClickListSpectra(pNMHDR,pResult);
	*pResult = 0;
}

