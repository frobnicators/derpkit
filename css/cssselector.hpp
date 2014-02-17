#ifndef CSS_SELECTOR_HPP
#define CSS_SELECTOR_HPP

#include <string>

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

		const Type type;
		const std::string value;

		//bool match(...);

		/** Debug method */
		const char * type_as_string();

};

#endif
