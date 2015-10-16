/////////////////////////////////////////////////////////////////////////////
// DxIntr.h
//
// Header file for interface between DriverX drivers and user-mode components.
//
// DriverX, Version 3.4
//
// Copyright (C) 1995-1998, Tetradyne Software Inc.
// All rights reserved.

#ifndef DXINTR_H
#define DXINTR_H

#define MAX_DEVICE_NAME_LEN 40 
#define NUM_PORTS 			16		// Number of port ranges per device
#define MAX_PORT_INDEX		(NUM_PORTS-1)
#define USE_DIRECT 			1000 	
#define MAX_MEM_INDEX		15

#pragma pack(8)

/////////////////////////////////////////////////////////////////////
// Constants for fFlags field of MAPMEM_INPUT structure.
//
// DXMEM_TYPE_DEVICE returns memory range specified by MemIndex
// registry/INF file parameter.
//
// DXMEM_TYPE_DEVICE2 (added in ver 3.4) returns the memory range
// specified by the nIndex field of the MAPMEM_INPUT structure.

#define DXMEM_TYPE_PHYSICAL		0
#define DXMEM_TYPE_DEVICE		1
#define DXMEM_TYPE_DMA_BUFFER	2
#define DXMEM_TYPE_DEVICE2		3

///////////////////////////////////////////////////////////////////////
// Input structure for IOCTL_MAP_MEMORY IOCTL request.

typedef struct
{
	ULONG	nDeviceID;
	ULONG	fType;				// DXMEM_TYPE_xxx constant
	ULONG	fInterfaceType;		// only used for DXMEM_TYPE_PHYSICAL
	ULONG	nBusNumber;			// ""
	ULONG	nAddress;			// ""
	ULONG	nLength;			// ""
	ULONG	fAddressSpace;		// ""
	ULONG	nIndex;
	
} MAPMEM_INPUT, *PMAPMEM_INPUT;	

///////////////////////////////////////////////////////////////////////
// Output (return) structure for IOCTL_MAP_MEMORY IOCTL request.

typedef struct
{
	ULONG	nPhysicalAddress; // low 32-bits
	ULONG	nHalAddress;
	ULONG	nVirtualAddress;
	ULONG	nDriverVirtualAddress;
	ULONG	nCount;
	ULONG	nPhysicalAddressHigh; // high 32-bits
	ULONG	Reserved2;
	ULONG	Reserved3;

} MAPMEM_OUTPUT, *PMAPMEM_OUTPUT;

///////////////////////////////////////////////////////////////////////
// Input structure for IOCTL_UNMAP_MEMORY IOCTL request.

typedef struct
{
	ULONG	nDeviceID;
	ULONG	fType;
	ULONG	nDriverVirtualAddress;
	ULONG	nIndex; 
	ULONG	Reserved2;
	ULONG	Reserved3;
	ULONG	Reserved4;
	ULONG	Reserved5;

} UNMAPMEM_INPUT, *PUNMAPMEM_INPUT;

///////////////////////////////////////////////////////////////////////
// Operation request structure

typedef struct _DXOP
{
	ULONG fType;

	// Note: second ULONG must be offset for all port ops
	union
	{
		struct
		{
			ULONG nOffset;
			ULONG nValue;
			PVOID pVar;

		} Inp;

		struct
		{
			ULONG nOffset;
			ULONG nValue;

		} Outp;

		struct
		{
			ULONG nMask;
			ULONG bBreak;

		} BIA; // BreakIfAnd

		struct
		{
			ULONG nOffset;
			ULONG nTimeout;
			ULONG nMask;
			ULONG nDataSize;
			ULONG bLoopWhile;

		} WIA; // WhileInpAnd

		struct
		{
			ULONG nOffset;
			ULONG nTimeout;
			ULONG nMask;
			ULONG nDataSize;
			ULONG bLoopWhile;
			ULONG nValue;

		} WIE; // WhileInpEqual

		struct
		{
			// Combined with the fType, this makes sizeof(DXOP)=32 bytes
			ULONG nValue1;
			ULONG nValue2;
			ULONG nValue3;
			ULONG nValue4;
			ULONG nValue5;
			ULONG nValue6;
			ULONG nValue7;

		} Value;
	};

} DXOP, *PDXOP;

// Port operation codes (note that low byte is data size)

#define DXOP_PORTOP			0x0080

#define DXOP_INP_BASE		0x0080
#define DXOP_INP			0x0081
#define DXOP_INPW			0x0082
#define DXOP_INPD			0x0084

#define DXOP_OUTP_BASE		0x00C0
#define DXOP_OUTP			0x00C1
#define DXOP_OUTPW			0x00C2
#define DXOP_OUTPD			0x00C4

