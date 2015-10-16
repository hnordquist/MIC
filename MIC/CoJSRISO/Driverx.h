///////////////////////////////////////////////////////////////////////
// Driverx.h
//
// Header for C interface to DriverX kernel-mode driver services.
//
// DriverX, Version 3.4
//
// Copyright (C) 1995-1997, Tetradyne Software Inc.
// All rights reserved.

#ifndef DRIVERX_H
#define DRIVERX_H

#include <Windows.h>
#include <Winioctl.h>
#include "DxIntr.h"

#define DX_MAJOR_VER	3
#define DX_MINOR_VER	4

///////////////////////////////////////////////////////////////////////
// Debug macros

#ifdef __cplusplus
extern "C" {
#endif

VOID _stdcall HwTrace(LPCSTR, ...);
VOID _stdcall HwAssertionFailure(LPCSTR lpszFile, int nLine);

#ifdef __cplusplus
}
#endif

#ifdef _DEBUG

	#ifndef TRACE
		#define TRACE HwTrace
	#endif
	#ifndef ASSERT
		#define ASSERT(c) 	if(!(c)) HwAssertionFailure(__FILE__,__LINE__);
	#endif

#else 	
	// _DEBUG not defined

	#ifndef TRACE
		#define TRACE 1 ? (VOID)0 : HwTrace
	#endif
	#ifndef ASSERT
		#define ASSERT(c) ((VOID)0)
	#endif

#endif	// ifdef _DEBUG

#define MAX_NAME_LEN 			40
#define MAX_DEVICE_ID			15
#define BYTES_INTO_SHORT_ARRAY	5

///////////////////////////////////////////////////////////////////////
// Error codes

#define HW_SUCCESS				0
#define HW_EVENT_ERROR			3
#define HW_WAIT_INTR_ERROR		4
#define HW_IOCTL_ERROR			5

///////////////////////////////////////////////////////////////////////
// HWDEVICE_CONFIG 
//
// Defines a registry configuration for a device.

#define HWDEVICE_CONFIG_PRE33_SIZE 68

typedef struct
{
	BOOL	bNonIsaInterface;
	DWORD	fInterfaceType;	// NT DDK INTERFACE_TYPE enum
	DWORD	nBusNumber;

	BOOL	bUsesPortRange;	// TRUE if device uses a port range
	DWORD	nPortBase;
	DWORD	nPortCount;

	BOOL	bUsesInterrupt;	// TRUE if device uses an interrupt
	DWORD	nInterruptLevel;
	DWORD	nIsrOp;
	DWORD	nIsrData;
	DWORD	nIsrOffset;

	BOOL	bUsesMemRange;	// TRUE if device uses a memory range
	DWORD	nMemBase;
	DWORD	nMemCount;

	BOOL	bDynamic; 		// Must manual start

#ifdef __cplusplus
	union
	{
#endif
		DWORD nVendorID;
#ifdef __cplusplus
		BOOL bIgnoreConflicts;
	};
#endif

	DWORD nDeviceID;

	// New fields added in 3.3
    DWORD cbSize;

    // ISR ID added in 3.3 for PCI devices
    DWORD nIsrIdOp;
    DWORD nIsrIdOffset;
    DWORD nIsrIdMask;
    DWORD nIsrIdValue;

	// For PCI devices with memory-mapped I/O registers
	BOOL bMemoryIo;

	// For busmaster PCI devices which need a DMA buffer
	DWORD nDmaBufSize;

} HWDEVICE_CONFIG;	

#define DXISA_INTERFACE 1
#define DXPCI_INTERFACE 5

///////////////////////////////////////////////////////////////////////
// HWDEVICE 
//
// Encapsulates information about a connection to a kernel driver and 
// device object.

