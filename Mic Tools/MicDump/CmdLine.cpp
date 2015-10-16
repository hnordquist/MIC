#include "stdafx.h"
#include <shlwapi.h>
#include "CmdLine.h"


void TransformCmdLine::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if (counter == 0) // single file name or file suffix placeholder
	{
		direct = false; // there is a command line!
		if (bFlag)
		{
			TCHAR c = tolower(pszParam[0]);
			switch (c) {
				case '?':
				case 'h':
					help = true;
					break;
			}
		}
		else
		{
			dir = PathIsDirectory(pszParam);		
			if (!dir)
			{
				// get suffix
				CString ext(PathFindExtension(pszParam));
				int bioi = ext.CompareNoCase(".BI0");
				OutputDebugString(("\"" + ext) + "\"\r\n");
				if (bioi == 0)  // got a single BI0
				{
					OutputDebugString(" BIO!\r\n");
					bi0 = true;
				}
			}
			strcpy(fullpath,pszParam);
		}
	}
	else if (counter > 0)
	{
		if (bFlag)
		{
			ssnext = ginext = bfnext = false;
			TCHAR c = tolower(pszParam[0]);
			switch (c) {
				case 'g':
					gap = true;
					ginext = true;
					break;
				case 'o':
					output = true;
					break;
				case 't':
					trim = true;
					break;
				case 'b':
					bid = true;
					bfnext = true;
					break;
				case 's':
					pattern = true;
					ssnext = true;
					break;
				case '?':
				case 'h':
					help = true;
					break;
			}
		}
		else
		{
			if (gap && ginext)
			{
				interval = atoi(pszParam);
				if (interval < 1)
					interval = 2;
				ginext = false;
			}
			else if (pattern && ssnext)
			{
				strncpy(suffix, pszParam, 3);
				ssnext = false;
				CString ext(suffix);
				int bioi = ext.CompareNoCase("BI0");
				if (bioi == 0)  // got a BI0 suffix
				{
					bi0 = true;
				}
			}
			else if (bid && bfnext)
			{
				strcpy(bidfullpath, pszParam);
				bfnext = false;
			}
		}
	}
	char buff[512];
	sprintf(buff, "%d %s %d %d\r\n",counter, pszParam, bFlag, bLast);
	OutputDebugString(buff);
	counter++;
}

CString TransformCmdLine::ToString()
	{
		CString s;
		if (help)
		{
			s.Append("MICDump 1.1.0.0. July 8, 2013 -- now supercharged with BI0 Dragonball miniGRAND file processing!\r\n");
			s.Append("  <file>  [-g [#]] [-o]\r\n");
			s.Append("  <BI0 file>  [-g [#]] [-o] [-bid <bid folder>] [-trim]\r\n");
			s.Append("  <folder> -s BI0  [-g [#]] [-o] [-bid <bid folder>] [-trim]\r\n");
			s.Append("  <folder> -s [MGD,CHN,GRAND,MCA,ISR,JSR,PSR,HMR,VACOSS,BINARY,GPS,SOH,ESS,RAD,RFI] [-g [#]] [-o]\r\n");
			s.Append("   -s <file suffix>   common suffix of files to process\r\n");
			s.Append("   -g [seconds]   flag intervals less than specified # of seconds (2 is default) in the output\r\n");
			s.Append("   -o    open output text file(s) after processing \r\n");
			s.Append("   -bid <folder>   generate legacy BID file from BI0 file, save BID file in <folder>\r\n");
			s.Append("   -trim    remove first 5 sub-second BI0 events, prevents Rad Review import error messages\r\n");
			return s;
		}

		s.Append(dir ? "dir  " : "file "); 
		s.Append(fullpath);
		s.Append("\r\n");
		if (dir)
		{
			TCHAR buff[32];
			if (suffix[0] == 0)
				sprintf(buff, "no file suffix (error)\r\n");
			else
				sprintf(buff, "file suffix: %s\r\n", suffix);
			s.Append(buff);
		}
		if (gap)
		{
			TCHAR buff[32];
			sprintf(buff, "gap analysis for %d sec\r\n", interval);
			s.Append(buff);
		}
		s.Append(output ? "open output file(s)\r\n" : ""); 

		if (bi0)
		{
			if (bid)
			{
				TCHAR buff[512];
				sprintf(buff, "generate bid file in %s\r\n", (bidfullpath[0] ? bidfullpath : "source folder"));
				s.Append(buff);
			}
			s.Append(trim ? "remove 5 initial records\r\n" : "");
		}

		return s;
	}