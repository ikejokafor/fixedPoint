#include "FixedPoint.h"

FixedPoint::FixedPoint() {
	m_length = 32;
	m_numFracBits = 16;
	m_value = (int64_t)0;
}

FixedPoint::FixedPoint(int length, int numFracBits) {
	m_length = length; 
	m_numFracBits = numFracBits;
	m_value = (int64_t)0;
}


FixedPoint::FixedPoint(int length, int numFracBits, int64_t value) {
	m_length = length;
	m_numFracBits = numFracBits;
	m_value = value;
}

FixedPoint::FixedPoint(int length, int numFracBits, int value) {
	m_length = length;
	m_numFracBits = numFracBits;
	m_value = ((int64_t)value << numFracBits);
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
	uint64_t retValue;
    // memcpy to avoid potential casting issues
	memcpy(&retValue, &m_value, sizeof(int64_t));
	return (retValue & mask);
}

uint64_t FixedPoint::GetFracPart(int64_t value, int numFracBits) {
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - numFracBits);
	uint64_t retValue;
	memcpy(&retValue, &value, sizeof(int64_t));
	return (retValue & mask);
}

uint64_t FixedPoint::GetIntPart() {
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	uint64_t retValue;
    // memcpy to avoid potential casting issues
	memcpy(&retValue, &m_value, sizeof(int64_t));
	mask = mask >> (MAX_FIXED_POINT_WIDTH - m_length);
	retValue = retValue & mask;
	return (retValue >> m_numFracBits);
}

uint64_t FixedPoint::GetIntPart(int64_t value, int length, int numFracBits) {
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	uint64_t retValue;
    // memcpy to avoid potential casting issues
	memcpy(&retValue, &value, sizeof(int64_t));
	mask = mask >> (MAX_FIXED_POINT_WIDTH - length);
	retValue = retValue & mask;
	return (retValue >> numFracBits);
}

int64_t FixedPoint::GetValue() {
	return m_value;
}

void FixedPoint::SetValue(float value) {
	m_value = (int64_t)ceil((float)(value * (2, m_numFracBits)));
}

void FixedPoint::SetValue(int64_t value) {
	m_value = value;
}

void FixedPoint::SetValue(uint64_t value) {
	// memcpy to avoid potential casting issues
	memcpy(&m_value, &value, sizeof(uint64_t));
}

void FixedPoint::SetValue(int value) {
	m_value = ((int64_t)value << numFracBits);
}

void FixedPoint::SetParam(int length, int numFracBits) {
	uint64_t fracPart = m_GetFracPart();
	uint64_t intPart = m_GetIntPart();
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
	}
	else if (currentNumIntBits < newNumIntBits && sign_bit != 0) {
		// sign extend if necessary
		uint64_t mask = 0xFFFFFFFFFFFFFFFF;
		mask = mask << (currentNumIntBits);
		intPart = intPart | mask;
	}
	m_length = length;

	uint64_t value_tmp = ((intPart << numFracBits) | fracPart);
    // memcpy to avoid potential casting issues
	memcpy(&m_value, &value_tmp, sizeof(uint64_t));
}

float FixedPoint::toFloat(){
	int64_t value = GetValue();
	uint64_t intPart = GetIntPart(value, m_length, m_numFracBits);
	
	// get the two's compliment if necessary
	bool negative = false;
	if ((intPart >> (m_length - m_numFracBits - 1)) != 0) {
		negative = true;
		value = (~value) + (int64_t)1;
	}
	intPart = GetIntPart(value, m_length, m_numFracBits);
	uint64_t fracPart = GetFracPart(value, m_numFracBits);
	float intToFloat = (float)intPart;

	// get fractional component
	int power = -1;
	float fracToFloat = 0;
	uint64_t mask = 0x0000000000000001;
	mask = mask << (m_numFracBits - 1);
	for (int i = 0; i < numFracBits; i++, power--, mask = mask >> 1){
		uint64_t bit = mask & fracPart;
		if (bit!=0){
			fracToFloat += ((float)pow((float)2, power));
		}
	}
	float fixedToFloat = intToFloat + fracToFloat;

	// put sign bit after adding integer and fractional parts
	if (negative) {
		uint32_t temp;
        // memcpy to avoid potential casting issues
		memcpy(&temp, &fixedToFloat, sizeof(float));
		temp = temp | (uint32_t)0x80000000;
		memcpy(&fixedToFloat, &temp, sizeof(float));
	}

	return fixedToFloat;
}

float FixedPoint::toFloat(int64_t value, int length, int numFracBits) {
	uint64_t intPart = GetIntPart(value, length, numFracBits);

	// get the two's compliment if necessary
	bool negative = false;
	if ((intPart >> (length - numFracBits - 1)) != 0) {
		negative = true;
		value = (~value) + (int64_t)1;
	}
	intPart = GetIntPart(value, length, numFracBits);
	uint64_t fracPart = GetFracPart(value, numFracBits);
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
	if (negative) {
		uint32_t temp;
        // memcpy to avoid potential casting issues
		memcpy(&temp, &fixedToFloat, sizeof(float));
		temp = temp | (uint32_t)0x80000000;
		memcpy(&fixedToFloat, &temp, sizeof(float));
	}

	return fixedToFloat;
}

