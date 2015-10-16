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

#include <windows.h>

class CComport
{
public:
	CComport(unsigned Port, unsigned Timeout);
	~CComport(void);
private:
	HANDLE m_hComport;
public:
	int Send(PBYTE pBuffer, int Count);
	int Recv(PBYTE pBuffer, int Count);
public:
	int SendFrame(PBYTE pBuffer, int Count);
	int RecvFrame(PBYTE pBuffer, int Size);
};


#define SOF_CHAR 0xC0
#define EOF_CHAR 0xC1
#define ESC_CHAR 0x7D
#define ESC_MASK 0x20
