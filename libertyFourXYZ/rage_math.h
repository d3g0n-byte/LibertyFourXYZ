#pragma once
#include <Windows.h>
#include "rage_float16.h"

namespace rage {
	class Vector4 {
	public:
		float x;
		float y;
		float z;
		float w;

		Vector4();
		Vector4& operator=(Vector4& vec);
	};

	class Quaternion {
	public:
		float x;
		float y;
		float z;
		float w;
	};

	class Vector3 {
	public:
		float x;
		float y;
		float z;
		float w;

		Vector3();
		Vector3& operator=(Vector3& vec);
	};

	class Vector2 {
	public:
		float x;
		float y;
	};

	class spdSphere {
	public:
		rage::Vector4 m_vPositionAndRadius;

		spdSphere();
		spdSphere& operator=(spdSphere& sphere);
	};

	class spdAABB {
	public:
		rage::Vector4 m_vMinAndBoundingRadius;
		rage::Vector4 m_vMaxAndBoundingRadius; //m_vMaxAndInscribedRadius;

		spdAABB();
		spdAABB& operator=(spdAABB& vec);
	};

	class Matrix34 {
	public:
		rage::Vector3 a;
		rage::Vector3 b;
		rage::Vector3 c;
		rage::Vector3 d;
	};
	class Matrix44 : public Matrix34 { }; // ToDo

	class Half2 {
	public:
		Float16 x;
		Float16 y;

		Half2();
		Half2(Vector2 &_vec);
		Half2(float _x, float _y);
	};

	class Half4 {
	public:
		Float16 x;
		Float16 y;
		Float16 z;
		Float16 w;

		Half4();
		Half4(Vector4& _vec);
		Half4(float _x, float _y, float _z, float _w);

	};

}

struct VectorData {
	float x, y, z;
};

struct ColourData {
	BYTE r, g, b;
};
