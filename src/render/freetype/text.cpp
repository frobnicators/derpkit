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

struct draw_context {
	unsigned char* buffer;
	size_t sanity;
	unsigned int w;
	unsigned int h;
};

/**
 * Draw a character on the bitmap.
 * @param ox Offset x
 * @param oy Offset y
 */
static void draw_char(struct draw_context& ctx, unsigned int ox, unsigned int oy, FT_Bitmap* bitmap){
	for ( unsigned int y = 0; y < bitmap->rows; y++ ){
		for ( unsigned int x = 0; x < bitmap->width; x++ ){
			const size_t p = y * bitmap->width + x;         /* index into font bitmap */
			const size_t q = (y + oy) * ctx.w + (x + ox);   /* index into output buffer */

			if ( q >= ctx.sanity ){
				break;
			}

			if ( x + ox > ctx.w || y + oy > ctx.h ){
				continue;
			}

			/** @todo blending */
			ctx.buffer[q] = bitmap->buffer[p];
		}
	}
}

void Text::draw(const TextHandle& text){
	const auto& box = text.box;
	const auto& font = text.font;
	const auto& str = text.text;
	auto& texture = text.texture;

	/* skip empty boxes */
	if ( box.w == 0 || box.y == 0 || !text.buffer ){
		return;
	}

	FT_Face face = static_cast<FT_Face>(font.data);
	FT_GlyphSlot slot = face->glyph;

	struct draw_context ctx;
	ctx.buffer = text.buffer;
	ctx.sanity = text.buffer_size;
	ctx.w = box.w;
	ctx.h = box.h;

	/* reset buffer */
	memset(text.buffer, 0, text.buffer_size);

	int x = 0;
	int y = 32; /** @todo line-height */
	for ( auto ch: str ){
		if ( FT_Load_Char(face, ch, FT_LOAD_RENDER) != FT_Err_Ok ){
			continue;
		}

		draw_char(ctx, x + slot->bitmap_left, y - slot->bitmap_top, &slot->bitmap);

		x += slot->advance.x >> 6;
	}

	texture.upload(ctx.buffer, ivec2(box.w, box.h));

	text.dirty = false;
}

}}
