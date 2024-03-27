#include "mp3_pc_wtd.h"
#include "rage_base.h"
#include "dds.h"
#include "rage_grcTexture.h"
#include "utils.h"

namespace rage {
	class grcTexturePC_v11 : public pgBaseRefCounted {
	public:
		WORD m_wResourceType;
		WORD m_wLayerCount;
		void* m_pCachedTexture;
		DWORD m_dwPhysicalSize;
		rage::ConstString m_pszName;
		void* m_piTexture;
		WORD            m_wWidth;
		WORD            m_wHeight;
		WORD            _f24; // = 1
		WORD            m_wStride;
		DWORD           m_dwPixelFormat;
		BYTE            m_eTextureType;
		BYTE            m_nbLevels;
		WORD            _f2e;
		float m_fColorExpR;
		float m_fColorExpG;
		float m_fColorExpB;
		float m_fColorOfsR;
		float m_fColorOfsG;
		float m_fColorOfsB;
		grcTexturePC_v11* m_pPrev;
		grcTexturePC_v11* m_pNext;
		BYTE* m_pPixelData;
		BYTE __54[0x8];

		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
			pgBaseRefCounted::addToLayout(pLayout, dwDepth);
			this->m_pszName.addToLayout(pLayout, dwDepth + 1);
			pLayout->addObject<BYTE>(this->m_pPixelData, 6, this->m_dwPhysicalSize);
		}

		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
			pgBaseRefCounted::replacePtrs(pLayout, pRsc, dwDepth);
			this->m_pszName.replacePtrs(pLayout, pRsc, dwDepth + 1);
			auto pTxd = this->m_pPixelData;
			pLayout->setPtr(this->m_pPixelData);
			memcpy(pRsc->getFixup<BYTE>(this->m_pPixelData, this->m_dwPhysicalSize), pTxd, this->m_dwPhysicalSize);
		}

		void clearRefCount() {
			pgBaseRefCounted::clearRefCount();
		}

		void setRefCount() {
			pgBaseRefCounted::setRefCount();
		}

		void fromV8(grcTexturePC* pTxd) {
			this->m_wResourceType = pTxd->m_nbResourceType;
			m_wLayerCount = 0;
			m_pCachedTexture = NULL;
			m_dwPhysicalSize = pTxd->getPixelDataSize();
			m_pszName = pTxd->m_pszName;
			m_piTexture = NULL;
			m_wWidth = pTxd->m_wWidth;
			m_wHeight = pTxd->m_wHeight;
			_f24 = 1;
			m_wStride = pTxd->m_wMipStride;
			m_dwPixelFormat = pTxd->m_dwPixelFormat;
			m_eTextureType = pTxd->m_nbTextureType;
			m_nbLevels = pTxd->m_nbLevels;
			_f2e = 0;
			m_fColorExpR = pTxd->m_fColorExpR;
			m_fColorExpG = pTxd->m_fColorExpG;
			m_fColorExpB = pTxd->m_fColorExpB;
			m_fColorOfsR = pTxd->m_fColorOfsR;
			m_fColorOfsG = pTxd->m_fColorOfsG;
			m_fColorOfsB = pTxd->m_fColorOfsB;
			m_pPrev = NULL;
			m_pNext = NULL;
			if (m_dwPhysicalSize) {
				m_pPixelData = libertyFourXYZ::g_memory_manager.allocate<BYTE>("convert txd v8 to v11", m_dwPhysicalSize);
				memcpy(m_pPixelData, pTxd->m_pPixelData, m_dwPhysicalSize);
			}
			m_dwRefCount = pTxd->m_wUsageCount;
			memset(__54, 0x0, 8);
		}

		~grcTexturePC_v11() {
			if (m_pPixelData)
				libertyFourXYZ::g_memory_manager.release(m_pPixelData);
		}

		grcTexturePC_v11() {
			zero_fill_class(this);
		}

	};
}

namespace mp3_pc_wtd {
	void wtdFromDdsFiles(rage::ConstString& pszOutFile, DWORD dwCount, rage::ConstString* ppszDdsPath) {
		rage::pgDictionary<rage::grcTexturePC>* pTextureDictOld = libertyFourXYZ::g_memory_manager.allocate<rage::pgDictionary<rage::grcTexturePC>>("wtdFromDdsFiles v8 to v11, txdDictOld");
		for (DWORD i = 0; i < dwCount; i++) {
			rage::grcTexturePC* pTxd = libertyFourXYZ::g_memory_manager.allocate<rage::grcTexturePC>("wtd from dds files, for");
			rage::ConstString* path = ppszDdsPath + i;
			int result = pTxd->fromDdsPath(*path);
			if (!result) pTextureDictOld->addElement(pTxd->getHash(), pTxd);
			else libertyFourXYZ::g_memory_manager.release(pTxd);
		}

		for (WORD i = 0; i < pTextureDictOld->m_entries.size(); i++) {
			rage::grcTexturePC* pTexture = pTextureDictOld->m_entries[i].pElement;
			pTexture->recreateName();
		}

		// to v11
		rage::pgDictionary<rage::grcTexturePC_v11>* pTextureDict = libertyFourXYZ::g_memory_manager.allocate<rage::pgDictionary<rage::grcTexturePC_v11>>("wtdFromDdsFiles v8 to v11, txdDictNew");
		for (WORD i = 0; i < pTextureDictOld->m_entries.m_count; i++) {
			rage::grcTexturePC_v11* pTxd = libertyFourXYZ::g_memory_manager.allocate<rage::grcTexturePC_v11>("txd v11");
			pTxd->fromV8(pTextureDictOld->m_entries.m_pElements[i].get());
			pTextureDict->addElement(pTextureDictOld->m_codes[i], pTxd);
		}


		// delete old
		libertyFourXYZ::g_memory_manager.release<rage::pgDictionary<rage::grcTexturePC>>(pTextureDictOld);

		pTextureDict->clearRefCount();

		libertyFourXYZ::rsc85_layout layout;
		layout.dwMainObjectSize = sizeof * pTextureDict;
		layout.mainObj.first = pTextureDict;

		pTextureDict->setRefCount();
		pTextureDict->addToLayout(&layout, 0);
		layout.create();

		rage::datResource* pResource = libertyFourXYZ::g_memory_manager.allocate<rage::datResource>("wtdFromDdsFiles v11, res");
		pResource->m_pszDebugName = pszOutFile.getFileNameWithoutExt();

		pResource->m_pMap->validateMap(layout.getResourceInfo());
		pTextureDict->replacePtrs(&layout, pResource, 0);
		memcpy(pResource->getFixup((rage::pgDictionary<rage::grcTexturePC_v11>*)layout.mainObj.second, sizeof(*pTextureDict)), pTextureDict, sizeof(*pTextureDict));
		memset((size_t*)pResource->m_pMap->chunks[0].pDest + 1, 0xff, sizeof(size_t));

		layout.setOldPtrs();
		libertyFourXYZ::g_memory_manager.release<rage::pgDictionary<rage::grcTexturePC_v11>>(pTextureDict);

		pResource->saveRawResource(pszOutFile.getFilePath(), pszOutFile.getFileNameWithoutExt(), 1);
		pResource->saveResource(pszOutFile.getFilePath(), pszOutFile.getFileNameWithoutExt(), "wtd", 11, layout.getResourceInfo(), 0);

		libertyFourXYZ::g_memory_manager.release<rage::datResource>(pResource);

	}
}
