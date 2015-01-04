#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "specificity.hpp"

#include <sstream>

namespace css {

std::string Specificity::debug_string() const {
	std::stringstream ss;
	ss.setf(std::ios::hex, std::ios::basefield);
	ss << (int)important << (int)a << (int)b << (int)c;

	return ss.str();
}

}
