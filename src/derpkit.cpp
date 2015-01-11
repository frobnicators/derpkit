#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/derpkit.hpp>
#include <derpkit/dom/inspector.hpp>
#include "render/text.hpp"

namespace derpkit {

void initialize(){
#ifdef ENABLE_DEBUG
	dom::Inspector::initialize();
#endif

	render::Text::initialize();
}

void cleanup(){
	render::Text::cleanup();

#ifdef ENABLE_DEBUG
	dom::Inspector::cleanup();
#endif
}

}