FixedPoint operator+(FixedPoint &operand0, FixedPoint &operand1) {
	int64_t result = operand0.value + operand1.value;

	uint64_t result_sign_bit = (operand0.GetIntPart(result, operand0.length, operand0.numFracBits)) >> (operand0.length - operand0.numFracBits- 1);
	uint64_t operand0_sign_bit = (operand0.GetIntPart()) >> (operand0.length - operand0.numFracBits - 1);
	uint64_t operand1_sign_bit = (operand1.GetIntPart()) >> (operand1.length - operand1.numFracBits - 1);
	if ((operand0_sign_bit == 0 && operand1_sign_bit == 0 && result_sign_bit > 0)
		|| (operand0_sign_bit > 0 && operand1_sign_bit > 0 && result_sign_bit == 0)) {
		cout << "OverFlow in Addition: operands are " << operand0 << " and " << operand1 << endl;
		exit(1);
	}

	return FixedPoint(operand0.length, operand0.numFracBits, result);
}

FixedPoint operator-(FixedPoint &operand0, FixedPoint &operand1) {
	int64_t result = operand0.value - operand1.value;
	int64_t result_tmp = result;

	uint64_t result_sign_bit = (operand0.GetIntPart(result, operand0.length, operand0.numFracBits)) >> (operand0.length - operand0.numFracBits - 1);
	uint64_t operand0_sign_bit = (operand0.GetIntPart()) >> (operand0.length - operand0.numFracBits - 1);
	uint64_t operand1_sign_bit = (operand1.GetIntPart()) >> (operand1.length - operand1.numFracBits - 1);
	if ((operand0_sign_bit == 0 && operand1_sign_bit != 0 && result_sign_bit != 0)
		|| (operand0_sign_bit != 0 && operand1_sign_bit == 0 && result_sign_bit == 0)) {
		cout << "OverFlow in subtraction: operands are " << operand0 << " and " << operand1 << endl;
		exit(1);
	}

	return FixedPoint(operand0.length, operand0.numFracBits, result);
}

FixedPoint operator*(FixedPoint &operand0, FixedPoint &operand1) {
	int64_t result = operand0.value * operand1.value;
	int64_t result_tmp = result;

	// mask to get valid part
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - (operand0.length + operand1.length));
	result_tmp = result_tmp & mask;

	// sign extend if necessary
	mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask << (operand0.length + operand1.length);
	uint64_t result_sign_bit = (operand0.GetIntPart(result, (operand0.length + operand1.length), (operand0.numFracBits + operand1.numFracBits))) >> ((operand0.length - operand1.numFracBits) + (operand0.length - operand1.numFracBits) - 1);
	if (result_sign_bit != 0){
		result_tmp = result_tmp | mask;
	} 

	if (operand0.GetValue() != 0 && result_tmp / operand0.GetValue() != operand1.GetValue()) {
		cout << "OverFlow in Multiplication: operands are " << operand0 << " and " << operand1 << endl;
		exit(1);
	}
	
	return FixedPoint((operand0.length + operand1.length), (operand0.numFracBits + operand1.numFracBits), result);
}

FixedPoint operator/(FixedPoint &operand0, FixedPoint &operand1) {
	int64_t result = operand0.value / operand1.value;
	int64_t result_tmp = result;

	//mask to get valid part
	uint64_t mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask >> (MAX_FIXED_POINT_WIDTH - operand0.length);
	result_tmp = result_tmp & mask;

	//sign extend if necessary
	mask = 0xFFFFFFFFFFFFFFFF;
	mask = mask << (operand0.length);
	uint64_t result_sign_bit = (operand0.GetIntPart(result, operand0.length, operand0.numFracBits)) >> (operand0.length - operand0.numFracBits - 1);
	if (result_sign_bit != 0){
		result_tmp = result_tmp | mask;
	}

	if (result_tmp * operand0.GetValue() != operand1.GetValue()) {
		cout << "OverFlow in Division: operands are " << operand0 << " and " << operand1 << endl;
		exit(1);
	}

	return FixedPoint(operand0.length, operand0.numFracBits, result);
}

FixedPoint& FixedPoint::operator=(const FixedPoint &obj) {
	m_value = obj.value;
	m_length = obj.length;
	m_numFracBits = obj.numFracBits;
	return *this;
}

bool operator<(const FixedPoint &operand0, const FixedPoint &operand1) {
	if (operand0.value < operand1.value) {
		return true;
	} else {
		return false;
	}
}

bool operator<=(const FixedPoint &operand0, const FixedPoint &operand1) {
	if (operand0.value <= operand1.value) {
		return true;
	} else {
		return false;
	}
}

bool operator>(const FixedPoint &operand0, const FixedPoint &operand1) {
	if (operand0.value > operand1.value) {
		return true;
	} else {
		return false;
	}
}

bool operator>=(const FixedPoint &operand0, const FixedPoint &operand1) {
	if (operand0.value >= operand1.value) {
		return true;
	} else {
		return false;
	}
}

bool operator==(const FixedPoint &operand0, const FixedPoint &operand1) {
	if (operand0.value == operand1.value) {
		return true;
	} else {
		return false;
	}
}

bool operator!=(const FixedPoint &operand0, const FixedPoint &operand1) {
	if (operand0.value != operand1.value) {
		return true;
	} else {
		return false;
	}
}

ostream& operator<<(ostream& o, FixedPoint value) {
	o << value.GetValue();
	return o;
}