/*
void CEOSSDataStat::DisplayString(char* szFinal,char* szItem)
{
	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szBuff[MAX_PATH];
	unsigned long ulBuffSize;
	CString cString(szItem);
	cString += "...\r\n";
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szFinal,
		0,KEY_QUERY_VALUE |KEY_ENUMERATE_SUB_KEYS, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		ulBuffSize = sizeof(szBuff);
		lRet = RegQueryValueExA(hKey,szItem,NULL,
			&dwType,(unsigned char *)szBuff,&ulBuffSize);
		if (lRet == 0)
			cString += szBuff;
		else
			cString += "ERROR: Cannot read from registry";
		SetDlgItemText(IDC_EDIT1,cString);
		RegCloseKey(hKey);
	}
}

void CEOSSDataStat::DisplayLatestEosses(char* szFinal,char*szItem)
{
	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szTemp[16];
	unsigned long ulBuffSize;
	CString cString("LATEST EOSSes...\r\n");
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szFinal,
		0,KEY_QUERY_VALUE |KEY_ENUMERATE_SUB_KEYS, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		ulBuffSize = sizeof(EOSSes);
		lRet = RegQueryValueExA(hKey,szItem,NULL,
			&dwType,(unsigned char *)EOSSes,&ulBuffSize);
		if (lRet == 0)
		{
			for (int i = 0; i < 32; i++)
			{
				if (EOSSes[i])
				{
					cString += itoa(EOSSes[i],szTemp,10);
					cString += "\r\n";
				}
			}
			SetDlgItemText(IDC_EDIT1,cString);
		}
		else
			SetDlgItemText(IDC_EDIT1,"ERROR: Cannot read from registry");
		RegCloseKey(hKey);
	}
}

void CEOSSDataStat::DisplayStatus(char* szFinal,char* szItem)
{
	STATUS sStatusBuffer;
	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szTemp[64];
	unsigned long ulBuffSize;
	CString cString;
	SetDlgItemText(IDC_EDIT1,szItem);
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szFinal,
		0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		ulBuffSize = sizeof(sStatusBuffer);
		lRet = RegQueryValueExA(hKey,szItem,NULL,
			&dwType,(BYTE*)&sStatusBuffer,&ulBuffSize);
		if (lRet == 0)
		{
			cString += "STATUS...\r\n\r\n";
			sprintf(szTemp,"Address:\t\t%05u\r\n",sStatusBuffer.Seal);
			cString += szTemp;

			SYSTEMTIME sSystemTime;
			EossToSystemTime(&sSystemTime,&sStatusBuffer.Time);
			sprintf(szTemp,"Time:\t\t%04d\\%02d\\%02d %02d:%02d:%02d\r\n",
				sSystemTime.wYear,sSystemTime.wMonth,sSystemTime.wDay,
				sSystemTime.wHour,sSystemTime.wMinute,sSystemTime.wSecond);
			cString += szTemp;

			UnMakeID(sStatusBuffer.Operator1, szTemp);
			cString += "Oper. 1\t\t";
			cString += szTemp;
			UnMakeID(sStatusBuffer.Operator2, szTemp);
			cString += "\r\nOper. 2\t\t";
			cString += szTemp;
			cString += "\r\n";

			sprintf(szTemp,"Seal wire\t\t%s\r\n",(sStatusBuffer.Flags & FLAG_FOOPEN)?"open":"closed");
			cString += szTemp;

			sprintf(szTemp,"Seal wire\t\t%sfailed\r\n",(sStatusBuffer.Flags & FLAG_FOFAIL)?"":"not ");
			cString += szTemp;

			sprintf(szTemp,"Drill protection\t%s\r\n",(sStatusBuffer.Flags & FLAG_SHORT)?"short":(sStatusBuffer.Flags & FLAG_BROKEN)?"broken":"safe");
			cString += szTemp;

			cString += "\r\nMain battery\r\n";

			sprintf(szTemp," - Compartment\t%s\r\n",(sStatusBuffer.Flags & FLAG_HOPEN)?"open":"closed");
			cString += szTemp;

			sprintf(szTemp," - Voltage\t\t%3.1f V\r\n",((float)sStatusBuffer.MainBattery)/10.0);
			cString += szTemp;

			sprintf(szTemp," - Power consumed\t%d of 2000 mAh\r\n",
				sStatusBuffer.BatteryConsumed/1000);
			cString += szTemp;

			strncpy(szTemp,sStatusBuffer.BatteryType,8);
			szTemp[8] = NULL;
			cString += " - Type\t\t";
			cString += szTemp;
			cString += "\r\n";

			sprintf(szTemp,"%02x%02x %02x%02x %02x%02x %02x%02x\r\n",
				sStatusBuffer.BatID[0],	sStatusBuffer.BatID[1],
				sStatusBuffer.BatID[2],	sStatusBuffer.BatID[3],
				sStatusBuffer.BatID[4],	sStatusBuffer.BatID[5],
				sStatusBuffer.BatID[6],	sStatusBuffer.BatID[7]);
			cString += " - ID\t\t";
			cString += szTemp;

			sprintf(szTemp,"%04d",sStatusBuffer.BatteryMfg);
			cString += " - Manufactured\t20";
			cString += szTemp[2];
			cString += szTemp[3];
			cString += "-";
			cString += szTemp[0];
			cString += szTemp[1];
			cString += "\r\n";

			EOSSTIME sEOSStime;
			sEOSStime.HalfDays = sStatusBuffer.BatteryInstalled;
			sEOSStime.Seconds = 0;
			SYSTEMTIME ssSystemTime;
			EossToSystemTime(&ssSystemTime, &sEOSStime);

			sprintf(szTemp," - Installed\t%d-%02d-%02d\r\n",
				ssSystemTime.wYear,ssSystemTime.wMonth,ssSystemTime.wDay);
			cString += szTemp;

			cString += "\r\nEmergency battery\r\n";

			sprintf(szTemp," - Voltage\t\t%3.1f V\r\n",((float)sStatusBuffer.EmergencyBattery)/10.0);
			cString += szTemp;

			sprintf(szTemp," - Power consumed\t%d of 170 mAh\r\n",
				sStatusBuffer.EBatteryConsumed/1000);
			cString += szTemp;

			sprintf(szTemp,"%04d",sStatusBuffer.EBatteryMfg);
			cString += " - Manufactured\t20";
			cString += szTemp[2];
			cString += szTemp[3];
			cString += "-";
			cString += szTemp[0];
			cString += szTemp[1];
			cString += "\r\n";

			sEOSStime.HalfDays = sStatusBuffer.EBatteryInstalled;
			sEOSStime.Seconds = 0;
			EossToSystemTime(&ssSystemTime, &sEOSStime);

			sprintf(szTemp," - Installed\t%d-%02d-%02d\r\n",
				ssSystemTime.wYear,ssSystemTime.wMonth,ssSystemTime.wDay);
			cString += szTemp;

			sprintf(szTemp,"\r\nTemperature\t%d °C\r\n",sStatusBuffer.Temperature);
			cString += szTemp;

			cString += "\r\nVACOSS emulation\r\n";
			sprintf(szTemp," - Enabled\t%s\r\n",sStatusBuffer.VacossEnabled?"yes":"no");
			cString += szTemp;

			sprintf(szTemp," - Attachements\t%lu\r\n",sStatusBuffer.VacossAttachments);
			cString += szTemp;

			cString += "\r\nFlash memory\r\n";

			sprintf(szTemp," - Warnings\t%d\r\n",sStatusBuffer.FlashWarnings);
			cString += szTemp;

			sprintf(szTemp," - Errors\t\t%d\r\n",sStatusBuffer.FlashErrors);
			cString += szTemp;

			cString += "\r\nMajority-vote memory\r\n";

			sprintf(szTemp," - Warnings\t%d\r\n",sStatusBuffer.MajoWarnings);
			cString += szTemp;

			sprintf(szTemp," - Errors\t\t%d\r\n",sStatusBuffer.MajoErrors);
			cString += szTemp;

			sprintf(szTemp,"\r\nEPROM CRC\t0x%04X\r\n",sStatusBuffer.EPROMCrc16);
			cString += szTemp;
			SetDlgItemText(IDC_EDIT1,cString);
		}
		else
			SetDlgItemText(IDC_EDIT1,"ERROR: Cannot read from registry");
		RegCloseKey(hKey);
	}

}

void CEOSSDataStat::DisplaySOHLog(char* szFinal,char* szItem)
{
	SOHLOGPAGE sLogPage;
	LOGINFO sLogInfo;
	unsigned int uiPageIndex;
	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szTemp[64];
	char szPageName[16];
	unsigned long ulBuffSize;
	int iSoHRecords;
	iSoHRecords = INT_MAX;
	int iSoHCount = 0;
	CString cString;
	CString cStringHeaders;
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szFinal,
		0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{

		ulBuffSize = sizeof(LOGINFO);
		lRet = RegQueryValueExA(hKey,"LOGINFO",NULL,
			&dwType,(BYTE*)&sLogInfo,&ulBuffSize);
		if (lRet == 0)
			iSoHRecords = sLogInfo.SoHRecords;	
		
		bool bDone = false;
		uiPageIndex = 1;
		while (!bDone)
		{
	
			sprintf(szPageName,"%s_%03u",szItem,uiPageIndex++);
			ulBuffSize = sizeof(SOHLOGPAGE);
			lRet = RegQueryValueExA(hKey,szPageName,NULL,
				&dwType,(BYTE*)&sLogPage,&ulBuffSize);
			if (lRet == 0)
			{
				if (memcmp(&sLogPage,"FAILED READ",11) == 0)
				{
					sprintf(szTemp,"FAILED READ\tPage Number %d\r\n",uiPageIndex-1);
					cString += szTemp;
					cStringHeaders = szTemp;
				}
				else
				{

					sprintf(szTemp,"\r\nPage Header\t%s\r\n",szPageName);
					cStringHeaders += szTemp;
					cString += szTemp;
					
					sprintf(szTemp," - Seal\t\t%u\r\n",sLogPage.Header.Seal);
					cStringHeaders += szTemp;
					cString += szTemp;
					
					sprintf(szTemp," - Log\t\t%u\r\n",sLogPage.Header.Log);
					cStringHeaders += szTemp;
					cString += szTemp;
					
					sprintf(szTemp," - Page\t\t%u\r\n",sLogPage.Header.Page);
					cStringHeaders += szTemp;
					cString += szTemp;
					
					SYSTEMTIME sTime;
					EossToSystemTime(&sTime, &sLogPage.Header.Time);
					sprintf(szTemp," - Time\t\t%u\\%02u\\%02u %02u:%02u:%02u\r\n",
						sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
					cStringHeaders += szTemp;
					cString += szTemp;

					sprintf(szTemp," - Authenticator\t0x%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
						sLogPage.Header.Authenticator[0],
						sLogPage.Header.Authenticator[1],
						sLogPage.Header.Authenticator[2],
						sLogPage.Header.Authenticator[3],
						sLogPage.Header.Authenticator[4],
						sLogPage.Header.Authenticator[5],
						sLogPage.Header.Authenticator[6],
						sLogPage.Header.Authenticator[7]);
						cStringHeaders += szTemp;
						cString += szTemp;
				}

				
				for (int i = 0; i < SOHRECORDS_PER_PAGE; i++)
				{
					if (iSoHRecords >= 0)
					{
							SYSTEMTIME sTime;
							EOSSTIME eTime;
							eTime.HalfDays = sLogPage.Entries[i].Date;
							eTime.Seconds = 0;
							EossToSystemTime(&sTime, &eTime);
							sprintf(szTemp," - %02d\t%04u\\%02u\\%02u   %3d° %3d°   %3.1fv %3.1fv   %4u %3u\r\n",++iSoHCount,
								sTime.wYear,sTime.wMonth,sTime.wDay,
								sLogPage.Entries[i].MaxTemperature,
								sLogPage.Entries[i].MinTemperature,
								(float)sLogPage.Entries[i].EmergencyBattery/10.0,
								(float)sLogPage.Entries[i].MainBattery/10.0,
								sLogPage.Entries[i].OnBatteries*12,
								sLogPage.Entries[i].MVEvents);
//							cString = szTemp + cString;
							cString += szTemp;
					}
					iSoHRecords--;
				}
//				cString = "\r\n" + cString;
			}
			else
				bDone = true;
		}
		cString = "SOH LOG...\r\n\r\nRecords\tDate, Max °C, Min °C, Emerg. Bat. V.,\r\n\tMain Bat. V., On Bat. Min., MV Events\r\n" + cString;

		cString += "\r\n";
//		cString += cStringHeaders;
		RegCloseKey(hKey);
	}
	else
		cString += "ERROR: Cannot read from registry";
	SetDlgItemText(IDC_EDIT1,cString);
}

void CEOSSDataStat::DisplaySEALLog(char* szFinal,char* szItem)
{
	SEALLOGPAGE sLogPage;
	LOGINFO sLogInfo;
	unsigned int uiPageIndex;
	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szTemp[64];
	char szPageName[16];
	unsigned long ulBuffSize;
	int iSealEvents;
	iSealEvents = INT_MAX;
	CString cString;
	CString cStringHeader;
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szFinal,
		0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{

		ulBuffSize = sizeof(LOGINFO);
		lRet = RegQueryValueExA(hKey,"LOGINFO",NULL,
			&dwType,(BYTE*)&sLogInfo,&ulBuffSize);
		if (lRet == 0)
			iSealEvents = sLogInfo.SealEvents;	

		bool bDone = false;
		uiPageIndex = 1;
		while (!bDone)
		{
			sprintf(szPageName,"%s_%03u",szItem,uiPageIndex++);
			ulBuffSize = sizeof(SEALLOGPAGE);
			lRet = RegQueryValueExA(hKey,szPageName,NULL,
				&dwType,(BYTE*)&sLogPage,&ulBuffSize);
			if (lRet == 0)
			{
				if (memcmp(&sLogPage,"FAILED READ",11) == 0)
				{
					sprintf(szTemp,"FAILED READ\tPage Number %d\r\n",uiPageIndex-1);
					cString += szTemp;
					cStringHeader += szTemp;
				}
				else
				{

					sprintf(szTemp,"Page Header\t%s\r\n",szPageName);
					cStringHeader += szTemp;
					
					sprintf(szTemp," - Seal\t\t%u\r\n",sLogPage.Header.Seal);
					cStringHeader += szTemp;
					
					sprintf(szTemp," - Log\t\t%u\r\n",sLogPage.Header.Log);
					cStringHeader += szTemp;
					
					sprintf(szTemp," - Page\t\t%u\r\n",sLogPage.Header.Page);
					cStringHeader += szTemp;
					
					SYSTEMTIME sTime;
					EossToSystemTime(&sTime, &sLogPage.Header.Time);
					sprintf(szTemp," - Time\t\t%u\\%02u\\%02u %02u:%02u:%02u\r\n",
						sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
					cStringHeader += szTemp;

					sprintf(szTemp," - Authenticator\t0x%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
						sLogPage.Header.Authenticator[0],
						sLogPage.Header.Authenticator[1],
						sLogPage.Header.Authenticator[2],
						sLogPage.Header.Authenticator[3],
						sLogPage.Header.Authenticator[4],
						sLogPage.Header.Authenticator[5],
						sLogPage.Header.Authenticator[6],
						sLogPage.Header.Authenticator[7]);
					cStringHeader += szTemp;

					for (int i = 0; i < SEALEVENTS_PER_PAGE; i++)
					{
						if (iSealEvents > 0)
						{
							if (iSealEvents != (signed)sLogInfo.SealEvents)
							{
								SYSTEMTIME sTime;
								EossToSystemTime(&sTime, &sLogPage.Entries[i]);
								sprintf(szTemp," - %02d %s\t%04u\\%02u\\%02u %02u:%02u:%02u\r\n",i,(i&1)?"Closed":"Opened",
									sTime.wYear,sTime.wMonth,sTime.wDay,
									sTime.wHour,sTime.wMinute,sTime.wSecond);
								cString = szTemp + cString;
							}
						}
						iSealEvents--;
					}
				}
				cString = "SEAL LOG...\r\n\r\nEvents\r\n" + cString;
				cString += "\r\n";
//				cString += cStringHeader;

			}
			else
				bDone = true;
		}
		RegCloseKey(hKey);
	}
	else
		cString += "ERROR: Cannot read from registry";
	SetDlgItemText(IDC_EDIT1,cString);
}

void CEOSSDataStat::DisplayLOGInfo(char* szFinal,char* szItem)
{
	LOGINFO sLogInfo;
	unsigned int uiKeyNumber;
	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szTemp[64];
	unsigned long ulBuffSize;
	CString cString;
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szFinal,
		0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		ulBuffSize = sizeof(LOGINFO);
		lRet = RegQueryValueExA(hKey,szItem,NULL,
			&dwType,(BYTE*)&sLogInfo,&ulBuffSize);
		if (lRet == 0)
		{
			cString += "LOG INFO...\r\n";

			sprintf(szTemp,"\r\nTotal PAD Pages\t%d\r\n",sLogInfo.TotalPadPages);
			cString += szTemp;
			sprintf(szTemp,"Ext. Status Flags\t0x%02x\r\n",sLogInfo.ExStatusFlags);
			cString += szTemp;

			uiKeyNumber = sLogInfo.KeysetNumber;
			uiKeyNumber &= 0x00ffffff;
			sprintf(szTemp,"Key Number\t%u-%u\r\n",uiKeyNumber>>8,sLogInfo.KeysetNumber & 0xff000000>>24);
			cString += szTemp;

			SYSTEMTIME sysTime;
			EossToSystemTime(&sysTime, &sLogInfo.KeysetTime);
			sprintf(szTemp,"Key Time\t\t%04u\\%02u\\%02u %02u:%02u:%02u\r\n",
				sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
			cString += szTemp;			


			cString += "\r\nSeal Events Log\r\n";

			sprintf(szTemp," - Events\t\t%d\r\n",sLogInfo.SealEvents);
			cString += szTemp;

			sprintf(szTemp," - First Page\t%d\r\n",sLogInfo.SealLogFirstPage);
			cString += szTemp;

			sprintf(szTemp," - Last Page\t%d\r\n",sLogInfo.SealLogLastPage);
			cString += szTemp;

			sprintf(szTemp," - Start Page\t%d\r\n",sLogInfo.SealLogStartPage);
			cString += szTemp;

			sprintf(szTemp," - Current Page\t%d\r\n",sLogInfo.SealLogCurrentPage);
			cString += szTemp;

			cString += "\r\nSystem Events Log\r\n";

			sprintf(szTemp," - Events\t\t%d\r\n",sLogInfo.SystemEvents);
			cString += szTemp;

			sprintf(szTemp," - First Page\t%d\r\n",sLogInfo.SystemLogFirstPage);
			cString += szTemp;

			sprintf(szTemp," - Last Page\t%d\r\n",sLogInfo.SystemLogLastPage);
			cString += szTemp;

			sprintf(szTemp," - Start Page\t%d\r\n",sLogInfo.SystemLogStartPage);
			cString += szTemp;

			sprintf(szTemp," - Current Page\t%d\r\n",sLogInfo.SystemLogCurrentPage);
			cString += szTemp;

			cString += "\r\nSOH Log\r\n";

			sprintf(szTemp," - SoH Records\t%d\r\n",sLogInfo.SoHRecords);
			cString += szTemp;

			sprintf(szTemp," - First Page\t%d\r\n",sLogInfo.SoHLogFirstPage);
			cString += szTemp;

			sprintf(szTemp," - Last Page\t%d\r\n",sLogInfo.SoHLogLastPage);
			cString += szTemp;

			sprintf(szTemp," - Start Page\t%d\r\n",sLogInfo.SoHLogStartPage);
			cString += szTemp;

			sprintf(szTemp," - Current Page\t%d\r\n",sLogInfo.SoHLogCurrentPage);
			cString += szTemp;

		}
		else
			cString += "ERROR: Cannot read from registry";
			
		RegCloseKey(hKey);
	}
	else
		cString += "ERROR: Cannot read from registry";
	SetDlgItemText(IDC_EDIT1,cString);
}

void CEOSSDataStat::DisplaySystemLog(char* szFinal,char* szItem)
{
	SYSTEMLOGPAGE sLogPage;
	LOGINFO sLogInfo;
	unsigned int uiPageIndex;
	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szTemp[128];
	char szPageName[16];
	unsigned long ulBuffSize;
	int iSystemEvents;
	iSystemEvents = INT_MAX;
	CString cString;
	CString cStringHeaders("Page Headers\r\n");
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szFinal,
		0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		ulBuffSize = sizeof(LOGINFO);
		lRet = RegQueryValueExA(hKey,"LOGINFO",NULL,
			&dwType,(BYTE*)&sLogInfo,&ulBuffSize);
		if (lRet == 0)
			iSystemEvents = sLogInfo.SystemEvents;
		int iEventCount = 0;
		bool bDone = false;
		uiPageIndex = 1;
		while (!bDone)
		{
			sprintf(szPageName,"%s_%03u",szItem,uiPageIndex++);
			ulBuffSize = sizeof(SYSTEMLOGPAGE);
			lRet = RegQueryValueExA(hKey,szPageName,NULL,
				&dwType,(BYTE*)&sLogPage,&ulBuffSize);
			if (lRet == 0)
			{
				if (memcmp(&sLogPage,"FAILED READ",11) == 0)
				{
					sprintf(szTemp,"FAILED READ\tPage Number %d\r\n",uiPageIndex-1);
					cString += szTemp;
					cStringHeaders += szTemp;
				}
				else
				{
			
					sprintf(szTemp,"Registry Entry\t%s\r\n",szPageName);
					cStringHeaders += szTemp;
					
					sprintf(szTemp," - Seal\t\t%u\r\n",sLogPage.Header.Seal);
					cStringHeaders += szTemp;
					
					sprintf(szTemp," - Log\t\t%u\r\n",sLogPage.Header.Log);
					cStringHeaders += szTemp;
					
					sprintf(szTemp," - Page\t\t%u\r\n",sLogPage.Header.Page);
					cStringHeaders += szTemp;
					
					SYSTEMTIME sTime;
					EossToSystemTime(&sTime, &sLogPage.Header.Time);
					sprintf(szTemp," - Time\t\t%u\\%02u\\%02u %02u:%02u:%02u\r\n",
						sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
					cStringHeaders += szTemp;

					sprintf(szTemp," - Authenticator\t0x%02X%02X%02X%02X%02X%02X%02X%02X\r\n\r\n",
						sLogPage.Header.Authenticator[0],
						sLogPage.Header.Authenticator[1],
						sLogPage.Header.Authenticator[2],
						sLogPage.Header.Authenticator[3],
						sLogPage.Header.Authenticator[4],
						sLogPage.Header.Authenticator[5],
						sLogPage.Header.Authenticator[6],
						sLogPage.Header.Authenticator[7]);
					cStringHeaders += szTemp;

					for (int i = 0; i < SYSTEMEVENTS_PER_PAGE; i++)
					{
						if (iSystemEvents > 0)
						{
							SYSTEMTIME sTime;
							EossToSystemTime(&sTime, &sLogPage.Entries[i].Time);
							sprintf(szTemp," - %03d  %04u\\%02u\\%02u %02u:%02u:%02u  %s\r\n",
								++iEventCount,
								sTime.wYear,sTime.wMonth,sTime.wDay,
								sTime.wHour,sTime.wMinute,sTime.wSecond,
								GetEossMessage(sLogPage.Entries[i].Code));
							cString = szTemp + cString;
						}
						iSystemEvents--;
					}
				}
			}
			else
				bDone = true;
		}
		cString = "SYSTEM LOG...\r\n\r\nEvents\r\n" + cString;
		cString += "\r\n";
//		cString += cStringHeaders;
		RegCloseKey(hKey);
	}
	else
		cString += "ERROR: Cannot read from registry";
	SetDlgItemText(IDC_EDIT1,cString);
}

CString CEOSSDataStat::GetEossMessage(unsigned short usCode)
{
	CString cString;
	char szTemp[64];
	if (usCode > 0 && usCode < 19683)
	{
//		char szID[4];
		UnMakeID(usCode,szTemp);
		cString = szTemp;
		cString += " logged on as operator 1";
	}
	else if (usCode > 20480 && usCode < 40163)
	{
//		char szID[4];
		UnMakeID(usCode - 20480,szTemp);
		cString = szTemp;
		cString += " logged on as operator 2";
	}
	else if (usCode == 40960)
		cString = "Operator(s) logged off.";
	else if (usCode == 40976) cString = "Date and time set.";
	else if (usCode == 40977) cString = "Date and time set.";
	else if (usCode == 40992) cString = "Battery compartment opened";
	else if (usCode == 40993) cString = "Battery compartment closed";
	else if (usCode == 41008) cString = "Seal tampered with. Key-set erased";
	else if (usCode == 41024) cString = "Access denied. Authorization failed";
	else if (usCode == 41040) cString = "New key-set generated";
	else if (usCode == 41056) cString = "Reset";
	else if (usCode == 41072) cString = "Key-set erased on operator request";
	else if (usCode == 41073) cString = "Temperator out of range. Key-set erased";
	else if (usCode == 41088) cString = "Entering deep sleep mode";
	if (Unattended(usCode)) cString += " WHILE NOT LOGGED IN";
	return cString;
}

void CEOSSDataStat::DisplayVersion(char* szFinal,char* szItem)
{	
	char cVersion[2];
	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szTemp[64];
	unsigned long ulBuffSize;
	CString cString;
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szFinal,
		0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		ulBuffSize = sizeof(cVersion);
		lRet = RegQueryValueExA(hKey,szItem,NULL,
			&dwType,(BYTE*)cVersion,&ulBuffSize);
		if (lRet == 0)
		{
			cString += "VERSION...\r\n";
			sprintf(szTemp," - Major\t\t%u\r\n - Minor\t\t%u\r\n",cVersion[0],cVersion[1]);
			cString += szTemp;

			SetDlgItemText(IDC_EDIT1,cString);
		}
		else
			SetDlgItemText(IDC_EDIT1,"ERROR: Cannot read from registry");
		RegCloseKey(hKey);
	}
}

void CEOSSDataStat::DisplayKeyInfo(char* szFinal,char* szItem)
{

	BYTE bKeyInfo[4];
	unsigned int uiKeyTime;
	unsigned int uiKeyNumber;
	LONG lRet;
	HKEY hKey;
	DWORD dwType;
	char szTemp[64];
	unsigned long ulBuffSize;
	CString cString;
	SetDlgItemText(IDC_EDIT1,szItem);
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		szFinal,
		0,KEY_QUERY_VALUE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		ulBuffSize = sizeof(bKeyNumber);
		lRet = RegQueryValueExA(hKey,"KEY NUMBER",NULL,
			&dwType,bKeyNumber,&ulBuffSize);
		if (lRet == 0)
		{
			cString += "KEY...\r\n";
			memcpy(&uiKeyNumber,bKeyNumber,sizeof(bKeyNumber));
			uiKeyNumber &= 0x00ffffff;
			sprintf(szTemp," - Number\t\t%u-%u\r\n",uiKeyNumber>>8,bKeyNumber[0]);
			cString += szTemp;
			ulBuffSize = sizeof(uiKeyTime);
			lRet = RegQueryValueExA(hKey,"KEY TIME",NULL,
				&dwType,(BYTE*)&uiKeyTime,&ulBuffSize);
			if (lRet == ERROR_SUCCESS)
			{
				COleDateTimeSpan cSpan(0,0,0,uiKeyTime);
				COleDateTime cTime(2000,1,1,0,0,0);
				cTime += cSpan;
				cString += cTime.Format(" - Date\t\t%Y\\%m\\%d %H:%M:%S\r\n");;
			}
			else
				cString += "ERROR: Cannot read from registry";

		}
		else
			cString += "ERROR: Cannot read from registry";

		RegCloseKey(hKey);
	}
	else
		cString += "ERROR: Cannot read from registry";

	SetDlgItemText(IDC_EDIT1,cString);
}


//-----------------------------------------------------------------------------
//
// get up to 3 captital letters from 16 bits
// (there's no error checking here...)
// 
void CEOSSDataStat::UnMakeID(unsigned short Id, char * szOperator)
{
	  szOperator[0] = NULL;
	  unsigned char Digit;
	  CString szId;

      while (Id) {
         Digit = Id % 27;
         if (Digit)
            szId = (char)(Digit + 64) + szId;
         else
            szId = " " + szId;
         Id = Id / 27;
      }
      strcpy(szOperator, szId);
}

bool CEOSSDataStat::Unattended(unsigned short usCode)
{
	bool bLoggedOff = true;
	if ((usCode > 0) && (usCode < 19683))
		bLoggedOff = false;
	else if ((usCode > 20480) && (usCode < 40163))
		bLoggedOff = false;
	else if (usCode == 40960)
		bLoggedOff = true;
	return (bLoggedOff && (usCode != 40960) && (usCode != 41024));
}


void CEOSSDataStat::EossToSystemTime(SYSTEMTIME * pSystemTime, EOSSTIME * pEossTime)
{
	SYSTEMTIME SystemTime;
	
	SystemTime.wYear = 2000;
	SystemTime.wMonth = 1;
	SystemTime.wDay = 1;
	SystemTime.wHour = 0;
	SystemTime.wMinute = 0;
	SystemTime.wSecond = 0;
	SystemTime.wMilliseconds = 0;

	ULONGLONG FileTime;
	
	SystemTimeToFileTime(&SystemTime, (LPFILETIME)&FileTime);

	ULONGLONG EossTime = pEossTime->HalfDays * 43200 + (43200 - pEossTime->Seconds);

	FileTime += (EossTime * 10000000);
	
	FileTimeToSystemTime((LPFILETIME)&FileTime, pSystemTime);
}
*/