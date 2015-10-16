//file EventStateMachine.cpp

#include "stdafx.h"
#include "EventInstrument.h"
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

void CEventInstrument::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//wparam is the type of message and lparam is the checksum good or bad
	//this is true for all messages EXCEPT the battery backed value message
	//in this one case lparam contains the number of bytes in bbm

	//throw away any message which has a bad checksum -- it can't be
	//trusted.  the tick timer will sum up a counter to force a 
	//retransmition if necessary.  The checksum routine will record
	//the message with an error to the proper files

	int temp;
	char command[32];

	m_pNow = COleDateTime::GetCurrentTime();
	int dCurrentDay = m_pNow.GetDay();
	if (dCurrentDay != m_dPreviousDay)
	{
		m_dPreviousDay = dCurrentDay;
		m_dBreakSentCount = 0;
		m_dTimeOutCount = 0;
		m_dInvalidTimeCount= 0;
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,"40013 EVENT Start new day");
	}

//	if (WParam == ECHO_RECORD)
//	{
//		if (m_IsMiniGrand != 1)
//			m_pPropertySheet->SetUpMiniGrand(1); 
//		m_IsMiniGrand = 1;
//	}
//	if (WParam == ECHO_4_REC)
//	{
//		if (m_IsMiniGrand != 0)
//			m_pPropertySheet->SetUpMiniGrand(0);
//		m_IsMiniGrand = 0;
//		WParam = ECHO_RECORD;
//	}
	//if we couldn't register this instrument 
	//or if disk is full so we are blocking write
	//then lock it up in state 9900 -- don't send anything
	if ((m_bGoodRegister == false) || (m_bBlockWrite == true))
		m_dCurrent_State = 9900;

	if (
		(WParam == IDC_BADYEAR_VALUE)&&
		m_bGoodRegister &&
		((m_dCurrent_State < 8000)||(m_dCurrent_State > 8999))
	   )
	{
		m_bDoingAutoTimeSet = true;
		m_dTickToInquire = m_dInquireInt;
	}

	//if checksum result in LParam then was it bad
	if ((WParam != IDC_BBM_VALUE) && (LParam == CHECKSUM_BAD)) 
	{
		m_bChecksumFail = true;
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
		if (m_dStatusInt)m_dTickToStatus++;
		//bump time from last command
		m_dTick_Count++;
		if ((m_dRetransmit > m_dCommFail) &&
			(m_dCurrent_State != 9010) &&
			(m_dCurrent_State != 9500))
		{		
				m_dRetransmit = 0;
				m_dTick_Count =	0;
				m_dTimeOutCount++;
				m_dCurrent_State = 9000; //hard fail
		}
	}

	if (
		(WParam == FORCEDATETIME)&&
		(m_bGoodRegister)&&
		((m_dCurrent_State < 8000)||(m_dCurrent_State > 8999))
	   )
	{
		m_dSave_State = m_dCurrent_State;
//		switch (m_IsMiniGrand)
//		{
//		case 0:
//			m_dCurrent_State = 8500;
//			break;
//		case 1:
			m_dCurrent_State = 8000;
//			break;
//		case -1:
//		default:
//			m_dCurrent_State = m_dSave_State;
//		}
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
			m_pPropertySheet->m_pCloseButton->SetFaceColor(m_bInHardFail?COLOR_ERROR:COLOR_WAIT);
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"COMSTATUSDE\r",m_iNode+OOBFLAG);
//TRACE("Transmit->COMSTATUSEDE\n");
			m_pPropertySheet->UpdateSummary("COMSTATUSDE");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "40020 COMSTATUSDE");
			m_dCurrent_State = 1010;
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

		//got the expected "TIME =" record back
		if (WParam == ECHO_RECORD)
		{
			m_dRetransmit = 0;
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
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
			COM_Transmit(ISZSTRING,"COMSTATUSDE\r",m_iNode+OOBFLAG);
//TRACE("Transmit->COMSTATUSDE\n");
			m_pPropertySheet->UpdateSummary("COMSTATUSDE");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "40030 COMSTATUSDE");
			m_dRetransmit++;
			m_dCurrent_State = 1040;
		}
		break;

	case 1040:
		//something came back but didn't understand anything that came back
