//$History: VERSION.CPP $
 * 
 * *****************  Version 1  *****************
 * User: Pelowitz     Date: 3/12/08    Time: 3:51p
 * Created in $/TRACKER
 * At or near retirement
// 
// *****************  Version 2  *****************
// User: Pelowitz     Date: 4/26/05    Time: 1:50p
// Updated in $/Tracker
// Added version to title bar, added VERSION class to get version from the
// resources.
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 4/26/05    Time: 1:29p
// Created in $/Tracker
#include "stdafx.h"
#include "Version.h"
     
CVersionInfo::CVersionInfo(): m_VerData(NULL)
{
    DWORD   hInfo;
    DWORD   dwLen;
    char    name[_MAX_PATH];

    struct 
	{
        WORD    first;
        WORD    second;
    } *lpBuffer;
     
    HMODULE hModule = AfxGetInstanceHandle();
    GetModuleFileName(hModule, name, sizeof(name));

    dwLen = GetFileVersionInfoSize(name, &hInfo);

    m_VerData = (void *)new char[dwLen];

    GetFileVersionInfo(name, hInfo,  dwLen, m_VerData);

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