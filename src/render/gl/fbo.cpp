#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "render/impl.hpp"
#include "gl.hpp"

#include <derpkit/utils/logging.hpp>

namespace derpkit {
namespace render {
namespace impl {

struct RenderTarget {
	GLuint resource;
	GLuint texture;
	ivec2 size;
};

RenderTarget* create_rendertarget(const ivec2& resolution) {
	RenderTarget* target = new RenderTarget();
	glGenFramebuffers(1, &target->resource);
	glGenTextures(1, &target->texture);

	target->size = resolution;

	glBindFramebuffer(GL_FRAMEBUFFER, target->resource);

	glBindTexture(GL_TEXTURE_2D, target->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_INT, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target->texture, 0);

	// TODO: Remove fatal and return nullptr

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE){
		switch( status ) {
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			Logging::fatal("Framebuffer object format is unsupported by the video hardware. (GL_FRAMEBUFFER_UNSUPPORTED_EXT)\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			Logging::fatal("Framebuffer incomplete attachment. (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			Logging::fatal("Framebuffer incomplete missing attachment. (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			Logging::fatal("Framebuffer incomplete dimensions. (GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			Logging::fatal("Framebuffer incomplete formats. (GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			Logging::fatal("Framebuffer incomplete draw buffer. (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			Logging::fatal("Framebuffer incomplete read buffer. (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
			Logging::fatal("Framebuffer incomplete multisample buffer. (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT)\n");
			break;
		default:
			Logging::fatal("Framebuffer incomplete\n");
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return target;
}

void free_rendertarget(RenderTarget* target) {
	glDeleteFramebuffers(1, &target->resource);
	glDeleteTextures(1, &target->texture);
	delete target;
}

void bind_rendertarget(RenderTarget* target) {
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, target->size.x, target->size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, target->resource);
}

void unbind_rendertarget() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glPopAttrib();
}


}
}
}
