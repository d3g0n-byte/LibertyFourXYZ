#include "memory_manager.h"

#include <psapi.h>
#include "trace.h"
#include <stdio.h>
#include <new>
#include <iostream>

namespace libertyFourXYZ {

	class_memory_manager::item::item(/*void* _pAddres,*/ DWORD _dwSize, const char* _pszName) {
		//this->pAddres = _pAddres;
		size = _dwSize;
#ifdef _DEBUG
		strcpy(this->szName, _pszName);
#endif // _DEBUG
	}

	class_memory_manager g_memory_manager;

	void class_memory_manager::destroy() { }

	void _destroy() {
		g_memory_manager.destroy();
	}

	class_memory_manager::class_memory_manager() {
		atexit(_destroy);
		this->dwIncorrectUsesCount = 0;
		this->used = 0;
		this->maxUsed = 0;
	}

	class_memory_manager::~class_memory_manager() {
		if (this->size()) {
//			error("[MEMORY MANAGER] Warning: Memory leak!");
//			for (size_t i = 0; i < this->items.count; i++) {
////#ifdef _DEBUG
//				error("[MEMORY MANAGER] Memory '%s' not released at address 0x%p. Size - %u", this->items.items[i].szName, (void*)this->items.ptrs[i], this->items.items[i].size);
////#else
////				error("[MEMORY MANAGER] Memory not released at address 0x%p. Size - %u", (void*)this->items.ptrs[i], this->items.items[i].size);
////#endif // _DEBUG
//			}
		}
		if (this->dwIncorrectUsesCount) // ??
			error("[MEMORY MANAGER] Warning: %u attempts to access unreserved memory were detected. This needs to be fixed\n", this->dwIncorrectUsesCount);

#ifdef _DEBUG
		DWORD dwUsedBytes = this->maxUsed;
		DWORD dwUsedGB = dwUsedBytes / 1000000000; dwUsedBytes %= 1000000000;
		DWORD dwUsedMB = dwUsedBytes / 1000000; dwUsedBytes %= 1000000;
		DWORD dwUsedKB = dwUsedBytes / 1000; dwUsedBytes %= 1000;

		if (this->maxUsed >= 1000000000)
			error("[MEMORY MANAGER] Peak memory usage %d.%03d gb", dwUsedGB, dwUsedMB);
		else if (this->maxUsed > 1000000)
			//trace("peak memory usage %f mb", (double)this->maxUsed / 1000000);
			error("[MEMORY MANAGER] Peak memory usage %d.%03d mb", dwUsedMB, dwUsedKB);
		else if (this->maxUsed > 1000)
			error("[MEMORY MANAGER] Peak memory usage %d.%03d kb", dwUsedKB, dwUsedBytes);
		else  error("[MEMORY MANAGER] Peak memory usage %d bytes", dwUsedBytes);
#endif // _DEBUG
	}

	bool class_memory_manager::isValid(void* ptr) { return this->items.isValid(ptr); }

	bool class_memory_manager::size() {
		return items.count;
	}

	void class_memory_manager::addIncorrectUsage() {
		dwIncorrectUsesCount++;
	}

	size_t class_memory_manager::getUsedMemory() {
		PROCESS_MEMORY_COUNTERS_EX pmc;
		pmc.cb = sizeof(pmc);
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		return pmc.PrivateUsage;
	}

	void* class_memory_manager::allocate(const char* pszName, bool bReg, size_t size, size_t alignment) {
#ifdef _DEBUG
		if (!pszName) {
			error("[MEMORY MANAGER] To allocate new memory, a name longer than 2 characters is required");
			return NULL;
		}
		if (strlen(pszName) < 2 || strlen(pszName) >= nameSize) {
			error("[MEMORY MANAGER] name '%s' is invalid", pszName);
			return NULL;
		}
#endif // _DEBUG

		if (!size) {
			error("[MEMORY MANAGER] Cannot allocate 0 bytes");
			this->dwIncorrectUsesCount++;
			return NULL;
		}

		void* mem = NULL;
		if (alignment == 0)
			mem = malloc(size);
		else
			if (alignment && !(alignment & (alignment - 1)))
				mem = _aligned_malloc(size, alignment);
			else
				error("[MEMORY MANAGER] %u is not a power of two", (DWORD)alignment);

		if (!mem) {
			error("[MEMORY MANAGER] Failed to allocate memory");
			this->dwIncorrectUsesCount++;
			return NULL;
		}
		if(bReg)
			items.insert(mem, item(size, pszName));
#ifdef _DEBUG
		//this->used += size;
		this->used = class_memory_manager::getUsedMemory();
		if (this->used > this->maxUsed)this->maxUsed = this->used;
	//	trace("[MEMORY MANAGER] %u bytes allocated at 0x%p, name=%s. Used: %d.%03d kb", (DWORD)size, mem, pszName, this->used / 1000, this->used % 1000);

		//DWORD totalUsed = getTotalMemUsed();
		//trace("[MEMORY MANAGER] %u bytes allocated at 0x%p, name=%s. Used: %d.%03d kb. Total used: %d.%03d kb",  dwObjSize * dwCount, pObj, pszName, 
		//	this->used / 1000, this->used % 1000, totalUsed / 1000, totalUsed % 1000);
#else
	//	trace("[MEMORY MANAGER] %u bytes allocated", (DWORD)size);
		//memset((BYTE*)pObj, 0xcd, dwObjSize * dwCount);
#endif // _DEBUG

		return mem;
	}

