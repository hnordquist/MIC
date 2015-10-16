// WatchISO.cpp

#include "stdafx.h"
#include "CoWatchISO.h"
#include "WatchISO.h"
#include "ini_definitions.h"
#include "NewWatchWindow.h"
//#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchISO

//1
STDMETHODIMP CWatchISO::AllowWriteToDisk(/*[in]*/ VARIANT_BOOL vb)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.BlockWrite(vb == VARIANT_FALSE);
	return S_OK;
}

//2
STDMETHODIMP CWatchISO::AppendGUIValues(/*[in]*/ BSTR bstrfilename)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char szFilename[MAX_PATH];
	Convert_BSTR_to_Character_String(
		bstrfilename, szFilename, sizeof(szFilename));
	FILE *outfile = fopen(szFilename,"at");
	m_inst.IssuePrint(outfile);
	fclose(outfile);

	return S_OK;
}

//3
STDMETHODIMP CWatchISO::CollectParametersForNewISO(
	/*[out]*/ BSTR *pbstrNameOut,
	/*[out]*/ VARIANT_BOOL *pvbHaveNewISO)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString csNameOut;

	bool bHaveNewISO = m_inst.CollectParametersForNewISO(csNameOut);

	if (bHaveNewISO)
	{
		*pvbHaveNewISO = VARIANT_TRUE;
		const char *string_in = LPCTSTR(csNameOut);
		BSTR bstr_out;
		Convert_Character_String_to_BSTR((char *)string_in, bstr_out);
		*pbstrNameOut = bstr_out;
	}
	else
	{
		*pvbHaveNewISO = VARIANT_FALSE;
		*pbstrNameOut = BSTR("No input");
	}
	return S_OK;
}

//4
STDMETHODIMP CWatchISO::CreateISO(/*[out]*/ VARIANT_BOOL *pvbSuccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	bool bresult = m_inst.CreateInstrument();
	*pvbSuccess = bresult?VARIANT_TRUE:VARIANT_FALSE;

	return S_OK;
}

//5
STDMETHODIMP CWatchISO::DriveBBMToZero(
	/*[in]*/ VARIANT_BOOL vbDriveToZero, 
	/*[in]*/ VARIANT_BOOL vbInPowerFail)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_inst.DriveToZero(
		vbDriveToZero == VARIANT_TRUE, 
		vbInPowerFail == VARIANT_TRUE);

	return S_OK;
}

//6
STDMETHODIMP CWatchISO::EstablishButtonKeeperCommunicationsPath(
			/*[in]*/ ULONG ulButtonKeeperHandle, 
			/*[in]*/ ULONG ulCheckButtonRegisteredMessageValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_inst.EstablishButtonKeeperCommunicationsPath(
		(HWND)ulButtonKeeperHandle, 
		(UINT)ulCheckButtonRegisteredMessageValue);

	return S_OK;
}

//7
STDMETHODIMP CWatchISO::EstablishSpecialUPSCommunicationsPath(
			/*[in]*/ ULONG ulSendToHandle, 
			/*[in]*/ ULONG ulQueryAllPausedRegisteredMessageValue,
			/*[in]*/ ULONG ulDriveAllToZeroRegisteredMessageValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		//
		// this is needed only when talking to UPS instruments
		//
	return S_OK;
}

//8
STDMETHODIMP CWatchISO::EstablishValidationCommunicationsPath(
			/*[in]*/ ULONG ulValidationHandle, 
			/*[in]*/ ULONG ulValidationRegisteredMessageValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_inst.EstablishValidationCommunicationsPath(
		(HWND)ulValidationHandle,
		(UINT)ulValidationRegisteredMessageValue);

	return S_OK;
}

//9
STDMETHODIMP CWatchISO::get_CurrentBBM(/*[out]*/ ULONG *pCurrentBBM)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	// Watch ISO doesn't have any bbm's
	*pCurrentBBM = 0;
	return S_OK;
}

//10
STDMETHODIMP CWatchISO::get_ID(/*[out]*/ BSTR *pbstrID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *id = m_inst.GetID();
	Convert_Character_String_to_BSTR(id,*pbstrID);
	return S_OK;
}


//11
STDMETHODIMP CWatchISO::get_InShutDown(VARIANT_BOOL *pvbInShutDown)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	bool b = m_inst.InShutDown();
	*pvbInShutDown = b?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

//12
STDMETHODIMP CWatchISO::get_InStartUp(VARIANT_BOOL *pvbInStartUp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	bool b = m_inst.InStartUp();
	*pvbInStartUp = b?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

//13
STDMETHODIMP CWatchISO::get_ISOCategory(/*[out]*/ ULONG *pCategory)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pCategory = (ULONG)m_inst.GetISOCategory();
	return S_OK;
}

//14
STDMETHODIMP CWatchISO::get_ISOTypeString(BSTR *pbstrTypeString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *type = m_inst.GetType();
	Convert_Character_String_to_BSTR(type,*pbstrTypeString);
	return S_OK;
}

//15
STDMETHODIMP CWatchISO::get_IsPaused(VARIANT_BOOL *pvbIsPaused)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	bool b = m_inst.IsPaused();
	*pvbIsPaused = b?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

