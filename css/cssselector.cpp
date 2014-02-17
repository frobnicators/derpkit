#include "cssselector.hpp"

CSSSelector::CSSSelector(Type type, const std::string &value) : type(type), value(value) {
	printf("[CSS] Created selector %s = %s\n", type_as_string(), value.c_str());
}

const char * CSSSelector::type_as_string() {
	static const char * convert[] = {
		"TAG",
		"CLASS",
		"ID",
		"PSEUDO",
		"UNKNOWN"
	};

	return convert[type];
}
