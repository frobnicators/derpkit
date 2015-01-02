#ifndef DERPKIT_CSS_SELECTOR_HPP
#define DERPKIT_CSS_SELECTOR_HPP

#include "dom/node.hpp"
#include <string>
#include <vector>

namespace css {

enum SelectorType {
	TYPE_TAG = 0,
	TYPE_CLASS,
	TYPE_ID,
	TYPE_PSEUDO,
	TYPE_ANY,
	TYPE_UNKNOWN
};

class SelectorAtom {
	public:

		SelectorAtom(SelectorType type, const std::string &value);

		SelectorType type;
		std::string value;

		/** Debug method */
		const char * type_as_string() const;

};

class Selector {
	public:
		bool match(dom::Node node);

		void calculate_specificity();

		const Specificity& specificity() const {
			return m_specificity;
		}

		const std::vector<SelectorAtom>& atoms() const {
			return m_atoms;
		}

		void DERPKIT_EXPORT print() const;
	private:
		std::vector<SelectorAtom> m_atoms;
		Specificity m_specificity;

		friend class CSS;
};

}

#endif
