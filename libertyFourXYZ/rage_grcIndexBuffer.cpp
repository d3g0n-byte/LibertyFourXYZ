#include "rage_grcIndexBuffer.h"

#include "rage_datResource.h"
#include "rsc85_layout.h"

namespace rage {

	grcIndexBuffer::grcIndexBuffer() {
		this->m_dwIndexCount = 0;
		this->m_pIndexData.pElement = NULL;
	}


	grcIndexBuffer::~grcIndexBuffer() {
		this->m_dwIndexCount = 0;
		if (this->m_pIndexData.pElement)
			dealloc_arr(this->m_pIndexData.pElement);
	}

	void grcIndexBuffer::place(rage::datResource* rsc) {
		datBase::place(rsc);
		if (this->m_dwIndexCount) {
			WORD* idxPtr = rsc->getFixup<WORD>(this->m_pIndexData.pElement, this->m_dwIndexCount * 2);
			this->m_pIndexData.pElement = new("idxBuf, data")WORD [this->m_dwIndexCount];
			memcpy(this->m_pIndexData.pElement, idxPtr, this->m_dwIndexCount * sizeof WORD);
		}
	}

	void grcIndexBuffer::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		datBase::replacePtrs(pLayout, pRsc, dwDepth);
		if (this->m_dwIndexCount) {
			auto origData = this->m_pIndexData.pElement;
			pLayout->setPtr(this->m_pIndexData.pElement);
			DWORD dwSize = sizeof * this->m_pIndexData.pElement  * this->m_dwIndexCount;
			memcpy(pRsc->getFixup(this->m_pIndexData.pElement, dwSize), origData, dwSize);
		}

	}

	void grcIndexBuffer::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		datBase::addToLayout(pLayout, dwDepth);
		pLayout->addObject(this->m_pIndexData.pElement, 6,this->m_dwIndexCount);
	}

	grcIndexBufferD3D::grcIndexBufferD3D() { this->m_pD3DBuffer = NULL; }
	grcIndexBufferD3D::~grcIndexBufferD3D() { /*this->m_pD3DBuffer = NULL; */}

	void grcIndexBufferD3D::place(rage::datResource* rsc) {
		//this->~grcIndexBufferD11();
		//grcIndexBufferD11* realPtr = rsc->getFixup<rage::grcIndexBufferD11>(ptr);
		//memcpy((BYTE*)this + sizeof(size_t), (BYTE*)&realPtr + sizeof(size_t), sizeof(*realPtr) - sizeof(size_t) - sizeof(size_t) * 0x8);
		grcIndexBuffer::place(rsc);

		this->m_pD3DBuffer = NULL;
		//memset(this->padding, 0xcd, sizeof(size_t) * 0x8);
	}

	//rage::grcIndexBufferD11& grcIndexBufferD11::operator=(rage::grcIndexBufferD11& ib) {
	//	this->~grcIndexBufferD11();
	//	if (ib.m_pIndexData.pElement) {
	//		DWORD dwSize = sizeof WORD * ib.m_dwIndexCount;
	//		this->m_pIndexData.pElement = libertyFourXYZ::g_memory_manager.allocate<WORD>(dwSize);
	//		memcpy(this->m_pIndexData.pElement, ib.m_pIndexData.pElement, dwSize);
	//	}
	//	this->m_dwIndexCount = ib.m_dwIndexCount;
	//	this->m_pD3DBuffer = NULL;
	//	return *this;
	//}

	void grcIndexBufferD3D::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		grcIndexBuffer::replacePtrs(pLayout, pRsc, dwDepth);
	}

	void grcIndexBufferD3D::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		grcIndexBuffer::addToLayout(pLayout, dwDepth);

	}


}