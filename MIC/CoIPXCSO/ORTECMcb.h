//ORTECMcb.h


#ifndef ORTECMCB_H
#define ORTECMCB_H

#include "mcbcio32/mcbcio32.h"
#include "cmcb.h"
class CComORTEC;

class CORTECMcb : public CMcb
{
public:
	CORTECMcb(char * DetName, DWORD DetId, CSpctrm *pSpecInfo, void *pParent = NULL);
	~CORTECMcb();

	bool SendCommand(char *Cmd);
	void SetQuiet(bool quiet);

	bool m_bQuiet;
	CComORTEC *m_pParent;
};


#endif //ORTECMCB_H