#include "rage_base.h"

#include "memory_manager.h"
#include "settings.h"

namespace rage {

	pgBase::pgBase() {
		this->m_pPageMap = NULL;
	}

	void pgBasePageMap::fillMap(rage::datResource* rsc) {
		this->nbVCount = rsc->m_pMap->nbVirtualCount;
		this->nbPCount = rsc->m_pMap->nbPhysicalCount;
		memset(this->__2, 0x0, 2);
		for (BYTE i = 0; i < this->nbVCount + this->nbPCount; i++)
			this->dwData[i] = rsc->m_pMap->chunks[i].pSrc;

	}

	void pgBase::allocatePageMap() { this->m_pPageMap = new ("allocatePageMap") rage::pgBasePageMap(); }

	void pgBase::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		if (dwDepth || !libertyFourXYZ::g_bUsePageMap) {
			if (this->m_pPageMap) dealloc(this->m_pPageMap);
		}
		else {
			this->m_pPageMap->fillMap(pRsc);
			pgBasePageMap* pPageMap = this->m_pPageMap;
			pLayout->setPtr<pgBasePageMap>(this->m_pPageMap);
			memcpy(pRsc->getFixup<pgBasePageMap>(this->m_pPageMap, -1), pPageMap, 4 + (pRsc->m_pMap->nbVirtualCount + pRsc->m_pMap->nbPhysicalCount) * sizeof(size_t));
		}
	}

	void pgBase::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		if (dwDepth) {
			if (this->m_pPageMap) dealloc(this->m_pPageMap);
		}
		else {
			if (!this->m_pPageMap) this->allocatePageMap();
			pLayout->addObject<pgBasePageMap>(this->m_pPageMap, 5);
		}
		//else if (this->m_pPageMap)
		//	pLayout->addObject<pgBasePageMap>(this->m_pPageMap, 5);

	}
	
	void pgBase::place(rage::datResource* rsc) {
		this->m_pPageMap = NULL;
	}

	void pgBase::clearRefCount() {
		datBase::clearRefCount();
	}

	void pgBase::setRefCount() {
		datBase::setRefCount();
	}

	pgBase::~pgBase() {
		if (this->m_pPageMap)
			dealloc(this->m_pPageMap);
	}

	void pgBaseRefCounted::clearRefCount() {
		pgBase::clearRefCount();
		this->m_usageCount = 0;
	}

	void pgBaseRefCounted::setRefCount() {
		pgBase::setRefCount();
		this->m_usageCount++;
	}

}