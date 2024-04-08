#pragma once
#include <Windows.h>
#include "rage_datResource.h"

#include "rage_array.h"

namespace rage {
	class datBase {
	public:
		__forceinline datBase() {}
		inline virtual ~datBase() {}
		inline void place(rage::datResource* rsc) {}
		inline /*virtual*/ void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {}
		inline /*virtual*/ void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {}
		inline void clearRefCount(){}
		inline void setRefCount(){}
		};

	class pgBasePageMap {
	public:
		BYTE nbVCount;
		BYTE nbPCount;
		BYTE __2[2];
		union {
			BYTE* pData[0x80]; // real ptr
			DWORD dwData[0x80]; // offset in rsc
		};

		void fillMap(rage::datResource* rsc);
	};

	class pgBase : public datBase {
	public:
		pgBasePageMap* m_pPageMap;

		pgBase();
		~pgBase();
		
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void place(rage::datResource* rsc);
		void clearRefCount();
		void setRefCount();
		void allocatePageMap();

	};

	// from rdr
	class pgBaseRefCounted : public pgBase {
	public:
		DWORD m_usageCount;
		
		void clearRefCount();
		void setRefCount();
	};

	// this is something like a smart pointer from rage
	template <typename T> class datOwner {
		// do not use this function. It is unsafe and can lead to memory leaks if used incorrectly
		void setElement(T*&element) { memcpy(this, &element, sizeof(size_t)); }
	public:
		T* pElement;
		
		// get a pointer to an element
		T* get() { return this->pElement; }
		datOwner(T* element) : pElement(NULL) { this->setElement(element); }
		datOwner() { this->pElement = NULL; }
		bool isNotEmpty() { return this->pElement; }

		void allocate() { this->destroy() ; this->pElement = new("dawOwner, allocate") T(); }
		void destroy() { if (this->isNotEmpty()) dealloc(this->pElement); }

		~datOwner() { destroy(); }
	
		rage::datOwner<T>& operator=(T* n) {
			this->destroy();
			this->setElement(n);
			return *this;
		}

		void place(rage::datResource* rsc) { 
			if constexpr (helpers::hasUsageCount<T>)
				rsc->fixupAndPlaceObj(pElement);
			else {
				auto tmp = rsc->getFixup(this->pElement, sizeof(T));
				this->pElement = new("datOwner, place, element")T;
				copy_class(this->pElement, tmp);
				if constexpr (helpers::hasPlace<T>)
					this->pElement->place(rsc);
			}
		}

		//template<typename Type1> void place_1ptr(rage::datResource* rsc, Type1*p) {
		//	auto tmp = rsc->getFixup(this->pElement, sizeof(T));
		//	this->pElement = new("datOwner, place1ptr")T;
		//	copy_class(this->pElement, tmp);
		//
		//		this->pElement->place(rsc, p);
		//}

		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
			//if constexpr (helpers::hasUsageCount<T>)
			//	helpers::addObjectWithCounterToLayout(pLayout, 5, dwDepth + 1, pElement);
			//else {
				pLayout->addObject<T>(this->pElement, 5);
				if constexpr (helpers::hasAddToLayout<T>)
					this->pElement->addToLayout(pLayout, dwDepth + 1);
			//}
		}

		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
			//if constexpr (helpers::hasUsageCount<T>) {
			//	helpers::replacePtrsInObjWithCounter(pLayout, pRsc, dwDepth + 1, pElement);
			//}
			//else {
				if constexpr (helpers::hasReplacePtrs<T>)
					this->pElement->replacePtrs(pLayout, pRsc, dwDepth + 1);
				auto _pElement = this->pElement;
				pLayout->setPtr<T>(this->pElement);

