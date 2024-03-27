#include "rage_rmcLod.h"

#include "rage_grmModel.h"

namespace rage {

	rmcLod::rmcLod() {};
	rmcLod::~rmcLod() {};

	void rmcLod::place(rage::datResource* rsc) {
		//auto tmp = rsc->getFixup<rage::rmcLod>(ptr);
		//memcpy(this, tmp, sizeof * this);

		if (this->m_models.m_count)
			this->m_models.place(rsc);
	}

	void rmcLod::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		if (this->m_models.m_count)
			this->m_models.addToLayout(pLayout, dwDepth);
	}
	
	void rmcLod::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		if (this->m_models.m_count)
			this->m_models.replacePtrs(pLayout, pRsc, dwDepth);
	}
	void rmcLod::clearRefCount() {
		if (this->m_models.m_count)
			this->m_models.clearRefCount();
	}
	void rmcLod::setRefCount() {
		if (this->m_models.m_count)
			this->m_models.setRefCount();
	}

	rmcLodGroup::rmcLodGroup() { 
		memset(&this->m_boundingBox, 0x0, sizeof this->m_boundingBox);
		memset(&this->m_vCenter, 0x0, sizeof this->m_vCenter);
		memset(&this->m_dwBucketMask, 0xff, 0x10); // -1
		memset(&this->m_fLodThresh, 0x0, 0x10);
		memset(&this->m_fRadius, 0x0, sizeof this->m_fRadius);
		memset(&this->__64, 0x0, 0xc);
	}

	rmcLodGroup::~rmcLodGroup() {};

	//rmcLodGroup& rmcLodGroup::operator=(rmcLodGroup& rLodGruoup) {
	//	this->m_vCenter = rLodGruoup.m_vCenter;
	//	this->m_boundingBox = rLodGruoup.m_boundingBox;
	//	this->m_lod = rLodGruoup.m_lod;
	//	this->m_fLodThresh = rLodGruoup.m_fLodThresh;
	//	this->m_dwBucketMask = rLodGruoup.m_dwBucketMask;
	//	return *this;
	//}

	// в нем сам lodGroup вже будет прочитан, но надо инициализировать элементы. Поинтеры для фикса будут из него
	void rmcLodGroup::place(rage::datResource* rsc) {
		for (BYTE i = 0; i < 4; i++) {
			if (this->m_lod[i].pElement)
				this->m_lod[i].place(rsc);
			//rage::datOwner<rage::rmcLod>* realPtr1 = rsc->getFixup<rage::datOwner<rage::rmcLod>>(this->m_lod.get(i));
		}
	}

	void rmcLodGroup::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		for (BYTE i = 0; i < 4; i++)
			if (this->m_lod[i].pElement)
				this->m_lod[i].addToLayout(pLayout, dwDepth);
	}

	void rmcLodGroup::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		for (BYTE i = 0; i < 4; i++)
			if (this->m_lod[i].pElement)
				this->m_lod[i].replacePtrs(pLayout, pRsc, dwDepth);

	}

	void rmcLodGroup::clearRefCount() {
		for (BYTE i = 0; i < 4; i++)
			if (this->m_lod[i].pElement)
				this->m_lod[i].clearRefCount();
	}

	void rmcLodGroup::setRefCount() {
		for (BYTE i = 0; i < 4; i++)
			if (this->m_lod[i].pElement)
				this->m_lod[i].setRefCount();
	}

}