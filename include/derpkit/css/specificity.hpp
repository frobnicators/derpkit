#ifndef DERPKIT_CSS_SPECIFICITY
#define DERPKIT_CSS_SPECIFICITY

#include <cstdint>
#include <cstring>
#include <string>

namespace css {

class Specificity {
public:
	uint8_t important;
	uint8_t a;
	uint8_t b;
	uint8_t c;

	void reset() {
		memset(&important, 0, 4);
	}

	Specificity() { }

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

	std::string debug_string() const;
};

}

#endif
