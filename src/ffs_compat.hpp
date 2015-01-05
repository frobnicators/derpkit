#ifndef DERPKIT_FFS_COMPAT_HPP
#define DERPKIT_FFS_COMPAT_HPP

#include <derpkit/utils/logging.hpp>
#include <string>

namespace Logging {

	struct Component {
	};

	Component add_component(std::string name);

	void __FORMAT__(printf, 2,3) __NORETURN__ fatal(Component component, const char* fmt, ...);

	void __FORMAT__(printf, 2,3) error(Component component, const char* fmt, ...);
	void __FORMAT__(printf, 2,3) warning(Component component, const char* fmt, ...);
	void __FORMAT__(printf, 2,3) info(Component component, const char* fmt, ...);
	void __FORMAT__(printf, 2,3) verbose(Component component, const char* fmt, ...);
};

#define FFS_ASSERT_FATAL(condition, ...) if(!(condition)) Logging::fatal(__VA_ARGS__)

//#define FFS_ASSERT_FATAL(condition, ...) {}

#endif
