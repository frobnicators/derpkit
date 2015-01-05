#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ffs_compat.hpp"
#include <cstdlib>
#include <cerrno>
#include <cstring>

namespace Logging {
	Component add_component(std::string name) {
		return Component();
	}

	void fatal(Component component, const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(FATAL, fmt, ap);
		va_end(ap);

		abort();
	}

	void error(Component component, const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(ERROR, fmt, ap);
		va_end(ap);
	}

	void warning(Component component, const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(WARNING, fmt, ap);
		va_end(ap);
	}

	void info(Component component, const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(INFO, fmt, ap);
		va_end(ap);
	}

	void verbose(Component component, const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(VERBOSE, fmt, ap);
		va_end(ap);
	}
};
