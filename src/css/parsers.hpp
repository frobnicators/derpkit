#ifndef DERPKIT_CSS_PARSER_HPP
#define DERPKIT_CSS_PARSER_HPP

#include "dom/state.hpp"
#include <derpkit/css/expression.hpp>

namespace css {
namespace parsers {

void display(dom::Display& out, const Expression* val);
void length(dom::Length& out, const Expression* val);
void color(dom::Color& out, const Expression* val);

void number(float& out, const Term& val);

}

}

#endif
