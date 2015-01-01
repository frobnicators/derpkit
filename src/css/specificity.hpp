#ifndef DERPKIT_CSS_SPECIFICITY
#define DERPKIT_CSS_SPECIFICITY

#include <cstdint>
#include <cstring>

namespace css {

class Specificity {
private:
	uint8_t important;
	uint8_t a;
	uint8_t b;
	uint8_t c;
public:
	Specificity(uint8_t important, uint8_t a, uint8_t b, uint8_t c)
	: important(important)
	, a(a)
	, b(b)
	, c(c)
	{ }

	Specificity(const Specificity& other) {
		memcpy(&important, &other.important, 4);
	}
	Specificity& operator=(const Specificity& other) {
		memcpy(&important, &other.important, 4);
		return *this;
	}
	bool inline operator==(const Specificity& other) const {
		return memcmp(&important, &other.important, 4) == 0;
	}
	bool inline operator<(const Specificity& other) const {
		return memcmp(&important, &other.important, 4) < 0;
	}

};

}

#endif