typedef struct hwdevice_tag
{
	// Pointer to user-defined ISR callback routine
	VOID (*pIsr)(struct hwdevice_tag* pDevice);

	// Error handler
	VOID (*pErrorHandler)(struct hwdevice_tag* pDevice, DWORD nError);

	// Extension data (C++ classes use this as ptr to containing object)
	PVOID pExtension;

	// Handle to kernel-mode device object (handle to VxD on Win95)
	HANDLE hDevice;

	// Handle for use by interrupt thread
	HANDLE hDeviceIsr;

	// Device ID (only used on Windows 95)
	DWORD nID;

	// ISR thread handle and ID
	HANDLE hIsrThread;
	DWORD nIsrThreadID;

	// Critical section for synchronizing with ISR thread
	CRITICAL_SECTION crSection;

	// Handle to cancel event set by main thread to stop ISR thread
	HANDLE hCancelEvent;

	// Return from wait interrupt call
	WAIT_INTERRUPT_OUTPUT waitOutput;

} HWDEVICE;

///////////////////////////////////////////////////////////////////////
// HWMEMORY 
//
// Holds info about a mapped memory range (physical, device, or DMA 
// buffer).

typedef struct
{
	// Return data from IOCTL_MAP_MEMORY request
	MAPMEM_OUTPUT output;

	// High word is index; Low word is DXMEM_TYPE_xxx flag
	DWORD fType;	

	// Associated device for mapping pre-defined device memory range
	HWDEVICE* pDevice;

} HWMEMORY;

///////////////////////////////////////////////////////////////////////
// HWPCI_CONFIG_SPACE
//
// PCI configuration space definition.

#define PCI_TYPE0_ADDRESSES 6

typedef struct
{
	WORD nVendorID;
	WORD nDeviceID;
	WORD fCommand;
	WORD fStatus;
	BYTE nRevisionID;
	BYTE nProgInterface;
	BYTE nSubClass;
	BYTE nBaseClass;
	BYTE nCacheLineSize;
	BYTE nLatencyTimer;
	BYTE fHeaderType;
	BYTE fBuiltInSelfTest;

	union 
	{
		struct
		{
			DWORD nAddresses[PCI_TYPE0_ADDRESSES];
			DWORD cis;
			WORD  nSubVendorID;
			WORD  nSubSystemID;
			DWORD nROMBaseAddress;
			DWORD reserved[2];			
			BYTE  fInterruptLine;
			BYTE  fInterruptPin;
			BYTE  nMinimumGrant;
			BYTE  nMaximumLatency;
		} type0;
	} u;

	BYTE deviceDependant[192];

} HWPCI_CONFIG_SPACE;

///////////////////////////////////////////////////////////////////////
// Hardware Library Prototypes

