// EOSSCSO.cpp : Implementation of CEOSSCSO

#include "stdafx.h"
#include "CoEOSSCSO.h"
#include "EOSSCSO.h"
#include "NewEOSSComm.h"
#include "ini_definitions.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//local windows registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB; 
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;

/////////////////////////////////////////////////////////////////////////////
// CEOSSCSO

//1
STDMETHODIMP CEOSSCSO::CollectParametersForNewCSO(
	/*[out]*/ BSTR *pbstrNameOut, 
	/*[out]*/ VARIANT_BOOL *pvbHaveNewCSO)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString csNameOut;

	bool bHaveNewCSO = m_pcomm->CollectParametersForNewCSO(csNameOut);

	if (bHaveNewCSO)
	{
		*pvbHaveNewCSO = VARIANT_TRUE;
		const char *string_in = LPCTSTR(csNameOut);
		BSTR bstr_out;
		Convert_Character_String_to_BSTR((char *)string_in, bstr_out);
		*pbstrNameOut = bstr_out;
	}
	else
	{
		*pvbHaveNewCSO = VARIANT_FALSE;
		*pbstrNameOut = BSTR("");
	}
	return S_OK;
}

//2
STDMETHODIMP CEOSSCSO::EnableHeartbeat(/*[in]*/VARIANT_BOOL vbEnable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->WatchDogOk(vbEnable == VARIANT_TRUE);
	return S_OK;
}

//3
STDMETHODIMP CEOSSCSO::get_CSOTypeString(BSTR *pbstrType)
{
//	TRACE("CEOSSCSO::get_CSOTypeString()\n");
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *string_in = m_pcomm->GetType();
	Convert_Character_String_to_BSTR(string_in, *pbstrType);
	return S_OK;
}

//4
STDMETHODIMP CEOSSCSO::get_LastError(/*[out]*/ BSTR *pbstrErrorString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	BSTR bstr_out;
	CString *pcs = m_pcomm->GetLastError();
	const char *pch = LPCTSTR(*pcs);
	Convert_Character_String_to_BSTR((char *)pch, bstr_out);
	pbstrErrorString = &bstr_out;

	return S_OK;
}

//5
STDMETHODIMP CEOSSCSO::get_Name(/*[out]*/ BSTR *pbstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *string_in = m_pcomm->GetName();
	Convert_Character_String_to_BSTR(string_in, *pbstrName);

	return S_OK;
}

//6
STDMETHODIMP CEOSSCSO::get_VersionString(/*[out]*/ BSTR *pbstrVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	//CVersionInfo VersionInfo;
	*pbstrVersion = m_pcomm->m_csNameVersionDate.AllocSysString(); //SCR 118
	return S_OK;
}


//7
STDMETHODIMP CEOSSCSO::put_DialogAutoCloseTime(/*[in]*/ ULONG seconds)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->PutDialogCloseTime(1000*(int)seconds);
	return S_OK;
}

//8
STDMETHODIMP CEOSSCSO::put_INIPathname(/*[in]*/ BSTR bstrINIPathname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(
		bstrINIPathname,string_out, sizeof(string_out));
	m_pcomm->PutIniFileName(string_out);

	return S_OK;
}

//9
STDMETHODIMP CEOSSCSO::put_Name(/*[in]*/BSTR bstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(bstrName, string_out, sizeof(string_out));
	strncpy(m_pcomm->m_pName,string_out,sizeof(m_pcomm->m_pName));

	return S_OK;
}

//10
STDMETHODIMP CEOSSCSO::put_WindowsRegisteredMessageStrings(
			/*[in]*/ ULONG ulRECEIVE,
			/*[in]*/ ULONG ulRECEIVE_OOB,
			/*[in]*/ ULONG ulSTART,
			/*[in]*/ ULONG ulSTOP,
			/*[in]*/ ULONG ulRESET)
{
//	TRACE("CEOSSCSO::put_WindowsRegisteredMessageStrings()\n");
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_pcomm->SetWindowsRegisteredMessageStrings(
		ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET);

	//we need some of the unique message values locally at this level, too.
	uIDC_RECEIVE         = ulRECEIVE;
	uIDC_RECEIVE_OOB     = ulRECEIVE_OOB;
	uIDC_COM_START       = ulSTART;
	uIDC_COM_STOP        = ulSTOP;
	uIDC_COM_RESET       = ulRESET;

	return S_OK;
}