//		if (WParam == ECHO_2_REC)
//		{
//			m_dCurrent_State = 1060;
//			m_dRetransmit = 0;
//			m_dTimeOutCount = 0;
//			break;
//		}
//
//		//got a "??" back (mini-grand)
//		if (WParam == ECHO_3_REC)
//		{
//			m_dCurrent_State = 1070;
//			m_dRetransmit = 0;
//			m_dTimeOutCount = 0;
//			break;
//		}

		//got the expected "TIME =" record back
		if (WParam == ECHO_RECORD)
		{
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			break;
		}

		//if we didn't get anything back go directly to hard fail
		if (m_dTick_Count > m_dTimeOut)
		{
			if (m_dRetransmit >= m_dCommFail)
			{
				m_dRetransmit = 0;
				m_dCurrent_State = 9500; //long hard fail
				m_dTick_Count = 0;
				m_dTimeOutCount++;
			}
			else
			{
				m_dCurrent_State = 9000; //hard fail
				m_dTimeOutCount++;
			}
		}
		break;

//	//didn't understand what came back so try to reset a grand3
//	case 1060:
//		//wait here if paused
//		if (m_bPause) 
//		{
//			m_pButton->SetFaceColor(COLOR_PAUSE,true);
//			m_pPropertySheet->m_pCloseButton->put_FaceColor(COLOR_PAUSE,true);
//			break;
//		}
//
//		//send the string which should bring a grand3 back to monitor mode
//		m_pComm->Transmit(ISZSTRING,"END\r",m_iNode+OOBFLAG);
//		m_pPropertySheet->UpdateSummary("END");
//		m_dCurrent_State = 1061;
//		break;
//
//	case 1061:
//		m_pComm->Transmit(ISZSTRING,"USER\r",m_iNode+OOBFLAG);
//		m_pPropertySheet->UpdateSummary("USER");
//		m_dCurrent_State = 1062;
//		break;
//
//	case 1062:
//		m_pComm->Transmit(ISZSTRING,"YES\r",m_iNode+OOBFLAG);
//		m_pPropertySheet->UpdateSummary("YES");
//		if (m_bDoDump)
//				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "400x0 END\n\rUSER\n\rYES\n\r");
//		m_dTick_Count = 0;
//		m_dCurrent_State = 1000;//restart
//		break;
//
//	//got a ?? so we are working on a mini and can get into monitor with a "90"
//	case 1070:
//		if (m_bPause) 
//		{
//			m_bIsPaused = true;
//			m_pButton->SetFaceColor(COLOR_PAUSE,true);
//			m_pPropertySheet->m_pCloseButton->put_FaceColor(COLOR_PAUSE,true);
//			break;
//		}
//		m_pComm->Transmit(ISZSTRING,"90\r",m_iNode+OOBFLAG);
//		m_pPropertySheet->UpdateSummary("90");
//		if (m_bDoDump)
//				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "400x0 90\n\r");
//		m_dTick_Count = 0;
//		m_dCurrent_State = 1000;//restart
//		break;

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
		m_bInDumpBBM = false;
		//send the command
		command[0] = 0x54;
		command[1] = 0x06;
		PostToWatch("\nDUMPLAST ");
		COM_Transmit(2,command,m_iNode+OOBFLAG);
//TRACE("Transmit->DUMPLAST 54 06\n");
		m_pPropertySheet->UpdateSummary("DUMPLAST (54 06)");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "40030 DUMPLAST (54 06)");
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
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"40040 EVENT Timeout on receive DUMPLAST response");
			else if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"40050 EVENT Timeout on receive DUMPLAST response (msg filter on)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL,"40060 EVENT Timeout on receive DUMPLAST response");
			m_dTimeOutCount++;
		} 
		//beak out and wait for correct response
		if (WParam != END_DUMP_RECORD)
			break;
		//got it 
		m_dTimeOutCount = 0;
		//do we need to save these records -- the only way we need to is if
		//we sent a dumpok and then didn't get a dumpok response back and 
		//consequently are resetting this instrument
		temp = m_pPropertySheet->m_dAcquireRecordCount;
