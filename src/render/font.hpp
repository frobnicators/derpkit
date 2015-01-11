#ifndef DERPKIT_RENDER_FONT_HPP
#define DERPKIT_RENDER_FONT_HPP

namespace derpkit { namespace render {

class DERPKIT_EXPORT FontDefinition {
public:
	~FontDefinition();
	
	static FontDefinition default_font();
	static FontDefinition manual(const char* filename, signed long size);

private:
	FontDefinition();
};

}}

#endif /* DERPKIT_RENDER_FONT_HPP */
