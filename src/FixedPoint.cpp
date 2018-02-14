#include "FixedPoint.hpp"
using namespace std;


FixedPoint::FixedPoint() {
	m_length = MAX_FIXED_POINT_WIDTH;
	m_numFracBits = DEFAULT_FRAC_WIDTH;
	m_value = (int64_t)0;
}


FixedPoint::FixedPoint(int value) {
	m_length = MAX_FIXED_POINT_WIDTH;
	m_numFracBits = DEFAULT_FRAC_WIDTH;
	m_value = (value << m_numFracBits);
}


FixedPoint::FixedPoint(float value) {
	m_length = MAX_FIXED_POINT_WIDTH;
	m_numFracBits = DEFAULT_FRAC_WIDTH;
	m_value = (int64_t)ceil((float)(value * (2, m_numFracBits)));
}


FixedPoint::FixedPoint(int length, int numFracBits) {
	m_length = length; 
	m_numFracBits = numFracBits;
	m_value = (int64_t)0;
}


FixedPoint::FixedPoint(int length, int numFracBits, int64_t value) {
	m_length = length;
	m_numFracBits = numFracBits;
	uint64_t m_value_tmp = ~(uint64_t)value + (uint64_t)1;
	m_value_tmp = (m_value_tmp << numFracBits);
	m_value = ~m_value_tmp + (uint64_t)1;
}


FixedPoint::FixedPoint(int length, int numFracBits, float value) {
	m_length = length;
	m_numFracBits = numFracBits;
	m_value = (int64_t)ceil((float)(value * pow((float)2, numFracBits)));
}


FixedPoint::~FixedPoint() {

}


uint64_t FixedPoint::GetFracPart() {
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - m_numFracBits);
	return (uint64_t)m_value & mask;
}


uint64_t FixedPoint::GetFracPart(int64_t value, int numFracBits) {
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - numFracBits);
	return ((uint64_t)value & mask);
}


uint64_t FixedPoint::GetIntPart() {
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - m_length);
	return (((uint64_t)m_value & mask) >> m_numFracBits);
}


uint64_t FixedPoint::GetIntPart(int64_t value, int length, int numFracBits) {
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - length);
	return (((uint64_t)value & mask) >> numFracBits);
}


int64_t FixedPoint::GetValue() {
	return m_value;
}


void FixedPoint::SetValue(int64_t value) {
	m_value = value;
}


void FixedPoint::SetValue(float value) {
	m_value = (int64_t)ceil((float)(value * (2, m_numFracBits)));
}


void FixedPoint::SetParam(int length, int numFracBits) {
	uint64_t fracPart = GetFracPart();
	uint64_t intPart = GetIntPart();
	uint64_t sign_bit = (intPart >> (m_length - m_numFracBits - 1));
	int currentNumIntBits = (m_length - m_numFracBits);
	int newNumIntBits = length - numFracBits;

	if (m_numFracBits > numFracBits) {
		fracPart = fracPart >> (m_numFracBits - numFracBits);
	} else if (m_numFracBits < numFracBits) {
		fracPart = fracPart << (numFracBits - m_numFracBits);
	} 
	m_numFracBits = numFracBits;

	if (currentNumIntBits > newNumIntBits) {
		uint64_t mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_WIDTH - newNumIntBits);
		intPart = intPart & mask;
	} else if (currentNumIntBits < newNumIntBits && sign_bit != 0) {
		// sign extend
		uint64_t mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask << (currentNumIntBits);
		intPart = intPart | mask;
	}
	m_length = length;

	int64_t m_value = ((intPart << numFracBits) | fracPart);
}

float FixedPoint::toFloat() {
	int64_t value = GetValue();
	uint32_t signBit = 0;
	uint64_t _value = value;

	uint64_t intPart = GetIntPart(value, m_length, m_numFracBits);
	uint64_t fracPart = GetFracPart(value, m_numFracBits);
	// get the two's compliment if necessary
	if ((intPart >> (m_length - m_numFracBits - 1)) != 0) {
		signBit = 0x80000000;
		_value = (~_value) + (uint64_t)1;
		intPart = GetIntPart(_value, m_length, m_numFracBits);
		fracPart = GetFracPart(_value, m_numFracBits);
	}

	float intToFloat = (float)intPart;

	// get fractional component
	int power = -1;
	float fracToFloat = 0;
	uint64_t mask = 0x0000000000000001;
	mask = mask << (m_numFracBits - 1);
	for (int i = 0; i < m_numFracBits; i++, power--, mask = mask >> 1){
		uint64_t bit = mask & fracPart;
		if (bit != 0){
			fracToFloat += ((float)pow((float)2, power));
		}
	}

	float fixedToFloat = intToFloat + fracToFloat;

	// put sign bit after adding integer and fractional parts
	uint32_t temp;
	uint8_t *temp_ptr = reinterpret_cast<uint8_t*>(&temp);
	uint8_t *fixedToFloat_ptr = reinterpret_cast<uint8_t*>(&fixedToFloat);
	temp_ptr[0] = fixedToFloat_ptr[0]; temp_ptr[1] = fixedToFloat_ptr[1]; temp_ptr[2] = fixedToFloat_ptr[2]; temp_ptr[3] = fixedToFloat_ptr[3];
	temp = temp | (uint32_t)signBit;
	fixedToFloat_ptr[0] = temp_ptr[0]; fixedToFloat_ptr[1] = temp_ptr[1]; fixedToFloat_ptr[2] = temp_ptr[2]; fixedToFloat_ptr[3] = temp_ptr[3];
	return fixedToFloat;
}

