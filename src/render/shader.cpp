#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/shader.hpp>
#include <derpkit/render/rendertarget.hpp>
#include "impl.hpp"

#include <derpkit/utils/logging.hpp>

#ifdef ENABLE_DEBUG
#include <derpkit/utils/file.hpp>
#endif

#include <set>
#include <algorithm>

namespace derpkit {
namespace render {

Shader* Shader::s_current = nullptr;
Shader** Shader::s_shaders = nullptr;

Shader::Shader(impl::Shader* shader)
: m_impl(shader)
, m_last_rt(nullptr)
{ }

Shader::~Shader() {
	impl::free_shader(m_impl);
}

void Shader::initialize() {
	s_shaders = new Shader*[_Shader_Count];
	std::vector<impl::ShaderStage*> stages;
	for(unsigned int i=0; i<shaderdefs::_ShaderSource_Count; ++i) {
		const ShaderSourceDef& source = shaderdefs::stage_sources[i];
		impl::ShaderStage* stage = impl::create_shaderstage_from_source(
				source.stage,
				source.name,
				source.source);
		stages.push_back(stage);
	}

	for(unsigned int i=0; i<_Shader_Count; ++i) {
		const ShaderProgramDef& program = shaderdefs::programs[i];
		std::vector<impl::ShaderStage*> program_stages;

		bool valid = true;

		std::set<int> uniforms;
		std::set<SamplerDef> samplers;

		for(unsigned int s : program.stages) {
			if(stages[s] == nullptr) {
				Logging::error("A shader stage of shader %s was not compiled correctly.\n", program.name);
				valid = false;
			} else {
				program_stages.push_back(stages[s]);
				for(int u = 0; shaderdefs::stage_sources[s].uniforms[u] != -1; ++u) {
					uniforms.insert(shaderdefs::stage_sources[s].uniforms[u]);
				}

				for(int u = 0; shaderdefs::stage_sources[s].samplers[u].uniform_name != nullptr; ++u) {
					samplers.insert(shaderdefs::stage_sources[s].samplers[u]);
				}
			}
		}

		if(!valid) break;

		s_shaders[i] = new Shader(create_shader(program.name, program_stages));

		if(s_shaders[i]->valid()) {
			impl::bind_shader(s_shaders[i]->m_impl);
			for(int u : uniforms) {
				 s_shaders[i]->m_uniforms[static_cast<UniformId>(u)] = Uniform(impl::get_uniform(s_shaders[i]->m_impl, shaderdefs::uniform_names[u]));
			}
			for(const SamplerDef& sdef : samplers) {
				impl::Uniform* u = impl::get_uniform(s_shaders[i]->m_impl, sdef.uniform_name);
				impl::uniform_set(u, sdef.texture);
				impl::free_uniform(u);
			}

			s_shaders[i]->m_proj_mat = s_shaders[i]->get_uniform(Uniform_projectionMatrix);
			s_shaders[i]->m_model_mat = s_shaders[i]->get_uniform(Uniform_modelMatrix);
		}
	}

	std::for_each(stages.begin(), stages.end(), impl::free_shaderstage);
	impl::unbind_shader();
}

void Shader::cleanup() {
	for(unsigned int i=0; i<_Shader_Count; ++i) {
		delete s_shaders[i];
	}

	delete[] s_shaders;
}

const Shader* Shader::get(ShaderId shaderId) {
	if(shaderId < _Shader_Count) {
		return s_shaders[shaderId];
	} else {
		Logging::fatal("Invalid shaderId %d (out of range)\n", shaderId);
		return nullptr;
	}
}

void Shader::bind() const {
	if(valid()) {
		if(s_current != this) {
			impl::bind_shader(m_impl);
		}
		if(m_last_rt != RenderTarget::current()) {
			m_last_rt = RenderTarget::current();
			if(m_last_rt != nullptr) {
				m_proj_mat.set(m_last_rt->ortho());
			}
		}
	} else {
		impl::unbind_shader();
	}
	s_current = const_cast<Shader*>(this);
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

const Shader::Uniform& Shader::get_uniform(UniformId uniformId) const {
	static Shader::Uniform invalid;

	auto it = m_uniforms.find(uniformId);
	if(it != m_uniforms.end()) {
		return it->second;
	} else {
		Logging::fatal("Couldn't find uniform %s\n", shaderdefs::uniform_names[uniformId]);
		return invalid;
	}
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

void Shader::Uniform::set(const ivec2 &v) const {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const vec3 &v) const {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const vec4& v) const {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const vec2 &v) const {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, v);
}

void Shader::Uniform::set(const mat3 &m) const {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, m);
}

void Shader::Uniform::set(float f) const {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, f);
}

void Shader::Uniform::set(int i) const {
	if(!valid()) {
		Logging::fatal("Can't set invalid uniform\n");
		return;
	}
	impl::uniform_set(m_uniform, i);
}


}
}
