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
#pragma once

// <file> [-g [#]] [-o]
// <BI0 file> [-g [#]] [-o] [-bid <bid folder>] [-trim]
// <folder> -s BI0 [-g [#]] [-o] [-bid <bid folder>] [-trim]
// <folder> -s [MGD,CHN,GRAND,MCA,ISR,JSR,PSR,HMR,VACOSS,BINARY,GPS,SOH,ESS,RAD,RFI] [-g [#]] [-o]

// todo: if <folder> -s is required
class TransformCmdLine : public CCommandLineInfo
{
public:
	TransformCmdLine() : counter(0), dir(true), bi0(false), 
		gap(false), interval(2),
		output(false), trim(false), bid(false), pattern(false), 
		ginext(false), ssnext(false), bfnext(false), help(false), direct(true),
		CCommandLineInfo() {memset(suffix,0,32);memset(bidfullpath,0,_MAX_PATH+2);memset(fullpath,0,_MAX_PATH+2);}

	TCHAR suffix[32], fullpath[_MAX_PATH+2], bidfullpath[_MAX_PATH+2];
	bool dir, bi0, gap, output, trim, bid, pattern;
	int interval, counter;
	bool ginext, ssnext, bfnext, help, direct;

	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	CString ToString();

};
