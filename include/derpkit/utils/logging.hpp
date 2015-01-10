#ifndef DERPKIT_UTILS_LOGGING_HPP
#define DERPKIT_UTILS_LOGGING_HPP

#include <cstdio>
#include <cstdarg>

namespace Logging {
	enum Severity {
		FATAL,
		ERROR,
		WARNING,
		INFO,
		VERBOSE,
		DEBUG,
	};

	void __FORMAT__(printf, 2,3) message(Severity severity, const char* fmt, ...);
	void vmessage(Severity severity, const char* fmt, va_list ap);

#ifdef ENABLE_DEBUG
	// Never use fatal in release...
	void __FORMAT__(printf, 1,2) __NORETURN__ fatal(const char* fmt, ...);
#endif

	void __FORMAT__(printf, 1,2) error(const char* fmt, ...);
	void __FORMAT__(printf, 1,2) warning(const char* fmt, ...);
	void __FORMAT__(printf, 1,2) info(const char* fmt, ...);
	void __FORMAT__(printf, 1,2) verbose(const char* fmt, ...);
};

#endif
