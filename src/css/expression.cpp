#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>
#include <derpkit/css/expression.hpp>
#include <derpkit/utils/string_utils.hpp>

namespace css {

void Expression::print() const {
	for(const auto& term : terms) {
		term.print();
	}
}

const char * Term::operator_as_string() const {
	static const char * convert[] = {
		" ",
		"+",
		"-",
	};

	return convert[op];
}

const char * Term::type_as_string() const {
	static const char * convert[] = {
		"invalid",
		"string",
		"number",
		"url",
		"hexcolor",
		"function",
	};

	return convert[type];
}

void Term::print() const {
	printf("<%s> (%s) %s", operator_as_string(), type_as_string(), value.c_str());
}

std::string Expression::to_string() const {
	std::stringstream ss;
	for(const auto& term : terms) {
		ss << term.operator_as_string() << term.value;
	}

	return str_trim(ss.str());
}

}
