#ifndef DERPKIT_RENDER_UTILS_HPP
#define DERPKIT_RENDER_UTILS_HPP

#include <derpkit/render/math.hpp>

namespace derpkit {
namespace render {

class DERPKIT_EXPORT Utils {
	public:
		static void inline draw_rect(float x, float y, float width, float height) {
			draw_rect(vec2(x, y), vec2(width, height));
		}

		static void draw_rect(const vec2& pos, const vec2& size);
		static void draw_rect(const box& box);

		static void check_for_errors(const char* context);
};

}
}

#endif
