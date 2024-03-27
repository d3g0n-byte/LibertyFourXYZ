#pragma once

//#include "rage_datResource.h"

namespace rage { class datResourceFileHeader; class datResource; union datResourceInfo; }

namespace libertyFourXYZ {

	int readRsc85Resource(const char* pszPath, rage::datResourceFileHeader* pResHeader, rage::datResource* pResource);
	//void readRootStruct(void *pRoot, DWORD dwRootObjSize, rage::datResource* pResource, rage::datResourceInfo* pResInfo);

}