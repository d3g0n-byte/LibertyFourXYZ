#include "memory_manager.h"

#include <vector>
#include <map>
#include "trace.h"
#include <psapi.h>

namespace libertyFourXYZ {

	class_memory_manager::item::item(/*void* _pAddres,*/ DWORD _dwSize, const char* _pszName) {
		//this->pAddres = _pAddres;
		dwSize = _dwSize;
#ifdef _DEBUG
		strcpy(this->szName, _pszName);
#endif // _DEBUG
	}

	class_memory_manager g_memory_manager;

	class_memory_manager::class_memory_manager() {
		this->dwIncorrectUsesCount = 0;
		this->used = 0;
		this->maxUsed = 0;
	}

	class_memory_manager::~class_memory_manager() {
		if (this->size()) {
			printf("[MEMORY MANAGER] Warning: Memory leak!\n");
			for (auto a : this->items)
#ifdef _DEBUG
				trace("[MEMORY MANAGER] Memory '%s' not released at address 0x%p. Size - %u", a.second.szName, a.first, a.second.dwSize);
#else
				trace("[MEMORY MANAGER] Memory not released at address 0x%p. Size - %u", a.first, a.second.dwSize);
#endif // _DEBUG

		}
		if (this->dwIncorrectUsesCount) {
			printf("[MEMORY MANAGER] Warning: %u attempts to access unreserved memory were detected. This needs to be fixed\n", this->dwIncorrectUsesCount);
		}

#ifdef _DEBUG
		DWORD dwUsedBytes = this->maxUsed;
		DWORD dwUsedGB = dwUsedBytes / 1000000000; dwUsedBytes %= 1000000000;
		DWORD dwUsedMB = dwUsedBytes / 1000000; dwUsedBytes %= 1000000;
		DWORD dwUsedKB = dwUsedBytes / 1000; dwUsedBytes %= 1000;

		if (this->maxUsed >= 1000000000)
			trace("[MEMORY MANAGER] Peak memory usage %d.%03d gb", dwUsedGB, dwUsedMB);
		else if (this->maxUsed > 1000000)
			//trace("peak memory usage %f mb", (double)this->maxUsed / 1000000);
			trace("[MEMORY MANAGER] Peak memory usage %d.%03d mb", dwUsedMB, dwUsedKB);
		else if (this->maxUsed > 1000)
			trace("[MEMORY MANAGER] Peak memory usage %d.%03d kb", dwUsedKB, dwUsedBytes);
		else  trace("[MEMORY MANAGER] Peak memory usage %d bytes", dwUsedBytes);
#endif // _DEBUG

	}

	bool class_memory_manager::isValid(void* ptr) { return class_memory_manager::items.count(ptr); }

	size_t class_memory_manager::getUsedMemory() {
		size_t retVal = 0;
		for (auto a : this->items)
			retVal+= a.second.dwSize;
		return retVal;
	}


	bool class_memory_manager::size() {
		return libertyFourXYZ::class_memory_manager::items.size();
	}


	size_t class_memory_manager::getTotalMemUsed() {
		PROCESS_MEMORY_COUNTERS_EX pmc;
		pmc.cb = sizeof(pmc);
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		return pmc.PrivateUsage;
	}

//	void* class_memory_manager::_allocate(const char* pszName, DWORD dwMemSize) {
//		if (!pszName) {
//			error("[MEMORY MANAGER] To allocate new memory, a name longer than 2 characters is required");
//			return NULL;
//		}
//		if (strlen(pszName) < 2 || strlen(pszName) >= nameSize) {
//			error("[MEMORY MANAGER] name '%s' is invalid", pszName);
//			return NULL;
//		}
//
//		if (!dwMemSize) {
//			error("[MEMORY MANAGER] Cannot allocate 0 bytes");
//			this->dwIncorrectUsesCount++;
//			return NULL;
//		}
//
//		BYTE* pObj = new BYTE[dwMemSize];
//		if (!pObj) {
//			error("[MEMORY MANAGER] Failed to allocate memory");
//			this->dwIncorrectUsesCount++;
//			return NULL;
//		}
//		//trace("[MEMORY MANAGER] %u bytes allocated", dwObjSize * dwCount);
//		items.insert(std::make_pair(pObj, dwMemSize));
//#ifdef _DEBUG
//		this->used += dwMemSize;
//		if (this->used > this->maxUsed)this->maxUsed = this->used;
//		trace("[MEMORY MANAGER] %u bytes allocated. Used: %d.%03d kb", dwMemSize, this->used / 1000, this->used % 1000);
//#else
//		trace("[MEMORY MANAGER] %u bytes allocated", dwObjSize * dwCount);
//		//memset((BYTE*)pObj, 0xcd, dwObjSize * dwCount);
//#endif // _DEBUG
//
//		return pObj;
//
//	}

};