/////////////////////////////////////////////////////////////////////////////
// Dxclass.h
//
// Interface definition of DriverX classes.
//
// DriverX Version 3.4
//
// Copyright (C) 1995-1997 Tetradyne Software Inc.
// All rights reserved.

#ifndef DXCLASS_H
#define DXCLASS_H

extern "C"
{
#include "Driverx.h"
}

/////////////////////////////////////////////////////////////////////
// CDxException

class CDxException
{
public:
	CDxException(DWORD nCode, LPCTSTR pszDesc);
	DWORD GetCode();
	LPCTSTR GetDescription();
protected:
	enum {MaxDescLength=256};
	DWORD m_nCode;
	TCHAR m_szDesc[MaxDescLength];
};

/////////////////////////////////////////////////////////////////////
// CDeviceConfig

class CDeviceConfig : public HWDEVICE_CONFIG
{
// Construction
public:
	CDeviceConfig();

// Operations
	VOID ConfigurePortRange(DWORD nBase, DWORD nCount);
	VOID ConfigureMemRange(DWORD nBase, DWORD nCount);
	VOID ConfigureInterrupt(DWORD nLevel);
	VOID ConfigureIsrOp(DWORD nOp, DWORD nOffset, DWORD nData);
	VOID ConfigureIsrIdOp(DWORD nOp, DWORD nOffset, DWORD nMask, DWORD nData);
	VOID ConfigureDynamic(BOOL bDyn=TRUE);
	VOID ConfigureMemoryIo(BOOL bValue=TRUE);
};

///////////////////////////////////////////////////////////////////////
// COpQueue operation queue class.

class COpQueue
{
friend class CDevice;
public:
	COpQueue();
	~COpQueue();

	BOOL AllocateOpQueue(DWORD nIndex, DWORD nOps, DWORD cbData);
	BOOL QueueAllocated();
	DWORD GetQueueResults(PDWORD pcbProcessed);
	PVOID GetQueueBuffer();
	VOID QueueInp(DWORD nOffset, PDWORD pdwReturn);
	VOID QueueInpw(DWORD nOffset, PDWORD pdwReturn);
	VOID QueueInpl(DWORD nOffset, PDWORD pdwReturn);
	VOID QueueOutp(DWORD nOffset, DWORD nData);
	VOID QueueOutpw(DWORD nOffset, DWORD nData);
	VOID QueueOutpl(DWORD nOffset, DWORD nData);
	VOID QueueBufInp(DWORD nOffset);
	VOID QueueBufInpw(DWORD nOffset);
	VOID QueueBufInpl(DWORD nOffset);
	VOID QueueBufOutp(DWORD nOffset);
	VOID QueueBufOutpw(DWORD nOffset);
	VOID QueueBufOutpl(DWORD nOffset);
	VOID QueueTempOutp(DWORD nOffset);
	VOID QueueTempOutpw(DWORD nOffset);
	VOID QueueTempOutpl(DWORD nOffset);
	VOID QueueWhileInpAnd(DWORD nOffset, USHORT nDataSize, 
			DWORD nMask, BOOL bLoopWhile, DWORD nLimit);
	VOID QueueWhileInpEqual(DWORD nOffset, USHORT nDataSize, 
			DWORD nMask, DWORD nValue, BOOL bLoopWhile, DWORD nLimit);
	VOID QueueLoopBack();
	VOID QueueBreakIfAnd(DWORD nMask, BOOL bBreak);
	VOID QueueOrTemp(DWORD nMask);
	VOID QueueAndTemp(DWORD nMask);

protected:
	DXOPHEADER* m_pQueue;
};

///////////////////////////////////////////////////////////////////////
// CDevice class

class CDevice : public HWDEVICE
{
// Construction
public:
	CDevice();
	virtual ~CDevice();
	BOOL ConnectDevice(LPCSTR pszName);
	BOOL IsConnected();
	void DisconnectDevice();

// Operations
public:
	BOOL	ConnectInterrupt(DWORD nLevel, DWORD nVector=-1,
				DWORD fInterfaceType=1 /* Isa */, DWORD nBusNumber=0);
	BOOL	DisconnectInterrupt();

