#ifndef __FIXED_POINT_HPP__
#define __FIXED_POINT_HPP__

#define MAX_FIXED_POINT_WIDTH	32
#define DEFAULT_FRAC_WIDTH		16

#include <stdint.h>
#include <iostream>
#include <math.h>

#ifndef NDEBUG
#define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif


// Notes:
//		Be sure to use SetParam() to make sure values are the correct bit widths.
//		No alignment is done before or after arithmetic. The params of operand0 are assumed for overflow calculation 
//		for airthmetic operations where the bit width would remain the same (ie addition or substraction) and the DSP48_WIDTH
//		macros for multiplication
//
//		reinterpret cast are used to ensure no casting issues across windows or linux
//
//		Warnings about casting from unsigned numbers to signed numbers can be ignored
//
//      For mult() or div() ensure that the length, and numFracbits passed in is less than or equal to the products length or numFracBits
//

class FixedPoint {

	public:
		FixedPoint();	
		FixedPoint(int32_t value);
		FixedPoint(float value);
		FixedPoint(int length, int numFracBits);					
		FixedPoint(int length, int numFracBits, int32_t value);		// function is assuming just a regular number not a fixed point number
		FixedPoint(int length, int numFracBits, float value);
		~FixedPoint();
		uint32_t GetFracPart();
		static uint32_t GetFracPart(int numFracBits, int32_t value);
		uint32_t GetIntPart();
		static uint32_t GetIntPart(int length, int numFracBits, int32_t value);
		int32_t GetValue();
		void SetParam(int length, int numFracBits);
		float toFloat();
		static float toFloat(int32_t value, int length, int numFracBits);
		static FixedPoint mult(FixedPoint operand0, FixedPoint operand1, int length, int numFracBits);
		friend FixedPoint operator+(FixedPoint &operand0, FixedPoint &operand1);
		friend FixedPoint operator-(FixedPoint &operand0, FixedPoint &operand1);
		friend FixedPoint operator*(FixedPoint &operand0, FixedPoint &operand1);
		friend FixedPoint operator/(FixedPoint &operand0, FixedPoint &operand1);
        FixedPoint& operator+=(FixedPoint &rhs);
        FixedPoint& operator-=(FixedPoint &rhs);
        FixedPoint& operator*=(FixedPoint &rhs);
        FixedPoint& operator/=(FixedPoint &rhs);
		FixedPoint &operator=(const FixedPoint &obj);
		friend bool operator<(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator<=(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator>(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator>=(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator==(const FixedPoint &operand0, const FixedPoint &operand1);
		friend bool operator!=(const FixedPoint &operand0, const FixedPoint &operand1);
		friend std::ostream& operator<<(std::ostream& o, FixedPoint value);

		int32_t		m_value;    // need to use signed number for two's complement arithmetic
		int			m_numFracBits;
		int			m_length;


	protected:


	private:


};

#endif