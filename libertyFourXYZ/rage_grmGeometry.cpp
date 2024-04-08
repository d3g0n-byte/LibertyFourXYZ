#include "rage_grmGeometry.h"

#include "rage_grcFvf.h"
#include "rage_grcVertexBuffer.h"
#include "rage_grcIndexBuffer.h"

namespace rage {

	grmGeometry::~grmGeometry() {
		this->m_piVertexDeclaration = NULL;
		this->m_nType = GEOMETRYQB;
	}

	grmGeometry::grmGeometry() {
		this->m_piVertexDeclaration = NULL;
		this->m_nType = GEOMETRYQB;
	}

	void grmGeometry::place(rage::datResource* rsc) {
		datBase::place(rsc);
		//error("[rage::grmGeometry::place] Not implemented. Please don't use it");
		this->m_piVertexDeclaration = NULL;
	}

	void grmGeometry::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		datBase::replacePtrs(pLayout, pRsc, dwDepth);
	}

	void grmGeometry::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		datBase::addToLayout(pLayout, dwDepth);
	}

	grmGeometryQB::~grmGeometryQB() {
		for (BYTE i = 0; i < 4; i++)
			if(this->m_vertexBuffers[i].pElement)
				dealloc(this->m_vertexBuffers[i].pElement);
		for (BYTE i = 0; i < 4; i++)
			if(this->m_indexBuffers[i].pElement)
				dealloc(this->m_indexBuffers[i].pElement);

		this->m_dwIndexCount = 0;
		this->m_dwFaceCount = 0;
		this->m_wVertexCount = 0;
		this->m_wIndicesPerFace = 0;
		//this->m_bDoubleBuffered = 0;
		if (this->m_wBoneCount)
			dealloc_arr(this->m_pBoneMapping);
		this->m_wStride = 0;
		this->m_wBoneCount = 0;
		//*(size_t*)this->__40 = 0xcdcdcdcd;
		//for (BYTE i = 1; i < 3; i++)
		//	*((size_t*)this->__40 + i) = NULL;
	}

	grmGeometryQB::grmGeometryQB() {
		this->m_piVertexDeclaration = NULL;
		this->m_nType = GEOMETRYQB;
		//for (BYTE i = 0; i < 8; i++)
		//	*((size_t*)m_vertexBuffers.getElements() + i) = NULL;

		this->m_dwIndexCount = 0;
		this->m_dwFaceCount = 0;
		this->m_wVertexCount = 0;
		this->m_wIndicesPerFace = 0;
		//this->m_bDoubleBuffered = 0;
		this->m_pBoneMapping = NULL;
		this->m_wStride = 0;
		this->m_wBoneCount = 0;
		//*(size_t*)this->__40 = 0xcdcdcdcd;
		//for (BYTE i = 1; i < 3; i++)
		//	*((size_t*)this->__40 + i) = NULL;
	}

	void grmGeometryQB::place(rage::datResource* rsc) {
		//this->~grmGeometryQB();
		grmGeometry::place(rsc);

		////grmGeometryQB* realPtr = rsc->getFixup<rage::grmGeometryQB>(ptr);
		//memcpy((BYTE*)this + sizeof(size_t), (BYTE*)&realPtr + sizeof(size_t), sizeof(*realPtr) - sizeof(size_t)); // пропускаем vmt

		for (BYTE i = 0; i < 4; i++) {
			if (this->m_vertexBuffers[i].pElement)
				this->m_vertexBuffers[i].place(rsc);
		}
		for (BYTE i = 0; i < 4; i++) {
			if (this->m_indexBuffers[i].pElement)
				this->m_indexBuffers[i].place(rsc);
		}
		
		if (this->m_wBoneCount) {
			WORD* pBones = rsc->getFixup<WORD>(this->m_pBoneMapping, this->m_wBoneCount * sizeof(WORD));
			this->m_pBoneMapping = new("geom, place, mtx") WORD [this->m_wBoneCount];
			memcpy(this->m_pBoneMapping, pBones, sizeof * this->m_pBoneMapping * this->m_wBoneCount);
		}
	}

	void grmGeometryQB::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		grmGeometry::replacePtrs(pLayout, pRsc, dwDepth);

		for (BYTE i = 0; i < 4; i++)
			if (this->m_vertexBuffers[i].pElement)
				this->m_vertexBuffers[i].replacePtrs(pLayout, pRsc, dwDepth);
		for (BYTE i = 0; i < 4; i++)
			if (this->m_indexBuffers[i].pElement)
				this->m_indexBuffers[i].replacePtrs(pLayout, pRsc, dwDepth);

		if (this->m_wBoneCount) {
			auto origData = this->m_pBoneMapping;
			pLayout->setPtr(this->m_pBoneMapping);
			DWORD dwSize = sizeof *this->m_pBoneMapping * this->m_wBoneCount;
			memcpy(pRsc->getFixup(this->m_pBoneMapping, dwSize), origData, dwSize);
		}


	}

	void grmGeometryQB::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		grmGeometry::addToLayout(pLayout, dwDepth);

		for (BYTE i = 0; i < 4; i++)
			if (this->m_vertexBuffers[i].pElement)
				this->m_vertexBuffers[i].addToLayout(pLayout, dwDepth);
		for (BYTE i = 0; i < 4; i++)
			if (this->m_indexBuffers[i].pElement)
				this->m_indexBuffers[i].addToLayout(pLayout, dwDepth);

		if (this->m_wBoneCount)
			pLayout->addObject(this->m_pBoneMapping, 5, this->m_wBoneCount);
	}


}