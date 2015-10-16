// ParallelPort.h

#ifndef PARALLELPORT_H
#define PARALLELPORT_H

#include "DxClass.h"
#include <winsvc.h>
class ParallelPort
{
public:

	BOOL ConfigureDriver();
	BOOL StartDriver();
	BOOL ConfigureDevice(char* name, int base);
	void DisconnectDevice();
	BOOL ConnectDevice(char *deviceName);
	int WriteByte(int offset, int data);
	int ReadByte(int offset);
	int SetPin(int pin);
	int UnSetPin(int pin);
	int IsPinSet(int pin);

private:
	CDevice device;
};

#endif // PARALLELPORT_H