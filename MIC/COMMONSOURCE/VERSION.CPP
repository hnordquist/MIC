// version.cpp
 
#include <stdafx.h>
#include "Version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
     
CVersionInfo::CVersionInfo(): m_VerData(NULL)
{
    DWORD   hInfo;
    DWORD   dwLen;
	char name[_MAX_PATH];
    struct 
	{
        WORD    first;
        WORD    second;
    } *lpBuffer;
     
    HMODULE hModule = AfxGetInstanceHandle();
    GetModuleFileName(hModule, m_name, sizeof(m_name));

    dwLen = GetFileVersionInfoSize(m_name, &hInfo);

    m_VerData = (void *)new char[dwLen];

    GetFileVersionInfo(m_name, hInfo,  dwLen, m_VerData);

    VerQueryValue(m_VerData,"\\VarFileInfo\\Translation",
            (LPVOID*)&lpBuffer,(unsigned int *) &dwLen);
      
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\", 
                    lpBuffer->first, lpBuffer->second);
    m_sSubBlockHead = name;
}

CVersionInfo::~CVersionInfo()
{
    delete m_VerData;
}
     
CString CVersionInfo::GetValue(CString const sSubBlock)
{
    CString sRet;
    unsigned int uSize;
    LPCTSTR lpBuffer;

    CString sBlock = m_sSubBlockHead + sSubBlock; 

    BOOL bRet = VerQueryValue(m_VerData, 
                    (char *)((LPCTSTR) sBlock), 
                    (void**)&lpBuffer, &uSize);
     
    if (bRet)
        sRet = lpBuffer;
     
    return sRet;
}