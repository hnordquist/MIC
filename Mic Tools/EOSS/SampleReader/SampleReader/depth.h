#pragma once

typedef enum {eExistence, eSealChallenge, eStatus, eStatusAndLog, eStatusAndLogAndLastReadDelta, eReadAllData, eQueryLevelMax} QueryLevel; 
const char* levelnames[eQueryLevelMax + 1] = 
	{"Existence", "SealChallenge", "SealStatus", "SealStatusAndLog", "SealStatusAndLogAndLastReadDelta", "ReadAllData", "QueryLevelMax"};


#define  READ_LOG "READER_LOG"
#define  READ_COUNT "READER_COUNT"
#define  QUERY_DEPTH "QUERY DEPTH"
#define  USE_FULL_DATA_INTERVAL "FULL_DATA_INTERVAL_CHECK"
#define  FULL_DATA_INTERVAL "FULL_DATA_INTERVAL"
#define  USE_OVERRIDE_QUERY_DEPTH "USE_OVERRIDE_QUERY_DEPTH"
#define  OVERRIDE_QUERY_DEPTH "OVERRIDE_QUERY_DEPTH"