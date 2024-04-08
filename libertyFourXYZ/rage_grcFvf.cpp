#include "rage_grcFvf.h"
#include "utils.h"

#include "trace.h"

#include "rage_datResource.h"

namespace rage {

	const DWORD grcFvf::s_TypeSizes[]{
	0x2,	// float16
	0x4,	// float16_2
	0x6,	// float16_3
	0x8,	// float16_4
	0x4,	// float
	0x8,	// float2
	0xc,	// float3
	0x10,	// float4
	0x4,	// ubyte4
	0x4,	// color, DWORD
	0x4,	// dec3n
	0x8,	//
	0x8,	//
	0x0,	//
	0x4,	// short2
	0x8		// short4
	};

	const grcFvfChannels grcFvf::s_DynamicOrder[]{
				grcFvfChannels::fvfPosition,
	grcFvfChannels::fvfNormal,
	grcFvfChannels::fvfTangent0,
	grcFvfChannels::fvfTangent1,
	grcFvfChannels::fvfTexture0,
	grcFvfChannels::fvfTexture1,
	grcFvfChannels::fvfTexture2,
	grcFvfChannels::fvfTexture3,
	grcFvfChannels::fvfTexture4,
	grcFvfChannels::fvfTexture5,
	grcFvfChannels::fvfTexture6,
	grcFvfChannels::fvfTexture7,
	grcFvfChannels::fvfWeight,
	grcFvfChannels::fvfBinormal0,
	grcFvfChannels::fvfBinormal1,
	grcFvfChannels::fvfBinding,
	grcFvfChannels::fvfDiffuse,
	grcFvfChannels::fvfSpecular
	};

	DWORD grcFvf::getSize(grcFvfChannels channel) {
		DWORD dwFvf = *(DWORD*)this;
		DWORD qwFvfChannelSizes = *((unsigned __int64*)this + 1);
		BYTE nbChannelType;

		// если канал не используется
		if (((1 << channel) & dwFvf) != 1 << channel)
			return 0;

		if (this->m_bDynamicOrder)
			nbChannelType = rage::grcFvf::getDynamicDataSizeType(channel);
		else
			nbChannelType = (qwFvfChannelSizes >> (4 * channel)) & 0xf;
		return s_TypeSizes[nbChannelType];
	}

	DWORD grcFvf::getDynamicDataSizeType(grcFvfChannels channel) {
		bool bIsOrbis = 0;
		bool bIsCell = 0;

		if (channel < 0)
			return 16;
		if (channel <= 1)
			return 7;
		if (channel == 2) {
			if (bIsOrbis || bIsCell) // в ps3 не нужно разворачивать канал с индексом 2 ибо он будет ubyte4
				return 8;
			return 9;
		}
		if (channel == 3)
			return 7;
		if (channel > 5) {
			if (channel > 17) {
				if (channel == 18)
					//error("Invalid FVF channel"); // зачем?
				return 16;
			}
			return 7;
		}
		return 9;
	}

	DWORD grcFvf::getOffset(grcFvfChannels channel) {
		DWORD dwPosition = 0;
		if (this->m_bDynamicOrder) {
			DWORD dwFvf = *(DWORD*)this;
			for (grcFvfChannels* curChannel = (grcFvfChannels*)s_DynamicOrder; ; curChannel++) {
				if (((1 << *curChannel) & dwFvf) == 1 << *curChannel) {
					if (*curChannel == channel)
						return dwPosition;
					dwPosition += rage::grcFvf::getSize(*curChannel);
				}
			}
		}
		else
			for (BYTE i = 0; i < channel; i++)
				if ((this->m_UsedElements.dwVal >> i) & 1)
					dwPosition += rage::grcFvf::getSize((grcFvfChannels)i);
		return dwPosition;
	}

	bool grcFvf::getChannelIsInUsed(grcFvfChannels channel) { return (this->m_UsedElements.dwVal >> channel) & 1; }

	grcDataType grcFvf::getChannelType(grcFvfChannels channel) {
		if (channel < fvfTangent0)
			return (grcDataType)((this->m_Types.qwVal >> (channel * 4)) & 0xf);
		else if (channel <= fvfTangent0 && channel >= fvfTangent1)
			return this->m_Types.m_eTangentType;
		else if (channel <= fvfBinormal0 && channel >= fvfBinormal1)
			return this->m_Types.m_eBinormalType;
		else {
			error("[grcFvf] unk element type");
			return (grcDataType)-1;
		}
	}

	void grcFvf::recomputeTotalSize() {
		this->m_nbTotalSize = 0;
		DWORD dwFvf = *(DWORD*)this;
		for (BYTE i = 0; i < 16; i++)
			if (((1 << i) & dwFvf) == 1 << i) // можно обойтись без проверки
				this->m_nbTotalSize += s_TypeSizes[(this->m_Types.qwVal >> (i * 4)) & 0xf];
		if (this->m_bDynamicOrder)
			//this->m_nbTotalSize = (this->m_nbTotalSize + 0xf) & ~0xf;
			error("dynamic order");
	}

	grcFvf::~grcFvf() { memset(this, 0x0, sizeof * this); }
	grcFvf::grcFvf() { memset(this, 0x0, sizeof * this); }

	void grcFvf::place(rage::datResource* rsc) { }
	void grcFvf::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {}
	void grcFvf::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {}


};