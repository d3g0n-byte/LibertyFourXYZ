#include "rage_grcFvf.h"
#include "utils.h"

#include "trace.h"

#include "rage_datResource.h"

namespace rage {

	const DWORD sm_TypeSizes[0x10]{
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

	const rage::grcFvf::grcFvfChannels s_DynamicOrder[0x12]{
		rage::grcFvf::grcFvfChannels::grcfcPosition,
		rage::grcFvf::grcFvfChannels::grcfcNormal,
		rage::grcFvf::grcFvfChannels::grcfcTangent0,
		rage::grcFvf::grcFvfChannels::grcfcTangent1,
		rage::grcFvf::grcFvfChannels::grcfcTexture0,
		rage::grcFvf::grcFvfChannels::grcfcTexture1,
		rage::grcFvf::grcFvfChannels::grcfcTexture2,
		rage::grcFvf::grcFvfChannels::grcfcTexture3,
		rage::grcFvf::grcFvfChannels::grcfcTexture4,
		rage::grcFvf::grcFvfChannels::grcfcTexture5,
		rage::grcFvf::grcFvfChannels::grcfcTexture6,
		rage::grcFvf::grcFvfChannels::grcfcTexture7,
		rage::grcFvf::grcFvfChannels::grcfcWeight,
		rage::grcFvf::grcFvfChannels::grcfcBinormal0,
		rage::grcFvf::grcFvfChannels::grcfcBinormal1,
		rage::grcFvf::grcFvfChannels::grcfcBinding,
		rage::grcFvf::grcFvfChannels::grcfcDiffuse,
		rage::grcFvf::grcFvfChannels::grcfcSpecular
	};

	DWORD rage::grcFvf::getSize(rage::grcFvf::grcFvfChannels channel) {
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
		return rage::sm_TypeSizes[nbChannelType];
	}

	DWORD rage::grcFvf::getDynamicDataSizeType(rage::grcFvf::grcFvfChannels channel) {
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

	DWORD rage::grcFvf::getOffset(rage::grcFvf::grcFvfChannels channel) {
		DWORD dwPosition = 0;
		if (this->m_bDynamicOrder) {
			DWORD dwFvf = *(DWORD*)this;
			for (rage::grcFvf::grcFvfChannels* curChannel = (rage::grcFvf::grcFvfChannels*)rage::s_DynamicOrder; ; curChannel++) {
				if (((1 << *curChannel) & dwFvf) == 1 << *curChannel) {
					if (*curChannel == channel)
						return dwPosition;
					dwPosition += rage::grcFvf::getSize(*curChannel);
				}
			}
		}
		else {
			// Внимание! если канал не используется, rage::grcFvf::getSize должен возвращать  0
			for (BYTE i = 0; i < channel; i++)
				if ((this->m_fvf.dwVal >> i) & 1)
					dwPosition += rage::grcFvf::getSize((rage::grcFvf::grcFvfChannels)i);
		}
		return dwPosition;
	}

	bool grcFvf::getChannelIsInUsed(rage::grcFvf::grcFvfChannels channel) { return (this->m_fvf.dwVal >> channel) & 1; }

	grcFvf::grcDataSize grcFvf::getChannelType(rage::grcFvf::grcFvfChannels channel) {
		if (channel < grcfcTangent0)
			return (grcFvf::grcDataSize)((this->m_fvfChannelSizes.qwVal >> (channel * 4)) & 0xf);
		else if (channel <= grcfcTangent0 && channel >= grcfcTangent1)
			return this->m_fvfChannelSizes.m_eTangentType;
		else if (channel <= grcfcBinormal0 && channel >= grcfcBinormal1)
			return this->m_fvfChannelSizes.m_eBinormalType;
		else {
			error("[grcFvf] unk channel type");
			return (grcFvf::grcDataSize)-1;
		}
	}

	void rage::grcFvf::recomputeTotalSize() {
		this->m_nbFvfSize = 0;
		DWORD dwFvf = *(DWORD*)this;
		for (BYTE i = 0; i < 16; i++)
			if (((1 << i) & dwFvf) == 1 << i) // можно обойтись без проверки
				this->m_nbFvfSize += sm_TypeSizes[(this->m_fvfChannelSizes.qwVal >> (i * 4)) & 0xf];
		if (this->m_bDynamicOrder)
			//this->m_nbFvfSize = alignValue(this->m_nbFvfSize, 16);
			this->m_nbFvfSize = (this->m_nbFvfSize + 0xf) & ~0xf;

	}

	grcFvf::~grcFvf() { memset(this, 0x0, sizeof * this); }
	grcFvf::grcFvf() { memset(this, 0x0, sizeof * this); }
	//inline rage::grcFvf& grcFvf::operator=(rage::grcFvf& fvf) { memcpy(this, &fvf, sizeof fvf); return *this; }

	// принимает rage поинтер
	void rage::grcFvf::place(rage::datResource* rsc) { /*memcpy(this, rsc->getFixup<rage::grcFvf>(ptr), sizeof *this);*/ }
	void rage::grcFvf::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {}
	void rage::grcFvf::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {}


};