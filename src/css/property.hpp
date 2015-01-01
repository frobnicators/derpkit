#ifndef DERPKIT_CSS_PROPERTY_HPP
#define DERPKIT_CSS_PROPERTY_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace css {

class Property {
	public:
		Property(const std::string &property);

		void print() const;

		std::string property;
		std::string value;

		bool important;
};

}

#endif
