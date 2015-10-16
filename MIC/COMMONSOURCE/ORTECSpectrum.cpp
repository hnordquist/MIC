// ORTECSpectrum.cpp

#include "stdafx.h"
#include "ORTECSpectrum.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CORTECSpectrum::CORTECSpectrum(
		long maxchannels,
		long StartChan, 
		long LiveTime, 
		long TrueTime):

	CChnSpc(maxchannels,(long *)m_rawdata,StartChan,LiveTime,TrueTime)
{
//	TRACE("CORTECSpectrum::CTOR() pid:%u tid:0x%X\n", _getpid(),GetCurrentThreadId());

	m_channelsabovethreshold = 0;
	m_maximumvalue = 0;
	m_threshold = 0;
	m_nonzerochannels = 0;
	memset(m_rawdata,0, sizeof(m_rawdata));
}

CORTECSpectrum::~CORTECSpectrum()
{
//	TRACE("CORTECSpectrum::DTOR() pid:%u tid:0x%X\n", _getpid(),GetCurrentThreadId());
}

DWELL_TYPE CORTECSpectrum::DwellType(){return m_edwelltype;}
void CORTECSpectrum::SetDwellType(DWELL_TYPE value){m_edwelltype = value;}
UINT CORTECSpectrum::ChannelsAboveThreshold(){return m_channelsabovethreshold;}
UINT CORTECSpectrum::NonZeroChannels(){return m_nonzerochannels;}
UINT CORTECSpectrum::MaximumValue(){return m_maximumvalue;}
UINT *CORTECSpectrum::RawData(){return m_rawdata;}
UINT CORTECSpectrum::Threshold(){return m_threshold;}
void CORTECSpectrum::SetThreshold (UINT value){m_threshold = value;}

void CORTECSpectrum::EvaluateRawData()
{
	//TRACE("CORTECSpectrum::EvaluateRawData()\n");
	m_channelsabovethreshold = 0;
	m_maximumvalue = 0;
	m_nonzerochannels = 0;

	for (int i = 0; i < NumChans(); i++)
	{
		if (m_rawdata[i] > 0)
			m_nonzerochannels++;

		if (m_rawdata[i] > m_threshold)
			m_channelsabovethreshold++;

		if (m_rawdata[i] > m_maximumvalue)
			m_maximumvalue = m_rawdata[i];
	}
}