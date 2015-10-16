// NewILONComm.cpp : implementation file
//

#include "stdafx.h"
#include "INI_definitions.h"
#include "NewILONComm.h"
#include "CSO_definitions.h"
#include "TimedMessageDialog.h"
#include "ComILON.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                 Constructors and Destructors
//
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CNewILONComm, CDialog)

CNewILONComm::CNewILONComm():CDialog(CNewILONComm::IDD)
{
//	TRACE("CNewILONComm::CTOR #1()\n");
}
CNewILONComm::CNewILONComm(char* cIniFile,int iDlgClose,char *name)
	: CDialog(CNewILONComm::IDD)
{
//	TRACE(_T("CNewILONComm::CTOR #2()\n"));

	strncpy(m_szIniFile, cIniFile, sizeof(m_szIniFile));
	strncpy(m_pName, name, sizeof(m_pName));
	m_dDlgCloseMilliseconds = iDlgClose;

	m_dParity   = 3;
	m_dDataBits = 8;
	m_dStopBits = 1;
	m_dBaudRate = 9600;
	m_szPort[0] = NULL;
	m_pCreateButton = NULL;

	m_dCurSel_Serial = -1;

	m_csTitle = CString("ILON CSO") + CString(" - ");
	m_csTitle += CString(": Create Comm");

	//{{AFX_DATA_INIT(CNewILONComm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CNewILONComm::~CNewILONComm()
{
//	TRACE("CNewILONComm::DTOR()\n");
	delete m_pCreateButton;
}

BEGIN_MESSAGE_MAP(CNewILONComm, CDialog)
	//{{AFX_MSG_MAP(CNewILONComm)
	ON_BN_CLICKED(IDC_96E71_BUTTON, On96e71Button)
	ON_BN_CLICKED(IDC_96N81_BUTTON, On96n81Button)
	ON_BN_CLICKED(IDC_BAUD1200_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_DATABITS16_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_PARITYEVEN_RADIO, OnParityRadio)
	ON_LBN_SELCHANGE(IDC_PORT_LIST, OnSelchangePortList)
	ON_BN_CLICKED(IDC_STOPBITS1_RADIO, OnStopbitsRadio)
	ON_EN_CHANGE(IDC_EDIT_WDNODE, OnChangeEditWdnode)
	ON_BN_CLICKED(IDC_BAUD115200_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD128000_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD14400_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD1800_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD19200_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD2400_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD38400_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD4800_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD56000_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD57600_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD600_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD7200_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD9600_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_DATABITS5_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_DATABITS6_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_DATABITS7_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_DATABITS8_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_PARITYMARK_RADIO, OnParityRadio)
	ON_BN_CLICKED(IDC_PARITYNONE_RADIO, OnParityRadio)
	ON_BN_CLICKED(IDC_PARITYODD_RADIO, OnParityRadio)
	ON_BN_CLICKED(IDC_STOPBITS15_RADIO, OnStopbitsRadio)
	ON_BN_CLICKED(IDC_STOPBITS2_RADIO, OnStopbitsRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CNewILONComm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewILONComm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CNewILONComm::On96e71Button() 
{
//	TRACE("CNewILONComm::On96e71Button()\n");

	// force even parity
	((CButton*)GetDlgItem(IDC_PARITYEVEN_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_PARITYODD_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_PARITYNONE_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_PARITYMARK_RADIO))->SetCheck(0);
	
	//force 7 bits
	((CButton*)GetDlgItem(IDC_DATABITS5_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS6_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS7_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_DATABITS8_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS16_RADIO))->SetCheck(0);

	//force 1 stop bit
	((CButton*)GetDlgItem(IDC_STOPBITS1_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_STOPBITS15_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_STOPBITS2_RADIO))->SetCheck(0);

	//force 9600 baud
	((CButton*)GetDlgItem(IDC_BAUD600_RADIO))-> SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD1200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD1800_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD2400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD4800_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD7200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD9600_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_BAUD14400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD19200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD38400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD56000_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD57600_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD115200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD128000_RADIO))->SetCheck(0);

	//set our member vars appropriately 
	m_dParity = 1;
	m_dDataBits = 7;
	m_dStopBits = 1;
	m_dBaudRate = 9600;
}

