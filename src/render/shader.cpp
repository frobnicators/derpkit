#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/shader.hpp>
#include "impl.hpp"

#include <derpkit/utils/logging.hpp>

#ifdef ENABLE_DEBUG
#include <derpkit/utils/file.hpp>
#endif

namespace derpkit {
namespace render {

Shader* Shader::s_current = nullptr;

Shader::Shader(impl::Shader* shader) : m_impl(shader) { }

Shader::~Shader() {
	impl::free_shader(m_impl);
}

#ifdef ENABLE_DEBUG
Shader* Shader::from_file(const std::string& filename) {
	impl::Shader* shader = impl::create_shader_from_filename(filename);
	if(shader == nullptr) return nullptr;

	return new Shader(shader);
}
#endif

void Shader::bind() {
	impl::bind_shader(m_impl);
	s_current = this;
}

void Shader::unbind() {
	impl::unbind_shader();
	s_current = nullptr;
}

void Shader::set_projection(const mat3& m) {
	if(s_current == nullptr) Logging::fatal("No shader bound\n");
	impl::set_projection(s_current->m_impl, m);
}

void Shader::set_model_matrix(const mat3& m) {
	if(s_current == nullptr) Logging::fatal("No shader bound\n");
	impl::set_model_matrix(s_current->m_impl, m);
}

}
}
