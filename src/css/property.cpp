#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/css/property.hpp>

namespace css {

Property::Property(const std::string &property) : property(property), important(false) {};

void Property::print() const {
	printf("%s: ", property.c_str());
	for(const auto& expr : expressions) {
		expr.print();
		printf(", ");
	}
	if(important) {
		printf(" !important");
	}
	printf("\n");
}

}
