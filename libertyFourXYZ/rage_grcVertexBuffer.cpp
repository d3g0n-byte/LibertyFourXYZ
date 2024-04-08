#include "rage_grcVertexBuffer.h"

#include "rage_datResource.h"
#include "rsc85_layout.h"
#include "rage_grcFvf.h"

namespace rage {
	grcVertexBuffer::grcVertexBuffer() {
		this->m_bLocked = 0;
		this->m_dwVertexSize = 0;
		*((BYTE*)this + 7) = 0; // flags
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
			dealloc_arr(this->m_pVertexData.pElement);
		this->m_threadId = 0;
		this->m_wVertexCount = 0;
	}

	void grcVertexBuffer::place(rage::datResource* rsc) {

		datBase::place(rsc);
		if (this->m_pVertexData.pElement) {
			DWORD dwSize = this->m_wVertexCount * this->m_dwVertexSize;
			BYTE* vertsPtr = rsc->getFixup<BYTE>(this->m_pVertexData.pElement, dwSize);
			this->m_pVertexData.pElement = new("vertBuf, place, data")BYTE [dwSize];
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

		this->m_pDeclarations.addToLayout(pLayout, dwDepth);

		pLayout->addObject(this->m_pVertexData.pElement, 6, this->m_wVertexCount * this->m_dwVertexSize);
	}


	grcVertexBufferD3D::grcVertexBufferD3D() : m_pD3DBuffer(NULL){
	}

	grcVertexBufferD3D::~grcVertexBufferD3D() {
	}

	void grcVertexBufferD3D::place(rage::datResource* rsc) {
		grcVertexBuffer::place(rsc);
		this->m_pD3DBuffer = NULL;
	}

	void grcVertexBufferD3D::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		grcVertexBuffer::replacePtrs(pLayout, pRsc, dwDepth);
	}

	void grcVertexBufferD3D::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		grcVertexBuffer::addToLayout(pLayout, dwDepth);
	}

}