#ifndef CSS_SELECTOR_HPP
#define CSS_SELECTOR_HPP

#include <string>
#include <vector>

class CSSSelector {
	public:
		enum Type {
			TAG = 0,
			CLASS,
			ID,
			PSEUDO,
			UNKNOWN
		};

		CSSSelector(Type type, const std::string &value);

		Type type;
		std::string value;

		//bool match(...);

		/** Debug method */
		const char * type_as_string() const;

};

typedef std::vector<CSSSelector> CSSSelectorGroup;

#endif
