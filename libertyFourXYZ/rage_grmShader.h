#pragma once
#include "rage_base.h"


namespace rage {
	class grcTexturePC;
	class grcTextureReference;
	class Vector4;
	class Matrix44;
	class datResource;
	class grcEffect;/* {
	public:
		BYTE __0[0x58];
	};*/

	class grcInstanceData {
	public:
		struct Entry {
			union {
				// 0
				grcTexturePC* pTxd; // ->nbResType == 0
				grcTextureReference* pTxdRef; // ->nbResType == 2
				Vector4* pVec4;		// 1, 8(type=6), 14,15,16
				Matrix44* pMtx;		// 4
			};
		};


		Entry* pEntries;
		grcEffect* pBasis;
		DWORD dwCount;
		DWORD dwTotalSize;
		BYTE* pTypes;
		DWORD dwBasisHashCode; // fxc hash
		void* _f18; // ptr?
		DWORD _f1c;
		DWORD* pEntriesHashes;
		DWORD _f24;
		DWORD _f28;
		void* _f2c; // ptr?

		grcInstanceData();
		~grcInstanceData();

		void place(datResource* pRsc, pgDictionary<grcTexturePC>* pTxd = NULL);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

		DWORD recomputeTotalSize();

	};

	class grmShader : public pgBase {
	public:
		BYTE m_nbVersion;
		BYTE m_nbDrawBucket;
		union {
			struct {
				BYTE bHavePreset : 1; // have material?(sps)
				BYTE unk_flags : 7;
			};
			BYTE m_nbFlags;
		};
		BYTE _fb;
		WORD _fc;
		WORD m_wIndex;
		DWORD _f10;
		grcInstanceData m_instanceData;

		grmShader();
		~grmShader();

		void place(datResource* pRsc, pgDictionary<grcTexturePC>* pTxd = NULL);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

	};

	class grmShaderFx : public grmShader {
	public:
		rage::ConstString m_pszName; // +0x44
		rage::ConstString m_pszPresetName; // +0x48
		rage::ConstString _f4c; // can be used in our resources
		rage::ConstString _f50; // can be used in our resources
		DWORD m_dwPresetIndex; // only if have m_pszMaterialName/m_pszPresetName
		void* _f58; // -> structure without wmt. see in eflc_pc_1120.idb func rage::grmShaderFactoryStandard::createShader

		grmShaderFx();
		~grmShaderFx();

		void place(datResource* pRsc, pgDictionary<grcTexturePC>* pTxd = NULL);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();
	};

	class grmShaderGroup : public datBase {
	public:
		pgDictionary<grcTexturePC> *m_pTexture; // int - tmp
		//datOwner<pgDictionary<grcTexturePC>> m_pTexture; // int - tmp
		atArray<datOwner<grmShaderFx>> m_shaders;
		BYTE __10[0x30];
		grcArray<DWORD> m_vertexFormat;
		grcArray<DWORD> m_indexMapping;

		grmShaderGroup();
		~grmShaderGroup();

		void place(datResource* pRsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

	};

}