#ifndef DERPKIT_CSS_COLORS_HPP
#define DERPKIT_CSS_COLORS_HPP

#include <map>
#include <string>

namespace css {

struct color_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

extern std::map<std::string,color_t> webcolors;

}

#endif
