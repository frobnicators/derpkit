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

Shader::Shader(impl::Shader* shader)
: m_impl(shader) {
	m_proj_mat = get_uniform("u_proj");
	m_model_mat = get_uniform("u_model");
}

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
	s_current->m_proj_mat.set(m);
}

void Shader::set_model_matrix(const mat3& m) {
	if(s_current == nullptr) Logging::fatal("No shader bound\n");
	s_current->m_model_mat.set(m);
}

Shader::Uniform Shader::get_uniform(const std::string& name) const {
	return Uniform(impl::get_uniform(m_impl, name));
}

Shader::Uniform::Uniform() : m_uniform(nullptr) { }
Shader::Uniform::Uniform(impl::Uniform* uniform) : m_uniform(uniform) { }

Shader::Uniform::Uniform(const Uniform& u) {
	m_uniform = impl::copy_uniform(u.m_uniform);
}

Shader::Uniform::Uniform(Uniform&& u) : m_uniform(u.m_uniform) {
	u.m_uniform = nullptr;
}

Shader::Uniform::~Uniform() {
	free_uniform(m_uniform);
}

Shader::Uniform& Shader::Uniform::operator=(const Uniform& u) {
	free_uniform(m_uniform);
	m_uniform = impl::copy_uniform(u.m_uniform);
	return *this;
}

Shader::Uniform& Shader::Uniform::operator=(Uniform&& u) {
	free_uniform(m_uniform);
	m_uniform = u.m_uniform;
	u.m_uniform = nullptr;
	return *this;
}

void Shader::Uniform::set(const ivec2 &v) {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const vec3 &v) {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const vec4& v) {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const vec2 &v) {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const mat3 &m) {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, m);
}

void Shader::Uniform::set(float f) {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, f);
}

void Shader::Uniform::set(int i) {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, i);
}


}
}
