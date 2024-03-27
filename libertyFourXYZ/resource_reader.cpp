#include "resource_reader.h"
#include "rage_datResource.h"


#pragma warning (disable : 4996) // for FILE*
#include "compression/zstd/zstd.h"
#include "compression/zlib/zlib.h"
#include "rage_grcTexture.h"
#include "utils.h"

namespace libertyFourXYZ {

	void readRootStruct(void* pRoot, DWORD dwRootObjSize, rage::datResource* pResource, rage::datResourceInfo* pResInfo) {
		//pTextureDict->place(pResource, (rage::pgDictionary<rage::grcTexturePC>*)(pResourceHeader->flags.getVBlockStart() + 0x50000000));
		//void* realPtr = pResource->getFixup((void*)(pResInfo->getVBlockStart() + 0x50000000), dwRootObjSize);

		//copy_class(pRoot, realPtr, 1, dwRootObjSize);
	}

	int readRsc85Resource(const char* pszPath, rage::datResourceFileHeader* pResHeader, rage::datResource* pResource) {
		if (!pResHeader)		return 1;
		else if (!pResource)	return 2;
		else if (!pszPath)		return 3;

		pResource->m_pszDebugName = rage::ConstString(pszPath).getFileNameWithoutExt();


		// загружаем ресурс 
		FILE* fIn = fopen(pszPath, "rb");

		if (!fIn) return 4;

		fseek(fIn, 0, SEEK_END);
		DWORD dwFileSize = ftell(fIn);
		fseek(fIn, 0, SEEK_SET);

		fread(pResHeader, 0x8, 0x1, fIn);

		// only rsc5 and 85
		if (pResHeader->dwSignature == 88298322) // rsc5
			fread(&pResHeader->flags, 0x4, 0x1, fIn);
		else if (pResHeader->dwSignature == 2235781970) // rsc85
			fread(&pResHeader->flags, 0x8, 0x1, fIn);
		else return 5;

		DWORD dwDecompressedSize = pResHeader->flags.getVirtualSize() + pResHeader->flags.getPhysicalSize();
		BYTE* pDecompressed = libertyFourXYZ::g_memory_manager.allocate<BYTE>("datRes, read, decomp", dwDecompressedSize);
		if (!pResHeader->flags.bUseExtendedSize && !pResHeader->flags.oldInfo.bCompressed)
			fread(pDecompressed, 0x1, dwDecompressedSize, fIn);
		else {
			DWORD dwCompressedSize = dwFileSize - (pResHeader->flags.bUseExtendedSize ? 0x10 : 0xc);
			BYTE* pCompressed = libertyFourXYZ::g_memory_manager.allocate<BYTE>("datRes, read, comp", dwCompressedSize);
			fread(pCompressed, 0x1, dwCompressedSize, fIn);

			uLongf dwDecompressedSize2 = dwDecompressedSize;
			//ZSTD_decompress(pDecompressed, dwDecompressedSize, pCompressed, dwCompressedSize);
			uncompress(pDecompressed, &dwDecompressedSize2, pCompressed, dwCompressedSize);
			libertyFourXYZ::g_memory_manager.release<BYTE>(pCompressed);

		}
		fclose(fIn);


		pResource->m_pMap->validateMap(&pResHeader->flags);
		pResource->m_pMap->fillMap(pDecompressed);

		libertyFourXYZ::g_memory_manager.release<BYTE>(pDecompressed);



		return 0;
	}
}