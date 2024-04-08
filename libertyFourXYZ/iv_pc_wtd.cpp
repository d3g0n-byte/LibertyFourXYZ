#include "iv_pc_wtd.h"

#include "rage_datResource.h"
#include "rage_grcTexture.h"
#include "resource_reader.h"
#include "memory_manager.h"

#include "rsc85_layout.h"

#include "utils.h"

//#include "compression/zstd/zstd.h"
#include "compression/zlib/zlib.h"

#pragma warning (disable : 4996) // for FILE*

namespace iv_pc_wtd {

	void processWTD(rage::ConstString & pszFilePath) {

		rage::ConstString pszResName = pszFilePath.getFileNameWithoutExt();

		rage::datResource* pResource = new("processWtd, pResource1") rage::datResource(pszResName);
		rage::datResourceFileHeader* pResourceHeader = new("processWtd, pResHeader")rage::datResourceFileHeader;
		libertyFourXYZ::readRsc85Resource((char*)pszFilePath.c_str(), pResourceHeader, pResource);

		rage::pgDictionary<rage::grcTexturePC>* pTextureDict = new("processWtd, txdDict")rage::pgDictionary<rage::grcTexturePC>;
		//pTextureDict->place(pResource, (rage::pgDictionary<rage::grcTexturePC>*)(pResourceHeader->flags.getVBlockStart() + 0x50000000));

		auto realPtr = pResource->getFixup((rage::pgDictionary<rage::grcTexturePC>*)(pResourceHeader->flags.getVBlockStart() + 0x50000000), sizeof (*pTextureDict));
		copy_class<rage::pgDictionary<rage::grcTexturePC>>(pTextureDict, realPtr);

		pTextureDict->place(pResource);
		dealloc(pResourceHeader);
		dealloc(pResource);

		trace("<textures>");


		for (WORD i = 0; i < pTextureDict->m_entries.size(); i++) {
			rage::grcTexturePC* pTexture = pTextureDict->m_entries[i].pElement;
			trace("\t<name>%s</name>", pTexture->m_pszName.c_str());
		}
		trace("</textures>");

		// загрузили. тепер добавимо pPageMap
		//pTextureDict->m_pPageMap = libertyFourXYZ::g_memory_manager.allocate<rage::pgBasePageMap>();

		libertyFourXYZ::rsc85_layout layout;
		layout.setMainObject(pTextureDict);

		pTextureDict->addToLayout(&layout, 0);
		layout.create();

		pResource = new("processWtd, pResource2")rage::datResource(pszResName);
		pResource->m_pMap->validateMap(layout.getResourceInfo());
		pTextureDict->replacePtrs(&layout, pResource, 0);
		memcpy(pResource->getFixup((rage::pgDictionary<rage::grcTexturePC>*)layout.mainObj.second, sizeof(*pTextureDict)), pTextureDict, sizeof(*pTextureDict));

		layout.setOldPtrs();
		dealloc(pTextureDict);

		pResource->saveRawResource(pszFilePath.getFilePath(), pszFilePath.getFileNameWithoutExt(), 0);
		pResource->saveResource(pszFilePath.getFilePath(), "newTxd", "wtd", 8, layout.getResourceInfo(), 0);

		dealloc(pResource);


	}

