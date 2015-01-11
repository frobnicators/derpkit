#ifndef DERPKIT_RENDER_SHADER_HPP
#define DERPKIT_RENDER_SHADER_HPP

#include <derpkit/render/math.hpp>
#include <derpkit/css/datatype.hpp>

#include <string>

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
		void bind();
		void unbind();

		static void set_projection(const mat3& m);
		static void set_model_matrix(const mat3& m);

		struct Uniform {
			void set(const ivec2& v);
			void set(const vec3& v);
			void set(const vec4& v);
			void set(const vec2& v);
			void set(const mat3& m);
			void set(float f);
			void set(int i);

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

		Uniform get_uniform(const std::string& name) const;

#ifdef ENABLE_DEBUG
		static Shader* from_file(const std::string& filename);
#endif
	private:
		Shader(impl::Shader* shader);
		impl::Shader* m_impl;

		Uniform m_proj_mat;
		Uniform m_model_mat;

		static Shader* s_current;
};

}
}

#endif
