#ifndef __FIXED_POINT__
#define __FIXED_POINT__

#define MAX_FIXED_POINT_WIDTH	64

#include <stdint.h>
#include <string>
#include <iostream>
#include <math.h>
using namespace std;

// Notes:
//		Be sure to use SetParam() to make sure values are the correct bit widths.
//		No alignment is done before or after arithmetic. The params of operand0 are assumed for overflow calculation 
//		for airthmetic operations where the bit width would remain the same, ie addition
//		subtraction, division

class FixedPoint {

	public:
		FixedPoint();
		FixedPoint(int length, int numFracBits);
		FixedPoint(int length, int numFracBits, int64_t value);
		FixedPoint(int length, int numFracBits, int value);
		FixedPoint(int length, int numFracBits, float value);
		~FixedPoint();
		uint64_t GetFracPart();
		uint64_t GetFracPart(int64_t value, int numFracBits);
		uint64_t GetIntPart();
		uint64_t GetIntPart(int64_t value, int length, int numFracBits);
		int64_t GetValue();
		void SetValue(float value);
		void SetValue(int64_t value);
		void SetValue(uint64_t value);
		void SetValue(int value);
		void SetParam(int length, int numFracBits);
		float toFloat();
		float toFloat(int64_t value, int length, int numFracBits);
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
		friend ostream& operator<<(ostream& o, FixedPoint value);
		
	protected:

	private:
		int64_t		m_value;
		int			m_numFracBits;
		int			m_length;

};

#endif