#ifndef DERPKIT_CSS_EXPRESSION_HPP
#define DERPKIT_CSS_EXPRESSION_HPP

#include <string>
#include <vector>

namespace css {

class DERPKIT_EXPORT Term {
	public:
		enum Operator {
			OP_NONE,
			OP_PLUS,
			OP_MINUS,
			OP_SOLIDUS
		};

		enum Type {
			TYPE_INVALID,
			TYPE_STRING,
			TYPE_NUMBER,
			TYPE_URL,
			TYPE_HEXCOLOR,
			TYPE_FUNCTION,
			/*...*/
		};

		Term() : op(OP_NONE), type(TYPE_INVALID) {};

		/** How to combine this with previous term */
		Operator op;
		Type type;
		std::string value;

		void print() const;

		const char* operator_as_string() const;
		const char* type_as_string() const;
};

class DERPKIT_EXPORT Expression {
	public:
		std::vector<Term> terms;

		std::string to_string() const;

		void print() const;

};

}

#endif
