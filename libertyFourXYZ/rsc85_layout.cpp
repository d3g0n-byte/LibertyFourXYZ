#include "rsc85_layout.h"

#include "utils.h"

#include "rage_array.h"
#include "settings.h"
#include "memory_manager.h"
namespace libertyFourXYZ {

	const static bool g_bUseNewMargePagesMethod = 1;
	const static bool g_bUseSmallPages = 1;

	rsc85_layout::rsc85_pair::rsc85_pair(void* p1, void* p2) {
		this->obj[0] = p1;
		this->obj[1] = p2;
	}


	DWORD rsc85_layout::rsc85_object::getSize() {
		DWORD dwRetVal = this->dwSize;
		if (this->bArray) dwRetVal *= this->dwArrayCount;
		return dwRetVal;
	}
	rsc85_layout::~rsc85_layout() {
		if (this->bCreated)
			libertyFourXYZ::g_memory_manager.release<rage::datResourceInfo>(this->resourceInfo);
	}

	rsc85_layout::rsc85_layout() : resourceInfo(NULL), bCreated(0) { }

	rsc85_layout::rsc85_object::rsc85_object(void* _ptr, DWORD _dwSize, DWORD _dwArrayCount, bool _bArray, DWORD _dwBlockType) {
		this->ptr = _ptr;
		this->dwSize = _dwSize;
		this->dwArrayCount = _dwArrayCount;
		this->bArray = _bArray;
		this->dwBlockType = _dwBlockType;
	}

	struct rsc5_info {
		DWORD pageCount : 7;
		DWORD page2 : 1;
		DWORD page4 : 1;
		DWORD page8 : 1;
		DWORD page16 : 1;
		DWORD pageSizeFactor : 4;
		DWORD __pad : 17;
		DWORD pageSize;

		rsc5_info() { memset(this, 0x0, sizeof(*this)); }
	};