	void mergeWtdsToWmtd(DWORD dwCount, rage::ConstString* pszWtdFiles) {

		rage::datResourceFileHeader* pResourceHeader;
		rage::datResource* pResource;

		rage::pgDictionary<rage::pgDictionary<rage::grcTexturePC>> *pMegaTextureDict =
			new("mergeWtdsToWmtd, dict") rage::pgDictionary<rage::pgDictionary<rage::grcTexturePC>>;
		//pMegaTextureDict->m_pPageMap = libertyFourXYZ::g_memory_manager.allocate<rage::pgBasePageMap>();
		//memset(pMegaTextureDict->m_pPageMap, 0, 0x4);
#ifndef _DEBUG
		//memset(pMegaTextureDict->m_pPageMap + 0x4, 0xcd, 0x20c);
#endif // _DEBUG


		for (DWORD i = 0; i < dwCount; i++) {
			pResource = new("mergeWtdsToWmtd, pResLoop")rage::datResource("megaTxdDict");
			pResource->m_pszDebugName = pszWtdFiles[i].getFileNameWithoutExt();
			pResourceHeader = new("mergeWtdsToWmtd, pResHeaderLoop") rage::datResourceFileHeader;
			libertyFourXYZ::readRsc85Resource(pszWtdFiles[i].c_str(), pResourceHeader, pResource);
			
			rage::pgDictionary<rage::grcTexturePC>* pTextureDict = new("mergeWtdsToWmtd, smallDict")rage::pgDictionary<rage::grcTexturePC>;
			//pTextureDict->place(pResource, (rage::pgDictionary<rage::grcTexturePC>*)(pResourceHeader->flags.getVBlockStart() + 0x50000000));
			pTextureDict->place(pResource);
			
			dealloc(pResourceHeader);
			dealloc(pResource);

			pMegaTextureDict->addElement(atStringHash(pszWtdFiles[i].getFileNameWithoutExt(), 0), pTextureDict);
			//dealloc(pTextureDict); // ??

		}
		
		libertyFourXYZ::rsc85_layout layout;

		layout.dwMainObjectSize = sizeof * pMegaTextureDict;
		layout.mainObj.first = pMegaTextureDict;

		pMegaTextureDict->addToLayout(&layout, 0);
		layout.create();

		pResource = new("mergeWtdsToWmtd, res")rage::datResource("megaTextureDictionary");
		pResource->m_pMap->validateMap(layout.getResourceInfo());
		pMegaTextureDict->replacePtrs(&layout, pResource, 0);
		//memcpy(pResource->getFixup<rage::grcTexturePC>((rage::grcTexturePC*)layout.mainObj.second), pMegaTextureDict, sizeof(*pMegaTextureDict));
		copy_class(pResource->getFixup((rage::pgDictionary<rage::pgDictionary<rage::grcTexturePC>>*)layout.mainObj.second, sizeof(*pMegaTextureDict)), pMegaTextureDict);
		layout.setOldPtrs();

		dealloc(pMegaTextureDict);

		FILE* fOut = fopen(rage::ConstString::format("%s/big_txd_dict.bin", pszWtdFiles->getFilePath()), "wb");
		for (BYTE i = 0; i < pResource->m_pMap->nbPhysicalCount + pResource->m_pMap->nbVirtualCount; i++)
			fwrite(pResource->m_pMap->chunks[i].pDest, 1, pResource->m_pMap->chunks[i].dwSize, fOut);
		fclose(fOut);

		dealloc(pResource);

	}

	void wtdFromDdsFiles(rage::ConstString &pszOutFile, DWORD dwCount, rage::ConstString* ppszDdsPath) {
		rage::pgDictionary<rage::grcTexturePC>* pTextureDict = new("wtdFromDdsFiles, txdDict")rage::pgDictionary<rage::grcTexturePC>;
		for (DWORD i = 0; i < dwCount; i++) {
			rage::grcTexturePC *pTxd = new("wtd from dds files, for") rage::grcTexturePC;
			rage::ConstString* path = ppszDdsPath + i;
			int result = pTxd->fromDdsPath(*path);
			if(!result) pTextureDict->addElement(pTxd->getHash(), pTxd);
			else dealloc(pTxd);
		}

		for (WORD i = 0; i < pTextureDict->m_entries.size(); i++) {
			rage::grcTexturePC* pTexture = pTextureDict->m_entries[i].pElement;
			pTexture->recreateName();
		}
		pTextureDict->clearRefCount();

		libertyFourXYZ::rsc85_layout layout;
		layout.dwMainObjectSize = sizeof * pTextureDict;
		layout.mainObj.first = pTextureDict;

		pTextureDict->setRefCount();
		pTextureDict->addToLayout(&layout, 0);
		layout.create();

		rage::datResource* pResource = new("wtdFromDdsFiles, res")rage::datResource(pszOutFile.getFileNameWithoutExt());
		
		pResource->m_pMap->validateMap(layout.getResourceInfo());
		pTextureDict->replacePtrs(&layout, pResource, 0);
		memcpy(pResource->getFixup((rage::pgDictionary<rage::grcTexturePC>*)layout.mainObj.second, sizeof(*pTextureDict)), pTextureDict, sizeof(*pTextureDict));

		layout.setOldPtrs();
		dealloc(pTextureDict);

		pResource->saveRawResource(pszOutFile.getFilePath(), pszOutFile.getFileNameWithoutExt(), 1);
		pResource->saveResource(pszOutFile.getFilePath(), pszOutFile.getFileNameWithoutExt(), "wtd", 8, layout.getResourceInfo(), 0);

		dealloc(pResource);


	}

};