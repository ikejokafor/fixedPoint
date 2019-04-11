#include "fixedPoint.hpp"
using namespace std;


fixedPoint::fixedPoint() {
	m_length = MAX_FIXED_POINT_LENGTH;
	m_numFracBits = DEFAULT_NUM_FRAC_BITS;
	m_value = 0;
}


fixedPoint::fixedPoint(int64_t value) {
	// verify there are no casting issues when using a uint64_t mask here
	m_length = MAX_FIXED_POINT_LENGTH;
	m_numFracBits = DEFAULT_NUM_FRAC_BITS;
	int numIntBits = MAX_FIXED_POINT_LENGTH - DEFAULT_NUM_FRAC_BITS;
	if (value < 0) {
		// take magnitude 
		int64_t m_value_mag = abs(value);
		// mask and shift
		uint64_t mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_LENGTH - numIntBits);
		m_value_mag = (m_value_mag & mask) << DEFAULT_NUM_FRAC_BITS;  // assuming no casting issues here
		// converted to two's complement so no sign extenstion needed
		m_value = ~m_value_mag + (uint64_t)1;
	} else {
		uint64_t mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_LENGTH - numIntBits);
		m_value = (value & mask) << DEFAULT_NUM_FRAC_BITS;	// assuming no casting issues here
	}
}


fixedPoint::fixedPoint(float value) {
	m_length = MAX_FIXED_POINT_LENGTH;
	m_numFracBits = DEFAULT_NUM_FRAC_BITS;
	m_value = (int64_t)floorf(float((value * pow(float(2), m_numFracBits))));
}


fixedPoint::fixedPoint(int length, int numFracBits) {
	m_length = length; 
	m_numFracBits = numFracBits;
	m_value = 0;
}


fixedPoint::fixedPoint(int length, int numFracBits, int64_t value, bool normalize) {
	// verify there are no casting issues when using a uint64_t mask here
	m_length = length;
	m_numFracBits = numFracBits;
	int numIntBits = length - numFracBits;
	if(normalize) {	// if using as constructor for new variable
		if (value < 0) {
			// take magnitude 
			int64_t m_value_mag = abs(value);
			// mask and shift
			uint64_t mask = 0xFFFFFFFFFFFFFFFF;
			mask = mask >> (MAX_FIXED_POINT_LENGTH - numIntBits);
			m_value_mag = (m_value_mag & mask) << numFracBits;
			// converted to two's complement so no sign extenstion needed
			m_value = ~m_value_mag + (uint64_t)1;
		} else {
			uint64_t mask = 0xFFFFFFFFFFFFFFFF;
			mask = mask >> (MAX_FIXED_POINT_LENGTH - numIntBits);
			m_value = (value & mask) << numFracBits;
		}
	} else {	// if reg arithm no need to noramlize
		m_value = value;
	}
}


fixedPoint::fixedPoint(int length, int numFracBits, float value) {
	m_length = length;
	m_numFracBits = numFracBits;
	// convert to fixed point
	m_value = (int64_t)floorf(float((value * pow(float(2), float(numFracBits)))));	// no sign extension needed
	SetParam(MAX_FIXED_POINT_LENGTH, numFracBits, length, numFracBits, m_value);
}


fixedPoint_t fixedPoint::create(int length, int numFracBits, float value) {
	// convert to fixed point
	fixedPoint_t fxValue = (fixedPoint_t)floorf(float((value * powf(float(2), float(numFracBits)))));	// no sign extension needed
	SetParam(MAX_FIXED_POINT_LENGTH, numFracBits, length, numFracBits, fxValue);
	return fxValue;
}


fixedPoint::~fixedPoint() {

}


int64_t fixedPoint::GetFracPart() {
	// use modf here instead
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_LENGTH - m_numFracBits);
	return m_value & mask;
}


int64_t fixedPoint::GetFracPart(int numFracBits, int64_t value) {
	// use modf here instead
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_LENGTH - numFracBits);
	return value & mask;
}


