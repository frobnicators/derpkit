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

std::string Expression::to_string() const {
	std::stringstream ss;
	for(const auto& term : terms) {
		ss << term.operator_as_string() << term.value;
		if(term.type == Term::TYPE_FUNCTION) {
			ss << "(";
			if(term.function_args != nullptr) {
				auto it=term.function_args->expressions.begin();

				if(it != term.function_args->expressions.end()) {
					ss << (it++)->to_string();
				}
				while ( it != term.function_args->expressions.end()) {
					ss << ", ";
					ss << (it++)->to_string();
				}
				//TODO: Print selector
			}
			ss << ")";
		}
	}

	return str_trim(ss.str());
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

}
