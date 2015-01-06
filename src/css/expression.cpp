#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>
#include <derpkit/css/expression.hpp>
#include <derpkit/css/function_args.hpp>
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
	if(type==TYPE_FUNCTION) {
		printf("(");
		if(function_args != nullptr) {
			for(const auto& expr : function_args->expressions) {
				printf("[");
				expr.print();
				printf("]");
			}
			if(!function_args->selector.units().empty()) {
				function_args->selector.print();
			}
		}
		printf(")");
	}
}

Term::Term(Term&& term) : op(OP_NONE), type(TYPE_INVALID), value(std::move(term.value)), function_args(term.function_args) {
	term.function_args = nullptr;
}

Term::Term(const Term& term) : op(term.op), type(term.type), value(term.value) {
	if(term.function_args != nullptr) {
		function_args = new FunctionArgs(*term.function_args);
	} else {
		function_args = nullptr;
	}
};

Term::~Term() {
	delete function_args;
}

std::ostream& operator<<(std::ostream& os, const Expression& expr) {
	bool first = true;
	for(const auto& term : expr.terms) {
		if(!first) os << term.operator_as_string();
		first = false;
		os << term.value;
		if(term.type == Term::TYPE_FUNCTION) {
			os << "(";
			if(term.function_args != nullptr) {
				str_join(os, term.function_args->expressions.begin(), term.function_args->expressions.end(), ", ");;
			}
			os << ")";
		}
	}

	return os;
}

}
