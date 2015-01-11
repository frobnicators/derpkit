#ifndef DERPKIT_RENDER_FONT_HPP
#define DERPKIT_RENDER_FONT_HPP

namespace derpkit { namespace render {

/* overridden by implementation */
typedef void* Font;

class DERPKIT_EXPORT FontDefinition {
public:
	~FontDefinition();

	static FontDefinition default_font();
	static FontDefinition create(const char* family, signed long size);

private:
	friend class Text;

	FontDefinition(Font data);
	Font data;
};

namespace FontLookup {

const char* filename_from_family(const char* family);

}

}}

#endif /* DERPKIT_RENDER_FONT_HPP */
