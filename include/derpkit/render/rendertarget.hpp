#ifndef DERPKIT_RENDERTARGET_HPP
#define DERPKIT_RENDERTARGET_HPP

#include <derpkit/render/math.hpp>

namespace derpkit {
namespace render {

namespace impl {
	struct RenderTarget;
}

class DERPKIT_EXPORT RenderTarget {
	public:
		RenderTarget(int width, int height);
		~RenderTarget();

		void set_background_color(const vec4& color);

		void begin_frame();
		void end_frame();

		const mat3& ortho() const { return m_ortho; }
		const ivec2& resolution() const { return m_resolution; }

		static const RenderTarget* current();

	private:
		mat3 m_ortho;
		ivec2 m_resolution;
		vec4 m_background_color;

		impl::RenderTarget* m_impl;

		static const RenderTarget* s_current;
};

}
}

#endif
