#include "rage_float16.h"
#include <iostream>

namespace rage {

	Float16::Float16() : m_wVal(0) {};
	Float16::Float16(const Float16& _val) : m_wVal(_val.m_wVal) { } // to make it faster 
	Float16::Float16(const float& _val) { (*this) = _val; }
	Float16::~Float16() {};
	Float16::operator float() { return toFloat32(*this); }
	Float16& Float16::operator = (const Float16& _val) { return *(Float16*)&(this->m_wVal = _val.m_wVal); } // ToDo: check it
	Float16& Float16::operator = (const float& _val) { return (*this) = toFloat16(_val); } // ToDo: check it
	bool Float16::operator == (const Float16& _val) const { return this->m_wVal == _val.m_wVal; }
	bool Float16::operator != (const Float16& _val) const { return !(this->m_wVal == _val.m_wVal); }
	float Float16::getFloatDiff(float val) { return fabsf(fabsf(val) - fabsf(Float16::toFloat32(Float16::toFloat16(val)))); }

	float Float16::toFloat32(Float16 _val) {
		unsigned short float16_value = _val.m_wVal;
		unsigned sign = float16_value >> 15;
		unsigned exponent = (float16_value >> 10) & 0x1F;
		unsigned fraction = (float16_value & 0x3FF);
		unsigned float32_value;
		if (exponent == 0) {
			// zero
			if (fraction == 0) { float32_value = (sign << 31); }
			else {
				exponent = 127 - 14;
				while ((fraction & (1 << 10)) == 0) {
					exponent--;
					fraction <<= 1;
				}
				fraction &= 0x3FF;
				float32_value = (sign << 31) | (exponent << 23) | (fraction << 13);
			}
		}
		/* Inf or NaN */
		else if (exponent == 0x1F) { float32_value = (sign << 31) | (0xFF << 23) | (fraction << 13); }
		/* ordinary number */
		else { float32_value = (sign << 31) | ((exponent + (127 - 15)) << 23) | (fraction << 13); }
		return *((float*)&float32_value);
	}

	Float16 Float16::toFloat16(float _val) {
		Float16 fOutput;
		unsigned uiInput = *(unsigned*)&_val;

		if (0.0f == _val) {
			fOutput.m_wVal = 0;
			return fOutput;
		}

		else if (-0.0f == _val) {
			fOutput.m_wVal = 0x8000;
			return fOutput;
		}

		unsigned uiSignBit = uiInput >> 31;
		unsigned uiMantBits = (uiInput & 0x007fffff) >> 13;
		int  iExpBits = (uiInput >> 23) & 0xff;

		if (iExpBits == 0xff && uiMantBits == 0)
			fOutput.m_wVal = (uiSignBit ? 0xfc00 : 0x7c00);
		else {
			fOutput.m_wVal = ((((uiSignBit) << 15) & 0x8000) | ((fOutput.m_wVal) & (0x7c00 | 0x03ff)));
			if (iExpBits == 0)
				fOutput.m_wVal = ((((0) << 10) & 0x7c00) | ((fOutput.m_wVal) & (0x8000 | 0x03ff)));
			else if (0xff == iExpBits)
				fOutput.m_wVal = ((((0x1F) << 10) & 0x7c00) | ((fOutput.m_wVal) & (0x8000 | 0x03ff)));
			else {
				int iExponent = iExpBits - 127 + 15;

				if (iExponent < 0) { iExponent = 0; }
				else if (iExponent > 31) iExponent = 31;

				fOutput.m_wVal = ((((iExponent) << 10) & 0x7c00) | ((fOutput.m_wVal) & (0x8000 | 0x03ff)));
			}
			fOutput.m_wVal = ((((uiMantBits) << 0) & 0x03ff) | ((fOutput.m_wVal) & (0x8000 | 0x7c00)));
		}
		return fOutput;
	}
}