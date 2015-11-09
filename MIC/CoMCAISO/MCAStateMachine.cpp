/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
// MCAStateMachine.cpp

#include "stdafx.h"
#include "MCAInst.h"
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
void CMCAInst::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//wparam is the type of message and lparam is the checksum good or bad
	//this is true for all messages EXCEPT the battery backed value message
	//in this one case lparam contains the number of bytes in bbm

	//throw away any message which has a bad checksum -- it can't be
	//trusted.  the tick timer will sum up a counter to force a 
	//retransmition if necessary.  The checksum routine will record
	//the message with an error to the proper files
	int i;

	m_pNow = COleDateTime::GetCurrentTime();
	int dCurrentDay = m_pNow.GetDay();
	if (dCurrentDay != m_dPreviousDay)
	{
		m_dPreviousDay = dCurrentDay;
		m_dBreakSentCount = 0;
		m_dTimeOutCount = 0;
		m_dInvalidTimeCount=0;
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,"33560 MCA Start new day\n\r");
	}
	//if we couldn't register this instrument 
	//or if disk is full so we are blocking write
	//then lock it up in state 9900 -- don't send anything
	if (!m_bGoodRegister || m_bBlockWrite)
		m_dCurrent_State = 9900;

	if ((WParam != IDC_BBM_VALUE) && (LParam == CHECKSUM_BAD)) 
	{
		m_bChecksumFail = true;
		return;
	}

	if (
		(WParam == IDC_BADYEAR_VALUE)&&
		m_bGoodRegister &&
		((m_dCurrent_State < 8000)||(m_dCurrent_State > 8999))
	   )
	{
		m_bDoingAutoTimeSet = true;
		m_dTickToInquire = m_dInquireInt;
	}

    if (WParam == MONITORFAIL_RECORD)
	{
		m_pButton->SetErrorIcon_Skull();
		m_pButton->put_Flash(VARIANT_TRUE);
		m_pButton->put_FaceColor(COLOR_ERROR);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR);
		FlagComm(1,true);			
		return;
	}

	if (WParam == IDC_BBM_VALUE)
		m_iCurrentBBM = LParam;

	//count the time since the last send of any kind
	//      the time since the last inquire2
	//      the time since the last dumpstat (iff != 0)
	if (WParam == TIMER_TICK)
	{
		//bump time increments to next inquire
		m_dTickToInquire++;
		//bump time increments to next status
		if (m_dStatusInt) m_dTickToStatus++;
		//bump time from last command
		m_dTick_Count++;
		if (
		    (m_dRetransmit > m_dCommFail) 
			&& (m_dCurrent_State != 9010) 
			&& (m_dCurrent_State != 9100) 
			&& (m_dCurrent_State != 9101) 
			&& (m_dCurrent_State != 9500)
		   )
		{		
				m_dTimeOutCount++;
				m_dRetransmit = 0;
				m_dTick_Count =	0;
				m_dCurrent_State = 9000; //hard fail
		}
	}

	if (
		(WParam == FORCEDATETIME)&&
		(m_bGoodRegister)&&
		(m_dCurrent_State < 8000)
	   )
	{
		m_dSave_State = m_dCurrent_State;
		m_dCurrent_State = 8000;
	}

	if (m_dCurrent_State < 9000)
		m_bInShutDown = false;
	else
		m_bInShutDown = true;

	if (m_dCurrent_State >= 1100)
		m_bInStartUp = false;
	else
		m_bInStartUp = true;
	//this is the virtual instrument state machine
	switch (m_dCurrent_State)
	{
	//start by sending echo,echo
	case 1000:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		if (WParam == TIMER_TICK)
		{
			m_pButton->SetErrorIcon_Flag();
			m_pButton->put_FaceColor(m_bInHardFail?COLOR_ERROR:COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(m_bInHardFail?COLOR_ERROR:COLOR_WAIT,true);
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"ECHO\r",m_iNode);
			m_pPropertySheet->UpdateSummary("ECHO");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33570 ECHO\n\r");
			m_dCurrent_State = 1010;
			m_dPrevious_State = 1000;
		}
		break;

	//wait for response 
	case 1010:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		//something came back but didn't understand anything that came back
		if (WParam == ECHO_2_REC)
		{
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//got a "??" back (mini-grand)
		if (WParam == ECHO_3_REC)
		{
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//got the expected "TIME =" record back
		if (WParam == ECHO_RECORD)
		{
			m_dRetransmit = 0;
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dTimeOutCount = 0;
		}

		if (WParam == BUSY_RECORD)
		{
			m_dPrevious_State = 1000;
			m_dCurrent_State = 9100;
			m_dTimeOutCount = 0;
		}

		//if we didn't get anything back go to transmit second ECHO
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dCurrent_State = 1030;//second transmit
		}
		break;

	case 1030:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		//send second echo command
		if (WParam == TIMER_TICK)
		{
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"ECHO\r",m_iNode);
			m_pPropertySheet->UpdateSummary("ECHO");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33580 ECHO\n\r");
			m_dRetransmit++;
			m_dCurrent_State = 1040;
		}
		break;

	case 1040:
		//something came back but didn't understand anything that came back
		if (WParam == ECHO_2_REC)
		{
			m_dCurrent_State = 1070;
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			break;
		}

		//got a "??" back 
		if (WParam == ECHO_3_REC)
		{
			m_dCurrent_State = 1070;
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			break;
		}

		//got the expected "TIME =" record back
		if (WParam == ECHO_RECORD)
		{
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			break;
		}

		//received a "BUSY"
		if (WParam == BUSY_RECORD)
		{
			m_dPrevious_State = 1000;
			m_dCurrent_State = 9100;
			m_dTimeOutCount = 0;
		}

		//if we didn't get anything back go directly to hard fail
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTimeOutCount++;
			if (m_dRetransmit >= m_dCommFail)
			{
				m_dRetransmit = 0;
				m_dCurrent_State = 9500; //long hard fail
				m_dTick_Count = 0;
			}
			else
				m_dCurrent_State = 9000; //hard fail
		}
		break;

	//got a ?? so we are working on a mini and can get into monitor with a "90"
	case 1070:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		COM_Transmit(ISZSTRING,"90\r",m_iNode);
		m_pPropertySheet->UpdateSummary("90");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33590 90\n\r");
		m_dTick_Count = 0;
		m_dCurrent_State = 9100;//restart
		break;

	//got past the echo,echo,response sequence, now need to send a dumplast
	case 1100:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		//we are in a dumplast sequence