	BOOL	EnableIsr(HANDLE hThread, DWORD nID);
	BOOL	IsIsrEnabled();
	VOID	DisableIsr();
	VOID	BeginIsrSynch();
	VOID	EndIsrSynch();
	DWORD	GetIsrData();
	DWORD	GetIsrIdData();

	BOOL	Execute(COpQueue* pQueue);

	BOOL	MapPorts(DWORD nBase, DWORD nCount, PDWORD pnIndex);
	BOOL	MapPortsEx(DWORD nBase, DWORD nCount, DWORD fInterfaceType,
				DWORD nBusNumber, PDWORD pnIndex);
	BOOL	UnmapPorts(DWORD nIndex);

	BOOL	OutPort(DWORD nIndex, DWORD nOffset, DWORD nDataSize, DWORD nData);
	BOOL	outp(DWORD nOffset, BYTE cValue);
	BOOL	outpw(DWORD nOffset, WORD nValue);
	BOOL	outpl(DWORD nOffset, DWORD nValue);

	DWORD	InPort(DWORD nIndex, DWORD nOffset, DWORD nDataSize);
	BYTE	inp(DWORD nOffset);
	WORD	inpw(DWORD nOffset);
	DWORD	inpl(DWORD nOffset);

	BOOL AutoIncReadPort(DWORD nIndex, DWORD nOffset, VOID* pBuf, 
			DWORD nCount, DWORD nDataSize=1);
	BOOL AutoIncWritePort(DWORD nIndex, DWORD nOffset, VOID* pBuf, 
			DWORD nCount, DWORD nDataSize=1);

	static BOOL IsDriverConfigured();
	static BOOL ConfigureDriver();
	static BOOL ConfigureDevice(LPCSTR pszName, CDeviceConfig& config);
	static BOOL EnumerateDevices(DWORD nIndex, char* pszBuffer, 
							CDeviceConfig* pConfig);
	static BOOL GetDeviceConfig(LPCSTR pszName, CDeviceConfig* pConfig);
	static BOOL DeleteDeviceConfiguration(LPCSTR pszName);

	static BOOL StartDevice(LPCTSTR pszName);
	static BOOL StopDevice(LPCTSTR pszName);
	static BOOL IsDeviceRunning(LPCTSTR pszName);

	BOOL GetProperty(DWORD nCode, PDWORD pnState);
	BOOL SetProperty(DWORD nCode, DWORD nValue);
	VOID SetErrorHandler(VOID (*pHandler)(HWDEVICE* pDevice, DWORD nError) 
				= CDevice::ErrorHandler);

	BOOL SetBusDataByOffset(ULONG nOffset, PVOID pBuffer, ULONG cbBuffer);
	BOOL GetBusDataByOffset(ULONG nOffset, PVOID pBuffer, ULONG cbBuffer);

// Overridables
public:
	virtual VOID OnInterrupt();

// Implementation
protected:
	static VOID IsrCallback(HWDEVICE* pHwdevice);
	static VOID ErrorHandler(HWDEVICE* pDevice, DWORD nError);
};

/////////////////////////////////////////////////////////////////////
// CMemBase class
//
// Base class for a memory range (could be a device memory range,
// physical memory range, or DMA buffer).

class CMemBase : protected HWMEMORY
{
// Construction
public:
	CMemBase();
	virtual ~CMemBase();

// Properties
	PBYTE GetMemoryPointer();
	PVOID GetBase();
	DWORD GetPhysicalAddress();
	DWORD GetSize();
	DWORD GetMemoryIndex();
	void SetMemoryIndex(DWORD nIndex);

// Operations
	BOOL MapMemory(DWORD nBaseAddress, DWORD nCount,
				DWORD fInterfaceType=1 /* Isa */, DWORD nBusNumber=0);
	BOOL MapDeviceMemory(CDevice* pDevice);
	BOOL MapDmaBuffer(CDevice* pDevice);
	BOOL UnmapMemory();
};