	void rsc85_layout::process5(rsc85_object* pObjects, DWORD dwObjectCount, BYTE nbBlockType) {
		DWORD dwBasePageSize = 0x1000; // use 0x1580 for PS3s gfx!

		// main info of block
		rsc5_info info;

		if (dwObjectCount) {

			// minimal page size
			if (g_bMergeRscPages) {
				info.pageSizeFactor = g_nbMaxPageSizeForMerge;
				info.pageSize = dwBasePageSize << info.pageSizeFactor;
			}

			// looking for the largest object
			for (DWORD i = 0; i < dwObjectCount; i++)
				while (pObjects[i].getSize() > info.pageSize)
					info.pageSize = dwBasePageSize << ++info.pageSizeFactor;
		
			// perhaps the most important thing by which we learn information about this block
			std::vector<DWORD> aPageMappping(dwObjectCount != 0, info.pageSize);

			// start info
			DWORD* pdwPageSize = aPageMappping.data();
			DWORD dwPosInPage = 0;
			DWORD dwPagePos = 0;
			BYTE nbPageIndex = 0;
			DWORD pBase = nbBlockType << 28;
			info.pageCount = 1;

			// an array by which we find out which object has already been used
			std::vector<BYTE> aUsedObjects(dwObjectCount, 0);

			// start pos
			if (nbBlockType == 5) {
				this->mainObj.second = (void*)(dwPosInPage + pBase);
				dwPosInPage += alignValue(this->dwMainObjectSize, 0x10);
			}

			DWORD aSmallPagesSizes[]{ 0,0,0,0 };
			BYTE aUsedSmallPages[]{ info.page2, info.page4, info.page8, info.page16 };

			// start of first and second pass
			for (BYTE x = 0; x < 1 + g_bUseSmallPages; x++) { // first - big pages, second - small pages

				// the loop runs as long as there is at least one unused object
				while (!allTrue((bool*)aUsedObjects.data(), dwObjectCount)) {
					DWORD dwSelectedObjectSize = 0;
					__int64 qwSelectedObjectIndex = -1;

					// looking for the largest object
					for (DWORD i = 0; i < dwObjectCount; i++) {
						if (aUsedObjects[i]) continue;
						DWORD dwCurObjectSize = pObjects[i].getSize();
						if (dwPosInPage + dwCurObjectSize <= *pdwPageSize && dwCurObjectSize > dwSelectedObjectSize) {
							dwSelectedObjectSize = dwCurObjectSize;
							qwSelectedObjectIndex = i;
						}
					}
					// if we have found a suitable object
					if (qwSelectedObjectIndex != -1) {
						auto pObject = pObjects + qwSelectedObjectIndex; // get object entry
						pos.insert(std::make_pair(pObject->ptr, (void*)(dwPosInPage + pBase + dwPagePos))); // add object pos to std::map
						dwPosInPage += alignValue(dwSelectedObjectSize, 0x10); // and align the position on the page
						aUsedObjects[qwSelectedObjectIndex]++; // 

					}
					// if we have not found an object or the position in the page is equal to its size, go to the next page or create a new one
					else if (qwSelectedObjectIndex == -1 || dwPosInPage == *pdwPageSize) {
						dwPagePos += *pdwPageSize;
						nbPageIndex++;
						if (nbPageIndex >= aPageMappping.size()) {
							if (x == 1) { // if second pass
								trace("[RSC85] Can't create pages on the second pass. Fix this in rsc85_layout.cpp");
								return;
							}
							else {
								aPageMappping.push_back(info.pageSize);
								info.pageCount++;
							}
						}
						pdwPageSize = aPageMappping.data() + nbPageIndex;
						dwPosInPage = 0;
					}
				}

				if (x == 1) { // remove unused small pages on the second pass

					// remove unused small pages
					while (nbPageIndex != aPageMappping.size() - 1)
						for (char i = 3; i >=0 ; i--)
							if (aUsedSmallPages[i]) {
								aUsedSmallPages[i] = 0;
								aPageMappping.pop_back();
								break;
							}

					BYTE nbLastPageCount = aUsedSmallPages[0] + aUsedSmallPages[1] + aUsedSmallPages[2] + aUsedSmallPages[3];

					if (!aUsedSmallPages[3] && nbLastPageCount) { // if not used last small page and block have small pages
						DWORD dwLastPageSize = aPageMappping.back();
						BYTE usePage = 0;

						// check what small page size we need
						for (char i = 3; i >= 0; i--)
							if (dwPosInPage <= aSmallPagesSizes[i] /*&& !aUsedSmallPages[i]*/) { usePage = i; break; }

						// delete the last small page, no matter what size it is
						for (char i = 3; i >= 0; i--)
							if (aUsedSmallPages[i]) {
								aUsedSmallPages[i] = 0;
								aPageMappping.pop_back();
								break;
							}
						
						// adding the last small page
						aUsedSmallPages[usePage] = 1;
						aPageMappping.push_back(aSmallPagesSizes[usePage]);
					}
				}


				// if we have only one large page and if we use the minimum size for the page, then if necessary it needs to be reduced so that it does not waste more space
				if (libertyFourXYZ::g_bMergeRscPages && x == 0)
					if (aPageMappping.size() == 1) {
						while (info.pageSizeFactor != 0 && dwPosInPage <= info.pageSize / 2)
							info.pageSize = dwBasePageSize << --info.pageSizeFactor;
						aPageMappping[0] = info.pageSize;
					}

				// we have a page size, so we generate the size of small pages
				if (x == 0) {
					aSmallPagesSizes[0] = info.pageSize / 2, aSmallPagesSizes[1] = info.pageSize / 4,
						aSmallPagesSizes[2] = info.pageSize / 8, aSmallPagesSizes[3] = info.pageSize / 16;
				}

				// if this is the first pass and if we need to use small pages, we need to check if the last large page can be split
				if (x == 0 && g_bUseSmallPages /*&& info.pageCount > 1*/) {

					bool bLastPageCanBeSplit = 1;
					std::vector<std::pair<DWORD, DWORD>> posInPageAndSmallPageIndex;
					std::vector<DWORD> objIndex;

					DWORD dwLastPagePos = 0;
					for (char i = 0; i < aPageMappping.size() - 1; i++) dwLastPagePos += aPageMappping[i];
					DWORD adwUsedMemInSmallPages[]{ 0,0,0,0 };
					if (info.pageCount == 1 && nbBlockType == 5) // if this is the first page, we need to reserve space for the main structure
						adwUsedMemInSmallPages[0] = alignValue(dwMainObjectSize, 0x10);

					// checking each last page object to see if we can split the last page
					for (DWORD i = 0; i < dwObjectCount; i++) {

						// if the object position is duplicated, cancel the entire process
						if (this->pos.count(pObjects[i].ptr) != 1) {
							trace("[RSC85] The object is duplicate or was not found");
							return;
						}
						DWORD dwCurPtr = (DWORD)this->pos[pObjects[i].ptr];
						if (dwCurPtr >> 28 == nbBlockType) dwCurPtr &= 0x0fffffff;
						else continue;
						if (dwCurPtr < dwLastPagePos) continue;
						DWORD dwCurObjectSize = pObjects[i].getSize();
						bool bOk = 0;

						// check in which small page this object can be placed
						for (char j = 3; j >= 0; j--) {
							DWORD dwFreeInPage = aSmallPagesSizes[j] - adwUsedMemInSmallPages[j];

							if (dwCurObjectSize < dwFreeInPage) {
								objIndex.push_back(i);

								for (BYTE n = 0; n < 4; n++)
									if (n == j) aUsedSmallPages[j] = 1;

								adwUsedMemInSmallPages[j] += alignValue(dwCurObjectSize, 0x10);
								bOk = 1;
								break;
							}
						}

						// if not possible, we will return the pages to their standard position
						if (!bOk) { bLastPageCanBeSplit = 0; break; }
					}

					// if the page can be split, we need to remove objects from the last page to create a layout for them in the second pass.
					if (bLastPageCanBeSplit) {
						if (info.pageCount == 1 && nbBlockType == 5) // dont forget to reserve a place for the main structure
							dwPosInPage = alignValue(dwMainObjectSize, 0x10);
						else dwPosInPage = 0;

						info.pageCount--;

						dwPagePos = dwLastPagePos; // set the current position in the layout creation algorithm to the position of the first small page

						// fixing the array with pages
						aPageMappping.pop_back();
						nbPageIndex = aPageMappping.size();
						for (BYTE i = 0; i < 4; i++)
							if (aUsedSmallPages[i])
								aPageMappping.push_back(aSmallPagesSizes[i]);
						pdwPageSize = aPageMappping.data() + nbPageIndex;

						// clear the position of elements that are on the last page
						for (DWORD i = 0; i < objIndex.size(); i++) {
							this->pos.erase(pObjects[objIndex[i]].ptr);
							aUsedObjects[objIndex[i]] = 0;
						}
					}
					else break;
				}
				
				// we need at least one large page
				if (x == 1 && info.pageCount < 1) {
					bool bOk = 0;
					for (BYTE i = 0; i < 4; i++) {
						if (aUsedSmallPages[i] && info.pageSizeFactor > i ) {
							info.pageCount++;
							info.pageSizeFactor -= i + 1;
							info.pageSize = dwBasePageSize << info.pageSizeFactor;

							aUsedSmallPages[i] = 0;
							for (BYTE j = i + 1; j < 4; j++) {
								if (aUsedSmallPages[j]) {
									aUsedSmallPages[j - i - 1] = 1;
									aUsedSmallPages[j] = 0;
								}
								//aUsedSmallPages[j - i - 1];
							}
							bOk = 1;
							break;
						}
					}
					if (!bOk) {
						info.pageCount = 1;
						aUsedSmallPages[0] = 0;
						aUsedSmallPages[1] = 0;
						aUsedSmallPages[2] = 0;
						aUsedSmallPages[3] = 0;
					}
				}
			}
			info.page2 = aUsedSmallPages[0];
			info.page4 = aUsedSmallPages[1];
			info.page8 = aUsedSmallPages[2];
			info.page16 = aUsedSmallPages[3];
		}

		if (nbBlockType == 5) {
			this->resourceInfo->oldInfo.dwVPage0 = info.pageCount;
			this->resourceInfo->oldInfo.dwVPage1 = info.page2;
			this->resourceInfo->oldInfo.dwVPage2 = info.page4;
			this->resourceInfo->oldInfo.dwVPage3 = info.page8;
			this->resourceInfo->oldInfo.dwVPage4 = info.page16;
			this->resourceInfo->oldInfo.dwVSize = info.pageSizeFactor;
		}
		else if (nbBlockType == 6) {
			this->resourceInfo->oldInfo.dwPPage0 = info.pageCount;
			this->resourceInfo->oldInfo.dwPPage1 = info.page2;
			this->resourceInfo->oldInfo.dwPPage2 = info.page4;
			this->resourceInfo->oldInfo.dwPPage3 = info.page8;
			this->resourceInfo->oldInfo.dwPPage4 = info.page16;
			this->resourceInfo->oldInfo.dwPSize = info.pageSizeFactor;
		}

	}