//		m_bInDumpLast = true;
		m_bInDumpLast = false;
		//send the command
		COM_Transmit(ISZSTRING,"DUMPLAST\r",m_iNode);
		m_pPropertySheet->UpdateSummary("DUMPLAST");
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33600 DUMPLAST\n\r");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1110;
		break;

	//wait here for a response to the dumplast
	case 1110:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"33610 MCA Timeout on receive DUMPLAST response\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"33620 MCA Timeout on receive DUMPLAST response (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL,"33610 MCA Timeout on receive DUMPLAST response\n\r");
		} 

		if (WParam == BUSY_RECORD)
		{
			m_dTimeOutCount = 0;
			m_dPrevious_State = 1100;
			m_dCurrent_State = 9100;
		}

		//break out and wait for correct response
		if ((WParam != END_DUMP_RECORD) && (WParam != END_DUMP2_RECORD))
			break;
		//got it 
		m_dTimeOutCount = 0;

		//do we need to save these records -- the only way we need to is if
		//we sent a dumpok and then didn't get a dumpok response back and 
		//consequently are resetting this instrument
		if (
			//we are not in initial startup and
			(m_dPrevMCATime) &&
			//we did get some records in the dumplast and
			(m_pPropertySheet->m_dAcquireRecordCount) &&
			//those records date time are after the latest recorded
			(m_dPrevMCATime < m_pPropertySheet->MCAAcquireRec[0].JulianSeconds)
		   )
		{
			int temp = m_pPropertySheet->m_dAcquireRecordCount;

			for (i = 0;i<temp;i++)
			{
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"33630 Acquire Record from DUMPLAST written in MCA file (15)\n\r");
				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33630 Acquire Record from DUMPLAST written in MCA file (15)\n\r");
				//issue warning if out of sequence
				if (m_dPrevMCATime > m_pPropertySheet->MCAAcquireRec[i].JulianSeconds)
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"33640 Acquire Record Out Of Order (15)\n\r");
				//update the "most recent" acquire record time
				m_dPrevMCATime = m_pPropertySheet->MCAAcquireRec[i].JulianSeconds;
				//write this record in the bid file

				bool bAccept = true;
#ifdef ALLOWTRIGGERHOLDOFF
				POSITION pos = m_ExclusionList.GetHeadPosition();
				if (pos)
				{
					for (int i=0;i < m_ExclusionList.GetCount();i++)
					{
						EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
						if (m_dPrevMCATime > sExclusionRec.uiStartTime && 
							m_dPrevMCATime < sExclusionRec.uiEndTime)
						{
							bAccept = false;
						}
					}
				}
#endif
				if (bAccept)
				{
					if (m_pPropertySheet->WriteToMCAFile(
						&(m_pPropertySheet->AcquireTime[i]),
						&m_pPropertySheet->MCAAcquireRec[i]) == false)
					{
						m_pPropertySheet->UpdateSummary("Could not open MCA file on DUMPLAST");
					}
				}
#ifdef ALLOWTRIGGERHOLDOFF
				else
				{
					CString cMessage;
					cMessage.Format("Excluded Record: %d",m_dPrevMCATime);
					m_pPropertySheet->UpdateSummary((LPSTR)LPCSTR(cMessage));
					PostToWatch((LPSTR)LPCSTR(cMessage));
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"33650 Acquire Record from DUMPLAST in an EXCLUSION time (15)\n\r");
					if (m_bDoDump)
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33650 Acquire Record from DUMPLAST in an EXCLUSION time (15)\n\r");
				}
#endif

			}
		}
		m_pPropertySheet->m_dAcquireRecordCount = 0;
		//don't save any more acquire records -- until next dumpbbm
		m_bInDumpLast = true;


		//so... we are no longer in dump last sequence
