#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/texture.hpp>

#include "impl.hpp"

namespace derpkit {
namespace render {

Texture::Texture(const std::string& path) : m_unit(-1) {
	m_impl = impl::load_texture(path);
}

Texture::~Texture() {
	impl::free_texture(m_impl);
}

void Texture::bind(int unit) {
	impl::bind_texture(m_impl, unit);
	m_unit = unit;
}
void Texture::unbind() {
	if(m_unit < 0) return;
	impl::unbind_texture(m_unit);
}

}
}
