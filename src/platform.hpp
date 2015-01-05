#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * Platform-specific macros.
 *
 * NORETURN:
 *   Declares that a function never returns (hints about compiler code-paths)
 * NORETURN_PTR:
 *   Same as NORETURN but for function pointers (some compilers doesn't support
 *   NORETURN on function pointers.)
 */

#ifdef __clang__
#       define __PURE__
#       define __CONST__
#       define __DEPRECATED__(msg) __attribute__ ((deprecated(msg)))
#       define __SENTINEL__ __attribute__ ((sentinel))
#       define __HOT__
#       define __COLD__
#       define __NONNULL__(...) __attribute__((nonnull (__VA_ARGS__)))
#       define __NORETURN__ __attribute__((noreturn))
#       define __NORETURN_PTR__
#       define __UNUSED__
#       define __WARN_UNUSED__
#       define __ALIGNED__(size) __attribute__((aligned(size)))
#       define __FORMAT__(type, index, first)
#       define __PATH_SEPARATOR__ '/'
#       define __HIDDEN__ __attribute__((visibility("hidden")))
#elif defined(__GNUC__)
#       define __PURE__ __attribute__((pure))
#       define __CONST__ __attribute__((const))
#       define __DEPRECATED__(msg) __attribute__ ((deprecated(msg)))
#       define __SENTINEL__ __attribute__ ((sentinel))
#       define __HOT__ __attribute__ ((hot))
#       define __COLD__ __attribute__ ((cold))
#       define __NONNULL__(...) __attribute__((nonnull (__VA_ARGS__)))
#       define __NORETURN__ __attribute__((noreturn))
#       define __NORETURN_PTR__ __attribute__((noreturn))
#       define __UNUSED__ __attribute__ ((unused))
#       define __WARN_UNUSED__ __attribute__((warn_unused_result))
#       define __ALIGNED__(size) __attribute__((aligned(size)))
#       define __FORMAT__(type, index, first) __attribute__((format(type, index, first)))
#       define __PATH_SEPARATOR__ '/'
#       define __HIDDEN__ __attribute__((visibility("hidden")))
#elif defined(_WIN32)
#       define __PURE__
#       define __CONST__
#       define __DEPRECATED__(msg) __declspec(deprecated(msg))
#       define __SENTINEL__
#       define __HOT__
#       define __COLD__
#       define __NONNULL__(...)
#       define __NORETURN__  __declspec(noreturn)
#       define __NORETURN_PTR__
#       define __UNUSED__
#       define __WARN_UNUSED__
#       define __ALIGNED__(size) __declspec(align(size))
#       define __FORMAT__(type, index, first)
#       define __PATH_SEPARATOR__ '\\'
#       define __HIDDEN__
#else
#       error Unknown compiler/platform
#endif

#endif /* PLATFORM_H */
