#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "font.hpp"

namespace derpkit { namespace render {

FontDefinition::FontDefinition(){

}

FontDefinition::~FontDefinition(){

}

FontDefinition FontDefinition::default_font(){
	return FontDefinition();
}

FontDefinition FontDefinition::manual(const char* filename, signed long size){
	return FontDefinition();
}

}}
