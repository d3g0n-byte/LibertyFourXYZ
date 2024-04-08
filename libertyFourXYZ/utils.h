#pragma once
#include <Windows.h>
#include <type_traits>

DWORD alignValue(DWORD dwValue, DWORD dwAlignTo);
bool allTrue(bool* pbBools, DWORD dwCount);
DWORD atStringHash(const char* pszString, DWORD dwBase = 0);
void setConsoleTextColor(WORD wColor);
unsigned __int64 setBits(unsigned __int64 qwValue, DWORD dwPos, unsigned __int64 qwBitsValue, DWORD dwValSize);
DWORD crc_z(DWORD dwBase, BYTE *pData, size_t size);

template<typename T> void copy_class(T* pDst, T* pSrc, size_t count = 1,size_t size = sizeof(T)) {
	if (std::has_virtual_destructor<T>::value) for (size_t i = 0; i < count; i++) memcpy((size_t*)(pDst + i) + 1, (size_t*)(pSrc + i)+ 1, size - sizeof(size_t));
	else for (size_t i = 0; i < count; i++) memcpy(pDst + i, pSrc + i, size);
}

template<typename T> void zero_fill_class(T* pMem, size_t size = sizeof(T)) {
	if (std::has_virtual_destructor<T>::value) memset((size_t*)pMem + 1, 0x0, size - sizeof(size_t));
	else memset(pMem, 0x0, size);
}

