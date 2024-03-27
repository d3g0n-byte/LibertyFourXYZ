#pragma once
#include <Windows.h>
#include <type_traits>

DWORD alignValue(DWORD dwValue, DWORD dwAlignTo);
bool allTrue(bool* pbBools, DWORD dwCount);
DWORD atStringHash(const char* pszString, DWORD dwBase);
void setConsoleTextColor(WORD wColor);
unsigned __int64 setBits(unsigned __int64 qwValue, DWORD dwPos, unsigned __int64 qwBitsValue, DWORD dwValSize);

template<typename T> void copy_class(T* pDst, T* pSrc, size_t count = 1,size_t size = sizeof(T)) {
	if (std::has_virtual_destructor<T>::value) for (size_t i = 0; i < count; i++) memcpy((size_t*)(pDst + i) + 1, (size_t*)(pSrc + i)+ 1, size - sizeof(size_t));
	else for (size_t i = 0; i < count; i++) memcpy(pDst + i, pSrc + i, size);
}

template<typename T> void zero_fill_class(T* pMem, size_t size = sizeof(T)) {
	if (std::has_virtual_destructor<T>::value) memset((size_t*)pMem + 1, 0x0, size - sizeof(size_t));
	else memset(pMem, 0x0, size);
}

namespace helpers {
	template<typename T, typename = void>
	struct _hasPlace : std::false_type {};
	template<typename T>
	struct _hasPlace<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().place(NULL)), void>::value>> : std::true_type {};
	template<typename T>
	constexpr bool hasPlace = _hasPlace<T>::value;

	template<typename T, typename = void>
	struct _has_destructor : std::false_type {};
	template<typename T>
	struct _has_destructor<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().~T()), void>::value>> : std::true_type {};
	template<typename T>
	constexpr bool hasDestructor = _has_destructor<T>::value;

	template<typename T, typename = void>
	struct _has_addToLayout : std::false_type {};
	template<typename T>
	struct _has_addToLayout<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().addToLayout(NULL, 0)), void>::value>> : std::true_type {};
	template<typename T>
	constexpr bool hasAddToLayout = _has_addToLayout<T>::value;

	template<typename T, typename = void>
	struct _has_replacePtrs : std::false_type {};
	template<typename T>
	struct _has_replacePtrs<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().replacePtrs(NULL, NULL, 0)), void>::value>> : std::true_type {};
	template<typename T>
	constexpr bool hasReplacePtrs = _has_replacePtrs<T>::value;

	template<typename T, typename = void>
	struct _has_clearRefCount : std::false_type {};
	template<typename T>
	struct _has_clearRefCount<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().clearRefCount()), void>::value>> : std::true_type {};
	template<typename T>
	constexpr bool hasClearRefCount = _has_clearRefCount<T>::value;

	template<typename T, typename = void>
	struct _hasSetRefCount : std::false_type {};
	template<typename T>
	struct _hasSetRefCount<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().setRefCount()), void>::value>> : std::true_type {};
	template<typename T>
	constexpr bool hasSetRefCount = _hasSetRefCount<T>::value;


}
