#pragma once
#include "rage_string.h"
#include "rage_math.h"

namespace rage {

	class crBoneData {
	public:
		ConstString m_pszName;
		union {
			struct {
				DWORD bLockRotXYZ : 1;
				DWORD bLockRotX : 1;
				DWORD bLockRotY : 1;
				DWORD bLockRotZ : 1;
				DWORD bLimitRotX : 1;
				DWORD bLimitRotY : 1;
				DWORD bLimitRotZ : 1;
				DWORD bLockTransX : 1;
				DWORD bLockTransY : 1;
				DWORD bLockTransZ : 1;
				DWORD bLimitTransX : 1;
				DWORD bLimitTransY : 1;
				DWORD bLimitTransZ : 1;
				DWORD bLockScaleX : 1;
				DWORD bLockScaleY : 1;
				DWORD bLockScaleZ : 1;
				DWORD bLimitScaleX : 1;
				DWORD bLimitScaleY : 1;
				DWORD bLimitScaleZ : 1;
				DWORD bVisible : 1;
			};
			DWORD m_dwFlags;
		};
		crBoneData* m_pNext;
		crBoneData* m_pChild;
		crBoneData* m_pParent;
		WORD m_wIndex;
		WORD m_wId;
		WORD m_wMirrorIndex;
		BYTE m_nbTransFlagsCount;
		BYTE m_nbRotFlagsCount;
		BYTE m_nbScaleFlagsCount;
		BYTE _1d[3];
		rage::Vector3 m_vDefaultTranslation; // m_vOffset;
		rage::Vector3 m_vDefaultRotation; //m_vRotationEuler;
		rage::Quaternion m_vDefaultRotationQuat; //m_vRotationQuaternion;
		rage::Vector3 m_vDefaultScale; //m_vScale;
		rage::Vector3 m_vGlobalOffset; //m_vParentModelOffset;
		rage::Vector3 m_vJointOrient; //m_vOrient;
		rage::Vector3 m_vScaleOrient; //m_vSorient;
		rage::Vector3 m_vTransMin;
		rage::Vector3 m_vTransMax;
		rage::Vector3 m_vRotMin;
		rage::Vector3 m_vRotMax;
		void* m_pJointData;
		DWORD m_dwNameHash;
		DWORD _fd8;
		DWORD _fdc;

		//crBoneData& operator=(crBoneData& bone);
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);

	};
}