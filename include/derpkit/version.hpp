#ifndef DERPKIT_VERSION_HPP
#define DERPKIT_VERSION_HPP

#include <derpkit/export.hpp>

namespace derpkit {
	typedef struct {
		int major;
		int minor;
		int micro;
	} version_t;

	DERPKIT_EXPORT const char* version_string();
	DERPKIT_EXPORT version_t version();
};

#endif /* DERPKIT_VERSION_HPP */
