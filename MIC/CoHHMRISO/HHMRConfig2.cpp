// HHMRConfig2.cpp

#include "stdafx.h"
#include "ISO_definitions.h"
#include "resource.h"
#include "HHMRConfig2.h"
#include "HHMRInst.h"
#include "SimpleYellowButton.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT uIDC_RECEIVE;

/////////////////////////////////////////////////////////////////////////////
// CHHMRConfig2 property page

IMPLEMENT_DYNCREATE(CHHMRConfig2, CPropertyPage)

CHHMRConfig2::CHHMRConfig2()
{
}

CHHMRConfig2::CHHMRConfig2(UINT id, CISO* pParent,bool DoingConfig) 
	: CPropertyPage(id)
{
	m_pParent = pParent;
	m_pPS = NULL;
	m_bCollectingParametersForNewISO = DoingConfig;
	m_bInitDone = false;
	m_pCameraApplyButton = NULL;

	uIDC_RECEIVE = m_pParent->m_uIDC_RECEIVE;

	//{{AFX_DATA_INIT(CHHMRConfig2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CHHMRConfig2::~CHHMRConfig2()
{
	delete m_pCameraApplyButton;
}

void CHHMRConfig2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHHMRConfig2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CHHMRConfig2, CPropertyPage)
	//{{AFX_MSG_MAP(CHHMRConfig2)
	ON_BN_CLICKED(IDC_HHMRCONFIG_APPLY,	OnHHMRConfig2Apply)
	ON_BN_CLICKED(IDC_HHMRCONFIG_DEFAULT,OnHHMRConfig2Default)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_HHMRCONFIG_CURRENT,OnHHMRConfig2Current)
	ON_BN_CLICKED(IDC_HHMRCONFIG_CAMERAAPPLY,OnHHMRCameraApply)
	ON_BN_CLICKED(IDC_BUTTON_TESTPORT, OnButtonTestport)
	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_CAMERAPORT1,		OnChangeConfigCameraPorts)
	ON_BN_CLICKED(IDC_CAMERAPORT2,		OnChangeConfigCameraPorts)
	ON_BN_CLICKED(IDC_CAMERAPORT3,		OnChangeConfigCameraPorts)
	ON_BN_CLICKED(IDC_CAMERAPORTNONE,	OnChangeConfigCameraPorts)

	ON_BN_CLICKED(IDC_CAMERATRIGREALS,	OnChangeConfigMost)
	ON_BN_CLICKED(IDC_CAMERATRIGTOTALS, OnChangeConfigMost)
	ON_BN_CLICKED(IDC_CAMERATRIGTOTALS1,OnChangeConfigMost)

	ON_EN_CHANGE(IDC_CAMERAPIN,			OnChangeConfigMost)
	ON_EN_CHANGE(IDC_HHMRCONFIG_EDIT1,	OnChangeConfigMost)
	ON_EN_CHANGE(IDC_HHMRCONFIG_EDIT2,	OnChangeConfigMost)
	ON_EN_CHANGE(IDC_HHMRCONFIG_EDIT3,	OnChangeConfigMost)
	ON_EN_CHANGE(IDC_HHMRCONFIG_EDIT4,	OnChangeConfigMost)
	ON_EN_CHANGE(IDC_HHMRCONFIG_EDIT5,	OnChangeConfigMost)
	ON_EN_CHANGE(IDC_HHMRCONFIG_EDIT6,	OnChangeConfigMost)
	ON_EN_CHANGE(IDC_HHMRCONFIG_EDIT7,	OnChangeConfigMost)
	ON_EN_CHANGE(IDC_HHMRCONFIG_EDIT8,	OnChangeConfigMost)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CHHMRConfig2::OnChangeConfigCameraPorts()
{
	//force "apply" before "test"
	GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(false);

	if (m_pCameraApplyButton)
		m_pCameraApplyButton->EnableWindow(true);

	OnChangeConfigMost();
}

