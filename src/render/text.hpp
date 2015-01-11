#ifndef DERPKIT_RENDER_TEXT_HPP
#define DERPKIT_RENDER_TEXT_HPP

#include "font.hpp"
#include <derpkit/render/math.hpp>
#include <derpkit/render/texture.hpp>
#include <string>

namespace derpkit { namespace render {

class DERPKIT_EXPORT TextHandle {
public:
	TextHandle();
	~TextHandle();

	void set_text(const char* format, ...);
	void set_text(std::string text);
	void set_font(FontDefinition font);
	void set_box(struct box box);
	void update(struct box box, const char* text, FontDefinition font);
	void update(struct box box, std::string text, FontDefinition font);

	bool is_dirty() const { return dirty; }

protected:
	void resize_buffer();

private:
	friend class Text;

	size_t buffer_size;
	unsigned char* buffer;

	mutable bool dirty;
	std::string text;
	struct box box;
	FontDefinition font;
	mutable Texture texture;
};

class DERPKIT_EXPORT Text {
 public:
	static void initialize();
	static void cleanup();

	static void draw(const TextHandle& text);
	static void blit(const TextHandle& text);
};

}}

#endif /* DERPKIT_RENDER_TEXT_HPP */
