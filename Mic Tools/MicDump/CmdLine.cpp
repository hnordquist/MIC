/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-040. This software was exported from the United States in accordance with the Export
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