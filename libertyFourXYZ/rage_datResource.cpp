#include "rage_datResource.h"

#include "rage_string.h"

#include "trace.h"
#include "compression/zlib/zlib.h"

namespace rage {
	DWORD datResourceInfoOld::getVirtualPageSize() { return 0x1000 << this->dwVSize; }
	DWORD datResourceInfoOld::getPhysicalPageSize() { return 0x1000 << this->dwPSize; }

	DWORD datResourceInfoOld::getVirtualSize() {
		DWORD _dwFlags1 = *(DWORD*)this;
		//return (0x100 << ((_dwFlags1 >> 0xb) & 0xf)) * (_dwFlags1 & 0x7ff);
		//return (_dwFlags1 & 0x7ff) << (((_dwFlags1 >> 11) & 0xf) + 8);

		DWORD dwPageSize = this->getVirtualPageSize();
		return dwPageSize * this->dwVPage0 + (this->dwVPage1 ? dwPageSize / 2 : 0) + (this->dwVPage2 ? dwPageSize / 4 : 0) +
			(this->dwVPage3 ? dwPageSize / 8 : 0) + (this->dwVPage4 ? dwPageSize / 16 : 0);

	}

	DWORD datResourceInfoOld::getPhysicalSize() {
		DWORD _dwFlags1 = *(DWORD*)this;
		//return (0x100 << ((_dwFlags1 >> 0x1a) & 0xf)) * ((_dwFlags1 >> 0xf) & 0x7ff);
		//return ((_dwFlags1 >> 15) & 0x7ff) << (((_dwFlags1 >> 26) & 0xf) + 8);

		DWORD dwPageSize = this->getPhysicalPageSize();
		return dwPageSize * this->dwPPage0 + (this->dwPPage1 ? dwPageSize / 2 : 0) + (this->dwPPage2 ? dwPageSize / 4 : 0) +
			(this->dwPPage3 ? dwPageSize / 8 : 0) + (this->dwPPage4 ? dwPageSize / 16 : 0);

	}

	BYTE datResourceInfoOld::getVPageCount() const { return static_cast<BYTE>(this->dwVPage0 + this->dwVPage1 + this->dwVPage2 + this->dwVPage3 + this->dwVPage4); }
	BYTE datResourceInfoOld::getPPageCount() const { return static_cast<BYTE>(this->dwPPage0 + this->dwPPage1 + this->dwPPage2 + this->dwPPage3 + this->dwPPage4); }

	DWORD datResourceInfo::getVirtualSize() {
		DWORD _dwFlags1 = *(DWORD*)this;
		if (this->bUseExtendedSize)
			return (*((DWORD*)this + 1) << 12) & 0x3FFF000;
		else
			//return (0x100 << ((_dwFlags1 >> 0xb) & 0xf)) * (_dwFlags1 & 0x7ff);
			return this->oldInfo.getVirtualSize();
	}

	DWORD datResourceInfo::getPhysicalSize() {
		DWORD _dwFlags1 = *(DWORD*)this;
		if (this->bUseExtendedSize)
			return (*((DWORD*)this + 1) >> 2) & 0x3fff000;
		else
			//return (0x100 << ((_dwFlags1 >> 0x1a) & 0xf)) * ((_dwFlags1 >> 0xf) & 0x7ff);
			return this->oldInfo.getPhysicalSize();
	}

	DWORD datResourceInfo::getVBlockStart() {
		if (!this->bUseExtendedSize) return 0;
		DWORD currentPos = 0;
		if (this->dwStartPage > 3)
			return currentPos;
		currentPos += this->dwVPage65536 * 0x10000;
		if (this->dwStartPage == 3)
			return currentPos;
		currentPos += this->dwVPage32768 * 0x8000;
		if (this->dwStartPage == 2)
			return currentPos;
		currentPos += this->dwVPage16384 * 0x4000;
		if (this->dwStartPage == 1)
			return currentPos;
		currentPos += this->dwVPage8192 * 0x2000;
		return currentPos;
	}
	
	datResourceChunk::datResourceChunk() { memset(this, 0x0, sizeof * this); }
	datResourceChunk::~datResourceChunk() {
		if(this->dwSize)
			libertyFourXYZ::g_memory_manager.release<BYTE>(this->pDest);
		memset(this, 0x0, sizeof * this);
	}

	void datResourceMap::validateMap(rage::datResourceInfo* pInfo) {
		if (pInfo->bUseExtendedSize)
			this->generateMap(pInfo);
		else
			this->generateMap(&pInfo->oldInfo);


		rage::datResourceChunk* chunk = chunks;
		for (BYTE i = 0; i < this->nbVirtualCount + this->nbPhysicalCount; i++) {
			chunk->pDest = libertyFourXYZ::g_memory_manager.allocate<BYTE>("datResource, chunk", chunk->dwSize);
#ifndef _DEBUG
			memset(chunk->pDest, 0xcd, chunk->dwSize);
#endif // _DEBUG
			chunk++;
		}

		// ToDo: implementation
	}