//		if (
//			//we are not in initial startup and
//			(m_dPrevBIDTime) &&
//			//we did get some records in the dumplast and
//			(m_pPropertySheet->m_dAcquireRecordCount) &&
//			//those records date time are after the latest recorded
//			(m_dPrevBIDTime < m_pPropertySheet->AcquireRec[0].JulianSeconds)
//		   )
//		{
//			for (i = 0;i<temp;i++)
//			{
//				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"Acquire Record from DUMPLAST written in BID file (15)\n\r");
//				if (m_bDoDump)
//					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Acquire Record from DUMPLAST written in BID file (15)\n\r");
//				//issue warning if out of sequence
//				if (m_dPrevBIDTime > m_pPropertySheet->AcquireRec[i].JulianSeconds)
//					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"Acquire Record Out Of Order (15)\n\r");
//				//update the "most recent" acquire record time
//				m_dPrevBIDTime = m_pPropertySheet->AcquireRec[i].JulianSeconds;
//				//write this record in the bid file
//				if (m_pPropertySheet->WriteToBIDFile(
//					&(m_pPropertySheet->AcquireTime[i]),
//					&(m_pPropertySheet->AcquireRec[i]),
//					m_pPropertySheet->AcquireCS[i],
//					m_pPropertySheet->AcquireAUTH[i],
//					m_pPropertySheet->AcquireAUTHIgnore[i]) == false)
//				{
//					m_pPropertySheet->UpdateSummary("Could not open EVT file on DUMPLAST");
//				}
//			}
//		}
//		else
//		{
//			//save the time of the latest received  
//			//acquire record for authentication
//			if (temp)
//				m_pPropertySheet->m_dLastAcquireTime = m_pPropertySheet->AcquireRec[temp-1].JulianSeconds;
//		}
//		if (temp) 
//			m_pPropertySheet->m_dPreviousBIDTime = 
//				m_pPropertySheet->AcquireRec[temp-1].JulianSeconds;
//
//		m_pPropertySheet->m_dAcquireRecordCount = 0;
//		//don't save any more acquire records -- until next dumpbbm
		m_bInDumpBBM = false;

		//so... we are no longer in dump last sequence
//		m_bInDumpBBM = false;
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
		command[0] = 0x54;
		command[1] = 0x03;
		PostToWatch("\nDUMPSTAT ");
		COM_Transmit(2,command,m_iNode+OOBFLAG);
//TRACE("Transmit->DUMPSTAT 54 03\n");
		m_pPropertySheet->UpdateSummary("DUMPSTAT (54 03)");
		//update the dmp file
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "40070 DUMPSTAT (54 03)");
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
			m_dTick_Count = 0;
			m_dCurrent_State = 1200;
			m_dTimeOutCount++;
			break;
		}

		//wait here if we don't have a end dump record
		if (WParam != STATUS_RECORD)
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
		m_dCurrent_State = 2000;
		//fall through to immediately transmit the command

	//state 2000 is the "normal" resting state
	case 2000:

		//we can't have an initial initialization time out any longer
		m_bInitInitTimeOut = false;
		m_dHardFailCount = 0;
		m_dBreakSentCount = 0;
		m_dTimeOutCount = 0;
		//if we are paused then don't let state change, just
		//wait for next message to arrive by exiting
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}

		//if enough time has passed that we need to do
		//another inquire2 then do it
		if (m_dTickToInquire >= m_dInquireInt)  
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
			command[0] = 0x54;
			command[1] = 0x04;
			PostToWatch("\nINQUIRE2 ");
			COM_Transmit(2,command,m_iNode+OOBFLAG);