void CHHMRConfig2::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPS)m_pPS->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CHHMRConfig2::SetPropertySheet(CHHMRPropertySheet *pPS, bool bDoingConfig)
{
	m_pPS = pPS;
	m_bCollectingParametersForNewISO = bDoingConfig;
}

void CHHMRConfig2::OnChangeConfigMost()
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (m_bInitDone && !m_bCollectingParametersForNewISO)
	{
		if (m_pCameraApplyButton)
			m_pCameraApplyButton->EnableWindow(true);

		//force "apply" before "test"
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(false);

		bool bEnable = (1 != ((CButton*)GetDlgItem(IDC_CAMERAPORTNONE))->GetCheck());

		GetDlgItem(IDC_CAMERAPIN)->EnableWindow(bEnable);
		GetDlgItem(IDC_CAMERAPIN_SPIN)->EnableWindow(bEnable);
		GetDlgItem(IDC_CAMERATRIGREALS)->EnableWindow(bEnable);
		GetDlgItem(IDC_CAMERATRIGTOTALS)->EnableWindow(bEnable);
		GetDlgItem(IDC_CAMERATRIGTOTALS1)->EnableWindow(bEnable);
		GetDlgItem(IDC_HHMRCONFIG_EDIT1)->EnableWindow(bEnable);
		GetDlgItem(IDC_HHMRCONFIG_EDIT2)->EnableWindow(bEnable);
		GetDlgItem(IDC_HHMRCONFIG_EDIT3)->EnableWindow(bEnable);
		GetDlgItem(IDC_HHMRCONFIG_EDIT4)->EnableWindow(bEnable);
		GetDlgItem(IDC_HHMRCONFIG_EDIT5)->EnableWindow(bEnable);
		GetDlgItem(IDC_HHMRCONFIG_EDIT6)->EnableWindow(bEnable);
		GetDlgItem(IDC_HHMRCONFIG_EDIT7)->EnableWindow(bEnable);
		GetDlgItem(IDC_HHMRCONFIG_EDIT8)->EnableWindow(bEnable);
//		GetDlgItem(IDC_HHMRCONFIG_DEFAULT)->EnableWindow(bEnable);
//		GetDlgItem(IDC_HHMRCONFIG_CURRENT)->EnableWindow(bEnable);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHHMRConfig2 message handlers
void CHHMRConfig2::DoApply()
{
	if (m_pPS)m_pPS->SetToClose(0);

	char *Name = m_pParent->m_pName;

	char Temp[16];

	bool bActivateTestButton = true;

	if (((CButton*)GetDlgItem(IDC_CAMERAPORT1))->GetCheck()==1)
		strcpy(Temp,"PORT1");
	else if	(((CButton*)GetDlgItem(IDC_CAMERAPORT2))->GetCheck()==1)
		strcpy(Temp,"PORT2");
	else if (((CButton*)GetDlgItem(IDC_CAMERAPORT3))->GetCheck()==1)
		strcpy(Temp,"PORT3");
	else
	{
		KillTimer(TRIGGER_CAMERA_END);
		strcpy(Temp,"NONE");
		bActivateTestButton = false;
	}

	WritePrivateProfileString(Name,"CAMERA_PORT",Temp, m_pParent->m_szIniFile);

	if (bActivateTestButton)
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(true);

	SaveItem(Name,"CAMERA_PIN",	IDC_CAMERAPIN);

	Temp[0] = NULL;
	if (((CButton*)GetDlgItem(IDC_CAMERATRIGTOTALS))->GetCheck()==1)
		strcat(Temp,"T");
	if (((CButton*)GetDlgItem(IDC_CAMERATRIGREALS))->GetCheck()==1)
		strcat(Temp,"R");
	if (((CButton*)GetDlgItem(IDC_CAMERATRIGTOTALS1))->GetCheck()==1)
		strcat(Temp,"1");
	WritePrivateProfileString(Name,"CAMERA_TRIGGER_TYPE",Temp, m_pParent->m_szIniFile);

	int Days = GetDlgItemInt(IDC_HHMRCONFIG_EDIT1);
	int Hours= GetDlgItemInt(IDC_HHMRCONFIG_EDIT2);
	int Min  = GetDlgItemInt(IDC_HHMRCONFIG_EDIT3);
	int Sec  = GetDlgItemInt(IDC_HHMRCONFIG_EDIT4);
	while (Sec >= 60) 
	{
		Sec -= 60;
		Min++;
	}
	while (Min >= 60)
	{
		Min -= 60;
		Hours++;
	}
	while (Hours >= 24)
	{
		Hours -= 24;
		Days++;
	}
	sprintf(Temp,"%d:%d:%d:%d",Days,Hours,Min,Sec);
	WritePrivateProfileString(Name,"CAMERA_TRIGGER_HOLDOFF",Temp, m_pParent->m_szIniFile);

	Days = GetDlgItemInt(IDC_HHMRCONFIG_EDIT5);
	Hours= GetDlgItemInt(IDC_HHMRCONFIG_EDIT6);
	Min  = GetDlgItemInt(IDC_HHMRCONFIG_EDIT7);
	Sec  = GetDlgItemInt(IDC_HHMRCONFIG_EDIT8);
	while (Sec >= 60) 
	{
		Sec -= 60;
		Min++;
	}
	while (Min >= 60)
	{
		Min -= 60;
		Hours++;
	}
	while (Hours >= 24)
	{
		Hours -= 24;
		Days++;
	}
	sprintf(Temp,"%d:%d:%d:%d",Days,Hours,Min,Sec);
	WritePrivateProfileString(Name,"CAMERA_TRIGGER_SUPPRESS",Temp, m_pParent->m_szIniFile);
}

void CHHMRConfig2::OnHHMRCameraApply()
{
	if (m_pPS)m_pPS->SetToClose(0);

	//if we are not doing initial configuration continue
	if (!m_bCollectingParametersForNewISO)
	{
		//do security
		if (m_pParent->Validate())
		{
			if (m_pCameraApplyButton)
				m_pCameraApplyButton->EnableWindow(false);
			//GetDlgItem(IDC_HHMRCONFIG_CAMERAAPPLY)->EnableWindow(false);
			//save stuff to ini file
			DoApply();
			//save stuff to MIC itself
			m_pPS->SetCamera();
			//update parallel port stuff
		}
	}
}

void CHHMRConfig2::OnHHMRConfig2Apply() 
{
	if(m_pPS)m_pPS->SetToClose(0);

	if (m_pParent->Validate())
	{
		((CHHMRModParam*)m_pPS->GetPage(0))->DoApply();
		DoApply();
	}
}

void CHHMRConfig2::BlockCamera()
{
	((CButton*)GetDlgItem(IDC_CAMERAPORT1))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CAMERAPORT2))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CAMERAPORT3))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CAMERAPORTNONE))->SetCheck(1);
	GetDlgItem(IDC_CAMERAPORT1)->EnableWindow(false);
	GetDlgItem(IDC_CAMERAPORT2)->EnableWindow(false);
	GetDlgItem(IDC_CAMERAPORT3)->EnableWindow(false);
	GetDlgItem(IDC_CAMERAPORTNONE)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(false);
	GetDlgItem(IDC_CAMERAPIN)->EnableWindow(false);
	GetDlgItem(IDC_CAMERAPIN_SPIN)->EnableWindow(false);
	GetDlgItem(IDC_CAMERATRIGREALS)->EnableWindow(false);
	GetDlgItem(IDC_CAMERATRIGTOTALS)->EnableWindow(false);
	GetDlgItem(IDC_CAMERATRIGTOTALS1)->EnableWindow(false);
	GetDlgItem(IDC_HHMRCONFIG_EDIT1)->EnableWindow(false);
	GetDlgItem(IDC_HHMRCONFIG_EDIT2)->EnableWindow(false);
	GetDlgItem(IDC_HHMRCONFIG_EDIT3)->EnableWindow(false);
	GetDlgItem(IDC_HHMRCONFIG_EDIT4)->EnableWindow(false);
	GetDlgItem(IDC_HHMRCONFIG_EDIT5)->EnableWindow(false);
	GetDlgItem(IDC_HHMRCONFIG_EDIT6)->EnableWindow(false);
	GetDlgItem(IDC_HHMRCONFIG_EDIT7)->EnableWindow(false);
	GetDlgItem(IDC_HHMRCONFIG_EDIT8)->EnableWindow(false);
}


