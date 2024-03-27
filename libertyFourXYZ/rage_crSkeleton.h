#pragma once
#include "rage_array.h"
#include "rage_base.h"
#include "rage_math.h"

namespace libertyFourXYZ { class rsc85_layout; };
namespace rage { class datResource; class crBoneData; }


namespace rage{
	class crJointData {
	public:
		struct crJointControlPoint {
			float fMaxSwing;
			float fMinTwist;
			float fMaxTwist;
		};

		int m_nBoneID;
		DWORD m_JointDOFs;
		bool m_bUseTwistLimits;
		BYTE m_nUsePerControlTwistLimits;
		BYTE __e[2];
		int m_nNumControlPoints;
		BYTE __14[0xc];
		rage::Vector3 m_twistAxis;
		int m_fTwistLimitMin;
		int m_fTwistLimitMax;
		int m_fSoftLimitScale;
		BYTE __3c[0x4];
		rage::Quaternion m_qZeroRotation;
		crJointControlPoint m_controlPoints[8];
		rage::Vector3 m_vZeroRotationEulers;

		inline virtual ~crJointData() {}
		__forceinline crJointData() {}
		inline void place(datResource* rsc){}
		inline void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth){}
		inline void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth){}
		rage::crJointData& operator=(crJointData& obj);

	};

	class crJointDataFile {
	public:
		atArray<crJointData> m_JointData;

		__forceinline virtual ~crJointDataFile(){}
		inline crJointDataFile(){}
	};

	class crSkeletonData {
	public:
		struct BoneIdData {
			WORD m_wId;
			WORD m_wIndex;
		};

		rage::datRef<crBoneData> m_pBones;
		rage::datRef<DWORD> m_pParentBoneIndices;
		rage::datRef<rage::Matrix34> m_pBoneWorldOrient;
		rage::datRef<rage::Matrix34> m_pBoneWorldOrientInverted;
		rage::datRef<rage::Matrix34> m_pBoneLocalTransforms;
		//rage::datRef<rage::Matrix34> m_pBoneLocalTransforms2; // rdr
		WORD m_wNumBones;
		WORD m_wTransLockCount;
		WORD m_wRotLockCount;
		WORD m_wScaleLockCount;
		union {
			struct {
				DWORD _f0 : 1;
				DWORD bHaveBoneMappings : 1;
				DWORD bHaveBoneWorldOrient : 1;
				DWORD bAuthoredOrientation : 1;
				DWORD _f4 : 1;
				DWORD __5 : 27;
			}; // flags
			DWORD m_dwFlags;
		};
		rage::atArray<crSkeletonData::BoneIdData> m_boneIdMappings; // ид и индексы. сортировака по ид
		DWORD m_dwUsageCount;
		DWORD m_dwCrc;
		rage::ConstString m_pszJointDataFileName;
		crJointDataFile m_jointData;

		float __fastcall calcAuthoredOffset(int nBoneId, rage::Vector3& a3);
		
		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();

		void buildHierarchy();
		void clearHierarchy();
		void replacePtrsInHierarchy(libertyFourXYZ::rsc85_layout* pLayout);

		crSkeletonData();
		~crSkeletonData();

	};

}