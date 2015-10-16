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

#include "comport.h"

//-----------------------------------------------------------------------------

CComport::CComport(unsigned Port, unsigned Timeout)
{	

	wchar_t strDevice[32];
	
	swprintf(strDevice, sizeof(strDevice), _T("\\\\.\\COM%u"), Port);

   m_hComport = CreateFile(strDevice, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
   if (m_hComport == INVALID_HANDLE_VALUE)
	{
		DWORD Error = GetLastError(); 
		throw Error;
	}

   DCB Dcb;
   Dcb.DCBlength = sizeof(DCB);

   if (!GetCommState(m_hComport, &Dcb))
	{
		DWORD Error = GetLastError();
		CloseHandle(m_hComport);
		throw Error;
   }

   Dcb.BaudRate = 9600;
   Dcb.StopBits = ONESTOPBIT;
   Dcb.ByteSize = 8;
   Dcb.Parity = NOPARITY;
   Dcb.fBinary = 1;

   if (!SetCommState(m_hComport, &Dcb))
	{
		DWORD Error = GetLastError();
		CloseHandle(m_hComport);
		throw Error;
   }

   COMMTIMEOUTS Timeouts;
   Timeouts.ReadTotalTimeoutConstant = Timeout;
   Timeouts.WriteTotalTimeoutConstant = Timeout;
   Timeouts.ReadIntervalTimeout = 0;
   Timeouts.ReadTotalTimeoutMultiplier = 0;
	Timeouts.WriteTotalTimeoutMultiplier = 0;

   if (!SetCommTimeouts(m_hComport, &Timeouts))
	{
		DWORD Error = GetLastError();
		CloseHandle(m_hComport);
		throw Error;
   }
}

//-----------------------------------------------------------------------------

CComport::~CComport(void)
{
	CloseHandle(m_hComport);
}

//-----------------------------------------------------------------------------

int CComport::Send(PBYTE pBuffer, int Count)
{
	DWORD Sent;
	WriteFile(m_hComport, pBuffer, Count, &Sent, 0);
	return Sent;
}

//-----------------------------------------------------------------------------

int CComport::Recv(PBYTE pBuffer, int Count)
{
	DWORD Received;
	ReadFile(m_hComport, pBuffer, Count, &Received, 0);
	return Received;
}

//-----------------------------------------------------------------------------

int CComport::SendFrame(PBYTE pBuffer, int Count)
{
	BYTE Byte = SOF_CHAR;
	if (Send(&Byte, 1) == 0)
		return 0;
	
	int Sent = 0;
	while (Sent < Count)
	{
		Byte = pBuffer[Sent];
		if ((Byte == SOF_CHAR) ||
			 (Byte == EOF_CHAR) || 
			 (Byte == ESC_CHAR))
		{
			BYTE Esc = ESC_CHAR;
			if (Send(&Esc, 1) == 0)
				return 0;

			Byte ^= ESC_MASK;
		}

		if (Send(&Byte, 1) == 0)
			return 0;

		Sent++;
	}

	Byte = EOF_CHAR;
	if (Send(&Byte, 1) == 0)
		return 0;

	return Sent;
}

//-----------------------------------------------------------------------------

int CComport::RecvFrame(PBYTE pBuffer, int Size)
{
	int Received = 0;
	bool Start = FALSE;
	bool Escape = FALSE;
	bool bDone = FALSE;

	for (;!bDone;)
	{
//		if (Received > Size)
//			return 0;

		BYTE Byte;
		if (Recv(&Byte, 1) == 0)
		{
			Received = 0;
			bDone = TRUE;
		}
		else
		{
			switch (Byte)
			{
			case SOF_CHAR:
				Received = 0;
				Start = TRUE;
				Escape = FALSE;
				break;

			case EOF_CHAR:
				Start = FALSE;
				Escape = FALSE;
				if (Received)
				{
					bDone = TRUE;
					break;
				}
	//				return Received;
				Received = 0;
				break;

			case ESC_CHAR:
				Escape = TRUE;
				break;

			default:
				if (!Start)
					break;
				if (Escape)
				{
					Byte ^= ESC_MASK;
					Escape = FALSE;
				}
				//if we can put it in the buffer then do so
				//else we may have had a problem with the
				//serial
				if (Received < Size)
					pBuffer[Received++] = Byte;
				else
					bDone = TRUE;			
			}
		}
	}
	return Received;
}

//-----------------------------------------------------------------------------
