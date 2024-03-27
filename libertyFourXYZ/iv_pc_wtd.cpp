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

		rage::datResource* pResource = libertyFourXYZ::g_memory_manager.allocate<rage::datResource>("processWtd, pResource1");
		pResource->m_pszDebugName = pszResName;
		rage::datResourceFileHeader* pResourceHeader = libertyFourXYZ::g_memory_manager.allocate<rage::datResourceFileHeader>("processWtd, pResHeader");
		libertyFourXYZ::readRsc85Resource((char*)pszFilePath.c_str(), pResourceHeader, pResource);

		rage::pgDictionary<rage::grcTexturePC>* pTextureDict = libertyFourXYZ::g_memory_manager.allocate<rage::pgDictionary<rage::grcTexturePC>>("processWtd, txdDict");
		//pTextureDict->place(pResource, (rage::pgDictionary<rage::grcTexturePC>*)(pResourceHeader->flags.getVBlockStart() + 0x50000000));

		auto realPtr = pResource->getFixup((rage::pgDictionary<rage::grcTexturePC>*)(pResourceHeader->flags.getVBlockStart() + 0x50000000), sizeof (*pTextureDict));
		copy_class<rage::pgDictionary<rage::grcTexturePC>>(pTextureDict, realPtr);

		pTextureDict->place(pResource);
		libertyFourXYZ::g_memory_manager.release<rage::datResourceFileHeader>(pResourceHeader);
		libertyFourXYZ::g_memory_manager.release<rage::datResource>(pResource);

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

		pResource = libertyFourXYZ::g_memory_manager.allocate<rage::datResource>("processWtd, pResource2");
		pResource->m_pMap->validateMap(layout.getResourceInfo());
		pTextureDict->replacePtrs(&layout, pResource, 0);
		memcpy(pResource->getFixup((rage::pgDictionary<rage::grcTexturePC>*)layout.mainObj.second, sizeof(*pTextureDict)), pTextureDict, sizeof(*pTextureDict));

		layout.setOldPtrs();
		libertyFourXYZ::g_memory_manager.release<rage::pgDictionary<rage::grcTexturePC>>(pTextureDict);

		pResource->saveRawResource(pszFilePath.getFilePath(), pszFilePath.getFileNameWithoutExt(), 0);
		pResource->saveResource(pszFilePath.getFilePath(), "newTxd", "wtd", 8, layout.getResourceInfo(), 0);

		libertyFourXYZ::g_memory_manager.release<rage::datResource>(pResource);


	}

	void mergeWtdsToWmtd(DWORD dwCount, rage::ConstString* pszWtdFiles) {

		rage::datResourceFileHeader* pResourceHeader;
		rage::datResource* pResource;

		rage::pgDictionary<rage::pgDictionary<rage::grcTexturePC>> *pMegaTextureDict =
			libertyFourXYZ::g_memory_manager.allocate<rage::pgDictionary<rage::pgDictionary<rage::grcTexturePC>>>("mergeWtdsToWmtd, dict");
		//pMegaTextureDict->m_pPageMap = libertyFourXYZ::g_memory_manager.allocate<rage::pgBasePageMap>();
		//memset(pMegaTextureDict->m_pPageMap, 0, 0x4);
#ifndef _DEBUG
		//memset(pMegaTextureDict->m_pPageMap + 0x4, 0xcd, 0x20c);
#endif // _DEBUG


		for (DWORD i = 0; i < dwCount; i++) {
			pResource = libertyFourXYZ::g_memory_manager.allocate<rage::datResource>("mergeWtdsToWmtd, pResLoop");
			pResource->m_pszDebugName = pszWtdFiles[i].getFileNameWithoutExt();
			pResourceHeader = libertyFourXYZ::g_memory_manager.allocate<rage::datResourceFileHeader>("mergeWtdsToWmtd, pResHeaderLoop");
			libertyFourXYZ::readRsc85Resource(pszWtdFiles[i].c_str(), pResourceHeader, pResource);
			
			rage::pgDictionary<rage::grcTexturePC>* pTextureDict = libertyFourXYZ::g_memory_manager.allocate<rage::pgDictionary<rage::grcTexturePC>>("mergeWtdsToWmtd, smallDict");
			//pTextureDict->place(pResource, (rage::pgDictionary<rage::grcTexturePC>*)(pResourceHeader->flags.getVBlockStart() + 0x50000000));
			pTextureDict->place(pResource);
			
			libertyFourXYZ::g_memory_manager.release<rage::datResourceFileHeader>(pResourceHeader);
			libertyFourXYZ::g_memory_manager.release<rage::datResource>(pResource);

			pMegaTextureDict->addElement(atStringHash(pszWtdFiles[i].getFileNameWithoutExt(), 0), pTextureDict);
			libertyFourXYZ::g_memory_manager.release<rage::pgDictionary<rage::grcTexturePC>>(pTextureDict);

		}
		
		libertyFourXYZ::rsc85_layout layout;

		layout.dwMainObjectSize = sizeof * pMegaTextureDict;
		layout.mainObj.first = pMegaTextureDict;

		pMegaTextureDict->addToLayout(&layout, 0);
		layout.create();

		pResource = libertyFourXYZ::g_memory_manager.allocate<rage::datResource>("mergeWtdsToWmtd, res");
		pResource->m_pszDebugName = "megaTextureDictionary";
		pResource->m_pMap->validateMap(layout.getResourceInfo());
		pMegaTextureDict->replacePtrs(&layout, pResource, 0);
		//memcpy(pResource->getFixup<rage::grcTexturePC>((rage::grcTexturePC*)layout.mainObj.second), pMegaTextureDict, sizeof(*pMegaTextureDict));
		copy_class(pResource->getFixup((rage::pgDictionary<rage::pgDictionary<rage::grcTexturePC>>*)layout.mainObj.second, sizeof(*pMegaTextureDict)), pMegaTextureDict);
		layout.setOldPtrs();

		libertyFourXYZ::g_memory_manager.release<rage::pgDictionary<rage::pgDictionary<rage::grcTexturePC>>>(pMegaTextureDict);

		FILE* fOut = fopen(rage::ConstString::format("%s/big_txd_dict.bin", pszWtdFiles->getFilePath()), "wb");
		for (BYTE i = 0; i < pResource->m_pMap->nbPhysicalCount + pResource->m_pMap->nbVirtualCount; i++)
			fwrite(pResource->m_pMap->chunks[i].pDest, 1, pResource->m_pMap->chunks[i].dwSize, fOut);
		fclose(fOut);

		libertyFourXYZ::g_memory_manager.release<rage::datResource>(pResource);

	}

	void wtdFromDdsFiles(rage::ConstString &pszOutFile, DWORD dwCount, rage::ConstString* ppszDdsPath) {
		rage::pgDictionary<rage::grcTexturePC>* pTextureDict = libertyFourXYZ::g_memory_manager.allocate<rage::pgDictionary<rage::grcTexturePC>>("wtdFromDdsFiles, txdDict");
		for (DWORD i = 0; i < dwCount; i++) {
			rage::grcTexturePC *pTxd = libertyFourXYZ::g_memory_manager.allocate<rage::grcTexturePC>("wtd from dds files, for");
			rage::ConstString* path = ppszDdsPath + i;
			int result = pTxd->fromDdsPath(*path);
			if(!result) pTextureDict->addElement(pTxd->getHash(), pTxd);
			else libertyFourXYZ::g_memory_manager.release(pTxd);
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

		rage::datResource* pResource = libertyFourXYZ::g_memory_manager.allocate<rage::datResource>("wtdFromDdsFiles, res");
		pResource->m_pszDebugName = pszOutFile.getFileNameWithoutExt();
		
		pResource->m_pMap->validateMap(layout.getResourceInfo());
		pTextureDict->replacePtrs(&layout, pResource, 0);
		memcpy(pResource->getFixup((rage::pgDictionary<rage::grcTexturePC>*)layout.mainObj.second, sizeof(*pTextureDict)), pTextureDict, sizeof(*pTextureDict));

		layout.setOldPtrs();
		libertyFourXYZ::g_memory_manager.release<rage::pgDictionary<rage::grcTexturePC>>(pTextureDict);

		pResource->saveRawResource(pszOutFile.getFilePath(), pszOutFile.getFileNameWithoutExt(), 1);
		pResource->saveResource(pszOutFile.getFilePath(), pszOutFile.getFileNameWithoutExt(), "wtd", 8, layout.getResourceInfo(), 0);

		libertyFourXYZ::g_memory_manager.release<rage::datResource>(pResource);


	}

};