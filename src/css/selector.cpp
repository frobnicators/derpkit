#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/css/selector.hpp>
#include <derpkit/css/css.hpp>
#include <sstream>

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

const char * SelectorUnit::combinator_as_string() const {
	static const char * convert[] = {
		"<none>",
		"_",
		">",
		"+",
		"~"
	};

	return convert[combinator];
}

Selector::Selector(const char* css) {
	CSS::from_source_to_selector(css, *this);
}

bool SelectorAtom::match(dom::Node node) const {
	switch ( type ) {
	case TYPE_ANY:
	case TYPE_UNKNOWN:
		return true;
	case TYPE_ID:
	{
		const char* id = node.get_attribute("id");
		return id ? id == value : false;
	}
	case TYPE_TAG:
		return node.tag_name() == value;
	case TYPE_CLASS:
	{
		for(const std::string& cls : node.classes()) {
			if(cls == value) return true;
		}
		return false;
	}
	// TODO: PSEUDO
	}
	return false;
}

bool SelectorUnit::match(dom::Node node) const {
	for(const auto& atom : m_atoms) {
		if(!atom.match(node)) return false;
	}
	return true;
}

void Selector::print() const {
	for(const auto & unit : m_units) {
		printf("[");
		for(const auto& atom : unit.atoms()) {
			printf("%s=%s ", atom.type_as_string(), atom.value.c_str());
		}
		printf("] {%s} ", unit.combinator_as_string());
	}
	printf("(%s)", m_specificity.debug_string().c_str());
}

void Selector::calculate_specificity() {
	m_specificity.reset();

	for(const auto& unit : m_units) {
		for(const auto& atom : unit.atoms()) {
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

std::ostream& operator<<(std::ostream& os, const SelectorAtom& atom) {
	switch(atom.type) {
		case TYPE_TAG:
			os << atom.value;
			break;
		case TYPE_CLASS:
			os << "." << atom.value;
			break;
		case TYPE_ID:
			os << "#" << atom.value;
			break;
		case TYPE_PSEUDO:
			os << ":" << atom.value;
			break;
		case TYPE_ANY:
			os << "*";
			break;
		default:
			break;
	}
	return os;
}

std::string Selector::to_string() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

std::ostream& operator<<(std::ostream& os, const SelectorUnit& unit) {
	for(const auto& atom : unit.atoms()) {
		os << atom;
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const Selector& selector) {
	for(const auto& unit : selector.units()) {
		os << unit;
		if(unit.combinator != COMBINATOR_NONE) {
			os << unit.combinator_as_string();
		}
	}
	return os;
}

}
