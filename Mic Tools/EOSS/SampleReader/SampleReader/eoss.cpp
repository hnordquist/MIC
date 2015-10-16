//-----------------------------------------------------------------------------
//
// EOSS - Electronic Optical Sealing System
//
// Sample Seal Reader
//
// (c) LANS LLC, 2009
// Written by David Pelowitz
// Modified by Joe Longo
//
// based on 
// (c) 2007 Dr. Neumann Consultants
// D-50259 Pulheim, Schiffgesweg 9, Germany
//
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "eoss.h"

#include "crc.h"

//-----------------------------------------------------------------------------

CEoss::CEoss(ADDRESS Address, CComport * pComport, CDongle * pDongle)
{
	m_pComport = pComport;
	m_pDongle = pDongle;
	m_Address = Address;
	for (int i = 0; i < MAX_EOSS; i++)
		EOSSes[i] = 0;
}

//-----------------------------------------------------------------------------

CEoss::~CEoss(void)
{
}

//-----------------------------------------------------------------------------
//
// send a package
// returns true on success 
//
bool CEoss::SendTo(PACKAGE * pPackage, int Length)
{
	FRAME Frame;

	Frame.Header.Receiver = m_Address;

	Frame.Header.Sender = 0;
	Frame.Header.Size = Length + (unsigned short)sizeof(Frame.Header) + 2;

	memcpy(&Frame.Package, pPackage, Length);

	unsigned short Crc = Crc16(&Frame, sizeof(Frame.Header) + Length, 0);

	int TotalLength = sizeof(Frame.Header) + Length;

	unsigned char * pTail = (unsigned char *)&Frame + TotalLength;

	*(unsigned short *)pTail = Crc;

	TotalLength += 2;

	return m_pComport->SendFrame((PBYTE)&Frame, TotalLength) == TotalLength;
}

bool CEoss::SendToAll(SPACKAGE * pPackage, int Length)
{
	SFRAME Frame;

	Frame.sHeader.Receiver = 0;

	Frame.sHeader.Sender = -1;
	Frame.sHeader.Size = Length + (unsigned short)sizeof(Frame.sHeader) + 2;

	memcpy(&Frame.sPackage, pPackage, Length);

	unsigned short Crc = Crc16(&Frame, sizeof(Frame.sHeader) + Length, 0);

	int TotalLength = sizeof(Frame.sHeader) + Length;

	unsigned char * pTail = (unsigned char *)&Frame + TotalLength;

	*(unsigned short *)pTail = Crc;

	TotalLength += 2;

	return m_pComport->SendFrame((PBYTE)&Frame, TotalLength) == TotalLength;
}


//-----------------------------------------------------------------------------
//
// receive a package
// returns true on success
//
bool CEoss::RecvFrom(PACKAGE * pPackage, int * pLength)
{
	FRAME Frame;

	int Received = m_pComport->RecvFrame((PBYTE)&Frame, sizeof(FRAME));

	if (Received < (sizeof(Frame.Header) + 2))
		return false; // too few

	if (Frame.Header.Sender != m_Address)
		return false; // not from the seal

	if (Frame.Header.Receiver != 0)
		return false; // not for us

	Received -= 2;

	unsigned short Crc = *(unsigned short *)((unsigned char *)&Frame + Received);

	if (Crc16(&Frame, Received, 0) != Crc)
		return false; // crc error

	Received -= sizeof(Frame.Header);
	
	memcpy(pPackage, &Frame.Package, Received);

	*pLength = Received;

	return true; 
}

//-----------------------------------------------------------------------------
//
// receive a package
// returns count of received packets on success
//
int CEoss::RecvFromAll()
{
	IFRAME Frame;
	int iCount = 0;
	int Received;
	while ((Received = m_pComport->RecvFrame((PBYTE)&Frame, sizeof(Frame))) > 0)
	{

		//if we timed out on too small a frame then we may have a 
		//problem with all of the next frames also so drain the input queue
//		if (Received < (sizeof(Frame.sHeader) + 2))
//		{
//			while ((Received = m_pComport->RecvFrame((PBYTE)&Frame,sizeof(Frame))) != 0);
//			return FALSE; // too few bytes -- ignore this frame
//		}
		if (Received == sizeof(Frame))
		{
			Received -= 2;
	
			unsigned short Crc = *(unsigned short *)((unsigned char *)&Frame + Received);

		//if we have a crc error then lets drain the queue and start over
//		if (Crc16(&Frame, Received, 0) != Crc)
//		{
//			while ((Received = m_pComport->RecvFrame((PBYTE)&Frame,sizeof(Frame))) != 0);
//			return FALSE; // crc error
//		}

			if (Crc16(&Frame, Received, 0) == Crc)
			{
				//Received -= sizeof(Frame.sHeader);
				if (iCount < MAX_EOSS)
					EOSSes[iCount++] = Frame.sHeader.Sender;
			}
		}
	}
	return iCount; 
}


