#include "FixedPoint.hpp"
using namespace std;


FixedPoint::FixedPoint() {
	m_length = MAX_FIXED_POINT_WIDTH;
	m_numFracBits = DEFAULT_FRAC_WIDTH;
	m_value = 0;
}


FixedPoint::FixedPoint(int32_t value) {
	m_length = MAX_FIXED_POINT_WIDTH;
	m_numFracBits = DEFAULT_FRAC_WIDTH;
	int numIntBits = MAX_FIXED_POINT_WIDTH - DEFAULT_FRAC_WIDTH;
	if (value < 0) {
		// take magnitude 
		uint32_t m_value_mag = ~(uint32_t)value + (uint32_t)1;
		// mask and shift
		uint32_t mask = 0xFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_WIDTH - numIntBits);
		m_value_mag = (m_value_mag & mask) << DEFAULT_FRAC_WIDTH;
		// converted to two's complement so no sign extenstion needed
		m_value = ~m_value_mag + (uint32_t)1;
	} else {
		uint32_t mask = 0xFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_WIDTH - numIntBits);
		m_value = (value & mask) << DEFAULT_FRAC_WIDTH;
	}
}


FixedPoint::FixedPoint(float value) {
	m_length = MAX_FIXED_POINT_WIDTH;
	m_numFracBits = DEFAULT_FRAC_WIDTH;
	m_value = (int32_t)ceil(float((value * pow(float(2), m_numFracBits))));
}


FixedPoint::FixedPoint(int length, int numFracBits) {
	m_length = length; 
	m_numFracBits = numFracBits;
	m_value = 0;
}


FixedPoint::FixedPoint(int length, int numFracBits, int32_t value) {
	m_length = length;
	m_numFracBits = numFracBits;
	int numIntBits = length - numFracBits;
	if (value < 0) {
		// take magnitude 
		uint32_t m_value_mag = ~(uint32_t)value + (uint32_t)1;
		// mask and shift
		uint32_t mask = 0xFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_WIDTH - numIntBits);
		m_value_mag = (m_value_mag & mask) << numFracBits;
		// converted to two's complement so no sign extenstion needed
		m_value = ~m_value_mag + (uint32_t)1;
	} else {
		uint32_t mask = 0xFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_WIDTH - numIntBits);
		m_value = (value & mask) << numFracBits;
	}
}


FixedPoint::FixedPoint(int length, int numFracBits, float value) {
	m_length = length;
	m_numFracBits = numFracBits;
	// convert to fixed point
	m_value = (int32_t)ceil(float((value * pow(float(2), numFracBits))));
}


FixedPoint::~FixedPoint() {

}


uint32_t FixedPoint::GetFracPart() {
	uint32_t mask = 0xFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - m_numFracBits);
	return (uint32_t(m_value) & mask);
}


uint32_t FixedPoint::GetFracPart(int numFracBits, int32_t value) {
	uint32_t mask = 0xFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - numFracBits);
	return (uint32_t(value) & mask);
}


uint32_t FixedPoint::GetIntPart() {
	uint32_t mask = 0xFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - m_length);
	return ((uint32_t(m_value & mask)) >> m_numFracBits);
}


uint32_t FixedPoint::GetIntPart(int length, int numFracBits, int32_t value) {
	uint32_t mask = 0xFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - length);
	return ((uint32_t(value & mask)) >> numFracBits);
}


int32_t FixedPoint::GetValue() {
	return m_value;
}


void FixedPoint::SetParam(int length, int numFracBits) {
	uint32_t fracPart = GetFracPart();
	uint32_t intPart = GetIntPart();
	// Get sign of number
	uint32_t negative = (intPart >> (m_length - m_numFracBits - 1));
	int currentNumIntBits = (m_length - m_numFracBits);
	int newNumIntBits = length - numFracBits;

	if (m_numFracBits > numFracBits) {
		fracPart = fracPart >> (m_numFracBits - numFracBits);
	} else if (m_numFracBits < numFracBits) {
		fracPart = fracPart << (numFracBits - m_numFracBits);
	} 
	m_numFracBits = numFracBits;

	if (!negative && newNumIntBits < currentNumIntBits) {
		uint32_t mask = 0xFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_WIDTH - newNumIntBits);
		intPart = intPart & mask;
	} else if (negative && newNumIntBits < currentNumIntBits) {
		// sign extend
		uint32_t mask = 0xFFFFFFFF;
		mask = mask << (newNumIntBits);
		intPart = intPart | mask;
	}
	m_length = length;

	m_value = ((intPart << numFracBits) | fracPart);
}


float FixedPoint::toFloat() {
	return float(GetValue()) * pow(float(2), float(-m_numFracBits));
}

float FixedPoint::toFloat(int numFracBits, int32_t value) {
	return float(value) * pow(float(2), float(-numFracBits));;
}

FixedPoint FixedPoint::mult(int length, int numFracBits, FixedPoint operand0, FixedPoint operand1) {
	int32_t result = operand0.m_value * operand1.m_value;
    result = result >> ((operand0.m_numFracBits + operand1.m_numFracBits) - numFracBits);
	return FixedPoint(length, numFracBits, result);
}


FixedPoint operator+(FixedPoint &operand0, FixedPoint &operand1) {
	if (operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
		cout << "[ERROR]: Arithmetic of two different parameters" << endl;
		exit(1);
	}

	int32_t result = operand0.m_value + operand1.m_value;

	uint32_t result_sign_bit = (FixedPoint::GetIntPart(operand0.m_length, operand0.m_numFracBits, result)) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint32_t operand0_sign_bit = (operand0.GetIntPart()) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint32_t operand1_sign_bit = (operand1.GetIntPart()) >> (operand1.m_length - operand1.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && operand1_sign_bit == 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && operand1_sign_bit != 0 && result_sign_bit == 0)) {
		cout << "OverFlow in Addition: operands are " << operand0 << " and " << operand1 << endl;
		exit(1);
	}

	return FixedPoint(operand0.m_length, operand0.m_numFracBits, result);
}


