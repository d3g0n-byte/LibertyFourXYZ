#pragma once

#include "rage_datResource.h"
#include "rage_base.h"
#include "rage_string.h"

namespace libertyFourXYZ { class rsc85_layout; };

class dds;
class IDirect3DDevice9;

namespace rage {
	class grcTexturePC;

	enum eTextureResourceType {
		TEXTURE,						// Standard texture
		TEXTURE_RENDERTARGET,			// Render target
		TEXTURE_REFERENCE,				// Reference to an existing texture
		TEXTURE_DICTIONARY_REFERENCE,	// Reference to an existing texture in a dictionary
	};

	class grcTextureReferenceBase : public pgBase {
	public:
		BYTE m_nbResourceType;
		BYTE _f9;
		WORD m_wUsageCount;
		DWORD _fc;

		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

		//rage::grcTextureReferenceBase& operator=(rage::grcTextureReferenceBase& txd);
	};

	class grcTextureReference : public grcTextureReferenceBase {
	public:
		DWORD _f10;
		rage::ConstString m_pszName; // name or 'null'. 'null' is empty texture. name without pack:/, memory:$... etc
		rage::grcTexturePC* m_pTexture; // NULL in resource.

		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

		//rage::grcTextureReference& operator=(rage::grcTextureReference& txd);
	};

	class grcTexture : public pgBase {
	public:
		BYTE m_nbResourceType; // 0 - rage::grcTexture{platform}, 2 - rage::grcTextureReference, 1 - skip?
		BYTE m_nbDepth; 
		WORD m_wUsageCount; // 0x4000?
		DWORD _fc; // dwSceneNumber?
		DWORD _f10;

		grcTexture();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

		//rage::grcTexture& operator=(rage::grcTexture& txd);
	};

	class grcTexturePC : public grcTexture {
	public:
		rage::ConstString m_pszName;
		IDirect3DDevice9* m_piTexture;
		WORD m_wWidth;
		WORD m_wHeight;
		DWORD m_dwPixelFormat;
		WORD m_wMipStride;
		BYTE m_nbTextureType;
		BYTE m_nbLevels;
		float m_fColorExpR;
		float m_fColorExpG;
		float m_fColorExpB;
		float m_fColorOfsR;
		float m_fColorOfsG;
		float m_fColorOfsB;
		grcTexturePC* m_pPrev;
		grcTexturePC* m_pNext;
		BYTE* m_pPixelData;
		BYTE __4c[4]; // BYTE _f4c is copy of mip count?

		DWORD getPixelDataSize();
		DWORD getFirstMipSize();
		void recreateName();

		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

		int fromDds(dds* pDds, const char* pszName = NULL);
		int fromDdsMemory(BYTE* pData, const char* pszName = NULL);
		int fromDdsPath(const char *pszPath, const char* pszName = NULL);
		int ToDds(dds* pDds);
		int toDdsMemory(BYTE* pData, const char* pszName = NULL);
		int toDdsPath(const char *pszPath, const char* pszName = NULL);
		ConstString getClearName();
		DWORD getHash();
		grcTexturePC();
		~grcTexturePC();
		//rage::grcTexturePC& operator=(rage::grcTexturePC& txd);
	};

	static_assert(sizeof(grcTexturePC) == 0x50, "[rage::grcTexturePC] invalid size");


}