void CHHMRConfig2::SaveItem(char *szName, char *szItem, int dDlgItem)
{
	char temp[MAX_PATH+1];
	GetDlgItem(dDlgItem)->GetWindowText(temp,sizeof(temp));
	WritePrivateProfileString(szName, szItem, temp, m_pParent->m_szIniFile);
}

void CHHMRConfig2::OnHHMRConfig2Current() 
{
	if (m_pPS)m_pPS->SetToClose(0);

	char *Name = m_pParent->m_pName;

	//get all the stuff and save it
	OnHHMRConfig(Name);

	if (m_pCameraApplyButton)
		m_pCameraApplyButton->EnableWindow(false);

	if (((CButton*)GetDlgItem(IDC_CAMERAPORTNONE))->GetCheck()==1)
	{
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(false);
		GetDlgItem(IDC_CAMERAPIN)->EnableWindow(false);
		GetDlgItem(IDC_CAMERAPIN_SPIN)->EnableWindow(false);
		GetDlgItem(IDC_CAMERATRIGREALS)->EnableWindow(false);
		GetDlgItem(IDC_CAMERATRIGTOTALS)->EnableWindow(false);
		GetDlgItem(IDC_CAMERATRIGTOTALS1)->EnableWindow(false);
		GetDlgItem(IDC_HHMRCONFIG_EDIT1)->EnableWindow(false);
		GetDlgItem(IDC_HHMRCONFIG_EDIT2)->EnableWindow(false);
		GetDlgItem(IDC_HHMRCONFIG_EDIT3)->EnableWindow(false);
		GetDlgItem(IDC_HHMRCONFIG_EDIT4)->EnableWindow(false);
		GetDlgItem(IDC_HHMRCONFIG_EDIT5)->EnableWindow(false);
		GetDlgItem(IDC_HHMRCONFIG_EDIT6)->EnableWindow(false);
		GetDlgItem(IDC_HHMRCONFIG_EDIT7)->EnableWindow(false);
		GetDlgItem(IDC_HHMRCONFIG_EDIT8)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(true);
		GetDlgItem(IDC_CAMERAPIN)->EnableWindow(true);
		GetDlgItem(IDC_CAMERAPIN_SPIN)->EnableWindow(true);
		GetDlgItem(IDC_CAMERATRIGREALS)->EnableWindow(true);
		GetDlgItem(IDC_CAMERATRIGTOTALS)->EnableWindow(true);
		GetDlgItem(IDC_CAMERATRIGTOTALS1)->EnableWindow(true);
		GetDlgItem(IDC_HHMRCONFIG_EDIT1)->EnableWindow(true);
		GetDlgItem(IDC_HHMRCONFIG_EDIT2)->EnableWindow(true);
		GetDlgItem(IDC_HHMRCONFIG_EDIT3)->EnableWindow(true);
		GetDlgItem(IDC_HHMRCONFIG_EDIT4)->EnableWindow(true);
		GetDlgItem(IDC_HHMRCONFIG_EDIT5)->EnableWindow(true);
		GetDlgItem(IDC_HHMRCONFIG_EDIT6)->EnableWindow(true);
		GetDlgItem(IDC_HHMRCONFIG_EDIT7)->EnableWindow(true);
		GetDlgItem(IDC_HHMRCONFIG_EDIT8)->EnableWindow(true);
	}
}

