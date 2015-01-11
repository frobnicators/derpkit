#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "render/freetype/freetype.hpp"
#include "render/text.hpp"

namespace derpkit { namespace render {

FT_Library library;

void Text::initialize(){
	FT_Error error = FT_Init_FreeType(&library);
	if ( error != FT_Err_Ok ){
		fprintf(stderr, "failed to initialize freetype\n");
	}
}

void Text::cleanup(){
	FT_Done_FreeType(library);
}

}}
