#ifndef DERPKIT_VERSION_HPP
#define DERPKIT_VERSION_HPP

namespace derpkit {
	typedef struct {
		int major;
		int minor;
		int micro;
	} version_t;

	const char* version_string();
	version_t version();
};

#endif /* DERPKIT_VERSION_HPP */