void CHHMRConfig2::OnHHMRConfig2Default()
{
	if (m_pPS)
		m_pPS->SetToClose(0);

	OnHHMRConfig(HHMR_DEFAULT);

	bool bEnable;

	if (m_bCollectingParametersForNewISO)
	{
		bEnable = true;
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(false);
	}
	else
	{
		if (m_pCameraApplyButton)
			m_pCameraApplyButton->EnableWindow(true);

		bEnable = ((CButton*)GetDlgItem(IDC_CAMERAPORTNONE))->GetCheck()!=1;
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(bEnable);
	}

	//GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(bEnable);
	GetDlgItem(IDC_CAMERAPIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_CAMERAPIN_SPIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_CAMERATRIGREALS)->EnableWindow(bEnable);
	GetDlgItem(IDC_CAMERATRIGTOTALS)->EnableWindow(bEnable);
	GetDlgItem(IDC_CAMERATRIGTOTALS1)->EnableWindow(bEnable);
	GetDlgItem(IDC_HHMRCONFIG_EDIT1)->EnableWindow(bEnable);
	GetDlgItem(IDC_HHMRCONFIG_EDIT2)->EnableWindow(bEnable);
	GetDlgItem(IDC_HHMRCONFIG_EDIT3)->EnableWindow(bEnable);
	GetDlgItem(IDC_HHMRCONFIG_EDIT4)->EnableWindow(bEnable);
	GetDlgItem(IDC_HHMRCONFIG_EDIT5)->EnableWindow(bEnable);
	GetDlgItem(IDC_HHMRCONFIG_EDIT6)->EnableWindow(bEnable);
	GetDlgItem(IDC_HHMRCONFIG_EDIT7)->EnableWindow(bEnable);
	GetDlgItem(IDC_HHMRCONFIG_EDIT8)->EnableWindow(bEnable);
}

