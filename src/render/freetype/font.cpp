#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "render/freetype/freetype.hpp"
#include "render/font.hpp"

namespace derpkit { namespace render {

FontDefinition::FontDefinition(Font font)
	: data(font) {

}

FontDefinition::~FontDefinition(){
	//FT_Face face = static_cast<FT_Face>(data);
	//FT_Done_Face(face);
}

FontDefinition FontDefinition::default_font(){
	return FontDefinition::create("sans-serif", 12);
}

FontDefinition FontDefinition::create(const char* family, signed long size){
	const auto filename = FontLookup::filename_from_family(family);
	if ( !filename ){
		fprintf(stderr, "Failed to load font %s\n", family);
		return FontDefinition(nullptr);
	}

	FT_Face face;
	FT_Error error = FT_New_Face(library, filename, 0, &face);
	if ( error != FT_Err_Ok ){
		fprintf(stderr, "failed to create face for %s\n", family);
		return FontDefinition(nullptr);
	}

	FT_Set_Char_Size(face, size * 64, 0, 100, 0);
	return FontDefinition(face);
}

}}
