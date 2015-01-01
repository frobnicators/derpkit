#include "property.hpp"

namespace css {

Property::Property(const std::string &property) : property(property), important(false) {};

void Property::print() const {
	printf("%s: %s", property.c_str(), value.c_str());
	if(important) {
		printf(" !important");
	}
	printf("\n");
}

}
