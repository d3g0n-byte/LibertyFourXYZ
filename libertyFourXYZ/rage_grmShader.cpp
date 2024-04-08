#include "rage_grmShader.h"
#include "rage_datResource.h"
#include "rage_grcTexture.h"
#include "memory_manager.h"
#include "rage_math.h"


namespace rage {
	void grcInstanceData::place(datResource* pRsc) {
		auto params = pRsc->getFixup(this->pEntries, this->dwTotalSize);
		auto types = pRsc->getFixup(this->pTypes, -1);
		auto hashes = pRsc->getFixup(this->pEntriesHashes, -1);
		
		this->pEntries = new("instanceData, place, params") Entry [this->dwCount];
		this->pEffect = NULL; // created in game
		this->pTypes = new("instanceData, place, types")BYTE[this->dwCount];
		this->pEntriesHashes = new("instanceData, place, hash") DWORD [this->dwCount];

		memcpy(this->pEntriesHashes, hashes, sizeof(*this->pEntriesHashes) * this->dwCount);
		
		memcpy(this->pTypes, types, sizeof(*this->pTypes) * this->dwCount);

		memcpy(this->pEntries, params, sizeof(*this->pEntries) * this->dwCount);
		for (DWORD i = 0; i < this->dwCount; i++) {
			if (this->pEntries[i].pVec4 == NULL) continue; // skip null params
			void* origParam = pRsc->getFixup(this->pEntries[i].pVec4, -1);

			if (this->pTypes[i] != 0) { // if type not txd then
				if ((size_t)origParam > (size_t)params + this->dwTotalSize) {
					error("[grcInstanceData::place] parameter out of effect. Index '%u'", i);
					return;
				}

				DWORD dwParamSize = 0;
				DWORD dwArraySize = 0;
				switch (this->pTypes[i]) {
				case 8:
					dwArraySize = 6;
					break;
				case 4:
				case 1:
				case 14:
				case 15:
				case 16:
					dwArraySize = this->pTypes[i];
					break;
				default:
					error("[grcInstanceData::place] unk param type '%u'", (DWORD)this->pTypes[i]);
					return;
				}

				if (this->pTypes[i] != 4) { // vectors
					dwParamSize = dwArraySize * sizeof(Vector4);
					this->pEntries[i].pVec4 = new("instanceData, place, vec")Vector4 [dwArraySize];
					memmove(this->pEntries[i].pVec4, origParam, dwParamSize);
				}
				else { // matrix
					this->pEntries[i].pMtx = new("instanceData, place, mtx") Matrix44;
					memmove(this->pEntries[i].pMtx, origParam, sizeof(Matrix44));
				}
			}
			else {
				BYTE type = ((grcTexture*)origParam)->m_nbResourceType;

				if (type == eTextureResourceType::TEXTURE)
					pRsc->fixupAndPlaceObj(this->pEntries[i].pTxd);
				else if(type != eTextureResourceType::TEXTURE_RENDERTARGET){
					 if(type == eTextureResourceType::TEXTURE_REFERENCE)
						 pRsc->fixupAndPlaceObj(this->pEntries[i].pTxdRef);
					 else {
						 error("[grcInstanceData::place] Unknown texture resource type '%u'", (DWORD)type);
						 return;
					 }
				}
				/*grcTexture* pBaseTxd = (grcTexture*)origParam;
				if (pBaseTxd->m_nbResourceType == eTextureResourceType::TEXTURE) {
					if (!bHaveIncludedTxd) {
						error("[grcInstanceData::place] the resource uses included textures, but they were not detected");
						return;
					}
					grcTexturePC* pTmpTxd = new("instanceData, place, tempTxd") grcTexturePC;
					copy_class(pTmpTxd, (grcTexturePC*)pBaseTxd);
					pTmpTxd->m_usageCount = 1; // delete if copy_class set usageCount to 0
					pTmpTxd->place(pRsc);
					DWORD dwHash = pTmpTxd->getHash();
					dealloc(pTmpTxd);
					if (!pTxdDict->hasElementByHash(dwHash)) {
						error("[grcInstanceData::place] the texture is listed as included, but it was not found. If you see this message, it is a bug in this tool");
						return;
					}

					this->pEntries[i].pTxd = pTxdDict->getElementAddresByHash(dwHash);
				}
				else if (pBaseTxd->m_nbResourceType != eTextureResourceType::TEXTURE_RENDERTARGET) {
					if (pBaseTxd->m_nbResourceType == eTextureResourceType::TEXTURE_REFERENCE) {
						this->pEntries[i].pTxdRef = new("instanceData, place, txdref") grcTextureReference;
						copy_class(this->pEntries[i].pTxdRef, (grcTextureReference*)pBaseTxd);
						this->pEntries[i].pTxdRef->place(pRsc);
					}
					else {
						error("[grcInstanceData::place] Unknown texture resource type '%u'", (DWORD)pBaseTxd->m_nbResourceType);
						return;
					}
				}*/
			}
		}
	}