	void rsc85_layout::process85(rsc85_object* pObjects, DWORD dwObjectCount, BYTE nbBlockType) {
		// настраиваем страницы
		BYTE nbPage2MaxCount;
		BYTE nbPage1MaxCount;
		BYTE nbPage0MaxCount;
		DWORD dwBasePageSize;
		DWORD dwPage2Size;
		DWORD dwPage1Size;
		DWORD dwPage0Size;
		if (nbBlockType == 6) {
			nbPage2MaxCount = 255;
			nbPage1MaxCount = 15;
			nbPage0MaxCount = 7;
			dwBasePageSize = 0x20000;
		}
		else if (nbBlockType == 5) {
			nbPage2MaxCount = 255;
			nbPage1MaxCount = 63;
			nbPage0MaxCount = 3;
			dwBasePageSize = 0x10000;
		}
		dwPage2Size = dwBasePageSize * 2;
		dwPage1Size = dwBasePageSize * 4;
		dwPage0Size = dwBasePageSize * 8;

		BYTE nbPage2Count = 0;
		BYTE nbPage1Count = 0;
		BYTE nbPage0Count = 0;
		WORD wBasePageCount = 0;

		WORD wPage65536 = 0;
		bool bPage32768 = 0;
		bool bPage16384 = 0;
		bool bPage8192 = 0;
		bool bPage4096 = 0;
		std::vector<BYTE> aUsedObjects(dwObjectCount, 0);

		for (DWORD i = 0; i < dwObjectCount; i++) {
			DWORD dwSize = pObjects[i].getSize();

			if (dwSize > dwPage1Size)
				nbPage0Count++;
			else if (dwSize > dwPage2Size)
				nbPage1Count++;
			else if (dwSize > dwBasePageSize)
				nbPage2Count++;
		}
	
		std::vector<DWORD> aPageMappping;

		// добавляем в начало маленькую страницу
		if (nbBlockType == 5) {
			aPageMappping.push_back(4096);
			bPage4096 = 1;
		}

		for (BYTE i = 0; i < nbPage0Count; i++)
			aPageMappping.push_back(dwPage0Size);
		for (BYTE i = 0; i < nbPage1Count; i++)
			aPageMappping.push_back(dwPage1Size);
		for (BYTE i = 0; i < nbPage2Count; i++)
			aPageMappping.push_back(dwPage2Size);


		char sbStartPage;

		if (nbBlockType == 5)
			sbStartPage = -1;


		if (!aPageMappping.size()) {
			aPageMappping.push_back(dwBasePageSize);
			wBasePageCount++;
		}

		bool bMainObjPlaced = 0;

		DWORD* pdwPageSize = aPageMappping.data();
		DWORD dwPosInPage = 0;

		if (nbBlockType == 5) {
			dwPosInPage += alignValue(this->dwMainObjectSize, 0x10);
			bMainObjPlaced = 1;
		}

		DWORD dwPagePos = 0;
		BYTE nbPageIndex = 0;

		DWORD pBase = nbBlockType << 28;

		while (!allTrue((bool*)aUsedObjects.data(), dwObjectCount)) {
			DWORD dwSelectedObjectSize = 0;
			__int64 qwSelectedObjectIndex = -1;
			for (DWORD i = 0; i < dwObjectCount; i++) {
				DWORD dwCurObjectSize = pObjects[i].getSize();
				if (dwPosInPage + dwCurObjectSize <= *pdwPageSize && dwCurObjectSize > dwSelectedObjectSize && !aUsedObjects[i]) {
					dwSelectedObjectSize = dwCurObjectSize;
					qwSelectedObjectIndex = i;
				}
			}

			if (qwSelectedObjectIndex != -1) {
				auto pObject = pObjects + qwSelectedObjectIndex;
				this->pos.insert(std::make_pair(pObject->ptr, (void*)(dwPosInPage + pBase + dwPagePos)));
				dwPosInPage += alignValue(dwSelectedObjectSize, 16);
				aUsedObjects[qwSelectedObjectIndex]++;

			}
			if (qwSelectedObjectIndex == -1 || dwPosInPage == *pdwPageSize) {
				dwPagePos += *pdwPageSize;
				nbPageIndex++;
				if (nbPageIndex >= aPageMappping.size()) {
					aPageMappping.push_back(dwBasePageSize);
					wBasePageCount++;
				}
				pdwPageSize = aPageMappping.data() + nbPageIndex;
				dwPosInPage = 0;
			}
		}

		// разделяем страницу на маленькие для оптимизации
		DWORD dwSmallPageSize16 = 4096;
		DWORD dwSmallPageSize8 = 8192;
		DWORD dwSmallPageSize4 = 16384;
		DWORD dwSmallPageSize2 = 32768;

		if (*pdwPageSize == dwBasePageSize && dwPosInPage <= dwSmallPageSize16 + dwSmallPageSize8 + dwSmallPageSize4 + dwSmallPageSize2) {
			DWORD dwBlockSize = 0;
			for (BYTE i = 0; i < aPageMappping.size(); i++) { dwBlockSize += aPageMappping[i]; }

			wBasePageCount--;

			///*
			std::vector<BYTE> usedPagesVariation;
			for (BYTE i = 1; i < 0x10; i++) {
				bool bOk = 1;

				bPage4096 = i & 1;
				bPage8192 = (i >> 1) & 1;
				bPage16384 = (i >> 2) & 1;
				bPage32768 = (i >> 3) & 1;

				if (bPage4096 && nbBlockType == 5) continue;

				DWORD dwCurSmallPagesSize = bPage4096 * dwSmallPageSize16 + bPage8192 * dwSmallPageSize8 + bPage16384 * dwSmallPageSize4 + bPage32768 * dwSmallPageSize2;
				if (dwPosInPage <= dwCurSmallPagesSize) {
					usedPagesVariation.push_back(i);

					BYTE nbUsedSmallPages = bPage4096 + bPage8192 + bPage16384 + bPage32768;
					if (nbUsedSmallPages > 1) { // если использовалось больше чем одна маленькая страница нужно перестроить
						DWORD dwLastPageSize = aPageMappping[nbPageIndex];
						DWORD dwUsedInLastPage = dwPosInPage;
						DWORD dwFreeMemInLastPage = dwCurSmallPagesSize - dwUsedInLastPage; // мы переместим объект, который находится находится на двух маленьких страниц в конец и для этого нам нужно знать сколько свободно

						std::vector<DWORD>smallPageSize;
						if (bPage32768)
							smallPageSize.push_back(dwSmallPageSize2);
						if (bPage16384)
							smallPageSize.push_back(dwSmallPageSize4);
						if (bPage8192)
							smallPageSize.push_back(dwSmallPageSize8);
						if (bPage4096)
							smallPageSize.push_back(dwSmallPageSize16);

						std::vector<DWORD>smallPagesPos(nbUsedSmallPages);
						DWORD dwTmp = dwBlockSize - dwBasePageSize;
						for (BYTE j = 0; j < nbUsedSmallPages; j++) {
							smallPagesPos[j] = dwTmp;
							dwTmp += smallPageSize[j];
						}

						// мы знаем сколько маленьких страниц и знаем где начинается каждая из них относительно старта
						// ищем объекты, которые стоят на двух страницах

						//std::vector<DWORD>fixedPagesEnd(0, nbUsedSmallPages - 1); 
						BYTE nbFixedSmallPages = 0;

						for (DWORD j = 0; j < nbUsedSmallPages; j++) {
							DWORD dwPageEnd = smallPagesPos[j] + smallPageSize[j];
							void* pPageStart = (BYTE*)(smallPagesPos[j] + pBase);
							void* pPageEnd = (BYTE*)(dwPageEnd + pBase);
							for (DWORD n = 0; n < dwObjectCount; n++) {
								DWORD dwObjectSize = pObjects[n].getSize();
								void* pCurrentObj = pObjects[n].ptr;

								auto pObjStart = this->pos[pCurrentObj];
								auto pObjEnd = (BYTE*)pObjStart + dwObjectSize;

								if ((size_t)pObjStart == (size_t)pPageEnd) { // эту страницу не надо фиксить ибо объект начинается с новой страницы
									nbFixedSmallPages++;
									break;
								}
								else if ((size_t)pObjStart > (size_t)pPageStart &&
									(size_t)pObjStart < (size_t)pPageEnd &&
									(size_t)pObjEnd >(size_t)pPageEnd) { // 
									DWORD dwNeedMem = (size_t)pPageEnd - (size_t)pObjStart; // узнаем сколько нам нужно свободной памяти
									if (dwNeedMem > dwFreeMemInLastPage) {
										bOk = 0;
										break;
									}

									// смещаем все поинтеры
									for (DWORD o = 0; o < dwObjectCount; o++) {
										size_t num1 = pBase + dwBlockSize;
										size_t num2 = (size_t)this->pos[pObjects[o].ptr];
										size_t num3 = (size_t)pObjStart;
										if (num2 >= num3 &&
											num2 < num1) {
											this->pos[pObjects[o].ptr] = (BYTE*)this->pos[pObjects[o].ptr] + dwNeedMem;
										}
									}
									nbFixedSmallPages++;
									break;
								}
							}
							if (!bOk)
								break;
						}
						if (bOk)
							break;
					}
					else {
						if (bPage4096)
							aPageMappping[nbPageIndex] = dwSmallPageSize16;
						else if (bPage8192)
							aPageMappping[nbPageIndex] = dwSmallPageSize8;
						else if (bPage16384)
							aPageMappping[nbPageIndex] = dwSmallPageSize4;
						else if (bPage32768)
							aPageMappping[nbPageIndex] = dwSmallPageSize2;
						else wBasePageCount++;
						break;
					}
				}
			}

		}

		//if (*pdwPageSize == dwBasePageSize) {
		//	wBasePageCount--;
		//	/*if (dwPosInPage <= 4096) { зарезервовано для начальной страницы
		//		bPage4096 = 1;
		//		aPageMappping[nbPageIndex] = 4096;
		//	}
		//	else */if (dwPosInPage <= 8192) {
		//		bPage8192 = 1;
		//		aPageMappping[nbPageIndex] = 8192;
		//	}
		//	else if (dwPosInPage <= 16384) {
		//		bPage16384 = 1;
		//		aPageMappping[nbPageIndex] = 16384;
		//	}
		//	else if (dwPosInPage <= 32768) {
		//		bPage32768 = 1;
		//		aPageMappping[nbPageIndex] = 32768;
		//	}
		//	else {
		//		wBasePageCount++;
		//	}
		//}


		// перемещаем первую страницу в конец
		if (aPageMappping.size() > 1 && aPageMappping[0] == 0x1000 && nbBlockType == 5) {
			bPage4096 = 1;

			aPageMappping.erase(aPageMappping.begin());

			DWORD dwNum1 = 0x1000; // сколько нужно отминусовать во всех кроме последних
			DWORD dwNum2 = 0; for (DWORD i : aPageMappping) dwNum2 += i; // сколько нужно прибавить поинтерам в последней странице

			aPageMappping.push_back(0x1000); // добавляем в конец

			DWORD dwNum1_1 = dwNum1 | pBase;
			DWORD dwNum2_1 = dwNum2 | pBase;

			// ToDo разобраться с заменой адреса
			for (auto i : this->pos) {
				if ((size_t)i.second >= pBase && (size_t)i.second < dwNum1_1)
					this->pos[i.first] = (BYTE*)i.second + dwNum2;
				else if ((size_t)i.second >= dwNum1_1 && (size_t)i.second < dwNum2_1 + dwNum1)
					this->pos[i.first] = (BYTE*)i.second - dwNum1;
			}

			sbStartPage = 0;


		}


		if (nbBlockType == 5) {
			this->resourceInfo->dwStartPage = sbStartPage;

			this->resourceInfo->dwVPage2 = nbPage2Count;
			this->resourceInfo->dwVPage1 = nbPage1Count;
			this->resourceInfo->dwVPage0 = nbPage0Count;

			wPage65536 = wBasePageCount + nbPage2Count * 2 + nbPage1Count * 4 + nbPage0Count * 8;
			this->resourceInfo->dwVPage65536 = wPage65536;
			this->resourceInfo->dwVPage32768 = bPage32768;
			this->resourceInfo->dwVPage16384 = bPage16384;
			this->resourceInfo->dwVPage8192 = bPage8192;
			this->resourceInfo->dwVPage4096 = bPage4096;

			this->mainObj.second = (void*)(pBase + this->resourceInfo->getVBlockStart());

		}
		else if (nbBlockType == 6) {
			this->resourceInfo->dwPPage2 = nbPage2Count;
			this->resourceInfo->dwPPage1 = nbPage1Count;
			this->resourceInfo->dwPPage0 = nbPage0Count;

			wPage65536 = wBasePageCount * 2 + nbPage2Count * 4 + nbPage1Count * 8 + nbPage0Count * 16;
			this->resourceInfo->dwPPage65536 = wPage65536;
			this->resourceInfo->dwPPage32768 = bPage32768;
			this->resourceInfo->dwPPage16384 = bPage16384;
			this->resourceInfo->dwPPage8192 = bPage8192;
			this->resourceInfo->dwPPage4096 = bPage4096;
		}
	}