void CHHMRConfig2::OnHHMRConfig(char *Name)
{
	char temp[_MAX_PATH];

	GetPrivateProfileString(
		Name,"CAMERA_PORT","NONE",temp,sizeof(temp),m_pParent->m_szIniFile);
	
	if (strcmp(temp,"PORT1")==0)
	{
		((CButton*)GetDlgItem(IDC_CAMERAPORT1))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_CAMERAPORT2))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_CAMERAPORT3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_CAMERAPORTNONE))->SetCheck(0);
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(true);
	}
	else if (strcmp(temp,"PORT2")==0)
	{
		((CButton*)GetDlgItem(IDC_CAMERAPORT1))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_CAMERAPORT2))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_CAMERAPORT3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_CAMERAPORTNONE))->SetCheck(0);
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(true);
	}
	else if (strcmp(temp,"PORT3")==0)
	{
		((CButton*)GetDlgItem(IDC_CAMERAPORT1))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_CAMERAPORT2))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_CAMERAPORT3))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_CAMERAPORTNONE))->SetCheck(0);
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(true);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CAMERAPORT1))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_CAMERAPORT2))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_CAMERAPORT3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_CAMERAPORTNONE))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(true);
	}

	int Pin = GetPrivateProfileInt(Name,"CAMERA_PIN",2,m_pParent->m_szIniFile);
	if ((Pin < 0)||(Pin > 25))
	{
		WritePrivateProfileString(Name,"CAMERA_PIN","2",m_pParent->m_szIniFile);
		Pin = 2;
	}
	((CSpinButtonCtrl*)GetDlgItem(IDC_CAMERAPIN_SPIN))->SetPos(Pin);

	GetPrivateProfileString(Name,"CAMERA_TRIGGER_TYPE","",temp,sizeof(temp),m_pParent->m_szIniFile);
	((CButton*)GetDlgItem(IDC_CAMERATRIGREALS))->SetCheck(strchr(temp,'R')?1:0);
	((CButton*)GetDlgItem(IDC_CAMERATRIGTOTALS))->SetCheck(strchr(temp,'T')?1:0);
	((CButton*)GetDlgItem(IDC_CAMERATRIGTOTALS1))->SetCheck(strchr(temp,'1')?1:0);