//TRACE("Transmit->INQUIRE2 54 04\n");
			m_pPropertySheet->UpdateSummary("INQUIRE2 (54 04)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "40080 INQUIRE2 (54 04)");

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
			command[0] = 0x54;
			command[1] = 0x03;
			PostToWatch("\nDUMPSTAT ");
			COM_Transmit(2,command,m_iNode+OOBFLAG);
//TRACE("Transmit->DUMPSTAT 54 03\n");
			m_pPropertySheet->UpdateSummary("DUMPSTAT (54 03)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "40090 DUMPSTAT (54 03)");
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
			command[0] = 0x54;
			command[1] = 0x08;
			PostToWatch("\nDUMPBBM ");
			COM_Transmit(2,command,m_iNode+OOBFLAG);
//TRACE("Transmit->DUMPBBM (54 08)\n");
			m_pPropertySheet->UpdateSummary("DUMPBBM (54 08)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "40100 DUMPBBM (54 08)");
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_bInDumpBBM = true;
			m_dRetransmit++;
			m_dCurrent_State = 2200; 
			m_dTick_Count = 0;
			break;//redundant
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
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "40110 EVENT Timeout on INQUIRE2 command");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "40120 EVENT Timeout on INQUIRE2 command (msg filter on)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "40130 EVENT Timeout on INQUIRE2 command");
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
		if (WParam == IDC_BBM_VALUE)
		{
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
				if ((m_bDriveToZero) /*&& (!m_bDoAnalyzeAfterDT0)*/)
				{
					m_bPause = true;
					m_bDriveToZero = false;
				}
			}
			m_dTimeOutCount = 0;
		}

		//received the expected monitor_id2 record so go back to "rest" state
		if (WParam == MONITOR_ID2_RECORD)
		{
			m_dRetransmit = 0;
			m_dTickToInquire = 0;
			m_dTick_Count = 0;
			m_dTimeOutCount = 0;
			m_dCurrent_State = 2000;
		}

		break;

	case 2200:
		//we just sent a DUMPBBM command
		//time out?  we may time out on the specific message we need so
		//don't break--instead continue to see if we got it
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_bInDumpBBM = false;
			m_dCurrent_State = 2000;
			m_pPropertySheet->m_dAt = 0;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dTimeOutCount++;

			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, 
				m_bDoBinaryBBM?"41140 EVENT Timeout on DUMPBBBM command":"41150 EVENT Timeout on DUMPBBM command");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, 
				m_bDoBinaryBBM?"41160 EVENT Timeout on DUMPBBBM command (msg filter on)":"41170 EVENT Timeout on DUMPBBM command (msg filter on)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, 
				m_bDoBinaryBBM?"41180 EVENT Timeout on DUMPBBBM command":"41190 EVENT Timeout on DUMPBBM command");
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
//		if (WParam == ECHO_3_REC)
//		{
//			m_bInDumpBBM = false;
//			m_pPropertySheet->m_dAcquireRecordCount = 0;
//			m_dTick_Count = 0;
//			m_dRetransmit = 0;
//			m_dTimeOutCount = 0;
//			m_dCurrent_State = 1070;
//
//			//caution here -- we'll allow the next if to 
//			//break us out
//		}

		//wait for end dumpbbm record
		if (WParam != END_DUMP_RECORD)
			break;

		//got an end dump record so we can continue with the dumpbbm sequence
		m_dTimeOutCount = 0;
		m_dCurrent_State = 2210;
		m_bInDumpBBM = false;
		//fall through iff end_dump_record received
		//this allows "quick" looping on the expected dumpbbm/

	case 2210:
		//just received an end dump record and possibly a bunch of other records
		//one or more of which may have a bad checksum
		//if we are paused in the middle of a dumpbbm sequence this is 
		//NOT a good state to wait in.   

		//if the dumpbbm terminated in the "new" end of dump then 
		//we MUST have same number of acquire records that the 
		//instrument thinks it sent
		if ((m_dNumberOfAcquires >=0) && (m_dNumberOfAcquires != m_pPropertySheet->m_dAcquireRecordCount))
		{
			m_dCurrent_State = 2000;
			//m_bChecksumFail = false;  //removed by pjm for dp 3 Aug 2005
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_bInDumpBBM = false;
			m_pPropertySheet->UpdateSummary("INSUFFICIENT NUMBER OF ACQUIRE RECORDS RECEIVED");

			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, NULL, "41200 EVENT INSUFFICIENT NUMBER OF ACQUIRE RECORDS RECEIVED");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "41210 EVENT INSUFFICIENT NUMBER OF ACQUIRE RECORDS RECEIVED");
			m_dRetransmit = 0;
			if (!m_bChecksumFail) //changed by pjm for dp 3 Aug 2005
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
			
			//the next dumpbbm will set this to true
			m_bInDumpBBM = false;

			//hard coded if more than 5 checksum failures 
			//then give up and move on
			if (m_dChecksumCount <= m_dCommFail) //5) changed by pjm for dp 3 Aug 2005
			{
				break;
			}
			else 
			{
				FlagComm(1,true);
				m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, "41220 EVENT HARD CHECKSUM ERROR");
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
		command[0] = 0x54;
		command[1] = 0x09;
		PostToWatch("\nDUMPOK");
		COM_Transmit(2,command,m_iNode+OOBFLAG);