				memcpy(pRsc->getFixup<T>(this->pElement, sizeof(*_pElement)), _pElement, sizeof(*_pElement));
			//}
		}

		void clearRefCount() {
			if (helpers::hasClearUsageCount<T>)
					this->pElement->clearRefCount();
		}

		void setRefCount() {
			if (helpers::hasSetUsageCount<T>)
				this->pElement->setRefCount();
		}
	};

	template <typename T> class datRef {
	public:
		T* pElement;

		T& operator[](DWORD dwElementIndex) { return *(this->pElement + dwElementIndex); }


	};



	template <class T> class pgDictionary : public pgBase {
	protected:
		struct HashPointerPair {
			DWORD   dwHash;
			size_t ptr;
		};

		static int compareHashPairs(HashPointerPair* a, HashPointerPair* b) {
			if (a->dwHash > b->dwHash)
				return 1;
			return a->dwHash == b->dwHash ? 0 : -1;
		};

	public:
		pgDictionary<T>* m_pParent;
		DWORD m_usageCount;
		rage::atArray<DWORD> m_codes;
		rage::atArray<rage::datOwner<T>> m_entries;

		pgDictionary() {
			this->m_pPageMap = NULL;
			this->m_pParent = NULL;
			this->m_usageCount = 0;
		}

		~pgDictionary() {
			//if (!this->m_usageCount)
			//	return;

			this->m_usageCount = 0;

		}

		void addElement(DWORD dwHash, T* pEntry) {

			this->m_codes.add(dwHash);
			rage::datOwner<T>* obj = new("pgDict, add element") datOwner<T>;
			//trace("0x10x", obj);
			*obj = pEntry;

			//WORD newSlot = -1;
			//bool bIsSorted = 0;
			//for (WORD i = 0; i < m_codes.m_count; i++) {
			//	if (dwHash < m_codes[i]) {
			//		newSlot = i;
			//		bIsSorted = 1;
			//		break;
			//	}
			//}

			this->m_entries.add(obj);
			
			//// ToDo: fix
			//zero_fill_class(obj);
			//libertyFourXYZ::g_memory_manager.release(obj);

			//// we do partial sorting. Total sorting is too expensive
			//if (bIsSorted) {
			//	DWORD hash = m_codes.back();
			//	T* hash = &m_entries.back();
			//	if(newSlot == 0)
			//}

			sortItem();


		}

		T* getElementAddresByHash(DWORD dwHash) {
			//for (WORD i = 0; i < this->m_codes.m_count; i++) if (this->m_codes[i] == dwHash) return this->m_entries[i].pElement;
			
			// to use it for our purposes, we need to do a quick hash search, without a total search
			WORD left = 0;
			WORD right = this->m_codes.m_count;
			WORD mid;

			while (left <= right) {
				mid = (left + right) / 2;
				if (dwHash < this->m_codes[mid]) right = mid - 1;
				else if (dwHash > this->m_codes[mid]) left = mid + 1;
				else return this->m_entries[mid].pElement;
			}
			error("[pgDictionary] Object with hash %#010x not found", dwHash);
			return NULL;
		}

		bool hasElementByHash(DWORD dwHash) {
			return this->getElementAddresByHash(dwHash) != NULL;
		}

		bool hasElementByName(const char* pszName) {
			return this->hasElementByHash(atStringHash(pszName));
		}

		//T& getElementByHash(DWORD dwHash) {
		//	return *getElementAddresByHash(dwHash);
		//}

		//T& getElementByName(const char* pszName) {
		//	return *getElementAddresByHash(dwHash);
		//}

		void deleteElement(WORD wIndex) {
			if (wIndex >= this->m_entries.m_count) {
				error("[pgDictionary] Object with index %u not found", (DWORD)wIndex);
				return;
			}
			this->m_codes.remove(wIndex);
			this->m_entries.remove(wIndex);
		}

		//rage::pgDictionary<T>& operator=(rage::pgDictionary<T>& pDict) {
		//	pgBase::operator=(pDict);
		//	this->m_dwRefCount = pDict.m_dwRefCount;
		//	this->m_pParent = NULL;
		//	this->m_codes = pDict.m_codes;
		//	this->m_entries = pDict.m_entries;
		//	return *this;
		//}

		int compare(const void* arg1, const void* arg2) {
			return _stricmp(*(char**)arg1, *(char**)arg2);
		}

		void sortItem() {   // make hashes and sort entries by hash
			if (!this->m_entries.size() || !this->m_codes.size())
				return;
			HashPointerPair* pairs = new("pgDict, sort, pair") HashPointerPair[this->m_entries.size()];
			for (size_t i = 0; i < this->m_codes.m_count; i++) {
				pairs[i].dwHash = this->m_codes[i];
				memcpy(&pairs[i].ptr, &this->m_entries[i], sizeof size_t);
			}
			qsort(pairs, this->m_codes.m_count, sizeof(HashPointerPair), (int(__cdecl*)(const void*, const void*)) & pgDictionary<T>::compareHashPairs);
			for (DWORD i = 0; i < this->m_codes.m_count; i++) {
				this->m_codes[i] = pairs[i].dwHash;
				memcpy(&this->m_entries[i], &pairs[i].ptr, sizeof size_t);
			}
			dealloc_arr(pairs);
		}

		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
			pgBase::addToLayout(pLayout, dwDepth);

			//if (!this->m_usageCount)
			//	return;
			//if (this->m_pParent) {
			//	pLayout->addObject<rage::pgDictionary<T>>(this->m_pParent, 5);
			//	this->m_pParent->addToLayout(pLayout, dwDepth + 1);
			//}
			this->m_codes.addToLayout(pLayout, dwDepth);
			this->m_entries.addToLayout(pLayout, dwDepth);
		}

		void place(rage::datResource* rsc) {
			pgBase::place(rsc);

			if (this->m_pPageMap) this->m_pPageMap = NULL;
			if (this->m_pParent) this->m_pParent = NULL;
			this->m_usageCount = 1;

			this->m_codes.place(rsc);
			this->m_entries.place(rsc);
		}

		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
			pgBase::replacePtrs(pLayout, pRsc, dwDepth);

			if (this->m_pParent) {
				this->m_pParent = NULL;
			}
			this->m_codes.replacePtrs(pLayout, pRsc, dwDepth);
			this->m_entries.replacePtrs(pLayout, pRsc, dwDepth);

		}

		void clearRefCount() {
			pgBase::clearRefCount();
			this->m_usageCount = 0;
			this->m_entries.clearRefCount();
		}

		void setRefCount() {
			pgBase::setRefCount();
			this->m_usageCount++;
			this->m_entries.setRefCount();
		}


	};
}