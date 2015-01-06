#ifndef DERPKIT_CSS_STATE_HPP
#define DERPKIT_CSS_STATE_HPP

#include <derpkit/css/datatype.hpp>

namespace css {

struct State {
	Display display;
	Color color;
	Color background_color;
	Length width;
	Position position;
	Length height;
};

}

#endif /* DERPKIT_CSS_STATE_HPP */