	void datResourceMap::fillMap(BYTE* pRawResource) {
		DWORD dwPosition = 0;

		rage::datResourceChunk* chunk = chunks;
		for (BYTE i = 0; i < this->nbVirtualCount + this->nbPhysicalCount; i++) {
			memmove(chunk->pDest, pRawResource + dwPosition, chunk->dwSize);
			dwPosition += chunk->dwSize;
			chunk++;
		}
	}

	void datResourceMap::printMap(int(__cdecl* print_message)(const char* msg, ...)) {
		print_message("<datResourceMap>\n");
		rage::datResourceChunk* chunk = chunks;
		for (BYTE i = 0; i < this->nbVirtualCount + this->nbPhysicalCount; i++) {
			print_message("\t<chunk src=\"0x%08x\" dest=\"0x%08x\" size=\"%u\">\n", chunk->pSrc, chunk->pDest, chunk->dwSize);
			chunk++;
		}
		print_message("</datResourceMap>\n");
	}

	datResourceMap::~datResourceMap() {
		memset(this, 0x0, 0x10); // только до чанков
	}

	void datResourceMap::generateMap(rage::datResourceInfoOld* resInfo) {
		this->nbVirtualCount = resInfo->getVPageCount();
		this->nbPhysicalCount = resInfo->getPPageCount();
		this->nbRootVirtualChunk = 0;
		this->pAllocatedVPages = NULL;
		this->_f8 = 0;

		rage::datResourceChunk* pb = this->chunks;

		DWORD dwPageSize = resInfo->getVirtualPageSize();
		DWORD dwTotalSize = resInfo->getVirtualSize();
		DWORD dwBase = 0x50000000;
		for (BYTE b = 0; b < nbVirtualCount; b++, pb++) {
			while (dwPageSize > dwTotalSize) dwPageSize >>= 1;
			pb->pSrc = dwBase;
			pb->pDest = NULL;
			pb->dwSize = dwPageSize;

			dwBase += dwPageSize;
			dwTotalSize -= dwPageSize;
		}

		dwPageSize = resInfo->getPhysicalPageSize();
		dwTotalSize = resInfo->getPhysicalSize();
		dwBase = 0x60000000;
		for (BYTE b = 0; b < this->nbPhysicalCount; b++, pb++) {
			while (dwPageSize > dwTotalSize) dwPageSize >>= 1;
			pb->pSrc = dwBase;
			pb->pDest = NULL;
			pb->dwSize = dwPageSize;

			dwBase += dwPageSize;
			dwTotalSize -= dwPageSize;
		}
	}

	void datResourceMap::generateMap(rage::datResourceInfo* resInfo) {

		if (!resInfo->bUseExtendedSize) { // на всякий случай
			this->generateMap(&resInfo->oldInfo);
			return;
		}

		this->pAllocatedVPages = NULL;
		this->pDataStart = 0;
		this->nbRootVirtualChunk = 0;
		this->_f8 = 0;
		
		WORD v = resInfo->dwFlags2 & 0x3fff;
		WORD p = (resInfo->dwFlags2 >> 14) & 0x3fff;

		rage::datResourceChunk* pb = this->chunks;
		if (v) {
			DWORD dwSize = v << 12;
			DWORD dwPageSize = 0x80000;
			DWORD _counts[4] = { resInfo->dwVPage0, resInfo->dwVPage1, resInfo->dwVPage2, 0x7fffffff }; 
			DWORD* pCount = _counts;
			DWORD dwBase = 0x50000000;

			DWORD dwStartPage = 0xffffffff;
			DWORD dwPageIndex = 0;

			for (DWORD i = 0; i < 4; i++, pCount++, dwPageSize >>= 1) {
				for (DWORD i = 0; dwSize && i < *pCount; i++, dwPageIndex++) {
					while (dwPageSize > dwSize) dwPageSize >>= 1;

					if (((0x1000 << resInfo->dwStartPage) == dwPageSize) && (0xffffffff == dwStartPage)) dwStartPage = dwPageIndex;

					pb->pSrc = dwBase;
					pb->pDest = NULL;
					pb->dwSize = dwPageSize;
					pb++;

					dwSize -= dwPageSize;
					dwBase += dwPageSize;
				}
			}

			this->nbRootVirtualChunk = static_cast<BYTE>(dwStartPage);
			this->nbVirtualCount = static_cast<BYTE>(dwPageIndex);
		}
		if (p) { // исправлено 
			DWORD dwSize = p << 12;
			DWORD dwPageSize = 0x100000;
			DWORD _counts[4] = { resInfo->dwPPage0, resInfo->dwPPage1, resInfo->dwPPage2, 0x7fffffff };
			DWORD* pCount = _counts;
			DWORD dwBase = 0x60000000;

			DWORD dwPageIndex = 0;


			for (DWORD i = 0; i < 4; i++, pCount++, dwPageSize >>= 1) {
				if (!dwSize) continue;

				for (DWORD i = 0; dwSize && i < *pCount; i++, dwPageIndex++) {
					while (dwPageSize > dwSize) dwPageSize >>= 1;


					pb->pSrc = dwBase;
					pb->pDest = NULL;
					pb->dwSize = dwPageSize;
					pb++;

					dwSize -= dwPageSize;
					dwBase += dwPageSize;
				}
			}
			this->nbPhysicalCount = static_cast<BYTE>(dwPageIndex);
		}
	}

