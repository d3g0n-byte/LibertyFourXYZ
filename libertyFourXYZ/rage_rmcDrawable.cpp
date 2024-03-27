#include "rage_rmcDrawable.h"
#include "rage_grmShader.h"
#include "rage_crSkeleton.h"

#include "rage_base.h"
#include "rage_datResource.h"
#include "rage_array.h"
#include "rage_light.h"

namespace rage {
	void rmcDrawableBase::place(datResource* pRsc) {
		pgBase::place(pRsc);

		if (this->m_pShaderGroup) {
			auto realShaders = pRsc->getFixup(this->m_pShaderGroup, sizeof(*this->m_pShaderGroup));
			this->m_pShaderGroup = libertyFourXYZ::g_memory_manager.allocate<rage::grmShaderGroup>("drawableBase, place, shaders" );
			copy_class(this->m_pShaderGroup, realShaders);
			this->m_pShaderGroup->place(pRsc);
		}
		else
			trace("WARNING: resource '%s' without shader group", pRsc->m_pszDebugName);

	}

	rmcDrawableBase::rmcDrawableBase() : m_pShaderGroup(NULL) { }

	rmcDrawableBase::~rmcDrawableBase() {
		if (this->m_pShaderGroup)
			libertyFourXYZ::g_memory_manager.release(this->m_pShaderGroup);
	}

	void rmcDrawableBase::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		pgBase::addToLayout(pLayout, dwDepth);

		pLayout->addObject(this->m_pShaderGroup, 5);
		this->m_pShaderGroup->addToLayout(pLayout, dwDepth + 1);
	}

	void rmcDrawableBase::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		pgBase::replacePtrs(pLayout, pRsc, dwDepth);

		if (this->m_pShaderGroup) {
			this->m_pShaderGroup->replacePtrs(pLayout, pRsc, dwDepth + 1);
			auto origData = this->m_pShaderGroup;
			pLayout->setPtr(this->m_pShaderGroup);
			DWORD dwSize = sizeof * this->m_pShaderGroup;
			memcpy(pRsc->getFixup(this->m_pShaderGroup, dwSize), origData, dwSize);
		}
	}

	void rmcDrawableBase::clearRefCount() {
		pgBase::clearRefCount();
		if (this->m_pShaderGroup)
			this->m_pShaderGroup->clearRefCount();
	}

	void rmcDrawableBase::setRefCount() {
		pgBase::setRefCount();
		if (this->m_pShaderGroup)
			this->m_pShaderGroup->setRefCount();
	}


	void rmcDrawable::place(datResource* pRsc) {
		rmcDrawableBase::place(pRsc);
		if (this->m_pSkeleton) {
			auto realSkel = pRsc->getFixup(this->m_pSkeleton, sizeof(*this->m_pSkeleton));
			this->m_pSkeleton = libertyFourXYZ::g_memory_manager.allocate<rage::crSkeletonData>("drawable, place, skel");
			copy_class(this->m_pSkeleton, realSkel);
			this->m_pSkeleton->place(pRsc);
		}
		this->m_lodgroup.place(pRsc);
	}

	rmcDrawable::rmcDrawable() : m_pSkeleton(NULL) { }
	
	rmcDrawable::~rmcDrawable() {
		if(this->m_pSkeleton)
			libertyFourXYZ::g_memory_manager.release(this->m_pSkeleton);
		for (BYTE i = 0; i < 4; i++)
			this->m_lodgroup.m_lod[i].~datOwner(); // ToDo: check
	}

	void rmcDrawable::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		rmcDrawableBase::addToLayout(pLayout, dwDepth);

		if (this->m_pSkeleton) {
			pLayout->addObject(this->m_pSkeleton, 5);
			this->m_pSkeleton->addToLayout(pLayout, dwDepth + 1);
		}

		this->m_lodgroup.addToLayout(pLayout, dwDepth);
	}

	void rmcDrawable::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		rmcDrawableBase::replacePtrs(pLayout, pRsc, dwDepth);

		if (this->m_pSkeleton) {
			this->m_pSkeleton->replacePtrs(pLayout, pRsc, dwDepth);
			auto origData = this->m_pSkeleton;
			pLayout->setPtr(this->m_pSkeleton);
			DWORD dwSize = sizeof * this->m_pSkeleton;
			memcpy(pRsc->getFixup(this->m_pSkeleton, dwSize), origData, dwSize);
		}
		this->m_lodgroup.replacePtrs(pLayout, pRsc, dwDepth);
	}

	void rmcDrawable::clearRefCount() {
		rmcDrawableBase::clearRefCount();
		if (this->m_pSkeleton)
			this->m_pSkeleton->clearRefCount();
		this->m_lodgroup.clearRefCount();
	}

	void rmcDrawable::setRefCount() {
		rmcDrawableBase::setRefCount();
		if (this->m_pSkeleton)
			this->m_pSkeleton->setRefCount();
		this->m_lodgroup.setRefCount();
	}
}

void gtaDrawable::place(rage::datResource* pRsc) {
	rage::rmcDrawable::place(pRsc);
	if(this->m_light.m_count)
		this->m_light.place(pRsc);
}

gtaDrawable::gtaDrawable() { }
gtaDrawable::~gtaDrawable() { }

void gtaDrawable::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
	rmcDrawable::addToLayout(pLayout, dwDepth);
	if(this->m_light.m_count)
		this->m_light.addToLayout(pLayout, dwDepth);
}

void gtaDrawable::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
	rage::rmcDrawable::replacePtrs(pLayout, pRsc, dwDepth);
	if (this->m_light.m_count)
		this->m_light.replacePtrs(pLayout, pRsc, dwDepth);
}

void gtaDrawable::clearRefCount() {
	rage::rmcDrawable::clearRefCount();
	this->m_light.clearRefCount();
}

void gtaDrawable::setRefCount() {
	rage::rmcDrawable::setRefCount();
	this->m_light.setRefCount();
}
