#include "property.hpp"

namespace css {

Property::Property(const std::string &property) : property(property), important(false) {};

void Property::print() const {
	printf("%s:", property.c_str());
	for(const std::string & v : values) {
		printf(" %s", v.c_str());
	}
	if(important) {
		printf(" !important");
	}
	printf("\n");
}

}
