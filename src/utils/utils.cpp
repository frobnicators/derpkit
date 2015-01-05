#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/utils/utils.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctype.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

namespace utils {

unsigned long utime(){
#ifdef HAVE_GETTIMEOFDAY
	struct timeval cur;
	gettimeofday(&cur, NULL);
	return (unsigned long)(cur.tv_sec * 1000000 + cur.tv_usec);
#elif defined(WIN32)
	static int initialized = 0;
	static int dst_warning = 0;
	static long long int divider = 0;
	static LARGE_INTEGER qpc_freq;

	/* verify timer precision */
	if ( !initialized ){
		QueryPerformanceFrequency(&qpc_freq);
		if ( qpc_freq.QuadPart < 1000000 ){
			// Don't use logging here, since I dont want yaml dependencies in my compiler thank you very much.
			std::cerr << "warning: gettimeofday() requires µs precision but is not available (have " << qpc_freq.QuadPart << " ticks per second)" << std::endl;
		}

		/* set divider to calculate µs */
		divider = qpc_freq.QuadPart / 1000000;
		initialized = 1;
	}

	/* time query */
	LARGE_INTEGER tmp;
	QueryPerformanceCounter(&tmp);
	return (unsigned long)(tmp.QuadPart / divider);
#else
#error util_utime() is not defined for this platform.
#endif
}

void usleep(useconds_t wait){
#ifdef HAVE_USLEEP
	::usleep(wait);
#elif defined(WIN32)
	Sleep(wait / 1000); /** @todo Sleep only has ms-precision (and bad such). */
#else
#error util_usleep() is not defined for this platform.
#endif
}

unsigned long sec_to_us(float time) {
	return static_cast<unsigned long>(USDIVIDER * time);
}

}

