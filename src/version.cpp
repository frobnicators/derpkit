#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "derpkit/version.hpp"

namespace derpkit {
	const char* version_string(){
		return PACKAGE_NAME "-" VERSION;
	}

	version_t version(){
		version_t tmp = {VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO};
		return tmp;
	}
};
