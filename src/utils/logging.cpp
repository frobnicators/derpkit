#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/utils/logging.hpp>
#include <cstdlib>
#include <cerrno>
#include <cstring>

namespace Logging {
	static const char* severity_to_string(Severity severity);
	static char* linebuffer = nullptr;
	static Severity lineseverity;

	static void write(Severity severity, const char* message){
		fprintf(stderr, "[ %s ] %s\n", severity_to_string(severity), message);
	}

	static const char* severity_to_string(Severity severity){
		switch ( severity ){
		case FATAL:   return "!";
		case ERROR:   return "E";
		case WARNING: return "W";
		case INFO:    return "I";
		case VERBOSE: return "V";
		case DEBUG:   return "D";
		};
		abort();
	}

	void vmessage(Severity severity, const char* fmt, va_list ap){

		char* message = nullptr;
		if ( vasprintf(&message, fmt, ap) == -1 ){
			fprintf(stderr, "memory allocation failed\n");
			abort();
		}

		/* concatenate buffered string */
		if ( linebuffer ){
			char* tmp = message;
			if ( asprintf(&message, "%s%s", linebuffer, message) == -1 ){
				fprintf(stderr, "asprintf(..) failed: %s\n", strerror(errno));
				abort();
			}
			free(tmp);
			free(linebuffer);
			linebuffer = nullptr;
		}

		/* split content based on newline, so prefixes can be written to each line. */
		char* line = message;
		while ( line ){
			/* not using strtok as it removes drops multiple serial delimiters */
			char* end = strchr(line, '\n');
			if ( end ){
				*end = 0;
			} else {
				/* store remainder of string to buffer */
				if ( line[0] != 0 ){
					linebuffer = strdup(line);
					lineseverity = severity;
				}
				break;
			}

			write(severity, line);

			/* move to next line */
			line = end+1;
		}

		free(message);
	}

	void fatal(const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(FATAL, fmt, ap);
		va_end(ap);
#ifdef ENABLE_DEBUG
		abort();
#endif
	}

	void error(const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(ERROR, fmt, ap);
		va_end(ap);
	}

	void warning(const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(WARNING, fmt, ap);
		va_end(ap);
	}

	void info(const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(INFO, fmt, ap);
		va_end(ap);
	}

	void verbose(const char* fmt, ...){
		va_list ap;
		va_start(ap, fmt);
		vmessage(VERBOSE, fmt, ap);
		va_end(ap);
	}
};