//16
STDMETHODIMP CWatchISO::get_FullName(/*[out]*/ BSTR *pbstrFullName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *fullname = m_inst.GetFullName();
	Convert_Character_String_to_BSTR(fullname,*pbstrFullName);
	return S_OK;
}

//17
STDMETHODIMP CWatchISO::get_LastError(/*[out]*/ BSTR *pbstrErrorString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CString *pcs = m_inst.GetLastError();
	Convert_CString_to_BSTR(*pcs,*pbstrErrorString);
	return S_OK;
}

//18
STDMETHODIMP CWatchISO::get_LocationAndExtensionsForDataFiles(
	BSTR *pbstrLocation,
	BSTR *pbstrExtensions)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char *string_in = m_inst.GetSaveLoc();

	if (NULL == string_in || 0 == strlen(string_in))
		*pbstrLocation = 0;
	else
		Convert_Character_String_to_BSTR(string_in,*pbstrLocation);

	string_in = m_inst.GetFileExtensionsList();

	if (NULL == string_in || 0 == strlen(string_in))
		*pbstrExtensions = 0;
	else
		Convert_Character_String_to_BSTR(string_in,*pbstrExtensions);

	return S_OK;
}

//19
STDMETHODIMP CWatchISO::get_Name(/*[out]*/ BSTR *pbstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *name = m_inst.GetName();
	Convert_Character_String_to_BSTR(name,*pbstrName);
	return S_OK;
}

//20
STDMETHODIMP CWatchISO::get_VersionString(/*[out]*/ BSTR *pbstrVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	//CVersionInfo VersionInfo;
	*pbstrVersion = m_inst.m_csNameVersionDate.AllocSysString(); //SCR 118
	return S_OK;
}

//21
STDMETHODIMP CWatchISO::PauseInstrument(/*[in]*/ VARIANT_BOOL vbPauseCommand)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PauseInstrument(vbPauseCommand == VARIANT_TRUE);
	return S_OK;
}

//22
STDMETHODIMP CWatchISO::put_DialogAutoCloseTime(/*[in]*/ ULONG seconds)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutDialogAutoCloseTime(1000*(int)seconds);
	return S_OK;
}

//23
STDMETHODIMP CWatchISO::put_ICBPointer(/*[in]*/ IUnknown* pICB)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutICBPointer((ICB *)pICB);
	return S_OK;
}

//24
STDMETHODIMP CWatchISO::put_ICSOPointer(/*[in]*/ IUnknown* pICSO)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutICSOPointer((ICSO*)pICSO);
	return S_OK;
}

//25
STDMETHODIMP CWatchISO::put_INIPathname(/*[in]*/ BSTR bstrINIPathname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(bstrINIPathname,string_out,sizeof(string_out));
	m_inst.PutINIPathname(string_out);
	return S_OK;
}

//26
STDMETHODIMP CWatchISO::put_LongNameFormatString(BSTR bstrFormatString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[128];
	Convert_BSTR_to_Character_String(bstrFormatString,string_out, sizeof(string_out));
	CString cs = string_out;
	m_inst.PutLongNameFormatString(cs);
	return S_OK;
}

//27
STDMETHODIMP CWatchISO::put_MaximumRetryRepetitions(ULONG ulNumberRepetitions)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutMaxRepetition((int)ulNumberRepetitions);
	return S_OK;
}

//28
STDMETHODIMP CWatchISO::put_Name(/*[in]*/ BSTR bstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(bstrName, string_out,sizeof(string_out));
	m_inst.PutName(string_out);
	return S_OK;
}

//29
STDMETHODIMP CWatchISO::put_NormalShutdown(VARIANT_BOOL vbNormalShutdown)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutNormalShutdown(VARIANT_TRUE == vbNormalShutdown);
	return S_OK;
}

//30
STDMETHODIMP CWatchISO::put_UTILPathname(/*[in]*/ BSTR bstrUTILPathname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(
		bstrUTILPathname,string_out,sizeof(string_out));
	m_inst.PutUTILPathname(string_out);
	return S_OK;
}

//31
STDMETHODIMP CWatchISO::put_WindowsRegisteredMessageStrings(
	/*[in]*/ ULONG ulRECEIVE,
	/*[in]*/ ULONG ulRECEIVE_OOB,
	/*[in]*/ ULONG ulSTART,
	/*[in]*/ ULONG ulSTOP,
	/*[in]*/ ULONG ulRESET)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.SetWindowsRegisteredMessageStrings(
		ulRECEIVE,
		ulRECEIVE_OOB,
		ulSTART,
		ulSTOP,
		ulRESET);
	return S_OK;
}

//32
STDMETHODIMP CWatchISO::ResetISO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.Reset();
	return S_OK;
}

//33
STDMETHODIMP CWatchISO::ShowISOGUI(/*[in]*/ VARIANT_BOOL vbCommand)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.ShowInstrument(vbCommand == VARIANT_TRUE);
	return S_OK;
}

//34
STDMETHODIMP CWatchISO::ShutdownISO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.UnRegInstrument();
	return S_OK;
}

//35
STDMETHODIMP CWatchISO::UPSGoDumb()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return S_OK;
}


//36
STDMETHODIMP CWatchISO::UseShortFilename(/*[in]*/ VARIANT_BOOL vbCommand)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return S_OK;
}
