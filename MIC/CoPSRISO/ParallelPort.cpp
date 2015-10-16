// ParallelPort.cpp

#include "StdAfx.h"
#include "ParallelPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL ParallelPort::ConfigureDriver()
{
  if (!HwIsDriverConfigured())
	return HwConfigureDriver();
  return TRUE;
}

BOOL ParallelPort::StartDriver()
{
	int i;
	SC_HANDLE scHandle = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE); 
	if (scHandle)
	{
		scHandle = OpenService(scHandle, "DriverX",SERVICE_START);
		if (scHandle) 
			i = StartService(scHandle,0,NULL);
	}
//TRACE("StartService: %d\r\n",i);
	return scHandle?TRUE:FALSE;
}

BOOL ParallelPort::ConfigureDevice(char *name, int base)
{
	HWDEVICE_CONFIG config;
	memset(&config, 0,sizeof(HWDEVICE_CONFIG));
	config.cbSize = sizeof(HWDEVICE_CONFIG);
	config.bIgnoreConflicts = TRUE;
	HwConfigurePortRange(&config, base, 3);

	return HwConfigureDeviceEx((LPCSTR)name,&config);
}

void ParallelPort::DisconnectDevice()
{
	device.DisconnectDevice();
}

BOOL ParallelPort::ConnectDevice(char *deviceName)
{
	return device.ConnectDevice(deviceName);
}

int ParallelPort::ReadByte(int offset)
{
  return device.inp(offset);
}

int ParallelPort::WriteByte(int offset, int data)
{
  return device.outp(offset, (unsigned char)data);
}

int ParallelPort::SetPin(int pin)
{

  int in=0;
  int out=0;
  int shift=0;

  switch(pin)
  {
	  //data register pins
    case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:

		shift=pin-2;

		in=device.inp(0);
		out=in | 1<<shift;

		if(!device.outp(0, (unsigned char)out)) return 0;
		break;

	  //control register pins
	case 1:
	case 14:
	case 16:
	case 17:

		switch(pin)
		{
  		  case 1:
			shift=0;
			break;
  		  case 14:
			shift=1;
			break;
		  case 16:
			shift=2;
			break;
		  case 17:
			shift=3;
			break;
		}		

		in=device.inp(2);

		out=in^11;
		out=out | 1<<shift;
		out=out^11;
        
		if(!device.outp(2, (unsigned char)out)) return 0;
		break;
  }

  return 1;
}

int ParallelPort::UnSetPin(int pin)
{

  int in=0;
  int out=0;
  int shift=0;

  switch(pin)
  {
	  //data register pins
    case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:

		shift=pin-2;

		in=device.inp(0);
		out=in & (~(1<<shift));

		if(!device.outp(0, (unsigned char)out)) return 0;
		break;

	  //control register pins
	case 1:
	case 14:
	case 16:
	case 17:

		switch(pin)
		{
  		  case 1:
			shift=0;
			break;
  		  case 14:
			shift=1;
			break;
		  case 16:
			shift=2;
			break;
		  case 17:
			shift=3;
			break;
		}		

		in=device.inp(2);

		out=in^11;
		out=out & (~(1<<shift));
		out=out^11;

		if(!device.outp(2, (unsigned char)out)) return 0;
		break;
  }

  return 1;
}

int ParallelPort::IsPinSet(int pin)
{
  int in=0;
  int shift=0;

  switch(pin)
  {
	  //data register pins
    case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:

      in=device.inp(0);
      shift=pin-2;
      break;

	  //status register pins
	case 10:
	case 11:
	case 12:
	case 13:
	case 15:

	  in=device.inp(1);
	  in=in^128;
	  in=in>>3;

	  switch(pin)
	  {
		case 10:
		  shift=3;
		  break;
		case 11:
		  shift=4;
		  break;
		case 12:
		  shift=2;
		  break;
		case 13:
		  shift=1;
		  break;
		case 15:
		  shift=0;
		  break;
	  }
	  break;

		//control register
	case 1:
	case 14:
	case 16:
	case 17:

	  in=device.inp(2);
	  in^=11;

      switch(pin)
	  {
	    case 1:
	  	  shift=0;
		  break;
		case 14:
		  shift=1;
		  break;
	    case 16:
		  shift=2;
		  break;
		case 17:
		  shift=3;
		  break;
	  }
	  break;

	default: return 0;
  }

  if(in&1<<shift) return 1;
  else return 0;

}