#ifdef __cplusplus
extern "C"
{
#endif

// Driverx.c Prototypes

BOOL _stdcall IsOn95();
BOOL _stdcall IsOnNT();
VOID _stdcall HwTrace(LPCSTR pszFormat, ...);
VOID _stdcall HwAssertionFailure(LPCSTR pszFile, int nLine);

HWDEVICE* _stdcall HwNewDevice(VOID (*pIsr)(HWDEVICE*));
VOID _stdcall CommonConstructDevice(HWDEVICE* pDevice, VOID (*pIsr)(HWDEVICE*));
VOID _stdcall HwSetErrorHandler(HWDEVICE* pDevice, VOID (*pHandler)(HWDEVICE*, DWORD nError));
BOOL _stdcall HwConnectDevice(HWDEVICE* pDevice, LPCTSTR pszDevice);
VOID _stdcall HwDisconnectDevice(HWDEVICE* pDevice);
VOID _stdcall HwDeleteDevice(HWDEVICE* pDevice);
VOID _stdcall CommonDestroyDevice(HWDEVICE *pDevice);
BOOL _stdcall HwExecute(HWDEVICE* pDevice, PDXOPHEADER pOps);
BOOL _stdcall HwEnableIsr(HWDEVICE* pDevice, HANDLE hThread, 
		DWORD nThreadID);
VOID _stdcall HwDisableIsr(HWDEVICE* pDevice);
UINT _stdcall HwIsrThreadProc(PVOID pContext);
UINT HwMfcIsrThreadProc(PVOID pContext);
VOID HwBcppIsrThreadProc(PVOID pContext);
BOOL _stdcall HwSetProperty(HWDEVICE* pDevice, DWORD nCode, 
		DWORD nState);
BOOL _stdcall HwGetProperty(HWDEVICE* pDevice, DWORD nCode, 
		PDWORD pnState);
BOOL _stdcall HwConnectInterrupt(HWDEVICE* pDevice, DWORD nInterruptLevel);
BOOL _stdcall HwConnectInterruptEx(HWDEVICE* pDevice, DWORD nInterruptLevel,
	DWORD nVector, DWORD fInterfaceType, DWORD nBusNumber);
BOOL _stdcall HwDisconnectInterrupt(HWDEVICE* pDevice);
DWORD _stdcall HwGetIsrData(HWDEVICE* pDevice);
DWORD _stdcall HwGetIsrIdData(HWDEVICE* pDevice);
VOID _stdcall HwBeginIsrSynch(HWDEVICE* pDevice);
VOID _stdcall HwEndIsrSynch(HWDEVICE* pDevice);

BOOL _stdcall HwMapPorts(HWDEVICE* pDevice, DWORD nBase, 
		DWORD nCount, PDWORD pnIndex);
BOOL _stdcall HwMapPortsEx(HWDEVICE* pDevice, DWORD nBase, DWORD nCount, 
		DWORD fInterfaceType, DWORD nBusNumber, PDWORD pnIndex);
BOOL _stdcall HwUnmapPorts(HWDEVICE* pDevice, DWORD nIndex);
DWORD _stdcall HwInPort(HWDEVICE* pDevice, DWORD nIndex, DWORD nOffset, DWORD nDataSize);
BYTE _stdcall HwInp(HWDEVICE* pDevice, DWORD nOffset);
WORD _stdcall HwInpw(HWDEVICE* pDevice, DWORD nOffset);
DWORD _stdcall HwInpl(HWDEVICE* pDevice, DWORD nOffset);
BOOL _stdcall HwOutPort(HWDEVICE* pDevice, DWORD nIndex, DWORD nOffset, DWORD nDataSize, DWORD nData);
BOOL _stdcall HwOutp(HWDEVICE* pDevice, DWORD nOffset, DWORD nData);
BOOL _stdcall HwOutpw(HWDEVICE* pDevice, DWORD nOffset, DWORD nData);
BOOL _stdcall HwOutpl(HWDEVICE* pDevice, DWORD nOffset, DWORD nData);
BOOL _stdcall HwAutoIncReadPort(HWDEVICE* pDevice, DWORD nIndex, 
		DWORD nOffset, PVOID pBuf, DWORD nCount, DWORD nDataSize);
BOOL _stdcall HwAutoIncWritePort(HWDEVICE* pDevice, DWORD nIndex, 
		DWORD nOffset, PVOID pBuf, DWORD nCount, DWORD nDataSize);
BOOL _stdcall HwGetBusDataByOffset(HWDEVICE* pDevice, ULONG nOffset, PVOID pBuffer, ULONG cbBuffer);
BOOL _stdcall HwSetBusDataByOffset(HWDEVICE* pDevice, ULONG nOffset, PVOID pBuffer, ULONG cbBuffer);

// Operation queue routines

PDXOPHEADER _stdcall HwAllocateOpQueue(DWORD nIndex, DWORD nOps, DWORD cbData);
VOID _stdcall HwDeleteOpQueue(PDXOPHEADER pOpHeader);
DWORD _stdcall HwGetQueueResults(PDXOPHEADER pHeader, PDWORD pcbProcessed);
VOID _stdcall HwQueueDelay(PDXOPHEADER pHeader, DWORD nMicroSec);

VOID _stdcall HwQueueInPort(PDXOPHEADER pHeader, DWORD nOffset, DWORD nSize, 
		PDWORD pRetData);
VOID _stdcall HwQueueInp(PDXOPHEADER pHeader, DWORD nOffset, PDWORD pRetData);
VOID _stdcall HwQueueInpw(PDXOPHEADER pHeader, DWORD nOffset, PDWORD pRetData);
VOID _stdcall HwQueueInpl(PDXOPHEADER pHeader, DWORD nOffset, PDWORD pRetData);

VOID _stdcall HwQueueOutPort(PDXOPHEADER pHeader, DWORD nOffset, DWORD nSize, 
		DWORD nData);
VOID _stdcall HwQueueOutp(PDXOPHEADER pHeader, DWORD nOffset, DWORD nData);
VOID _stdcall HwQueueOutpw(PDXOPHEADER pHeader, DWORD nOffset, DWORD nData);
VOID _stdcall HwQueueOutpl(PDXOPHEADER pHeader, DWORD nOffset, DWORD nData);

VOID _stdcall HwQueueBufInPort(PDXOPHEADER pHeader, DWORD nOffset, DWORD nSize);
VOID _stdcall HwQueueBufInp(PDXOPHEADER pHeader, DWORD nOffset);
VOID _stdcall HwQueueBufInpw(PDXOPHEADER pHeader, DWORD nOffset);
VOID _stdcall HwQueueBufInpl(PDXOPHEADER pHeader, DWORD nOffset);

VOID _stdcall HwQueueBufOutPort(PDXOPHEADER pHeader, DWORD nOffset, DWORD nSize);
VOID _stdcall HwQueueBufOutp(PDXOPHEADER pHeader, DWORD nOffset);
VOID _stdcall HwQueueBufOutpw(PDXOPHEADER pHeader, DWORD nOffset);
VOID _stdcall HwQueueBufOutpl(PDXOPHEADER pHeader, DWORD nOffset);

VOID _stdcall HwQueueTempOutPort(PDXOPHEADER pHeader, DWORD nOffset, DWORD nSize);
VOID _stdcall HwQueueTempOutp(PDXOPHEADER pHeader, DWORD nOffset);
VOID _stdcall HwQueueTempOutpw(PDXOPHEADER pHeader, DWORD nOffset);
VOID _stdcall HwQueueTempOutpl(PDXOPHEADER pHeader, DWORD nOffset);

PVOID _stdcall HwGetQueueBuffer(PDXOPHEADER pHeader);
VOID _stdcall HwQueueWhileInpAnd(PDXOPHEADER pHeader, DWORD nOffset, 
		DWORD nDataSize, DWORD nMask, BOOL bLoopWhile, DWORD nLimit);
VOID _stdcall HwQueueWhileInpEqual(PDXOPHEADER pHeader, DWORD nOffset, 
		DWORD nDataSize, DWORD nMask, DWORD nValue, BOOL bLoopWhile, DWORD nLimit);
VOID _stdcall HwQueueLoopBack(PDXOPHEADER pHeader);
VOID _stdcall HwQueueBreakIfAnd(PDXOPHEADER pHeader, DWORD nMask, BOOL bBreak);
VOID _stdcall HwQueueOrTemp(PDXOPHEADER pHeader, DWORD nMask);
VOID _stdcall HwQueueAndTemp(PDXOPHEADER pHeader, DWORD nMask);
BOOL _stdcall HwEnableQueueOption(PDXOPHEADER pHeader, DWORD fOption, BOOL bEnable);

// Memory routines

HWMEMORY* _stdcall HwNewMemory();
VOID CommonConstructMemory(HWMEMORY* pMem);
VOID _stdcall HwDeleteMemory(HWMEMORY* pMem);
BOOL _stdcall HwMapDeviceMemory(HWMEMORY* pMem, HWDEVICE* pDevice);
BOOL _stdcall HwMapDmaBuffer(HWMEMORY* pMem, HWDEVICE* pDevice);
BOOL _stdcall HwMapMemory(HWMEMORY* pMem, DWORD nBaseAddress, DWORD nCount);
BOOL _stdcall HwMapMemoryEx(HWMEMORY* pMem, DWORD nBaseAddress, DWORD nCount,
		DWORD fInterfaceType, DWORD nBusNumber);
BOOL _stdcall HwUnmapMemory(HWMEMORY* pMem);
PBYTE _stdcall HwGetMemoryPointer(HWMEMORY* pMem);
DWORD _stdcall HwGetPhysicalAddress(HWMEMORY* pMem);
VOID _stdcall HwReadMemory(HWMEMORY* pMem, DWORD nOffset, 
		LPBYTE pData, DWORD nCount);
BYTE _stdcall HwReadByteMem(HWMEMORY* pMem, DWORD nOffset);
WORD _stdcall HwReadWordMem(HWMEMORY* pMem, DWORD nOffset);
DWORD _stdcall HwReadLongMem(HWMEMORY* pMem, DWORD nOffset);
VOID _stdcall HwWriteMemory(HWMEMORY* pMem, DWORD nOffset, 
		LPBYTE pData, DWORD nCount);
VOID _stdcall HwWriteByteMem(HWMEMORY* pMem, DWORD nOffset, BYTE b);
VOID _stdcall HwWriteWordMem(HWMEMORY* pMem, DWORD nOffset, WORD w);
VOID _stdcall HwWriteLongMem(HWMEMORY* pMem, DWORD nOffset, DWORD dw);
DWORD _stdcall HwGetMemoryIndex(HWMEMORY* pMem);
VOID _stdcall HwSetMemoryIndex(HWMEMORY* pMem, DWORD dwValue);

// Configuration routines

BOOL _stdcall HwConfigureDevice(LPCSTR pszName, HWDEVICE_CONFIG* pConfig);
BOOL _stdcall HwConfigureDeviceEx(LPCSTR pszName, HWDEVICE_CONFIG* pConfig);
BOOL _stdcall HwConfigureDriver();
BOOL _stdcall HwDeleteDeviceConfiguration(LPCSTR pszName);
BOOL _stdcall HwEnumerateDevices(DWORD nIndex, LPSTR pszName, HWDEVICE_CONFIG* pConfig);
BOOL _stdcall HwEnumerateDevicesEx(DWORD nIndex, LPSTR pszName, HWDEVICE_CONFIG* pConfig);
BOOL _stdcall HwGetDeviceConfig(LPCSTR pszName, HWDEVICE_CONFIG* pConfig);
BOOL _stdcall HwGetDeviceConfigEx(LPCSTR pszName, HWDEVICE_CONFIG* pConfig);
VOID _stdcall HwZeroDeviceConfig(HWDEVICE_CONFIG* pConfig);
VOID _stdcall HwZeroDeviceConfigEx(HWDEVICE_CONFIG* pConfig);
VOID _stdcall HwConfigureInterface(HWDEVICE_CONFIG* pConfig, 
	DWORD fInterfaceType, DWORD nBusNumber);
VOID _stdcall HwConfigureInterrupt(HWDEVICE_CONFIG* pConfig, DWORD nLevel);
VOID _stdcall HwConfigureIsrOp(HWDEVICE_CONFIG* pConfig, DWORD nIsrOp, 
				DWORD nIsrOffset, DWORD nIsrData);
VOID _stdcall HwConfigureIsrIdOp(HWDEVICE_CONFIG* pConfig, DWORD nOp, 
				DWORD nOffset, DWORD nMask, DWORD nValue);
VOID _stdcall HwConfigureMemoryIo(HWDEVICE_CONFIG* pConfig, BOOL bMemoryIo);
VOID _stdcall HwConfigurePortRange(HWDEVICE_CONFIG* pConfig,
				DWORD nPortBase, DWORD nPortCount);
VOID _stdcall HwConfigureMemRange( HWDEVICE_CONFIG* pConfig,
				DWORD nMemBase, DWORD nMemCount);
VOID _stdcall HwConfigureIgnoreConflicts(HWDEVICE_CONFIG* pConfig, BOOL bEnable);
BOOL _stdcall HwIsDriverConfigured();
BOOL _stdcall HwStartDevice(LPCSTR pszName);
BOOL _stdcall HwStopDevice(LPCSTR pszName);
BOOL _stdcall HwIsDeviceRunning(LPCTSTR pszDevice);

#ifdef __cplusplus
}
#endif

#endif // ifndef HARDLIB_H


