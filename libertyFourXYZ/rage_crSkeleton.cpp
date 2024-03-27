#include "rage_crSkeleton.h"

#include "memory_manager.h"
#include "utils.h"
#include "rage_datResource.h"
#include "rage_math.h"
#include "rage_crBone.h"

const int g_addNumBonesToBones = 1;

namespace rage {

	float crSkeletonData::calcAuthoredOffset(int nBoneId, rage::Vector3& a3) {
		rage::Matrix34* v3 = &this->m_pBoneLocalTransforms[nBoneId];
		rage::Vector2 v4; v4.x = v3->d.x; v4.y = v3->d.y;
		float result = v3->d.z;
		a3.x = v4.x; a3.y = v4.y; //*(__int64*)&a3.x = *(__int64*)&v4
		a3.z = result;
		a3.w = v3->d.w;

		//if (nBoneId >= 1 && !this->m_dwFlags.bAuthoredOrientation) {
		//    rage::Matrix34* m_pCumulativeInverseJointScaleOrients = this->m_pBoneWorldOrientInverted.pElement;
		//    rage::Vector2 *v7 = (rage::Vector2*)&m_pCumulativeInverseJointScaleOrients[this->m_pParentBoneIndices[nBoneId]];
		//    if (!m_pCumulativeInverseJointScaleOrients) // if(!this->dwFlags.bHaveBoneWorldOrient)
		//        v7 = (float32x2_t*)0x714F190520LL;
		//    v8 = vadd_f32(
		//        v7[6],
		//        vadd_f32(
		//            vadd_f32(vmul_n_f32((float32x2_t)v7->n64_u64[0], v4.n64_f32[0]), vmul_lane_f32(v7[2], v4, 1)),
		//            vmul_n_f32(v7[4], result))).n64_u64[0];
		//    v9 = (float)(vmuls_n_f32(v7[1].n64_f32[0], v4.n64_f32[0]) + vmuls_lane_f32(v7[3].n64_f32[0], v4, 1))
		//        + (float)(result * v7[5].n64_f32[0]);
		//    v10 = v7[7].n64_f32[0];
		//    *(_QWORD*)&a3->x = v8;
		//    result = v10 + v9;
		//    a3->z = result;
		//}

		return 0;

	}

	void crSkeletonData::place(rage::datResource* rsc) {
		//crSkeletonData* realPtr = rsc->getFixup<rage::crSkeletonData>(ptr, sizeof(*this));
		//memcpy(this, realPtr, sizeof(*this));


		//if (!rsc->getFixup<rage::crBoneData>(this->m_pBones.pElement, sizeof(*this->m_pBones.pElement) * this->m_wNumBones))
		//	return;

		crBoneData* pBones = rsc->getFixup<rage::crBoneData>(this->m_pBones.pElement, sizeof(*this->m_pBones.pElement) * this->m_wNumBones);
		this->m_pBones.pElement = libertyFourXYZ::g_memory_manager.allocate<crBoneData>("skel::place, bones", this->m_wNumBones);
		memcpy(this->m_pBones.pElement, pBones, sizeof(*this->m_pBones.pElement) * this->m_wNumBones);
		for (WORD i = 0; i < this->m_wNumBones; i++)
			this->m_pBones[i].place(rsc);
		
		DWORD *pParentIndex = rsc->getFixup(this->m_pParentBoneIndices.pElement, sizeof(DWORD) * this->m_wNumBones);
		this->m_pParentBoneIndices.pElement = libertyFourXYZ::g_memory_manager.allocate<DWORD>("skel::place, parent index", this->m_wNumBones);
		memcpy(this->m_pParentBoneIndices.pElement, pParentIndex, sizeof(*this->m_pParentBoneIndices.pElement) * this->m_wNumBones);

		if (this->bHaveBoneWorldOrient) {
			Matrix34* pOrients = rsc->getFixup(this->m_pBoneWorldOrientInverted.pElement, sizeof(Matrix34) * this->m_wNumBones * 2);
			if ((size_t)this->m_pBoneWorldOrient.pElement - (size_t)this->m_pBoneWorldOrientInverted.pElement != sizeof(Matrix34) * this->m_wNumBones) {
				error("[crSkeletonData] this->bHaveBoneWorldOrient is not valid");
				return;
			}

			this->m_pBoneWorldOrientInverted.pElement = libertyFourXYZ::g_memory_manager.allocate<Matrix34>("skel::place, invert orient", this->m_wNumBones);
			memcpy(this->m_pBoneWorldOrientInverted.pElement, pOrients, sizeof(*this->m_pBoneWorldOrientInverted.pElement) * this->m_wNumBones);

			this->m_pBoneWorldOrient.pElement = libertyFourXYZ::g_memory_manager.allocate<Matrix34>("skel::place, orient", this->m_wNumBones);
			memcpy(this->m_pBoneWorldOrient.pElement, pOrients + this->m_wNumBones, sizeof(*this->m_pBoneWorldOrient.pElement) * this->m_wNumBones);
		}
		else {
			this->m_pBoneWorldOrient.pElement = NULL;
			this->m_pBoneWorldOrientInverted.pElement = NULL;
		}

		Matrix34* pPos = rsc->getFixup(this->m_pBoneLocalTransforms.pElement, sizeof(Matrix34) * this->m_wNumBones);
	
		this->m_pBoneLocalTransforms.pElement = libertyFourXYZ::g_memory_manager.allocate<Matrix34>("skel::place, transform", this->m_wNumBones);
		memcpy(this->m_pBoneLocalTransforms.pElement, pPos, sizeof(*this->m_pBoneLocalTransforms.pElement) * this->m_wNumBones);

		if (bHaveBoneMappings) {
			BoneIdData *pIdIndex = rsc->getFixup(this->m_boneIdMappings.m_pElements, sizeof(BoneIdData) * this->m_wNumBones);
			//this->m_boneIdMappings.m_pElements = libertyFourXYZ::g_memory_manager.allocate<BoneIdData>(this->m_wNumBones);
			this->m_boneIdMappings.place(rsc);
		}
		else
			memset(&this->m_boneIdMappings, 0x0, sizeof(this->m_boneIdMappings));
		
		this->m_pszJointDataFileName.place(rsc);

	}

