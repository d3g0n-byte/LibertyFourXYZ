#pragma once

namespace libertyFourXYZ {
	extern bool g_bForceUseRsc5;
	extern bool g_bMergeRscPages;
	extern BYTE g_nbMaxPageSizeForMerge;
	extern bool g_bUsePageMap;

	void readSettings();
}