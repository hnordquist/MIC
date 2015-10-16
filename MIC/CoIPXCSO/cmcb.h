/* cmcb.h   definition of the ORTEC MCB class */

#ifndef CMCB_H
#define CMCB_H

//#include  "cchnspc.h"
#include "gamnew.h"
#include  "cmca.h"

#define CMCB_MAX_TRIES          10
#define CMCB_LEN_COMMAND        79
#define CMCB_LEN_REPLY         127
#define CMCB_TICKS_PER_SECOND   50

enum CMCB_STATUS {
	CMCB_OK,                // Success.
	CMCB_VERSION_MISMATCH,	// Orw.lib and Ors.h version mismatch.
	CMCB_NOT_OPEN,          // OrwOpen hasn't been called.
	CMCB_ALREADY_OPEN,      // OrwOpen has already been called.
	CMCB_INIT_FAIL,         // error calling MIOStartup.
	CMCB_BUSY,              // the detector is currently acquiring data.
	CMCB_NOT_BUSY,          // the detector isn't currently acquiring data.
	CMCB_STILL_BUSY,        // the detector is still busy after STOP.
	CMCB_UMCBI_LIST,        // couldn't access the master list.
	CMCB_RSP_GARBLE,        // response from detector was garbled.
	CMCB_UMCBI_ERROR,       // error encountered; details in pCtx.
	CMCB_IDETIDX_TOO_BIG,   // iDetIndex is outside range of master list.
	CMCB_UMCBI_OPEN,        // MIOXxx error trying to open connection.
	CMCB_NO_PRESET,         // error in retrieveing preset.
	CMCB_PRESET_REACHED,    // on START, preset time already exceeded.
	CMCB_DUMMY_FILE_IO,     // I/O error with dummy file.
	CMCB_SPW_ERR,           // error return from SpwXxx routine.
	CMCB_BAD_INPUT,         // a function input check failed.
	CMCB_BAD_INDEX,         // invalid index into detector list.
	CMCB_BAD_HANDLE,        // invalid detector handle.
	CMCB_ILLEGAL
};

class CMcb : public CMca
{
public:
	// constructor
	CMcb(char * DetName, DWORD DetId, CSpctrm *pSpecInfo);
	
	// destructor
	~CMcb();

	virtual bool       CheckStatus(void);
	virtual bool       Close(void);
	virtual DWORD      DetId(void);
	virtual bool       GetDetectorInfo(int Index, DWORD *Id, int StrMax,
					char *Desc, int *DetMax);
	virtual bool       GetSpectrum(void);
	virtual bool       IoCtl(void *Input=(void *)0);
	virtual bool       IsValid(void);
	virtual bool       Open(bool Simulate=false, char *File=(char *)0);
	virtual bool       Resume(void);
	virtual bool       SaveSpectrum(char *File, void *Input=(void *)0);
	virtual void       SetDetId(DWORD Id);
	virtual CChnSpc * pSpctrm(void);
	virtual bool       StartAcquire(void *Input=(void *)0);
	virtual bool       StopAcquire(void);
	virtual bool       Suspend(void);

protected:
//private: // <<QA>> So that CMICMcb can derive from CMcb  (PJM 2/19/2003)
	char      szCommand[CMCB_LEN_COMMAND+1];
	DWORD      lDetId;
	HDET       hDet;
	int        iError;
	int        iErrorMacro;
	int        iErrorMicro;
	int        iLength;
	bool       bLogErrors;
	char *    szLogFile;
	char      szReply[CMCB_LEN_REPLY+1];
	long       lSimLiveTime;
	long       lSimTrueTime;
	bool       bSpecInfo;
	CSpctrm * pxSpecInfo;
	CChnSpc *  pSpec;
	int        iStatus;
	bool       bValid;

	virtual void  ClearError(void);
	virtual bool  GetInt(char *Cmd, int *Value);
	virtual bool  GetPreset(bool *Live, long *Time);
	virtual bool  GetString(char *Cmd, int Max, char *Buffer);
	virtual bool  GetTriple(char *Cmd, int *Value1, int *Value2, int *Value3);
	virtual bool  GetULong(char *Cmd, ULONG *Value);
	virtual void  LogEvent(bool, char *);
	virtual bool  Query(char *Cmd);
	virtual bool  SendCommand(char *Cmd);
	virtual bool  SetDateTime(void);
};

#endif
