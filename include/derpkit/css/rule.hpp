#ifndef DERPKIT_CSS_RULE_HPP
#define DERPKIT_CSS_RULE_HPP

#include <derpkit/css/selector.hpp>
#include <derpkit/css/property.hpp>

#include <vector>

namespace css {

class Rule {
	public:
		Rule(const CSS* css) : m_css(css) {}
		/**
		 * Debug print
		 */
		void DERPKIT_EXPORT print(int indent=0) const;

		const std::vector<Selector>& selectors() const {
			return m_selectors;
		}

		const std::vector<Property>& properties() const {
			return m_properties;
		}

		const CSS* css() const {
			return m_css;
		}

	private:
		std::vector<Selector> m_selectors;
		std::vector<Property> m_properties;

		const CSS* m_css;

		friend class CSS;
};

}

#endif
