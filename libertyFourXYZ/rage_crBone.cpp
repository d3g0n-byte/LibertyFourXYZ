#include "rage_crBone.h"

#include "memory_manager.h"
#include "utils.h"
#include "rage_datResource.h"

namespace rage {

	void crBoneData::place(rage::datResource* rsc) {
		this->m_pszName.place(rsc);
		
		this->m_pJointData = NULL;
		this->m_pNext = NULL;
		this->m_pParent = NULL;
		this->m_pChild = NULL;

		this->m_dwNameHash = atStringHash(this->m_pszName, 0);

	}

	crBoneData& crBoneData::operator=(crBoneData& bone) {
		this->m_pszName = bone.m_pszName;
		this->m_dwFlags = bone.m_dwFlags;
		memcpy(&this->m_wIndex, &bone.m_wIndex, sizeof(crBoneData) - offsetof(crBoneData, m_wIndex));

		return *this;
	}

	void crBoneData::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		this->m_pszName.addToLayout(pLayout, dwDepth);
	}

	void crBoneData::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		this->m_pszName.replacePtrs(pLayout, pRsc, dwDepth);
	}


}