	crSkeletonData::crSkeletonData() {
		this->m_pBones.pElement = NULL;
		this->m_pParentBoneIndices.pElement = NULL;
		this->m_pBoneWorldOrient.pElement = NULL;
		this->m_pBoneWorldOrientInverted.pElement = NULL;
		this->m_pBoneLocalTransforms.pElement = NULL;
		this->m_wNumBones = 0;
		this->m_wTransLockCount = 0;
		this->m_wRotLockCount = 0;
		this->m_wScaleLockCount = 0;
		this->m_dwFlags = 0;
		this->m_dwUsageCount = 1;
		this->m_dwCrc = 0;
		//this->m_jointData = NULL;
	}

	crSkeletonData::~crSkeletonData() {
		if (this->m_pBones.pElement)
			libertyFourXYZ::g_memory_manager.release(this->m_pBones.pElement);
		if (this->m_pParentBoneIndices.pElement)
			libertyFourXYZ::g_memory_manager.release(this->m_pParentBoneIndices.pElement);
		if (this->bHaveBoneWorldOrient) {
			libertyFourXYZ::g_memory_manager.release(this->m_pBoneWorldOrient.pElement);
			libertyFourXYZ::g_memory_manager.release(this->m_pBoneWorldOrientInverted.pElement);
		}
		if (this->m_pBoneLocalTransforms.pElement)
			libertyFourXYZ::g_memory_manager.release(this->m_pBoneLocalTransforms.pElement);
		this->m_wNumBones = 0;
		this->m_wTransLockCount = 0;
		this->m_wRotLockCount = 0;
		this->m_wScaleLockCount = 0;
		this->m_dwFlags = 0;
		this->m_dwUsageCount = 0;
		this->m_dwCrc = 0;
		//if (this->m_jointData)
		//	libertyFourXYZ::g_memory_manager.release(this->m_jointData);
	}

	void crSkeletonData::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		if (g_addNumBonesToBones)
			pLayout->addObject((BYTE*)this->m_pBones.pElement, 5, this->m_wNumBones * sizeof(*this->m_pBones.pElement) + 0x10);
		else pLayout->addObject(this->m_pBones.pElement, 5, this->m_wNumBones);

		for (WORD i = 0; i < this->m_wNumBones; i++)
			this->m_pBones[i].addToLayout(pLayout, dwDepth + 1);

		pLayout->addObject(this->m_pParentBoneIndices.pElement, 5, this->m_wNumBones);

		if (this->bHaveBoneWorldOrient) {
			pLayout->addObject(this->m_pBoneWorldOrientInverted.pElement, 5, this->m_wNumBones * 2);
		}
		
		pLayout->addObject(this->m_pBoneLocalTransforms.pElement, 5, this->m_wNumBones);
		
