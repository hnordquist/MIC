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

#include "stdafx.h"

#include "dongle.h"

#include "crc.h"

//-----------------------------------------------------------------------------

#define KEY_AUTHENTICATION		0
#define KEY_AUTHORIZATION		1
#define KEY_IDENTIFICATION		2
#define KEY_ENCRYPTION			3

//-----------------------------------------------------------------------------

CDongle::CDongle(void)
{
	HRESULT Result;
	
	/*
   IClassFactory * ClassFactory;
   Result = CoGetClassObject(__uuidof(EossCrypto), CLSCTX_LOCAL_SERVER, 0, IID_IClassFactory, (void **)&ClassFactory);
	printf("\nGetClassObject: %x", Result);
   Result = ClassFactory->CreateInstance(0, __uuidof(IEossCrypto), (void **)&m_pEossCrypto);
	printf("\nCreateInstance: %x", Result);
	*/

	Result = CoCreateInstance(__uuidof(EossCrypto), 0, 
				CLSCTX_LOCAL_SERVER , __uuidof(IEossCrypto), (void **)&m_pEossCrypto);
	if (!SUCCEEDED(Result))
	{
		strcpy_s(szMessage,sizeof(szMessage),"EOSS Crypto Dongle Error: ");
		switch (Result)
		{
		case (REGDB_E_CLASSNOTREG):
			strcat_s(szMessage,sizeof(szMessage),"REGDB_E_CLASSNOTREG ");
			break;
		case (CLASS_E_NOAGGREGATION):
			strcat_s(szMessage,sizeof(szMessage),"CLASS_E_NOAGGREGATION ");
			break;
		case (E_NOINTERFACE):
			strcat_s(szMessage,sizeof(szMessage),"E_NOINTERFACE ");
			break;
		default:
			sprintf_s(szMessage,sizeof(szMessage),"EOSS Crypto Dongle Error: %x",Result);
		}
		throw szMessage;
	}
}

//-----------------------------------------------------------------------------

CDongle::~CDongle(void)
{
	m_pEossCrypto->Release();
}

//-----------------------------------------------------------------------------


bool CDongle::Hash(DESBLOCK * pBlocks, int nBlocks, DESBLOCK Value)
{
	int Bytes = nBlocks * sizeof(DESBLOCK);
	BSTR bstrInput = SysAllocStringByteLen(NULL, Bytes);
	memcpy(bstrInput, pBlocks, Bytes);

	BSTR bstrOutput = SysAllocStringByteLen(NULL, sizeof(DESBLOCK));
	memcpy(bstrOutput, Value, sizeof(DESBLOCK));

	HRESULT Result = m_pEossCrypto->Hash(&bstrInput, nBlocks, &bstrOutput);
	
	memcpy(Value, bstrOutput, sizeof(DESBLOCK));

	SysFreeString(bstrInput);
	SysFreeString(bstrOutput);

	return Result == S_OK;
}

//-----------------------------------------------------------------------------

bool CDongle::Encrypt(unsigned long KeysetNumber, unsigned long KeysetTime, int Key,
					DESBLOCK * pBlocks, int nBlocks)
{
	int Bytes = nBlocks * sizeof(DESBLOCK);
	BSTR bstrData = SysAllocStringByteLen(NULL, Bytes);
	memcpy(bstrData, pBlocks, Bytes);

	HRESULT Result = m_pEossCrypto->Encrypt(KeysetNumber, KeysetTime, Key,
								&bstrData, nBlocks);

	memcpy(pBlocks, bstrData, Bytes);

	SysFreeString(bstrData);

	return Result == S_OK;
}

//-----------------------------------------------------------------------------

bool CDongle::Decrypt(unsigned long KeysetNumber, unsigned long KeysetTime, int Key,
					DESBLOCK * pBlocks, int nBlocks)
{
	int Bytes = nBlocks * sizeof(DESBLOCK);
	BSTR bstrData = SysAllocStringByteLen(NULL, Bytes);
	memcpy(bstrData, pBlocks, Bytes);

	HRESULT Result = m_pEossCrypto->Decrypt(KeysetNumber, KeysetTime, Key, &bstrData, nBlocks);

	memcpy(pBlocks, bstrData, Bytes);

	SysFreeString(bstrData);

	bool bres = (Result == S_OK);
	if (!bres)
		notes.Format(_T("IEossCrypto Decrypt failure: 0x%lx, %lu %lu %d %d"), 
						Result, KeysetNumber, KeysetTime, Key, nBlocks);
	return bres;

}

//-----------------------------------------------------------------------------

bool CDongle::Authorize(unsigned long KeysetNumber, unsigned long KeysetTime,
					DESBLOCK Challenge, REQUEST * pRequest, int nBlocks)
{
	DESBLOCK Block;
	memcpy(Block, Challenge, sizeof(DESBLOCK));

	Block[0] ^= pRequest->Command;
	Block[2] ^= pRequest->Parameter;

	unsigned short Crc = Crc16(&pRequest->Data, nBlocks * sizeof(DESBLOCK), 0);

	Block[4] ^= (Crc & 0xFF);
	Block[6] ^= (Crc >> 8);

	if (!Decrypt(KeysetNumber, KeysetTime, KEY_AUTHORIZATION, (DESBLOCK *)Block, 1))
	{
		return false;
	}

	memcpy(pRequest->Authorization, Block, sizeof(DESBLOCK));

	return true;
}

//-----------------------------------------------------------------------------

bool CDongle::Decipher(unsigned long KeysetNumber, unsigned long KeysetTime,
				DESBLOCK * pData, int nBlocks)
{
	return Decrypt(KeysetNumber, KeysetTime, KEY_ENCRYPTION, pData, nBlocks);
}

//-----------------------------------------------------------------------------

bool CDongle::Identify(unsigned long KeysetNumber, unsigned long KeysetTime,
				DESBLOCK Challenge, REPLY * pReply, int nBlocks)
{
	if (!Hash((DESBLOCK *)&pReply->Data, nBlocks, Challenge))
		return false;

	if (!Encrypt(KeysetNumber, KeysetTime, KEY_IDENTIFICATION, (DESBLOCK *)Challenge, 1))
		return false;

	return memcmp(Challenge, pReply->Authentication, sizeof(DESBLOCK)) == 0;
}

//-----------------------------------------------------------------------------


