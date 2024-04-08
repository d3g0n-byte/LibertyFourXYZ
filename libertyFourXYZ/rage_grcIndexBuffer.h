#pragma once
#include "rage_base.h"

namespace rage {
	class grcIndexBuffer : public datBase {
	public:
		DWORD m_dwIndexCount;
		rage::datRef<WORD> m_pIndexData;

		grcIndexBuffer();
		virtual ~grcIndexBuffer();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		//rage::grcIndexBuffer& operator=(rage::grcIndexBuffer& ib);
	};

	class grcIndexBufferD3D : public grcIndexBuffer {
	public:
		void* m_pD3DBuffer; // = this + sizeof rage::grcIndexBuffer + sizeof(size_t)
		size_t padding[8];

		grcIndexBufferD3D();
		~grcIndexBufferD3D();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		//rage::grcIndexBufferD11& operator=(rage::grcIndexBufferD11& ib);

	};


};