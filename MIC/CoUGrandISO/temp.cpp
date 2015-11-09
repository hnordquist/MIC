	//some temporary buffers
	char szTemp1[32];
	char szTempFV[32];
	char szTempFC[32];
	char szTempSN[32];
	CString cTemp;/*
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


	//Get a text version of the record
	BMBInfoTextRecord BMBInfoTextRec;
	memset(&BMBInfoTextRec,NULL, sizeof(BMBInfoTextRecord));

	//get the address of a binary record
	BMBInfoRecord* psBMBInfoRecord = (BMBInfoRecord*)pBinaryAt;

	//convert big endian to little endian 
	Reverse((BYTE*)&(psBMBInfoRecord->uiJulianSeconds),sizeof(psBMBInfoRecord->uiJulianSeconds));
	Reverse((BYTE*)&(psBMBInfoRecord->usFirmwareChkSum),sizeof(psBMBInfoRecord->usFirmwareChkSum));

	//only do this if binary record is going to be 
	//entered into the binary file
	memcpy(&(RecordX[m_dRecordCountX]),psBMBInfoRecord,sizeof(BMBInfoRecord));

	//set opcode in text version
	sprintf(szTemp1,"%02x",psBMBInfoRecord->bOpcode);
	memcpy(&BMBInfoTextRec.chOpcode,szTemp1,sizeof(BMBInfoTextRec.chOpcode));

	//set time in text version
	sprintf(szTemp1,"%10d",psBMBInfoRecord->uiJulianSeconds);
	memcpy(&BMBInfoTextRec.cdJulianSeconds,szTemp1,sizeof(BMBInfoTextRec.cdJulianSeconds));

	CJulianTime cTime;
	cTime.Init(psBMBInfoRecord->uiJulianSeconds);

	bool bStale = true;

	//build the serial number and use it to find which column to put this data in
	sprintf(szTempSN,"%02x%02x%02x%02x%02x%02x",
		psBMBInfoRecord->bSerialNumber[0],
		psBMBInfoRecord->bSerialNumber[1],
		psBMBInfoRecord->bSerialNumber[2],
		psBMBInfoRecord->bSerialNumber[3],
		psBMBInfoRecord->bSerialNumber[4],
		psBMBInfoRecord->bSerialNumber[5]);
	memcpy(&BMBInfoTextRec.chSerialNumber,szTempSN,sizeof(BMBInfoTextRec.chSerialNumber));

	int iColumn = 0;
	if      (strncmp((char*)&BMBInfoTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1),sizeof(BMBInfoTextRec.chSerialNumber)) == 0) iColumn = 1;
	else if (strncmp((char*)&BMBInfoTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2),sizeof(BMBInfoTextRec.chSerialNumber)) == 0) iColumn = 2;
	else if (strncmp((char*)&BMBInfoTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3),sizeof(BMBInfoTextRec.chSerialNumber)) == 0) iColumn = 3;
	else if (strncmp((char*)&BMBInfoTextRec.chSerialNumber,GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4),sizeof(BMBInfoTextRec.chSerialNumber)) == 0) iColumn = 4;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1))==0) iColumn = 5;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2))==0) iColumn = 6;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3))==0) iColumn = 7;
	else if (strlen(GetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4))==0) iColumn = 8;

	//get the firmware version
	sprintf(szTempFV,"%02d.%02d.%02d",
		psBMBInfoRecord->b3dFirmwareVersion[0],
		psBMBInfoRecord->b3dFirmwareVersion[1],
		psBMBInfoRecord->b3dFirmwareVersion[2]);
	memcpy(&BMBInfoTextRec.c3dFirmwareVersion,szTempFV,sizeof(BMBInfoTextRec.c3dFirmwareVersion));

	//get the firmware checksum
	sprintf(szTempFC,"%04x",psBMBInfoRecord->usFirmwareChkSum);
	memcpy(&BMBInfoTextRec.cdFirmwareChkSum,szTempFC,sizeof(BMBInfoTextRec.cdFirmwareChkSum));

	if (m_uiBMBInfoRecordTime < cTime.TotalSeconds())
	{
		//newer than the last one received
		bStale = false;

		//save the time of this new record
		m_uiBMBInfoRecordTime = cTime.TotalSeconds();

		//set date and time
		switch (iColumn) {
		case 5:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,BMBInfoTextRec.chSerialNumber,sizeof(BMBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM1,"SET_BMBIR_SERIALNUM1",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 1:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI1,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI1,cTime.Time());
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV1,BMBInfoTextRec.c3dFirmwareVersion,sizeof(BMBInfoTextRec.c3dFirmwareVersion));
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC1,BMBInfoTextRec.cdFirmwareChkSum,sizeof(BMBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBIR_TIME1,"SET_BMBIR_TIME1",cTemp,false);
			if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER1,"SET_BMBIR_FIRMWAREVER1",szTempFV,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM1,"SET_BMBIR_FIRMWARECKSUM1",szTempFC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC1,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;
		case 6:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,BMBInfoTextRec.chSerialNumber,sizeof(BMBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM2,"SET_BMBIR_SERIALNUM2",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL2,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 2:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI2,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI2,cTime.Time());
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV2,BMBInfoTextRec.c3dFirmwareVersion,sizeof(BMBInfoTextRec.c3dFirmwareVersion));
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC2,BMBInfoTextRec.cdFirmwareChkSum,sizeof(BMBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBIR_TIME2,"SET_BMBIR_TIME2",cTemp,false);
			if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER2,"SET_BMBIR_FIRMWAREVER2",szTempFV,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM2,"SET_BMBIR_FIRMWARECKSUM2",szTempFC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC2,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;
		case 7:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,BMBInfoTextRec.chSerialNumber,sizeof(BMBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM3,"SET_BMBIR_SERIALNUM3",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 3:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI3,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI3,cTime.Time());
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV3,BMBInfoTextRec.c3dFirmwareVersion,sizeof(BMBInfoTextRec.c3dFirmwareVersion));
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC3,BMBInfoTextRec.cdFirmwareChkSum,sizeof(BMBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBIR_TIME3,"SET_BMBIR_TIME3",cTemp,false);
			if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER3,"SET_BMBIR_FIRMWAREVER3",szTempFV,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM3,"SET_BMBIR_FIRMWARECKSUM3",szTempFC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC3,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;
		case 8:
			SetString(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,BMBInfoTextRec.chSerialNumber,sizeof(BMBInfoTextRec.chSerialNumber));
			if (!CurrentSetParam(SET_BMBIR_SERIALNUM4,"SET_BMBIR_SERIALNUM4",szTempSN,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_SERIAL4,COLOR_YELLOW);
				m_iESBInfo_Tab |= 0x0002;
			}
			//fall through to process
		case 4:
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATADATEI4,cTime.Date());
			SetString(BMBINFOPAGE,IDC_BMBINFO_DATATIMEI4,cTime.Time());
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV4,BMBInfoTextRec.c3dFirmwareVersion,sizeof(BMBInfoTextRec.c3dFirmwareVersion));
			SetString(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC4,BMBInfoTextRec.cdFirmwareChkSum,sizeof(BMBInfoTextRec.cdFirmwareChkSum));
			cTemp = cTime.Date();
			cTemp += " ";
			cTemp += cTime.Time();
			cTemp += ".";
			cTemp += szTemp1;
			CurrentSetParam(SET_BMBIR_TIME4,"SET_BMBIR_TIME4",cTemp,false);
			if (!CurrentSetParam(SET_BMBIR_FIRMWAREVER4,"SET_BMBIR_FIRMWAREVER4",szTempFV,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREV4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			if (!CurrentSetParam(SET_BMBIR_FIRMWARECKSUM4,"SET_BMBIR_FIRMWARECKSUM4",szTempFC,false))
			{
				SetColor(BMBINFOPAGE,IDC_BMBINFO_FIRMWAREC4,COLOR_YELLOW);
				m_iBMBInfo_Tab |= 0x0002;
			}
			break;
		}
	}

	//got a binary file so don't have a message checksum
	BMBInfoTextRec.chCheckSum[0] = '-';
	BMBInfoTextRec.chCheckSum[1] = '-';

	//set the cr and lf in the message
	BMBInfoTextRec.cCR = '\r';
	BMBInfoTextRec.cLF = '\n';

	//send it off to be displayed if possible
	//and save it for later
	char cBuff[1024];
	strcpy(cBuff,"00000 ");
	strncat(cBuff,(const char*)&BMBInfoTextRec,sizeof(BMBInfoTextRec));
	cBuff[sizeof(BMBInfoTextRec) + 6] = NULL;

	if (!bStale)
	{
		//save the text version to send to the display
		switch (iColumn)
		{
		case 1:
		case 5:
			m_cBMBInfoRecord1 = cBuff+6;
			break;
		case 2:
		case 6:
			m_cBMBInfoRecord2 = cBuff+6;
			break;
		case 3:
		case 7:
			m_cBMBInfoRecord3 = cBuff+6;
			break;
		case 4:
		case 8:
			m_cBMBInfoRecord4 = cBuff+6;
			break;
		}
		
		//send to the display now if we can
		if (m_pPropertySheet)
			m_pPropertySheet->OnBMBInfoRecord(cBuff+6);
	}

	WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);

	WriteToSOHFileX((AcquireRecordX*)psBMBInfoRecord);

	//send it to dump file if required
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, &cBuff[6], bStale);

	PostMessage(IDC_RECEIVE_RESULT,BMBINFORECORD_OPCODE,CHECKSUM_GOOD);

	return sizeof(BMBInfoRecord	);
