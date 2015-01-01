#ifndef DERPKIT_CSS_SELECTOR_HPP
#define DERPKIT_CSS_SELECTOR_HPP

#include <string>
#include <vector>

namespace css {

class Selector {
	public:
		enum Type {
			TAG = 0,
			CLASS,
			ID,
			PSEUDO,
			UNKNOWN
		};

		Selector(Type type, const std::string &value);

		Type type;
		std::string value;

		//bool match(...);

		/** Debug method */
		const char * type_as_string() const;

};

typedef std::vector<Selector> SelectorGroup;

}

#endif