void CNewILONComm::On96n81Button() 
{
//	TRACE("CNewILONComm::On96n81Button()\n");

	//force no parity
	((CButton*)GetDlgItem(IDC_PARITYEVEN_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_PARITYODD_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_PARITYNONE_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_PARITYMARK_RADIO))->SetCheck(0);

	//force 8 data bits
	((CButton*)GetDlgItem(IDC_DATABITS5_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS6_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS7_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS8_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_DATABITS16_RADIO))->SetCheck(0);

	//force 1 stop bits
	((CButton*)GetDlgItem(IDC_STOPBITS1_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_STOPBITS15_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_STOPBITS2_RADIO))->SetCheck(0);

	//force 9600 baud
	((CButton*)GetDlgItem(IDC_BAUD600_RADIO))-> SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD1200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD1800_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD2400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD4800_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD7200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD9600_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_BAUD14400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD19200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD38400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD56000_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD57600_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD115200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD128000_RADIO))->SetCheck(0);

	//set our member vars appropriately
	m_dParity = 3;
	m_dDataBits = 8;
	m_dStopBits = 1;
	m_dBaudRate = 9600;
}

void CNewILONComm::OnOK() 
{
//	TRACE("CNewILONComm::OnOK()\n");

	char buff2[32];
	CString csText;

	if (m_pName[0] == ';')
	{
		csText = CString("A \"Name\" for a communications object cannot start with \";\".");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//comm objects cannot have the same name
	GetPrivateProfileString(
		COMMUNICATIONS,
		m_pName,"ERROR",buff2,sizeof(buff2)-1,m_szIniFile);

	if (strcmp(buff2,"ERROR")!=0)
	{
		csText = CString("A Communications object by this\nname already exists!\n"\
				"Select a different name.");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//because we are using INI sections to save the information
	//we cannot have the same name as an instrument section either
	GetPrivateProfileString(
		INSTRUMENTS,
		m_pName,"ERROR",buff2,sizeof(buff2)-1,m_szIniFile);

	if (strcmp(buff2,"ERROR")!=0)
	{
		csText = CString("An Instrument object by this\nname already exists!\n"\
				"Select a different name.");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//get the port from the port list
	int port;
	if ((port=((CListBox*)GetDlgItem(IDC_PORT_LIST))->GetCurSel())== LB_ERR)
	{
		csText = CString("A Communications PORT must be selected!  ");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}


	ShowWindow(SW_SHOW);  //<<QA>> temporary until fixed below.

	//save all to ini file
	//entry in the communications for this com object
	WritePrivateProfileString(COMMUNICATIONS,m_pName,ILON_DISCOVERY_TYPE,m_szIniFile);
	//long break allowed
	WritePrivateProfileString(m_pName,LONGBREAK,"YES", m_szIniFile);
	//parity
	switch (m_dParity)
	{
			case 0:
				WritePrivateProfileString(m_pName,PARITY,"ODD",m_szIniFile);
				break;
			case 1:
				WritePrivateProfileString(m_pName,PARITY,"EVEN",m_szIniFile);
				break;
			case 2:
				WritePrivateProfileString(m_pName,PARITY,"MARK",m_szIniFile);
				break;
			default:
				WritePrivateProfileString(m_pName,PARITY,"NONE",m_szIniFile);
	}

	WritePrivateProfileString(m_pName,STOPBITS,itoa(m_dStopBits,buff2,10),m_szIniFile);
	WritePrivateProfileString(m_pName,DATABITS,itoa(m_dDataBits,buff2,10),m_szIniFile);
	WritePrivateProfileString(m_pName,BAUDRATE,itoa(m_dBaudRate,buff2,10),m_szIniFile);
	((CListBox*)GetDlgItem(IDC_PORT_LIST))->GetText(port,buff2);
	WritePrivateProfileString(m_pName,PORT,buff2,m_szIniFile);
		
	WritePrivateProfileString(
			m_pName,WATCHDOG,itoa(((CButton*)GetDlgItem(IDC_CHECK_WATCHDOG))->GetCheck(),buff2,10),m_szIniFile);
	WritePrivateProfileString(
			m_pName,WATCHDOGPORT,itoa(GetDlgItemInt(IDC_EDIT_WDNODE),buff2,10),m_szIniFile);

	CDialog::OnOK();
}

void CNewILONComm::OnBaudRadio() 
{
//	TRACE("CNewILONComm::OnBaudRadio()\n");

	// get the currently selected baud rate
	if		(((CButton*)GetDlgItem(IDC_BAUD600_RADIO))->GetCheck()==1)
		m_dBaudRate = 600;
	else if (((CButton*)GetDlgItem(IDC_BAUD1200_RADIO))->GetCheck()==1)
		m_dBaudRate = 1200;
	else if	(((CButton*)GetDlgItem(IDC_BAUD1800_RADIO))->GetCheck()==1)
		m_dBaudRate = 1800;
	else if	(((CButton*)GetDlgItem(IDC_BAUD2400_RADIO))->GetCheck()==1)
		m_dBaudRate = 2400;
	else if (((CButton*)GetDlgItem(IDC_BAUD4800_RADIO))->GetCheck()==1)
		m_dBaudRate = 4800;
	else if	(((CButton*)GetDlgItem(IDC_BAUD7200_RADIO))->GetCheck()==1)
		m_dBaudRate = 7200;
	else if	(((CButton*)GetDlgItem(IDC_BAUD9600_RADIO))->GetCheck()==1)
		m_dBaudRate = 9600;
	else if	(((CButton*)GetDlgItem(IDC_BAUD14400_RADIO))->GetCheck()==1)
		m_dBaudRate = 14400;
	else if (((CButton*)GetDlgItem(IDC_BAUD19200_RADIO))->GetCheck()==1)
		m_dBaudRate = 19200;
	else if	(((CButton*)GetDlgItem(IDC_BAUD38400_RADIO))->GetCheck()==1)
		m_dBaudRate = 38400;
	else if (((CButton*)GetDlgItem(IDC_BAUD56000_RADIO))->GetCheck()==1)
		m_dBaudRate = 56000;
	else if	(((CButton*)GetDlgItem(IDC_BAUD57600_RADIO))->GetCheck()==1)
		m_dBaudRate = 57600;
	else if	(((CButton*)GetDlgItem(IDC_BAUD115200_RADIO))->GetCheck()==1)
		m_dBaudRate = 115200;
	else if (((CButton*)GetDlgItem(IDC_BAUD128000_RADIO))->GetCheck()==1)
		m_dBaudRate = 128000;
	else 
		m_dBaudRate = 9600;
}

BOOL CNewILONComm::OnInitDialog() 
{
//	TRACE("CNewILONComm::OnInitDialog()\n");

	CDialog::OnInitDialog();

	// NO PARITY, 8 DATA BITS, 1 STOP BITS, and 9600 to start
	On96n81Button();  //parity=none, databits=8, stopbits=1, baud-9600
	PopulateSerialPortList();

	CEdit *pedit = ((CEdit*)GetDlgItem(IDC_NAME_EDIT));;
	pedit->SetWindowText(m_pName);
	pedit->EnableWindow(false);

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WD))->SetRange(1,32767);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WD))->SetPos(1);

	//create the create button
	m_pCreateButton = new CYellowButton("Create",IDOK,m_szIniFile,this);

	return TRUE;
}

