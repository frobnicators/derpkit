#ifndef DERPKIT_CSS_PARSER_HPP
#define DERPKIT_CSS_PARSER_HPP

#include "dom/state.hpp"

namespace css {
namespace parsers {

void display(dom::Display& out, const char* val);
void number(float& out, const char* val);
void length(dom::Length& out, const char* val);
void color(dom::Color& out, const char* val);

}

}

#endif