float FixedPoint::toFloat(int64_t value, int length, int numFracBits) {
	uint32_t signBit = 0;
	uint64_t _value = value;

	uint64_t intPart = GetIntPart(value, length, numFracBits);
	uint64_t fracPart = GetFracPart(value, numFracBits);
	// get the two's compliment if necessary
	if ((intPart >> (length - numFracBits - 1)) != 0) {
		signBit = 0x80000000;
		_value = (~_value) + (uint64_t)1;
		intPart = GetIntPart(_value, length, numFracBits);
		fracPart = GetFracPart(_value, numFracBits);
	}

	float intToFloat = (float)intPart;

	// get fractional component
	int power = -1;
	float fracToFloat = 0;
	uint64_t mask = 0x0000000000000001;
	mask = mask << (numFracBits - 1);
	for (int i = 0; i < numFracBits; i++, power--, mask = mask >> 1){
		uint64_t bit = mask & fracPart;
		if (bit != 0){
			fracToFloat += ((float)pow((float)2, power));
		}
	}
	float fixedToFloat = intToFloat + fracToFloat;

	// put sign bit after adding integer and fractional parts
	uint32_t temp;
	uint8_t *temp_ptr = reinterpret_cast<uint8_t*>(&temp);
	uint8_t *fixedToFloat_ptr = reinterpret_cast<uint8_t*>(&fixedToFloat);
	temp_ptr[0] = fixedToFloat_ptr[0]; temp_ptr[1] = fixedToFloat_ptr[1]; temp_ptr[2] = fixedToFloat_ptr[2]; temp_ptr[3] = fixedToFloat_ptr[3];
	temp = temp | (uint32_t)signBit;
	fixedToFloat_ptr[0] = temp_ptr[0]; fixedToFloat_ptr[1] = temp_ptr[1]; fixedToFloat_ptr[2] = temp_ptr[2]; fixedToFloat_ptr[3] = temp_ptr[3];
	return fixedToFloat;
}

FixedPoint FixedPoint::mult(FixedPoint operand0, FixedPoint operand1, int length, int numFracBits) {
	int64_t result = operand0.m_value * operand1.m_value;
    int result_numFracBits = operand0.m_numFracBits + operand1.m_numFracBits;
    result = result >> (result_numFracBits - numFracBits);
	int64_t result_tmp = result;

	if (operand0.GetValue() != 0) {
		// mask to get valid part of result_tmp
		uint64_t mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_WIDTH - length);
		result_tmp = result_tmp & mask;

		// sign extend if necessary  for result_tmp
		mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask << length;
		uint64_t result_sign_bit = (FixedPoint::GetIntPart(result_tmp, length, (operand0.m_numFracBits + operand1.m_numFracBits))) >> (length - (operand1.m_numFracBits + operand1.m_numFracBits) - 1);
		if (result_sign_bit != 0) {
			result_tmp = result_tmp | mask;
		}

		// use result_tmp to check for overflow	
		if (result_tmp / operand0.GetValue() != operand1.GetValue()) {
			cout << "OverFlow in Multiplication: operands are " << operand0 << " and " << operand1 << endl;
			exit(1);
		}
	}

	return FixedPoint(length, (operand0.m_numFracBits + operand1.m_numFracBits), result);
}


FixedPoint FixedPoint::div(FixedPoint operand0, FixedPoint operand1, int length, int numFracBits) {
	int64_t result = operand0.m_value / operand1.m_value;
	int64_t result_tmp = result;

	// mask to get valid part of result_tmp
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - length);
	result_tmp = result_tmp & mask;

	// sign extend if necessary for result_tmp
	mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask << length;
	uint64_t result_sign_bit = (operand0.GetIntPart(result, length, operand0.m_numFracBits)) >> (length - operand0.m_numFracBits - 1);
	if (result_sign_bit != 0) {
		result_tmp = result_tmp | mask;
	}

	// use result_tmp to check for overflow
	if (result_tmp * operand0.GetValue() != operand1.GetValue()) {
		cout << "OverFlow in Division: operands are " << operand0 << " and " << operand1 << endl;
		exit(1);
	}

	return FixedPoint(length, operand0.m_numFracBits, result);
}


