// Config2.cpp : implementation file
//

#include "stdafx.h"
#include "Tracker.h"
#include "Config2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfig2 dialog


CConfig2::CConfig2(CWnd* pParent /*=NULL*/)
	: CDialog(CConfig2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfig2)
	m_cWidth = _T("");
	m_cHeight = _T("");
	m_cColumns = _T("");
	m_cFlagSize = _T("");
	//}}AFX_DATA_INIT

	m_dButtonWidth=175; 
	m_dButtonHeight=75;
	m_dButtonsWide=1;
	m_dButtonFlagSize=40;
}


void CConfig2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfig2)
	DDX_Control(pDX, IDC_SPIN4, m_pSpinFlagSize);
	DDX_Control(pDX, IDC_SPIN3, m_pSpinColumns);
	DDX_Control(pDX, IDC_SPIN2, m_pSpinHeight);
	DDX_Control(pDX, IDC_SPIN1, m_pSpinWidth);
	DDX_Text(pDX, IDC_EDIT1, m_cWidth);
	DDV_MaxChars(pDX, m_cWidth, 3);
	DDX_Text(pDX, IDC_EDIT2, m_cHeight);
	DDV_MaxChars(pDX, m_cHeight, 3);
	DDX_Text(pDX, IDC_EDIT3, m_cColumns);
	DDV_MaxChars(pDX, m_cColumns, 3);
	DDX_Text(pDX, IDC_EDIT4, m_cFlagSize);
	DDV_MaxChars(pDX, m_cFlagSize, 3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfig2, CDialog)
	//{{AFX_MSG_MAP(CConfig2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfig2 message handlers
void CConfig2::SetValues(int Width, int Height, int Columns, int Flags)
{
	m_dButtonWidth = Width;
	m_dButtonHeight = Height;
	m_dButtonsWide = Columns;
	m_dButtonFlagSize = Flags;
}
void CConfig2::GetValues(int *Width, int *Height, int *Columns, int *Flags)
{
	*Width = atoi(m_cWidth);
	*Height = atoi(m_cHeight);
	*Columns = atoi(m_cColumns);
	*Flags = atoi(m_cFlagSize);
}

BOOL CConfig2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pSpinWidth.SetRange(1,499);
	m_pSpinHeight.SetRange(1,499);
	m_pSpinColumns.SetRange(1,499);
	m_pSpinFlagSize.SetRange(1,499);

	m_pSpinWidth.SetPos(m_dButtonWidth); 
	m_pSpinHeight.SetPos(m_dButtonHeight);
	m_pSpinColumns.SetPos(m_dButtonsWide);
	m_pSpinFlagSize.SetPos(m_dButtonFlagSize);
			
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

