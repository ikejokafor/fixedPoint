#ifndef __FIXED_POINT_HPP__
#define __FIXED_POINT_HPP__

#define MAX_FIXED_POINT_LENGTH	64
#define DEFAULT_NUM_FRAC_BITS	32


#include <stdint.h>
#include <iostream>
#include <limits.h>
#include <cstdint> 
#include <math.h>
#include <signal.h>

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
//      TODO: for create function, must chop of top bits to make sure integer portion is correct

typedef int64_t fixedPoint_t;

class fixedPoint {

	public:
		fixedPoint();	
		fixedPoint(int64_t value);
		fixedPoint(float value);
		fixedPoint(int length, int numFracBits);					
		fixedPoint(int length, int numFracBits, int64_t value, bool normalize = true);
		fixedPoint(int length, int numFracBits, float value);
		static fixedPoint_t create(int length, int numFracBits, float value);
		~fixedPoint();
		int64_t GetFracPart();
		static int64_t GetFracPart(int numFracBits, int64_t value);
		int64_t GetIntPart();
		static int64_t GetIntPart(int length, int numFracBits, int64_t value);
		int64_t GetValue();
		void SetParam(int newLength, int newNumFracBits);
        static void SetParam(int oldLength, int oldNumFracBits, int newLength, int newNumFracBits, fixedPoint_t &num);
		static void SetParam(int oldLength, int oldNumFracBits, int newLength, int newNumFracBits, fixedPoint_t *num_arry, int arryLength);
		float toFloat();
		static float toFloat(int numFracBits, fixedPoint_t value);
		friend fixedPoint operator+(fixedPoint &operand0, fixedPoint &operand1);
		friend fixedPoint operator-(fixedPoint &operand0, fixedPoint &operand1);
		friend fixedPoint operator*(fixedPoint &operand0, fixedPoint &operand1);
		friend fixedPoint operator/(fixedPoint &operand0, fixedPoint &operand1);
        fixedPoint& operator+=(fixedPoint &rhs);
        fixedPoint& operator-=(fixedPoint &rhs);
        fixedPoint& operator*=(fixedPoint &rhs);
        fixedPoint& operator/=(fixedPoint &rhs);
		fixedPoint& operator=(const fixedPoint &obj);
		friend bool operator<(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator<=(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator>(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator>=(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator==(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator!=(const fixedPoint &operand0, const fixedPoint &operand1);
		friend std::ostream& operator<<(std::ostream& o, fixedPoint value);

		int64_t		m_value;    // need to use signed number for two's complement arithmetic
		int			m_numFracBits;
		int			m_length;


	protected:


	private:


};

#endif