//TRACE("Transmit->DUMPOK (54 09)\n");
		m_pPropertySheet->UpdateSummary("DUMPOK (54 09)");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "41230 DUMPOK (54 09)");
		m_dRetransmit++;
		m_dCurrent_State = 2220;
		//set these to prime the next state
		m_bdumpOK = false;
		m_bbbmVal = false;
		break;

	case 2220:
		//we just sent a DUMPOK message so we need the dumpok response record
		//we should receive two messages here m_bdumpOK and bbm value

		//got a bbm value so test the bbm value for empty and
		//set the flag to continue the dump and the got bbm value flag
		if ((WParam == IDC_BBM_VALUE))
		{
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (LParam > 0)
				m_bdoingBBM = true;
			else
			{
				m_bdoingBBM = false;
				if ((m_bDriveToZero) /*&& (!m_bDoAnalyzeAfterDT0)*/)
				{
					m_bPause = true;
					m_bDriveToZero = false;
				}
			}
			m_bbbmVal = true;
		}

		//don't fail on waiting for dump ok response
		//assume that the instrument got at least one
		//of the dumpok messages -- need the check for
		//bbm value message first so we don't loose it
		if (m_dRetransmit > m_dCommFail)
		{
			WParam = DUMP_OK_RECORD;
			m_pPropertySheet->WriteToFile(TO_PFM|TO_CEV, TYPE_COMP, NULL, 
				"41240 EVENT Multiple Timeout on DUMPOK -- assuming ok");
			m_pPropertySheet->UpdateSummary("DUMPOK -- assuming ok");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "41250 DUMPOK --assuming ok");
		}

		//got a good dumpok but need to receive the bbm value before we 
		//continue.  so just set one of the flags
		if (WParam == DUMP_OK_RECORD) 
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_bdumpOK = true;
			int temp = m_pPropertySheet->m_dAcquireRecordCount;
			bool bDidWriteThemAll = true;
			m_dTimeOutCount = 0;
			for (int i = 0;i<temp;i++)
			{
				//issue warning if out of sequence
//				if (m_dPrevBIDTime && 
//					(m_dPrevBIDTime > m_pPropertySheet->AcquireRec[i].uiJulian))
//				{
//					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"Acquire Record Out Of Order (15)");
//				}
				//update the "most recent" acquire record time
//				m_dPrevBIDTime = m_pPropertySheet->AcquireRec[i].JulianSeconds;
				//record we are attempting to write
				m_iLastRecordWritten = i;
				//write this record in the bid file
//				if (m_pPropertySheet->WriteToBIDFile(
//						&m_pPropertySheet->AcquireTime[i],
//						&m_pPropertySheet->AcquireRec[i],
//						m_pPropertySheet->AcquireCS[i],
//						m_pPropertySheet->AcquireAUTH[i], 
//						m_pPropertySheet->AcquireAUTHIgnore[i])==false)
				if (m_pPropertySheet->WriteToEVTFile(i) == false)
				{
					//we failed on this write		
					m_pPropertySheet->UpdateSummary("Could not open EVT file");
					m_dCurrent_State = 2221;
					m_pButton->put_FaceColor(COLOR_CYAN);
					m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN);
					m_dTick_Count = 0;
					m_dTimeOutCount = 0;
					bDidWriteThemAll = false;
					break;
				}
				else
				{
					//we succeeded on this write
					m_pPropertySheet->m_dPreviousEVTTime = m_pPropertySheet->AcquireRec[i].uiJulian;
				}
			}
