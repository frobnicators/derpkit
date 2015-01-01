#ifndef DERPKIT_CSS_RULE_HPP
#define DERPKIT_CSS_RULE_HPP

#include <vector>
#include "selector.hpp"
#include "property.hpp"

namespace css {

class Rule {
	public:
		/**
		 * Debug print
		 */
		void print(int indent=0) const;

	private:
		std::vector<SelectorGroup> m_selectors;
		std::vector<Property> m_properties;

		friend class CSS;
};

}

#endif