FixedPoint operator-(FixedPoint &operand0, FixedPoint &operand1) {
	if (operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
		cout << "[ERROR]: Arithmetic of two different parameters" << endl;
		exit(1);
	}

	int32_t result = operand0.m_value - operand1.m_value;

	uint32_t result_sign_bit = (FixedPoint::GetIntPart(operand0.m_length, operand0.m_numFracBits, result)) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint32_t operand0_sign_bit = (operand0.GetIntPart()) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint32_t operand1_sign_bit = (operand1.GetIntPart()) >> (operand1.m_length - operand1.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && operand1_sign_bit != 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && operand1_sign_bit == 0 && result_sign_bit == 0)) {
		cout << "OverFlow in subtraction: operands are " << operand0 << " and " << operand1 << endl;
		exit(1);
	}

	return FixedPoint(operand0.m_length, operand0.m_numFracBits, result);
}


FixedPoint operator*(FixedPoint &operand0, FixedPoint &operand1) {
	int32_t result = operand0.m_value * operand1.m_value;
	return FixedPoint((operand0.m_length + operand1.m_length), (operand0.m_numFracBits + operand1.m_numFracBits), result);
}


FixedPoint operator/(FixedPoint &operand0, FixedPoint &operand1) {
	int32_t result = operand0.m_value / operand1.m_value;
	return FixedPoint(operand0.m_length, operand0.m_numFracBits, result);
}


FixedPoint& FixedPoint::operator+=(FixedPoint &rhs) {
	if (this->m_length != rhs.m_length || this->m_numFracBits != rhs.m_numFracBits) {
		cout << "[ERROR]: Arithmetic of two different parameters" << endl;
		exit(1);
	}

    int32_t result = this->m_value + rhs.m_value;

	uint32_t result_sign_bit = (FixedPoint::GetIntPart(this->m_length, this->m_numFracBits, result)) >> (this->m_length - this->m_numFracBits - 1);
	uint32_t operand0_sign_bit = (this->GetIntPart()) >> (this->m_length - this->m_numFracBits - 1);
	uint32_t rhs_sign_bit = (rhs.GetIntPart()) >> (rhs.m_length - rhs.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && rhs_sign_bit == 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && rhs_sign_bit != 0 && result_sign_bit == 0)) {
		cout << "OverFlow in Addition: operands are " << (*this) << " and " << rhs << endl;
		exit(1);
	}

	return *this;
}


FixedPoint& FixedPoint::operator-=(FixedPoint &rhs) {
	if (this->m_length != rhs.m_length || this->m_numFracBits != rhs.m_numFracBits) {
		cout << "[ERROR]: Arithmetic of two different parameters" << endl;
		exit(1);
	}

	int32_t result = this->m_value - rhs.m_value;

	uint32_t result_sign_bit = (this->GetIntPart(this->m_length, this->m_numFracBits, result)) >> (this->m_length - this->m_numFracBits - 1);
	uint32_t operand0_sign_bit = (this->GetIntPart()) >> (this->m_length - this->m_numFracBits - 1);
	uint32_t rhs_sign_bit = (rhs.GetIntPart()) >> (rhs.m_length - rhs.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && rhs_sign_bit != 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && rhs_sign_bit == 0 && result_sign_bit == 0)) {
		cout << "OverFlow in subtraction: operands are " << (*this) << " and " << rhs << endl;
		exit(1);
	}

	return *this;
}


FixedPoint& FixedPoint::operator*=(FixedPoint &rhs) {
	this->m_value *= this->m_value * rhs.m_value;
	return *this;   
}


FixedPoint& FixedPoint::operator/=(FixedPoint &rhs) {
	this->m_value *= this->m_value / rhs.m_value;
	return *this;   
}


FixedPoint& FixedPoint::operator=(const FixedPoint &obj) {
    if(m_length != obj.m_length || m_numFracBits != obj.m_numFracBits) {
        cout << "[ERROR]: assignment of two different parameters" << endl;
        exit(1);
    }
	m_value = obj.m_value;
	m_length = obj.m_length;
	m_numFracBits = obj.m_numFracBits;
	return *this;
}


bool operator<(const FixedPoint &operand0, const FixedPoint &operand1) {
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
	if (operand0.m_value < operand1.m_value) {
		return true;
	} else {
		return false;
	}
}


bool operator<=(const FixedPoint &operand0, const FixedPoint &operand1) {
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits !=  operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
	if (operand0.m_value <= operand1.m_value) {
		return true;
	} else {
		return false;
	}
}


bool operator>(const FixedPoint &operand0, const FixedPoint &operand1) {
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
	if (operand0.m_value > operand1.m_value) {
		return true;
	} else {
		return false;
	}
}


bool operator>=(const FixedPoint &operand0, const FixedPoint &operand1) {
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
	if (operand0.m_value >= operand1.m_value) {
		return true;
	} else {
		return false;
	}
}


bool operator==(const FixedPoint &operand0, const FixedPoint &operand1) {
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
	if (operand0.m_value == operand1.m_value) {
		return true;
	} else {
		return false;
	}
}


bool operator!=(const FixedPoint &operand0, const FixedPoint &operand1) {
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
	if (operand0.m_value != operand1.m_value) {
		return true;
	} else {
		return false;
	}
}


ostream& operator<<(ostream& o, FixedPoint value) {
	o << value.GetValue();
	return o;
}