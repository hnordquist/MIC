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

#import "tokcom.tlb" no_namespace raw_interfaces_only 
#include <atlstr.h>
class CDongle
{
public:
	CDongle(void);
	~CDongle(void);
private:
	IEossCrypto * m_pEossCrypto;
	char szMessage[80];
private:
	unsigned long m_KeysetNumber;
	unsigned long m_KeysetTime;
	CString notes;
private:
	bool Hash(DESBLOCK * pBlocks, int nBlocks, DESBLOCK Value);
    bool Encrypt(unsigned long KeysetNumber, unsigned long KeysetTime, int Key, DESBLOCK * pBlocks, int nBlocks);
    bool Decrypt(unsigned long KeysetNumber, unsigned long KeysetTime, int Key, DESBLOCK * pBlocks, int nBlocks);
public:
	bool Authorize(unsigned long KeysetNumber, unsigned long KeysetTime,
				DESBLOCK Challenge, REQUEST * pRequest, int nBlocks);

	bool Decipher(unsigned long KeysetNumber, unsigned long KeysetTime,
				DESBLOCK * pData, int nBlocks);

	bool Identify(unsigned long KeysetNumber, unsigned long KeysetTime,
				DESBLOCK Challenge, REPLY * pReply, int nBlocks);

	CString& GetNotes() {return notes; }
	void ResetNotes() {notes = ""; }

};