#ifndef DERPKIT_CSS_SELECTOR_HPP
#define DERPKIT_CSS_SELECTOR_HPP

#include <string>
#include <vector>
#include <derpkit/dom/node.hpp>

namespace css {

enum SelectorType {
	TYPE_TAG = 0,
	TYPE_CLASS,
	TYPE_ID,
	TYPE_PSEUDO,
	TYPE_ANY,
	TYPE_UNKNOWN,
};

enum SelectorCombinator {
	COMBINATOR_NONE = 0, /* last unit in selector */
	COMBINATOR_DESCENDANT,
	COMBINATOR_CHILD,
	COMBINATOR_ADJACENT_SIBLING,
	COMBINATOR_GENERAL_SIBLING
};

class SelectorAtom {
	public:

		SelectorAtom(SelectorType type, const std::string &value);

		SelectorType type;
		std::string value;

		bool match(dom::Node) const;

		/** Debug method */
		const char * type_as_string() const;
};

class SelectorUnit {
	public:
		SelectorUnit() : combinator(COMBINATOR_NONE) {}

		bool match(dom::Node) const;

		const std::vector<SelectorAtom>& atoms() const {
			return m_atoms;
		}

		/**
		 * How to combinate this with the next unit */
		SelectorCombinator combinator;

		/** Debug method */
		const char * combinator_as_string() const;
	private:
		std::vector<SelectorAtom> m_atoms;
		friend class CSS;
};

class DERPKIT_EXPORT Selector {
	public:
		Selector() {};
		Selector(const char* css);

		void calculate_specificity();

		const Specificity& specificity() const {
			return m_specificity;
		}

		const std::vector<SelectorUnit>& units() const {
			return m_units;
		}

		void print() const;

		std::string to_string() const;
	private:
		std::vector<SelectorUnit> m_units;
		Specificity m_specificity;

		friend class CSS;
};

std::ostream& operator<<(std::ostream& os, const SelectorAtom&);
std::ostream& operator<<(std::ostream& os, const SelectorUnit&);
std::ostream& operator<<(std::ostream& os, const Selector&);

}

#endif
