#include "rage_grcVertexBuffer.h"

#include "rage_datResource.h"
#include "rsc85_layout.h"
#include "rage_grcFvf.h"

namespace rage {
	grcVertexBuffer::grcVertexBuffer() {
		this->m_bLocked = 0;
		this->m_dwVertexSize = 0;
		*((BYTE*)this + 7) = 0; // flags
		//this->m_pDeclarations = libertyFourXYZ::g_memory_manager.allocate<rage::grcFvf>(); // ?
		this->m_pDeclarations = NULL;
		this->m_pLockedData = NULL; // неt смысла использовать
		this->m_pVertexData.pElement = NULL;
		this->m_threadId = 0;
		this->m_wVertexCount = 0;

	}

	grcVertexBuffer::~grcVertexBuffer() {
		this->m_bLocked = 0;
		this->m_dwVertexSize = 0;
		*((BYTE*)this + 7) = 0; // flags

		this->m_pLockedData = NULL;
		if (this->m_pVertexData.pElement)
			libertyFourXYZ::g_memory_manager.release<BYTE>(this->m_pVertexData.pElement);
		this->m_threadId = 0;
		this->m_wVertexCount = 0;
	}

	void grcVertexBuffer::place(rage::datResource* rsc) {
		//error("[rage::grcVertexBuffer::place] Not implemented. Please don't use it");

		datBase::place(rsc);
		if (this->m_pVertexData.pElement) {
			DWORD dwSize = this->m_wVertexCount * this->m_dwVertexSize;
			BYTE* vertsPtr = rsc->getFixup<BYTE>(this->m_pVertexData.pElement, dwSize);
			this->m_pVertexData.pElement = libertyFourXYZ::g_memory_manager.allocate<BYTE>("vertBuf, place, data", dwSize);
			memcpy(this->m_pVertexData.pElement, vertsPtr, dwSize);
			this->m_pLockedData = NULL;
		}
		this->m_pDeclarations.place(rsc);
	}

	void grcVertexBuffer::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		datBase::replacePtrs(pLayout, pRsc, dwDepth);
		if (this->m_wVertexCount) {
			auto origData = this->m_pVertexData.pElement;
			pLayout->setPtr(this->m_pVertexData.pElement);
			DWORD dwSize = this->m_dwVertexSize * this->m_wVertexCount;
			memcpy(pRsc->getFixup(this->m_pVertexData.pElement, dwSize), origData, dwSize);
			this->m_pLockedData = this->m_pVertexData.pElement;
		}
		this->m_pDeclarations.replacePtrs(pLayout, pRsc, dwDepth);

	}

	void grcVertexBuffer::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {

		datBase::addToLayout(pLayout, dwDepth);

		//pLayout->addObject(this->m_pDeclarations.pElement, 5);
		this->m_pDeclarations.addToLayout(pLayout, dwDepth);

		pLayout->addObject(this->m_pVertexData.pElement, 6, this->m_wVertexCount * this->m_dwVertexSize);
	}


	grcVertexBufferD11::grcVertexBufferD11() : m_pD3DBuffer(NULL){
		//this->m_pDeclarations = libertyFourXYZ::g_memory_manager.allocate<rage::grcFvf>();
	}

	grcVertexBufferD11::~grcVertexBufferD11() {
		//this->m_pD3DBuffer = NULL;
	}

	void grcVertexBufferD11::place(rage::datResource* rsc) {
		grcVertexBuffer::place(rsc);
		this->m_pD3DBuffer = NULL;
		//this->~grcVertexBufferD11();

		//grcVertexBufferD11* realPtr = rsc->getFixup<rage::grcVertexBufferD11>(ptr);
		//memcpy((BYTE*)this + sizeof(size_t), (BYTE*)&realPtr + sizeof(size_t), sizeof(*realPtr) - sizeof(size_t) - sizeof(size_t) * 0x8);
	}

	void grcVertexBufferD11::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		grcVertexBuffer::replacePtrs(pLayout, pRsc, dwDepth);
	}

	void grcVertexBufferD11::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		grcVertexBuffer::addToLayout(pLayout, dwDepth);
	}

	//rage::grcVertexBufferD11& grcVertexBufferD11::operator=(rage::grcVertexBufferD11& vb) {
	//	this->~grcVertexBufferD11();
	//	if (vb.m_pVertexData.pElement) {
	//		DWORD dwSize = this->m_wVertexCount * this->m_dwVertexSize;
	//		this->m_pVertexData.pElement = libertyFourXYZ::g_memory_manager.allocate<BYTE>(dwSize);
	//		memcpy(this->m_pVertexData.pElement, vb.m_pVertexData.pElement, dwSize);
	//	}
	//	this->m_wVertexCount = vb.m_wVertexCount;
	//	this->m_bLocked = vb.m_bLocked;
	//	*(BYTE*)&this->m_nbFlags = *(BYTE*)&vb.m_nbFlags;
	//	this->m_pLockedData = vb.m_pLockedData;
	//	this->m_dwVertexSize = vb.m_dwVertexSize;
	//	this->m_threadId = vb.m_threadId;
	//	this->m_pDeclarations = vb.m_pDeclarations;
	//	this->m_pD3DBuffer = NULL;
	//	return *this;
	//}


}