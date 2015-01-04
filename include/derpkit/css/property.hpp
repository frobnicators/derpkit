#ifndef DERPKIT_CSS_PROPERTY_HPP
#define DERPKIT_CSS_PROPERTY_HPP

#include <string>
#include <vector>
#include <cstdint>

#include <derpkit/css/expression.hpp>

namespace css {

class Property {
	public:
		Property(const std::string &property);

		void print() const;

		std::string property;
		std::vector<Expression> expressions;

		bool important;
};

}

#endif
