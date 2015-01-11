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

Shader::Uniform Shader::get_uniform(const std::string& name) const {
	return Uniform(impl::get_uniform(m_impl, name));
}

Shader::Uniform::Uniform(impl::Uniform* uniform) : m_uniform(uniform) { }

Shader::Uniform::Uniform(const Uniform& u) {
	m_uniform = impl::copy_uniform(u.m_uniform);
}

Shader::Uniform::Uniform(Uniform&& u) : m_uniform(u.m_uniform) {
	u.m_uniform = nullptr;
}

Shader::Uniform::~Uniform() {
	if(m_uniform != nullptr) free_uniform(m_uniform);
}

void Shader::Uniform::set(const ivec2 &v) {
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const vec3 &v) {
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const css::Color &color) {
	impl::uniform_set(m_uniform, color);
}

void Shader::Uniform::set(const vec2 &v) {
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const mat3 &m) {
	impl::uniform_set(m_uniform, m);
}

void Shader::Uniform::set(float f) {
	impl::uniform_set(m_uniform, f);
}

void Shader::Uniform::set(int i) {
	impl::uniform_set(m_uniform, i);
}


}
}
