#pragma once
#include <Windows.h>

namespace rage { class ConstString; }

namespace iv_pc_wtd {
	void processWTD(rage::ConstString& pszFilePath);
	void mergeWtdsToWmtd(DWORD dwCount, rage::ConstString* pszWtdFiles);
	void wtdFromDdsFiles(rage::ConstString &pszOutFile, DWORD dwCount, rage::ConstString* ppszDdsPath);

};