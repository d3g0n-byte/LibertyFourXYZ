#include "rage_math.h"

namespace rage {
	Vector4::Vector4() { this->x = this->y = this->z = this->w = 0.0f; }
	Vector4& Vector4::operator=(Vector4& vec) {
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
		this->w = vec.w;
		return *this;
	}
	
	
	Vector3::Vector3() { this->x = this->y = this->z = this->w = 0.0f; }
	Vector3& Vector3::operator=(Vector3& vec) {
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
		this->w = vec.w;
		return *this;
	}

	spdAABB::spdAABB() {};
	spdAABB& spdAABB::operator=(spdAABB& vec) {
		this->m_vMaxAndBoundingRadius = vec.m_vMaxAndBoundingRadius;
		this->m_vMinAndBoundingRadius = vec.m_vMinAndBoundingRadius;
		return*this;
	}

	spdSphere::spdSphere() {}
	spdSphere& spdSphere::operator=(spdSphere& sphere) {
		this->m_vPositionAndRadius = sphere.m_vPositionAndRadius;
		return*this;
	}

	Half2::Half2() { }

	Half2::Half2(Vector2& _vec) {
		this->x = _vec.x;
		this->y = _vec.y;
	}

	Half2::Half2(float _x, float _y) {
		this->x = _x;
		this->y = _y;
	}

	Half4::Half4() { }

	Half4::Half4(Vector4& _vec) {
		this->x = _vec.x;
		this->y = _vec.y;
		this->z = _vec.z;
		this->w = _vec.w;
	}

	Half4::Half4(float _x, float _y, float _z, float _w) {
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->w = _w;
	}


}