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
//CJulianTime Class Header File

//DR 243: Correct processing in CJulian of year < 1982
//8 Mar 2006 pjm (most of changes are by dgp, pjm re-inserted MIC 1.907 changes)


#if !defined (CJULIANTIME)
#define CJULIANTIME

#define BASE_YEAR			1952
#define BASE_MONTH			1
#define BASE_DAY			1
#define BASE_HOUR			0
#define BASE_MINUTE			0
#define BASE_SECOND			0
#define HALF_MILLISECOND	(.5 / ( 1000. * 60. * 60. * 24.))
#define SECONDS_PER_YEAR	86400L
#define HALF_SECOND			5.7870370370370370e-6

class CJulianTime
{
public:
	CJulianTime(char *);
	CJulianTime(CString);   //MIC_CB_028 to MIC 1.907 PJM 1/20/05
	CJulianTime(unsigned int uiTime);
	CJulianTime(double dTime);
	CJulianTime();
	void Init(char*);
	void Init(unsigned Input);
	void Init(double Input);
	void FinishInit();
	int Year();
	int Month();
	int Day();
	int Hour();
	int Minute();
	int Second();
	unsigned int TotalSeconds();
	char *Date();
	char *Time();
	bool IsValid();
	~CJulianTime();

	char	m_pDate[11];
	char	m_pTime[14];

protected:

	char	m_pCharacter[32];
	double	m_dDouble;
	bool	m_bIsValid;
	int		m_Year;
	int		m_Month;
	int		m_Day;
	int		m_Hour;
	int		m_Minute;
	int		m_Second;
	int		m_MSecond;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif