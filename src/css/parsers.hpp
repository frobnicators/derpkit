#ifndef DERPKIT_CSS_PARSER_HPP
#define DERPKIT_CSS_PARSER_HPP

#include <derpkit/css/datatype.hpp>
#include <derpkit/css/expression.hpp>
#include <derpkit/css/state.hpp>

namespace css {
namespace parsers {

enum Inherit {
	INHERIT,
	NO_INHERIT,
};

typedef void (*parser_func)(void* ptr, const Expression* val);

struct property {
	const char* name;
	const char* initial;
	Inherit inherit;
	size_t offset;
	size_t size;
	parser_func parser;
};

void display(void* ptr, const Expression* val);
void length(void* ptr, const Expression* val);
void color(void* ptr, const Expression* val);

void number(float& out, const Term& val);



}

}

#endif