///////////////////////////////////////////////////////////////////////
// CMemory - BYTE memory class.

class CMemory : public CMemBase
{
public:
	BYTE& operator[](int nIndex);
	operator BYTE*();
};

///////////////////////////////////////////////////////////////////////
// CWordMemory - WORD memory class.

class CWordMemory : public CMemBase
{
public:
	WORD& operator[](int nIndex);
	operator WORD*();
};

///////////////////////////////////////////////////////////////////////
// CLongMemory - DWORD memory class.

class CLongMemory : public CMemBase
{
public:
	DWORD& operator[](int nIndex);
	operator DWORD*();
};

///////////////////////////////////////////////////////////////////////
// CDxException inlines

inline CDxException::CDxException(DWORD nCode, LPCTSTR pszDesc)
{
	m_nCode = nCode;
	strncpy(m_szDesc, pszDesc, MaxDescLength);
}

inline DWORD CDxException::GetCode()
	{ return m_nCode; };

inline LPCTSTR CDxException::GetDescription()
	{ return m_szDesc; };

/////////////////// CDeviceConfig()

inline CDeviceConfig::CDeviceConfig()
	{ HwZeroDeviceConfigEx(this); };

inline VOID CDeviceConfig::ConfigurePortRange(DWORD nBase, DWORD nCount)
	{ HwConfigurePortRange(this, nBase, nCount); };

inline VOID CDeviceConfig::ConfigureMemRange(DWORD nBase, DWORD nCount)
	{ HwConfigureMemRange(this, nBase, nCount); };

inline VOID CDeviceConfig::ConfigureInterrupt(DWORD nLevel)
	{ HwConfigureInterrupt(this, nLevel); };

inline VOID CDeviceConfig::ConfigureIsrOp(DWORD nOp, DWORD nOffset, DWORD nData)
	{ HwConfigureIsrOp(this, nOp, nOffset, nData); };

inline VOID CDeviceConfig::ConfigureDynamic(BOOL bDyn)
	{ bDynamic = bDyn; };

inline VOID CDeviceConfig::ConfigureIsrIdOp(DWORD nOp, DWORD nOffset, DWORD nMask, DWORD nData)
	{ HwConfigureIsrIdOp(this, nOp, nOffset, nMask, nData); };

inline VOID CDeviceConfig::ConfigureMemoryIo(BOOL bValue)
	{ bMemoryIo = bValue; };

/////////////////// COpQueue inlines

inline COpQueue::COpQueue()
	{ m_pQueue = NULL; };

inline COpQueue::~COpQueue()
	{ if (m_pQueue) HwDeleteOpQueue(m_pQueue); };

inline BOOL COpQueue::AllocateOpQueue(DWORD nIndex, DWORD nOps, DWORD cbData)
{ 
	m_pQueue = HwAllocateOpQueue(nIndex, nOps, cbData);
	return m_pQueue != NULL; 
};

inline BOOL COpQueue::QueueAllocated()
	{ return (m_pQueue != NULL); };

inline DWORD COpQueue::GetQueueResults(PDWORD pcbProcessed)
	{ return HwGetQueueResults(m_pQueue, pcbProcessed); };

inline VOID COpQueue::QueueInp(DWORD nOffset, PDWORD pdwReturn)
	{ HwQueueInPort(m_pQueue, nOffset, 1, pdwReturn); };

inline VOID COpQueue::QueueInpw(DWORD nOffset, PDWORD pdwReturn)
	{ HwQueueInPort(m_pQueue, nOffset, 2, pdwReturn); };

inline VOID COpQueue::QueueInpl(DWORD nOffset, PDWORD pdwReturn)
	{ HwQueueInPort(m_pQueue, nOffset, 4, pdwReturn); };

inline VOID COpQueue::QueueOutp(DWORD nOffset, DWORD nData)
	{ HwQueueOutPort(m_pQueue, nOffset, 1, nData); };