//	int TrigThreshRate = GetPrivateProfileInt(Name,"CAMERA_TRIGGER_TRESHOLD_RATE",50,m_szIniFile);
//	if (TrigThreshRate < 0) TrigThreshRate = 0;
//	SetDlgItemInt(IDC_CAMERATRIGTHRESH,TrigThreshRate);	
	
	int Day,Hour,Min,Sec;
	GetPrivateProfileString(Name,"CAMERA_TRIGGER_HOLDOFF","0:0:0:5",temp,sizeof(temp),m_pParent->m_szIniFile);
	if (sscanf(temp,"%d:%d:%d:%d",&Day,&Hour,&Min,&Sec) != 4)
		Day = Hour = Min = Sec = 0;
	while (Sec >= 60) 
	{
		Sec -= 60;
		Min++;
	}
	while (Min >= 60)
	{
		Min -= 60;
		Hour++;
	}
	while (Hour >= 24)
	{
		Hour -= 24;
		Day++;
	}
	SetDlgItemInt(IDC_HHMRCONFIG_EDIT1,Day);
	SetDlgItemInt(IDC_HHMRCONFIG_EDIT2,Hour);
	SetDlgItemInt(IDC_HHMRCONFIG_EDIT3,Min);
	SetDlgItemInt(IDC_HHMRCONFIG_EDIT4,Sec);

	GetPrivateProfileString(Name,"CAMERA_TRIGGER_SUPPRESS","0:0:10:0",temp,sizeof(temp),m_pParent->m_szIniFile);
	if (sscanf(temp,"%d:%d:%d:%d",&Day,&Hour,&Min,&Sec) != 4)
		Day = Hour = Min = Sec = 0;
	while (Sec >= 60) 
	{
		Sec -= 60;
		Min++;
	}
	while (Min >= 60)
	{
		Min -= 60;
		Hour++;
	}
	while (Hour >= 24)
	{
		Hour -= 24;
		Day++;
	}
	SetDlgItemInt(IDC_HHMRCONFIG_EDIT5,Day);
	SetDlgItemInt(IDC_HHMRCONFIG_EDIT6,Hour);
	SetDlgItemInt(IDC_HHMRCONFIG_EDIT7,Min);
	SetDlgItemInt(IDC_HHMRCONFIG_EDIT8,Sec);
}

BOOL CHHMRConfig2::OnSetActive() 
{
//	TRACE("CHHMRConfig2::OnSetActive\n");

	if (m_bCollectingParametersForNewISO)
	{
		if (m_pCameraApplyButton)
			m_pCameraApplyButton->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BUTTON_TESTPORT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_MUST_BE)->SetWindowText("");

		GetDlgItem(IDC_CAMERAPIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_CAMERAPIN_SPIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_CAMERATRIGREALS)->EnableWindow(TRUE);
		GetDlgItem(IDC_CAMERATRIGTOTALS)->EnableWindow(TRUE);
		GetDlgItem(IDC_CAMERATRIGTOTALS1)->EnableWindow(TRUE);
		GetDlgItem(IDC_HHMRCONFIG_EDIT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_HHMRCONFIG_EDIT2)->EnableWindow(TRUE);
		GetDlgItem(IDC_HHMRCONFIG_EDIT3)->EnableWindow(TRUE);
		GetDlgItem(IDC_HHMRCONFIG_EDIT4)->EnableWindow(TRUE);
		GetDlgItem(IDC_HHMRCONFIG_EDIT5)->EnableWindow(TRUE);
		GetDlgItem(IDC_HHMRCONFIG_EDIT6)->EnableWindow(TRUE);
		GetDlgItem(IDC_HHMRCONFIG_EDIT7)->EnableWindow(TRUE);
		GetDlgItem(IDC_HHMRCONFIG_EDIT8)->EnableWindow(TRUE);

		GetDlgItem(IDC_HHMRCONFIG_CURRENT)->ShowWindow(SW_HIDE);
		m_pPS->GetDlgItem(ID_WIZBACK)->ShowWindow(SW_SHOW);
		m_pPS->GetDlgItem(ID_WIZBACK)->EnableWindow(TRUE);
		m_pPS->GetDlgItem(ID_WIZNEXT)->ShowWindow(SW_HIDE);
		m_pPS->GetDlgItem(ID_WIZFINISH)->ShowWindow(SW_SHOW);
		m_pPS->GetDlgItem(ID_WIZFINISH)->EnableWindow(TRUE);

		//SCR 118
		//m_pParent->EstablishVersionString();
	}
	else
	{
		if (m_pCameraApplyButton)
			m_pCameraApplyButton->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_HHMRCONFIG_CURRENT)->ShowWindow(SW_SHOW);
	}
	BOOL result = CPropertyPage::OnSetActive();
	return result;
}

