#pragma once

#include "rage_datResource.h"
#include "helpers.h"
#include "utils.h"

namespace rage {

	static const DWORD g_dwAlignCapacityTo = 4; // use: only 4, 16, < 2 is off

	template <typename T1, int Align = 0, typename T2 = WORD> class atArray {

		void alignCapacity() {
			//if (g_dwAlignCapacityTo < 2 || sizeof (T1) > 4) return;
			//auto n = g_dwAlignCapacityTo - 1;
			//this->m_capacity = (this->m_capacity + n) & ~n;
		}

		// return: slot index
		T2 addSlot() {
			if (this->m_count + 1 < this->m_count) {
				error("[atArray] Unable to add element. Max array size reached");
				return this->m_count - 1;
			}

			if (this->m_count < this->m_capacity) { // if arr have a free slot
				this->m_count++;
				return this->m_count - 1;
			}
			else {
				T1* pOld = NULL;
				T2 oldSize = 0;
				if (this->m_capacity > 0) { pOld = this->m_pElements; oldSize = this->m_count; }
				this->m_count = ++this->m_capacity;
				this->alignCapacity();
				
				this->m_pElements = new("arArr, addSlot")T1[this->m_capacity];

				if (pOld) {
					memcpy(this->m_pElements, pOld, sizeof T1 * oldSize);

					// without this, it deletes the old memory by running the destructor, which should not be done to avoid losing data in the array.\
					   how to fix it?
					memset(pOld, 0x0, sizeof T1 * oldSize);
					dealloc_arr(pOld); // ToDo: fix

				}

				return oldSize;
			}
		}


	public:
		T1* m_pElements;
		T2 m_count;
		T2 m_capacity;

		void destroy() {
			if (this->m_pElements && this->m_capacity)
				dealloc_arr(this->m_pElements);
			this->m_count = this->m_capacity = 0;
		}

		void initialize(T2 elementsCount) {
			this->m_count = elementsCount;
			this->m_capacity = this->m_count;
			if (this->m_capacity > 0)
				this->m_pElements = new("at array, initialize")T1 [m_capacity];
			else
				this->m_pElements = NULL;
		}

		~atArray() { this->destroy(); }
		atArray() : m_pElements(NULL), m_count(0), m_capacity(0) { }
		atArray(T2 elementsCount) { this->initialize(elementsCount); }
		atArray(T2 elementsCount, T1 element) { 
			this->initialize(elementsCount); 
			for (T2 i = 0; i < elementsCount; i++)
				this->m_pElements[i] = element;
		}
	
		bool indexIsValid(T2 index) {
			return !(index >= this->m_count || index < 0);
		}

		//void add(T1& element) {
		//	T2 slotIndex = this->addSlot();
		//	this->m_pElements[slotIndex] = element;
		//}

		void add(T1 element) {
			T2 slotIndex = this->addSlot();
			this->m_pElements[slotIndex] = element;
		}

		// add element by element pointer. Redirects the pointer to a new address
		void add(T1*& pElement) {
			T2 slotIndex = this->addSlot();
			memcpy(&this->m_pElements[slotIndex], pElement, sizeof * pElement);

			zero_fill_class(pElement);
			dealloc(pElement);

			pElement = this->m_pElements + slotIndex;

		}

		void remove(T2 elementIndex) {
			if (elementIndex + 1 > this->m_count || !this->indexIsValid(elementIndex)) {
				error("[atArray] Unable to remove %ull element. Index is out of bounds", (unsigned __int64)elementIndex);
				return;
			}

			T2 newCount = this->m_count - 1;

			bool bIsFirstElement = 0;
			bool bIsLastElement = 0;
			if (elementIndex == 0)
				bIsFirstElement = 1;
			else if (elementIndex == newCount)
				bIsLastElement = 1;

			T1* pOldMem = this->m_pElements;
			bool bDeleteOldMem = 0;

			if (this->m_capacity - newCount >= 4) {
				this->m_pElements = new("atArray, resize")T1[newCount];
				this->m_capacity = newCount;
				bDeleteOldMem = 1;
			}

			if (bIsFirstElement)
				memcpy(this->m_pElements, pOldMem + 1, newCount * sizeof T1);
			else if (bIsLastElement)
				memcpy(this->m_pElements, pOldMem, newCount * sizeof T1);
			else {
				memcpy(this->m_pElements, pOldMem, elementIndex * sizeof T1);
				memcpy(this->m_pElements - elementIndex, pOldMem + elementIndex + 1, (newCount - elementIndex) * sizeof(T1));
			}

			if(bDeleteOldMem) dealloc_arr(pOldMem);
			this->m_count = newCount;
		}
		
		void removeLast() { this->remove(this->m_count - 1); }
		T1& back() { return *(this->m_pElements + this->m_count - 1); }

		DWORD getSizeInBytes() { return sizeof T1 * this->m_count; }
		DWORD getCapacityInBytes() { return sizeof T1 * this->m_capacity; }
		T2 size() { return this->m_count; }
		T2 capacity() { return this->m_capacity; }
		T1* getElements() { return this->m_pElements; }
		T1* data() { return this->getElements(); } // 
		T1* get(T2 elementIndex) {
			if (!this->indexIsValid(elementIndex)) {
				trace("[atArray] Unable to get %ull element. Index is out of bounds", (unsigned __int64)elementIndex);
				return NULL;
			}
			return this->getElements() + elementIndex;
		}

		T1& operator[](T2 elementIndex) { return *get(elementIndex); }

		void place(rage::datResource* rsc) {
			T1* tmp = rsc->getFixup<T1>(this->m_pElements, sizeof(T1) * this->m_capacity); // we get a pointer to the elements and remember it
			this->m_pElements = new("atArr, place1") T1[this->m_count]; // request new memory for elements

			copy_class(this->m_pElements, tmp, this->m_count); // copy the memory from the resource to our array of elements. 'copy_class' is used to prevent _vmt values ​​from being replaced

			if constexpr (helpers::hasPlace<T1>) // if there is a 'place' function, we will execute it for each object.
				for (T2 i = 0; i < this->m_count; i++)
					this->m_pElements[i].place(rsc);
		}

		//template<typename Type> void place_1ptr(rage::datResource* rsc, Type *p) {
		//	T1* tmp = rsc->getFixup<T1>(this->m_pElements, sizeof(T1) * this->m_capacity); // we get a pointer to the elements and remember it
		//	this->m_pElements = new("atArr, place2") T1[this->m_count]; // request new memory for elements

		//	copy_class(this->m_pElements, tmp, this->m_count); // copy the memory from the resource to our array of elements. 'copy_class' is used to prevent _vmt values ​​from being replaced

		//	for (T2 i = 0; i < this->m_count; i++)
		//		this->m_pElements[i].place_1ptr(rsc, p);
		//}


		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
			pLayout->addObject<T1>((T1*)this->getElements(), 5, this->m_capacity);

			if constexpr (helpers::hasAddToLayout<T1>)
				for (T2 i = 0; i < this->m_count; i++)
					this->m_pElements[i].addToLayout(pLayout, dwDepth + 1);
		}

		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
			auto pElements = this->m_pElements;