	datResource::datResource(const char* pszDebugName) {
		this->m_pszDebugName = pszDebugName;
		this->m_pMap = libertyFourXYZ::g_memory_manager.allocate<rage::datResourceMap>("datResource, map");
	}

	datResource::datResource() {
		this->m_pszDebugName = "unnamed";
		this->m_pMap = libertyFourXYZ::g_memory_manager.allocate<rage::datResourceMap>("datResource, map");
	}

	datResource::~datResource() {
		libertyFourXYZ::g_memory_manager.release<rage::datResourceMap>(this->m_pMap);
	}


	datResourceFileHeader::datResourceFileHeader() {
		memset(this, 0x0, sizeof * this);
	}

	void datResource::saveRawResource(const char* pszPath, const char* pszName, bool bUseSysGfx) {
		FILE* fOut;
		if (!bUseSysGfx) {
			fOut = fopen(rage::ConstString::format("%s/%s.bin", pszPath, pszName), "wb");
			for (BYTE i = 0; i < this->m_pMap->nbPhysicalCount + this->m_pMap->nbVirtualCount; i++)
				fwrite(this->m_pMap->chunks[i].pDest, 1, this->m_pMap->chunks[i].dwSize, fOut);
			fclose(fOut);
		}
		else {
			if (this->m_pMap->nbVirtualCount) {
				fOut = fopen(rage::ConstString::format("%s/%s.sys", pszPath, pszName), "wb");
				for (BYTE i = 0; i < this->m_pMap->nbVirtualCount; i++)
					fwrite(this->m_pMap->chunks[i].pDest, 1, this->m_pMap->chunks[i].dwSize, fOut);
				fclose(fOut);
			}
			if (this->m_pMap->nbPhysicalCount) {
				fOut = fopen(rage::ConstString::format("%s/%s.gfx", pszPath, pszName), "wb");
				for (BYTE i = this->m_pMap->nbVirtualCount; i < this->m_pMap->nbPhysicalCount + this->m_pMap->nbVirtualCount; i++)
					fwrite(this->m_pMap->chunks[i].pDest, 1, this->m_pMap->chunks[i].dwSize, fOut);
				fclose(fOut);
			}
		}
	}

	void datResource::saveResource(const char* pszPath, const char* pszName, const char* pszExt, DWORD dwVersion, datResourceInfo* pResInfo, BYTE nbCompressionIndex) {
		BYTE* pUncompressed = libertyFourXYZ::g_memory_manager.allocate<BYTE>("datRes, save, uncompBuf", pResInfo->getVirtualSize() + pResInfo->getPhysicalSize());
		BYTE* pCompressed = libertyFourXYZ::g_memory_manager.allocate<BYTE>("datRes, save, compBuf", pResInfo->getVirtualSize() + pResInfo->getPhysicalSize());
	
		if (!pResInfo->bUseExtendedSize) pResInfo->oldInfo.bCompressed = 1;

		DWORD dwPos = 0;
		for (BYTE i = 0; i < this->m_pMap->nbPhysicalCount + this->m_pMap->nbVirtualCount; i++) {
			memcpy(pUncompressed + dwPos, this->m_pMap->chunks[i].pDest, this->m_pMap->chunks[i].dwSize);
			dwPos += this->m_pMap->chunks[i].dwSize;
		}
		DWORD dwCompressedSize = 0;
		if (nbCompressionIndex == 0) { // zlib
			uLongf dsize = dwCompressedSize = pResInfo->getVirtualSize() + pResInfo->getPhysicalSize();
			compress(pCompressed, &dsize, pUncompressed, pResInfo->getVirtualSize() + pResInfo->getPhysicalSize());
			dwCompressedSize = dsize;
		}

		libertyFourXYZ::g_memory_manager.release<BYTE>(pUncompressed);

		FILE* fOut = fopen(rage::ConstString::format("%s/%s.%s", pszPath, pszName, pszExt), "wb");
		DWORD dwMagic;
		if (pResInfo->bUseExtendedSize) {
			dwMagic = 2235781970;
			fwrite(&dwMagic, sizeof(dwMagic), 1, fOut);
			fwrite(&dwVersion, sizeof(dwVersion), 1, fOut);
			fwrite(pResInfo, sizeof(*pResInfo), 1, fOut);
		}
		else {
			dwMagic = 88298322;
			fwrite(&dwMagic, sizeof(dwMagic), 1, fOut);
			fwrite(&dwVersion, sizeof(dwVersion), 1, fOut);
			fwrite(pResInfo, sizeof(pResInfo->oldInfo), 1, fOut);
		}
		fwrite(pCompressed, 1, dwCompressedSize, fOut);
		libertyFourXYZ::g_memory_manager.release<BYTE>(pCompressed);

		fclose(fOut);

	}


}