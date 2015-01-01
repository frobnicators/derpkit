#include "selector.hpp"

namespace css {

Selector::Selector(Type type, const std::string &value) : type(type), value(value) { }

const char * Selector::type_as_string() const {
	static const char * convert[] = {
		"TAG",
		"CLASS",
		"ID",
		"PSEUDO",
		"UNKNOWN"
	};

	return convert[type];
}

}