inline VOID COpQueue::QueueOutpw(DWORD nOffset, DWORD nData)
	{ HwQueueOutPort(m_pQueue, nOffset, 2, nData); };

inline VOID COpQueue::QueueOutpl(DWORD nOffset, DWORD nData)
	{ HwQueueOutPort(m_pQueue, nOffset, 4, nData); };

inline VOID COpQueue::QueueBufInp(DWORD nOffset)
	{ HwQueueBufInPort(m_pQueue, nOffset, 1); };

inline VOID COpQueue::QueueBufInpw(DWORD nOffset)
	{ HwQueueBufInPort(m_pQueue, nOffset, 2); };

inline VOID COpQueue::QueueBufInpl(DWORD nOffset)
	{ HwQueueBufInPort(m_pQueue, nOffset, 4); };

inline VOID COpQueue::QueueBufOutp(DWORD nOffset)
	{ HwQueueBufOutPort(m_pQueue, nOffset, 1); };

inline VOID COpQueue::QueueBufOutpw(DWORD nOffset)
	{ HwQueueBufOutPort(m_pQueue, nOffset, 2); };

inline VOID COpQueue::QueueBufOutpl(DWORD nOffset)
	{ HwQueueBufOutPort(m_pQueue, nOffset, 4); };

inline VOID COpQueue::QueueTempOutp(DWORD nOffset)
	{ HwQueueTempOutPort(m_pQueue, nOffset, 1); };

inline VOID COpQueue::QueueTempOutpw(DWORD nOffset)
	{ HwQueueTempOutPort(m_pQueue, nOffset, 2); };

inline VOID COpQueue::QueueTempOutpl(DWORD nOffset)
	{ HwQueueTempOutPort(m_pQueue, nOffset, 4); };

inline PVOID COpQueue::GetQueueBuffer()
	{ return HwGetQueueBuffer(m_pQueue); };

inline VOID COpQueue::QueueWhileInpAnd(DWORD nOffset, USHORT nDataSize, 
		DWORD nMask, BOOL bLoopWhile, DWORD nLimit)
	{ HwQueueWhileInpAnd(m_pQueue, nOffset, nDataSize, nMask,
			bLoopWhile, nLimit); };

inline VOID COpQueue::QueueWhileInpEqual(DWORD nOffset, USHORT nDataSize, 
		DWORD nMask, DWORD nValue, BOOL bLoopWhile, DWORD nLimit)
	{ HwQueueWhileInpEqual(m_pQueue, nOffset, nDataSize, nMask, nValue,
			bLoopWhile, nLimit); };

inline VOID COpQueue::QueueLoopBack()
	{ HwQueueLoopBack(m_pQueue); };

inline VOID COpQueue::QueueBreakIfAnd(DWORD nMask, BOOL bBreak)
	{ HwQueueBreakIfAnd(m_pQueue, nMask, bBreak); };

inline VOID COpQueue::QueueOrTemp(DWORD nMask)
	{ HwQueueOrTemp(m_pQueue, nMask); };

inline VOID COpQueue::QueueAndTemp(DWORD nMask)
	{ HwQueueAndTemp(m_pQueue, nMask); };

/////////////////// CDevice inlines

inline CDevice::CDevice()
{ 
	CommonConstructDevice(this, IsrCallback); 
	pExtension = this;
};

inline CDevice::~CDevice()
	{ CommonDestroyDevice(this); };

inline BOOL CDevice::ConnectDevice(LPCSTR pszName)
	{ return HwConnectDevice(this, pszName); }

inline void CDevice::DisconnectDevice()
	{ HwDisconnectDevice(this); }

inline BOOL CDevice::IsConnected()
	{ return (hDevice == INVALID_HANDLE_VALUE) ? FALSE : TRUE; };

inline BOOL CDevice::MapPorts(DWORD nBase, DWORD nCount, PDWORD pnIndex)
	{ return HwMapPorts(this, nBase, nCount, pnIndex); };

