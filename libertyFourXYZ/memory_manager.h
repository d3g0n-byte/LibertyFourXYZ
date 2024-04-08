#pragma once
#include <Windows.h>
#include "trace.h"
#include "helpers.h"
// changelog: 
// march 14, 2024: added names for allocation
// april 2, 2024: changes to most of the functionality, added operators for convenience
// april 3, 2024: (ptr_map) new search for objects by index, sorting, added resizing objects if necessary, removed unnecessary code
// april 3, 2024: (ptr_map) added quick sort(only the object that was added is sorted)
// april 4, 2024: destructor moved to "destroy" function
// april 8, 2024: (ptr_map) added radius in object search

namespace libertyFourXYZ {
	extern class class_memory_manager {
	private:
		static const int nameSize = 0x30;

		static struct item {
			size_t size;
#ifdef _DEBUG
			char szName[nameSize];
#endif // _DEBUG

			item(DWORD _dwSize, const char* pszName);
		};

		class ptr_map {
			struct HashPointerPair {
				size_t ptr;
				size_t item;
			};

			static int compareHashPairs(HashPointerPair* a, HashPointerPair* b);

			// unused
			void sortItems();

			// return: new slot index
			size_t addNewSlot();

			size_t alignment = 0x100;
		public:
			size_t* ptrs;
			item* items;
			size_t count;
			size_t capacity;

			ptr_map();
			~ptr_map();
	
			void insert(void* ptr, item itm);
			size_t getIndexByPtr(void* ptr);
			void erase(void* ptr);
			bool isValid(void* ptr) { return getIndexByPtr(ptr) != -1; }

		};

		ptr_map items;

		DWORD dwIncorrectUsesCount;
		size_t used;
		size_t maxUsed;


	public:
		class_memory_manager();
		~class_memory_manager();

		void destroy();
		bool isValid(void* ptr);
		int deletePtr(void* ptr);
		static size_t getUsedMemory();
		void* allocate(const char*, bool, size_t, size_t);
		bool size();
		void addIncorrectUsage();

	} g_memory_manager;

};

template <typename _Type> void dealloc(_Type*& pMem) {
	// to fix the problem I described in the discord server.
	if constexpr (helpers::hasUsageCount<_Type>) {
		if (pMem->m_usageCount == 0) {
			error("[MEMORY MANAGER] Unable to delete object. Usage count is zero");
			return;
		}
		pMem->m_usageCount--;
		if (pMem->m_usageCount == 0) {
//#ifndef _DEBUG
			libertyFourXYZ::g_memory_manager.deletePtr(pMem);
//#endif // !_DEBUG
			delete pMem;
		}
		pMem = NULL;
		return;
	}

//#ifndef _DEBUG
	libertyFourXYZ::g_memory_manager.deletePtr(pMem);
//#endif // !_DEBUG
	delete pMem;
	pMem = NULL;
}
template <typename _Type> void dealloc_arr(_Type*& pMem) {
//#ifndef _DEBUG
	libertyFourXYZ::g_memory_manager.deletePtr(pMem);
//#endif // !_DEBUG
	delete[] pMem;
	pMem = NULL;
}

void* operator new(size_t size, const char* pszMemName, size_t alignment = 0);
void* operator new[](size_t size, const char* pszMemName, size_t alignment = 0);
