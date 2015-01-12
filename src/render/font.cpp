#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "font.hpp"

namespace derpkit { namespace render {

const char* FontLookup::filename_from_family(const char* family){
	/** @todo replace with real implementation */
	return "/usr/share/fonts/truetype/msttcorefonts/arial.ttf";
}

}}
