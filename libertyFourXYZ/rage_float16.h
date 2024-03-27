#pragma once

namespace rage {
	class Float16 {
		unsigned short m_wVal;
	public:
		Float16();
		Float16(const Float16& _val);
		Float16(const float& _val);
		~Float16();

		bool      operator == (const Float16& _val) const;
		bool      operator != (const Float16& _val) const;
		Float16& operator = (const Float16& _val);
		Float16& operator = (const float& _val);
		operator float();

		static float getFloatDiff(float val);

		static float toFloat32(Float16 _val);
		static Float16 toFloat16(float _val);
	};
}