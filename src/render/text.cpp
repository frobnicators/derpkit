#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "text.hpp"
#include <derpkit/render/utils.hpp>
#include <cstdarg>

namespace derpkit { namespace render {

TextHandle::TextHandle()
	: buffer_size(0)
	, buffer(nullptr)
	, dirty(true)
	, font(FontDefinition::default_font()){

}

TextHandle::~TextHandle(){
	free(buffer);
}


void TextHandle::set_text(const char* format, ...){
	char* tmp;
	va_list ap;
	va_start(ap, format);
	vasprintf(&tmp, format, ap);
	va_end(ap);
	this->text = tmp;
	free(tmp);
}

void TextHandle::set_text(std::string text){
	this->text = text;
}

void TextHandle::set_font(FontDefinition font){
	this->font = font;
}

void TextHandle::set_box(struct box box){
	this->box = box;
	resize_buffer();
}

void TextHandle::resize_buffer(){
	const size_t new_size = box.w * box.h;
	if ( new_size > buffer_size ){
		buffer = static_cast<unsigned char*>(realloc(buffer, new_size));
		buffer_size = new_size;
	}
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

void Text::blit(const TextHandle& text){
	const auto& box = text.box;
	text.texture.bind();
	Utils::draw_rect(box);
}

}}