	rage::datResourceInfo* rsc85_layout::getResourceInfo() {
		if (this->bCreated)
			return this->resourceInfo;
		else return NULL;
	}

	void rsc85_layout::create(bool bUseOld) {
		//trace("[")
		if (!this->bCreated)
			this->resourceInfo = libertyFourXYZ::g_memory_manager.allocate<rage::datResourceInfo>("create rsc85 layout");
		else
			*this->resourceInfo = rage::datResourceInfo();

		if (!bUseOld) {


			WORD wVBasePageMaxCount = 1023;
			BYTE nbVPage2MaxCount = 255;
			BYTE nbVPage1MaxCount = 63;
			BYTE nbVPage0MaxCount = 3;
			DWORD dwBaseVPageSize = 0x10000;

			WORD wPBasePageMaxCount = 1023;
			BYTE nbPPage2MaxCount = 255;
			BYTE nbPPage1MaxCount = 15;
			BYTE nbPPage0MaxCount = 7;
			DWORD dwBasePPageSize = 0x20000;


			DWORD dwVPage2Size = dwBaseVPageSize * 2;
			DWORD dwVPage1Size = dwBaseVPageSize * 4;
			DWORD dwVPage0Size = dwBaseVPageSize * 8;

			DWORD dwPPage2Size = dwBasePPageSize * 2;
			DWORD dwPPage1Size = dwBasePPageSize * 4;
			DWORD dwPPage0Size = dwBasePPageSize * 8;

			// проверка
			WORD wVBasePageCount = 0;
			BYTE nbVPage2Count = 0;
			BYTE nbVPage1Count = 0;
			BYTE nbVPage0Count = 0;

			WORD wPBasePageCount = 0;
			BYTE nbPPage2Count = 0;
			BYTE nbPPage1Count = 0;
			BYTE nbPPage0Count = 0;

			DWORD dwVBlockSize = 0;
			DWORD dwPBlockSize = 0;
			{

				for (size_t i = 0; i < this->objects.size(); i++) {
					DWORD dwObjectSize = this->objects[i].dwSize;
					if (this->objects[i].bArray)
						dwObjectSize *= this->objects[i].dwArrayCount;

					dwObjectSize = alignValue(dwObjectSize, 16);

					if (this->objects[i].dwBlockType == 5) {
						if (dwObjectSize > dwVPage0Size)
							bUseOld = 1;
						else if (dwObjectSize > dwVPage1Size)
							nbVPage0Count++;
						else if (dwObjectSize > dwVPage2Size)
							nbVPage1Count++;
						else if (dwObjectSize > dwBaseVPageSize)
							nbVPage2Count++;
						else
							wVBasePageCount++;

						if (wVBasePageCount > wVBasePageMaxCount ||
							nbVPage2Count > nbVPage2MaxCount ||
							nbVPage1Count > nbVPage2MaxCount ||
							nbVPage0Count > nbVPage2MaxCount)
							bUseOld = 1;
					}
					else if (this->objects[i].dwBlockType == 6) {
						if (dwObjectSize > dwPPage0Size)
							bUseOld = 1;
						else if (dwObjectSize > dwPPage1Size)
							nbVPage0Count++;
						else if (dwObjectSize > dwPPage2Size)
							nbPPage1Count++;
						else if (dwObjectSize > dwBasePPageSize)
							nbPPage2Count++;
						else
							wPBasePageCount += 2; // два ибо 

						if (wVBasePageCount > wVBasePageMaxCount ||
							nbVPage2Count > nbVPage2MaxCount ||
							nbVPage1Count > nbVPage2MaxCount ||
							nbVPage0Count > nbVPage2MaxCount)
							bUseOld = 1;
					}
					if (bUseOld)
						break;
				}

			}
		}


		std::vector<rsc85_object> aVObjects;
		std::vector<rsc85_object> aPObjects;

		for (DWORD i = 0; i < objects.size(); i++)
			if (objects[i].dwBlockType == 5)
				aVObjects.push_back(objects[i]);
			else if (objects[i].dwBlockType == 6)
				aPObjects.push_back(objects[i]);

		memset(this->resourceInfo, 0, sizeof *this->resourceInfo);
		if (bUseOld) {
			this->resourceInfo->oldInfo.bResource = 1;
			process5(aVObjects.data(), aVObjects.size(), 5);
			process5(aPObjects.data(), aPObjects.size(), 6);
		}
		else {
			this->resourceInfo->bRes = 1;
			this->resourceInfo->bUseExtendedSize = 1;
			process85(aVObjects.data(), aVObjects.size(), 5);
			process85(aPObjects.data(), aPObjects.size(), 6);
		}
		
		this->bCreated = 1;
		
	}
	
	void rsc85_layout::setOldPtrs() {
		for (DWORD i = 0; i < this->oldObjsPos.size(); i++) {
			//this->oldObjsPos[i].obj[0] = this->pair_tmp[i].obj[0];
			//this->oldObjsPos[i].obj[1] = this->pair_tmp[i].obj[1];

			void* pObj = this->oldObjsPos[i].obj[0];
			*(size_t**)pObj = (size_t*)this->oldObjsPos[i].obj[1];
		}
	}

}