FixedPoint operator+(FixedPoint &operand0, FixedPoint &operand1) {
	int64_t result = operand0.m_value + operand1.m_value;

	uint64_t result_sign_bit = (FixedPoint::GetIntPart(result, operand0.m_length, operand0.m_numFracBits)) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint64_t operand0_sign_bit = (operand0.GetIntPart()) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint64_t operand1_sign_bit = (operand1.GetIntPart()) >> (operand1.m_length - operand1.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && operand1_sign_bit == 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && operand1_sign_bit != 0 && result_sign_bit == 0)) {
		cout << "OverFlow in Addition: operands are " << operand0 << " and " << operand1 << endl;
		FILE *fd = fopen("errolog.txt", "w");
		fprintf(fd, "OverFlow in Addition: operands are %llu and %llu", operand0, operand1);
		fclose(fd);
		exit(1);
	}

	return FixedPoint(operand0.m_length, operand0.m_numFracBits, result);
}


FixedPoint operator-(FixedPoint &operand0, FixedPoint &operand1) {
	int64_t result = operand0.m_value - operand1.m_value;

	uint64_t result_sign_bit = (operand0.GetIntPart(result, operand0.m_length, operand0.m_numFracBits)) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint64_t operand0_sign_bit = (operand0.GetIntPart()) >> (operand0.m_length - operand0.m_numFracBits - 1);
	uint64_t operand1_sign_bit = (operand1.GetIntPart()) >> (operand1.m_length - operand1.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && operand1_sign_bit != 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && operand1_sign_bit == 0 && result_sign_bit == 0)) {
		cout << "OverFlow in subtraction: operands are " << operand0 << " and " << operand1 << endl;
		FILE *fd = fopen("errolog.txt", "w");
		fprintf(fd, "OverFlow in subtraction: operands are %llu and %llu", operand0, operand1);
		fclose(fd);
		exit(1);
	}

	return FixedPoint(operand0.m_length, operand0.m_numFracBits, result);
}


FixedPoint operator*(FixedPoint &operand0, FixedPoint &operand1) {
	int64_t result = operand0.m_value * operand1.m_value;
	int64_t result_tmp = result;

	if (operand0.GetValue() != 0) {
		// mask to get valid part of result_tmp
		uint64_t mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_WIDTH - (operand0.m_length + operand1.m_length));
		result_tmp = result_tmp & mask;

		// sign extend if necessary  for result_tmp
		mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask << (operand0.m_length + operand1.m_length);
		uint64_t result_sign_bit = (FixedPoint::GetIntPart(result_tmp, (operand0.m_length + operand1.m_length), (operand0.m_numFracBits + operand1.m_numFracBits))) >> ((operand0.m_length - operand1.m_numFracBits) + (operand0.m_length - operand1.m_numFracBits) - 1);
		if (result_sign_bit != 0) {
			result_tmp = result_tmp | mask;
		} 

		// use result_tmp to check for overflow	
		if (result_tmp / operand0.GetValue() != operand1.GetValue()) {
			cout << "OverFlow in Multiplication: operands are " << operand0 << " and " << operand1 << endl;
			FILE *fd = fopen("errolog.txt", "w");
			fprintf(fd, "OverFlow in Multiplication: operands are %llu and %llu", operand0, operand1);
			fclose(fd);
			exit(1);
		}
	}
	
	return FixedPoint((operand0.m_length + operand1.m_length), (operand0.m_numFracBits + operand1.m_numFracBits), result);
}


FixedPoint operator/(FixedPoint &operand0, FixedPoint &operand1) {
	int64_t result = operand0.m_value / operand1.m_value;
	int64_t result_tmp = result;

	// mask to get valid part of result_tmp
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - operand0.m_length);
	result_tmp = result_tmp & mask;

	// sign extend if necessary for result_tmp
	mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask << (operand0.m_length);
	uint64_t result_sign_bit = (operand0.GetIntPart(result, operand0.m_length, operand0.m_numFracBits)) >> (operand0.m_length - operand0.m_numFracBits - 1);
	if (result_sign_bit != 0){
		result_tmp = result_tmp | mask;
	}

	// use result_tmp to check for overflow
	if (result_tmp * operand0.GetValue() != operand1.GetValue()) {
		cout << "OverFlow in Division: operands are " << operand0 << " and " << operand1 << endl;
		FILE *fd = fopen("errolog.txt", "w");
		fprintf(fd, "OverFlow in Division: operands are %llu and %llu", operand0, operand1);
		fclose(fd);
		exit(1);
	}

	return FixedPoint(operand0.m_length, operand0.m_numFracBits, result);
}


