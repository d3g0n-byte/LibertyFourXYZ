#pragma once
//#include "rage_datResource.h"
#include <Windows.h>

namespace rage { class datResource; }
namespace libertyFourXYZ { class rsc85_layout; };

namespace rage {
	enum grcFvfChannels : DWORD {
		fvfPosition = 0,
		fvfWeight = 1,
		fvfBinding = 2,
		fvfNormal = 3,
		fvfDiffuse = 4,
		fvfSpecular = 5,
		fvfTexture0 = 6,
		fvfTexture1 = 7,
		fvfTexture2 = 8,
		fvfTexture3 = 9,
		fvfTexture4 = 0xa,
		fvfTexture5 = 0xb,
		fvfTexture6 = 0xc,
		fvfTexture7 = 0xd,
		fvfTangent0 = 0xe,
		fvfTangent1 = 0xf,
		fvfBinormal0 = 0x10,
		fvfBinormal1 = 0x11
	};

	enum grcDataType : unsigned __int64 {
		fvfHalf = 0, // unused in iv pc
		fvfHalf2 = 1,
		fvfHalf3 = 2, // unused in iv pc
		fvfHalf4 = 3,
		fvfFloat = 4,
		fvfFloat2 = 5,
		fvfFloat3 = 6,
		fvfFloat4 = 7,
		fvfUBYTE4 = 8,
		fvfColor = 9,
		fvfDec3n = 0xa, // unused in iv pc
		fvfunk1 = 0xb, // variation for ps3
		fvfunk2 = 0xc, // variation for ps3
		fvfunk3 = 0xd, // variation for ps3
		fvfShort2 = 0xe, // used in rdr terrain
		fvfShort4 = 0xf
	};


	class grcFvf {
	public:
		static const DWORD s_TypeSizes[/*0x10*/];
		static const grcFvfChannels s_DynamicOrder[/*0x12*/];

		// in iv and mcla there is info about 8 channels of tangents and binormals. I don't use them yet
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
		} m_UsedElements;
		BYTE m_nbTotalSize;
		union {
			struct {
				BYTE bPretransformed : 1;
				BYTE unk : 7;
			};
			BYTE nbVal;
		} m_nbFlags;
		BYTE m_bDynamicOrder;			// see iv_pc_wdr for example usage
		BYTE m_nbElementCount;
		union {
			struct {
				grcDataType m_ePositionType : 4;
				grcDataType m_eBlendWeightType : 4;
				grcDataType m_eBlendIndicesType : 4;
				grcDataType m_eNormalType : 4;
				grcDataType m_eColorType : 4;
				grcDataType m_eSpecularColorType : 4;
				grcDataType m_eTexCoord1Type : 4;
				grcDataType m_eTexCoord2Type : 4;
				grcDataType m_eTexCoord3Type : 4;
				grcDataType m_eTexCoord4Type : 4;
				grcDataType m_eTexCoord5Type : 4;
				grcDataType m_eTexCoord6Type : 4;
				grcDataType m_eTexCoord7Type : 4;
				grcDataType m_eTexCoord8Type : 4;
				grcDataType m_eTangentType : 4; // one type for all channels
				grcDataType m_eBinormalType : 4; // one type for all channels
			};
			unsigned __int64 qwVal;
		} m_Types;

		DWORD getSize(grcFvfChannels channel);
		DWORD getDynamicDataSizeType(grcFvfChannels channel);
		DWORD getOffset(grcFvfChannels channel);
		bool getChannelIsInUsed(grcFvfChannels channel);
		grcDataType getChannelType(grcFvfChannels channel);
		void recomputeTotalSize();

		grcFvf();
		~grcFvf();
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		//rage::grcFvf& operator=(rage::grcFvf& fvf);



	};

	//extern const DWORD sm_TypeSizes[0x10];
	//extern const grcFvfChannels s_DynamicOrder[0x12];


}