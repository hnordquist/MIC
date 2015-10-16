#pragma once

#include <afxtempl.h>
typedef struct InitialStatusData
{
	typedef enum {eUnknown, eISO, eCSO, eWatch, eHiddenWatch, eCOMCategory4, eCOMCategory5, eMaxCOMCategory} COMCategory;

	// this preserves enough information to generate a unique hash key and give the user some useful diagnostic messages.
	COMCategory Type;
	INT Node;
	DWORD StatusTickTime;

	CString Name, Version, Err;
	CString Port;//, Node; //StatusMessages;
	CString kHashKey;

	InitialStatusData()
	{
		Type = eUnknown;
		Node = -1;
		StatusTickTime = ::GetTickCount();
	}

	void SetHashKey()
	{
		kHashKey.Format("%s %s %s %lu",COMCategoryString[Type], Name, Version, StatusTickTime);
	}

	void PrepareCSOStatus(TCHAR* name, BSTR version, BSTR err, TCHAR* port)
	{
		Name = name;
		Port = port;
		//Node = -1;
		Version = version;
		Err = err;
		Type = eCSO;
		SetHashKey ();
	}

	void PrepareISOStatus(TCHAR* name, BSTR version, BSTR err, int node)
	{
		Name = name;
		Err = err;
		Version = version;
		Type = eISO;
		Node = node;
		SetHashKey ();
	}

	static const TCHAR* COMCategoryString[eMaxCOMCategory];
} InitialStatusDataStruct;

const TCHAR* InitialStatusData::COMCategoryString[InitialStatusDataStruct::eMaxCOMCategory] = {"Unknown", "ISO", "CSO", "Watch", "Hidden Watch", "COMCategory4", "COMCategory5"};


class CCOMStatusMap : public CTypedPtrMap<CMapStringToPtr, CString, InitialStatusDataStruct*> 
{

	public:

	CCOMStatusMap() {}
	~CCOMStatusMap() {}

	InitialStatusDataStruct* GetNewCSO(TCHAR* name, BSTR version, BSTR err)
	{
		CHAR port[256];
		InitialStatusDataStruct* p = new InitialStatusDataStruct();
		LPSTR f = port;
		GetPrivateProfileString(name, PORT,"PORT_ERR", f, sizeof(port), g_szIniFile);
		p->PrepareCSOStatus(name, version, err, port);
		SetAt(p->kHashKey, p);
		return p;
	}

	InitialStatusDataStruct* GetNewISO(TCHAR* name, BSTR version, BSTR err)
	{
		InitialStatusDataStruct* p = new InitialStatusDataStruct();
		int iInstNode = GetPrivateProfileInt(name,NODE,-1,g_szIniFile);
		p->PrepareISOStatus(name, version, err, iInstNode);
		SetAt(p->kHashKey, p);
		return p;
	}

	void PrepareNoticeStrings(CString& CSOs, CString& ISOs)
	{
		POSITION pos = GetStartPosition();
		while(pos != NULL)
		try
		{
			InitialStatusDataStruct* p;
			CString hk, temp;
			// Gets key (hk) and value (p)
			GetNextAssoc(pos, hk, p);

			if (p->Type == InitialStatusDataStruct::eCSO)
			{
				temp.Format("CSO \'%s\' assigned to port %s\r\n   %16s", p->Name, p->Port, p->Version);
				if (CSOs.GetLength() < 1)
					CSOs = "\r\nCSOs with errors:\r\n";
				CSOs.Append("\r\n");
				CSOs.Append(temp);
				CSOs.Append("\r\n");
			}
			else if (p->Type == InitialStatusDataStruct::eISO)
			{
				temp.Format("ISO \'%s\' at node %d, -- see other message box", p->Name, p->Node);
				if (ISOs.GetLength() < 1)
					ISOs = "\r\n\r\nISOs with errors:\r\n";
				ISOs.Append("\r\n");
				ISOs.Append(temp);
				ISOs.Append("\r\n");
			}
		}
		catch (...)
		{
		}
	}

	void ClearMap()
	{
		POSITION pos;
		for (pos = GetStartPosition(); pos != NULL;)
		{
			CString key;
			InitialStatusDataStruct* p = NULL;
			GetNextAssoc(pos, key, (InitialStatusDataStruct*&)p);
			if (p != NULL)
				delete p;
		}

	}


};



