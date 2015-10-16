//$History: LOCNUTCSO.h $
 * 
 * *****************  Version 2  *****************
 * User: Pelowitz     Date: 4/29/08    Time: 2:14p
 * Updated in $/MIC 2004/CoLOCNUTCSO
 * final check in prior to retirement
// * 
// * *****************  Version 1  *****************
// * User: Pelowitz     Date: 3/12/08    Time: 2:52p
// * Created in $/MIC 2004/CoLOCNUTCSO
// * At or near retirement (EOSS still being tested)
// 
// *****************  Version 2  *****************
// User: 149047       Date: 10/23/06   Time: 1:29p
// Updated in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 9/13/06    Time: 12:39p
// Created in $/MIC 2.004/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 1/11/06    Time: 7:47a
// Created in $/CoLOCNUTCSO
// 
// *****************  Version 1  *****************
// User: Moore        Date: 8/09/05    Time: 4:12p
// Created in $/MIC 2.000/CoLOCNUTCSO
// 
// *****************  Version 3  *****************
// User: Moore        Date: 6/24/05    Time: 5:12p
// Updated in $/MIC 2.0/CoLOCNUTCSO
// 
// *****************  Version 2  *****************
// User: Moore        Date: 5/04/05    Time: 10:07a
// Updated in $/MIC 2.0/CoLOCNUTCSO
// Fixed version static on GRAND component.
// 
// *****************  Version 1  *****************
// User: Moore        Date: 5/03/05    Time: 2:11p
// Created in $/MIC 2.0/CoLOCNUTCSO
// Added CoLOCNUTCSO to workspace.
//

// LOCNUTCSO.h : Declaration of the CLOCNUTCSO

#ifndef __LOCNUTCSO_H_
#define __LOCNUTCSO_H_

#include "resource.h"       // main symbols
#include "COM_Utilities.h"
#include "ComLOCNUT.h"

/////////////////////////////////////////////////////////////////////////////
// CLOCNUTCSO
class ATL_NO_VTABLE CLOCNUTCSO : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLOCNUTCSO, &CLSID_LOCNUTCSO>,
	public IDispatchImpl<ICSO, &IID_ICSO, &LIBID_COLOCNUTCSOLib>
{
public:
	CLOCNUTCSO()
	{
//		TRACE("CLOCNUTCSO::CTOR\n");
		m_pcomm = (CComLOCNUT*)AfxBeginThread(RUNTIME_CLASS( CComLOCNUT ));
	}
	~CLOCNUTCSO()
	{
//		TRACE("CLOCNUTCSO::DTOR - NOOP\n");
	}

	CComLOCNUT *m_pcomm;

DECLARE_REGISTRY_RESOURCEID(IDR_LOCNUTCSO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CLOCNUTCSO)
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

#endif //__LOCNUTCSO_H_
