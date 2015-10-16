//IPXCSO.cpp

// IPXCSO.cpp : Implementation of CIPXCSO
#include "stdafx.h"
#include "CoIPXCSO.h"
#include "IPXCSO.h"
#include "NewIPXDialog.h"
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
// CIPXCSO

//1
STDMETHODIMP CIPXCSO::CollectParametersForNewCSO(
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
STDMETHODIMP CIPXCSO::EnableHeartbeat(/*[in]*/VARIANT_BOOL vbEnable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->WatchDogOk(vbEnable == VARIANT_TRUE);
	return S_OK;
}

//3
STDMETHODIMP CIPXCSO::get_CSOTypeString(BSTR *pbstrType)
{
//	TRACE("CIPXCSO::get_CSOTypeString()\n");
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *string_in = m_pcomm->GetType();
	Convert_Character_String_to_BSTR(string_in, *pbstrType);
	return S_OK;
}

//4
STDMETHODIMP CIPXCSO::get_LastError(/*[out]*/ BSTR *pbstrErrorString)
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
STDMETHODIMP CIPXCSO::get_Name(/*[out]*/ BSTR *pbstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *string_in = m_pcomm->GetName();
	Convert_Character_String_to_BSTR(string_in, *pbstrName);

	return S_OK;
}

//6
STDMETHODIMP CIPXCSO::get_VersionString(/*[out]*/ BSTR *pbstrVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	//CVersionInfo VersionInfo;
	*pbstrVersion = m_pcomm->m_csNameVersionDate.AllocSysString(); //SCR 118
	return S_OK;
}


//7
STDMETHODIMP CIPXCSO::put_DialogAutoCloseTime(/*[in]*/ ULONG seconds)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->PutDialogCloseTime(1000*(int)seconds);
	return S_OK;
}

//8
STDMETHODIMP CIPXCSO::put_INIPathname(/*[in]*/ BSTR bstrINIPathname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(
		bstrINIPathname,string_out, sizeof(string_out));
	m_pcomm->PutIniFileName(string_out);

	return S_OK;
}

//9
STDMETHODIMP CIPXCSO::put_Name(/*[in]*/BSTR bstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(bstrName, string_out, sizeof(string_out));
	strncpy(m_pcomm->m_pName,string_out,sizeof(m_pcomm->m_pName));

	return S_OK;
}

//10
STDMETHODIMP CIPXCSO::put_WindowsRegisteredMessageStrings(
			/*[in]*/ ULONG ulRECEIVE,
			/*[in]*/ ULONG ulRECEIVE_OOB,
			/*[in]*/ ULONG ulSTART,
			/*[in]*/ ULONG ulSTOP,
			/*[in]*/ ULONG ulRESET)
{
//	TRACE("CIPXCSO::put_WindowsRegisteredMessageStrings()\n");
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
STDMETHODIMP CIPXCSO::put_ThresholdValue(/*[in]*/ ULONG threshold_value)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->SetThresholdValue((UINT)threshold_value);
	return S_OK;
}

//12
STDMETHODIMP CIPXCSO::RegisterISO(
			/*[in]*/ ULONG receiver_handle,
			/*[in]*/ ULONG Node,
			/*[in]*/ ISO_FUNCTIONAL_CATEGORY cat,
			/*[in]*/ VARIANT_BOOL vb_OOB, 
			/*[out]*/ VARIANT_BOOL *pvbResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	bool bresult = false;
	bool boutofbounds = (vb_OOB == VARIANT_TRUE);

	if ((cat == STANDARD_ISO) || (cat == UPS_ISO))
	{
		bresult = m_pcomm->Register(
			(HWND)receiver_handle, 
			boutofbounds, 
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
STDMETHODIMP CIPXCSO::ResetCSO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->PostThreadMessage(uIDC_COM_RESET,0,0);
	return S_OK;
}

//14
STDMETHODIMP CIPXCSO::SendHeartbeat()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->WatchDogSend();
	return S_OK;
}

//15
STDMETHODIMP CIPXCSO::SendToWatch(/*[in]*/ BSTR message, /*[in]*/ ULONG Node)
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
STDMETHODIMP CIPXCSO::SetUpPort(
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
STDMETHODIMP CIPXCSO::ShutdownCSO()
{
//	TRACE("CIPXCSO::ShutdownCSO()\n");
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_pcomm->DoStop();
	m_pcomm->PostThreadMessage(uIDC_COM_STOP,0,0);

	return S_OK;
}

//18
STDMETHODIMP CIPXCSO::StartCSO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_pcomm->PostThreadMessage(uIDC_COM_START,0,0);
	return S_OK;
}

//19
STDMETHODIMP CIPXCSO::Transmit(
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
STDMETHODIMP CIPXCSO::UnRegisterISO(/*[in]*/ ULONG Node, /*[in]*/ ISO_FUNCTIONAL_CATEGORY cat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if ((cat == STANDARD_ISO) || (cat == UPS_ISO))
		m_pcomm->UnRegister((int)Node);
	else if (cat == WATCH_ISO)
		m_pcomm->UnRegisterWatch((int)Node);

	return S_OK;
}