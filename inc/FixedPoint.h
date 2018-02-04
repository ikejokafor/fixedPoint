#ifndef __FIXED_POINT_H__
#define __FIXED_POINT_H__

#define MAX_FIXED_POINT_WIDTH	64
#define DEFAULT_FRAC_WIDTH		32

#include <stdint.h>
#include <iostream>


// Notes:
//		Be sure to use SetParam() to make sure values are the correct bit widths.
//		No alignment is done before or after arithmetic. The params of operand0 are assumed for overflow calculation 
//		for airthmetic operations where the bit width would remain the same (ie addition or substraction) and the DSP48_WIDTH
//		macros for multiplication
//
//		reinterpret cast are used to ensure no casting issues across windows or linux
//
//		Warnings about casting from unsigned numbers to signed numbers can be ignored

class FixedPoint {

	public:
		FixedPoint();	
		FixedPoint(int value);
		FixedPoint(float value);
		FixedPoint(int length, int numFracBits);					
		FixedPoint(int length, int numFracBits, int64_t value);		// function is assuming just a regular number not a fixed point number
		FixedPoint(int length, int numFracBits, float value);
		~FixedPoint();
		uint64_t GetFracPart();
		static uint64_t GetFracPart(int64_t value, int numFracBits);
		uint64_t GetIntPart();
		static uint64_t GetIntPart(int64_t value, int length, int numFracBits);
		int64_t GetValue();
		void SetValue(int64_t value);
		void SetValue(float value);
		void SetParam(int length, int numFracBits);
		float toFloat();
		static float toFloat(int64_t value, int length, int numFracBits);
		FixedPoint mult(FixedPoint operand0, FixedPoint operand1, int length);
		FixedPoint div(FixedPoint operand0, FixedPoint operand1, int length);
		friend FixedPoint operator+(FixedPoint &operand0, FixedPoint &operand1);
		friend FixedPoint operator-(FixedPoint &operand0, FixedPoint &operand1);
		friend FixedPoint operator*(FixedPoint &operand0, FixedPoint &operand1);
		friend FixedPoint operator/(FixedPoint &operand0, FixedPoint &operand1);
		FixedPoint &operator=(const FixedPoint &obj);
		friend bool operator<(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator<=(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator>(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator>=(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator==(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator!=(const FixedPoint &operand0, const FixedPoint &operand1);
		friend std::ostream& operator<<(std::ostream& o, FixedPoint value);

		int64_t		m_value;
		int			m_numFracBits;
		int			m_length;


	protected:


	private:


};

#endif