//			if (bDidWriteThemAll)
//				m_pPropertySheet->m_dAcquireRecordCount = 0;
//
			if (m_dCurrent_State == 2221) break;
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
//		if (WParam == ECHO_3_REC)
//		{
//			m_dTick_Count = 0;
//			m_dRetransmit = 0;
//			m_bInDumpBBM = false;
//			m_pPropertySheet->m_dAcquireRecordCount = 0;
//			m_dTimeOutCount = 0;
//			m_dCurrent_State = 1070;
//			break;
//		}

		//time out if we need to but check this message anyway by
		//not doing a break in the if above
		if ((m_dTick_Count > m_dTimeOut) && (WParam != DUMP_OK_RECORD))
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 2230;//retransmit as necessary
			m_dTimeOutCount++;

			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "41260 EVENT Timeout on DUMPOK command");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "41270 EVENT Timeout on DUMPOK command (msg filter on)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "41280 EVENT Timeout on DUMPOK command");
			break;
		}

		//iff we received both messages then go back to the "rest" state
		if (m_bdumpOK && m_bbbmVal) 
		{
			m_dCurrent_State = 2000;
			if (m_bdoingBBM)
				PostMessage(m_uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
		}

		break;

	case 2221:
		if (m_dTick_Count > m_dTimeOut)
		{
			bool bThisTimeFailed = false;
			m_dTimeOutCount++;
			m_dTick_Count = 0;
//			int temp = m_pPropertySheet->m_dAcquireRecordCount;
//			for (int i = m_iLastRecordWritten; i < m_pPropertySheet->m_dAcquireRecordCount; i++)
//			{
//				//issue warning if out of sequence
//				if (m_dPrevBIDTime && 
//					(m_dPrevBIDTime > m_pPropertySheet->AcquireRec[i].JulianSeconds))
//				{
//					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"Acquire Record Out Of Order (15)");
//				}
//				//update the "most recent" acquire record time
//				m_dPrevBIDTime = m_pPropertySheet->AcquireRec[i].JulianSeconds;
//				//record we are attempting to write
//				m_iLastRecordWritten = i;
//				//write this record in the bid file
//				if (m_pPropertySheet->WriteToBIDFile(
//						&m_pPropertySheet->AcquireTime[i],
//						&m_pPropertySheet->AcquireRec[i],
//						m_pPropertySheet->AcquireCS[i],
//						m_pPropertySheet->AcquireAUTH[i], 
//						m_pPropertySheet->AcquireAUTHIgnore[i])==true)
//				{
//					//ok so just continue
//					m_pPropertySheet->m_dPreviousBIDTime = m_pPropertySheet->AcquireRec[i].JulianSeconds;
//				}
//				else
//				{
//					//still can't write records so break out of the for loop and try again
//					bThisTimeFailed = true;
//					m_pPropertySheet->UpdateSummary("Still can not open BID file");
//					break;
//				}
//			}
			//if we failed and we've retried as many as possible then
			//bail out of trying and lock up the 
			if ((m_dTimeOutCount >= m_dCommFail) && bThisTimeFailed)
			{
				m_pPropertySheet->UpdateSummary("Still can not open EVT file -- giving up");
				m_pButton->SetDiskIcon_Floppy();
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
				m_pButton->SetDiskIcon_Floppy();
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

	case 2230:

		//if we received ANY checksum errors during the dumpbbm then
		//we want to break out and ask for a retransmit via another dumpbbm
		//but if the number of checksum fails is at hard-coded value then
		//record the hard fail and continue with the next dump
		if (m_bChecksumFail) 
		{ 
			//count this failure
			m_dChecksumCount++;
			//reset the checksum failure flag
			m_bChecksumFail = false;
			//hard coded if more than 5 checksum failures 
			//then give up and move on
			if (m_dChecksumCount > 5)
			{
				FlagComm(1,true);
				m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, "41290 EVENT HARD CHECKSUM ERROR ON DUMPBBMOK");
			}
		}

		//either we got a good BBMDUMP or we had excessive checksum errors
		//in both cases we need to continue
		m_pButton->put_FaceColor(COLOR_WAIT);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
		m_dTick_Count = 0;
		command[0] = 0x54;
		command[1] = 0x09;
		PostToWatch("\nDUMPOK Retransmit ");
		COM_Transmit(2,command,m_iNode+OOBFLAG);
//TRACE("Transmit->DUMPOK (54 09)\n");
		m_pPropertySheet->UpdateSummary("DUMPOK (54 09) Retransmit");

		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "41300 DUMPOK (54 09) Retransmit");
		m_dRetransmit++;
		m_dCurrent_State = 2220;
		//set these to prime the next state
		m_bdumpOK = false;
		m_bbbmVal = false;
		break;

	case 2300:
		//we just sent a dumpstat command

		//got the end dump record from the dumpstat command
		//so return to the "rest" state
//		if ((WParam == END_DUMP_RECORD) || (WParam == END_DUMP2_RECORD)) //DGP 2008/09/04
		if (WParam == STATUS_RECORD)
		{
			m_pButton->put_FaceColor(m_bPause?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_dRetransmit = 0;
			m_dTickToStatus = 0;
			m_bInDumpBBM = false;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dCurrent_State = 2000;
			m_dTimeOutCount = 0;
			break;
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_bInDumpBBM = false;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
			break;
		}

		//time out if we need to but check this record just in case
		//if timeout then return to the "rest" state -- next timer tick will trigger retransmit
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_pPropertySheet->m_dAt = 0;
			m_bInDumpBBM = false;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dTimeOutCount++;

			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "41310 EVENT Timeout on DUMPSTAT command");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "41320 EVENT Timeout on DUMPSTAT command (msg filter on)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "41330 EVENT Timeout on DUMPSTAT command");
			m_dCurrent_State = 2000;
		}
		break;


	case 8000:
		//send the command
		{
//			TRACE("Transmit->SetClock\n");
			CString cString;
			COleDateTime Now = COleDateTime::GetCurrentTime();
			int iYear = Now.GetYear();
			while (iYear > 99) iYear -= 100;
			command[0] = 0x49;
			command[1] = (BYTE)iYear;
			command[2] = (BYTE)Now.GetMonth();
			command[3] = (BYTE)Now.GetDay();
			command[4] = (BYTE)Now.GetHour();
			command[5] = (BYTE)Now.GetMinute();
			command[6] = (BYTE)Now.GetSecond();
			COM_Transmit(7,command,m_iNode+OOBFLAG);
			cString = Now.Format("41340 Set Clock to: %Y.%m.%d %H:%M:%S");
			PostToWatch("\n");
			PostToWatch((LPSTR)LPCSTR(cString));
			m_pPropertySheet->UpdateSummary((LPSTR)LPCSTR(cString));
			if (m_bDoDump)
			{
				//cString += "\n\r";
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, (LPSTR)LPCSTR(cString));
			}
		}
		//count the number of times set clock has been sent
		m_dRetransmit++;
		//go to the wait for response state
		m_dCurrent_State = 8001;
		//reset check sum fail to see if we get one on the response
		m_bChecksumFail = false;
		//reset tick counter
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
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "41350 EVENT Timeout on receive ACK or NAK response");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "41360 EVENT Timeout on receive ACK or NAK response (msg filter on)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "41370 EVENT Timeout on receive ACK or NAK response");
		}
		//wait here
		if (WParam != ECHO_RECORD)
			break;

		m_dTimeOutCount = 0;
		//did we get an ACK back
		if (LParam == 1)
		{
			//got through the time set sequence so set up for 
			//"normal" operations
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_dRetransmit = 0;
			m_bChecksumFail = false;
			m_bDoingAutoTimeSet = false;
			m_dCurrent_State = m_dSave_State;
		}
		//or a NAK back
		else
		{
			if (m_dRetransmit > m_iMaxRepetition)
			{
				m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
				m_dRetransmit = 0;
				m_bChecksumFail = false;
				m_bDoingAutoTimeSet = false;
				m_dCurrent_State = m_dSave_State;
			}
			else
				m_dCurrent_State = 8000;
		}

		break;

	case 9000:
		//hard fail trying to communicate with instrument
		m_bInHardFail = true;
		m_bInDumpBBM = false;
		m_pPropertySheet->m_dAcquireRecordCount = 0;
		//if initialization and hard fail then send the message
		if (m_bInitInitTimeOut)
		{
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "41380 EVENT Timeout on EVENT initialization");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "41390 EVENT Timeout on EVENT initialization (msg filter on)");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "41400 EVENT Timeout on EVENT initialization");
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
					COM_Transmit(ISZSTRING,LONGBREAKSET"\r",m_iNode+OOBFLAG);
