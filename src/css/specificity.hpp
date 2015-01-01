#ifndef CSS_CSS_SPECIFICITY
#define CSS_CSS_SPECIFICITY

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

	Specificity(const CSSSpecificity& other) {
		memcpy(&important, other.imporant, 4);
	}
	Specificity& operator=(const CSSSpecificity& other) {
		memcpy(&important, other.imporant, 4);
	}
	bool inline operator==(const CSSSpecificity& other) const {
		return memcmp(&important, other.important, 4) == 0;
	}
	bool inline operator<(const CSSSpecificity& other) const {
		return memcmp(&important, other.important, 4) < 0;
	}

};

}

#endif