int64_t fixedPoint::GetIntPart() {
	// use modf here instead
	return (m_value >> m_numFracBits);
}


int64_t fixedPoint::GetIntPart(int length, int numFracBits, int64_t value) {
	// use modf here instead
	return (value >> numFracBits);
}


int64_t fixedPoint::GetValue() {
	return m_value;
}


void fixedPoint::SetParam(int oldLength, int oldNumFracBits, int newLength, int newNumFracBits, fixedPoint_t *num_arry, int arryLength) {
	if(oldLength != newLength || oldNumFracBits != newNumFracBits) {
		for(int i = 0; i < arryLength; i++) {
			if (oldNumFracBits > newNumFracBits) {
				num_arry[i] = num_arry[i] >> (oldNumFracBits - newNumFracBits);
			} else if (oldNumFracBits < newNumFracBits) {
				num_arry[i] = num_arry[i] << (newNumFracBits - oldNumFracBits);
			}
			
			int currentNumIntBits = (oldLength - oldNumFracBits);
			int newNumIntBits = newLength - newNumFracBits;
			bool negative = (num_arry[i] < 0) ? true : false;
			if (!negative && newNumIntBits < currentNumIntBits) {
				uint64_t mask = 0xFFFFFFFFFFFFFFFF;
				mask = mask << (MAX_FIXED_POINT_LENGTH - newLength);
				mask = mask >> (MAX_FIXED_POINT_LENGTH - newLength);
				num_arry[i] = num_arry[i] & mask;
			} else if (negative && newNumIntBits < currentNumIntBits) {
				uint64_t mask = 0xFFFFFFFFFFFFFFFF;
				mask = mask << newLength;
				num_arry[i] = num_arry[i] | mask;	// assuming no casting issues here
			}
		}
	}
}


void fixedPoint::SetParam(int oldLength, int oldNumFracBits, int newLength, int newNumFracBits, fixedPoint_t &num) {
	if(oldLength != newLength || oldNumFracBits != newNumFracBits) {
		if (oldNumFracBits > newNumFracBits) {
			num = num >> (oldNumFracBits - newNumFracBits);
		} else if (oldNumFracBits < newNumFracBits) {
			num = num << (newNumFracBits - oldNumFracBits);
		}
		
		int currentNumIntBits = (oldLength - oldNumFracBits);			
		int newNumIntBits = newLength - newNumFracBits;
		bool negative = (num < 0) ? true : false;
		if (!negative && newNumIntBits < currentNumIntBits) {
			uint64_t mask = 0xFFFFFFFFFFFFFFFF;
			mask = mask << (MAX_FIXED_POINT_LENGTH - newLength);
			mask = mask >> (MAX_FIXED_POINT_LENGTH - newLength);
			num = num & mask;
		} else if (negative && newNumIntBits < currentNumIntBits) {
			uint64_t mask = 0xFFFFFFFFFFFFFFFF;
			mask = mask << newLength;
			num = num | mask;	// assuming no casting issues here
		}
	}
}


void fixedPoint::SetParam(int newLength, int newNumFracBits) {
	if (m_length != newLength || m_numFracBits != newNumFracBits) {
		if (m_numFracBits > newNumFracBits) {
			m_value = m_value >> (m_numFracBits - newNumFracBits);
		} else if (m_numFracBits < newNumFracBits) {
			m_value = m_value << (newNumFracBits - m_numFracBits);
		}

		int currentNumIntBits = (m_length - m_numFracBits);			
		int newNumIntBits = newLength - newNumFracBits;
		bool negative = (m_value < 0) ? true : false;
		if (!negative && newNumIntBits < currentNumIntBits) {
			uint64_t mask = 0xFFFFFFFFFFFFFFFF;
			mask = mask << (MAX_FIXED_POINT_LENGTH - newLength);
			mask = mask >> (MAX_FIXED_POINT_LENGTH - newLength);
			m_value = m_value & mask;
		} else if (negative && newNumIntBits < currentNumIntBits) {
			uint64_t mask = 0xFFFFFFFFFFFFFFFF;
			mask = mask << newLength;
			m_value = m_value | mask;	// assuming no casting issues here
		}
		m_numFracBits = newNumFracBits;
		m_length = newLength;
	}
}


