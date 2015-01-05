#ifndef FROB_CORE_UTILS_BASE64
#define FROB_CORE_UTILS_BASE64

#include <string>

/*
 * A base64 wrapper that's more fit for our needs
 */

namespace base64 {
	std::string encode(const char* data, size_t size);
	std::string decode(const std::string& str);
}

#endif