BOOL CHHMRConfig2::OnWizardFinish() 
{
	//This is only called in the temporary ISO
//	ASSERT(m_bCollectingParametersForNewISO);

	((CHHMRModParam*)m_pPS->GetPage(0))->DoApply();
	DoApply();
	return CPropertyPage::OnWizardFinish();
}

BOOL CHHMRConfig2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_CAMERAPIN))->SetLimitText(2);
	((CSpinButtonCtrl*)GetDlgItem(IDC_CAMERAPIN_SPIN))->SetRange(0,25);
//	((CEdit*)GetDlgItem(IDC_CAMERATRIGTHRESH))->SetLimitText(4);

	((CEdit*)GetDlgItem(IDC_HHMRCONFIG_EDIT1))->SetLimitText(2);
	((CEdit*)GetDlgItem(IDC_HHMRCONFIG_EDIT2))->SetLimitText(2);
	((CEdit*)GetDlgItem(IDC_HHMRCONFIG_EDIT3))->SetLimitText(2);
	((CEdit*)GetDlgItem(IDC_HHMRCONFIG_EDIT4))->SetLimitText(2);

	((CEdit*)GetDlgItem(IDC_HHMRCONFIG_EDIT5))->SetLimitText(2);
	((CEdit*)GetDlgItem(IDC_HHMRCONFIG_EDIT6))->SetLimitText(2);
	((CEdit*)GetDlgItem(IDC_HHMRCONFIG_EDIT7))->SetLimitText(2);
	((CEdit*)GetDlgItem(IDC_HHMRCONFIG_EDIT8))->SetLimitText(2);

	m_bInitDone = true;

	if (m_bCollectingParametersForNewISO)
		OnHHMRConfig2Default();
	else
		OnHHMRConfig2Current();

	m_pCameraApplyButton = new CSimpleYellowButton(  //SCR00114
		"Apply",
		IDC_HHMRCONFIG_CAMERAAPPLY,
		this);

	//Get the "Test Port and Pin" button squared away

	bool bActivateTestButton =  
		(((CButton*)GetDlgItem(IDC_CAMERAPORTNONE))->GetCheck()!=1);

	GetDlgItem(IDC_BUTTON_TESTPORT)->EnableWindow(bActivateTestButton);

	return TRUE;
}

void CHHMRConfig2::OnButtonTestport() 
{
	m_pParent->PostMessage(uIDC_RECEIVE, TRIGGER_CAMERA_START_TEST);
	//start camera trigger
	//SetTimer(TRIGGER_CAMERA_END,800,NULL);
}

void CHHMRConfig2::OnTimer(UINT nIDEvent) 
{
	//if (nIDEvent == TRIGGER_CAMERA_END)
	//{
		//KillTimer(TRIGGER_CAMERA_END);//TIMER_CAMERA_END);
		//m_pParent->PostMessage(uIDC_RECEIVE, TRIGGER_CAMERA_END);
	//}

	CPropertyPage::OnTimer(nIDEvent);
}

void CHHMRConfig2::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here

	CString cs1 = "Version " + m_pParent->m_csVersion;
	CString cs2 = CString( __DATE__ );

	SetDlgItemText(IDC_STATIC_VERSION, cs1);
	SetDlgItemText(IDC_STATIC_DATE, cs2);

#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG, "Release Build");
#endif	
}

