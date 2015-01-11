#ifndef DERPKIT_RENDER_SHADER_HPP
#define DERPKIT_RENDER_SHADER_HPP

#include <derpkit/render/math.hpp>
#include <derpkit/render/shaderresource.hpp>
#include "gen/shaderdefs.hpp"

#include <string>
#include <map>

namespace derpkit {
namespace render {

namespace impl {
	struct Shader;
	struct Uniform;
}

// TODO: Implement correct shader loading
// TODO: Cache shaders
class DERPKIT_EXPORT Shader {
	public:
		~Shader();
		void bind() const;
		void unbind() const;

		bool valid() const { return m_impl != nullptr; }

		static void set_projection(const mat3& m);
		static void set_model_matrix(const mat3& m);

		struct Uniform {
			void set(const ivec2& v) const;
			void set(const vec3& v) const;
			void set(const vec4& v) const;
			void set(const vec2& v) const;
			void set(const mat3& m) const;
			void set(float f) const;
			void set(int i) const;

			bool valid() const { return m_uniform != nullptr; }

			Uniform();
			Uniform(impl::Uniform* uniform);
			Uniform(const Uniform& u);
			Uniform(Uniform&& u);
			~Uniform();

			Uniform& operator=(const Uniform& u);
			Uniform& operator=(Uniform&& u);
		private:
			impl::Uniform* m_uniform;
		};

		const Uniform& get_uniform(UniformId uniformId) const;

		static void initialize();
		static void cleanup();

		static const Shader* get(ShaderId shaderId);
	private:
		Shader(impl::Shader* shader);
		impl::Shader* m_impl;

		Uniform m_proj_mat;
		Uniform m_model_mat;

		std::map<UniformId, Uniform> m_uniforms;

		static Shader* s_current;
		static Shader** s_shaders;
};

}
}

#endif
