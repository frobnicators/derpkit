#ifndef DERPKIT_UTILS_UTILS_HPP
#define DERPKIT_UTILS_UTILS_HPP

#include <ctime>
#include <sys/types.h>

#define USDIVIDER 1000000

namespace utils {

/**
 * Read a monotonic clock with µs-precision.
 */
DERPKIT_EXPORT unsigned long utime();

DERPKIT_EXPORT unsigned long sec_to_us(float time);

/**
 * Sleep µs.
 */
DERPKIT_EXPORT void usleep(useconds_t wait);

}

#endif
