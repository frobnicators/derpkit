#include "cssproperty.hpp"

CSSProperty::CSSProperty(const std::string &property) : property(property), important(false) {};

void CSSProperty::print() const {
	printf("%s:", property.c_str());
	for(const std::string & v : values) {
		printf(" %s", v.c_str());
	}
	if(important) {
		printf(" !important");
	}
	printf("\n");
}
