#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/rendertarget.hpp>
#include <derpkit/render/math.hpp>
#include "impl.hpp"

namespace derpkit {
namespace render {

const RenderTarget* RenderTarget::s_current = nullptr;

RenderTarget::RenderTarget(int width, int height) : m_resolution(width, height) {
	m_impl = impl::create_rendertarget(m_resolution);

	m_ortho = render::ortho(m_resolution);
}

RenderTarget::~RenderTarget() {
	impl::free_rendertarget(m_impl);
}

void RenderTarget::begin_frame() {
	impl::bind_rendertarget(m_impl);
	s_current = this;

	impl::clear();
}

void RenderTarget::end_frame() {
	impl::unbind_rendertarget();
	s_current = nullptr;
}

}
}
