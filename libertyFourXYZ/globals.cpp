#include "globals.h"

namespace libertyFourXYZ {

	char g_szExePath[MAX_PATH + 1];

	void initializeGlobals() { GetModuleFileNameA(NULL, g_szExePath, MAX_PATH); }

}