//		m_bInDumpLast = false;
		//prepare for the dumpstat command
		m_dRetransmit = 0;
		//go to that state
		m_dCurrent_State = 1200;
		//fall through to do the next transmit iff end dump received

	//transmit the dumpstat command
	case 1200:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		//send it
		COM_Transmit(ISZSTRING,"DUMPSTAT\r",m_iNode);
		m_pPropertySheet->UpdateSummary("DUMPSTAT");
		//update the dmp file
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33660 DUMPSTAT\n\r");
		//count the number of times we've sent this command
		m_dRetransmit++;
		//reset the tick count so that we can time out
		m_dTick_Count = 0;
		//set the next state to wait for a dumpstat response
		m_dCurrent_State = 1210;
		//start with no checksum failures
		m_bChecksumFail = false;
		break;

	//wait for a good response for the dumpstat command
	case 1210:

		//timeout if we've waited here too long
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTimeOutCount++;
			m_dTick_Count = 0;
			m_dCurrent_State = 1200;
			break;
		}

		if (WParam == BUSY_RECORD)
		{
			m_dPrevious_State = 1200;
			m_dCurrent_State = 9100;
			m_dTimeOutCount = 0;
		}

		//wait here if we don't have a end dump record
		if ((WParam != END_DUMP_RECORD) && (WParam != END_DUMP2_RECORD))
			break;

		//got a good end dump record
		m_dTimeOutCount = 0;
		if (m_bChecksumFail)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1200;
			break;
		}

		//got a good response 
		//so prepare for the next state
		m_dRetransmit = 0;
		m_dTick_Count = 0;
		m_bChecksumFail = false;

		//set next state to "send analyze command"
		m_dCurrent_State = 1300;
		//fall through to immediately transmit the command

	//send the analyze command
	case 1300:
		//wait here if pausing
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		//send the command
		COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
		m_pPropertySheet->UpdateSummary("ANALYZE");

		//update the dmp file
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33670 ANALYZE\n\r");
		//count the number of times analyze has been sent
		m_dRetransmit++;
		//go to the wait for response state
		m_dCurrent_State = 1310;
		//reset check sum fail to see if we get one on the response
		m_bChecksumFail = false;
		break;

	//wait for a response to the analyze command
	case 1310:
		//time out if need be
		if (m_dTick_Count >= m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1300;
			m_pPropertySheet->m_dAt = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33680 MCA Timeout on receive ANALYZE response\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33690 MCA Timeout on receive ANALYZE response (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "33680 MCA Timeout on receive ANALYZE response\n\r");
		}

		if (WParam == BUSY_RECORD)
		{
			m_dPrevious_State = 1300;
			m_dCurrent_State = 9100;
			m_dTimeOutCount = 0;
		}

		//wait here
		if (WParam != ANALYZE_OK_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		//"normal" operations
		if (m_bDoAnalyzeAfterDT0)
		{
			m_bDoAnalyzeAfterDT0 = false;
			m_dTickToInquire = m_dInquireInt;
		}

		m_dTimeOutCount = 0;
		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_dCurrent_State = 2000;
		m_bChecksumFail = false;
		//fall through to next state

	//state 2000 is the "normal" resting state
	case 2000:

		//we can't have an initial initialization time out any longer
		m_bInitInitTimeOut = false;
		m_dHardFailCount = 0;
		m_dBreakSentCount = 0;
		//if we are paused then don't let state change, just
		//wait for next message to arrive by exiting
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}

		//if we are driving to zero (power fail or copy data) then 
		//we need to empty the immediate buffer on the instrument
		//by sending an analyze command
		if (m_bDoAnalyzeAfterDT0)
		{
			//send this only once
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
			COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
			m_pPropertySheet->UpdateSummary("ANALYZE");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33700 ANALYZE\n\r");

			m_dRetransmit = 0;
			m_dTick_Count = 0;
			m_bChecksumFail = false;
			m_dCurrent_State = 1310;
			break;
		}

		//if enough time has passed that we need to do
		//another inquire2 then do it
		if (m_dTickToInquire >= m_dInquireInt)  
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
			COM_Transmit(ISZSTRING,"INQUIRE2\r",m_iNode);
			m_pPropertySheet->UpdateSummary("INQUIRE2");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33710 INQUIRE2\n\r");

			m_dRetransmit++;
			m_dCurrent_State = 2100; 
			m_dTick_Count = 0;
			break;
			
		} 

		//if StatusInterval is greater than 0 (e.g. we do want to do dumpstats)
		//and enough time has passed then do it.
		if ((m_dStatusInt > 0)&&(m_dTickToStatus >= m_dStatusInt))
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
			COM_Transmit(ISZSTRING,"DUMPSTAT\r",m_iNode);
			m_pPropertySheet->UpdateSummary("DUMPSTAT");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33720 DUMPSTAT\n\r");
			m_dRetransmit++;
			m_dCurrent_State = 2300; 
			m_dTick_Count = 0;
			break;
		}

		//if we aren't going to do an inquire2 or a dumpstat then are we 
		//in the process of doing a dumpbbm--if so then continue with another
		//if the bytes in bbm has been driven to 0 then doingbbm will be false
		if (m_bdoingBBM) 
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
			COM_Transmit(ISZSTRING,"DUMPBBM\r",m_iNode);
			m_pPropertySheet->UpdateSummary("DUMPBBM");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33730 DUMPBBM\n\r");
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_bInDumpLast = false;
			m_dRetransmit++;
			m_dCurrent_State = 2200; 
			m_dTick_Count = 0;
			break;
		} 
		break;

	case 2100:
		//inquire2 has just been sent 
		//normally two messages will be received in response
		//the first is the bbm value and the second is the id2 record
		//in the case of a mini-grand a "??" could also be receive iff
		//the grand has been taken out of monitor mode
		//all other messages are ignored and the state will eventually 
		//time out

		//have we timed out yet--we can timeout on the message we are waiting for
		//so DON'T break just incase
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 2000;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33740 MCA Timeout on INQUIRE2 command\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33750 MCA Timeout on INQUIRE2 command (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "33740 MCA Timeout on INQUIRE2 command\n\r");
			m_pPropertySheet->m_dAt = 0;
		}

		if ((WParam == IDC_BADYEAR_VALUE) && m_bAllowAutoTime)
		{
			m_bDoingAutoTimeSet = true;
			m_dTickToInquire = m_dInquireInt;
			m_dSave_State = 2000;
			m_dCurrent_State = 8000;
			break;
		}

		//got the expected bbm value message -- we should receive the 
		//MONITOR_ID2_RECORD message very shortly
		if ((WParam == IDC_BBM_VALUE))
		{
			m_dTimeOutCount = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if ((LParam > (m_bdoingBBM?0:m_dMaxBBM)) ||
				((LParam > 0) && m_bDriveToZero))
			{
				m_bdoingBBM=true;
				PostMessage(m_uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
			}
			else
			{
				m_bdoingBBM = false;
				if ((m_bDriveToZero)&&!(m_bDoAnalyzeAfterDT0))
				{
					m_bPause = true;
					m_bDriveToZero = false;
				}
			}
		}

		//we got a "??" from a mini-grand so try to reset it
		if (WParam == ECHO_3_REC)
		{
			m_dTimeOutCount = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1070;
		}

		//received the expected monitor_id2 record so go back to "rest" state
		if (WParam == MONITOR_ID2_RECORD)
		{
			m_dTimeOutCount = 0;
			m_dRetransmit = 0;
			if (!m_bDoAnalyzeAfterDT0)
				m_dTickToInquire = 0;
			m_dTick_Count = 0;
			m_dCurrent_State = 2000;
		}

		if (WParam == BUSY_RECORD)
		{
			m_dTimeOutCount = 0;
			m_dPrevious_State = 2000;
			m_dCurrent_State = 9100;
		}

		break;

	case 2200:
		//we just sent a DUMPBBM command
		//time out?  we may time out on the specific message we need so
		//don't break--instead continue to see if we got it
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 2000;
			m_pPropertySheet->m_dAt = 0;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33760 MCA Timeout on DUMPBBM command\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33770 MCA Timeout on DUMPBBM command (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "33760 MCA Timeout on DUMPBBM command\n\r");
		}

		if (WParam == BUSY_RECORD)
		{
			m_dTimeOutCount = 0;
			m_dPrevious_State = 2000;
			m_dCurrent_State = 9100;
		}

		if (WParam == ECHO_3_REC)
		{
			m_dTimeOutCount = 0;
			m_bInDumpLast = true;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1070;
			//caution here -- we'll allow the next if to 
			//break us out
		}

		//wait for end dumpbbm record
		if ((WParam != END_DUMP_RECORD) && (WParam != END_DUMP2_RECORD))
			break;

		//got a dump record so we can continue with the dumpbbm sequence
		m_dTimeOutCount = 0;
		m_dCurrent_State = 2210;
		m_bInDumpLast = true;
		//fall through iff end_dump_record received
		//this allows "quick" looping on the expected dumpbbm/

	case 2210:
		//just received a end dump record and possibly a bunch of other records
		//one or more of which may have a bad checksum
		//if we are paused in the middle of a dumpbbm sequence this is a good state
		//to wait in.
		
		//if the dumpbbm terminated in the "new" end of dump then 
		//we MUST have same number of acquire records that the 
		//instrument thinks it sent.   
		if ((m_dNumberOfAcquires >=0) && (m_dNumberOfAcquires != m_pPropertySheet->m_dAcquireRecordCount))
		{
			m_dCurrent_State = 2000;
			m_bChecksumFail = false;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_bInDumpLast = true;
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, NULL, "33780 INSUFFICIENT NUMBER OF ACQUIRE RECORDS RECEIVED\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33780 INSUFFICIENT NUMBER OF ACQUIRE RECORDS RECEIVED\n\r");
			m_dRetransmit = 0;
			break;
		}

		//if we received ANY checksum errors during the dumpbbm then
		//we want to break out and ask for a retransmit via another dumpbbm
		//but if the number of checksum fails is at hard-coded value then
		//record the hard fail and continue with the next dump
		if (m_bChecksumFail) 
		{ 
			//count this failure
			m_dChecksumCount++;
			//go back to get another dumpbbm
			m_dCurrent_State = 2000;
			//reset the checksum failure flag
			m_bChecksumFail = false;
			//we are going to ask for a new dumpbbm so 
			//get rid of all the existing acquire records
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			//indumplast actually means "not" in dumpbbm
			//the next dumpbbm will set this to false
			m_bInDumpLast = true;

			//hard coded if more than 5 checksum failures 
			//then give up and move on
			if (m_dChecksumCount <= 5)
			{
				break;
			}
			else 
			{
				FlagComm(1,true);
				m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, "33790 HARD CHECKSUM ERROR\n\r");
			}
		}

		//if count was <= 5 we break out of case and don't do the 
		//following stuff
		//reset the checksum count in preparation for the next hard checksum error
		m_dChecksumCount = 0;

		//either we got a good BBMDUMP or we had excessive checksum errors
		//in both cases we need to continue
		m_pButton->put_FaceColor(COLOR_WAIT);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"DUMPOK\r",m_iNode);
		m_pPropertySheet->UpdateSummary("DUMPOK");

		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33800 DUMPOK\n\r");
		m_dRetransmit++;
		m_dCurrent_State = 2220;
		//set these to prime the next state
		m_bdumpOK = false;
		m_bbbmVal = false;
		break;

	case 2220:
		//we just sent a DUMPOK message so we need the dumpok response record
		//we should receive two messages here m_bdumpOK and bbm value

		//got a good dumpok but need to receive the bbm value before we 
		//continue.  so just set one of the flags
		if (WParam == DUMP_OK_RECORD) 
		{
			m_dTimeOutCount = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_bdumpOK = true;
			int temp = m_pPropertySheet->m_dAcquireRecordCount;
			bool bDidWriteThemAll = true;
			m_dTimeOutCount = 0;
			//m_pPropertySheet->m_dAcquireRecordCount = 0;
			for (i = 0;i<temp;i++)
			{
				//issue warning if out of sequence
				if (m_dPrevMCATime && 
					(m_dPrevMCATime > m_pPropertySheet->MCAAcquireRec[i].JulianSeconds))
				{
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"33810 Acquire Record Out Of Order (15)\n\r");
				}
				//update the "most recent" acquire record time
				m_dPrevMCATime = m_pPropertySheet->MCAAcquireRec[i].JulianSeconds;

				bool bAccept = true;
#ifdef ALLOWTRIGGERHOLDOFF
				if (!m_ExclusionList.IsEmpty())
				{
					EXCLUSIONREC sExclusionRec = m_ExclusionList.GetHead();
					if (sExclusionRec.uiEndTime < m_dPrevMCATime)
						m_ExclusionList.RemoveHead();
					POSITION pos = m_ExclusionList.GetHeadPosition();
					if (pos)
					{
						for (int i=0; i < m_ExclusionList.GetCount(); i++)
						{
							EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
							if (m_dPrevMCATime > sExclusionRec.uiStartTime &&
								m_dPrevMCATime < sExclusionRec.uiEndTime)
							{
								bAccept = false;
							}
						}
					}
				}
#endif
				if (bAccept)
				{
					//write this record in the bid file
					if (m_pPropertySheet->WriteToMCAFile(
						&(m_pPropertySheet->AcquireTime[i]),
						&m_pPropertySheet->MCAAcquireRec[i])==false)
					{
						//we failed on this write		
						m_pPropertySheet->UpdateSummary("Could not open MCA file");
						m_dCurrent_State = 2221;
						m_pButton->put_FaceColor(COLOR_CYAN);
						m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN);
						m_dTick_Count = 0;
						m_dTimeOutCount = 0;
						bDidWriteThemAll = false;
						//break out of the for loop
						break;
					}
				}