		if (this->bHaveBoneMappings)
			this->m_boneIdMappings.addToLayout(pLayout, dwDepth);
		
		
	}
	void crSkeletonData::clearHierarchy() {
		for (WORD i = 0; i < this->m_wNumBones; i++) {
			this->m_pBones[i].m_pParent = NULL;
			this->m_pBones[i].m_pNext = NULL;
			this->m_pBones[i].m_pChild = NULL;
		}
	}

	void crSkeletonData::buildHierarchy() {
		this->clearHierarchy();

		// parent
		for (WORD i = 1; i < this->m_wNumBones; i++)
			this->m_pBones[i].m_pParent = this->m_pBones.pElement + this->m_pParentBoneIndices[i];

		// child
		for (WORD i = 0; i < this->m_wNumBones - 1; i++)
			if(i == this->m_pParentBoneIndices[i + 1])
				this->m_pBones[i].m_pChild = this->m_pBones.pElement + (i + 1);


			//for (WORD j = i + 1; j < this->m_wNumBones; j++)
			//	if (i == this->m_pParentBoneIndices[j]) {
			//		this->m_pBones[i].m_pChild = this->m_pBones.pElement + j;
			//		break;
			//	}

		// next
		for (WORD i = 1; i < this->m_wNumBones; i++)
			for (WORD j = i + 1; j < this->m_wNumBones; j++)
				if (this->m_pParentBoneIndices[j] == this->m_pParentBoneIndices[i]) {
					this->m_pBones[i].m_pNext = this->m_pBones.pElement + j;
					break;
				}
	}

	void crSkeletonData::replacePtrsInHierarchy(libertyFourXYZ::rsc85_layout* pLayout) {
		DWORD ptr = (DWORD)pLayout->getPtr(this->m_pBones.pElement);
		if (g_addNumBonesToBones)
			ptr += 0x10;

		DWORD base = (DWORD)this->m_pBones.pElement;

		for (WORD i = 0; i < this->m_wNumBones; i++) {

			DWORD* pVal = (DWORD*)&this->m_pBones[i].m_pNext;
			for (BYTE i = 0; i < 3; i++, pVal++) {
				if (!*pVal) continue;

				// get offset
				DWORD dwOffset = *pVal - base;

				// replace
				*pVal = ptr + dwOffset;
				
			}

		}

	}

	void crSkeletonData::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {

		this->buildHierarchy();

		for (WORD i = 0; i < this->m_wNumBones; i++)
			this->m_pBones[i].replacePtrs(pLayout, pRsc, dwDepth + 1);
		this->replacePtrsInHierarchy(pLayout);

		auto origData = this->m_pBones.pElement;
		pLayout->setPtr(this->m_pBones.pElement);

		if (g_addNumBonesToBones) {
			*pRsc->getFixup((DWORD*)this->m_pBones.pElement, -1) = this->m_wNumBones;
			this->m_pBones.pElement = (crBoneData*)((BYTE*)this->m_pBones.pElement + 0x10);
		}

		DWORD dwSize = sizeof(*this->m_pBones.pElement) * this->m_wNumBones;
		memcpy(pRsc->getFixup(this->m_pBones.pElement, dwSize), origData, dwSize);

		// ToDo: fix
		{
			auto _old = this->m_pBones.pElement;
			this->m_pBones.pElement = origData;
			this->clearHierarchy();
			this->m_pBones.pElement = _old;
		}

		auto origData2 = this->m_pParentBoneIndices.pElement;
		pLayout->setPtr(this->m_pParentBoneIndices.pElement);
		dwSize = sizeof(*this->m_pParentBoneIndices.pElement) * this->m_wNumBones;
		memcpy(pRsc->getFixup(this->m_pParentBoneIndices.pElement, dwSize), origData2, dwSize);

		if (this->bHaveBoneWorldOrient) {
			auto origData3 = this->m_pBoneWorldOrientInverted.pElement;
			pLayout->setPtr(this->m_pBoneWorldOrientInverted.pElement);
			dwSize = sizeof(*this->m_pBoneWorldOrientInverted.pElement) * this->m_wNumBones;
			memcpy(pRsc->getFixup(this->m_pBoneWorldOrientInverted.pElement, dwSize * 2), origData3, dwSize);

			origData3 = this->m_pBoneWorldOrient.pElement;
			pLayout->backupPtr(this->m_pBoneWorldOrient.pElement);
			this->m_pBoneWorldOrient.pElement = (Matrix34*)((size_t)this->m_pBoneWorldOrientInverted.pElement + dwSize);
			memcpy(pRsc->getFixup(this->m_pBoneWorldOrient.pElement, -1), origData3, dwSize);
		}

		auto origData3 = this->m_pBoneLocalTransforms.pElement;
		pLayout->setPtr(this->m_pBoneLocalTransforms.pElement);
		dwSize = sizeof(*this->m_pBoneLocalTransforms.pElement) * this->m_wNumBones;
		memcpy(pRsc->getFixup(this->m_pBoneLocalTransforms.pElement, dwSize), origData3, dwSize);

		if (this->bHaveBoneMappings)
			this->m_boneIdMappings.replacePtrs(pLayout, pRsc, dwDepth);
	}

	void crSkeletonData::clearRefCount() {
		this->m_dwUsageCount = 0;
	}

	void crSkeletonData::setRefCount() {
		this->m_dwUsageCount++;
	}
}
