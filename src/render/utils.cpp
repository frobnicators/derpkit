#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/utils.hpp>

#include "impl.hpp"

namespace derpkit {
namespace render {

void Utils::check_for_errors(const char* context) {
	impl::check_for_errors(context);
}


}
}