	int class_memory_manager::deletePtr(void* ptr) {
		size_t index = items.getIndexByPtr(ptr);
		if (index != -1) {
			this->used -= items.items[index].size;
			items.erase(ptr);
		}
		return 0;
	}

	class_memory_manager::ptr_map::ptr_map() : ptrs(NULL), items(NULL), count(0), capacity(0) {}
	class_memory_manager::ptr_map::~ptr_map() {
		if (count) {
			error("[MEMORY MANAGER] Warning: Memory leak!");
			for (size_t i = 0; i < count; i++) {
#ifdef _DEBUG
				error("[MEMORY MANAGER] Memory '%s' not released at address 0x%p. Size - %u", items[i].szName, (void*)ptrs[i], items[i].size);
#else
				error("[MEMORY MANAGER] Memory not released at address 0x%p. Size - %u", (void*)ptrs[i], items[i].size);
#endif // _DEBUG
			}
		}

		if (capacity) {
			free(ptrs);
			free(items);
			//capacity = 0;
			//count = 0;
			//ptrs = NULL;
			//items = NULL;
		}
	}

	void class_memory_manager::ptr_map::erase(void* ptr) {
		size_t index = getIndexByPtr(ptr);

		size_t newCount = count - 1;

		size_t newPtrsMemSize = sizeof(*ptrs) * newCount;
		size_t newItemsMemSize = sizeof(*items) * newCount;
		bool bIsLast = index == newCount;
		bool bIsFirst = index == 0;

		if (bIsFirst) {
			memcpy(ptrs, ptrs + 1, newPtrsMemSize);
			memcpy(items, items + 1, newItemsMemSize);
		}
		else if (bIsLast) {
			// nothing needs to be done unless we reduce capacity
		}
		else {
			memcpy(ptrs + index, ptrs + index + 1, (newCount - index) * sizeof(*ptrs));
			memcpy(items + index, items + index + 1, (newCount - index) * sizeof(*items));
		}
		count = newCount;

		size_t num = alignment - 1;
		size_t newCapacity = (newCount + num) & ~num;
		if (newCapacity < capacity) {
			newPtrsMemSize = newCapacity * sizeof(*ptrs);
			newItemsMemSize = newCapacity * sizeof(*items);
			void* newPtrsMem = malloc(newPtrsMemSize);
			void* newItemsMem = malloc(newItemsMemSize);
			if (!newPtrsMem || !newItemsMem) { error("[MEMORY MANAGER] map error"); return; }
			memcpy(newPtrsMem, ptrs, newPtrsMemSize);
			memcpy(newItemsMem, items, newItemsMemSize);
			free(ptrs);
			free(items);
			ptrs = (size_t*)newPtrsMem;
			items = (item*)newItemsMem;
			capacity = newCapacity;
		}
	}

	int class_memory_manager::ptr_map::compareHashPairs(HashPointerPair* a, HashPointerPair* b) {
		if (a->ptr > b->ptr)
			return 1;
		return a->ptr == b->ptr ? 0 : -1;
	};

	void class_memory_manager::ptr_map::sortItems() {
		if (!count)
			return;
		HashPointerPair* pairs = (HashPointerPair*)malloc(sizeof(HashPointerPair) * count);
		for (size_t i = 0; i < count; i++) {
			memcpy(&pairs[i].ptr, &ptrs[i], sizeof size_t);
			memcpy(&pairs[i].item, &items[i], sizeof size_t);
		}
		qsort(pairs, count, sizeof(HashPointerPair), (int(__cdecl*)(const void*, const void*)) & compareHashPairs);
		for (DWORD i = 0; i < count; i++) {
			memcpy(&ptrs[i], &pairs[i].ptr, sizeof size_t);
			memcpy(&items[i], &pairs[i].item, sizeof size_t);
		}
		delete[] pairs;
	}

	size_t class_memory_manager::ptr_map::getIndexByPtr(void* ptr) {
		size_t left = 0;
		size_t right = count - 1;
		size_t mid;

		while (left <= right) {
			mid = (left + right) / 2;
			if (mid >= count) return -1;

			if ((size_t)ptr >= ptrs[mid] && (size_t)ptr < ptrs[mid] + items[mid].size) // ??
				return mid;

			if ((size_t)ptr < ptrs[mid]) right = mid - 1;
			else if ((size_t)ptr > ptrs[mid]) left = mid + 1;
			else return mid;
		}
		return -1;
	}

