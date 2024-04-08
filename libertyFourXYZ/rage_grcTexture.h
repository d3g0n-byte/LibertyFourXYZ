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
		TEXTURE_REFERENCE,				// Ref to texture
	};

	class grcTextureReferenceBase : public pgBase {
	public:
		BYTE m_nbResourceType;
		BYTE _f9;
		WORD m_usageCount;
		DWORD _fc;

		void place(datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

	};

	class grcTextureReference : public grcTextureReferenceBase {
	public:
		DWORD _f10;
		ConstString m_pszName; // name or 'null'. 'null' is empty texture. name without pack:/, memory:$... etc
		grcTexturePC* m_pTexture; // NULL in resource.

		void place(datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

	};

	class grcTexture : public pgBase {
	public:
		BYTE m_nbResourceType; // 0 - rage::grcTexture{platform}, 2 - rage::grcTextureReference, 1 - Target?
		BYTE m_nbDepth; 
		WORD m_usageCount; // 0x4000?
		DWORD _fc;
		DWORD _f10;

		grcTexture();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

	};

	class grcTexturePC : public grcTexture {
	public:
		ConstString m_pszName;
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
		//grcTexturePC& operator=(grcTexturePC& txd);
	};

	static_assert(sizeof(grcTexturePC) == 0x50, "[rage::grcTexturePC] invalid size");


}