// ILONCSO.h : Declaration of the CILONCSO

#ifndef __ILONCSO_H_
#define __ILONCSO_H_

#include "resource.h"       // main symbols
#include "COM_Utilities.h"
#include "ComILON.h"

/////////////////////////////////////////////////////////////////////////////
// CILONCSO
class ATL_NO_VTABLE CILONCSO : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CILONCSO, &CLSID_ILONCSO>,
	public IDispatchImpl<ICSO, &IID_ICSO, &LIBID_COILONCSOLib>
{
public:
	CILONCSO()
	{
//		TRACE("CILONCSO::CTOR\n");
		m_pcomm = (CComILON*)AfxBeginThread(RUNTIME_CLASS( CComILON ));
	}
	~CILONCSO()
	{
//		TRACE("CILONCSO::DTOR - NOOP\n");
	}

	CComILON *m_pcomm;

DECLARE_REGISTRY_RESOURCEID(IDR_ILONCSO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CILONCSO)
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

#endif //__ILONCSO_H_
