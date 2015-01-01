#include "selector.hpp"

namespace css {

SelectorAtom::SelectorAtom(SelectorType type, const std::string &value) : type(type), value(value) { }

const char * SelectorAtom::type_as_string() const {
	static const char * convert[] = {
		"TAG",
		"CLASS",
		"ID",
		"PSEUDO",
		"ANY",
		"UNKNOWN"
	};

	return convert[type];
}

void Selector::print() const {
	for(const auto & atom : m_atoms) {
			printf(" %s=%s (%s)", atom.type_as_string(), atom.value.c_str(), m_specificity.debug_string().c_str());
	}
}

void Selector::calculate_specificity() {
	m_specificity.reset();

	for(const auto& atom : m_atoms) {
		switch(atom.type) {
			case TYPE_ID:
				++m_specificity.a;
				break;
			case TYPE_CLASS:
			case TYPE_PSEUDO:
			// case TYPE_ATTRIBUTE:
				++m_specificity.b;
				break;
			case TYPE_TAG:
				++m_specificity.c;
				break;
			case TYPE_ANY:
			case TYPE_UNKNOWN:
				// Not counted
				break;
		}
	}
}

}
