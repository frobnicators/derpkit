#ifndef DERPKIT_CSS_FUNCTION_ARGS_HPP
#define DERPKIT_CSS_FUNCTION_ARGS_HPP

#include <derpkit/css/selector.hpp>
#include <derpkit/css/expression.hpp>

namespace css {

struct FunctionArgs {
	Selector selector;
	std::vector<Expression> expressions;
};

}

#endif