inline BOOL CDevice::MapPortsEx(DWORD nBase, DWORD nCount, 
				DWORD fInterfaceType, DWORD nBusNumber, PDWORD pnIndex)
	{ return HwMapPortsEx(this, nBase, nCount, fInterfaceType, 
				nBusNumber, pnIndex); };

inline BOOL CDevice::UnmapPorts(DWORD nIndex)
	{ return HwUnmapPorts(this, nIndex); };

inline BOOL CDevice::OutPort(DWORD nIndex, DWORD nOffset, DWORD nDataSize, DWORD nData)
	{ return HwOutPort(this, nIndex, nOffset, nDataSize, nData); };

inline BOOL	CDevice::outp(DWORD nOffset, BYTE chValue)
	{ return HwOutp(this, nOffset, chValue); };

inline BOOL	CDevice::outpw(DWORD nOffset, WORD nValue)
	{ return HwOutpw(this, nOffset, nValue); };

inline BOOL	CDevice::outpl(DWORD nOffset, DWORD nValue)
	{ return HwOutpl(this, nOffset, nValue); };

inline DWORD CDevice::InPort(DWORD nIndex, DWORD nOffset, DWORD nDataSize)
	{ return HwInPort(this, nIndex, nOffset, nDataSize); };

inline BYTE	CDevice::inp(DWORD nOffset)
	{ return HwInp(this, nOffset); };

inline WORD	CDevice::inpw(DWORD nOffset)
	{ return HwInpw(this, nOffset); };

inline DWORD CDevice::inpl(DWORD nOffset)
	{ return HwInpl(this, nOffset); };

inline BOOL CDevice::AutoIncReadPort(DWORD nIndex, DWORD nOffset, VOID* pBuf, 
		DWORD nCount, DWORD nDataSize)
	{ return HwAutoIncReadPort(this, nIndex, nOffset, 
		pBuf, nCount, nDataSize); };

inline BOOL CDevice::AutoIncWritePort(DWORD nIndex, DWORD nOffset, VOID* pBuf, 
			DWORD nCount, DWORD nDataSize)
	{ return HwAutoIncWritePort(this, nIndex, nOffset,
		pBuf, nCount, nDataSize); };

inline BOOL CDevice::ConnectInterrupt(DWORD nLevel, DWORD nVector,
	DWORD fInterfaceType, DWORD nBusNumber)
{
	return HwConnectInterruptEx(this, nLevel, nVector, 
		fInterfaceType, nBusNumber);
}

inline BOOL CDevice::DisconnectInterrupt()
	{ return HwDisconnectInterrupt(this); }; 

inline BOOL CDevice::EnableIsr(HANDLE hThread, DWORD nID)
	{ return HwEnableIsr(this, hThread, nID); };

inline VOID CDevice::BeginIsrSynch()
	{ HwBeginIsrSynch(this); };

inline VOID CDevice::EndIsrSynch()
	{ HwEndIsrSynch(this); };

inline DWORD CDevice::GetIsrData()
	{ return HwGetIsrData(this); };

inline DWORD CDevice::GetIsrIdData()
	{ return HwGetIsrIdData(this); };

inline BOOL CDevice::IsIsrEnabled()
	{ return hIsrThread ? TRUE : FALSE; };

inline VOID CDevice::DisableIsr()
	{ HwDisableIsr(this); };

inline BOOL CDevice::Execute(COpQueue* pQueue)
{	
//	ASSERT(pQueue);
	return HwExecute(this, pQueue->m_pQueue); 
};

inline BOOL CDevice::IsDriverConfigured() 
	{ return HwIsDriverConfigured(); };

inline BOOL CDevice::ConfigureDriver()
	{ return HwConfigureDriver(); };

inline BOOL CDevice::DeleteDeviceConfiguration(LPCTSTR pszName)
	{ return HwDeleteDeviceConfiguration(pszName); }

inline BOOL CDevice::ConfigureDevice(LPCTSTR pszName, CDeviceConfig& config)
	{ return HwConfigureDeviceEx(pszName, &config); };