float fixedPoint::toFloat() {
	return float(GetValue()) * pow(float(2), float(-m_numFracBits));
}


float fixedPoint::toFloat(int numFracBits, int64_t value) {
	return float(value) * powf(float(2), float(-numFracBits));
}


fixedPoint operator+(fixedPoint &operand0, fixedPoint &operand1) {
	// redo this function for getting sign bit
	// verify there are not casting issues when using uint64_t here
#ifdef NUMERIC_CHECKING
	if (operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
		cout << "[ERROR]: Arithmetic of two different parameters" << endl;
		exit(1);
	}
#endif
    int64_t result = operand0.m_value + operand1.m_value;
#ifdef NUMERIC_CHECKING
	uint64_t result_sign_bit = (fixedPoint::GetIntPart(operand0.m_length, operand0.m_numFracBits, result)) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint64_t operand0_sign_bit = (operand0.GetIntPart()) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint64_t operand1_sign_bit = (operand1.GetIntPart()) >> (operand1.m_length - operand1.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && operand1_sign_bit == 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && operand1_sign_bit != 0 && result_sign_bit == 0)) {
		cout << "OverFlow in Addition: operands are " << operand0 << " and " << operand1 << endl;
		exit(1);
	}
#endif
	return fixedPoint(operand0.m_length, operand0.m_numFracBits, result, false);
}


fixedPoint operator-(fixedPoint &operand0, fixedPoint &operand1) {
	// redo this function for getting sign bit
	// verify there are not casting issues when using uint64_t here
#ifdef NUMERIC_CHECKING
	if (operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
		cout << "[ERROR]: Arithmetic of two different parameters" << endl;
		exit(1);
	}
#endif
	int64_t result = operand0.m_value - operand1.m_value;   
#ifdef NUMERIC_CHECKING
	uint64_t result_sign_bit = (fixedPoint::GetIntPart(operand0.m_length, operand0.m_numFracBits, result)) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint64_t operand0_sign_bit = (operand0.GetIntPart()) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint64_t operand1_sign_bit = (operand1.GetIntPart()) >> (operand1.m_length - operand1.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && operand1_sign_bit != 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && operand1_sign_bit == 0 && result_sign_bit == 0)) {
		cout << "OverFlow in subtraction: operands are " << operand0 << " and " << operand1 << endl;
		exit(1);
	}
#endif
	return fixedPoint(operand0.m_length, operand0.m_numFracBits, result, false);
}


fixedPoint operator*(fixedPoint &operand0, fixedPoint &operand1) {
	int64_t result = operand0.m_value * operand1.m_value;
	return fixedPoint((operand0.m_length + operand1.m_length), (operand0.m_numFracBits + operand1.m_numFracBits), result, false);
}


fixedPoint operator/(fixedPoint &operand0, fixedPoint &operand1) {
	int64_t result = operand0.m_value / operand1.m_value;
	return fixedPoint(operand0.m_length, operand0.m_numFracBits, result, false);
}


fixedPoint& fixedPoint::operator+=(fixedPoint &rhs) {
	// redo this function for getting sign bit
	// verify there are not casting issues when using uint64_t here
#ifdef NUMERIC_CHECKING   
	if (this->m_length != rhs.m_length || this->m_numFracBits != rhs.m_numFracBits) {
		cout << "[ERROR]: Arithmetic of two different parameters" << endl;
		exit(1);
	}
#endif
	this->m_value = this->m_value + rhs.m_value;
#ifdef NUMERIC_CHECKING    
	uint64_t result_sign_bit = (fixedPoint::GetIntPart(this->m_length, this->m_numFracBits, this->m_value)) >> (this->m_length - this->m_numFracBits - 1);
	uint64_t operand0_sign_bit = (this->GetIntPart()) >> (this->m_length - this->m_numFracBits - 1);
	uint64_t rhs_sign_bit = (rhs.GetIntPart()) >> (rhs.m_length - rhs.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && rhs_sign_bit == 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && rhs_sign_bit != 0 && result_sign_bit == 0)) {
		cout << "OverFlow in Addition: operands are " << (*this) << " and " << rhs << endl;
		exit(1);
	}