void CNewILONComm::PopulateSerialPortList()
{
//	TRACE("CNewILONComm::PopulateSerialPortList()\n");

	CListBox *plistbox = (CListBox*)GetDlgItem(IDC_PORT_LIST);

	HKEY HKey;
	DWORD KeyIndex = 0;
	DWORD Type;

	//can have 256 comm items (very unlikely) in the form COMxxx
	char ValueName[2048];
	char Value[2048];
	DWORD ValueNameSize = 2047;
	DWORD ValueSize = 2047;
	//find out what com NT thinks it has 
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\SERIALCOMM",
		0,KEY_READ,&HKey)==ERROR_SUCCESS)
	{
		while (RegEnumValue(HKey, KeyIndex,	
			ValueName, &ValueNameSize, NULL,
			&Type, (unsigned char*)&Value, &ValueSize)
			==ERROR_SUCCESS)
		{
			//and add these com objects iff there isn't a comm
			//object currently using it
			ValueSize = 2047;
			ValueNameSize = 2047;
			KeyIndex++;
			//if not currently used 
			bool used = false;
			//for each comm object check to see if it uses this port
			char buff[256];
			if (GetPrivateProfileSection(COMMUNICATIONS,buff,sizeof(buff), m_szIniFile)>0)
			{
				char *at = buff;				//char pointer to step through buff

		 		//step through list of communications using a pointer into buff
				while (strlen(at) > 0) 
				{
					//getting instrument names from INI file so strip off extra stuff
					CString At(at);
					At.TrimRight();
					char temp[32];
					char buff2[32];
					strcpy(temp,At.SpanExcluding((LPCTSTR)"="));
					if (strlen(temp)>0) 
					{
						GetPrivateProfileString(temp,"PORT","",buff2,sizeof(buff2),m_szIniFile);
						if (strcmp(buff2,Value)==0)
							used = true;
					}
   				at = at + strlen(at)+1;
				}
			}
			if (!used)
				plistbox->AddString(Value);
		}
		RegCloseKey(HKey);
	}
}