//TRACE("Transmit->LONGBREAKSET\n");
					m_pPropertySheet->UpdateSummary("LONG BREAK SET");
					m_dBreakSentCount++;
//					if (m_bDoDump)
//						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL,LONGBREAKSET);
				}
				else
				{
//TRACE("Transmit->LOCALLONGBREAK\n");
					if (COM_Transmit(ISZSTRING,LOCALLONGBREAK"\r",m_iNode+OOBFLAG)==-1)
					{
						m_pPropertySheet->UpdateSummary("LOCAL LONG BREAK held off");
						HeldOffLocal = true;
//						if (m_bDoDump)
//							m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL,"LOCAL LONG BREAK held off");
					}
					else
					{
						m_pPropertySheet->UpdateSummary("LOCAL LONG BREAK");
						m_dBreakSentCount++;
//						if (m_bDoDump)
//							m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL,"LOCAL LONG BREAK");
					}
					Local = true;
				}

				if (m_dBreakSentCount < m_iMaxRepetition)
				{
					if (HeldOffLocal)
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "41410 Local Break held off (start)");
					else
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"41420 Local Break sent (start)":"41430 Break sent (start)");
				}
				if (m_dBreakSentCount == m_iMaxRepetition)
				{
					if (HeldOffLocal)
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "41440 Local Break held off (start) (msg filter on)");
					else
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"41450 Local Break sent (start) (msg filter on)":"41460 Break sent (start) (msg filter on)");
				}

				if (m_bDoDump)
				{
					if (HeldOffLocal)
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "41470 Local LONG BREAK SET held off");
					else
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, (Local)?"41480 Local LONG BREAK SET":"41490 LONG BREAK SET");
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
				COM_Transmit(ISZSTRING,LONGBREAKCLR"\r",m_iNode+OOBFLAG);
