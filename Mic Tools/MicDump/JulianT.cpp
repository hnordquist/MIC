// C Julian 
#include "stdafx.h"
#include "JulianT.h"
#include <afxdisp.h>

CJulianTime::CJulianTime()
{
}

void
CJulianTime::Init(double Input)
{
	m_dDouble = Input;
	FinishInit();
}

void
CJulianTime::Init(unsigned Input)
{
	m_dDouble = (double)Input;
	FinishInit();
}

void
CJulianTime::Init(char*Input)
{
	sscanf(Input,"%lf",&m_dDouble);
	FinishInit();
}

void
CJulianTime::FinishInit()
{
	COleDateTimeSpan cSpan(m_dDouble/SECONDS_PER_YEAR);
	COleDateTime cTime(BASE_YEAR,BASE_MONTH,BASE_DAY,
		BASE_HOUR,BASE_MINUTE,BASE_SECOND);
	cTime = cTime + cSpan;
	cTime -= HALF_MILLISECOND;

	m_bIsValid = cTime.GetStatus() == COleDateTime::valid;
	sprintf(m_pDate,"%04i.%02i.%02i",
		cTime.GetYear(),cTime.GetMonth(),cTime.GetDay());
	sprintf(m_pTime,"%02i:%02i:%02i",
		cTime.GetHour(),cTime.GetMinute(),cTime.GetSecond());
	m_Year = cTime.GetYear();
	m_Month = cTime.GetMonth();
	m_Day = cTime.GetDay();
	m_Hour = cTime.GetHour();
	m_Minute = cTime.GetMinute();
	m_Second = cTime.GetSecond();
}

CJulianTime::CJulianTime(char* Input)
{
	Init(Input);
}

CJulianTime::~CJulianTime()
{
}

unsigned int
CJulianTime::TotalSeconds()
{
	return (unsigned int)(m_dDouble);
}

int
CJulianTime::Year()
{
	return m_Year;
}

int
CJulianTime::Month()
{
	return m_Month;
}

int
CJulianTime::Day()
{
	return m_Day;
}

int
CJulianTime::Hour()
{
	return m_Hour;
}

int
CJulianTime::Minute()
{
	return m_Minute;
}

int 
CJulianTime::Second()
{
	return m_Second;
}

char *
CJulianTime::Date()
{
	return m_pDate;
}

char *
CJulianTime::Time()
{
	return m_pTime;
}

bool
CJulianTime::IsValid()
{
	return m_bIsValid;
}