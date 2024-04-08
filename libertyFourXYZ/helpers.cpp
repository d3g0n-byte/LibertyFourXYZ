#include "helpers.h"
#include "rsc85_layout.h"
#include "rage_array.h"

int helpers::__::_addObjectWithCounterToLayout(libertyFourXYZ::rsc85_layout* pLayout, size_t size, char blockType, void* pObj) {
	if (!pLayout->isAddedToLayoutObj(pObj)) {
		pLayout->addAddedToLayoutObj(pObj);
		pLayout->addObject((BYTE*)pObj, blockType, size);
		return 0;
	}
	else
		return 1;

}

int helpers::__::_replacePtrsInObjWithCounter(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc,size_t depth, void* pObj) {
	if (!pLayout->isReplacedPtrsObj(pObj)) {
		pLayout->addReplacedPtrsObj(pObj);
		return 0;
	}
	else
		return 1;

}

void* helpers::__::_replacePtrsInObjWithCounter2(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc,size_t size, void** pObj) {
	auto origData = *pObj;
	pLayout->setPtr((BYTE*&)*pObj);

	memcpy(pRsc->getFixup(*pObj, size), origData, size);

	return NULL;
	//return pObj;

}

void* helpers::__::_replacePtrsInObjWithCounter3(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc,size_t size, void** pObj) {
	pLayout->setPtr((BYTE*&)*pObj);
	return NULL;
}