//11
STDMETHODIMP CEOSSCSO::put_ThresholdValue(/*[in]*/ ULONG threshold_value)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->SetThresholdValue((UINT)threshold_value);
	return S_OK;
}

//12
STDMETHODIMP CEOSSCSO::RegisterISO(
			/*[in]*/ ULONG receiver_handle,
			/*[in]*/ ULONG Node,
			/*[in]*/ ISO_FUNCTIONAL_CATEGORY cat,
			/*[in]*/ VARIANT_BOOL vb_OOB, 
			/*[out]*/ VARIANT_BOOL *pvbResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	bool bresult = false;
	bool boutofband = (vb_OOB == VARIANT_TRUE);

	if ((cat == STANDARD_ISO) || (cat == UPS_ISO))
	{
		bresult = m_pcomm->Register(
			(HWND)receiver_handle, 
			boutofband,
			(int)Node);  
	}
	else if (cat == WATCH_ISO)
	{
		bresult = m_pcomm->RegisterWatch(
			(HWND)receiver_handle,  
			(int)Node);
	}
	if (pvbResult != NULL)
		*pvbResult = bresult?VARIANT_TRUE:VARIANT_FALSE;

	return S_OK;
}

//13
STDMETHODIMP CEOSSCSO::ResetCSO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->PostThreadMessage(uIDC_COM_RESET,0,0);
	return S_OK;
}

//14
STDMETHODIMP CEOSSCSO::SendHeartbeat()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->WatchDogSend();
	return S_OK;
}

//15
STDMETHODIMP CEOSSCSO::SendToWatch(/*[in]*/ BSTR message, /*[in]*/ ULONG Node)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char string_out[128];
	Convert_BSTR_to_Character_String(message,string_out,sizeof(string_out));
	int bytes = strlen(string_out);  //<<QA>> is this checking really necessary??
	if (bytes > sizeof(string_out))
		bytes = sizeof(string_out);
	m_pcomm->SendToWatch(bytes, string_out, int(Node));

	return S_OK;
}

//16
STDMETHODIMP CEOSSCSO::SetUpPort(
	/*[in]*/ BSTR portname,
	/*[out]*/ VARIANT_BOOL *pvbResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char string_out[32];
	Convert_BSTR_to_Character_String(portname,string_out,sizeof(string_out));
	bool success = m_pcomm->SetUpPort(string_out);
	*pvbResult = success ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

//17
STDMETHODIMP CEOSSCSO::ShutdownCSO()
{
//	TRACE("CEOSSCSO::ShutdownCSO()\n");
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_pcomm->DoStop();
	m_pcomm->PostThreadMessage(uIDC_COM_STOP,0,0);

	return S_OK;
}

//18
STDMETHODIMP CEOSSCSO::StartCSO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->PostThreadMessage(uIDC_COM_START,0,0);
	return S_OK;
}

//19
STDMETHODIMP CEOSSCSO::Transmit(
			/*[in]*/ ULONG length,
			/*[in]*/ BLOB data, 
			/*[in]*/ ULONG ulnode, 
			/*[in]*/ VARIANT_BOOL vbQuiet, 
			/*[out]*/ ULONG *pBytesWritten)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	int bytes = m_pcomm->Transmit(
		(int)length,                   //ilength (must allow for -1)
		(char*)data.pBlobData,         // Output 
		(int)ulnode,                   // Node 
		(vbQuiet == VARIANT_TRUE ));   // looking for a simple bool

	*pBytesWritten = bytes;

	return S_OK;
}

//20
STDMETHODIMP CEOSSCSO::UnRegisterISO(/*[in]*/ ULONG Node, /*[in]*/ ISO_FUNCTIONAL_CATEGORY cat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if ((cat == STANDARD_ISO) || (cat == UPS_ISO))
		m_pcomm->UnRegister((int)Node);
	else if (cat == WATCH_ISO)
		m_pcomm->UnRegisterWatch((int)Node);

	return S_OK;
}
