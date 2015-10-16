// UGrandISO.h

#ifndef __UGRANDISO_H_
#define __UGRANDISO_H_

#include "resource.h"       // main symbols
#include "instrument.h"
#include "../CommonSource/COM_Utilities.h"
#include "../Interfaces/ICSO.h"
#include "../Interfaces/ICB.h"

/////////////////////////////////////////////////////////////////////////////
// CUGrandISO
class ATL_NO_VTABLE CUGrandISO : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUGrandISO, &CLSID_UGrandISO>,
	public IDispatchImpl<IISO, &IID_IISO, &LIBID_COUGRANDISOLib>
{
public:
	CUGrandISO()
	{
	}

	CInstrument m_inst;

DECLARE_REGISTRY_RESOURCEID(IDR_UGRANDISO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CUGrandISO)
	COM_INTERFACE_ENTRY(IISO)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IISO
public:

	STDMETHOD(AllowWriteToDisk)(VARIANT_BOOL vb);
	STDMETHOD(AppendGUIValues)(BSTR bstrfilename);

	STDMETHOD(CollectParametersForNewISO)(
		BSTR *pbstrNameOut, 
		VARIANT_BOOL *pvbCancelled);

	STDMETHOD(CreateISO)(VARIANT_BOOL *pvbSuccess);

	STDMETHOD(DriveBBMToZero)(VARIANT_BOOL vbCommand, VARIANT_BOOL vbInPowerFail);

	STDMETHOD(EstablishButtonKeeperCommunicationsPath)(
			ULONG ulButtonKeeperHandle,
			ULONG ulCheckButtonRegisteredMessageValue);

	STDMETHOD(EstablishSpecialUPSCommunicationsPath)(
			ULONG ulSendToHandle, 
			ULONG ulQueryAllPausedRegisteredMessageValue,
			ULONG ulDriveAllToZeroRegisteredMessageValue);

	STDMETHOD(EstablishValidationCommunicationsPath)(
			ULONG ulValidationHandle, 
			ULONG ulValidationRegisteredMessageValue);

	STDMETHOD(get_CurrentBBM)(ULONG *pCurrentBBM);
	STDMETHOD(get_ID)(BSTR *pbstrID);
	STDMETHOD(get_InShutDown)( VARIANT_BOOL *pvbInShutDown);
	STDMETHOD(get_InStartUp)(VARIANT_BOOL *pvbInStartUp);
	STDMETHOD(get_ISOCategory)(ULONG *pCategory);
	STDMETHOD(get_ISOTypeString)(BSTR *pTypeString);
	STDMETHOD(get_IsPaused)(VARIANT_BOOL *pvbIsPaused);
	STDMETHOD(get_FullName)(BSTR *pFullName);
	STDMETHOD(get_LastError)(BSTR *pbstrErrorString);

	STDMETHOD(get_LocationAndExtensionsForDataFiles)(
		BSTR *pbstrLocation,
		BSTR *pbstrExtensions);

	STDMETHOD(get_Name)(BSTR *pName);
	STDMETHOD(get_VersionString)(BSTR *pbstrVersion);
	STDMETHOD(PauseInstrument)(VARIANT_BOOL vbPauseCommand);
	STDMETHOD(put_DialogAutoCloseTime)(ULONG seconds);
	STDMETHOD(put_ICBPointer)(IUnknown* pICB);
	STDMETHOD(put_ICSOPointer)(IUnknown* pICSO);
	STDMETHOD(put_INIPathname)(BSTR pathname);
	STDMETHOD(put_LongNameFormatString)(BSTR bstrFormatString);
	STDMETHOD(put_MaximumRetryRepetitions)(ULONG uNumberRepetitions);
	STDMETHOD(put_Name)(BSTR bstrName);
	STDMETHOD(put_NormalShutdown)(VARIANT_BOOL vbNormalShutdown);
	STDMETHOD(put_UTILPathname)(BSTR bstrUTILPathname);

	STDMETHOD(put_WindowsRegisteredMessageStrings)(
			ULONG ulRECEIVE,
			ULONG ulRECEIVE_OOB,
			ULONG ulSTART,
			ULONG ulSTOP,
			ULONG ulRESET);

	STDMETHOD(ResetISO)();
	STDMETHOD(ShowISOGUI)(VARIANT_BOOL vbCommand);
	STDMETHOD(ShutdownISO)();
	STDMETHOD(UPSGoDumb)();
	STDMETHOD(UseShortFilename)(VARIANT_BOOL vbCommand);
};

#endif //__UGRANDISO_H_
