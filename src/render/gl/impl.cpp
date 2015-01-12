#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "render/impl.hpp"
#include "gl.hpp"

#ifdef ENABLE_DEBUG
#include <GL/glu.h>
#endif

#include <derpkit/utils/logging.hpp>

namespace derpkit {
namespace render {
namespace impl {

static GLuint vbo[2];
static GLuint vao;

// General

void DERPKIT_DEBUG_EXPORT initialize() {
	static const float vertices[] = {
		0.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 1.f,
		1.f, 1.f, 1.f, 1.f,
		1.f, 0.f, 1.f, 0.f,
	};

	static const unsigned int indices[] = { 0, 1, 3, 2 };

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0); // pos
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (const GLvoid*)(sizeof(float)*2)); //uv

	glBindVertexArray(0);

	glPopClientAttrib();

	check_for_errors("initialize()");
}

void DERPKIT_DEBUG_EXPORT cleanup() {
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, &vao);
}

void DERPKIT_DEBUG_EXPORT begin_frame() {
	glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT | GL_TEXTURE_BIT);
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	glBindVertexArray(vao);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void DERPKIT_DEBUG_EXPORT end_frame() {
	glPopClientAttrib();
	glPopAttrib();
}

void DERPKIT_DEBUG_EXPORT clear(const vec4& color) {
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void DERPKIT_DEBUG_EXPORT draw_rect() {
	glBindVertexArray(vao); // TODO: This is only needed for the debug draw into the window
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
}

void DERPKIT_DEBUG_EXPORT check_for_errors(const char* context) {
	int errors = 0 ;

	while ( true ) {
		GLenum x = glGetError() ;

		if ( x == GL_NO_ERROR )
			return;

		Logging::error("%s: OpenGL error: %s\n", context, gluErrorString(x));
		errors++;
	}
}

}
}
}
