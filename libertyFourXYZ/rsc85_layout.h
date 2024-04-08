#pragma once
#include <Windows.h>
#include <map>
#include "trace.h"

namespace rage { union datResourceInfo; template <typename T> class grcArray; };
namespace libertyFourXYZ { extern bool g_bForceUseRsc5; }

namespace libertyFourXYZ {
	class rsc85_layout {
	private:

		struct rsc85_object {
			void* ptr;
			DWORD dwSize;
			DWORD dwArrayCount;

			DWORD dwBlockType : 4;
			DWORD bArray : 1;
			DWORD unused : 27;

			DWORD getSize();
			rsc85_object(void* _ptr, DWORD _dwSize, DWORD _dwArrayCount, bool _bArray, DWORD _dwBlockType);
			inline rsc85_object() {};
		};
		struct rsc85_pair {
			void* first;
			void* second;

			rsc85_pair(void* p1, void* p2);
			inline rsc85_pair() {};
		};

		rage::datResourceInfo *resourceInfo;
		rage::grcArray<rsc85_object>* objects;
		std::map<void*, void*> pos;
		rage::grcArray<rsc85_pair> *oldObjsPos; // когда мы заменили поинтер на игровой нам нужно запомнить позицию этого поинтера чтобы потом вернуть его на место
		rage::grcArray<void*> *replacedPtrsObj; // only m_usageCount 
		rage::grcArray<void*> *addedToLayoutObj; // only m_usageCount 


		void process5(rsc85_object* pObjects, DWORD dwObjectCount, BYTE nbBlockType);
		void process85(rsc85_object* pObjects, DWORD dwObjectCount, BYTE nbBlockType);

		void create2(rsc85_object* pObjects, DWORD dwObjectCount, BYTE nbBlockType);

		void addObject(void*, size_t, DWORD, bool, BYTE);
		void* findPtr(void*);
		void addOldPtr(void*);
	public:

		
		DWORD bCreated : 1;
		DWORD dwMainObjectSize : 31;
		rsc85_pair mainObj;

		~rsc85_layout();
		rsc85_layout();

		void addReplacedPtrsObj(void*);
		void addAddedToLayoutObj(void*);
		bool isReplacedPtrsObj(void*);
		bool isAddedToLayoutObj(void*);

		template<class T> void setMainObject(T* pObj) {
			this->dwMainObjectSize = sizeof *pObj;
			this->mainObj.first = pObj;
		}

		template<class T> void addObject(T* pObj, BYTE nbBlockType, DWORD dwCount = 1) {
			if (!dwCount)
				error("[RSC85] Empty object at address 0x%p", pObj);

			//for (auto o : objects)
			//	if ((size_t)o.ptr == (size_t)pObj) { error("[RSC85] duplicate addresses when adding 0x%p", pObj); return; }

			//objects.push_back(rsc85_object((void*)pObj, sizeof(T), dwCount, 1, nbBlockType));
			addObject((void*)pObj, sizeof(T), dwCount, 1, nbBlockType);
		}
	
		//template<class T> void addObject(T* pObj, BYTE nbBlockType) { objects.push_back(rsc85_object((void*)pObj, sizeof(T), 1, 0, nbBlockType)); }

		void create(bool bIseOld = libertyFourXYZ::g_bForceUseRsc5);
		rage::datResourceInfo* getResourceInfo();
		//template<typename T> void setMainObj(T* pMainObj) {
		//	this->dwMainObjectSize = sizeof(*pMainObj);
		//	this->mainObj.first = pMainObj;
		//}
		


		template<class T> void setPtr(T*& pObj) {
			//if (pObj == NULL) {
			//	error("[RSC85] NULL ptr");
			//	pObj = NULL;
			//}
			//if (!pos.count((void*)pObj)) {
			//	error("[RSC85] position not found for 0x%p", pObj);
			//	pObj = NULL;
			//}
			//this->pair_tmp[this->oldObjsPos.size()].obj[0] = (void*)&pObj; this->pair_tmp[this->oldObjsPos.size()].obj[1] = (void*)pObj; // tmp!!
			
			//this->oldObjsPos.push_back(rsc85_pair((void*)&pObj, (void*)pObj));
			
			auto d = &pObj;

			addOldPtr((void*)&pObj);

			//pObj = (T*)pos[(void*)pObj];
			pObj = (T*)findPtr((void*)pObj);
		}


		template<class T> T* getPtr(T* pObj) {
			//if (pObj == NULL) {
			//	error("[RSC85] NULL ptr");
			//	return NULL;
			//}
			//if (!pos.count((void*)pObj)) {
			//	error("[RSC85] position not found for 0x%p", pObj);
			//	return NULL;
			//}
			//return (T*)pos[(void*)pObj];
			
			return (T*)findPtr(pObj);
		}

		// ToDo: check it
		template<class T> void backupPtr(T*& pObj) {
			//this->pair_tmp[this->oldObjsPos.size()].obj[0] = (void*)&pObj; this->pair_tmp[this->oldObjsPos.size()].obj[1] = (void*)pObj; // tmp!!
			
			//this->oldObjsPos.push_back(rsc85_pair((void*)&pObj, (void*)pObj));
			
			addOldPtr((void*)&pObj);
		}

		void setOldPtrs();

	};

};