//TRACE("Transmit->LONGBREAKCLR\n");
				m_pPropertySheet->UpdateSummary("LONG BREAK CLEAR");
				if (m_dBreakSentCount < m_iMaxRepetition)
					m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "41500 Break sent (end)");
				if (m_dBreakSentCount == m_iMaxRepetition)
					m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "41510 Break sent (end) (msg filter on)");
				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "41520 LONG BREAK CLEAR");
			}
			m_dLongBreak = 0;
			//this is how many timer ticks to wait until we continue with the reset
			if ((m_dTick_Count * m_dCycleInt) > 3000)//wait a minimum 3 seconds or longer
			{
				m_dTick_Count = 0;
				m_dHardFailCount++;
				m_dCurrent_State = 1000;
			}
		}
		break;

	//long hard fail recovery
	case 9500:							//	900sec/15min
		if ((m_dTick_Count * m_dCycleInt) > m_dSlowReset)//wait 15 minutes before we retry
		{
			m_dTick_Count = 0;
			m_dHardFailCount = 0;
			m_pButton->SetErrorIcon_Flag();
			m_dCurrent_State = 1000;
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
		//shut down this instrument
		PauseInstrument(true);	
		//but set it to attempt again in the future
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);	//dgp 18 Oct 2006
		m_pButton->put_FaceColor(COLOR_ERROR);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR);
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "41530 WRITE FAILURE on BID file, SUPPORT TERMINATED");
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
			m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "41540 Attempt restart from WRITE FAILURE on BID file");
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
