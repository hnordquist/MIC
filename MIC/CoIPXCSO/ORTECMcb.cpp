// ORTECMcb.cpp

#include "stdafx.h"
#include "ORTECMcb.h"
#include "ComORTEC.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CORTECMcb::CORTECMcb(char *DetName, DWORD DetId, CSpctrm *pSpecInfo, void *pParent) 
	: CMcb(DetName, DetId, pSpecInfo)
{
//	TRACE("CORTECMcb::CTOR() pid:%u tid:0x%X\n", _getpid(),GetCurrentThreadId());
	m_pParent = (CComORTEC *)pParent;
	m_bQuiet = false;
}

CORTECMcb::~CORTECMcb()
{
//	TRACE("CORTECMcb::DTOR() pid:%u tid:0x%X\n", _getpid(),GetCurrentThreadId());
}

void CORTECMcb::SetQuiet(bool quiet)
{
	m_bQuiet = quiet;
}

/*++  send command to MCB and receive the response */
bool CORTECMcb::SendCommand(char *Cmd) 
{
//	TRACE("CORTECMcb::SendCommand(%s)\n",Cmd);

	// send the command
	if (!m_bQuiet)
		m_pParent->SendToWatch(strlen(Cmd),Cmd,0);

	bool result =  CMcb::SendCommand(Cmd);

	// process the reply
	if (CMcb::iLength != 0) //have a response
	{
		// for the CSO to forward to the cracker
		m_pParent->CommandAndResponse(Cmd,CMcb::szReply);

		if (!m_bQuiet)
			m_pParent->SendToWatch(CMcb::iLength - 1, CMcb::szReply, 0);
	}
	return result;
}