	DWORD grcInstanceData::recomputeTotalSize() {
		DWORD dwSize = ((this->dwCount + 0xf) & ~0xf) + // types
			((this->dwCount * 4 + 0xf) & ~0xf) + // ptrs
			((this->dwCount * 4 + 0xf) & ~0xf);// hashes

		for (DWORD i = 0; i < this->dwCount; i++) {
			if (this->pTypes[i] == 0) continue; // txd
			if (this->pEntries[i].pVec4 == NULL) continue; // null param
			
			switch (this->pTypes[i]) {
			case 8:
				dwSize += sizeof(Vector4) * 6;
				break;
			case 4:
			case 1:
			case 14:
			case 15:
			case 16:
				dwSize += sizeof(Vector4) * this->pTypes[i];
				break;
			default:
				error("[grcInstanceData::calculateEffectSize] unk param type '%u'", (DWORD)this->pTypes[i]);
				return 0;
			}
		}

		return dwSize;
	}

	void grcInstanceData::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		this->dwTotalSize = this->recomputeTotalSize();
		pLayout->addObject((BYTE*)this->pEntries, 5, this->dwTotalSize);

		for (DWORD i = 0; i < this->dwCount; i++) {
			if (this->pTypes[i] != 0) continue;
			if (this->pEntries[i].pVec4 == NULL) continue;

			//if (this->pEntries[i].pTxdRef->m_nbResourceType == eTextureResourceType::TEXTURE_REFERENCE) {
			//	pLayout->addObject(this->pEntries[i].pTxdRef, 5);
			//	this->pEntries[i].pTxdRef->addToLayout(pLayout, dwDepth);
			//}
			if (this->pEntries[i].pTxdRef->m_nbResourceType == eTextureResourceType::TEXTURE_REFERENCE) {
				pLayout->addObject(this->pEntries[i].pTxdRef, 5);
				this->pEntries[i].pTxdRef->addToLayout(pLayout, dwDepth);
			}
			else if (this->pEntries[i].pTxdRef->m_nbResourceType == eTextureResourceType::TEXTURE) {
				pLayout->addObject(this->pEntries[i].pTxd, 5);
				this->pEntries[i].pTxd->addToLayout(pLayout, dwDepth);
			}

		}
	}

	grcInstanceData::grcInstanceData() {
		this->pEntries = NULL;
		this->pEffect = NULL;
		this->dwCount = 0;
		this->dwTotalSize = 0;
		this->pTypes = NULL;
		this->dwEffectHashCode = 0;
		this->_f18 = 0;
		this->_f1c = 0;
		this->pEntriesHashes = NULL;
		this->_f24 = 0;
		this->_f28 = 0;
		this->_f2c = 0;
	}

	grcInstanceData::~grcInstanceData() {
		if (this->dwCount) {
			//libertyFourXYZ::g_memory_manager.release()
			for (DWORD i = 0; i < this->dwCount; i++) {
				if (this->pEntries[i].pVec4 == NULL) continue; // skip null params
				switch (this->pTypes[i]) {
				case 0:
					//if (this->pEntries[i].pTxdRef->m_nbResourceType == eTextureResourceType::TEXTURE_REFERENCE)
					//	dealloc(this->pEntries[i].pTxdRef);

					// only with the condition that our dealloc function supports usageCount
					dealloc(this->pEntries[i].pTxdRef);
					break;
				case 4:
					dealloc(this->pEntries[i].pMtx);
					break;
				case 1:
				case 8:
				case 14:
				case 15:
				case 16:
					dealloc_arr(this->pEntries[i].pVec4);
					break;
				default:
					error("[grcInstanceData::~grcInstanceData] unk param type '%u'", (DWORD)this->pTypes[i]);
					return;
				}

			}
			dealloc_arr(this->pEntries);
			dealloc_arr(this->pTypes);
			dealloc_arr(this->pEntriesHashes);
		}
	}

	void grcInstanceData::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		DWORD dwPos = 0;
		DWORD dwBlockSize = this->dwTotalSize;
		DWORD dwTypesOffset = (this->dwCount * sizeof(size_t) + 0xf) & ~0xf;
		DWORD dwHashesOffset = dwTypesOffset + ((this->dwCount + 0xf) & ~0xf);
		DWORD dwParamsOffset = dwHashesOffset + ((this->dwCount * sizeof(size_t) + 0xf) & ~0xf);

		auto ppParams = this->pEntries;
		pLayout->setPtr(this->pEntries);

		auto pTypes = this->pTypes;
		pLayout->backupPtr(this->pTypes);
		this->pTypes = (BYTE*)((size_t)this->pEntries + dwTypesOffset);
		
		auto pHash = this->pEntriesHashes;
		pLayout->backupPtr(this->pEntriesHashes);
		this->pEntriesHashes = (DWORD*)((size_t)this->pEntries + dwHashesOffset);
		
		dwPos = dwParamsOffset;

		for (DWORD i = 0; i < this->dwCount; i++) {
			Vector4* data;
			if (ppParams[i].pVec4 == NULL) continue; // skip null params
			DWORD dwParamSize = 0;
			switch (pTypes[i]) {
			case 0:
				if (ppParams[i].pTxdRef->m_nbResourceType == eTextureResourceType::TEXTURE_REFERENCE) {
					ppParams[i].pTxdRef->replacePtrs(pLayout, pRsc, dwDepth + 1);
					
					auto origData = ppParams[i].pTxdRef;
					pLayout->setPtr(ppParams[i].pTxdRef);
					DWORD dwSize = sizeof * ppParams[i].pTxdRef;
					memcpy(pRsc->getFixup(ppParams[i].pTxdRef, dwSize), origData, dwSize);
				}
				else if (ppParams[i].pTxdRef->m_nbResourceType == eTextureResourceType::TEXTURE) {
					//pLayout->backupPtr(ppParams[i].txd.pTxd);
					pLayout->setPtr(ppParams[i].pTxd); // ToDo: fix it!

				}

				break;
			case 8:
			case 4:
			case 1:
			case 14:
			case 15:
			case 16:
				if(pTypes[i] == 8)
					dwParamSize = sizeof(Vector4) * 6;
				else dwParamSize = sizeof(Vector4) * pTypes[i];

				pLayout->backupPtr(ppParams[i].pVec4);
				data = ppParams[i].pVec4;
				ppParams[i].pVec4 = (Vector4*)((size_t)this->pEntries + dwPos);
				memcpy(pRsc->getFixup(ppParams[i].pVec4, -1), data, dwParamSize);
				dwPos += dwParamSize;
				break;
			default:
				error("[grcInstanceData::replacePtrs] unk param type '%u'", (DWORD)pTypes[i]);
				return;
			}

		}

		if (dwBlockSize < dwPos)
			error("[grcInstanceData::replacePtrs] the actual effect size is larger than expected. Contact the developer or fix it yourself");

		memcpy(pRsc->getFixup(this->pEntries, -1), ppParams, sizeof(*ppParams) * this->dwCount);
		memcpy(pRsc->getFixup(this->pTypes, -1), pTypes, sizeof(*pTypes) * this->dwCount);
		memcpy(pRsc->getFixup(this->pEntriesHashes, -1), pHash, sizeof(*pHash) * this->dwCount);


	}

	void grcInstanceData::clearRefCount() {
		for (DWORD i = 0; i < this->dwCount; i++) {
			if (this->pEntries[i].pVec4 == NULL) continue; // skip null params
			if (!this->pTypes[i]) {
				if (this->pEntries[i].pTxdRef->m_nbResourceType == eTextureResourceType::TEXTURE_REFERENCE) 
					this->pEntries[i].pTxdRef->clearRefCount();
			}
		}
	}

	void grcInstanceData::setRefCount() {
		for (DWORD i = 0; i < this->dwCount; i++) {
			if (this->pEntries[i].pVec4 == NULL) continue; // skip null params
			if (!this->pTypes[i]) {
				if (this->pEntries[i].pTxdRef->m_nbResourceType == eTextureResourceType::TEXTURE_REFERENCE)
					this->pEntries[i].pTxdRef->setRefCount();
				else if (this->pEntries[i].pTxdRef->m_nbResourceType == eTextureResourceType::TEXTURE) 
					this->pEntries[i].pTxd->setRefCount();
			}
		}
	}

	void grmShader::place(datResource* pRsc) {
		pgBase::place(pRsc);

		this->m_instanceData.place(pRsc);
	}

	grmShader::grmShader() {
		m_nbVersion = 2;
		m_nbDrawBucket = 0;
		m_nbFlags = 0;
		_fb = 0;
		_fc = 0;
		m_wIndex = 0;
		_f10 = 0;
	}

	grmShader::~grmShader() {};

	void grmShader::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		pgBase::addToLayout(pLayout, dwDepth);
		this->m_instanceData.addToLayout(pLayout, dwDepth);
	}

	void grmShader::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		pgBase::replacePtrs(pLayout, pRsc, dwDepth);
		this->m_instanceData.replacePtrs(pLayout, pRsc, dwDepth);
	}

	void grmShader::clearRefCount() {
		pgBase::clearRefCount();
		this->m_instanceData.clearRefCount();
	}

	void grmShader::setRefCount() {
		pgBase::setRefCount();
		this->m_instanceData.setRefCount();
	}

	void grmShaderFx::place(datResource* pRsc) {
		grmShader::place(pRsc);
		if(this->m_pszName)
			this->m_pszName.place(pRsc);
		if(this->m_pszPresetName)
			this->m_pszPresetName.place(pRsc);
		if(this->_f4c)
			this->_f4c.place(pRsc);
		if(this->_f50)
			this->_f50.place(pRsc);
	}

	grmShaderFx::grmShaderFx() : m_dwPresetIndex(0), _f58(NULL) {}
	grmShaderFx::~grmShaderFx() {}

	void grmShaderFx::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		grmShader::addToLayout(pLayout, dwDepth);
		if (this->m_pszName)
			this->m_pszName.addToLayout(pLayout, dwDepth);
		if (this->m_pszPresetName)
			this->m_pszPresetName.addToLayout(pLayout, dwDepth);
		if (this->_f4c)
			this->_f4c.addToLayout(pLayout, dwDepth);
		if (this->_f50)
			this->_f50.addToLayout(pLayout, dwDepth);
	}

	void grmShaderFx::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		grmShader::replacePtrs(pLayout, pRsc, dwDepth);
		if (this->m_pszName)
			this->m_pszName.replacePtrs(pLayout, pRsc, dwDepth);
		if (this->m_pszPresetName)
			this->m_pszPresetName.replacePtrs(pLayout, pRsc, dwDepth);
		if (this->_f4c)
			this->_f4c.replacePtrs(pLayout, pRsc, dwDepth);
		if (this->_f50)
			this->_f50.replacePtrs(pLayout, pRsc, dwDepth);
	}

	void grmShaderFx::clearRefCount() {
		grmShader::clearRefCount();
	}

	void grmShaderFx::setRefCount() {
		grmShader::setRefCount();
	}

	void grmShaderGroup::place(datResource* pRsc) {
		datBase::place(pRsc);

		if (this->m_pTexture) {
			auto origTxd = pRsc->getFixup(this->m_pTexture, sizeof(*this->m_pTexture));
			this->m_pTexture = new("shaderGroup, place, txd") pgDictionary<grcTexturePC>;
			copy_class(this->m_pTexture, origTxd);
			this->m_pTexture->place(pRsc);
			
			//this->m_pTexture.place(pRsc);
		}
		if (this->m_shaders.m_count)
			this->m_shaders.place(pRsc);
		if (this->m_vertexFormat.m_count)
			this->m_indexMapping.place(pRsc);
		if (this->m_indexMapping.m_count)
			this->m_vertexFormat.place(pRsc);
	}

	grmShaderGroup::grmShaderGroup() : m_pTexture(NULL) {
		memset(this->__10, 0x0, 0x30);
	}

	grmShaderGroup::~grmShaderGroup() {
		//this->m_shaders.~atArray(); // ToDo: fix
		if (this->m_pTexture)
			dealloc(this->m_pTexture);
	}

	void grmShaderGroup::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		datBase::addToLayout(pLayout, dwDepth);
		if (this->m_pTexture) {
			pLayout->addObject(this->m_pTexture, 5);
			this->m_pTexture->addToLayout(pLayout, dwDepth + 1);
		}
		if(this->m_shaders.m_count)
			this->m_shaders.addToLayout(pLayout, dwDepth);
		if (this->m_vertexFormat.m_count)
			this->m_vertexFormat.addToLayout(pLayout, dwDepth);
		if (this->m_indexMapping.m_count)
			this->m_indexMapping.addToLayout(pLayout, dwDepth);
		
	}

	void grmShaderGroup::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		datBase::replacePtrs(pLayout, pRsc, dwDepth);

		if (this->m_pTexture) {
			this->m_pTexture->replacePtrs(pLayout, pRsc, dwDepth + 1);
			auto origData = this->m_pTexture;
			pLayout->setPtr(this->m_pTexture);
			DWORD dwSize = sizeof * this->m_pTexture;
			memcpy(pRsc->getFixup(this->m_pTexture, dwSize), origData, dwSize);
		}

		if (this->m_shaders.m_count)
			this->m_shaders.replacePtrs(pLayout, pRsc, dwDepth);
		if (this->m_vertexFormat.m_count)
			this->m_vertexFormat.replacePtrs(pLayout, pRsc, dwDepth);
		if (this->m_indexMapping.m_count)
			this->m_indexMapping.replacePtrs(pLayout, pRsc, dwDepth);


	}

	void grmShaderGroup::clearRefCount() {
		datBase::clearRefCount();
		if (this->m_pTexture)
			this->m_pTexture->clearRefCount();
		this->m_shaders.clearRefCount();
	}

	void grmShaderGroup::setRefCount() {
		datBase::setRefCount();
		if (this->m_pTexture)
			this->m_pTexture->setRefCount();
		this->m_shaders.setRefCount();
	}

}