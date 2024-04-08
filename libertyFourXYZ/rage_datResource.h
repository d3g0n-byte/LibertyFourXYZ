#pragma once
#include <Windows.h>
#include "memory_manager.h"

#include "rage_string.h"

#include "rsc85_layout.h"

#include "utils.h"

namespace rage {


	class datResourceInfoOld {
	public:
		DWORD dwVPage4 : 1;
		DWORD dwVPage3 : 1;
		DWORD dwVPage2 : 1;
		DWORD dwVPage1 : 1;
		DWORD dwVPage0 : 7;
		DWORD dwVSize : 4;
		DWORD dwPPage4 : 1;
		DWORD dwPPage3 : 1;
		DWORD dwPPage2 : 1;
		DWORD dwPPage1 : 1;
		DWORD dwPPage0 : 7;
		DWORD dwPSize : 4;
		DWORD bCompressed : 1;
		DWORD bResource : 1;

		DWORD getVirtualPageSize();
		DWORD getPhysicalPageSize();
		DWORD getVirtualSize();
		DWORD getPhysicalSize();
		BYTE getVPageCount() const;
		BYTE getPPageCount() const;


	};

	union datResourceInfo {
	public:
		struct {
			datResourceInfoOld oldInfo;
			DWORD dwFlags2;
		};

		struct {
			// flags1
			DWORD dwVPage2 : 8;
			DWORD dwVPage1 : 6;
			DWORD dwVPage0 : 2;

			DWORD dwPPage2 : 8;
			DWORD dwPPage1 : 4;
			DWORD dwPPage0 : 3;
			DWORD bRes : 1;
			// flags2
			DWORD dwVPage4096 : 1;
			DWORD dwVPage8192 : 1;
			DWORD dwVPage16384 : 1;
			DWORD dwVPage32768 : 1;
			DWORD dwVPage65536 : 10;
			DWORD dwPPage4096 : 1;
			DWORD dwPPage8192 : 1;
			DWORD dwPPage16384 : 1;
			DWORD dwPPage32768 : 1;
			DWORD dwPPage65536 : 10;
			DWORD dwStartPage : 3;
			DWORD bUseExtendedSize : 1;
		};

		DWORD getVirtualSize();
		DWORD getPhysicalSize();
		DWORD getVBlockStart();
	};


	class datResourceChunk {
	public:
		DWORD pSrc;
		union {
			BYTE* pDest;
			DWORD dwData; // offset to this buffer in the global resource block
		};
		DWORD dwSize;

		datResourceChunk();
		~datResourceChunk();

	};

	class datResourceMap {
	public:

		datResourceMap() : nbVirtualCount(0), nbPhysicalCount(0), nbRootVirtualChunk(0), pAllocatedVPages(NULL), _f8(0) {}
		~datResourceMap();

		void printMap(int(__cdecl* print_message)(const char* msg, ...) = printf);

		BYTE nbVirtualCount;
		BYTE nbPhysicalCount;
		BYTE nbRootVirtualChunk;		// +3
		BYTE _f3;
		BYTE* pDataStart;			// +4
		DWORD _f8;
		BYTE* pAllocatedVPages;	// this->pAllocatedVPages = chunks[nbRootVirtualChunk]
		datResourceChunk chunks[128];		// +10

		void fillMap(BYTE* pRawResource);
		void generateMap(datResourceInfoOld* resInfo);
		void generateMap(datResourceInfo* resInfo);
		void validateMap(datResourceInfo* pInfo);

	};

	class datResource {
		void* _placeObj(void* pObj, void* ragePtr, __int64 size);
	
	public:
		datResourceMap* m_pMap;
		datResource* m_pNext;
		ConstString m_pszDebugName;
		bool m_bWasDefrag;
		BYTE __d[0x3]; // padding

		// 
		struct {
			rage::grcArray<void*>* m_pRagePtr;
			rage::grcArray<void*>* m_pRealPtr;
		} helper;

		datResource(const char* pszDebugName);
		//datResource();
		~datResource();

		// return NULL if not have or real ptr to obj
		void* checkPtrForDuplicate(void* ragePtr);
		void addPtrForDuplicate(void* ragePtr, void* realPtr);


		void saveRawResource(const char* pszPath, const char* pszName, bool bUseSysGfx);
		void saveResource(const char* pszPath, const char* pszName, const char *pszExt, DWORD dwVersion, datResourceInfo * pResInfo, BYTE nbCompressionIndex);

		template<class T> T* getFixup(T* ptr, __int64 dwSize/* = -1*/) {
			if (!ptr)
				return NULL;
			BYTE nbChunkCount = this->m_pMap->nbVirtualCount + m_pMap->nbPhysicalCount;

			BYTE nbBlockType = (DWORD)ptr >> 28;
			if (nbBlockType != 5 && nbBlockType != 6) {
				error("[datResource::getFixup] Resource '%s': Invalid fixup, block code %u is invalid", this->m_pszDebugName, (DWORD)nbBlockType);
				return NULL;
			}

			rage::datResourceChunk* chunk = this->m_pMap->chunks;
			for (BYTE i = 0; i < nbChunkCount; i++)
				if ((DWORD)ptr >= chunk->pSrc && (DWORD)ptr < chunk->pSrc + chunk->dwSize)
					if(dwSize == -1) return (T*)((DWORD)ptr - chunk->pSrc + chunk->pDest);
					else {
						size_t retVal = (size_t)((DWORD)ptr - chunk->pSrc + chunk->pDest);
						DWORD dwPosObjInPage = (DWORD)ptr - chunk->pSrc;
						if (dwPosObjInPage + dwSize > chunk->dwSize) {
							error("[datResource::getFixup] Resource '%s': object outside page boundaries. Page size = %u, position = %u, obj size = %u, code = %u, index = %u", 
								this->m_pszDebugName, 
								chunk->dwSize, 
								dwPosObjInPage, 
								(DWORD)dwSize, 
								chunk->pSrc >> 28, 
								i > this->m_pMap->nbVirtualCount? i  - this->m_pMap->nbVirtualCount : i);
							return NULL;
						}
						return (T*)retVal;
					}

				else chunk++;

			error("[datResource::getFixup] Resource '%s': Invalid fixup, address 0x%p is neither virtual nor physical", this->m_pszDebugName, ptr);
			return NULL;
		}

		// only for structures with 'm_usageCount'
		template <typename _Type>void fixupAndPlaceObj(_Type*& pObj, unsigned __int64 size = sizeof(_Type)) {
			if (void* result = checkPtrForDuplicate(pObj)) {
				pObj = (_Type*)result;
				pObj->m_usageCount++;
			}
			else {
				_Type* old = pObj;
			
				auto tmp = getFixup(pObj, size);
				pObj = new("datRes, fixupAndPlaceObj")_Type;

				addPtrForDuplicate(old, pObj);

				copy_class(pObj, tmp);
				
				pObj->m_usageCount = 1;

				if constexpr (helpers::hasPlace<_Type>)
					pObj->place(this);
			}

		}

	};

	class datResourceFileHeader {
	public:
		DWORD dwSignature;
		DWORD dwVersion;
		rage::datResourceInfo flags;

		datResourceFileHeader();
	};

}