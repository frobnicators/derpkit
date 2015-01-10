#ifndef DERPKIT_RENDER_SHADER_HPP
#define DERPKIT_RENDER_SHADER_HPP

#include <derpkit/render/math.hpp>

#include <string>

namespace derpkit {
namespace render {

namespace impl {
	struct Shader;
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

#ifdef ENABLE_DEBUG
		static Shader* from_file(const std::string& filename);
#endif
	private:
		Shader(impl::Shader* shader);
		impl::Shader* m_impl;

		static Shader* s_current;
};

}
}

#endif