			if constexpr (helpers::hasReplacePtrs<T1>)
				for (T2 i = 0; i < this->m_count; i++)
					this->m_pElements[i].replacePtrs(pLayout, pRsc, dwDepth + 1);

			pLayout->setPtr<T1>(this->m_pElements);
			memcpy(pRsc->getFixup<T1>(this->m_pElements, this->getCapacityInBytes()), pElements, this->getCapacityInBytes());
		}

		void clearRefCount() {
			if (helpers::hasClearUsageCount<T1>)
				for (auto &element : *this)
					element.clearRefCount();
		}

		void setRefCount() {
			if (helpers::hasSetUsageCount<T1>)
				for (auto &element : *this)
					element.setRefCount();
		}

		T1* begin() { return this->m_pElements; }
		T1* end() { return this->m_pElements + this->m_count; }
	};

	template <typename T1, DWORD dwCount> class atRangeArray {
	public:
		T1 m_elements[dwCount];

		T1* getElements() { return this->m_elements; }

		T1* get(DWORD elementIndex) {
			if (elementIndex >= dwCount) {
				trace("[atRangeArray] Unable to get %ull element. Index is out of bounds", (unsigned __int64)elementIndex);
				return NULL;
			}
			return this->getElements() + elementIndex;
		}

		DWORD size() { return dwCount; }

		T1& operator[](DWORD elementIndex) { return *get(elementIndex); }
	
		T1* begin() { return this->m_pElements; }
		T1* end() { return this->m_pElements + size(); }

	};

	template <typename T> class grcArray : public rage::atArray<T, 0, WORD> { }; 

	template <typename T1, int count> class atFixedArray {
	public:
		T1 m_elements[count];
		int nCount;

		T1* getElements() { return this->m_elements; }

		T1* get(DWORD elementIndex) {
			if (elementIndex >= nCount) {
				trace("[atFixedArray] Unable to get %ull element. Index is out of bounds", (unsigned __int64)elementIndex);
				return NULL;
			}
			return this->getElements() + elementIndex;
		}

		T1& operator[](DWORD elementIndex) { return *get(elementIndex); }

		T1* begin() { return this->m_pElements; }
		T1* end() { return this->m_pElements + this->nCount; }

	};


}