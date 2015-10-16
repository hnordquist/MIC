//file EOSSStateMachine.cpp

#include "stdafx.h"
#include "EOSSInstrument.h"
#include "ISO_definitions.h"
#include "CSO_definitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//this is the state machine which controls 
//	what commands to send, 
//	when to send them, and
//	what to expect in return.
//It receives a message for each message returned from an instrument
//as well as a "timer" message on intervals selected in "MIC:Configuration"

void CEOSSInstrument::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//wparam is the type of message and lparam is the checksum good or bad
	//this is true for all messages EXCEPT the battery backed value message
	//in this one case lparam contains the number of bytes in bbm

	//throw away any message which has a bad checksum -- it can't be
	//trusted.  the tick timer will sum up a counter to force a 
	//retransmition if necessary.  The checksum routine will record
	//the message with an error to the proper files

//	int temp;
//	char command[32];

	m_pNow = COleDateTime::GetCurrentTime();
	int dCurrentDay = m_pNow.GetDay();
//	if (dCurrentDay != m_dPreviousDay)
//	{
//		m_dPreviousDay = dCurrentDay;
//		m_dBreakSentCount = 0;
//		m_dTimeOutCount = 0;
//		m_dInvalidTimeCount= 0;
//		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,"40013 EOSS Start new day");
//	}

}
