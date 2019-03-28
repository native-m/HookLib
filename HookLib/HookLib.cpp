#include "HookLib.h"
#include <Windows.h>

void HookLib::Initialize()
{
	if(!m_bInitialized)
	{
		m_pHookTable = new std::map<uint8_t*, uint8_t*>;
		m_bInitialized = true;
	}
}

uint8_t* HookLib::DetourFunction(uint8_t* const pSrc, const uint8_t* pDest, int nLength)
{
	uint8_t* jump;
	DWORD dwVirtualProtectBackup, dwJumpBackup;

	if (nLength < 5)
		return nullptr;

	if (!VirtualProtect(pSrc, nLength, PAGE_READWRITE, &dwVirtualProtectBackup))
		return nullptr;

	jump = new uint8_t[nLength + 5];
	(*m_pHookTable)[pSrc] = jump;

	memcpy_s(jump, nLength + 5, pSrc, nLength);
	jump += nLength;

	jump[0] = 0xE9;
	*(uintptr_t*)(jump + 1) = (uintptr_t)(pSrc + nLength - jump) - 5;

	pSrc[0] = 0xE9;
	*(uintptr_t*)(pSrc + 1) = (uintptr_t)(pDest - pSrc) - 5;

	if (!VirtualProtect(pSrc, nLength, dwVirtualProtectBackup, &dwVirtualProtectBackup))
		return nullptr;

	if (!VirtualProtect(jump, (nLength + 5) * sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &dwJumpBackup))
		return nullptr;

	return jump - nLength;
}

uint8_t* HookLib::DetourMemory(uintptr_t uptrTargetAddr, void * pFuncAddr, int nLength)
{
	void* pTargetAddr = (void*)uptrTargetAddr;

	DWORD oldMemProtect;
	DWORD memProtect;
	uintptr_t relativeAddr;

	if (nLength < 5)
		return nullptr;

	if (!VirtualProtect(pTargetAddr, nLength, PAGE_EXECUTE_READWRITE, &oldMemProtect))
		return nullptr;

	memset(pTargetAddr, 0x90, nLength);
	relativeAddr = ((uintptr_t)pFuncAddr - (uintptr_t)uptrTargetAddr) - 5;

	*(uint8_t*)pTargetAddr = 0xE9;
	*(uintptr_t*)((uintptr_t)pTargetAddr + 1) = relativeAddr;

	if (!VirtualProtect(pTargetAddr, nLength, oldMemProtect, &memProtect))
		return nullptr;

	return (uint8_t*)(uptrTargetAddr + nLength);
}

void HookLib::Shutdown()
{
	DeleteAllHook();
	delete m_pHookTable;
	m_bInitialized = false;
}

int HookLib::CheckExistingHook()
{
	return 0;
}

void HookLib::DeleteAllHook()
{
	for (const auto& hook : *m_pHookTable)
		delete hook.second;
}

void HookLib::Utils::GetVMTPointer(const void* pObject, uintptr_t*& ppOutVMTPtr)
{
	ppOutVMTPtr = *((uintptr_t**)pObject);
}

void HookLib::Utils::GetVMTVector(const void* pObject, int num, std::vector<uintptr_t>& outVMTVector)
{
	uintptr_t* vmtPtr;

	GetVMTPointer(pObject, vmtPtr);
	outVMTVector.clear();

	for (int i = 0; i < num; i++)
		outVMTVector.push_back(vmtPtr[i]);
}

std::map<uint8_t*, uint8_t*>* HookLib::m_pHookTable = nullptr;
bool HookLib::m_bInitialized = false;