#ifdef ALLOWTRIGGERHOLDOFF
				else
				{
					CString cMessage;
					cMessage.Format("Excluded Record: %d",m_dPrevMCATime);
					m_pPropertySheet->UpdateSummary((LPSTR)LPCSTR(cMessage));
					PostToWatch((LPSTR)LPCSTR(cMessage));
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"33820 Acquire Record in an EXCLUSION time (15)\n\r");
					if (m_bDoDump)
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33820 Acquire Record in an EXCLUSION time (15)\n\r");
				}
#endif

			}
			if (bDidWriteThemAll)
				m_pPropertySheet->m_dAcquireRecordCount = 0;
			//break out of the case
			if (m_dCurrent_State == 2221)
				break;
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTimeOutCount = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_bInDumpLast = true;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dCurrent_State = 1070;
			break;
		}

		//time out if we need to but check this message anyway by
		//not doing a break in the if
		if ((m_dTick_Count > m_dTimeOut)&&(WParam != DUMP_OK_RECORD))
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 2210;//retransmit as necessary
//			m_pPropertySheet->m_dAt = 0;
//			m_bInDumpLast = true;
//			m_pPropertySheet->m_dAcquireRecordCount = 0;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33830 MCA Timeout on DUMPOK command\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33840 MCA Timeout on DUMPOK command (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "33830 MCA Timeout on DUMPOK command\n\r");
			break;
		}

		//got a bbm value so test the bbm value for empty and
		//set the flag to continue the dump and the got bbm value flag
		if ((WParam == IDC_BBM_VALUE))
		{
			m_dTimeOutCount = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (LParam > 0)
				m_bdoingBBM = true;
			else
			{
				m_bdoingBBM = false;
				if ((m_bDriveToZero) && (!m_bDoAnalyzeAfterDT0))
				{
					m_bPause = true;
					m_bDriveToZero = false;
				}
			}
			m_bbbmVal = true;
		}

		if (WParam == BUSY_RECORD)
		{
			m_dTimeOutCount = 0;
			m_dPrevious_State = 2000;
			m_dCurrent_State = 9100;
			break;
		}

		//iff we received both messages then go back to the "rest" state
		if (m_bdumpOK && m_bbbmVal) 
		{
			PostMessage(m_uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
			m_dCurrent_State = 2000;
		}
		break;

	case 2221:
		if (m_dTick_Count > m_dTimeOut)
		{
			bool bThisTimeFailed = false;
			m_dTimeOutCount++;
			m_dTick_Count = 0;
//			int temp = m_pPropertySheet->m_dAcquireRecordCount;
			if ((m_iLastRecordWritten < 0) || (m_iLastRecordWritten > 32))
				m_iLastRecordWritten = 0;
			for (int i = m_iLastRecordWritten; i < m_pPropertySheet->m_dAcquireRecordCount; i++)
			{
				//issue warning if out of sequence
				if (m_dPrevMCATime && 
					(m_dPrevMCATime > m_pPropertySheet->MCAAcquireRec[i].JulianSeconds))
				{
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"33850 Acquire Record Out Of Order (15)\n\r");
				}
				//update the "most recent" acquire record time
				//m_dPrevISRTime = m_pPropertySheet->ISRAcquireRec[i].JulianSeconds;
				//record we are attempting to write
				m_iLastRecordWritten = i;
				//write this record in the isr file
				if (m_pPropertySheet->WriteToMCAFile(
					&(m_pPropertySheet->AcquireTime[i]),
					&m_pPropertySheet->MCAAcquireRec[i]) == true)
				{
					//ok so just continue
					m_dPrevMCATime = m_pPropertySheet->MCAAcquireRec[i].JulianSeconds;
				}
				else
				{
					//still can't write records so break out of the for loop and try again
					bThisTimeFailed = true;
					m_pPropertySheet->UpdateSummary("Still can not open MCA file");
					break;
				}
			}
			//if we failed and we've retried as many as possible then
			//bail out of trying and lock up the 
			if ((m_dTimeOutCount >= m_dCommFail) && bThisTimeFailed)
			{
				m_pPropertySheet->UpdateSummary("Still can not open BID file -- giving up");
				m_pButton->SetDiskIcon_Floppy();//('<',true);  //disk
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 9800;
			}
			//got a good write this time
			if (!bThisTimeFailed)
			{
				//setup to add new records in the acquirerecord array
				m_pPropertySheet->m_dAcquireRecordCount = 0;
				//set the main button color back to something appropriate
				m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
				//set the property sheet's button color back
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
				//turn off flash
				m_pButton->put_Flash(VARIANT_FALSE);
				//set the disk icon back to the normal disk
				m_pButton->SetDiskIcon_Floppy();//('<',true);
				//turn off the disk icon
				m_pButton->ShowIcon_Disk(VARIANT_FALSE);
				//go back to the normal state
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 2000;
				//if we still have bbm then post a message to trigger 
				//getting more bbm immediately
				if (m_bdoingBBM)
					PostMessage(m_uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
			}
		}
		break;

	case 2300:
		//we just sent a dumpstat command

		//got the end dump record from the dumpstat command
		//so return to the "rest" state
		if ((WParam == END_DUMP_RECORD) || (WParam == END_DUMP2_RECORD))
		{
			m_dTimeOutCount = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_dRetransmit = 0;
			m_dTickToStatus = 0;
			m_bInDumpLast = true;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dCurrent_State = 2000;
			break;
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTimeOutCount = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_bInDumpLast = true;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dCurrent_State = 1070;
			break;
		}

		//time out if we need to but check this record just in case
		//if timeout then return to the "rest" state -- next timer tick will trigger retransmit
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_pPropertySheet->m_dAt = 0;
			m_bInDumpLast = true;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33860 MCA Timeout on DUMPSTAT command\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33870 MCA Timeout on DUMPSTAT command (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "33860 MCA Timeout on DUMPSTAT command\n\r");
			m_dCurrent_State = 2000;
		}

		if (WParam == BUSY_RECORD)
		{
			m_dTimeOutCount = 0;
			m_dPrevious_State = 2000;
			m_dCurrent_State = 9100;
		}

		break;


	case 8000:
		if (m_bPause || m_bDoingAutoTimeSet) 
		{
			SetTimer(TIMER_TICK,1000,NULL);
			if (m_bPause) m_dTickToInquire=0;
			if (m_dStatusInt)m_dTickToStatus=0;
		}

		//send the command
		COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
		m_pPropertySheet->UpdateSummary("ANALYZE");

		//update the dmp file
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "33880 ANALYZE\n\r");
		//count the number of times analyze has been sent
		m_dRetransmit++;
		//go to the wait for response state
		m_dCurrent_State = 8001;
		//reset check sum fail to see if we get one on the response
		m_bChecksumFail = false;
		//reset the tick counter
		m_dTick_Count = 0;
		break;

	//wait for a response to the analyze command
	case 8001:
		//time out if need be
		if (m_dTick_Count >= m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 8000;
			m_pPropertySheet->m_dAt = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33890 MCA Timeout on receive ANALYZE response\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33900 MCA Timeout on receive ANALYZE response (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "33890 MCA Timeout on receive ANALYZE response\n\r");
		}

		if (WParam == BUSY_RECORD)
		{
			m_dPrevious_State = 8001;
			m_dCurrent_State = 9100;
			m_dTimeOutCount = 0;
		}

		//wait here
		if (WParam != ANALYZE_OK_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		//"normal" operations
		if (m_bDoAnalyzeAfterDT0)
		{
			m_bDoAnalyzeAfterDT0 = false;
			m_dTickToInquire = m_dInquireInt;
		}

		m_dTimeOutCount = 0;
//		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
//		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_dCurrent_State = 8003;
		m_bChecksumFail = false;
		//fall through to send end

	case 8003:
		m_dCurrent_State = 8005;
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"END\r",m_iNode);
		m_pPropertySheet->UpdateSummary("END");
		m_pPropertySheet->WriteToFile(TO_PFM|TO_CEV, TYPE_COMP, NULL,"33910 Forcing Instrument to Computer Time\n\r");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL,"33920 END\n\r");
		break;

	case 8005:
		if ((WParam == MINIADC_REC)||(m_dTick_Count >= 4))
		{
			m_dCurrent_State = 8010;
			m_dTick_Count = 0;
		}
		break;

	case 8010:
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8020;
			m_dTick_Count = 0;
			m_pToday = COleDateTime::GetCurrentTime();
			COM_Transmit(ISZSTRING,(char*)LPCSTR(m_pToday.Format("2f %m %d %y %H %M %S\r")),m_iNode);
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP,TYPE_DUMP,NULL,(char*)LPCSTR(m_pToday.Format("33930 2f %m %d %y %H %M %S\r\n")));
			m_pPropertySheet->UpdateSummary((char*)LPCSTR(m_pToday.Format("2f %m %d %y %H %M %S\r")));
		}
		break;

	case 8020:
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8030;
			m_dTick_Count = 0;
			if (!m_bDoingAutoTimeSet)
				m_dTickToInquire=0;
			if (m_dStatusInt)m_dTickToStatus=0;
			KillTimer(TIMER_TICK);
			SetTimer(TIMER_TICK,m_dCycleInt,NULL);
			COM_Transmit(ISZSTRING,"90\r",m_iNode);
			m_pPropertySheet->UpdateSummary("90");
		//	m_dPrevious_Color = m_pButton->GetFaceColor();
			m_pButton->get_FaceColor((ULONG *)&m_dPrevious_Color);
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP,TYPE_DUMP,NULL,"33940 90\r\n");
			m_pButton->put_FaceColor(COLOR_CYAN);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN);
		}
		break;

	case 8030:
		//was 90,000 milli-seconds 
		if (((m_dTick_Count * m_dCycleInt) > DEADTIME) || (WParam == ECHO_4_REC))
		{
			m_dTick_Count = 0;
			m_pButton->put_FaceColor(m_dPrevious_Color);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(m_dPrevious_Color);
			m_dCurrent_State = m_dSave_State;
			//no matter what we don't need
			//to do another auto time set
			m_bDoingAutoTimeSet = false;

		}
		break;

	case 9000:
		//hard fail trying to communicate with instrument
		m_bInHardFail = true;
		m_bInDumpLast = false;
		m_pPropertySheet->m_dAcquireRecordCount = 0;
		//if initialization and hard fail then send the message
		if (m_bInitInitTimeOut)
		{
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "33950 MCA Timeout on MCA initialization\n\r");
			m_bInitInitTimeOut = false;
		}

		//start trying to reset things with a long break 
		//if the comm object won't allow a hard break then 
		//the comm object must catch the command and not do it
		//this command turns on break at the comm object
		if (WParam == TIMER_TICK)
		{
			if (m_dLongBreak == 0)
			{
				m_pButton->put_FaceColor(COLOR_ERROR);
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR);
				FlagComm(1,true);			
				bool Local = false;
				bool HeldOffLocal = false;
				if (m_dRetransmit < (m_dCommFail-1))
				{
					COM_Transmit(ISZSTRING,LONGBREAKSET"\r",m_iNode);
					m_pPropertySheet->UpdateSummary("LONG BREAK SET");
					m_dBreakSentCount++;
				}
				else
				{
					if (COM_Transmit(ISZSTRING,LOCALLONGBREAK"\r",m_iNode)==-1)
					{
						m_pPropertySheet->UpdateSummary("LOCAL LONG BREAK held off");
						HeldOffLocal = true;
					}
					else
					{
						m_pPropertySheet->UpdateSummary("LOCAL LONG BREAK");
						m_dBreakSentCount++;
					}
					Local = true;
				}

				if (m_dBreakSentCount < m_iMaxRepetition)
				{
					if (HeldOffLocal)
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "33960 Local Break held off (start)\n\r");
					else
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"33970 Local Break sent (start)\n\r":"33980 Long Break sent (start)\n\r");
				}
				if (m_dBreakSentCount == m_iMaxRepetition)
				{
					if (HeldOffLocal)
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "33990 Local Break held off (start) (msg filter on)\n\r");
					else
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"34000 Local Break sent (start) (msg filter on)\n\r":"34010 Long Break sent (start) (msg filter on)\n\r");
				}

				if (m_bDoDump)
				{
					if (HeldOffLocal)
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "34020 Local LONG BREAK SET held off\n\r");
					else
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, (Local)?"34030 Local LONG BREAK SET\n\r":"34040 LONG BREAK SET\n\r");
				}

			}
			m_dLongBreak++;
		}

		//wait here for a couple of timer ticks to make 
		//sure a long break is minimum 3 seconds
		if ((m_dLongBreak * m_dCycleInt) > 3000) 
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 9010;
		}
		break;

	case 9010:
		//still in hard fail
		m_bInHardFail = true;

		//send the command to clear the long break
		//then wait in this state for a couple timer cycles
		//to give the instrument time to reset after the long break
		if (WParam == TIMER_TICK)
		{
			if (m_dLongBreak)
			{
				m_dTick_Count = 0;
				COM_Transmit(ISZSTRING,LONGBREAKCLR"\r",m_iNode);
				m_pPropertySheet->UpdateSummary("LONG BREAK CLEAR");
				if (m_dBreakSentCount < m_iMaxRepetition)
					m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "34050 Break sent (end)\n\r");
				if (m_dBreakSentCount == m_iMaxRepetition)
					m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "34060 Break sent (end) (msg filter on)\n\r");
				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "34070 LONG BREAK CLEAR\n\r");
			}
			m_dLongBreak = 0;
			//this is how many timer ticks to wait until we continue with the reset
			if ((m_dTick_Count * m_dCycleInt) > 3000)//wait a minimum 3 seconds or longer
			{
				m_dLongBreak = 1;
				m_dTick_Count = 0;
				m_dHardFailCount++;
				m_dCurrent_State = 9020;//was 1000
			}
		}
		break;

	case 9020:
		if (m_dLongBreak)
		{
			m_dLongBreak = 0;
		//	m_dPrevious_Color = m_pButton->GetFaceColor();
			m_pButton->get_FaceColor((ULONG *)&m_dPrevious_Color);
			m_pButton->put_FaceColor(COLOR_CYAN);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN);
		}
		if ((m_dTick_Count * m_dCycleInt) > DEADTIME)//wait a minimum 30 seconds or longer
		{
			m_dTick_Count++;
			m_dHardFailCount++;
			m_dCurrent_State = 1000;
		}
		break;

		//wait for a while
	case 9100:
		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "34080 MCA Sent BUSY (start wait)\n\r");
	//	m_dPrevious_Color = m_pButton->GetFaceColor();
		m_pButton->get_FaceColor((ULONG *)&m_dPrevious_Color);
		m_pButton->put_FaceColor(COLOR_CYAN);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN);
		m_dCurrent_State = 9101;
		break;

	case 9101:
		if (((m_dTick_Count * m_dCycleInt) > DEADTIME) || (WParam == ECHO_4_REC))
		{
			m_dCurrent_State = m_dPrevious_State;
			m_dTick_Count = 0;
			m_pButton->put_FaceColor(m_dPrevious_Color);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(m_dPrevious_Color);
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "34090 MCA BUSY (end wait)\n\r");
		}
		break;

	//long hard fail recovery
	case 9500:							//	900sec = 15min 900000
		if ((m_dTick_Count * m_dCycleInt) > m_dSlowReset)//wait 15 minutes before we retry
		{
			m_dCurrent_State = 1000;
			m_dTick_Count = 0;
			m_dHardFailCount = 0;
			m_pButton->SetErrorIcon_Flag();
		}
		if (m_dTick_Count % 10 == 0)
		{
			m_pButton->SetErrorIcon_Skull();
			m_pButton->put_FaceColor(COLOR_ERROR);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR);
			FlagComm(1,true);			
		}
		break;

	case 9800:
		m_dCurrent_State = 9810;
		//shut down the instrument
		PauseInstrument(true);
		//but set it to attempt again in the future 
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		m_pButton->put_FaceColor(COLOR_ERROR);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR);
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "34100 WRITE FAILURE on MCA file\n\r");
		break;

	case 9810:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
		}
		else
		{	
			m_bIsPaused = false;
			PauseInstrument(false);
			m_dTick_Count = INT_MAX-1;
			m_dTimeOutCount = 0;
			m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "34110 Attempt restart from WRITE FAILURE on MCA file\n\r");
			m_dCurrent_State = 2221;
		}
		break;

	case 9900:
		if (m_bGoodRegister)
			m_dCurrent_State = 1000;
		else
		{
			m_dCurrent_State = 9900;
			m_pButton->SetErrorIcon_Bomb();
			m_pButton->ShowIcon_Error(VARIANT_TRUE);
			m_pButton->put_FaceColor(COLOR_ERROR);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR);
		}
		break;

	default:
		m_dCurrent_State = 1000;
	}
}
	