#define DXOP_TEMP_OUTP_BASE	0x00A0
#define DXOP_TEMP_OUTP		0x00A1
#define DXOP_TEMP_OUTPW		0x00A2
#define DXOP_TEMP_OUTPD		0x00A4

#define DXOP_BUF_INP_BASE	0x00E0
#define DXOP_BUF_INP		0x00E1
#define DXOP_BUF_INPW		0x00E2
#define DXOP_BUF_INPD		0x00E4

#define DXOP_BUF_OUTP_BASE	0x00F0
#define DXOP_BUF_OUTP		0x00F1
#define DXOP_BUF_OUTPW		0x00F2
#define DXOP_BUF_OUTPD		0x00F4

#define DXOP_DELAY			0x0001
#define DXOP_BREAK_IF_AND	0x0002
#define DXOP_WHILE_INP_AND	0x0003
#define DXOP_LOOP_BACK		0x0004
#define DXOP_BREAK_IF_EQUAL	0x0005
#define DXOP_OR_TEMP		0x0006
#define DXOP_AND_TEMP		0x0007
#define DXOP_WHILE_INP_EQU	0x0008

typedef struct _DXOPHEADER
{
	ULONG	nDeviceID;
	ULONG	nVersion;
	ULONG	nIndex;		// Specifies port range
	ULONG	nOps;		// Number of ops to be executed

	ULONG	nAllocated;	// Number of ops allocated
	ULONG	fInFlags;	// DXOPF_xxx flags
	ULONG	fOutFlags;	// DXOPF_xxx flags
	ULONG	cbReturn;	// Bytes of data read or written

	ULONG	cbData;		// Bytes of data following op structures
	ULONG	cbDataAllocated; // Bytes of data allocated
	ULONG	Reserved1;
	ULONG	Reserved2;

	DXOP	ops[1];

} DXOPHEADER, *PDXOPHEADER;

// DXOPHEADER flags

#define DXOPF_ISR_THREAD		0x01
#define DXOPF_SERVICING_ISR		0x01
#define DXOPF_SYNCH_WITH_ISR	0x02
#define DXOPF_DISABLE_INTS		0x04

#define DXOPF_TIMEOUT			0x100

///////////////////////////////////////////////////////////////////////
// The DriverX 3 driver supports a single operation which can be 
// executed on each ISR to stop the hardware from interrupting. The
// following codes can be defined as the "IsrOp" value in the device
// registry entry.

#define DXISR_INP		0x01
#define DXISR_INPW		0x02
#define DXISR_INPD		0x03
#define DXISR_OUTP		0x04
#define DXISR_OUTPW		0x05	
#define DXISR_OUTPD		0x06	
#define DXISR_INOUTP	0x07
#define DXISR_INOUTPW	0x08
#define DXISR_INOUTPD	0x09

///////////////////////////////////////////////////////////////////////
// DriverX 3.3 adds ISR ID operation for identifying if device is
// interrupting. Following codes can be defined as the "IsrIdOp" value
// in device registry entries.

#define DXISRID_INP_AND         0x01
#define DXISRID_INPW_AND        0x02
#define DXISRID_INPD_AND        0x03
#define DXISRID_NOT_INP_AND     0x04
#define DXISRID_NOT_INPW_AND    0x05
#define DXISRID_NOT_INPD_AND    0x06
#define DXISRID_INP_EQUAL       0x07
#define DXISRID_INPW_EQUAL      0x08
#define DXISRID_INPD_EQUAL      0x09

/////////////////////////////////////////////////////////////////////
// Input and output structures for IOCTL_WAIT_INTERRUPT command

typedef struct
{
	ULONG nCount;
	ULONG nData;		// Data from ISR operation
	ULONG nIdData;		// Data from ISR ID operation
	ULONG Reserved2;

} WAIT_INTERRUPT_OUTPUT, *PWAIT_INTERRUPT_OUTPUT;

/////////////////////////////////////////////////////////////////////
// Input and output structures for IOCTL_MAP_PORTS command

typedef struct
{
	ULONG	nDeviceID;
	ULONG	fInterfaceType;
	ULONG	nBusNumber;
	ULONG	nPortBase;
	ULONG	nPortCount;
	ULONG	nIndex;
	ULONG	Reserved1;
	ULONG	Reserved2;

} MAP_PORTS_INPUT, *PMAP_PORTS_INPUT;

typedef struct
{
	ULONG	nHalReturn;
	ULONG	nMappedAddress;
	ULONG	fAddressSpace;
	ULONG	nPortIndex;
	ULONG	Reserved1;
	ULONG	Reserved2;

} MAP_PORTS_OUTPUT, *PMAP_PORTS_OUTPUT;

