#ifndef CSS_APPLY_HPP
#define CSS_APPLY_HPP

#include "css.hpp"

namespace css {

class Apply {
	public:
		DERPKIT_EXPORT static void apply_to_tree(const CSS* css /* DOMTree* tree*/);
};

}

#endif
