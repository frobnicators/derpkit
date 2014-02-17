#ifndef CSS_PROPERTY_HPP
#define CSS_PROPERTY_HPP

#include <string>
#include <vector>

class CSSProperty {
	public:
		CSSProperty(const std::string &property);

		const std::string property;
		std::vector<std::string> values;
};

#endif
