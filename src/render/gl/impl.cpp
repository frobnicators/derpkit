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

// General

#ifdef ENABLE_DEBUG
void DERPKIT_DEBUG_EXPORT initialize() {
	static const float vertices[] = {
		0.f, 0.f, 0.f, 1.f,
		0.f, 1.f, 0.f, 0.f,
		1.f, 1.f, 1.f, 0.f,
		1.f, 0.f, 1.f, 1.f,
	};

	static const unsigned int indices[] = { 0, 1, 3, 2 };

	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	check_for_errors("initialize()");

	printf("created quad buffers\n");
}
#endif

void DERPKIT_DEBUG_EXPORT cleanup() {
	glDeleteBuffers(2, vbo);
}

void DERPKIT_DEBUG_EXPORT clear() {
	glClearColor(1.f, 0.f, 1.f, 1.f); //TODO: Allow color specification
	glClear(GL_COLOR_BUFFER_BIT);
}

void DERPKIT_DEBUG_EXPORT draw_rect() {
	// TODO: Push attribs
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0); // pos
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (const GLvoid*)(sizeof(float)*2)); //uv
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
