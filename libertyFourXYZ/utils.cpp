#include "utils.h"

#include "rage_string.h"
#include <filesystem>
#include <bitset>

DWORD alignValue(DWORD dwValue, DWORD dwAlignTo) {
	if (dwAlignTo)
		return (dwValue + dwAlignTo - 1) & ~(dwAlignTo - 1);
	else return dwValue;
}

bool allTrue(bool* pbBools, DWORD dwCount) {
	for (DWORD i = 0; i < dwCount; i++)
		if (!pbBools[i])
			return 0;
	return 1;
}

DWORD atStringHash(const char* pszString, DWORD dwBase) { // from rdr
	DWORD hash = dwBase;
	if (!pszString)
		return 0;

	char terminator = '\0'; // string terminator
	if (*pszString == '"') {    // skip quotes, and set terminating character
		terminator = '"';
		pszString++;
	}

	while (*pszString != terminator) {
		char c = *pszString;
		pszString++;
		if (c >= 'A' && c <= 'Z') c += 0x20; else if ('\\' == c) c = '/';   // case and slash convert
		hash += c;
		hash *= 0x401; // hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash *= 9;      // hash += hash << 3;
	hash ^= hash >> 11;
	hash *= 0x8001; // hash += hash << 15;
	return hash;
}

void setConsoleTextColor(WORD wColor = 0) {
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (wColor) {
	case 1:
		SetConsoleTextAttribute(hStdOut, FOREGROUND_RED);
		break;
	case 2:
		SetConsoleTextAttribute(hStdOut, FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	default:
		SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY);
		break;
	}
}

void getFilesPathFromFolder(rage::ConstString* ppszOutPath, DWORD* pCount, const char* pszExt) {

}

unsigned __int64 setBits(unsigned __int64 qwValue, DWORD dwPos, unsigned __int64 qwBitsValue, DWORD dwValSize) {
	std::bitset<64> b(qwValue);
	for (BYTE a = dwPos; a < dwPos + dwValSize; a++) b[a] = 0;
	return b.to_ullong() | ((qwBitsValue << (64 - dwValSize)) >> (64 - dwValSize)) << dwPos;
}

