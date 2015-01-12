#ifndef DERPKIT_RENDER_GL_TYPES_HPP
#define DERPKIT_RENDER_GL_TYPES_HPP

#include "render/impl.hpp"
#include "gl.hpp"

namespace derpkit {
namespace render {

namespace impl {

struct Texture2D {
	GLuint resource;
};

struct ImageData {
	unsigned char* data;
	ivec2 size;
	GLint internal_format;
	GLenum format;
};

struct RenderTarget {
	GLuint resource;
	Texture2D texture;
	ivec2 size;
};

struct Uniform {
	GLint location;
#ifdef ENABLE_DEBUG
	GLuint program;
	std::string name;
#endif
};

struct ShaderStage {
	GLuint resource;

	ShaderStage(GLuint i) : resource(i) {}
};

struct Shader {
	GLuint resource;
};


}
}
}


#endif