	void class_memory_manager::ptr_map::insert(void* ptr, item itm) {
		size_t slot = addNewSlot();

		bool bIsSorted = 0;

		for (size_t i = 0; i < count - 1; i++) {
			if ((size_t)ptr < ptrs[i]) {
				slot = i;
				bIsSorted = 1;
				break;
			}
		}
		if (bIsSorted) {
			size_t currentOld = count;
			size_t currentNew = currentOld - 1;
			do {
				currentOld--;
				currentNew--;
				ptrs[currentOld] = ptrs[currentNew];
				items[currentOld] = items[currentNew];
			} while (currentOld != 0 && currentOld > slot + 1);
		}
		ptrs[slot] = (size_t)ptr;
		items[slot] = itm;
	}

	size_t class_memory_manager::ptr_map::addNewSlot() {
		size_t retVal = count;
		size_t newCount = count + 1;
		if (newCount > capacity) {
			size_t num = alignment - 1;
			size_t newCapacity = (newCount + num) & ~num;

			size_t newPtrsMemSize = sizeof(*ptrs) * newCapacity;
			size_t newItemsMemSize = sizeof(*items) * newCapacity;

			size_t oldPtrsMemSize = sizeof(*ptrs) * capacity;
			size_t oldItemsMemSize = sizeof(*items) * capacity;

			//void* newMem = realloc()
			void* newPtrsMem = malloc(newPtrsMemSize);
			memmove(newPtrsMem, ptrs, oldPtrsMemSize);
			free(ptrs);
			ptrs = (size_t*)newPtrsMem;

			void* newItemsMem = malloc(newItemsMemSize);
			memmove(newItemsMem, items, oldItemsMemSize);
			free(items);
			items = (item*)newItemsMem;


			capacity = newCapacity;
		}
		count = newCount;
		return retVal;
	}

};

void* operator new(size_t size, const char* pszMemName, size_t alignment) {
	return libertyFourXYZ::g_memory_manager.allocate(pszMemName, 1, size, alignment);
}

void* operator new[](size_t size, const char* pszMemName, size_t alignment) {
	return libertyFourXYZ::g_memory_manager.allocate(pszMemName, 1, size, alignment);
}


//#ifdef _DEBUG
//
//void* operator new(size_t size) {
////#ifndef _DEBUG
////	bool bReg = 0;
////#else
////	bool bReg = 1;
////#endif // !_DEBUG
//	return libertyFourXYZ::g_memory_manager.allocate("unnamed", 0, size, 0);
//}
//
//void* operator new[](size_t size) {
////#ifndef _DEBUG
////	bool bReg = 0;
////#else
////	bool bReg = 1;
////#endif // !_DEBUG
//	return libertyFourXYZ::g_memory_manager.allocate("unnamed", 0, size, 0);
//}
//
//
//
//	void operator delete(void* _Block) noexcept {
////#ifdef _DEBUG
//		libertyFourXYZ::g_memory_manager.deletePtr(_Block);
////#endif // !_DEBUG
//		free(_Block);
//	}
//
//	void operator delete[](void* _Block) noexcept {
//#//ifdef _DEBUG
//		libertyFourXYZ::g_memory_manager.deletePtr(_Block);
////#endif // !_DEBUG
//		free(_Block);
//		}
//
//
//		// ToDo
//			void* operator new(size_t size, ::std::nothrow_t const&) noexcept {
////#ifndef _DEBUG
////			bool bReg = 0;
////#else
////			bool bReg = 1;
////#endif // !_DEBUG
//			return libertyFourXYZ::g_memory_manager.allocate("unnamed", 0, size, 0);
//		}
//
//		void* operator new[](size_t size, ::std::nothrow_t const&) noexcept {
////#ifndef _DEBUG
////			bool bReg = 0;
////#else
////			bool bReg = 1;
////#endif // !_DEBUG
//			return libertyFourXYZ::g_memory_manager.allocate("unnamed",0, size, 0);
//			}
//
//			void operator delete(void* _Block, ::std::nothrow_t const&) noexcept {
////#ifdef _DEBUG
//				libertyFourXYZ::g_memory_manager.deletePtr(_Block);
////#endif // !_DEBUG
//				free(_Block);
//			}
//
//			void operator delete[](void* _Block, ::std::nothrow_t const&) noexcept {
////#ifdef _DEBUG
//				libertyFourXYZ::g_memory_manager.deletePtr(_Block);
////#endif // !_DEBUG
//				free(_Block);
//				}
//
//#endif // !_DEBUG

