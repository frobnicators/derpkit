#include "cssselector.hpp"

CSSSelector::CSSSelector(Type type, const std::string &value) : type(type), value(value) { }

const char * CSSSelector::type_as_string() const {
	static const char * convert[] = {
		"TAG",
		"CLASS",
		"ID",
		"PSEUDO",
		"UNKNOWN"
	};

	return convert[type];
}