inline BOOL CDevice::EnumerateDevices(DWORD nIndex, LPTSTR pszBuffer,
				CDeviceConfig* pConfig)
	{ return HwEnumerateDevicesEx(nIndex, pszBuffer, pConfig); };

inline BOOL CDevice::GetDeviceConfig(LPCTSTR pszName, CDeviceConfig* pConfig)
	{ return HwGetDeviceConfigEx(pszName, pConfig); };

inline BOOL CDevice::StartDevice(LPCTSTR pszName)
	{ return HwStartDevice(pszName); };

inline BOOL CDevice::StopDevice(LPCTSTR pszName)
	{ return HwStopDevice(pszName); };

inline BOOL CDevice::IsDeviceRunning(LPCTSTR pszName)
	{ return HwIsDeviceRunning(pszName); };

inline BOOL CDevice::GetProperty(DWORD nCode, PDWORD pnState)
	{ return HwGetProperty(this, nCode, pnState); };

inline BOOL CDevice::SetProperty(DWORD nCode, DWORD nValue)
	{ return HwSetProperty(this, nCode, nValue); };

inline VOID CDevice::SetErrorHandler(VOID (*pErrorHandler)(HWDEVICE*,DWORD))
	{ HwSetErrorHandler(this, pErrorHandler); };

inline BOOL CDevice::SetBusDataByOffset(ULONG nOffset, PVOID pBuffer, ULONG cbBuffer)
	{ return HwSetBusDataByOffset(this, nOffset, pBuffer, cbBuffer); };

inline BOOL CDevice::GetBusDataByOffset(ULONG nOffset, PVOID pBuffer, ULONG cbBuffer)
	{ return HwGetBusDataByOffset(this, nOffset, pBuffer, cbBuffer); };

///////////////////////////////////////////////////////////////////////////
// CMemory inlines

inline CMemBase::CMemBase()
	{ CommonConstructMemory(this); };

inline CMemBase::~CMemBase()
	{ UnmapMemory(); };

inline BOOL CMemBase::MapMemory(DWORD nBaseAddress, DWORD nCount,
				DWORD fInterface, DWORD nBus)
	{ return HwMapMemoryEx(this, nBaseAddress, nCount, fInterface, nBus); };

inline BOOL CMemBase::MapDeviceMemory(CDevice* pDevice)
	{ return HwMapDeviceMemory(this, pDevice); };

inline BOOL CMemBase::MapDmaBuffer(CDevice* pDevice)
	{ return HwMapDmaBuffer(this, pDevice); };

inline BOOL CMemBase::UnmapMemory()
	{ return HwUnmapMemory(this); };

inline PVOID CMemBase::GetBase()
	{ return (PVOID)(output.nVirtualAddress); };

inline PBYTE CMemBase::GetMemoryPointer()
	{ return (PBYTE)(output.nVirtualAddress); };

inline DWORD CMemBase::GetPhysicalAddress()
	{ return HwGetPhysicalAddress(this); };

inline DWORD CMemBase::GetSize()
	{ return output.nCount; };

inline DWORD CMemBase::GetMemoryIndex()
	{ return HwGetMemoryIndex(this); };

inline void CMemBase::SetMemoryIndex(DWORD nIndex)
	{ HwSetMemoryIndex(this, nIndex); };

inline BYTE& CMemory::operator[](int nIndex)
	{ return(((BYTE*)output.nVirtualAddress)[nIndex]); };

inline CMemory::operator BYTE*()
	{ return (BYTE*)output.nVirtualAddress; };

inline WORD& CWordMemory::operator[](int nIndex)
	{ return(((WORD*)output.nVirtualAddress)[nIndex]); };

inline CWordMemory::operator WORD*()
	{ return (WORD*)output.nVirtualAddress; };

inline DWORD& CLongMemory::operator[](int nIndex)
	{ return(((DWORD*)output.nVirtualAddress)[nIndex]); };

inline CLongMemory::operator DWORD*()
	{ return (DWORD*)output.nVirtualAddress; };

#endif // ifndef DXCLASS_H

