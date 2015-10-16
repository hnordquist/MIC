// ColorButton.cpp

#include "stdafx.h"
#include "CoColorButton.h"
#include "ColorButton.h"
#include "ccbutton_messages.h"
#include "afxconv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton


STDMETHODIMP CColorButton::get_FaceColor(/*[out]*/ ULONG *pRGBvalue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	LRESULT lr = ::SendMessage(m_hwnd, WM_GET_FACE_COLOR, 0, 0);
	*pRGBvalue = ULONG(lr);
	return S_OK;
}
STDMETHODIMP CColorButton::get_Numeric(/*[out]*/ ULONG *pNumeric)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	LRESULT lr = ::SendMessage(m_hwnd, WM_GET_NUMERIC, 0, 0);
	*pNumeric = ULONG(lr);
	return S_OK;
}
STDMETHODIMP CColorButton::put_DisplayNumericString(VARIANT_BOOL vb, ULONG uRadix)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd,WM_DISPLAY_NUMERIC_STRING, WPARAM(vb), (LPARAM)uRadix);
	return S_OK;
}
STDMETHODIMP CColorButton::put_FaceColor(/*[in]*/ ULONG RGBcolor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_PUT_FACE_COLOR, WPARAM(RGBcolor), 0);
	return S_OK;
}
STDMETHODIMP CColorButton::put_Flash(/*[in]*/ VARIANT_BOOL vb)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_PUT_FLASH, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::put_Numeric(/*[in]*/ ULONG newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_PUT_NUMERIC, WPARAM(newValue),0);
	return S_OK;
}
STDMETHODIMP CColorButton::put_PreNumericString(/*[in]*/ BSTR bstr_in)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//<<QA>> see if this works.  see if this leaks.  If okay , then can
	//not include the COM_Utilities.cpp
	//USES_CONVERSION;
	//char *string_out = W2A(bstr_in);

	char string_out[16];
	Convert_BSTR_to_Character_String(bstr_in, string_out, sizeof(string_out));

	::SendMessage(m_hwnd, WM_PUT_NUMERIC_STRING, WPARAM(string_out),0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetDiskIcon_Floppy()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_DISK_ICON_FLOPPY, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetDiskIcon_Hourglass()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_DISK_ICON_HOURGLASS, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetErrorIcon_Bomb()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_ERROR_ICON_BOMB, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetErrorIcon_Flag()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_ERROR_ICON_FLAG, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetErrorIcon_Skull()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_ERROR_ICON_SKULL, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetMICObjectHandle(/*[in]*/ ULONG handle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_hwnd = (HWND)handle;
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Bell(/*[in]*/ VARIANT_BOOL vb)	//was SetBell(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_BELL, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Clock(/*[in]*/ VARIANT_BOOL vb)	//was SetTime(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_CLOCK, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Disk(/*[in]*/ VARIANT_BOOL vb)	//was SetDisk(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_DISK, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Error(/*[in]*/ VARIANT_BOOL vb)	//was SetFlag(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_ERROR, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Frown(/*[in]*/ VARIANT_BOOL vb)		//was SetOthr(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_FROWN, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Square(/*[in]*/ VARIANT_BOOL vb)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_SQUARE, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::get_VersionString(BSTR *pbstrVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//SCR 118
    DWORD   hInfo;
    DWORD   dwLen;
    char    name[_MAX_PATH];
	char*	VerData;
    CString sRet;
    unsigned int uSize;
    LPCTSTR lpBuffer2;
	LPCTSTR lpBuffer3;
	CString sSubBlockHead;

    struct 
	{
        WORD    first;
        WORD    second;
    } *lpBuffer;
     
    HMODULE hModule = AfxGetInstanceHandle();
    GetModuleFileName(hModule, name, sizeof(name));

    dwLen = GetFileVersionInfoSize(name, &hInfo);

    VerData = (char *)new char[dwLen];

    GetFileVersionInfo(name, hInfo,  dwLen, VerData);

    VerQueryValue(VerData,"\\VarFileInfo\\Translation",
            (LPVOID*)&lpBuffer,(unsigned int *) &dwLen);
      
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\", 
                    lpBuffer->first, lpBuffer->second);
    sSubBlockHead = name;


    CString sBlock = sSubBlockHead + "FileVersion"; 

    BOOL bRet = VerQueryValue(VerData, 
                    (char *)((LPCTSTR) sBlock), 
                    (void**)&lpBuffer2, &uSize);
     
	sBlock = sSubBlockHead + "InternalName";
	bRet = VerQueryValue(VerData,
		(char*)((LPCSTR) sBlock),
		(void**)&lpBuffer3, &uSize);


	CString csNameVersionDate;

	csNameVersionDate = lpBuffer3;
	csNameVersionDate += "\t";
	csNameVersionDate += lpBuffer2;
	csNameVersionDate.Replace(", ",".");
	csNameVersionDate += "\t";
	csNameVersionDate += __DATE__;

	Convert_CString_to_BSTR(csNameVersionDate,*pbstrVersion);

	delete VerData;

	return S_OK;
}
