#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "text.hpp"
#include <derpkit/render/utils.hpp>

namespace derpkit { namespace render {

TextHandle::TextHandle()
	: font(FontDefinition::default_font()){

	dirty = true;
}

TextHandle::~TextHandle(){

}


void TextHandle::set_text(const char* format, ...){
}

void TextHandle::set_text(std::string text){
	this->text = text;
}

void TextHandle::set_font(FontDefinition font){
	this->font = font;
}

void TextHandle::set_box(struct box box){
	this->box = box;
}

void TextHandle::update(struct box box, const char* text, FontDefinition font){
	set_box(box);
	set_text(text);
	set_font(font);
}

void TextHandle::update(struct box box, std::string text, FontDefinition font){
	set_box(box);
	set_text(text);
	set_font(font);
}

void Text::draw(const TextHandle& text){

}

void Text::blit(const TextHandle& text){
	const auto& box = text.box;
	Utils::draw_rect(box);
}

}}
