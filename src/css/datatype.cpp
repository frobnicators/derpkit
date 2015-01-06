#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/css/datatype.hpp>
#include <sstream>

namespace css {

std::string display_to_string(const Display& src){
	switch ( src ){
	case DISPLAY_NONE: return "none";
	case DISPLAY_INLINE: return "inline";
	case DISPLAY_BLOCK: return "block";
	case DISPLAY_INLINE_BLOCK: return "inline-block";
	}
	return "unknown";
}

std::string length_to_string(const Length& src){
	std::stringstream ss;

	switch ( src.unit ){
	case UNIT_AUTO:
		ss << "auto";
		break;

	case UNIT_PX:
		ss << src.scalar;
		ss << "px";
		break;

	case UNIT_PERCENT:
		ss << src.scalar;
		ss << "%";
		break;
	}

	return ss.str();
}

std::string color_to_string(const Color& src);

}
