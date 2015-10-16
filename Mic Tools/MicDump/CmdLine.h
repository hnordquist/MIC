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
