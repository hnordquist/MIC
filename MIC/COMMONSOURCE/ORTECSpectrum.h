// ORTECSpectrum.h 

#ifndef ORTECSPECTRUM_H
#define ORTECSPECTRUM_H

#include "gamnew.h"

enum DWELL_TYPE
{
	UNKNOWN_DWELL_0   = 0,
	REALTIME_DWELL_1  = 1,
	SHORT_DWELL_2     = 2,
	LONG_DWELL_3      = 3,
	IMMEDIATE_DWELL_4 = 4
};

#define GET_LONG_DWELL_SPECTRUM      "GET_LONG_DWELL_SPECTRUM"
#define GET_SHORT_DWELL_SPECTRUM     "GET_SHORT_DWELL_SPECTRUM"
#define GET_IMMEDIATE_DWELL_SPECTRUM "GET_IMMEDIATE_DWELL_SPECTRUM"
#define GET_REALTIME_DWELL_SPECTRUM  "GET_REALTIME_DWELL_SPECTRUM"

class CORTECSpectrum : public CChnSpc
{
public:
	CORTECSpectrum(
		long max, 
		long StartChan = 0L, 
		long LiveTime=0L, 
		long TrueTime=0L);

	virtual ~CORTECSpectrum();

	DWELL_TYPE DwellType();
	void SetDwellType(DWELL_TYPE value);
	UINT ChannelsAboveThreshold();
	UINT NonZeroChannels();
	UINT MaximumValue();
	UINT *RawData();
	UINT Threshold();
	void SetThreshold (UINT value);
	void EvaluateRawData();

private:
	DWELL_TYPE m_edwelltype;
	UINT m_channelsabovethreshold;
	UINT m_nonzerochannels;
	UINT m_maximumvalue;
	UINT m_rawdata[CS_MAX_CHANNELS];
	UINT m_threshold;
};

#endif //ORTECSPECTRUM_H
