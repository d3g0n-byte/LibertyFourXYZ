#pragma once
//#include "rage_datResource.h"
#include <Windows.h>

namespace rage { class datResource; }
namespace libertyFourXYZ { class rsc85_layout; };

namespace rage {
	class grcFvf {
	public:
		enum grcFvfChannels : DWORD {
			grcfcPosition = 0,
			grcfcWeight = 1,
			grcfcBinding = 2,
			grcfcNormal = 3,
			grcfcDiffuse = 4,
			grcfcSpecular = 5,
			grcfcTexture0 = 6,
			grcfcTexture1 = 7,
			grcfcTexture2 = 8,
			grcfcTexture3 = 9,
			grcfcTexture4 = 0xa,
			grcfcTexture5 = 0xb,
			grcfcTexture6 = 0xc,
			grcfcTexture7 = 0xd,
			grcfcTangent0 = 0xe,
			grcfcTangent1 = 0xf,
			grcfcBinormal0 = 0x10,
			grcfcBinormal1 = 0x11
		};

		enum grcDataSize : unsigned __int64 {
			grcdsHalf = 0,
			grcdsHalf2 = 1,
			grcdsHalf3 = 2,
			grcdsHalf4 = 3,
			grcdsFloat = 4,
			grcdsFloat2 = 5,
			grcdsFloat3 = 6,
			grcdsFloat4 = 7,
			grcdsUBYTE4 = 8,
			grcdsColor = 9,
			grcdsPackedNormal = 0xa,
			unk1 = 0xb, // variation for ps3
			unk2 = 0xc, // variation for ps3
			unk3 = 0xd, // variation for ps3
			grcdsShort2 = 0xe, // used in rdr terrain
			grcdsShort4 = 0xf
		};

		union {
			struct {
				DWORD m_bPosition : 1;
				DWORD m_bBlendWeight : 1;
				DWORD m_bBlendIndices : 1;
				DWORD m_bNormal : 1;
				DWORD m_bColor : 1;
				DWORD m_bSpecular : 1;
				DWORD m_bTexCoord0 : 1;
				DWORD m_bTexCoord1 : 1;
				DWORD m_bTexCoord2 : 1;
				DWORD m_bTexCoord3 : 1;
				DWORD m_bTexCoord4 : 1;
				DWORD m_bTexCoord5 : 1;
				DWORD m_bTexCoord6 : 1;
				DWORD m_bTexCoord7 : 1;
				DWORD m_bTangent0 : 1;
				DWORD m_bTangent1 : 1;
				DWORD m_bBinormal0 : 1;
				DWORD m_bBinormal1 : 1;
				DWORD _f18 : 14;
			};
			DWORD dwVal;
		} m_fvf;						// The fvf channels currently used.  Could be u16 now.
		BYTE m_nbFvfSize;				// The total size for this entire fvf
		union {
			struct {
				BYTE bPretransformed : 1;	// ?
				BYTE unk : 7;
			};
			BYTE nbVal;
		} m_nbFlags;					// Various flags to use (i.e. transformed positions, etc.)
		BYTE m_bDynamicOrder;			// This vertex format is in padded dynamic order instead of the standard order
		BYTE m_nbChannelCount;
		union {
			struct {
				grcDataSize m_ePositionType : 4;
				grcDataSize m_eBlendWeightType : 4;
				grcDataSize m_eBlendIndicesType : 4;
				grcDataSize m_eNormalType : 4;
				grcDataSize m_eColorType : 4;
				grcDataSize m_eSpecularColorType : 4;
				grcDataSize m_eTexCoord1Type : 4;
				grcDataSize m_eTexCoord2Type : 4;
				grcDataSize m_eTexCoord3Type : 4;
				grcDataSize m_eTexCoord4Type : 4;
				grcDataSize m_eTexCoord5Type : 4;
				grcDataSize m_eTexCoord6Type : 4;
				grcDataSize m_eTexCoord7Type : 4;
				grcDataSize m_eTexCoord8Type : 4;
				grcDataSize m_eTangentType : 4; // тип касательных
				grcDataSize m_eBinormalType : 4; // тип бинормалов
			};
			unsigned __int64 qwVal;
		} m_fvfChannelSizes;

		DWORD getSize(rage::grcFvf::grcFvfChannels channel);
		DWORD getDynamicDataSizeType(rage::grcFvf::grcFvfChannels channel);
		DWORD getOffset(rage::grcFvf::grcFvfChannels channel);
		bool getChannelIsInUsed(rage::grcFvf::grcFvfChannels channel);
		grcFvf::grcDataSize getChannelType(rage::grcFvf::grcFvfChannels channel);
		void recomputeTotalSize();

		grcFvf();
		~grcFvf();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		//rage::grcFvf& operator=(rage::grcFvf& fvf);



	};

	extern const DWORD sm_TypeSizes[0x10];
	extern const rage::grcFvf::grcFvfChannels s_DynamicOrder[0x12];
	// должно быть в rage::grcFvf


}