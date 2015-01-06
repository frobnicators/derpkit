#ifndef DERPKIT_CSS_DATATYPE_HPP
#define DERPKIT_CSS_DATATYPE_HPP

#include <cstdint>
#include <string>

namespace css {

enum Display {
	DISPLAY_NONE,
	DISPLAY_BLOCK,
	DISPLAY_INLINE,
	DISPLAY_INLINE_BLOCK,
};

enum LengthUnit {
	UNIT_AUTO,
	UNIT_PX,
	UNIT_PERCENT,
};

struct Length {
	float scalar;
	LengthUnit unit;
};

union Color {
	uint32_t val;
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
};

DERPKIT_EXPORT std::string display_to_string(const Display& src);
DERPKIT_EXPORT std::string length_to_string(const Length& src);
DERPKIT_EXPORT std::string color_to_string(const Color& src);

class Position {
public:
	enum Value {
		STATIC,
		RELATIVE,
		ABSOLUTE
	};

	Value value;

	static void parse(void* ptr, const Expression* val){ static_cast<Position*>(ptr)->parse(val); }
	bool parse(const Expression* val);

	std::string to_string() const;
};

}

#endif /* DERPKIT_CSS_DATATYPE_HPP */
