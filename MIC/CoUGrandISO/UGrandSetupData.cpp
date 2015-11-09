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
// UGrandSetupData.cpp

#include "stdafx.h"
#include "UGrandSetupData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUGrandSetupData

CUGrandSetupData::CUGrandSetupData()
{
	m_uItterator = 0;

	static LPTSTR szRow[] =	{
		"INQUIRE Response Record","SET_IRR_TIME","","Read Only",
		"-Board Serial Number","SET_IRR_BOARDSN","","Read Only",

		"µGRAND Status Record (55)","SET_UGSR_TIME","","Read Only",
		"-+5.0 Volt Reading","SET_UGSR_50V","","Read Only",
		"-+3.3 Volt Reading","SET_UGSR_33V","","Read Only",
		"-+2.5 Volt Reading","SET_UGSR_25V","","Read Only",
		"-PHD Volt Reading","SET_UGSR_PHDVOLT","","Read Only",
		"-Status Byte 1","SET_UGSR_STATUSBYTE1","","Read Only",
		"-Status Byte 2","SET_UGSR_STATUSBYTE2","","Read Only",
		"-Status Byte 3","SET_UGSR_STATUSBYTE3","","Read Only",
		"-Status Byte 4","SET_UGSR_STATUSBYTE4","","Read Only",
		"-CPU Up-Time(sec)","SET_UGSR_CPUUPTIME","","Read Only",
		"-CPU Temperature","SET_UGSR_CPUTEMP","","Read Only",
		"-Board Temperature","SET_UGSR_BOARDTEMP","","Read Only",
		"-Reserved","SET_UGSR_RESERVED","","Read Only",

		"PSB #1 Status Record (56)","SET_PSBSR_TIME1","","Read Only",				
		"-External PS Voltage","SET_PSBSR_EXTERNPSVOLTS1","","Read Only",		
		"-Status","SET_PSBSR_STATUS1","","Read Only",			
		"-5.0 V Output","SET_PSBSR_50VOLTOUT1","","Read Only",			
		"-3.3 V Output","SET_PSBSR_33VOLTOUT1","","Read Only",			
		"-HV Output V","SET_PSBSR_HVVOLTOUT1","","Read Only",			
		"-HV Current","SET_PSBSR_HVCURROUT1","","Read Only",			
		"-CPU Temp (C)","SET_PSBSR_CPUTEMPERATURE1","","Read Only",	
		"-Board Temp (C)","SET_PSBSR_BOARDTEMPERATURE1","","Read Only",	
		"-Serial Number","SET_PSBSR_PSBSERIALNUM1","","Read Only",		
		"-Reserved","SET_PSBSR_RESERVED1","","Read Only",			

		"PSB #2 Status Record (56)","SET_PSBSR_TIME2","","Read Only",				
		"-External PS Voltage","SET_PSBSR_EXTERNPSVOLTS2","","Read Only",		
		"-Status","SET_PSBSR_STATUS2","","Read Only",			
		"-5.0 V Output","SET_PSBSR_50VOLTOUT2","","Read Only",			
		"-3.3 V Output","SET_PSBSR_33VOLTOUT2","","Read Only",			
		"-HV Output V","SET_PSBSR_HVVOLTOUT2","","Read Only",			
		"-HV Current","SET_PSBSR_HVCURROUT2","","Read Only",			
		"-CPU Temp (C)","SET_PSBSR_CPUTEMPERATURE2","","Read Only",	
		"-Board Temp (C)","SET_PSBSR_BOARDTEMPERATURE2","","Read Only",	
		"-Serial Number","SET_PSBSR_PSBSERIALNUM2","","Read Only",		
		"-Reserved","SET_PSBSR_RESERVED2","","Read Only",			

		"PSB #3 Status Record (56)","SET_PSBSR_TIME3","","Read Only",				
		"-External PS Voltage","SET_PSBSR_EXTERNPSVOLTS3","","Read Only",	
		"-Status","SET_PSBSR_STATUS3","","Read Only",			
		"-5.0 V Output","SET_PSBSR_50VOLTOUT3","","Read Only",		
		"-3.3 V Output","SET_PSBSR_33VOLTOUT3","","Read Only",		
		"-HV Output V","SET_PSBSR_HVVOLTOUT3","","Read Only",		
		"-HV Current","SET_PSBSR_HVCURROUT3","","Read Only",		
		"-CPU Temp (C)","SET_PSBSR_CPUTEMPERATURE3","","Read Only",	
		"-Board Temp (C)","SET_PSBSR_BOARDTEMPERATURE3","","Read Only",	
		"-Serial Number","SET_PSBSR_PSBSERIALNUM3","","Read Only",		
		"-Reserved","SET_PSBSR_RESERVED3","","Read Only",			

		"PSB #4 Status Record (56)","SET_PSBSR_TIME4","","Read Only",				
		"-External PS Voltage","SET_PSBSR_EXTERNPSVOLTS4","","Read Only",	
		"-Status","SET_PSBSR_STATUS4","","Read Only",			
		"-5.0 V Output","SET_PSBSR_50VOLTOUT4","","Read Only",		
		"-3.3 V Output","SET_PSBSR_33VOLTOUT4","","Read Only",		
		"-HV Output V","SET_PSBSR_HVVOLTOUT4","","Read Only",		
		"-HV Current","SET_PSBSR_HVCURROUT4","","Read Only",		
		"-CPU Temp (C)","SET_PSBSR_CPUTEMPERATURE4","","Read Only",	
		"-Board Temp (C)","SET_PSBSR_BOARDTEMPERATURE4","","Read Only",	
		"-Serial Number","SET_PSBSR_PSBSERIALNUM4","","Read Only",		
		"-Reserved","SET_PSBSR_RESERVED4","","Read Only",	
		
		"BMB #1 Status Record (57)","SET_BMBSR_TIME1","","Read Only",
		"-Charge Current","SET_BMBSR_CHARGECURNT1","","Read Only",
		"-Discharge Current","SET_BMBSR_DISCHGCURNT1","","Read Only",
		"-Charge / Discharge Cycles","SET_BMBSR_CDCYCLES1","","Read Only",
		"-CPU Temp. (C)","SET_BMBSR_CPUTEMP1","","Read Only",
		"-Board Temp. (C)","SET_BMBSR_BOARDTEMP1","","Read Only",
		"-Time On Battery","SET_BMBSR_TIMEONBAT1","","Read Only",
		"-Battery Temp.","SET_BMBSR_BATTTEMP1","","Read Only",
		"-Battery Status","SET_BMBSR_BATTSTATUS1","","Read Only",
		"-Serial Number","SET_BMBSR_SERIALNUM1","","Read Only",
		"-Reserved","SET_BMBSR_RESERVED1","","",

		"BMB #2 Status Record (57)","SET_BMBSR_TIME2","","Read Only",
		"-Charge Current","SET_BMBSR_CHARGECURNT2","","Read Only",
		"-Discharge Current","SET_BMBSR_DISCHGCURNT2","","Read Only",
		"-Charge / Discharge Cycles","SET_BMBSR_CDCYCLES2","","Read Only",
		"-CPU Temp. (C)","SET_BMBSR_CPUTEMP2","","Read Only",
		"-Board Temp. (C)","SET_BMBSR_BOARDTEMP2","","Read Only",
		"-Time On Battery","SET_BMBSR_TIMEONBAT2","","Read Only",
		"-Battery Temp.","SET_BMBSR_BATTTEMP2","","Read Only",
		"-Battery Status","SET_BMBSR_BATTSTATUS2","","Read Only",
		"-Serial Number","SET_BMBSR_SERIALNUM2","","Read Only",
		"-Reserved","SET_BMBSR_RESERVED2","","",

		"BMB #3 Status Record (57)","SET_BMBSR_TIME3","","Read Only",
		"-Charge Current","SET_BMBSR_CHARGECURNT3","","Read Only",
		"-Discharge Current","SET_BMBSR_DISCHGCURNT3","","Read Only",
		"-Charge / Discharge Cycles","SET_BMBSR_CDCYCLES3","","Read Only",
		"-CPU Temp. (C)","SET_BMBSR_CPUTEMP3","","Read Only",
		"-Board Temp. (C)","SET_BMBSR_BOARDTEMP3","","Read Only",
		"-Time On Battery","SET_BMBSR_TIMEONBAT3","","Read Only",
		"-Battery Temp.","SET_BMBSR_BATTTEMP3","","Read Only",
		"-Battery Status","SET_BMBSR_BATTSTATUS3","","Read Only",
		"-Serial Number","SET_BMBSR_SERIALNUM3","","Read Only",
		"-Reserved","SET_BMBSR_RESERVED3","","",

		"BMB #4 Status Record (57)","SET_BMBSR_TIME4","","Read Only",
		"-Charge Current","SET_BMBSR_CHARGECURNT4","","Read Only",
		"-Discharge Current","SET_BMBSR_DISCHGCURNT4","","Read Only",
		"-Charge / Discharge Cycles","SET_BMBSR_CDCYCLES4","","Read Only",
		"-CPU Temp. (C)","SET_BMBSR_CPUTEMP4","","Read Only",
		"-Board Temp. (C)","SET_BMBSR_BOARDTEMP4","","Read Only",
		"-Time On Battery","SET_BMBSR_TIMEONBAT4","","Read Only",
		"-Battery Temp.","SET_BMBSR_BATTTEMP4","","Read Only",
		"-Battery Status","SET_BMBSR_BATTSTATUS4","","Read Only",
		"-Serial Number","SET_BMBSR_SERIALNUM4","","Read Only",
		"-Reserved","SET_BMBSR_RESERVED4","","",

		"µGRAND Information Record (5a)","SET_UGIR_TIME","","Read Only",
		"-Actel Version","SET_UGIR_ACTELVER","","Read Only",
		"-Firmware Version","SET_UGIR_FIRMWAREVER","","Read Only",
		"-Firmware Checksum","SET_UGIR_FIRMWARECKSUM","","Read Only",
		"-Serial Number","SET_UGIR_SERIALNUM","","Read Only",

		"ESB #1 Information Record (5b)","SET_ESBIR_TIME1","","Read Only",
		"-Firmware Version","SET_ESBIR_FIRMWAREVER1","","Read Only",
		"-Firmware Checksum","SET_ESBIR_FIRMWARECKSUM1","","Read Only",
		"-Serial Number","SET_ESBIR_SERIALNUM1","","Read Only",

		"ESB #2 Information Record (5b)","SET_ESBIR_TIME2","","Read Only",
		"-Firmware Version","SET_ESBIR_FIRMWAREVER2","","Read Only",
		"-Firmware Checksum","SET_ESBIR_FIRMWARECKSUM2","","Read Only",
		"-Serial Number","SET_ESBIR_SERIALNUM2","","Read Only",

		"ESB #3 Information Record (5b)","SET_ESBIR_TIME3","","Read Only",
		"-Firmware Version","SET_ESBIR_FIRMWAREVER3","","Read Only",
		"-Firmware Checksum","SET_ESBIR_FIRMWARECKSUM3","","Read Only",
		"-Serial Number","SET_ESBIR_SERIALNUM3","","Read Only",

		"ESB #4 Information Record (5b)","SET_ESBIR_TIME4","","Read Only",
		"-Firmware Version","SET_ESBIR_FIRMWAREVER4","","Read Only",
		"-Firmware Checksum","SET_ESBIR_FIRMWARECKSUM4","","Read Only",
		"-Serial Number","SET_ESBIR_SERIALNUM4","","Read Only",

		"PSB #1 Information Record (5c)","SET_PSBIR_TIME1","","Read Only",
		"-Firmware Version","SET_PSBIR_FIRMWAREVER1","","Read Only",
		"-Firmware Checksum","SET_PSBIR_FIRMWARECKSUM1","","Read Only",
		"-Serial Number","SET_PSBIR_SERIALNUM1","","Read Only",

		"PSB #2 Information Record (5c)","SET_PSBIR_TIME2","","Read Only",
		"-Firmware Version","SET_PSBIR_FIRMWAREVER2","","Read Only",
		"-Firmware Checksum","SET_PSBIR_FIRMWARECKSUM2","","Read Only",
		"-Serial Number","SET_PSBIR_SERIALNUM2","","Read Only",

		"PSB #3 Information Record (5c)","SET_PSBIR_TIME3","","Read Only",
		"-Firmware Version","SET_PSBIR_FIRMWAREVER3","","Read Only",
		"-Firmware Checksum","SET_PSBIR_FIRMWARECKSUM3","","Read Only",
		"-Serial Number","SET_PSBIR_SERIALNUM3","","Read Only",

		"PSB #4 Information Record (5c)","PET_ESBIR_TIME4","","Read Only",
		"-Firmware Version","SET_PSBIR_FIRMWAREVER4","","Read Only",
		"-Firmware Checksum","SET_PSBIR_FIRMWARECKSUM4","","Read Only",
		"-Serial Number","SET_PSBIR_SERIALNUM4","","Read Only",

		"BMB #1 Information Record (5d)","SET_BMBIR_TIME1","","Read Only",
		"-Firmware Version","SET_BMBIR_FIRMWAREVER1","","Read Only",
		"-Firmware Checksum","SET_BMBIR_FIRMWARECKSUM1","","Read Only",
		"-Serial Number","SET_BMBIR_SERIALNUM1","","Read Only",

		"BMB #2 Information Record (5d)","SET_BMBIR_TIME2","","Read Only",
		"-Firmware Version","SET_BMBIR_FIRMWAREVER2","","Read Only",
		"-Firmware Checksum","SET_BMBIR_FIRMWARECKSUM2","","Read Only",
		"-Serial Number","SET_BMBIR_SERIALNUM2","","Read Only",

		"BMB #3 Information Record (5d)","SET_BMBIR_TIME3","","Read Only",
		"-Firmware Version","SET_BMBIR_FIRMWAREVER3","","Read Only",
		"-Firmware Checksum","SET_BMBIR_FIRMWARECKSUM3","","Read Only",
		"-Serial Number","SET_BMBIR_SERIALNUM3","","Read Only",

		"BMB #4 Information Record (5d)","SET_BMBIR_TIME4","","Read Only",
		"-Firmware Version","SET_BMBIR_FIRMWAREVER4","","Read Only",
		"-Firmware Checksum","SET_BMBIR_FIRMWARECKSUM4","","Read Only",
		"-Serial Number","SET_BMBIR_SERIALNUM4","","Read Only",

		"µGRAND Config Record (50)","SET_UGGCR_TIME","","Read Only",
		"-Acquisition Interval","SET_UGGCR_COUNTTIME","","",
		"-Baud Rate","SET_UGGCR_BAUDRATE","","",
		"-Frame","SET_UGGCR_FRAME","","",
		"-PHD V. Setting","SET_UGGCR_PHDV","","",
		"-CPU Speed Select","SET_UGGCR_OSCSET","","",
		"-Mode Flags","SET_UGGCR_MODEFLAGS","","",
		"-Filter Control","SET_UGGCR_FILTERCONT","","",
		"-Imm. Buff. Size","SET_UGGCR_IMMBUFSIZE","","",
		"-Imm. Buff. Save Size","SET_UGGCR_IMMBUFFSAVESIZE","","",
		"-Local Background Size","SET_UGGCR_LOCBACKSIZE","","",
		"-Local Background End","SET_UGGCR_LOCBACKEND","","",
		"-Status Rec. Interval","SET_UGGCR_STATUSINT","","",
		"-Time Sync State","SET_UGGCR_TIMESYNC_STATE","","",
		"-Hourly Sync Time","SET_UGGCR_TIMESYNC_HOUR","","",
		"-Daily Sync Time","SET_UGGCR_TIMESYNC_DAILY","","",
		"-Sync. Low Tol.","SET_UGGCR_TIMESYNC_LOWTOL","","",
		"-Sync. Up Tol.","SET_UGGCR_TIMESYNC_UPPTOL","","",
		"-Inspector ID","SET_UGGCR_INSPECTOR_ID","","",
		"-Unit ID","SET_UGGCR_UNIT_ID","","",
		"-Impedance Settings","SET_UGGCR_IMPEDANCE_SET","","",
		"-Ch0 Kalman R","SET_UGGCR_CH0KALR","","",
		"-Ch1 Kalman R","SET_UGGCR_CH1KALR","","",
		"-Ch2 Kalman R","SET_UGGCR_CH2KALR","","",
		"-Ch3 Kalman R","SET_UGGCR_CH3KALR","","",
		"-Ch0 Kalman Q","SET_UGGCR_CH0KALQ","","",
		"-Ch1 Kalman Q","SET_UGGCR_CH1KALQ","","",
		"-Ch2 Kalman Q","SET_UGGCR_CH2KALQ","","",
		"-Ch3 Kalman Q","SET_UGGCR_CH3KALQ","","",

		"ESB #1 Config Record (52)","SET_ESBCR_TIME1","","Read Only",
		"-Nominal Acq. Interval","SET_ESBCR_NORMACQINTERV1","","",
		"-Anomalous Acq. Interval","SET_ESBCR_ANOMACQINTERV1","","",
		"-Low Power Acq. Interval","SET_ESBCR_LOPWACQINTERV1","","",
		"-Mode Flags","SET_ESBCR_MODEFLAGS1","","",
		"-Serial Number","SET_ESBCR_ESBSERIALNUM1","","",

		"ESB #2 Config Record (52)","SET_ESBCR_TIME2","","Read Only",
		"-Nominal Acq. Interval","SET_ESBCR_NORMACQINTERV2","","",
		"-Anomalous Acq. Interval","SET_ESBCR_ANOMACQINTERV2","","",
		"-Low Power Acq. Interval","SET_ESBCR_LOPWACQINTERV2","","",
		"-Mode Flags","SET_ESBCR_MODEFLAGS2","","",
		"-Serial Number","SET_ESBCR_ESBSERIALNUM2","","",

		"ESB #3 Config Record (52)","SET_ESBCR_TIME3","","Read Only",
		"-Nominal Acq. Interval","SET_ESBCR_NORMACQINTERV3","","",
		"-Anomalous Acq. Interval","SET_ESBCR_ANOMACQINTERV3","","",
		"-Low Power Acq. Interval","SET_ESBCR_LOPWACQINTERV3","","",
		"-Mode Flags","SET_ESBCR_MODEFLAGS3","","",
		"-Serial Number","SET_ESBCR_ESBSERIALNUM3","","",

		"ESB #4 Config Record (52)","SET_ESBCR_TIME4","","Read Only",
		"-Nominal Acq. Interval","SET_ESBCR_NORMACQINTERV4","","",
		"-Anomalous Acq. Interval","SET_ESBCR_ANOMACQINTERV4","","",
		"-Low Power Acq. Interval","SET_ESBCR_LOPWACQINTERV4","","",
		"-Mode Flags","SET_ESBCR_MODEFLAGS4","","",
		"-Serial Number","SET_ESBCR_ESBSERIALNUM4","","",

		"PSB #1 Config Record (53)","SET_PSBCR_TIME1","","Read Only",
		"-Mode Flags","SET_PSBCR_MODEFLAGS1","","",
		"-HV Set Point","SET_PSBCR_HVSETPOINT1","","",
		"-HV Config State","SET_PSBCR_CONFIGSTATE1","","",
		"-Serial Number","SET_PSBCR_PSBSERIALNUM1","","",

		"PSB #2 Config Record (53)","SET_PSBCR_TIME2","","Read Only",
		"-Mode Flags","SET_PSBCR_MODEFLAGS2","","",
		"-HV Set Point","SET_PSBCR_HVSETPOINT2","","",
		"-HV Config State","SET_PSBCR_CONFIGSTATE2","","",
		"-Serial Number","SET_PSBCR_PSBSERIALNUM2","","",

		"PSB #3 Config Record (53)","SET_PSBCR_TIME3","","Read Only",
		"-Mode Flags","SET_PSBCR_MODEFLAGS3","","",
		"-HV Set Point","SET_PSBCR_HVSETPOINT3","","",
		"-HV Config State","SET_PSBCR_CONFIGSTATE3","","",
		"-Serial Number","SET_PSBCR_PSBSERIALNUM3","","",

		"PSB #4 Config Record (53)","SET_PSBCR_TIME4","","Read Only",
		"-Mode Flags","SET_PSBCR_MODEFLAGS4","","",
		"-HV Set Point","SET_PSBCR_HVSETPOINT4","","",
		"-HV Config State","SET_PSBCR_CONFIGSTATE4","","",
		"-Serial Number","SET_PSBCR_PSBSERIALNUM4","","",

		"BMB #1 Config Record (54)","SET_BMBCR_TIME1","","Read Only",
		"-Mode Flags","SET_BMBCR_MODEFLAGS1","","",
		"-Pwr. On Status Interv.","SET_BMBCR_PONSTATINT1","","",
		"-Pwr. Off Status Interv.","SET_BMBCR_POFFSTATINT1","","",
		"-Serial Number","SET_BMBCR_BMBSERIALNUM1","","",
		
		"BMB #2 Config Record (54)","SET_BMBCR_TIME2","","Read Only",
		"-Mode Flags","SET_BMBCR_MODEFLAGS2","","",
		"-Pwr. On Status Interv.","SET_BMBCR_PONSTATINT2","","",
		"-Pwr. Off Status Interv.","SET_BMBCR_POFFSTATINT2","","",
		"-Serial Number","SET_BMBCR_BMBSERIALNUM2","","",
		
		"BMB #3 Config Record (54)","SET_BMBCR_TIME3","","Read Only",
		"-Mode Flags","SET_BMBCR_MODEFLAGS3","","",
		"-Pwr. On Status Interv.","SET_BMBCR_PONSTATINT3","","",
		"-Pwr. Off Status Interv.","SET_BMBCR_POFFSTATINT3","","",
		"-Serial Number","SET_BMBCR_BMBSERIALNUM3","","",
		
		"BMB #4 Config Record (54)","SET_BMBCR_TIME4","","Read Only",
		"-Mode Flags","SET_BMBCR_MODEFLAGS4","","",
		"-Pwr. On Status Interv.","SET_BMBCR_PONSTATINT4","","",
		"-Pwr. Off Status Interv.","SET_BMBCR_POFFSTATINT4","","",
		"-Serial Number","SET_BMBCR_BMBSERIALNUM4","","",

		"Channel Configuration Record (51)","SET_CCR_TIME","","Read Only",
		"-Used for Analysis","SET_CCR_USED","","",
		"-Threshold 1 Type and Direction","SET_CCR_T1TYPEDIR","","",
		"-Threshold 2 Type and Direction","SET_CCR_T2TYPEDIR","","",
		"-Ch0 Threshold 1 Value","SET_CCR_C0T1VALUE","","",
		"-Ch1 Threshold 1 Value","SET_CCR_C1T1VALUE","","",
		"-Ch2 Threshold 1 Value","SET_CCR_C2T1VALUE","","",
		"-Ch3 Threshold 1 Value","SET_CCR_C3T1VALUE","","",
		"-Ch0 Threshold 2 Value","SET_CCR_C0T2VALUE","","",
		"-Ch1 Threshold 2 Value","SET_CCR_C1T2VALUE","","",
		"-Ch2 Threshold 2 Value","SET_CCR_C2T2VALUE","","",
		"-Ch3 Threshold 2 Value","SET_CCR_C3T2VALUE","","",
		"-Ch0 Threshold 1 Entry/Exit","SET_CCR_C0T1EE","","",
		"-Ch1 Threshold 1 Entry/Exit","SET_CCR_C1T1EE","","",
		"-Ch2 Threshold 1 Entry/Exit","SET_CCR_C2T1EE","","",
		"-Ch3 Threshold 1 Entry/Exit","SET_CCR_C3T1EE","","",
		"-Ch0 Threshold 2 Entry/Exit","SET_CCR_C0T2EE","","",
		"-Ch1 Threshold 2 Entry/Exit","SET_CCR_C1T2EE","","",
		"-Ch2 Threshold 2 Entry/Exit","SET_CCR_C2T2EE","","",
		"-Ch3 Threshold 2 Entry/Exit","SET_CCR_C3T2EE","","",
		"-Ch0 Changing Sig. Multip","SET_CCR_C0CCM","","",
		"-Ch1 Changing Sig. Multip","SET_CCR_C1CCM","","",
		"-Ch2 Changing Sig. Multip","SET_CCR_C2CCM","","",
		"-Ch3 Changing Sig. Multip","SET_CCR_C3CCM","","",
		"-Ch0 Changing Sig. Entry/Exit","SET_CCR_C0CSEE","","",
		"-Ch1 Changing Sig. Entry/Exit","SET_CCR_C1CSEE","","",
		"-Ch2 Changing Sig. Entry/Exit","SET_CCR_C2CSEE","","",
		"-Ch3 Changing Sig. Entry/Exit","SET_CCR_C3CSEE","","",
		"-Ch0 Filter Limit","SET_CCR_FILTERLIM0","","",
		"-Ch1 Filter Limit","SET_CCR_FILTERLIM1","","",
		"-Ch2 Filter Limit","SET_CCR_FILTERLIM2","","",
		"-Ch3 Filter Limit","SET_CCR_FILTERLIM3","","",
		"-Ch0 Hysteresis","SET_CCR_HYSTER0","","",
		"-Ch1 Hysteresis","SET_CCR_HYSTER1","","",
		"-Ch2 Hysteresis","SET_CCR_HYSTER2","","",
		"-Ch3 Hysteresis","SET_CCR_HYSTER3","","",

		"Trigger Configuration Record (45)","SET_TCR_TIME","","Read Only",
		"-Trigger Pin 4 Logic","SET_TCR_TRIGGER4LOGIC","","",
		"-Trigger Pin 4 Ch/Type","SET_TCR_TRIGGER4CHTYPE","","",
		"-Trigger Pin 5 Logic","SET_TCR_TRIGGER5LOGIC","","",
		"-Trigger Pin 5 Ch/Type","SET_TCR_TRIGGER5CHTYPE","","",
		"-Trigger Pin 6 Logic","SET_TCR_TRIGGER6LOGIC","","",
		"-Trigger Pin 6 Ch/Type","SET_TCR_TRIGGER6CHTYPE","","",
		"-Trigger Pin 7 Logic","SET_TCR_TRIGGER7LOGIC","","",
		"-Trigger Pin 7 Ch/Type","SET_TCR_TRIGGER7CHTYPE","","",

		"Info Record (30)","SET_IIR_TIME","","Read Only",

		NULL
	};
	lpszRow = szRow;
}

CUGrandSetupData::~CUGrandSetupData()
{
}


char *
CUGrandSetupData::GetNext()
{
	if (lpszRow[m_uItterator])
		return lpszRow[m_uItterator++];
	else
		return NULL;
}


BEGIN_MESSAGE_MAP(CUGrandSetupData, CStatic)
	//{{AFX_MSG_MAP(CGrandSetupData)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUGrandSetupData message handlers
