#pragma once
#include "rage_base.h"
#include "rage_math.h"

namespace rage { class datResource; }

	// ToDo: check size
class CLightAttr : rage::datBase {
public:
	VectorData m_vPosition;
	VectorData m_vDirection;
	VectorData m_vOrigin;
	ColourData m_color;
	BYTE m_nbFlashiness;
	float _f2c;
	float _f30;
	float _f34;
	float m_fDistance;
	float m_fRange;
	float m_fCoronaSize;
	float m_fHotSpot;
	float m_fFalloff;
	DWORD m_dwFlags;
	DWORD m_dwCoronaHash;
	DWORD m_dwLuminescenceHash;
	struct {
		DWORD flags : 24;
		DWORD type : 8;
	} TypeFlags;
	float _f5c;
	float _f60;
	float _f64;
	DWORD m_dwBoneId : 16;
	DWORD _f66 : 16;

	inline void place(rage::datResource* pRsc) {}
	inline void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth){}
	inline void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth){}
	inline void clearRefCount() {}
	inline void setRefCount() {}


};

