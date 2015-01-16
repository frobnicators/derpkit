#ifndef DERPKIT_RENDER_DRAW_HPP
#define DERPKIT_RENDER_DRAW_HPP

#include <derpkit/render/math.hpp>
#include "gen/shaderdefs.hpp"

namespace derpkit {
namespace render {

class DERPKIT_EXPORT Draw {
	public:

		static void rect(const box& box, const vec4& color, ShaderId shader=Shader_color);
		static void rect(const box& box, const Texture& texture, const vec4& color=vec4(1.f), ShaderId shader=Shader_texture);

		static void rect(const box& box);

		static void push_transform(const Transform& transform);
		static void pop_transform();
	private:
		static mat3 m_transform;
};

}
}

#endif