/////////////////////////////////////////////////////////////////////
// Input structure for IOCTL_UNMAP_PORTS command

typedef struct
{
	ULONG	nDeviceID;
	ULONG	nPortIndex;
	ULONG	Reserved1;
	ULONG	Reserved2;

} UNMAP_PORTS_INPUT, *PUNMAP_PORTS_INPUT;

/////////////////////////////////////////////////////////////////////
// Input and output structures for IOCTL_CONNECT_INTERRUPT command

typedef struct
{
	ULONG	nDeviceID;
	ULONG 	fInterfaceType;
	ULONG	nBusNumber;
	ULONG	nInterruptLevel;
	ULONG	nInterruptVector;
	ULONG	fInterruptMode;

} CONNECT_INTERRUPT_INPUT, *PCONNECT_INTERRUPT_INPUT;

typedef struct
{
	// HalGetInterruptVector and IoConnectInterrupt results
	ULONG nMappedVector;	
	ULONG nIrql;			
	ULONG nAffinity;		
	ULONG nStatus;		

	// VPICD_Virtualize_IRQ success (only field used on Win95)
	ULONG nSuccess;

} CONNECT_INTERRUPT_OUTPUT, *PCONNECT_INTERRUPT_OUTPUT;

/////////////////////////////////////////////////////////////////////
// Input control structures for IOCTL_AUTOINC_READ/WRITE commands

typedef struct
{
	ULONG	nDeviceID;
	ULONG	nIndex;
	ULONG	nOffset;
	ULONG	nCount;		 // Size of data in bytes
	ULONG	nDataSize;	 // Byte size (1-byte,2-word,4-long)
	ULONG	bShortArray; // Reading bytes into unsigned short array
	ULONG	Reserved2;
	ULONG	Reserved3;

} AUTOINC_RW_INPUT, *PAUTOINC_RW_INPUT;

/////////////////////////////////////////////////////////////////////
// Input structure for IOCTL_SET_PROPERTY command

typedef struct
{
	ULONG	nDeviceID;
	ULONG	nPropCode;
	ULONG	nPropState;
	ULONG	nReserved;

} PROPERTY_INPUT, *PPROPERTY_INPUT;

///////////////////////////////////////////////////////////////////////
// Input structure for IOCTL_GET/SET_BUS_DATA_BY_OFFSET commands

typedef struct
{
	ULONG	nDeviceID;
	ULONG	nOffset;
	ULONG	cbBuffer;
	ULONG	nReserved1;
	ULONG	nReserved2;
	ULONG	nReserved3;
	ULONG	nReserved4;
	UCHAR	buffer[4];

} BUS_DATA_PARAMS, *PBUS_DATA_PARAMS;

///////////////////////////////////////////////////////////////////////
// Codes for IOCTL_GET/SET_PROPERTY

#define DXPROP_ISR_ENABLED			1
#define DXPROP_IS_INTR_CONNECTED	2
#define DXPROP_GET_DEVICE_ID		3
#define DXPROP_SLOT_NUMBER			4

/////////////////////////////////////////////////////////////////////
// IOCTL code definitions

#define IOCTL_EXECUTE_OPS \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2048,METHOD_BUFFERED,FILE_ANY_ACCESS)

// Maps a device, physical, or DMA buffer memory range into the callers address space
#define IOCTL_MAP_MEMORY \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2049,METHOD_BUFFERED,FILE_ANY_ACCESS)

// Unmaps a memory range previously mapped using IOCTL_MAP_MEMORY
#define IOCTL_UNMAP_MEMORY \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2050,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_WAIT_INTERRUPT \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2051,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_GET_PROPERTY \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2052,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_SET_PROPERTY \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2053,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_MAP_PORTS \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2054,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_UNMAP_PORTS \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2055,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_CONNECT_INTERRUPT \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2056,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_DISCONNECT_INTERRUPT \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2057,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_AUTOINC_READ \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2058,METHOD_IN_DIRECT,FILE_ANY_ACCESS)

#define IOCTL_AUTOINC_WRITE \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2059,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)

#define IOCTL_BUF_AUTOINC_READ \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2060,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_BUF_AUTOINC_WRITE \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2061,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_MAP_CONTIGUOUS_MEMORY \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2062,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_UNMAP_CONTIGUOUS_MEMORY \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2063,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_START_DEVICE \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2064,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_STOP_DEVICE \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2065,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_GET_DEVICE_ID \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2066,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_GET_BUS_DATA_BY_OFFSET \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2067,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_SET_BUS_DATA_BY_OFFSET \
	CTL_CODE( FILE_DEVICE_UNKNOWN,0x2068,METHOD_BUFFERED,FILE_ANY_ACCESS)

#pragma pack()

#endif // ifndef DXINTR_H

