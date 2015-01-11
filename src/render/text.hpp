#ifndef DERPKIT_RENDER_TEXT_HPP
#define DERPKIT_RENDER_TEXT_HPP

#include "font.hpp"
#include <derpkit/render/math.hpp>
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

private:
	friend class Text;

	bool dirty;
	std::string text;
	struct box box;
	FontDefinition font;
};

class DERPKIT_EXPORT Text {
 public:
	static void init();
	static void cleanup();

	static void draw(const TextHandle& text);
	static void blit(const TextHandle& text);
};

}}

#endif /* DERPKIT_RENDER_TEXT_HPP */
