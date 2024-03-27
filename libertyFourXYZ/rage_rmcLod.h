#pragma once

#include "rage_math.h"
#include "rage_array.h"
#include "rage_base.h"

namespace rage { class grmModel; }

namespace rage {
	class rmcLod {
	public:
		rage::grcArray<rage::datOwner<rage::grmModel>> m_models;

		rmcLod();
		~rmcLod();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

	};

	class rmcLodGroup {
	public:
		rage::spdSphere m_vCenter;
		rage::spdAABB m_boundingBox;
		rage::atRangeArray<rage::datOwner<rage::rmcLod>, 4> m_lod;
		rage::atRangeArray<float, 4> m_fLodThresh;
		rage::atRangeArray<DWORD, 4> m_dwBucketMask;
		float m_fRadius;
		BYTE __64[0xc];

		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

		rmcLodGroup();
		~rmcLodGroup();
	};
}