
/* cmca.h   definition of a generic MCA class */

#ifndef CMCA_H
#define CMCA_H

//#include  "cdattim.h"
#include "gamnew.h"

class CMca
{
public:
	// constructor
	CMca();
	
	// destructor
	~CMca();

	long         CountTime(void);
	int          CycleNum(void);
	const char * DetName(void);
	long         ElapsedTime(void);
	const char * ErrorMessage(void);
	bool         IsBusy(void);
	bool         IsDone(void);
	bool         IsOpen(void);
	bool         IsSuspended(void);
	void         LogErrors(bool Flag, char *File=(char *)0);
	int          NumCycles(void);
	void         SetClearFlag(bool Flag);
	void         SetCountTime(long Sec, bool LiveTime=true);
	void         SetCycleNum(int Num);
	void         SetDetName(char *Name, char *Mfg);
	void         SetLiveTimeFlag(bool Flag);
	void         SetNumCycles(int Num);
	void         SetSourceFile(char *File);
	void         Simulate(bool Flag);
	
protected:
	bool           bBusy;
	bool           bClear;
	long           lCountTime;
	int            iCycleNum;
	char *        szDetLogo;
	char *        szDetName;
	bool           bDone;
	double         dElapsedTime;
	const char *  szErrorMessage;
	bool           bLiveTime;
	bool           bLogErrors;
	char *        szLogFile;
	int            iNumCycles;
	bool           bOpen;
	double         dSaveTime;
	bool           bSimulate;
	char *        szSrcFile;
	CDatTim        xStartTime;
	bool           bSuspended;
	
private:
};

#endif
