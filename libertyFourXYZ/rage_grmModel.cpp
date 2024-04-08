#include "rage_grmModel.h"

#include "rage_grcFvf.h"
#include "rage_grcVertexBuffer.h"
#include "rage_grcIndexBuffer.h"
#include "rage_math.h"
#include "rage_grmGeometry.h"

//#include "fiTokenizer.h"
#include "errors.h"

namespace rage {
	grmModel::grmModel() {
		this->m_pBounds = NULL;
		this->m_pShaderMappings = NULL;
		this->m_nbBoneCount = 0;
		this->m_nbFlags = 0;
		this->m_nbType = 0;
		this->m_nbBoneIndex = 0;
		this->m_nbStride = 0;
		this->m_bSkinnned= 0;
		this->m_wShaderMappingCount = 0;
	}

	grmModel::~grmModel() {
		if(this->m_pBounds)
			dealloc_arr(this->m_pBounds);
		if(this->m_pShaderMappings)
			dealloc(this->m_pShaderMappings);
		this->m_nbBoneCount = 0;
		this->m_nbFlags = 0;
		this->m_nbType = 0;
		this->m_nbBoneIndex = 0;
		this->m_nbStride = 0;
		this->m_bSkinnned = 0;
		this->m_wShaderMappingCount = 0;
	}

	void grmModel::place(rage::datResource* rsc) {
		datBase::place(rsc);

		if(this->m_geometries.m_count)
			this->m_geometries.place(rsc);

		if (this->m_wShaderMappingCount) {
			WORD* realPtr = rsc->getFixup<WORD>(this->m_pShaderMappings, this->m_wShaderMappingCount * sizeof(WORD));
			this->m_pShaderMappings = new("mdl, place, shade")WORD [this->m_wShaderMappingCount];
			memcpy(this->m_pShaderMappings, realPtr, sizeof WORD * this->m_wShaderMappingCount);
		}
		if (this->m_pBounds) {
			DWORD dwCount = this->getBoundsCount();
			rage::Vector4* realPtr = rsc->getFixup(this->m_pBounds, sizeof(Vector4) * dwCount);
			this->m_pBounds = new("mdl, place, granitsa") Vector4 [dwCount];
			memcpy(this->m_pBounds, realPtr, sizeof rage::Vector4 * dwCount);
		}
	}

	DWORD grmModel::getBoundsCount() { return this->m_geometries.m_count > 1 ? this->m_geometries.m_count + 1 : this->m_geometries.m_count; }

	//grmModel& grmModel::operator=(rage::grmModel& model) {
	//	this->~grmModel();
	//	this->m_geometries = model.m_geometries;
	//	if (model.m_pavBounds) {
	//		DWORD dwCount = model.getBoundsCount();
	//		this->m_pavBounds = libertyFourXYZ::g_memory_manager.allocate<rage::Vector4>("mdl, op=, granitsa", dwCount);
	//		memcpy(this->m_pavBounds, model.m_pavBounds, sizeof rage::Vector4 * dwCount);
	//	}
	//	if (model.m_wShaderMappingCount) {
	//		this->m_pawShaderMappings = libertyFourXYZ::g_memory_manager.allocate<WORD>("mdl, place, shade", model.m_wShaderMappingCount);
	//		memcpy(this->m_pawShaderMappings, model.m_pawShaderMappings, sizeof WORD * model.m_wShaderMappingCount);
	//	}
	//	this->m_nbBoneCount = model.m_nbBoneCount;
	//	this->m_nbFlags = model.m_nbFlags;
	//	this->m_nbType = model.m_nbType;
	//	this->m_nbBoneIndex = model.m_nbBoneIndex;
	//	this->m_nbStride = model.m_nbStride;
	//	this->m_nbSkinFlag = model.m_nbSkinFlag;
	//	this->m_wShaderMappingCount = model.m_wShaderMappingCount;
	//	return *this;
	//}

	void grmModel::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		datBase::replacePtrs(pLayout, pRsc, dwDepth);

		if(this->m_geometries.m_count)
			this->m_geometries.replacePtrs(pLayout, pRsc, dwDepth);

		if (this->m_wShaderMappingCount) {
			auto origData = this->m_pShaderMappings;
			pLayout->setPtr(this->m_pShaderMappings);
			DWORD dwSize = sizeof * this->m_pShaderMappings * this->m_wShaderMappingCount;
			memcpy(pRsc->getFixup(this->m_pShaderMappings, dwSize), origData, dwSize);
		}
		if (this->m_pBounds) {
			auto origData = this->m_pBounds;
			pLayout->setPtr(this->m_pBounds);
			DWORD dwSize = sizeof * this->m_pBounds * this->getBoundsCount();
			memcpy(pRsc->getFixup(this->m_pBounds, dwSize), origData, dwSize);
		}


	}

	void grmModel::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		//error("[rage::grmModel::addToLayout] Not implemented. Please don't use it");
		datBase::addToLayout(pLayout, dwDepth);
		
		this->m_geometries.addToLayout(pLayout, dwDepth);

		if (this->m_wShaderMappingCount)
			pLayout->addObject(this->m_pShaderMappings, 5, this->m_wShaderMappingCount);
		if(this->m_pBounds)
			pLayout->addObject(this->m_pBounds, 5, this->getBoundsCount());


	}

	struct openFormats_mesh_v13 {
		float m_pos[3];
		float m_nrm[3];
		BYTE m_color[4];
		float m_tangent[4];
		float m_texCoord[5][2];
	};

	struct openFormats_skinned_mesh_v13 {
		float m_pos[3];
		float m_nrm[3];
		float m_weight[4];
		BYTE m_binding[4];
		BYTE m_color[4];
		float m_tangent[4];
		float m_texCoord[2][2];
	};

	HRESULT grmModel::fromLibertyFormats(const char* pszPath, BYTE nbMatrixIndex) {


		return 0;
	}

	int grmModel::fromOpenFormats(const char* pszPath, BYTE nbMatrixIndex) {


		return 0;
	}


}