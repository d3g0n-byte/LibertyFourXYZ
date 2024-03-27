#pragma once
#include "rage_base.h"
//#include "rage_math.h"
//#include "rage_grmGeometry.h"

namespace rage { class grmGeometryQB; class Vector4; };

namespace rage {
	class grmModel : public rage::datBase {
	public:
		rage::grcArray<rage::datOwner<rage::grmGeometryQB>> m_geometries;
		rage::Vector4* m_pavBounds;
		WORD* m_pawShaderMappings;
		BYTE m_nbBoneCount; // aka matrix
		BYTE m_nbFlags; // 0x1 - skinned
		BYTE m_nbType; // unused
		BYTE m_nbBoneIndex;
		BYTE m_nbStride;
		BYTE m_nbSkinFlag; // in V and tessellation
		WORD m_wShaderMappingCount;

		grmModel();
		~grmModel();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		//rage::grmModel& operator=(rage::grmModel& model);
		DWORD getBoundsCount();

		HRESULT fromLibertyFormats(const char* pszPath, BYTE nbMatrixIndex);
		int fromOpenFormats(const char* pszPath, BYTE nbMatrixIndex);

		//void addGeometry(rage::grmGeometryQB& geom);

	};

};