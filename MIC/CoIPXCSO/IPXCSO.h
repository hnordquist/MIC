// IPXCSO.h : Declaration of the CIPXCSO

#ifndef __IPXCSO_H_
#define __IPXCSO_H_

#include "resource.h"       // main symbols
#include "COM_Utilities.h"
#include "ComORTEC.h"

/////////////////////////////////////////////////////////////////////////////
// CIPXCSO
class ATL_NO_VTABLE CIPXCSO : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIPXCSO, &CLSID_IPXCSO>,
	public IDispatchImpl<ICSO, &IID_ICSO, &LIBID_COIPXCSOLib>
{
public:
	CIPXCSO()
	{
//		TRACE("CIPCCSO::CTOR\n");
		m_pcomm = (CComORTEC*)AfxBeginThread(RUNTIME_CLASS( CComORTEC ));
	}
	~CIPXCSO()
	{
//		TRACE("CIPXCSO::DTOR - NOOP\n");
	}

	CComORTEC *m_pcomm;

DECLARE_REGISTRY_RESOURCEID(IDR_IPXCSO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CIPXCSO)
	COM_INTERFACE_ENTRY(ICSO)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ICSO
public:
	STDMETHOD(CollectParametersForNewCSO)(
		/*[out]*/ BSTR *nameout,
		/*[out]*/ VARIANT_BOOL *pvbHaveNewCSO );

	STDMETHOD(EnableHeartbeat)(/*[in]*/VARIANT_BOOL bEnable);
	STDMETHOD(get_CSOTypeString)(/*[out]*/ BSTR *pType);
	STDMETHOD(get_LastError)(/*[out]*/ BSTR *errorstring);
	STDMETHOD(get_Name)(/*[out]*/ BSTR *pName);
	STDMETHOD(get_VersionString)(/*[out]*/ BSTR *pName);
	STDMETHOD(put_DialogAutoCloseTime)(/*[in]*/ ULONG seconds);
	STDMETHOD(put_INIPathname)(/*[in]*/ BSTR pathname);
	STDMETHOD(put_Name)(/*[in]*/BSTR pathname);

	STDMETHOD(put_WindowsRegisteredMessageStrings)(
			/*[in]*/ ULONG ulRECEIVE,
			/*[in]*/ ULONG ulRECEIVE_OOB,
			/*[in]*/ ULONG ulSTART,
			/*[in]*/ ULONG ulSTOP,
			/*[in]*/ ULONG ulRESET);

	STDMETHOD(put_ThresholdValue)(/*[in]*/ ULONG threshold_value);

	STDMETHOD(RegisterISO)(
			/*[in]*/ ULONG receiver_handle,
			/*[in]*/ ULONG Node,
			/*[in]*/ ISO_FUNCTIONAL_CATEGORY cat,
			/*[in]*/ VARIANT_BOOL vb_OOB, 
			/*[out]*/ VARIANT_BOOL *pvbResult);

	STDMETHOD(ResetCSO)();
	STDMETHOD(SendHeartbeat)();
	STDMETHOD(SendToWatch)(/*[in]*/ BSTR message, /*[in]*/ ULONG Node);
	STDMETHOD(SetUpPort)(/*[in]*/ BSTR portname, /*[out]*/ VARIANT_BOOL *pvbResult);
	STDMETHOD(ShutdownCSO)();
	STDMETHOD(StartCSO)();

	STDMETHOD(Transmit)(
			/*[in]*/ ULONG length,
			/*[in]*/ BLOB data, 
			/*[in]*/ ULONG node, 
			/*[in]*/ VARIANT_BOOL vbQuiet, 
			/*[out]*/ ULONG *pBytesWritten);

	STDMETHOD(UnRegisterISO)(/*[in]*/ ULONG Node, /*[in]*/ ISO_FUNCTIONAL_CATEGORY cat);
};


#endif //__IPXCSO_H_
