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
				libertyFourXYZ::g_memory_manager.release<rage::grcVertexBufferD11>(this->m_vertexBuffers[i].pElement);
		for (BYTE i = 0; i < 4; i++)
			if(this->m_indexBuffers[i].pElement)
				libertyFourXYZ::g_memory_manager.release<rage::grcIndexBufferD11>(this->m_indexBuffers[i].pElement);

		this->m_dwIndexCount = 0;
		this->m_dwFaceCount = 0;
		this->m_wVertexCount = 0;
		this->m_nbIndicesPerFace = 0;
		this->m_bDoubleBuffered = 0;
		if (this->m_wBoneCount)
			libertyFourXYZ::g_memory_manager.release(this->m_pawBoneMapping);
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
		this->m_nbIndicesPerFace = 0;
		this->m_bDoubleBuffered = 0;
		this->m_pawBoneMapping = NULL;
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
			WORD* pBones = rsc->getFixup<WORD>(this->m_pawBoneMapping, this->m_wBoneCount * sizeof(WORD));
			this->m_pawBoneMapping = libertyFourXYZ::g_memory_manager.allocate<WORD>("geom, place, mtx", this->m_wBoneCount);
			memcpy(this->m_pawBoneMapping, pBones, sizeof * this->m_pawBoneMapping * this->m_wBoneCount);
		}
	}

	//grmGeometryQB& grmGeometryQB::operator=(grmGeometryQB& geom) {
	//	this->~grmGeometryQB();
	//	
	//	this->m_piVertexDeclaration = NULL;
	//	this->m_nType = geom.m_nType;
	//	for (BYTE i = 0; i < 4; i++)
	//		if (geom.m_vertexBuffers[i])
	//			this->m_vertexBuffers[i] = geom.m_vertexBuffers[i];
	//	for (BYTE i = 0; i < 4; i++)
	//		if (geom.m_indexBuffers[i])
	//			this->m_indexBuffers[i] = geom.m_indexBuffers[i];
	//	
	//	this->m_dwIndexCount = geom.m_dwIndexCount;
	//	this->m_dwFaceCount = geom.m_dwFaceCount;
	//	this->m_wVertexCount = geom.m_wVertexCount;
	//	this->m_nbIndicesPerFace = geom.m_nbIndicesPerFace;
	//	this->m_bDoubleBuffered = geom.m_bDoubleBuffered;
	//	this->m_wStride = geom.m_wStride;
	//	this->m_wBoneCount = geom.m_wBoneCount;
	//	if (this->m_wBoneCount) {
	//		this->m_pawBoneMapping = libertyFourXYZ::g_memory_manager.allocate<WORD>(this->m_wBoneCount);
	//		memcpy(this->m_pawBoneMapping, geom.m_pawBoneMapping, sizeof WORD * this->m_wBoneCount);
	//	}
	//	*(DWORD*)this->__40 = *(DWORD*)geom.__40;
	//	*((DWORD*)this->__40 + 1) = *((DWORD*)geom.__40 + 1);
	//	*((DWORD*)this->__40 + 2) = *((DWORD*)geom.__40 + 2);
	//	return *this;
	//}

	void grmGeometryQB::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		grmGeometry::replacePtrs(pLayout, pRsc, dwDepth);

		for (BYTE i = 0; i < 4; i++)
			if (this->m_vertexBuffers[i].pElement)
				this->m_vertexBuffers[i].replacePtrs(pLayout, pRsc, dwDepth);
		for (BYTE i = 0; i < 4; i++)
			if (this->m_indexBuffers[i].pElement)
				this->m_indexBuffers[i].replacePtrs(pLayout, pRsc, dwDepth);

		if (this->m_wBoneCount) {
			auto origData = this->m_pawBoneMapping;
			pLayout->setPtr(this->m_pawBoneMapping);
			DWORD dwSize = sizeof *this->m_pawBoneMapping * this->m_wBoneCount;
			memcpy(pRsc->getFixup(this->m_pawBoneMapping, dwSize), origData, dwSize);
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
			pLayout->addObject(this->m_pawBoneMapping, 5, this->m_wBoneCount);
	}


}