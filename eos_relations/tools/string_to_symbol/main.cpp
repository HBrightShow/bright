#include<iostream>


static constexpr uint64_t string_to_symbol(uint8_t precision, const char* str) {
	uint32_t len = 0;
	while (str[len]) ++len;

	uint64_t result = 0;
	for (uint32_t i = 0; i < len; ++i) {
		if (str[i] < 'A' || str[i] > 'Z') {
			/// ERRORS?
		}
		else {
			result |= (uint64_t(str[i]) << (8 * (1 + i)));
		}
	}

	result |= uint64_t(precision);
	return result;
}

#define S(P,X) string_to_symbol(P,#X)

int main() {

	uint64_t temp = S(4, RAMCORE);


	return 0;
}