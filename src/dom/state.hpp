#ifndef DERPKIT_DOM_STATE_HPP
#define DERPKIT_DOM_STATE_HPP

#include <cstdint>
#include <string>

namespace dom {

enum LengthUnit {
	UNIT_AUTO,
	UNIT_PX,
	UNIT_PERCENT,
};

enum Display {
	DISPLAY_NONE,
	DISPLAY_BLOCK,
	DISPLAY_INLINE,
	DISPLAY_INLINE_BLOCK,
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

struct State {
	Display display;
	Color color;
	Color background_color;
	Length width;
	Length height;
};

std::string display_to_string(const Display& src);
std::string length_to_string(const Length& src);
std::string color_to_string(const Color& src);

}

#endif /* DERPKIT_DOM_STATE_HPP */
