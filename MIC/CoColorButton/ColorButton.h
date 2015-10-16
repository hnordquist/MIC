// ColorButton.h

#ifndef __COLORBUTTON_H_
#define __COLORBUTTON_H_

#include "resource.h"       // main symbols
#include "COM_Utilities.h"

/////////////////////////////////////////////////////////////////////////////
// CColorButton
class ATL_NO_VTABLE CColorButton : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CColorButton, &CLSID_ColorButton>,
	public IDispatchImpl<ICB, &IID_ICB, &LIBID_COCOLORBUTTONLib>
{
public:
	CColorButton()
	{
	}

	HWND m_hwnd;  //the handle for MIC's ccbutton object

DECLARE_REGISTRY_RESOURCEID(IDR_COLORBUTTON)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CColorButton)
	COM_INTERFACE_ENTRY(ICB)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ICB
public:

	STDMETHOD(get_FaceColor)(/*[out]*/ ULONG *pRGBvalue);
	STDMETHOD(get_Numeric)(/*[out]*/ ULONG *numeric);
	STDMETHOD(put_DisplayNumericString)(/*[in]*/ VARIANT_BOOL vb, /*[in]*/ ULONG uRadix);
	STDMETHOD(put_FaceColor)(/*[in]*/ ULONG RGBcolor);
	STDMETHOD(put_Flash)(/*[in]*/ VARIANT_BOOL vb);
	STDMETHOD(put_Numeric)(/*[in]*/ ULONG newValue);
	STDMETHOD(put_PreNumericString)(/*[in]*/ BSTR string);
	STDMETHOD(SetDiskIcon_Floppy)();
	STDMETHOD(SetDiskIcon_Hourglass)();
	STDMETHOD(SetErrorIcon_Bomb)();
	STDMETHOD(SetErrorIcon_Flag)();
	STDMETHOD(SetErrorIcon_Skull)();
	STDMETHOD(SetMICObjectHandle)(/*[in]*/ ULONG handle);
	STDMETHOD(ShowIcon_Bell)(/*[in]*/ VARIANT_BOOL vb);			//was SetBell(bool)
	STDMETHOD(ShowIcon_Clock)(/*[in]*/ VARIANT_BOOL vb);		//was SetTime(bool)
	STDMETHOD(ShowIcon_Disk)(/*[in]*/ VARIANT_BOOL vb);			//was SetDisk(bool)
	STDMETHOD(ShowIcon_Error)(/*[in]*/ VARIANT_BOOL vb);		//was SetFlag(bool)
	STDMETHOD(ShowIcon_Frown)(/*[in]*/ VARIANT_BOOL vb);		//was SetOthr(bool)
	STDMETHOD(ShowIcon_Square)(/*[in]*/ VARIANT_BOOL vb);
	STDMETHOD(get_VersionString)(/*[out]*/BSTR *pbstrVersion);
};

#endif //__COLORBUTTON_H_