void CNewILONComm::OnDatabitsRadio() 
{
//	TRACE("CNewILONComm::OnDatabitsRadio()\n");
	//user clicked a data bits radio, so find out which is the 
	//current selection
	if		(((CButton*)GetDlgItem(IDC_DATABITS5_RADIO))->GetCheck()==1)
		m_dDataBits = 5;
	else if (((CButton*)GetDlgItem(IDC_DATABITS6_RADIO))->GetCheck()==1)
		m_dDataBits = 6;
	else if	(((CButton*)GetDlgItem(IDC_DATABITS7_RADIO))->GetCheck()==1)
		m_dDataBits = 7;
	else if	(((CButton*)GetDlgItem(IDC_DATABITS8_RADIO))->GetCheck()==1)
		m_dDataBits = 8;
	else if	(((CButton*)GetDlgItem(IDC_DATABITS16_RADIO))->GetCheck()==1)
		m_dDataBits = 16;
	else 
		m_dDataBits = 8;
		
}

void CNewILONComm::OnParityRadio() 
{
//	TRACE("CNewILONComm::OnParityRadio()\n");
	// user clicked on one of the parity radio buttons 
	// so find out which is the current selection
	if		(((CButton*)GetDlgItem(IDC_PARITYODD_RADIO))->GetCheck()==1)
		m_dParity = 0;
	else if (((CButton*)GetDlgItem(IDC_PARITYEVEN_RADIO))->GetCheck()==1)
		m_dParity = 1;
	else if	(((CButton*)GetDlgItem(IDC_PARITYMARK_RADIO))->GetCheck()==1)
		m_dParity = 2;
	else // if (((CButton*)GetDlgItem(IDC_PARITYNONE_RADIO))->GetCheck()==1)
		m_dParity = 3;
}

void CNewILONComm::OnSelchangePortList() 
{
//	TRACE("CNewILONComm::OnSelchangePortList()\n");
	// user clicked on one of the port selections in the list box
	// so find out which is the current selection

	CListBox *plistbox = (CListBox*)GetDlgItem(IDC_PORT_LIST);
	int cursel = plistbox->GetCurSel();

	if (cursel == LB_ERR)
		m_pCreateButton->EnableWindow(FALSE);
	else
		m_pCreateButton->EnableWindow(TRUE);

	m_dCurSel_Serial = cursel;
}

void CNewILONComm::OnStopbitsRadio() 
{
//	TRACE("CNewILONComm::OnStopbitsRadio()\n");
	// user clicked on one of the stop bit radio buttons
	// so find out which is the current selection
	if		(((CButton*)GetDlgItem(IDC_STOPBITS1_RADIO))->GetCheck()==1)
		m_dStopBits = 1;
	else if (((CButton*)GetDlgItem(IDC_STOPBITS15_RADIO))->GetCheck()==1)
		m_dStopBits = 15;
	else if	(((CButton*)GetDlgItem(IDC_STOPBITS2_RADIO))->GetCheck()==1)
		m_dStopBits = 2;
	else 
		m_dStopBits = 1;
}

void CNewILONComm::OnChangeEditWdnode() 
{
//	TRACE("CNewILONComm::OnChangeEditWdnode()\n");

	int value = GetDlgItemInt(IDC_EDIT_WDNODE);
	if (value < 1) 
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WD))->SetPos(1);
	if (value > 32767)
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WD))->SetPos(32767);
}