FixedPoint& FixedPoint::operator+=(FixedPoint &rhs) {
    int64_t result = this->m_value + rhs.m_value;

	uint64_t result_sign_bit = (FixedPoint::GetIntPart(result, this->m_length, this->m_numFracBits)) >> (this->m_length - this->m_numFracBits - 1);
	uint64_t operand0_sign_bit = (this->GetIntPart()) >> (this->m_length - this->m_numFracBits - 1);
	uint64_t rhs_sign_bit = (rhs.GetIntPart()) >> (rhs.m_length - rhs.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && rhs_sign_bit == 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && rhs_sign_bit != 0 && result_sign_bit == 0)) {
		cout << "OverFlow in Addition: operands are " << (*this) << " and " << rhs << endl;
		FILE *fd = fopen("errolog.txt", "w");
		fprintf(fd, "OverFlow in Addition: operands are %llu and %llu", (*this), rhs);
		fclose(fd);
		exit(1);
	}

	return *this;
}


FixedPoint& FixedPoint::operator-=(FixedPoint &rhs) {
	int64_t result = this->m_value - rhs.m_value;

	uint64_t result_sign_bit = (this->GetIntPart(result, this->m_length, this->m_numFracBits)) >> (this->m_length - this->m_numFracBits - 1);
	uint64_t operand0_sign_bit = (this->GetIntPart()) >> (this->m_length - this->m_numFracBits - 1);
	uint64_t rhs_sign_bit = (rhs.GetIntPart()) >> (rhs.m_length - rhs.m_numFracBits - 1);
	if ((operand0_sign_bit == 0 && rhs_sign_bit != 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && rhs_sign_bit == 0 && result_sign_bit == 0)) {
		cout << "OverFlow in subtraction: operands are " << (*this) << " and " << rhs << endl;
		FILE *fd = fopen("errolog.txt", "w");
		fprintf(fd, "OverFlow in subtraction: operands are %llu and %llu", (*this), rhs);
		fclose(fd);
		exit(1);
	}

	return *this;
}


FixedPoint& FixedPoint::operator*=(FixedPoint &rhs) {
	int64_t result = this->m_value * rhs.m_value;
	int64_t result_tmp = result;

	if (this->GetValue() != 0) {
		// mask to get valid part of result_tmp
		uint64_t mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask >> (MAX_FIXED_POINT_WIDTH - (this->m_length + rhs.m_length));
		result_tmp = result_tmp & mask;

		// sign extend if necessary  for result_tmp
		mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask << (this->m_length + rhs.m_length);
		uint64_t result_sign_bit = (FixedPoint::GetIntPart(result_tmp, (this->m_length + rhs.m_length), (this->m_numFracBits + rhs.m_numFracBits))) >> ((this->m_length - rhs.m_numFracBits) + (this->m_length - rhs.m_numFracBits) - 1);
		if (result_sign_bit != 0) {
			result_tmp = result_tmp | mask;
		} 

		// use result_tmp to check for overflow	
		if (result_tmp / this->GetValue() != rhs.GetValue()) {
			cout << "OverFlow in Multiplication: operands are " << (*this) << " and " << rhs << endl;
			FILE *fd = fopen("errolog.txt", "w");
			fprintf(fd, "OverFlow in Multiplication: operands are %llu and %llu", (*this), rhs);
			fclose(fd);
			exit(1);
		}
	}
	
	return *this;   
}


FixedPoint& FixedPoint::operator/=(FixedPoint &rhs) {
	int64_t result = this->m_value / rhs.m_value;
	int64_t result_tmp = result;

	// mask to get valid part of result_tmp
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - this->m_length);
	result_tmp = result_tmp & mask;

	// sign extend if necessary for result_tmp
	mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask << (this->m_length);
	uint64_t result_sign_bit = (this->GetIntPart(result, this->m_length, this->m_numFracBits)) >> (this->m_length - this->m_numFracBits - 1);
	if (result_sign_bit != 0){
		result_tmp = result_tmp | mask;
	}

	// use result_tmp to check for overflow
	if (result_tmp * this->GetValue() != rhs.GetValue()) {
		cout << "OverFlow in Division: operands are " << (*this) << " and " << rhs << endl;
		FILE *fd = fopen("errolog.txt", "w");
		fprintf(fd, "OverFlow in Division: operands are %llu and %llu", (*this), rhs);
		fclose(fd);
		exit(1);
	}

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