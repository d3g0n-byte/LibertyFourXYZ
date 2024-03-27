#pragma once
#include "rage_base.h"
#include "rage_rmcLod.h"

namespace rage { class crSkeletonData; class grmShaderGroup; }
class CLightAttr;

namespace rage {
	class rmcDrawableBase : public rage::pgBase {
	public:
		grmShaderGroup* m_pShaderGroup;

		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

		rmcDrawableBase();
		~rmcDrawableBase();
		void destroy();
	};

	class rmcDrawable : public rmcDrawableBase {
	public:
		crSkeletonData* m_pSkeleton;
		rmcLodGroup m_lodgroup;

		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

		rmcDrawable();
		~rmcDrawable();
		void destroy();
	};
}

class gtaDrawable : public rage::rmcDrawable {
public:
	
	rage::atArray<CLightAttr> m_light;

	void place(rage::datResource* rsc);
	void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
	void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
	void clearRefCount();
	void setRefCount();

	gtaDrawable();
	~gtaDrawable();
	void destroy();
};

