#pragma once
#include "rage_base.h"
#include "rage_array.h"

namespace rage { class grcVertexBufferD3D; class grcIndexBufferD3D; }

namespace rage {

	class grmGeometry : public rage::datBase {
	public:
		enum eGeomertyType {
			GEOMETRYQB = 0,
			GEOMETRYEDGE = 1
		};

		void* m_piVertexDeclaration; // поинтер к rage::grcVertexDeclaration. Размер 0x10. ЭТО НЕ rage::grcFvf!
		int m_nType;

		grmGeometry();
		~grmGeometry();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		//rage::grmGeometry& operator=(rage::grmGeometry& geom);

	};


	class grmGeometryQB : public rage::grmGeometry {
	public:
		rage::atRangeArray<rage::datOwner<rage::grcVertexBufferD3D>, 4> m_vertexBuffers;
		rage::atRangeArray<rage::datOwner<rage::grcIndexBufferD3D>, 4> m_indexBuffers;
		DWORD m_dwIndexCount;
		DWORD m_dwFaceCount;
		WORD m_wVertexCount;
		WORD m_wIndicesPerFace;
		//BYTE m_nbIndicesPerFace;
		//bool m_bDoubleBuffered;
		WORD* m_pBoneMapping; // вертекс указывает индекс к реальных костям, но делает через этот массив. Не для фрагментных файлах
		WORD m_wStride;
		WORD m_wBoneCount;
		void* _InstanceVertexDeclarationD3D;
		void* _InstanceVertexBufferD3D;
		DWORD _UseGlobalStreamIndex;


		grmGeometryQB();
		~grmGeometryQB();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		//rage::grmGeometryQB& operator=(rage::grmGeometryQB& geom);

	};
}

