#pragma once
#include <Windows.h>

#include "trace.h"

#include <map>


// changelog: 
// march 14, 2024: added names for allocation

namespace libertyFourXYZ {
	extern class class_memory_manager {
	private:
		static const int nameSize = 0x30;

		static struct item {
			//void* pAddres;
			DWORD dwSize;
#ifdef _DEBUG
			char szName[nameSize];
#endif // _DEBUG

			item(/*void* _pAddres, */DWORD _dwSize, const char* pszName);
			inline item() { error("class_memory_manager::item used"); exit(0); }
		};

		std::map<void*, item> items;

		DWORD dwIncorrectUsesCount;
		size_t used;
		size_t maxUsed;

	public:
		class_memory_manager();
		~class_memory_manager();

		bool isValid(void* ptr);
		size_t getUsedMemory();

		static size_t getTotalMemUsed();

		template<class T> T* allocate(const char* pszName, DWORD dwCount) {
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

			DWORD dwObjSize = sizeof(T);
			if (!dwObjSize || !dwCount) {
				error("[MEMORY MANAGER] Cannot allocate 0 bytes");
				this->dwIncorrectUsesCount++;
				return NULL;
			}

			T* pObj = new T[dwCount];
			if (!pObj) {
				error("[MEMORY MANAGER] Failed to allocate memory");
				this->dwIncorrectUsesCount++;
				return NULL;
			}
			//trace("[MEMORY MANAGER] %u bytes allocated", dwObjSize * dwCount);
			items.insert(std::make_pair(pObj, item(dwObjSize * dwCount, pszName)));
#ifdef _DEBUG
			this->used += dwObjSize * dwCount;
			if (this->used > this->maxUsed)this->maxUsed = this->used;
			trace("[MEMORY MANAGER] %u bytes allocated at 0x%p, name=%s. Used: %d.%03d kb",  dwObjSize * dwCount, pObj, pszName, this->used / 1000, this->used % 1000);
			
			//DWORD totalUsed = getTotalMemUsed();
			//trace("[MEMORY MANAGER] %u bytes allocated at 0x%p, name=%s. Used: %d.%03d kb. Total used: %d.%03d kb",  dwObjSize * dwCount, pObj, pszName, 
			//	this->used / 1000, this->used % 1000, totalUsed / 1000, totalUsed % 1000);
#else
			trace("[MEMORY MANAGER] %u bytes allocated", dwObjSize * dwCount);
			//memset((BYTE*)pObj, 0xcd, dwObjSize * dwCount);
#endif // _DEBUG

			return pObj;
		}
		template<class T> T* allocate(const char* pszName) { return this->allocate<T>(pszName, 1); }

		// clear ans set ptr to NULL
		template<class T> void release(T*& ptr) {
			if (!ptr) {
				error("[MEMORY MANAGER] NULL pointer skipped");
				this->dwIncorrectUsesCount++;
				return;
			}
			/*else */if (!class_memory_manager::isValid(ptr)) {
				error("[MEMORY MANAGER] Memory not found");
				this->dwIncorrectUsesCount++;
				return;
			}
			size_t ptr2 = (size_t)ptr;
			delete[] ptr;
			ptr = NULL;

			DWORD dwSize = this->items[(void*)ptr2].dwSize;
			//auto o = this->items[(void*)ptr2];
			//DWORD dwSize = 0;
			items.erase((T*)ptr2);
			//trace("[MEMORY MANAGER] %u bytes cleared", items[(T*)ptr2]);
#ifdef _DEBUG
			this->used -= dwSize;
			trace("[MEMORY MANAGER] %u bytes cleared. Used: %d.%03d kb", dwSize, this->used / 1000, this->used % 1000);
			//DWORD totalUsed = getTotalMemUsed();
			//trace("[MEMORY MANAGER] %u bytes cleared. Used: %d.%03d kb. Total used: %d.%03d kb", dwSize,
			//	this->used / 1000, this->used % 1000, totalUsed / 1000, totalUsed % 1000);

#else
			trace("[MEMORY MANAGER] %u bytes cleared", dwSize);
#endif // _DEBUG

		}

		bool size();

	} g_memory_manager;

	//extern class_memory_manager g_memory_manager;

};