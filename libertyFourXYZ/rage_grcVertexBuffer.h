#pragma once

#include "rage_base.h"
namespace rage { class grcFvf; };

namespace rage {
	class grcVertexBuffer : rage::datBase {
	public:
		WORD m_wVertexCount;
		BYTE m_bLocked;
		union {
			struct {
				BYTE bDynamic : 1;
				BYTE bPreallocatedMemory : 1;
				BYTE bReadWrite : 1;
				BYTE _f3 : 5;
			};
			BYTE m_dw;
		} m_flags;
		void* m_pLockedData;
		DWORD m_dwVertexSize;
		rage::datOwner<rage::grcFvf> m_pDeclarations;
		size_t m_threadId;
		rage::datRef<BYTE> m_pVertexData;

		grcVertexBuffer();
		~grcVertexBuffer();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		//rage::grcVertexBuffer& operator=(rage::grcVertexBuffer& vb);
	};

	class grcVertexBufferD11 : public rage::grcVertexBuffer {
	public:
		void* m_pD3DBuffer; // = this + sizeof rage::grcIndexBuffer + sizeof(size_t)
		DWORD reversed[8];

		grcVertexBufferD11();
		~grcVertexBufferD11();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		//rage::grcVertexBufferD11& operator=(rage::grcVertexBufferD11& vb);

	};



};