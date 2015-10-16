//-----------------------------------------------------------------------------
//
// EOSS - Electronic Optical Sealing System
//
// Sample Seal Reader
//
// (c) 2007 Dr. Neumann Consultants
// D-50259 Pulheim, Schiffgesweg 9, Germany
//
//-----------------------------------------------------------------------------

#pragma once

#include "eossdefs.h"

#include "comport.h"

#include "dongle.h"

#include <atlstr.h>

class CEoss
{
private:
	CComport * m_pComport;
	CDongle * m_pDongle;
	ADDRESS m_Address;
public:
	CEoss(ADDRESS Address, CComport * pComport, CDongle * pDongle);
	~CEoss(void);
	ADDRESS EOSSes[MAX_EOSS];
	LONG m_KeysetNumber;
	LONG m_KeysetTime;
	char m_Version[2];

private:
	DESBLOCK ChallengeToken;
	CString notes;
private:
	bool SendTo(PACKAGE * pPackage, int Length);
	bool RecvFrom(PACKAGE * pPackage, int * pLength);
	bool Reqply(PACKAGE * pPackage, int DataBytesToSend, int DataBytesExpected);
	int  RecvFromAll();
	bool SendToAll(SPACKAGE * pPackage, int Length);
	int ReqWho(SPACKAGE * pPackage, int DataBytesToSend, int DataBytesExpected);
public:
	bool GetChallenge(void);
	bool Logon(char * szOperator1, char * szOperator2);
	bool GetStatus(STATUS * pStatus);
	bool GetLogInfo(LOGINFO * pLogInfo);
	unsigned int GetSealInfo();
	bool GetLogPage(LOGPAGE * pLogPage, unsigned char PageNumber);
	bool Logoff(void);
	ADDRESS GetAddress(int iAddress) {return EOSSes[iAddress];};
	void SetCurrentAddress(int iAddress) {m_Address = EOSSes[iAddress];};
	CString& GetNotes() {return notes; }
	void ResetNotes() {notes = ""; }

};

#include <windows.h>

void EossToSystemTime(SYSTEMTIME * pSystemTime, EOSSTIME * pEossTime);