#endif
	return *this;
}


fixedPoint& fixedPoint::operator-=(fixedPoint &rhs) {
	// redo this function for getting sign bit
	// verify there are not casting issues when using uint64_t here		
#ifdef NUMERIC_CHECKING
	if (this->m_length != rhs.m_length || this->m_numFracBits != rhs.m_numFracBits) {
		cout << "[ERROR]: Arithmetic of two different parameters" << endl;
		exit(1);
	}
#endif
	this->m_value = this->m_value - rhs.m_value;
#ifdef NUMERIC_CHECKING    
	uint64_t result_sign_bit = (this->GetIntPart(this->m_length, this->m_numFracBits, this->m_value)) >> (this->m_length - this->m_numFracBits - 1);
	uint64_t operand0_sign_bit = (this->GetIntPart()) >> (this->m_length - this->m_numFracBits - 1);
	uint64_t rhs_sign_bit = (rhs.GetIntPart()) >> (rhs.m_length - rhs.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && rhs_sign_bit != 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && rhs_sign_bit == 0 && result_sign_bit == 0)) {
		cout << "OverFlow in subtraction: operands are " << (*this) << " and " << rhs << endl;
		exit(1);
	}
#endif
	return *this;
}


fixedPoint& fixedPoint::operator*=(fixedPoint &rhs) {
	this->m_value *= this->m_value * rhs.m_value;
	return *this;   
}


fixedPoint& fixedPoint::operator/=(fixedPoint &rhs) {
	this->m_value *= this->m_value / rhs.m_value;
	return *this;   
}


fixedPoint& fixedPoint::operator=(const fixedPoint &obj) {
#ifdef NUMERIC_CHECKING    
    if(m_length != obj.m_length || m_numFracBits != obj.m_numFracBits) {
        cout << "[ERROR]: assignment of two different parameters" << endl;
        raise(4);
        exit(1);
    }
#endif   
	m_value = obj.m_value;
	m_length = obj.m_length;
	m_numFracBits = obj.m_numFracBits;
	return *this;
}


bool operator<(const fixedPoint &operand0, const fixedPoint &operand1) {
#ifdef NUMERIC_CHECKING    
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
#endif
	return (operand0.m_value < operand1.m_value);
}


bool operator<=(const fixedPoint &operand0, const fixedPoint &operand1) {
#ifdef NUMERIC_CHECKING  
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits !=  operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
#endif
	return (operand0.m_value <= operand1.m_value);
}


bool operator>(const fixedPoint &operand0, const fixedPoint &operand1) {
#ifdef NUMERIC_CHECKING
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
#endif
	return (operand0.m_value > operand1.m_value);
}


bool operator>=(const fixedPoint &operand0, const fixedPoint &operand1) {
#ifdef NUMERIC_CHECKING    
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
#endif
	return (operand0.m_value >= operand1.m_value);
}


bool operator==(const fixedPoint &operand0, const fixedPoint &operand1) {
#ifdef NUMERIC_CHECKING 
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
#endif
	return (operand0.m_value == operand1.m_value);
}


bool operator!=(const fixedPoint &operand0, const fixedPoint &operand1) {
#ifdef NUMERIC_CHECKING     
    if(operand0.m_length != operand0.m_length || operand1.m_numFracBits != operand1.m_numFracBits) {
        cout << "[ERROR]: comparison of two different parameters" << endl;
        exit(1);
    }
#endif
	return (operand0.m_value != operand1.m_value);
}


ostream& operator<<(ostream& o, fixedPoint value) {
	o << value.GetValue();
	return o;
}
