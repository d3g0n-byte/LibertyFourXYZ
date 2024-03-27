#pragma once
#include "Windows.h"

namespace rage { class ConstString; class grcTexturePC; }

// wtd v11 is only for the fusion fix and is not supported like the others
namespace mp3_pc_wtd {
	void wtdFromDdsFiles(rage::ConstString& pszOutFile, DWORD dwCount, rage::ConstString* ppszDdsPath);
}