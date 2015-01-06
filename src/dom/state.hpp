#ifndef DERPKIT_DOM_STATE_HPP
#define DERPKIT_DOM_STATE_HPP

#include <derpkit/css/datatype.hpp>

namespace dom {

struct State {
	css::Display display;
	css::Color color;
	css::Color background_color;
	css::Length width;
	css::Position position;
	css::Length height;
};

}

#endif /* DERPKIT_DOM_STATE_HPP */