//-----------------------------------------------------------------------------
//
// get initial challenge token
// returns true on success
//
bool CEoss::GetChallenge(void)
{
	PACKAGE Package;
	Package.Type = PCK_GETCHALLENGE;

	bool Result = SendTo(&Package, sizeof(Package.Type));
	if (!Result) return Result;

	int Length;
	Result = RecvFrom(&Package, &Length);
	if (!Result) return Result;

	if (Length != sizeof(CHALLENGE))
		return false;

	if (Package.Type != PCK_CHALLENGE)
		return false;

	m_KeysetNumber = Package.Challenge.KeySetNumber;
	m_KeysetTime = Package.Challenge.KeySetTime.HalfDays * 43200 + (43200 - Package.Challenge.KeySetTime.Seconds);
	m_Version[0] = Package.Challenge.Version1;
	m_Version[1] = Package.Challenge.Version2;
	memcpy(ChallengeToken, Package.Challenge.Token, sizeof(DESBLOCK));

	return true;
}

//-----------------------------------------------------------------------------
// 
// send request and receive reply
// returns true on success
//
bool CEoss::Reqply(PACKAGE * pPackage, int DataBytesToSend, int DataBytesExpected)
{
	pPackage->Type = PCK_REQUEST;

	// authorization
	if (!m_pDongle->Authorize(m_KeysetNumber, m_KeysetTime,
		ChallengeToken, &pPackage->Request, DataBytesToSend / sizeof(DESBLOCK)))
	{
		notes = m_pDongle->GetNotes(); m_pDongle->ResetNotes(); // Seen very often: 1) 0x80040005 Need to run the object to perform this operation, followed by 2) 0x800706ba The RPC server is unavailable, or 1) 0x80040005 The interface is unknown followed by 2) supra
		notes += "\nRequest authorization failed";
		return false;
	}

	// encryption (by means of 'decryption' direction)
	if (!m_pDongle->Decipher(m_KeysetNumber, m_KeysetTime, 
		(DESBLOCK *)&pPackage->Request.Data, DataBytesToSend / sizeof(DESBLOCK)))
	{
		notes = m_pDongle->GetNotes(); m_pDongle->ResetNotes();
		notes += "\nRequest encryption failed";
		return false;
	}


	if (!SendTo(pPackage, sizeof(pPackage->Request) - sizeof(pPackage->Request.Data) + DataBytesToSend))
	{
		notes = "Request SendTo failed";
		return false;
	}


	int Length;
	if (!RecvFrom(pPackage, &Length))
	{
		notes = "Request RecvFrom failed";
		return false;
	}


	if (pPackage->Type != PCK_REPLY)
	{
		notes = "Reply type '" + char(pPackage->Type);
		notes += "' not 'R'";
		return false;
	}


	if (DataBytesExpected)
	{
		if (Length != (DataBytesExpected + sizeof(pPackage->Reply.Type) + sizeof(pPackage->Reply.Token) + sizeof(pPackage->Reply.Authentication)))
		{
			notes = "Reply data length is the expected length:" + Length;
			return false;
		}


		// decryption		
		if (!m_pDongle->Decipher(m_KeysetNumber, m_KeysetTime,
			(DESBLOCK *)&pPackage->Reply.Data, DataBytesExpected / sizeof(DESBLOCK)))
		{
			notes = m_pDongle->GetNotes(); m_pDongle->ResetNotes();
			notes += "\nReply decryption failed";
			return false;
		}


		// identification 
		if (!m_pDongle->Identify(m_KeysetNumber, m_KeysetTime,
			ChallengeToken, &pPackage->Reply, DataBytesExpected / sizeof(DESBLOCK)))
		{
			notes = m_pDongle->GetNotes(); m_pDongle->ResetNotes();
			notes += "\nReply identification failed";
			return false;
		}
	}
	else
	{
		if (Length != (sizeof(pPackage->Reply.Type) + sizeof(pPackage->Reply.Token)))
		{
			notes = "Reply length mismatch";
			return false;
		}

	}

	memcpy(ChallengeToken, pPackage->Reply.Token, sizeof(DESBLOCK));

	return true;
}
//-----------------------------------------------------------------------------
// 
// send request and receive reply
// returns true on success
//
int CEoss::ReqWho(SPACKAGE * pPackage, int DataBytesToSend, int /*DataBytesExpected*/)
{
	if (!SendToAll(pPackage, DataBytesToSend))
		return 0;

	return RecvFromAll();
}

//-----------------------------------------------------------------------------
//
// put up to 3 captital letters into 16 bits
// (there's no error checking here...)
// 
unsigned short MakeID(char * szOperator)
{
	unsigned short Id = 0;
	while (char Letter = *szOperator++)
	{
		Letter -= 'A';
		Id *= 27;
		Id += Letter + 1;
	}
	return Id;
}

//-----------------------------------------------------------------------------
// 
// create log-on event
// returns true on success
//
bool CEoss::Logon(char * szOperator1, char * szOperator2)
{
	PACKAGE Package;

	Package.Request.Command = CMD_LOGON;
	Package.Request.Parameter = 0;

	Package.Request.Data.Initials.Operator1 = MakeID(szOperator1);
	Package.Request.Data.Initials.Operator2 = MakeID(szOperator2);

	return Reqply(&Package, sizeof(INITIALS), 0);
}

//-----------------------------------------------------------------------------
// 
// get seal status
// returns true on success
//
bool CEoss::GetStatus(STATUS * pStatus)
{
	PACKAGE Package;
	
	Package.Request.Command = CMD_STATUS;
	Package.Request.Parameter = 0;

	if (Reqply(&Package, 0, sizeof(STATUS)))
	{
		memcpy(pStatus, &Package.Reply.Data.Status, sizeof(STATUS));
		
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// 
// get log information
// returns true on success
//
bool CEoss::GetLogInfo(LOGINFO * pLogInfo)
{
	PACKAGE Package;

	Package.Request.Command = CMD_LOGINFO;
	Package.Request.Parameter = 0;

	if (Reqply(&Package, 0, sizeof(LOGINFO)))
	{
		memcpy(pLogInfo, &Package.Reply.Data.LogInfo, sizeof(LOGINFO));
		
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// 
// get log information (who is out there)
// returns true on success
//
unsigned int CEoss::GetSealInfo()
{
	SPACKAGE Package;
	Package.usType = CMD_LOGINFO;
	Package.ulLowest = 0;
	Package.ulHighest = 0xffffffff;
	unsigned int uiCount,uiTemp,uiLoopCount;
	for (int i = 0; i < 32; i++)
		Package.iNodes[i] = 0;
	uiCount = 0;
	uiLoopCount = 0;
	while ((uiTemp = ReqWho(&Package, sizeof(SPACKAGE), sizeof(Package.iNodes))) > 0)
	{
		if (uiTemp != FALSE)
		{
			uiCount = uiTemp;
			for (int i = 0; i < 32; i++)
				Package.iNodes[i] = EOSSes[i];
		}
		if (++uiLoopCount > 10) // jfl: tries 10 times
			break;
	}
	return uiCount;
}

//-----------------------------------------------------------------------------
// 
// get one log page
// returns true on success
//
bool CEoss::GetLogPage(LOGPAGE * pLogPage, unsigned char PageNumber)
{
	PACKAGE Package;

	Package.Request.Command = CMD_LOGPAGE;
	Package.Request.Parameter = PageNumber;

	if (Reqply(&Package, 0, sizeof(LOGPAGE)))
	{
		memcpy(pLogPage, &Package.Reply.Data.Page, sizeof(LOGPAGE));
		
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// 
// create logoff event
// returns true on success
//
bool CEoss::Logoff(void)
{
	PACKAGE Package;

	Package.Request.Command = CMD_LOGOFF;
	Package.Request.Parameter = 0;

	return Reqply(&Package, 0, 0);
}

//-----------------------------------------------------------------------------

void EossToSystemTime(SYSTEMTIME * pSystemTime, EOSSTIME * pEossTime)
{
	SYSTEMTIME SystemTime;
	
	SystemTime.wYear = 2000;
	SystemTime.wMonth = 1;
	SystemTime.wDay = 1;
	SystemTime.wHour = 0;
	SystemTime.wMinute = 0;
	SystemTime.wSecond = 0;
	SystemTime.wMilliseconds = 0;

	ULONGLONG FileTime;
	
	SystemTimeToFileTime(&SystemTime, (LPFILETIME)&FileTime);

	ULONGLONG EossTime = pEossTime->HalfDays * 43200 + (43200 - pEossTime->Seconds);

	FileTime += (EossTime * 10000000);
	
	FileTimeToSystemTime((LPFILETIME)&FileTime, pSystemTime);
}